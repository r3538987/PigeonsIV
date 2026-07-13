#pragma once

#include <Rage.h>

#include <array>

// GTA IV EFLC seagull coordinates from:
// https://github.com/whampson/pigeon-locator
//
// Copyright (c) 2018-2026 Wes Hampson
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

inline const std::array<rage::Vector3, 50> TLAD_SEAGULL_LOCATIONS = {{
    rage::Vector3(-31.12f, 1.08f, 22.03f),
    rage::Vector3(-403.92f, -2.34f, 17.735f),
    rage::Vector3(1400.12f, -291.02f, 20.135f),
    rage::Vector3(1003.24f, -502.44f, 15.26f),
    rage::Vector3(890.46f, -5.09f, 27.54f),
    rage::Vector3(1101.4f, 490.52f, 31.745f),
    rage::Vector3(1699.7f, 600.06f, 28.51f),
    rage::Vector3(2002.0f, 96.88f, 15.475f),
    rage::Vector3(1497.0f, 285.24f, 25.51f),
    rage::Vector3(1012.94f, 798.28f, 31.82f),
    rage::Vector3(-866.62f, 1306.12f, 21.27f),
    rage::Vector3(2387.0f, 603.0f, 4.815f),
    rage::Vector3(1799.74f, 900.0f, 15.48f),
    rage::Vector3(1309.0f, 87.0f, 34.13f),
    rage::Vector3(500.0f, 2003.5f, 20.893f),
    rage::Vector3(792.14f, 1799.02f, 36.557f),
    rage::Vector3(391.0f, 1397.0f, 8.54f),
    rage::Vector3(615.0f, 1588.06f, 26.2f),
    rage::Vector3(1308.0f, 1692.0f, 17.4f),
    rage::Vector3(1218.5f, 2009.68f, 12.865f),
    rage::Vector3(443.24f, 1805.42f, 25.0f),
    rage::Vector3(-2001.78f, 6.4f, 9.53f),
    rage::Vector3(-190.0f, 1412.0f, 19.5f),
    rage::Vector3(-299.12f, 1002.94f, 11.33f),
    rage::Vector3(-1301.1f, 803.3f, 24.79f),
    rage::Vector3(-1401.02f, 1201.0f, 27.5f),
    rage::Vector3(-1597.82f, 900.0f, 26.02f),
    rage::Vector3(-402.0f, 498.0f, 12.785f),
    rage::Vector3(-1197.06f, 600.28f, 7.01f),
    rage::Vector3(-103.58f, 700.34f, 11.855f),
    rage::Vector3(-1800.0f, 400.0f, 24.355f),
    rage::Vector3(111.94f, 189.62f, 13.893f),
    rage::Vector3(548.0f, 763.0f, 20.955f),
    rage::Vector3(101.6f, -299.76f, 17.175f),
    rage::Vector3(-303.0f, 203.94f, 13.75f),
    rage::Vector3(-1470.0f, 187.42f, 9.195f),
    rage::Vector3(-527.0f, 717.0f, 8.68f),
    rage::Vector3(-300.0f, 1599.0f, 19.43f),
    rage::Vector3(-560.64f, 1205.8f, 17.34f),
    rage::Vector3(-305.4f, -202.4f, 13.945f),
    rage::Vector3(-1190.0f, -295.0f, 2.06f),
    rage::Vector3(-1492.42f, 1503.0f, 12.64f),
    rage::Vector3(-76.0f, -584.0f, 13.76f),
    rage::Vector3(-1586.0f, -518.605f, 9.26f),
    rage::Vector3(488.0f, 16.06f, 14.96f),
    rage::Vector3(-1900.0f, -297.74f, 10.04f),
    rage::Vector3(-987.88f, 1800.04f, 20.46f),
    rage::Vector3(-996.74f, 996.36f, 23.475f),
    rage::Vector3(-1403.0f, 486.0f, 14.53f),
    rage::Vector3(-1610.44f, 668.72f, 25.63f),
}};

inline const std::array<rage::Vector3, 50> TBOGT_SEAGULL_LOCATIONS = {{
    rage::Vector3(-382.02f, -156.7f, 14.078f),
    rage::Vector3(-442.66f, 342.72f, 14.324f),
    rage::Vector3(-358.0f, 1282.0f, 23.235f),
    rage::Vector3(-159.5f, 1498.7f, 23.3f),
    rage::Vector3(33.16f, -300.92f, 18.355f),
    rage::Vector3(-421.0f, -43.92f, 10.422f),
    rage::Vector3(155.0f, -444.4f, 15.43f),
    rage::Vector3(-133.68f, 554.88f, 17.74f),
    rage::Vector3(-318.0f, 1276.0f, 22.565f),
    rage::Vector3(-324.54f, 1273.55f, 28.041f),
    rage::Vector3(-405.14f, 1465.16f, 19.398f),
    rage::Vector3(-181.94f, 835.88f, 12.165f),
    rage::Vector3(-385.5f, -482.7f, 3.34f),
    rage::Vector3(-289.0f, -116.24f, 334.95f),
    rage::Vector3(-263.12f, -113.02f, 334.95f),
    rage::Vector3(862.0f, -395.72f, 39.337f),
    rage::Vector3(-1458.48f, 1345.1f, 10.982f),
    rage::Vector3(-345.66f, 917.42f, 15.09f),
    rage::Vector3(-97.0f, 475.78f, 14.126f),
    rage::Vector3(87.0f, -324.0f, 17.197f),
    rage::Vector3(1519.04f, 577.94f, 29.045f),
    rage::Vector3(408.14f, 1026.36f, 26.445f),
    rage::Vector3(293.24f, -718.52f, 4.52f),
    rage::Vector3(953.2f, 438.52f, 18.755f),
    rage::Vector3(197.0f, 717.0f, 3.685f),
    rage::Vector3(1506.0f, 920.0f, 14.37f),
    rage::Vector3(-204.0f, -257.0f, 13.7f),
    rage::Vector3(395.0f, -545.1f, 8.645f),
    rage::Vector3(-438.8f, 152.92f, 8.86f),
    rage::Vector3(-441.68f, 630.06f, 11.73f),
    rage::Vector3(-579.42f, 1042.94f, 13.575f),
    rage::Vector3(-609.42f, 791.42f, 14.925f),
    rage::Vector3(34.12f, 122.0f, 15.74f),
    rage::Vector3(318.0f, -62.78f, 14.7f),
    rage::Vector3(496.0f, 180.0f, 7.68f),
    rage::Vector3(532.0f, -40.0f, 7.925f),
    rage::Vector3(-69.0f, 1136.0f, 13.82f),
    rage::Vector3(70.48f, -185.46f, 18.048f),
    rage::Vector3(-73.22f, -113.28f, 13.76f),
    rage::Vector3(68.02f, -417.52f, 18.715f),
    rage::Vector3(-25.86f, -554.04f, 18.01f),
    rage::Vector3(145.0f, -731.0f, 4.325f),
    rage::Vector3(-296.0f, -654.0f, 7.775f),
    rage::Vector3(-748.22f, -749.38f, 6.155f),
    rage::Vector3(687.52f, 1583.96f, 27.005f),
    rage::Vector3(-1438.54f, 623.0f, 21.9f),
    rage::Vector3(-1268.0f, -330.78f, 3.085f),
    rage::Vector3(-728.0f, 1514.0f, 3.92f),
    rage::Vector3(-385.0f, 1740.0f, 11.26f),
    rage::Vector3(1091.0f, 821.0f, 32.088f),
}};
