#include <plugin.h> // Plugin-SDK version 1004 from 2026-04-18 13:03:53
#include <CModelInfo.h>
#include <CObject.h>
#include <CPools.h>
#include <CRadar.h>
#include <CTimer.h>
#include <common.h>
#include <eModelHashes.h>
#include <extensions/Config.h>
#include <extensions/ScriptCommands.h>

#include "PigeonLocations.h"
#include "SeagullLocations.h"

#include <algorithm>
#include <array>
#include <cctype>
#include <cmath>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>

using namespace plugin;

namespace {
constexpr uint32_t SCAN_INTERVAL_MS = 500;
constexpr uint32_t STATUS_LOG_INTERVAL_MS = 10000;
constexpr int32_t MAX_REASONABLE_OBJECT_POOL_SIZE = 10000;
constexpr float DEFAULT_SEARCH_RADIUS = 300.0f;
constexpr float MIN_SEARCH_RADIUS = 10.0f;
constexpr float MAX_SEARCH_RADIUS = 500.0f;
constexpr float LOCATION_MATCH_RADIUS = 4.0f;
constexpr int32_t ENEMY_BLIP_COLOUR = 9;
constexpr uint32_t SEAGULL_MODEL_HASH = 0x9C7509BA; // CJ_SEAGULL

constexpr std::array<uint32_t, 5> PIGEON_MODEL_HASHES = {
    MODEL_HASH_CJ_PIGEON_05,
    MODEL_HASH_CJ_PIGEON_06,
    MODEL_HASH_CJ_PIGEON_07,
    MODEL_HASH_CJ_PIGEON_08,
    MODEL_HASH_CJ_PIGEON_1,
};

constexpr std::array<uint32_t, 1> SEAGULL_MODEL_HASHES = {
    SEAGULL_MODEL_HASH,
};
}

struct NativeCollectibleBlip {
    int32_t blip = 0;
    uint32_t modelHash = 0;
};

using NativeBlipMap = std::unordered_map<size_t, NativeCollectibleBlip>;

struct Main {
    config_file m_config{true, false};
    std::array<int32_t, PIGEON_MODEL_HASHES.size()> m_pigeonModelIndices{};
    std::unordered_map<int32_t, int32_t> m_blipsByObject;
    NativeBlipMap m_nativePigeonBlipsByLocation;
    NativeBlipMap m_nativeSeagullBlipsByLocation;
    std::string m_logPath;
    float m_searchRadius = DEFAULT_SEARCH_RADIUS;
    bool m_loggingEnabled = true;
    bool m_showWorldArrow = false;
    uint32_t m_lastScanTime = 0;
    uint32_t m_lastStatusLogTime = 0;
    int32_t m_lastEpisode = -1;
    bool m_modelIndicesResolved = false;
    bool m_scanTimerStarted = false;
    bool m_playerWasAvailable = false;

    Main() {
        m_pigeonModelIndices.fill(-1);
        m_logPath = PLUGIN_PATH("Pigeons.IV.log");
        LoadConfig();

        if (m_loggingEnabled) {
            std::ofstream log(m_logPath, std::ios::trunc);
            if (log) {
                log << "Pigeons.IV started. Waiting for gameplay.\n";
                log << "Configured collectible search radius: "
                    << m_searchRadius << " metres.\n";
            }
        }

        Events::gameProcessEvent += [] {
            gInstance.OnGameProcess();
        };
    }

    bool LoadBooleanSetting(
        const char* name,
        bool defaultValue,
        bool& saveConfig) {
        auto& setting = m_config[name];
        const int32_t defaultInteger = defaultValue ? 1 : 0;

        if (setting.isEmpty()) {
            setting = defaultInteger;
            saveConfig = true;
        }

        const char* valueBegin = setting._value.c_str();
        char* valueEnd = nullptr;
        const long configuredValue = std::strtol(
            valueBegin,
            &valueEnd,
            10);
        while (valueEnd &&
               std::isspace(static_cast<unsigned char>(*valueEnd))) {
            ++valueEnd;
        }

        if (valueEnd == valueBegin ||
            (valueEnd && *valueEnd != '\0') ||
            (configuredValue != 0 && configuredValue != 1)) {
            setting = defaultInteger;
            saveConfig = true;
            return defaultValue;
        }

        return configuredValue != 0;
    }

    void LoadConfig() {
        m_config.setUseEqualitySign(true);
        auto& distance = m_config["Distance"];
        bool saveConfig = false;

        if (distance.isEmpty()) {
            distance = DEFAULT_SEARCH_RADIUS;
            saveConfig = true;
        }

        const char* valueBegin = distance._value.c_str();
        char* valueEnd = nullptr;
        float configuredDistance = std::strtof(valueBegin, &valueEnd);
        while (valueEnd && std::isspace(static_cast<unsigned char>(*valueEnd)))
            ++valueEnd;

        if (valueEnd == valueBegin ||
            (valueEnd && *valueEnd != '\0') ||
            !std::isfinite(configuredDistance)) {
            configuredDistance = DEFAULT_SEARCH_RADIUS;
            distance = configuredDistance;
            saveConfig = true;
        }

        m_searchRadius = std::clamp(
            configuredDistance,
            MIN_SEARCH_RADIUS,
            MAX_SEARCH_RADIUS);

        if (m_searchRadius != configuredDistance) {
            distance = m_searchRadius;
            saveConfig = true;
        }

        m_loggingEnabled = LoadBooleanSetting(
            "Logging", true, saveConfig);
        m_showWorldArrow = LoadBooleanSetting(
            "ShowWorldArrow", false, saveConfig);

        if (saveConfig)
            m_config.save();
    }

    void Log(const char* format, ...) const {
        if (!m_loggingEnabled)
            return;

        char message[512]{};
        va_list args;
        va_start(args, format);
        vsnprintf_s(message, sizeof(message), _TRUNCATE, format, args);
        va_end(args);

        std::ofstream log(m_logPath, std::ios::app);
        if (log)
            log << message << '\n';
    }

    int32_t GetBlipDisplayMode() const {
        return m_showWorldArrow
            ? BLIP_DISPLAY_ARROW_AND_MAP
            : BLIP_DISPLAY_MAP_ONLY;
    }

    void ResolvePigeonModelIndices() {
        if (m_modelIndicesResolved)
            return;

        size_t resolvedCount = 0;
        for (size_t i = 0; i < PIGEON_MODEL_HASHES.size(); ++i) {
            uint32_t modelIndex = 0;
            CBaseModelInfo* modelInfo = CModelInfo::GetModelByHash(
                static_cast<int32_t>(PIGEON_MODEL_HASHES[i]), &modelIndex);

            if (modelInfo && modelIndex <= INT16_MAX) {
                m_pigeonModelIndices[i] = static_cast<int32_t>(modelIndex);
                ++resolvedCount;
                Log("Resolved pigeon hash 0x%08X to model index %u.",
                    PIGEON_MODEL_HASHES[i], modelIndex);
            }
            else {
                Log("Could not resolve pigeon hash 0x%08X.",
                    PIGEON_MODEL_HASHES[i]);
            }
        }

        m_modelIndicesResolved = true;
        Log("Resolved %u of %u pigeon model hashes.",
            static_cast<unsigned int>(resolvedCount),
            static_cast<unsigned int>(PIGEON_MODEL_HASHES.size()));
    }

    bool IsPigeonModel(int32_t modelIndex) const {
        for (const int32_t pigeonModelIndex : m_pigeonModelIndices) {
            if (pigeonModelIndex >= 0 && pigeonModelIndex == modelIndex)
                return true;
        }
        return false;
    }

    void CreatePigeonBlip(CObject* object, int32_t objectHandle) {
        int32_t blip = 0;
        Command<void, Commands::ADD_BLIP_FOR_OBJECT>(objectHandle, &blip);
        if (!blip) {
            Log("Pigeon object %d was found, but ADD_BLIP_FOR_OBJECT returned no blip.",
                objectHandle);
            return;
        }

        Command<void, Commands::CHANGE_BLIP_SPRITE>(blip, SPRITE_LEVEL);
        Command<void, Commands::CHANGE_BLIP_COLOUR>(blip, ENEMY_BLIP_COLOUR);
        Command<void, Commands::CHANGE_BLIP_DISPLAY>(
            blip, GetBlipDisplayMode());
        Command<void, Commands::CHANGE_BLIP_SCALE>(blip, 0.75f);
        Command<void, Commands::SET_BLIP_AS_SHORT_RANGE>(blip, true);

        m_blipsByObject.emplace(objectHandle, blip);

        const auto& position = object->m_pMatrix
            ? object->m_pMatrix->pos
            : rage::Vector4{
                object->m_Transform.m_vPosn.x,
                object->m_Transform.m_vPosn.y,
                object->m_Transform.m_vPosn.z,
                0.0f
            };

        Log("Created blip %d for pigeon object %d (model=%d, pos=%.2f %.2f %.2f).",
            blip, objectHandle, object->m_nModelIndex,
            position.x, position.y, position.z);
    }

    void RemoveBlip(int32_t objectHandle, int32_t blip) const {
        if (blip)
            Command<void, Commands::REMOVE_BLIP>(blip);
        Log("Removed blip %d for pigeon object %d.", blip, objectHandle);
    }

    void ClearNativeBlips(
        NativeBlipMap& blipsByLocation,
        const char* collectibleName) {
        for (const auto& [locationIndex, collectible] : blipsByLocation) {
            if (collectible.blip)
                Command<void, Commands::REMOVE_BLIP>(collectible.blip);
            Log("Removed native %s blip %d for location %u.",
                collectibleName,
                collectible.blip,
                static_cast<unsigned int>(locationIndex + 1));
        }
        blipsByLocation.clear();
    }

    void ClearBlips() {
        for (const auto& [objectHandle, blip] : m_blipsByObject)
            RemoveBlip(objectHandle, blip);
        m_blipsByObject.clear();

        ClearNativeBlips(m_nativePigeonBlipsByLocation, "pigeon");
        ClearNativeBlips(m_nativeSeagullBlipsByLocation, "seagull");
    }

    rage::Vector3 GetEntityPosition(const CEntity* entity) const {
        if (entity->m_pMatrix)
            return entity->m_pMatrix->pos;
        return entity->m_Transform.m_vPosn;
    }

    bool DoesCollectibleModelExistAt(
        const rage::Vector3& position,
        float radius,
        uint32_t modelHash) const {
        return Command<bool, Commands::DOES_OBJECT_OF_TYPE_EXIST_AT_COORDS>(
            position.x,
            position.y,
            position.z,
            radius,
            modelHash);
    }

    float DistanceSquared(
        const rage::Vector3& left,
        const rage::Vector3& right) const {
        const float dx = left.x - right.x;
        const float dy = left.y - right.y;
        const float dz = left.z - right.z;
        return dx * dx + dy * dy + dz * dz;
    }

    template <size_t ModelCount>
    uint32_t FindCollectibleModelAt(
        const rage::Vector3& position,
        const std::array<uint32_t, ModelCount>& modelHashes) const {
        for (const uint32_t modelHash : modelHashes) {
            if (DoesCollectibleModelExistAt(
                    position,
                    LOCATION_MATCH_RADIUS,
                    modelHash)) {
                return modelHash;
            }
        }
        return 0;
    }

    void CreateNativeCollectibleBlip(
        size_t locationIndex,
        const rage::Vector3& position,
        uint32_t modelHash,
        NativeBlipMap& blipsByLocation,
        const char* collectibleName) {
        int32_t blip = 0;
        Command<void, Commands::ADD_BLIP_FOR_COORD>(
            position.x,
            position.y,
            position.z,
            &blip);

        if (!blip) {
            Log("%s location %u (model 0x%08X) was detected, but its coordinate blip could not be created.",
                collectibleName,
                static_cast<unsigned int>(locationIndex + 1),
                modelHash);
            return;
        }

        Command<void, Commands::CHANGE_BLIP_SPRITE>(
            blip, SPRITE_LEVEL);
        Command<void, Commands::CHANGE_BLIP_COLOUR>(
            blip, ENEMY_BLIP_COLOUR);
        Command<void, Commands::CHANGE_BLIP_DISPLAY>(
            blip, GetBlipDisplayMode());
        Command<void, Commands::CHANGE_BLIP_SCALE>(
            blip, 0.75f);
        Command<void, Commands::SET_BLIP_AS_SHORT_RANGE>(
            blip, false);

        blipsByLocation.emplace(
            locationIndex,
            NativeCollectibleBlip{blip, modelHash});

        Log("Created %s blip %d for location %u (model 0x%08X, pos=%.2f %.2f %.2f).",
            collectibleName,
            blip,
            static_cast<unsigned int>(locationIndex + 1),
            modelHash,
            position.x,
            position.y,
            position.z);
    }

    template <size_t LocationCount, size_t ModelCount>
    void ScanNativeCollectiblePresence(
        const CPlayerPed* player,
        const std::array<rage::Vector3, LocationCount>& locations,
        const std::array<uint32_t, ModelCount>& modelHashes,
        NativeBlipMap& blipsByLocation,
        const char* collectibleName) {
        const rage::Vector3 playerPosition = GetEntityPosition(player);
        const float searchRadiusSquared =
            m_searchRadius * m_searchRadius;
        std::unordered_set<size_t> detectedLocations;

        for (size_t i = 0; i < locations.size(); ++i) {
            const rage::Vector3& position = locations[i];
            if (DistanceSquared(playerPosition, position) > searchRadiusSquared)
                continue;

            const uint32_t modelHash = FindCollectibleModelAt(
                position,
                modelHashes);
            if (!modelHash)
                continue;

            detectedLocations.insert(i);
            if (!blipsByLocation.contains(i)) {
                CreateNativeCollectibleBlip(
                    i,
                    position,
                    modelHash,
                    blipsByLocation,
                    collectibleName);
            }
        }

        for (auto it = blipsByLocation.begin();
             it != blipsByLocation.end();) {
            if (detectedLocations.contains(it->first)) {
                ++it;
                continue;
            }

            if (it->second.blip)
                Command<void, Commands::REMOVE_BLIP>(it->second.blip);

            const bool outOfRange = DistanceSquared(
                playerPosition,
                locations[it->first]) > searchRadiusSquared;
            if (outOfRange) {
                Log("Removed %s blip %d for location %u: location left the %.1f metre search radius.",
                    collectibleName,
                    it->second.blip,
                    static_cast<unsigned int>(it->first + 1),
                    m_searchRadius);
            }
            else {
                Log("Removed %s blip %d for location %u: collectible was destroyed or unloaded.",
                    collectibleName,
                    it->second.blip,
                    static_cast<unsigned int>(it->first + 1));
            }
            it = blipsByLocation.erase(it);
        }
    }

    const char* GetEpisodeName(int32_t episode) const {
        switch (episode) {
        case EPISODE_IV:
            return "GTA IV";
        case EPISODE_TLAD:
            return "The Lost and Damned";
        case EPISODE_TBOGT:
            return "The Ballad of Gay Tony";
        default:
            return "unknown";
        }
    }

    void ScanNativeCollectiblePresence(const CPlayerPed* player) {
        if (m_lastEpisode != gGameEpisode) {
            ClearNativeBlips(m_nativePigeonBlipsByLocation, "pigeon");
            ClearNativeBlips(m_nativeSeagullBlipsByLocation, "seagull");
            m_lastEpisode = gGameEpisode;
            Log("Active episode: %s (%d).",
                GetEpisodeName(gGameEpisode),
                gGameEpisode);

            if (gGameEpisode == EPISODE_TLAD ||
                gGameEpisode == EPISODE_TBOGT) {
                uint32_t modelIndex = 0;
                CBaseModelInfo* modelInfo = CModelInfo::GetModelByHash(
                    static_cast<int32_t>(SEAGULL_MODEL_HASH),
                    &modelIndex);
                if (modelInfo) {
                    Log("Resolved seagull hash 0x%08X (CJ_SEAGULL) to model index %u.",
                        SEAGULL_MODEL_HASH,
                        modelIndex);
                }
                else {
                    Log("Could not resolve seagull hash 0x%08X (CJ_SEAGULL).",
                        SEAGULL_MODEL_HASH);
                }
            }
        }

        switch (gGameEpisode) {
        case EPISODE_IV:
            ScanNativeCollectiblePresence(
                player,
                PIGEON_LOCATIONS,
                PIGEON_MODEL_HASHES,
                m_nativePigeonBlipsByLocation,
                "pigeon");
            break;
        case EPISODE_TLAD:
            ScanNativeCollectiblePresence(
                player,
                TLAD_SEAGULL_LOCATIONS,
                SEAGULL_MODEL_HASHES,
                m_nativeSeagullBlipsByLocation,
                "seagull");
            break;
        case EPISODE_TBOGT:
            ScanNativeCollectiblePresence(
                player,
                TBOGT_SEAGULL_LOCATIONS,
                SEAGULL_MODEL_HASHES,
                m_nativeSeagullBlipsByLocation,
                "seagull");
            break;
        }
    }

    void ScanForCollectibles(uint32_t now, const CPlayerPed* player) {
        auto* objectPool = CPools::ms_pObjectsPool;
        if (!objectPool)
            return;

        const int32_t poolSize = objectPool->GetSize();
        if (poolSize <= 0 || poolSize > MAX_REASONABLE_OBJECT_POOL_SIZE) {
            Log("Skipped object scan because pool size %d is invalid.", poolSize);
            return;
        }

        std::unordered_set<int32_t> seenPigeonHandles;
        int32_t occupiedObjects = 0;

        for (int32_t slot = 0; slot < poolSize; ++slot) {
            CObject* object = objectPool->GetSlot(slot);
            if (!object)
                continue;

            ++occupiedObjects;
            if (gGameEpisode != EPISODE_IV ||
                !IsPigeonModel(object->m_nModelIndex)) {
                continue;
            }

            const int32_t objectHandle = CPools::GetObjectRef(object);
            seenPigeonHandles.insert(objectHandle);

            if (!m_blipsByObject.contains(objectHandle))
                CreatePigeonBlip(object, objectHandle);
        }

        for (auto it = m_blipsByObject.begin(); it != m_blipsByObject.end();) {
            if (!seenPigeonHandles.contains(it->first)) {
                RemoveBlip(it->first, it->second);
                it = m_blipsByObject.erase(it);
            }
            else {
                ++it;
            }
        }

        ScanNativeCollectiblePresence(player);

        if (m_lastStatusLogTime == 0 ||
            now - m_lastStatusLogTime >= STATUS_LOG_INTERVAL_MS) {
            m_lastStatusLogTime = now;
            Log("Scan status: episode=%d pool=%d occupied=%d poolPigeons=%u objectBlips=%u nativePigeonBlips=%u nativeSeagullBlips=%u.",
                gGameEpisode,
                poolSize, occupiedObjects,
                static_cast<unsigned int>(seenPigeonHandles.size()),
                static_cast<unsigned int>(m_blipsByObject.size()),
                static_cast<unsigned int>(m_nativePigeonBlipsByLocation.size()),
                static_cast<unsigned int>(m_nativeSeagullBlipsByLocation.size()));
        }
    }

    void OnGameProcess() {
        auto* player = FindPlayerPed(0);
        auto* objectPool = CPools::ms_pObjectsPool;

        if (!player || !objectPool) {
            if (m_playerWasAvailable) {
                ClearBlips();
                Log("Gameplay became unavailable; cleared tracked collectible blips.");
            }
            m_playerWasAvailable = false;
            m_scanTimerStarted = false;
            m_lastEpisode = -1;
            return;
        }

        if (!m_playerWasAvailable)
            Log("Gameplay detected; starting collectible scans.");
        m_playerWasAvailable = true;

        if (gGameEpisode == EPISODE_IV)
            ResolvePigeonModelIndices();

        const uint32_t now = CTimer::GetTimeInMilliseconds();
        if (m_scanTimerStarted && now - m_lastScanTime < SCAN_INTERVAL_MS)
            return;

        m_scanTimerStarted = true;
        m_lastScanTime = now;
        ScanForCollectibles(now, player);
    }
} gInstance;
