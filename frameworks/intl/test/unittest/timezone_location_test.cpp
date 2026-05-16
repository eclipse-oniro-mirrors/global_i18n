/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <gtest/gtest.h>
#include <sys/stat.h>
#include "i18n_timezone.h"
#include "timezone_location_test.h"

using namespace OHOS::Global::I18n;
using testing::ext::TestSize;
using namespace std;
using namespace testing;

namespace OHOS {
namespace Global {
namespace I18n {
std::vector<std::tuple<double, double, std::vector<std::string>>> TimeZoneLocationTest::timeZoneTestcases {
    { -180.0, -90.0, { "Antarctica/McMurdo" } },
    { -180.0, -80.0, { "Antarctica/McMurdo" } },
    { -180.0, -70.0, { "Etc/GMT+12" } },
    { -180.0, -60.0, { "Etc/GMT+12" } },
    { -180.0, -50.0, { "Etc/GMT+12" } },
    { -180.0, -40.0, { "Etc/GMT+12" } },
    { -180.0, -30.0, { "Etc/GMT+12" } },
    { -180.0, -20.0, { "Etc/GMT+12" } },
    { -180.0, -10.0, { "Etc/GMT+12" } },
    { -180.0, 0.0, { "Etc/GMT+12" } },
    { -180.0, 10.0, { "Etc/GMT+12" } },
    { -180.0, 20.0, { "Etc/GMT+12" } },
    { -180.0, 30.0, { "Etc/GMT+12" } },
    { -180.0, 40.0, { "Etc/GMT+12" } },
    { -180.0, 50.0, { "Etc/GMT+12" } },
    { -180.0, 60.0, { "Etc/GMT+12" } },
    { -180.0, 70.0, { "Etc/GMT+12" } },
    { -180.0, 80.0, { "Etc/GMT+12" } },
    { -170.0, -90.0, { "Antarctica/McMurdo" } },
    { -170.0, -80.0, { "Antarctica/McMurdo" } },
    { -170.0, -70.0, { "Etc/GMT+11" } },
    { -170.0, -60.0, { "Etc/GMT+11" } },
    { -170.0, -50.0, { "Etc/GMT+11" } },
    { -170.0, -40.0, { "Etc/GMT+11" } },
    { -170.0, -30.0, { "Etc/GMT+11" } },
    { -170.0, -20.0, { "Etc/GMT+11" } },
    { -170.0, -10.0, { "Etc/GMT+11" } },
    { -170.0, 0.0, { "Etc/GMT+11" } },
    { -170.0, 10.0, { "Etc/GMT+11" } },
    { -170.0, 20.0, { "Etc/GMT+11" } },
    { -170.0, 30.0, { "Etc/GMT+11" } },
    { -170.0, 40.0, { "Etc/GMT+11" } },
    { -170.0, 50.0, { "Etc/GMT+11" } },
    { -170.0, 60.0, { "Etc/GMT+11" } },
    { -170.0, 70.0, { "Etc/GMT+11" } },
    { -170.0, 80.0, { "Etc/GMT+11" } },
    { -160.0, -90.0, { "Antarctica/McMurdo" } },
    { -160.0, -80.0, { "Antarctica/McMurdo" } },
    { -160.0, -70.0, { "Etc/GMT+11" } },
    { -160.0, -60.0, { "Etc/GMT+11" } },
    { -160.0, -50.0, { "Etc/GMT+11" } },
    { -160.0, -40.0, { "Etc/GMT+11" } },
    { -160.0, -30.0, { "Etc/GMT+11" } },
    { -160.0, -20.0, { "Etc/GMT+11" } },
    { -160.0, -10.0, { "Etc/GMT+11" } },
    { -160.0, 0.0, { "Etc/GMT+11" } },
    { -160.0, 10.0, { "Etc/GMT+11" } },
    { -160.0, 20.0, { "Etc/GMT+11" } },
    { -160.0, 30.0, { "Etc/GMT+11" } },
    { -160.0, 40.0, { "Etc/GMT+11" } },
    { -160.0, 50.0, { "Etc/GMT+11" } },
    { -160.0, 60.0, { "America/Anchorage" } },
    { -160.0, 70.0, { "America/Anchorage" } },
    { -160.0, 80.0, { "Etc/GMT+11" } },
    { -150.0, -90.0, { "Antarctica/McMurdo" } },
    { -150.0, -80.0, { "Antarctica/McMurdo" } },
    { -150.0, -70.0, { "Etc/GMT+10" } },
    { -150.0, -60.0, { "Etc/GMT+10" } },
    { -150.0, -50.0, { "Etc/GMT+10" } },
    { -150.0, -40.0, { "Etc/GMT+10" } },
    { -150.0, -30.0, { "Etc/GMT+10" } },
    { -150.0, -20.0, { "Etc/GMT+10" } },
    { -150.0, -10.0, { "Pacific/Kiritimati" } },
    { -150.0, 0.0, { "Etc/GMT+10" } },
    { -150.0, 10.0, { "Etc/GMT+10" } },
    { -150.0, 20.0, { "Etc/GMT+10" } },
    { -150.0, 30.0, { "Etc/GMT+10" } },
    { -150.0, 40.0, { "Etc/GMT+10" } },
    { -150.0, 50.0, { "Etc/GMT+10" } },
    { -150.0, 60.0, { "America/Anchorage" } },
    { -150.0, 70.0, { "America/Anchorage" } },
    { -150.0, 80.0, { "Etc/GMT+10" } },
    { -140.0, -90.0, { "Antarctica/McMurdo" } },
    { -140.0, -80.0, { "Etc/GMT+9" } },
    { -140.0, -70.0, { "Etc/GMT+9" } },
    { -140.0, -60.0, { "Etc/GMT+9" } },
    { -140.0, -50.0, { "Etc/GMT+9" } },
    { -140.0, -40.0, { "Etc/GMT+9" } },
    { -140.0, -30.0, { "Etc/GMT+9" } },
    { -140.0, -20.0, { "Etc/GMT+9" } },
    { -140.0, -10.0, { "Etc/GMT+9" } },
    { -140.0, 0.0, { "Etc/GMT+9" } },
    { -140.0, 10.0, { "Etc/GMT+9" } },
    { -140.0, 20.0, { "Etc/GMT+9" } },
    { -140.0, 30.0, { "Etc/GMT+9" } },
    { -140.0, 40.0, { "Etc/GMT+9" } },
    { -140.0, 50.0, { "Etc/GMT+9" } },
    { -140.0, 60.0, { "America/Yakutat" } },
    { -140.0, 70.0, { "Etc/GMT+9" } },
    { -140.0, 80.0, { "Etc/GMT+9" } },
    { -130.0, -90.0, { "Antarctica/McMurdo" } },
    { -130.0, -80.0, { "Etc/GMT+9" } },
    { -130.0, -70.0, { "Etc/GMT+9" } },
    { -130.0, -60.0, { "Etc/GMT+9" } },
    { -130.0, -50.0, { "Etc/GMT+9" } },
    { -130.0, -40.0, { "Etc/GMT+9" } },
    { -130.0, -30.0, { "Etc/GMT+9" } },
    { -130.0, -20.0, { "Etc/GMT+9" } },
    { -130.0, -10.0, { "Etc/GMT+9" } },
    { -130.0, 0.0, { "Etc/GMT+9" } },
    { -130.0, 10.0, { "Etc/GMT+9" } },
    { -130.0, 20.0, { "Etc/GMT+9" } },
    { -130.0, 30.0, { "Etc/GMT+9" } },
    { -130.0, 40.0, { "Etc/GMT+9" } },
    { -130.0, 50.0, { "Etc/GMT+9" } },
    { -130.0, 60.0, { "America/Whitehorse" } },
    { -130.0, 70.0, { "America/Inuvik" } },
    { -130.0, 80.0, { "Etc/GMT+9" } },
    { -120.0, -90.0, { "Antarctica/McMurdo" } },
    { -120.0, -80.0, { "Etc/GMT+8" } },
    { -120.0, -70.0, { "Etc/GMT+8" } },
    { -120.0, -60.0, { "Etc/GMT+8" } },
    { -120.0, -50.0, { "Etc/GMT+8" } },
    { -120.0, -40.0, { "Etc/GMT+8" } },
    { -120.0, -30.0, { "Etc/GMT+8" } },
    { -120.0, -20.0, { "Etc/GMT+8" } },
    { -120.0, -10.0, { "Etc/GMT+8" } },
    { -120.0, 0.0, { "Etc/GMT+8" } },
    { -120.0, 10.0, { "Etc/GMT+8" } },
    { -120.0, 20.0, { "Etc/GMT+8" } },
    { -120.0, 30.0, { "Etc/GMT+8" } },
    { -120.0, 40.0, { "America/Los_Angeles" } },
    { -120.0, 50.0, { "America/Vancouver" } },
    { -120.0, 60.0, { "America/Edmonton" } },
    { -120.0, 70.0, { "America/Inuvik", "America/Edmonton" } },
    { -120.0, 80.0, { "Etc/GMT+8" } },
    { -110.0, -90.0, { "Antarctica/McMurdo" } },
    { -110.0, -80.0, { "Etc/GMT+7" } },
    { -110.0, -70.0, { "Etc/GMT+7" } },
    { -110.0, -60.0, { "Etc/GMT+7" } },
    { -110.0, -50.0, { "Etc/GMT+7" } },
    { -110.0, -40.0, { "Etc/GMT+7" } },
    { -110.0, -30.0, { "Etc/GMT+7" } },
    { -110.0, -20.0, { "Etc/GMT+7" } },
    { -110.0, -10.0, { "Etc/GMT+7" } },
    { -110.0, 0.0, { "Etc/GMT+7" } },
    { -110.0, 10.0, { "Etc/GMT+7" } },
    { -110.0, 20.0, { "Etc/GMT+7" } },
    { -110.0, 30.0, { "America/Hermosillo" } },
    { -110.0, 40.0, { "America/Denver" } },
    { -110.0, 50.0, { "America/Swift_Current" } },
    { -110.0, 60.0, { "America/Edmonton" } },
    { -110.0, 70.0, { "America/Cambridge_Bay" } },
    { -110.0, 80.0, { "Etc/GMT+7" } },
    { -100.0, -90.0, { "Antarctica/McMurdo" } },
    { -100.0, -80.0, { "Etc/GMT+7" } },
    { -100.0, -70.0, { "Etc/GMT+7" } },
    { -100.0, -60.0, { "Etc/GMT+7" } },
    { -100.0, -50.0, { "Etc/GMT+7" } },
    { -100.0, -40.0, { "Etc/GMT+7" } },
    { -100.0, -30.0, { "Etc/GMT+7" } },
    { -100.0, -20.0, { "Etc/GMT+7" } },
    { -100.0, -10.0, { "Etc/GMT+7" } },
    { -100.0, 0.0, { "Etc/GMT+7" } },
    { -100.0, 10.0, { "Etc/GMT+7" } },
    { -100.0, 20.0, { "America/Mexico_City" } },
    { -100.0, 30.0, { "America/Chicago" } },
    { -100.0, 40.0, { "America/Chicago" } },
    { -100.0, 50.0, { "America/Winnipeg" } },
    { -100.0, 60.0, { "America/Rankin_Inlet" } },
    { -100.0, 70.0, { "America/Cambridge_Bay" } },
    { -100.0, 80.0, { "America/Rankin_Inlet" } },
    { -90.0, -90.0, { "Antarctica/McMurdo" } },
    { -90.0, -80.0, { "Etc/GMT+6" } },
    { -90.0, -70.0, { "Etc/GMT+6" } },
    { -90.0, -60.0, { "Etc/GMT+6" } },
    { -90.0, -50.0, { "Etc/GMT+6" } },
    { -90.0, -40.0, { "Etc/GMT+6" } },
    { -90.0, -30.0, { "Etc/GMT+6" } },
    { -90.0, -20.0, { "Etc/GMT+6" } },
    { -90.0, -10.0, { "Etc/GMT+6" } },
    { -90.0, 0.0, { "Pacific/Galapagos" } },
    { -90.0, 10.0, { "Etc/GMT+6" } },
    { -90.0, 20.0, { "America/Merida" } },
    { -90.0, 30.0, { "America/Chicago" } },
    { -90.0, 40.0, { "America/Chicago" } },
    { -90.0, 50.0, { "America/Winnipeg", "America/Toronto" } },
    { -90.0, 60.0, { "America/Rankin_Inlet" } },
    { -90.0, 70.0, { "America/Cambridge_Bay" } },
    { -90.0, 80.0, { "America/Rankin_Inlet" } },
    { -80.0, -90.0, { "Antarctica/McMurdo" } },
    { -80.0, -80.0, { "Antarctica/Rothera" } },
    { -80.0, -70.0, { "Antarctica/Rothera" } },
    { -80.0, -60.0, { "Etc/GMT+5" } },
    { -80.0, -50.0, { "Etc/GMT+5" } },
    { -80.0, -40.0, { "Etc/GMT+5" } },
    { -80.0, -30.0, { "Etc/GMT+5" } },
    { -80.0, -20.0, { "Etc/GMT+5" } },
    { -80.0, -10.0, { "America/Lima" } },
    { -80.0, 0.0, { "America/Guayaquil" } },
    { -80.0, 10.0, { "Etc/GMT+5" } },
    { -80.0, 20.0, { "Etc/GMT+5" } },
    { -80.0, 30.0, { "Etc/GMT+5" } },
    { -80.0, 40.0, { "America/New_York" } },
    { -80.0, 50.0, { "America/Toronto" } },
    { -80.0, 60.0, { "America/Iqaluit" } },
    { -80.0, 70.0, { "America/Iqaluit" } },
    { -80.0, 80.0, { "America/Iqaluit" } },
    { -70.0, -90.0, { "Antarctica/McMurdo" } },
    { -70.0, -80.0, { "Antarctica/Rothera" } },
    { -70.0, -70.0, { "Antarctica/Rothera" } },
    { -70.0, -60.0, { "Etc/GMT+5" } },
    { -70.0, -50.0, { "America/Argentina/Rio_Gallegos" } },
    { -70.0, -40.0, { "America/Argentina/Salta" } },
    { -70.0, -30.0, { "America/Santiago" } },
    { -70.0, -20.0, { "America/Santiago" } },
    { -70.0, -10.0, { "America/Rio_Branco" } },
    { -70.0, 0.0, { "America/Manaus" } },
    { -70.0, 10.0, { "America/Caracas" } },
    { -70.0, 20.0, { "America/Santo_Domingo" } },
    { -70.0, 30.0, { "Etc/GMT+5" } },
    { -70.0, 40.0, { "Etc/GMT+5" } },
    { -70.0, 50.0, { "America/Toronto" } },
    { -70.0, 60.0, { "America/Toronto" } },
    { -70.0, 70.0, { "America/Iqaluit" } },
    { -70.0, 80.0, { "America/Iqaluit" } },
    { -60.0, -90.0, { "Antarctica/McMurdo" } },
    { -60.0, -80.0, { "America/Argentina/Ushuaia" } },
    { -60.0, -70.0, { "America/Argentina/Ushuaia" } },
    { -60.0, -60.0, { "Etc/GMT+4" } },
    { -60.0, -50.0, { "Etc/GMT+4" } },
    { -60.0, -40.0, { "Etc/GMT+4" } },
    { -60.0, -30.0, { "America/Argentina/Cordoba" } },
    { -60.0, -20.0, { "America/Asuncion" } },
    { -60.0, -10.0, { "America/Cuiaba" } },
    { -60.0, 0.0, { "America/Manaus" } },
    { -60.0, 10.0, { "Etc/GMT+4" } },
    { -60.0, 20.0, { "Etc/GMT+4" } },
    { -60.0, 30.0, { "Etc/GMT+4" } },
    { -60.0, 40.0, { "Etc/GMT+4" } },
    { -60.0, 50.0, { "America/Blanc-Sablon" } },
    { -60.0, 60.0, { "Etc/GMT+4" } },
    { -60.0, 70.0, { "Etc/GMT+4" } },
    { -60.0, 80.0, { "America/Nuuk" } },
    { -50.0, -90.0, { "Antarctica/McMurdo" } },
    { -50.0, -80.0, { "America/Argentina/Ushuaia" } },
    { -50.0, -70.0, { "Etc/GMT+3" } },
    { -50.0, -60.0, { "Etc/GMT+3" } },
    { -50.0, -50.0, { "Etc/GMT+3" } },
    { -50.0, -40.0, { "Etc/GMT+3" } },
    { -50.0, -30.0, { "America/Sao_Paulo" } },
    { -50.0, -20.0, { "America/Sao_Paulo" } },
    { -50.0, -10.0, { "America/Araguaina" } },
    { -50.0, 0.0, { "America/Belem" } },
    { -50.0, 10.0, { "Etc/GMT+3" } },
    { -50.0, 20.0, { "Etc/GMT+3" } },
    { -50.0, 30.0, { "Etc/GMT+3" } },
    { -50.0, 40.0, { "Etc/GMT+3" } },
    { -50.0, 50.0, { "Etc/GMT+3" } },
    { -50.0, 60.0, { "Etc/GMT+3" } },
    { -50.0, 70.0, { "America/Nuuk" } },
    { -50.0, 80.0, { "America/Nuuk" } },
    { -40.0, -90.0, { "Antarctica/McMurdo" } },
    { -40.0, -80.0, { "America/Argentina/Ushuaia" } },
    { -40.0, -70.0, { "Etc/GMT+3" } },
    { -40.0, -60.0, { "Etc/GMT+3" } },
    { -40.0, -50.0, { "Etc/GMT+3" } },
    { -40.0, -40.0, { "Etc/GMT+3" } },
    { -40.0, -30.0, { "Etc/GMT+3" } },
    { -40.0, -20.0, { "America/Sao_Paulo" } },
    { -40.0, -10.0, { "America/Bahia" } },
    { -40.0, 0.0, { "Etc/GMT+3" } },
    { -40.0, 10.0, { "Etc/GMT+3" } },
    { -40.0, 20.0, { "Etc/GMT+3" } },
    { -40.0, 30.0, { "Etc/GMT+3" } },
    { -40.0, 40.0, { "Etc/GMT+3" } },
    { -40.0, 50.0, { "Etc/GMT+3" } },
    { -40.0, 60.0, { "Etc/GMT+3" } },
    { -40.0, 70.0, { "America/Nuuk" } },
    { -40.0, 80.0, { "America/Nuuk" } },
    { -30.0, -90.0, { "Antarctica/McMurdo" } },
    { -30.0, -80.0, { "Antarctica/Rothera" } },
    { -30.0, -70.0, { "Etc/GMT+2" } },
    { -30.0, -60.0, { "Etc/GMT+2" } },
    { -30.0, -50.0, { "Etc/GMT+2" } },
    { -30.0, -40.0, { "Etc/GMT+2" } },
    { -30.0, -30.0, { "Etc/GMT+2" } },
    { -30.0, -20.0, { "Etc/GMT+2" } },
    { -30.0, -10.0, { "Etc/GMT+2" } },
    { -30.0, 0.0, { "Etc/GMT+2" } },
    { -30.0, 10.0, { "Etc/GMT+2" } },
    { -30.0, 20.0, { "Etc/GMT+2" } },
    { -30.0, 30.0, { "Etc/GMT+2" } },
    { -30.0, 40.0, { "Etc/GMT+2" } },
    { -30.0, 50.0, { "Etc/GMT+2" } },
    { -30.0, 60.0, { "Etc/GMT+2" } },
    { -30.0, 70.0, { "America/Nuuk" } },
    { -30.0, 80.0, { "America/Nuuk" } },
    { -20.0, -90.0, { "Antarctica/McMurdo" } },
    { -20.0, -80.0, { "Antarctica/Rothera" } },
    { -20.0, -70.0, { "Etc/GMT+1" } },
    { -20.0, -60.0, { "Etc/GMT+1" } },
    { -20.0, -50.0, { "Etc/GMT+1" } },
    { -20.0, -40.0, { "Etc/GMT+1" } },
    { -20.0, -30.0, { "Etc/GMT+1" } },
    { -20.0, -20.0, { "Etc/GMT+1" } },
    { -20.0, -10.0, { "Etc/GMT+1" } },
    { -20.0, 0.0, { "Etc/GMT+1" } },
    { -20.0, 10.0, { "Etc/GMT+1" } },
    { -20.0, 20.0, { "Etc/GMT+1" } },
    { -20.0, 30.0, { "Etc/GMT+1" } },
    { -20.0, 40.0, { "Etc/GMT+1" } },
    { -20.0, 50.0, { "Etc/GMT+1" } },
    { -20.0, 60.0, { "Etc/GMT+1" } },
    { -20.0, 70.0, { "Etc/GMT+1" } },
    { -20.0, 80.0, { "America/Nuuk" } },
    { -10.0, -90.0, { "Antarctica/McMurdo" } },
    { -10.0, -80.0, { "Etc/UTC" } },
    { -10.0, -70.0, { "Etc/UTC" } },
    { -10.0, -60.0, { "Etc/GMT+1" } },
    { -10.0, -50.0, { "Etc/GMT+1" } },
    { -10.0, -40.0, { "Etc/GMT+1" } },
    { -10.0, -30.0, { "Etc/GMT+1" } },
    { -10.0, -20.0, { "Etc/GMT+1" } },
    { -10.0, -10.0, { "Etc/GMT+1" } },
    { -10.0, 0.0, { "Etc/GMT+1" } },
    { -10.0, 10.0, { "Africa/Conakry" } },
    { -10.0, 20.0, { "Africa/Nouakchott" } },
    { -10.0, 30.0, { "Etc/GMT+1" } },
    { -10.0, 40.0, { "Etc/GMT+1" } },
    { -10.0, 50.0, { "Etc/GMT+1" } },
    { -10.0, 60.0, { "Etc/GMT+1" } },
    { -10.0, 70.0, { "Etc/GMT+1" } },
    { -10.0, 80.0, { "Etc/GMT+1" } },
    { 0.0, -90.0, { "Antarctica/McMurdo" } },
    { 0.0, -80.0, { "Africa/Johannesburg" } },
    { 0.0, -70.0, { "Africa/Johannesburg" } },
    { 0.0, -60.0, { "Etc/GMT" } },
    { 0.0, -50.0, { "Etc/GMT" } },
    { 0.0, -40.0, { "Etc/GMT" } },
    { 0.0, -30.0, { "Etc/GMT" } },
    { 0.0, -20.0, { "Etc/GMT" } },
    { 0.0, -10.0, { "Etc/GMT" } },
    { 0.0, 0.0, { "Etc/GMT" } },
    { 0.0, 10.0, { "Africa/Accra" } },
    { 0.0, 20.0, { "Africa/Bamako" } },
    { 0.0, 30.0, { "Africa/Algiers" } },
    { 0.0, 40.0, { "Europe/Madrid" } },
    { 0.0, 50.0, { "Etc/GMT" } },
    { 0.0, 60.0, { "Etc/GMT" } },
    { 0.0, 70.0, { "Etc/GMT" } },
    { 0.0, 80.0, { "Etc/GMT" } },
    { 10.0, -90.0, { "Antarctica/McMurdo" } },
    { 10.0, -80.0, { "Antarctica/Troll" } },
    { 10.0, -70.0, { "Antarctica/Troll" } },
    { 10.0, -60.0, { "Etc/GMT-1" } },
    { 10.0, -50.0, { "Etc/GMT-1" } },
    { 10.0, -40.0, { "Etc/GMT-1" } },
    { 10.0, -30.0, { "Etc/GMT-1" } },
    { 10.0, -20.0, { "Etc/GMT-1" } },
    { 10.0, -10.0, { "Etc/GMT-1" } },
    { 10.0, 0.0, { "Africa/Libreville" } },
    { 10.0, 10.0, { "Africa/Lagos" } },
    { 10.0, 20.0, { "Africa/Niamey" } },
    { 10.0, 30.0, { "Africa/Tripoli" } },
    { 10.0, 40.0, { "Etc/GMT-1" } },
    { 10.0, 50.0, { "Europe/Berlin" } },
    { 10.0, 60.0, { "Europe/Oslo" } },
    { 10.0, 70.0, { "Etc/GMT-1" } },
    { 10.0, 80.0, { "Etc/GMT-1" } },
    { 20.0, -90.0, { "Antarctica/McMurdo" } },
    { 20.0, -80.0, { "Antarctica/Troll" } },
    { 20.0, -70.0, { "Antarctica/Troll" } },
    { 20.0, -60.0, { "Etc/GMT-1" } },
    { 20.0, -50.0, { "Etc/GMT-1" } },
    { 20.0, -40.0, { "Etc/GMT-1" } },
    { 20.0, -30.0, { "Africa/Johannesburg" } },
    { 20.0, -20.0, { "Africa/Windhoek" } },
    { 20.0, -10.0, { "Africa/Luanda" } },
    { 20.0, 0.0, { "Africa/Kinshasa" } },
    { 20.0, 10.0, { "Africa/Ndjamena" } },
    { 20.0, 20.0, { "Africa/Ndjamena" } },
    { 20.0, 30.0, { "Africa/Tripoli" } },
    { 20.0, 40.0, { "Europe/Tirane" } },
    { 20.0, 50.0, { "Europe/Warsaw" } },
    { 20.0, 60.0, { "Europe/Mariehamn" } },
    { 20.0, 70.0, { "Europe/Oslo" } },
    { 20.0, 80.0, { "Arctic/Longyearbyen" } },
    { 30.0, -90.0, { "Antarctica/McMurdo" } },
    { 30.0, -80.0, { "Antarctica/Syowa" } },
    { 30.0, -70.0, { "Antarctica/Syowa" } },
    { 30.0, -60.0, { "Etc/GMT-2" } },
    { 30.0, -50.0, { "Etc/GMT-2" } },
    { 30.0, -40.0, { "Etc/GMT-2" } },
    { 30.0, -30.0, { "Africa/Johannesburg" } },
    { 30.0, -20.0, { "Africa/Harare" } },
    { 30.0, -10.0, { "Africa/Lusaka" } },
    { 30.0, 0.0, { "Africa/Kampala" } },
    { 30.0, 10.0, { "Africa/Juba" } },
    { 30.0, 20.0, { "Africa/Khartoum" } },
    { 30.0, 30.0, { "Africa/Cairo" } },
    { 30.0, 40.0, { "Europe/Istanbul" } },
    { 30.0, 50.0, { "Europe/Kiev" } },
    { 30.0, 60.0, { "Europe/Moscow" } },
    { 30.0, 70.0, { "Europe/Oslo" } },
    { 30.0, 80.0, { "Etc/GMT-2" } },
    { 40.0, -90.0, { "Antarctica/McMurdo" } },
    { 40.0, -80.0, { "Antarctica/Syowa" } },
    { 40.0, -70.0, { "Antarctica/Syowa" } },
    { 40.0, -60.0, { "Etc/GMT-3" } },
    { 40.0, -50.0, { "Etc/GMT-3" } },
    { 40.0, -40.0, { "Etc/GMT-3" } },
    { 40.0, -30.0, { "Etc/GMT-3" } },
    { 40.0, -20.0, { "Etc/GMT-3" } },
    { 40.0, -10.0, { "Africa/Dar_es_Salaam" } },
    { 40.0, 0.0, { "Africa/Nairobi" } },
    { 40.0, 10.0, { "Africa/Addis_Ababa" } },
    { 40.0, 20.0, { "Asia/Riyadh" } },
    { 40.0, 30.0, { "Asia/Riyadh" } },
    { 40.0, 40.0, { "Europe/Istanbul" } },
    { 40.0, 50.0, { "Europe/Moscow" } },
    { 40.0, 60.0, { "Europe/Moscow" } },
    { 40.0, 70.0, { "Etc/GMT-3" } },
    { 40.0, 80.0, { "Etc/GMT-3" } },
    { 50.0, -90.0, { "Antarctica/McMurdo" } },
    { 50.0, -80.0, { "Antarctica/Syowa" } },
    { 50.0, -70.0, { "Antarctica/Syowa" } },
    { 50.0, -60.0, { "Etc/GMT-3" } },
    { 50.0, -50.0, { "Etc/GMT-3" } },
    { 50.0, -40.0, { "Etc/GMT-3" } },
    { 50.0, -30.0, { "Etc/GMT-3" } },
    { 50.0, -20.0, { "Etc/GMT-3" } },
    { 50.0, -10.0, { "Etc/GMT-3" } },
    { 50.0, 0.0, { "Etc/GMT-3" } },
    { 50.0, 10.0, { "Africa/Mogadishu" } },
    { 50.0, 20.0, { "Asia/Riyadh" } },
    { 50.0, 30.0, { "Asia/Tehran" } },
    { 50.0, 40.0, { "Etc/GMT-3" } },
    { 50.0, 50.0, { "Asia/Oral" } },
    { 50.0, 60.0, { "Europe/Moscow" } },
    { 50.0, 70.0, { "Etc/GMT-3" } },
    { 50.0, 80.0, { "Europe/Moscow" } },
    { 60.0, -90.0, { "Antarctica/McMurdo" } },
    { 60.0, -80.0, { "Antarctica/Mawson" } },
    { 60.0, -70.0, { "Antarctica/Mawson" } },
    { 60.0, -60.0, { "Etc/GMT-4" } },
    { 60.0, -50.0, { "Etc/GMT-4" } },
    { 60.0, -40.0, { "Etc/GMT-4" } },
    { 60.0, -30.0, { "Etc/GMT-4" } },
    { 60.0, -20.0, { "Etc/GMT-4" } },
    { 60.0, -10.0, { "Etc/GMT-4" } },
    { 60.0, 0.0, { "Etc/GMT-4" } },
    { 60.0, 10.0, { "Etc/GMT-4" } },
    { 60.0, 20.0, { "Etc/GMT-4" } },
    { 60.0, 30.0, { "Asia/Tehran" } },
    { 60.0, 40.0, { "Asia/Ashgabat" } },
    { 60.0, 50.0, { "Asia/Aqtobe" } },
    { 60.0, 60.0, { "Asia/Yekaterinburg" } },
    { 60.0, 70.0, { "Europe/Moscow" } },
    { 60.0, 80.0, { "Europe/Moscow" } },
    { 70.0, -90.0, { "Antarctica/McMurdo" } },
    { 70.0, -80.0, { "Antarctica/Mawson" } },
    { 70.0, -70.0, { "Antarctica/Mawson" } },
    { 70.0, -60.0, { "Etc/GMT-5" } },
    { 70.0, -50.0, { "Etc/GMT-5" } },
    { 70.0, -40.0, { "Etc/GMT-5" } },
    { 70.0, -30.0, { "Etc/GMT-5" } },
    { 70.0, -20.0, { "Etc/GMT-5" } },
    { 70.0, -10.0, { "Etc/GMT-5" } },
    { 70.0, 0.0, { "Etc/GMT-5" } },
    { 70.0, 10.0, { "Etc/GMT-5" } },
    { 70.0, 20.0, { "Etc/GMT-5" } },
    { 70.0, 30.0, { "Asia/Karachi" } },
    { 70.0, 40.0, { "Asia/Bishkek" } },
    { 70.0, 50.0, { "Asia/Almaty" } },
    { 70.0, 60.0, { "Asia/Yekaterinburg" } },
    { 70.0, 70.0, { "Asia/Yekaterinburg" } },
    { 70.0, 80.0, { "Etc/GMT-5" } },
    { 80.0, -90.0, { "Antarctica/McMurdo" } },
    { 80.0, -80.0, { "Antarctica/Davis" } },
    { 80.0, -70.0, { "Antarctica/Davis" } },
    { 80.0, -60.0, { "Etc/GMT-5" } },
    { 80.0, -50.0, { "Etc/GMT-5" } },
    { 80.0, -40.0, { "Etc/GMT-5" } },
    { 80.0, -30.0, { "Etc/GMT-5" } },
    { 80.0, -20.0, { "Etc/GMT-5" } },
    { 80.0, -10.0, { "Etc/GMT-5" } },
    { 80.0, 0.0, { "Etc/GMT-5" } },
    { 80.0, 10.0, { "Asia/Colombo" } },
    { 80.0, 20.0, { "Asia/Kolkata" } },
    { 80.0, 30.0, { "Asia/Kolkata" } },
    { 80.0, 40.0, { "Asia/Shanghai", "Asia/Urumqi" } },
    { 80.0, 50.0, { "Asia/Almaty" } },
    { 80.0, 60.0, { "Asia/Tomsk" } },
    { 80.0, 70.0, { "Asia/Krasnoyarsk" } },
    { 80.0, 80.0, { "Etc/GMT-5" } },
    { 90.0, -90.0, { "Antarctica/McMurdo" } },
    { 90.0, -80.0, { "Antarctica/Vostok" } },
    { 90.0, -70.0, { "Antarctica/Vostok" } },
    { 90.0, -60.0, { "Etc/GMT-6" } },
    { 90.0, -50.0, { "Etc/GMT-6" } },
    { 90.0, -40.0, { "Etc/GMT-6" } },
    { 90.0, -30.0, { "Etc/GMT-6" } },
    { 90.0, -20.0, { "Etc/GMT-6" } },
    { 90.0, -10.0, { "Etc/GMT-6" } },
    { 90.0, 0.0, { "Etc/GMT-6" } },
    { 90.0, 10.0, { "Etc/GMT-6" } },
    { 90.0, 20.0, { "Etc/GMT-6" } },
    { 90.0, 30.0, { "Asia/Shanghai" } },
    { 90.0, 40.0, { "Asia/Shanghai", "Asia/Urumqi" } },
    { 90.0, 50.0, { "Asia/Krasnoyarsk" } },
    { 90.0, 60.0, { "Asia/Krasnoyarsk" } },
    { 90.0, 70.0, { "Asia/Krasnoyarsk" } },
    { 90.0, 80.0, { "Asia/Krasnoyarsk" } },
    { 100.0, -90.0, { "Antarctica/McMurdo" } },
    { 100.0, -80.0, { "Antarctica/Vostok" } },
    { 100.0, -70.0, { "Antarctica/Vostok", "Antarctica/Casey" } },
    { 100.0, -60.0, { "Etc/GMT-7" } },
    { 100.0, -50.0, { "Etc/GMT-7" } },
    { 100.0, -40.0, { "Etc/GMT-7" } },
    { 100.0, -30.0, { "Etc/GMT-7" } },
    { 100.0, -20.0, { "Etc/GMT-7" } },
    { 100.0, -10.0, { "Etc/GMT-7" } },
    { 100.0, 0.0, { "Asia/Jakarta" } },
    { 100.0, 10.0, { "Asia/Bangkok" } },
    { 100.0, 20.0, { "Asia/Bangkok" } },
    { 100.0, 30.0, { "Asia/Shanghai" } },
    { 100.0, 40.0, { "Asia/Shanghai" } },
    { 100.0, 50.0, { "Asia/Ulaanbaatar" } },
    { 100.0, 60.0, { "Asia/Krasnoyarsk" } },
    { 100.0, 70.0, { "Asia/Krasnoyarsk" } },
    { 100.0, 80.0, { "Asia/Krasnoyarsk" } },
    { 110.0, -90.0, { "Antarctica/McMurdo" } },
    { 110.0, -80.0, { "Antarctica/Vostok" } },
    { 110.0, -70.0, { "Antarctica/Vostok", "Antarctica/Casey" } },
    { 110.0, -60.0, { "Etc/GMT-7" } },
    { 110.0, -50.0, { "Etc/GMT-7" } },
    { 110.0, -40.0, { "Etc/GMT-7" } },
    { 110.0, -30.0, { "Etc/GMT-7" } },
    { 110.0, -20.0, { "Etc/GMT-7" } },
    { 110.0, -10.0, { "Etc/GMT-7" } },
    { 110.0, 0.0, { "Asia/Pontianak" } },
    { 110.0, 10.0, { "Etc/GMT-7" } },
    { 110.0, 20.0, { "Asia/Shanghai" } },
    { 110.0, 30.0, { "Asia/Shanghai" } },
    { 110.0, 40.0, { "Asia/Shanghai" } },
    { 110.0, 50.0, { "Asia/Chita" } },
    { 110.0, 60.0, { "Asia/Yakutsk" } },
    { 110.0, 70.0, { "Asia/Yakutsk" } },
    { 110.0, 80.0, { "Etc/GMT-7" } },
    { 120.0, -90.0, { "Antarctica/McMurdo" } },
    { 120.0, -80.0, { "Australia/Perth" } },
    { 120.0, -70.0, { "Australia/Perth", "Antarctica/DumontDUrville", "Antarctica/Casey" } },
    { 120.0, -60.0, { "Etc/GMT-8" } },
    { 120.0, -50.0, { "Etc/GMT-8" } },
    { 120.0, -40.0, { "Etc/GMT-8" } },
    { 120.0, -30.0, { "Australia/Perth" } },
    { 120.0, -20.0, { "Australia/Perth" } },
    { 120.0, -10.0, { "Asia/Makassar" } },
    { 120.0, 0.0, { "Asia/Makassar" } },
    { 120.0, 10.0, { "Asia/Manila" } },
    { 120.0, 20.0, { "Etc/GMT-8" } },
    { 120.0, 30.0, { "Asia/Shanghai" } },
    { 120.0, 40.0, { "Asia/Shanghai" } },
    { 120.0, 50.0, { "Asia/Shanghai" } },
    { 120.0, 60.0, { "Asia/Yakutsk" } },
    { 120.0, 70.0, { "Asia/Yakutsk" } },
    { 120.0, 80.0, { "Etc/GMT-8" } },
    { 130.0, -90.0, { "Antarctica/McMurdo" } },
    { 130.0, -80.0, { "Australia/Perth" } },
    { 130.0, -70.0, { "Australia/Perth", "Antarctica/DumontDUrville" } },
    { 130.0, -60.0, { "Etc/GMT-9" } },
    { 130.0, -50.0, { "Etc/GMT-9" } },
    { 130.0, -40.0, { "Etc/GMT-9" } },
    { 130.0, -30.0, { "Australia/Adelaide" } },
    { 130.0, -20.0, { "Australia/Darwin" } },
    { 130.0, -10.0, { "Etc/GMT-9" } },
    { 130.0, 0.0, { "Asia/Jayapura" } },
    { 130.0, 10.0, { "Etc/GMT-9" } },
    { 130.0, 20.0, { "Etc/GMT-9" } },
    { 130.0, 30.0, { "Asia/Tokyo" } },
    { 130.0, 40.0, { "Etc/GMT-9" } },
    { 130.0, 50.0, { "Asia/Yakutsk" } },
    { 130.0, 60.0, { "Asia/Yakutsk" } },
    { 130.0, 70.0, { "Asia/Yakutsk" } },
    { 130.0, 80.0, { "Etc/GMT-9" } },
    { 140.0, -90.0, { "Antarctica/McMurdo" } },
    { 140.0, -80.0, { "Australia/Perth" } },
    { 140.0, -70.0, { "Australia/Perth", "Antarctica/DumontDUrville" } },
    { 140.0, -60.0, { "Etc/GMT-9" } },
    { 140.0, -50.0, { "Etc/GMT-9" } },
    { 140.0, -40.0, { "Etc/GMT-9" } },
    { 140.0, -30.0, { "Australia/Adelaide" } },
    { 140.0, -20.0, { "Australia/Brisbane" } },
    { 140.0, -10.0, { "Etc/GMT-9" } },
    { 140.0, 0.0, { "Etc/GMT-9" } },
    { 140.0, 10.0, { "Pacific/Chuuk" } },
    { 140.0, 20.0, { "Etc/GMT-9" } },
    { 140.0, 30.0, { "Etc/GMT-9" } },
    { 140.0, 40.0, { "Asia/Tokyo" } },
    { 140.0, 50.0, { "Asia/Vladivostok" } },
    { 140.0, 60.0, { "Asia/Vladivostok" } },
    { 140.0, 70.0, { "Asia/Vladivostok" } },
    { 140.0, 80.0, { "Etc/GMT-9" } },
    { 150.0, -90.0, { "Antarctica/McMurdo" } },
    { 150.0, -80.0, { "Australia/Perth" } },
    { 150.0, -70.0, { "Australia/Perth", "Antarctica/DumontDUrville" } },
    { 150.0, -60.0, { "Etc/GMT-10" } },
    { 150.0, -50.0, { "Etc/GMT-10" } },
    { 150.0, -40.0, { "Etc/GMT-10" } },
    { 150.0, -30.0, { "Australia/Sydney" } },
    { 150.0, -20.0, { "Etc/GMT-10" } },
    { 150.0, -10.0, { "Pacific/Port_Moresby" } },
    { 150.0, 0.0, { "Etc/GMT-10" } },
    { 150.0, 10.0, { "Etc/GMT-10" } },
    { 150.0, 20.0, { "Etc/GMT-10" } },
    { 150.0, 30.0, { "Etc/GMT-10" } },
    { 150.0, 40.0, { "Etc/GMT-10" } },
    { 150.0, 50.0, { "Etc/GMT-10" } },
    { 150.0, 60.0, { "Asia/Magadan" } },
    { 150.0, 70.0, { "Asia/Srednekolymsk" } },
    { 150.0, 80.0, { "Etc/GMT-10" } },
    { 160.0, -90.0, { "Antarctica/McMurdo" } },
    { 160.0, -80.0, { "Australia/Perth", "Antarctica/McMurdo" } },
    { 160.0, -70.0, { "Australia/Perth", "Antarctica/McMurdo", "Antarctica/DumontDUrville" } },
    { 160.0, -60.0, { "Etc/GMT-11" } },
    { 160.0, -50.0, { "Etc/GMT-11" } },
    { 160.0, -40.0, { "Etc/GMT-11" } },
    { 160.0, -30.0, { "Etc/GMT-11" } },
    { 160.0, -20.0, { "Etc/GMT-11" } },
    { 160.0, -10.0, { "Pacific/Guadalcanal" } },
    { 160.0, 0.0, { "Etc/GMT-11" } },
    { 160.0, 10.0, { "Etc/GMT-11" } },
    { 160.0, 20.0, { "Etc/GMT-11" } },
    { 160.0, 30.0, { "Etc/GMT-11" } },
    { 160.0, 40.0, { "Etc/GMT-11" } },
    { 160.0, 50.0, { "Etc/GMT-11" } },
    { 160.0, 60.0, { "Etc/GMT-11" } },
    { 160.0, 70.0, { "Asia/Srednekolymsk" } },
    { 160.0, 80.0, { "Etc/GMT-11" } },
    { 170.0, -90.0, { "Antarctica/McMurdo" } },
    { 170.0, -80.0, { "Antarctica/McMurdo" } },
    { 170.0, -70.0, { "Antarctica/McMurdo" } },
    { 170.0, -60.0, { "Etc/GMT-11" } },
    { 170.0, -50.0, { "Etc/GMT-11" } },
    { 170.0, -40.0, { "Etc/GMT-11" } },
    { 170.0, -30.0, { "Etc/GMT-11" } },
    { 170.0, -20.0, { "Pacific/Efate" } },
    { 170.0, -10.0, { "Etc/GMT-11" } },
    { 170.0, 0.0, { "Etc/GMT-11" } },
    { 170.0, 10.0, { "Pacific/Majuro" } },
    { 170.0, 20.0, { "Etc/GMT-11" } },
    { 170.0, 30.0, { "Etc/GMT-11" } },
    { 170.0, 40.0, { "Etc/GMT-11" } },
    { 170.0, 50.0, { "Etc/GMT-11" } },
    { 170.0, 60.0, { "Asia/Kamchatka" } },
    { 170.0, 70.0, { "Asia/Anadyr" } },
    { 170.0, 80.0, { "Etc/GMT-11" } },

};

void TimeZoneLocationTest::SetUpTestCase(void)
{
}

void TimeZoneLocationTest::TearDownTestCase(void)
{
}

void TimeZoneLocationTest::SetUp(void)
{}

void TimeZoneLocationTest::TearDown(void)
{}

/**
 * @tc.name: TimeZoneLocationTest0001
 * @tc.desc: Test GetTimezoneIdByLocation
 * @tc.type: FUNC
 */
HWTEST_F(TimeZoneLocationTest, TimeZoneLocationTest0001, TestSize.Level1)
{
    int index = 0;
    for (auto iter = timeZoneTestcases.begin(); iter != timeZoneTestcases.end(); ++iter) {
        double longitude = std::get<0>(*iter);
        double latitude = std::get<1>(*iter);
        std::vector<std::string> expects = std::get<2>(*iter);
        vector<std::string> tz = I18nTimeZone::GetTimezoneIdByLocation(longitude, latitude);
        EXPECT_EQ(tz.size(), expects.size());
        if (tz.size() != expects.size()) {
            printf("size not equals at line: %d\r\n", (index+31));
        }
        for (unsigned int i = 0; i < tz.size(); i++) {
            if (i >= expects.size()) {
                printf("at line: %d, tzid: %s, tzSize:%d\r\n", (index+31), tz[i].c_str(), tz.size());
                break;
            }
            EXPECT_EQ(tz[i], expects[i]);
            if (tz[i] != expects[i]) {
                printf("at line: %d\r\n", (index+31));
            }
        }
        ++index;
    }
}

/**
 * @tc.name: TimeZoneLocationTest0002
 * @tc.desc: Test GetTimezoneIdByLocation
 * @tc.type: FUNC
 */
HWTEST_F(TimeZoneLocationTest, TimeZoneLocationTest0002, TestSize.Level1)
{
    // 北京
    vector<std::string> beijing = I18nTimeZone::GetTimezoneIdByLocation(116.3, 39.5);
    EXPECT_TRUE(beijing.size() == 1 && beijing[0] == "Asia/Shanghai");
    // 洛杉矶
    vector<std::string> losAngeles = I18nTimeZone::GetTimezoneIdByLocation(-118.1, 34.0);
    EXPECT_TRUE(losAngeles.size() == 1 && losAngeles[0] == "America/Los_Angeles");
    // 里约热内卢
    vector<std::string> rio = I18nTimeZone::GetTimezoneIdByLocation(-43.1, -22.5);
    EXPECT_TRUE(rio.size() == 1 && rio[0] == "America/Sao_Paulo");
    // 悉尼
    vector<std::string> sydney = I18nTimeZone::GetTimezoneIdByLocation(150.5, -33.55);
    EXPECT_TRUE(sydney.size() == 1 && sydney[0] == "Australia/Sydney");
    // 乌鲁木齐
    vector<std::string> urumqi = I18nTimeZone::GetTimezoneIdByLocation(87.7, 43.8);
    EXPECT_TRUE(urumqi.size() == 2);
    bool containsShanghai = false;
    bool containsUrumqi = false;
    for (unsigned int i = 0; i < urumqi.size(); i++) {
        if (urumqi[i] == "Asia/Shanghai") {
            containsShanghai = true;
        }
        if (urumqi[i] == "Asia/Urumqi") {
            containsUrumqi = true;
        }
    }
    EXPECT_TRUE(containsShanghai);
    EXPECT_TRUE(containsUrumqi);
    // 科伊艾克
    vector<std::string> coyhaique = I18nTimeZone::GetTimezoneIdByLocation(-72.04, -45.34);
    EXPECT_TRUE(coyhaique.size() == 1 && coyhaique[0] == "America/Coyhaique");
}

/**
 * @tc.name: TimeZoneLocationTest0003
 * @tc.desc: Test GetTimezoneIdByLocation
 * @tc.type: FUNC
 */
HWTEST_F(TimeZoneLocationTest, TimeZoneLocationTest0003, TestSize.Level1)
{
    std::vector<std::string> tzIdList = I18nTimeZone::GetTimezoneIdByLocation(106.58314, 22.46933);
    EXPECT_EQ(tzIdList.size(), 1);
    EXPECT_EQ(tzIdList[0], "Asia/Bangkok");
    tzIdList = I18nTimeZone::GetTimezoneIdByLocation(106.7917876, 22.82248);
    EXPECT_EQ(tzIdList.size(), 1);
    EXPECT_EQ(tzIdList[0], "Asia/Bangkok");
    tzIdList = I18nTimeZone::GetTimezoneIdByLocation(106.726289, 22.85502);
    EXPECT_EQ(tzIdList.size(), 1);
    EXPECT_EQ(tzIdList[0], "Asia/Bangkok");
    tzIdList = I18nTimeZone::GetTimezoneIdByLocation(106.8472477, 22.83867);
    EXPECT_EQ(tzIdList.size(), 1);
    EXPECT_EQ(tzIdList[0], "Asia/Bangkok");
    tzIdList = I18nTimeZone::GetTimezoneIdByLocation(126.897702, 41.798380);
    EXPECT_EQ(tzIdList.size(), 1);
    EXPECT_EQ(tzIdList[0], "Asia/Pyongyang");
    printf("==================china and India=========== \r\n");
    tzIdList = I18nTimeZone::GetTimezoneIdByLocation(92.36, 27.11);
    EXPECT_EQ(tzIdList.size(), 1);
    EXPECT_EQ(tzIdList[0], "Asia/Shanghai");
    tzIdList = I18nTimeZone::GetTimezoneIdByLocation(93.80, 27.26);
    EXPECT_EQ(tzIdList.size(), 1);
    EXPECT_EQ(tzIdList[0], "Asia/Shanghai");
    tzIdList = I18nTimeZone::GetTimezoneIdByLocation(95.57, 28.90);
    EXPECT_EQ(tzIdList.size(), 1);
    EXPECT_EQ(tzIdList[0], "Asia/Shanghai");
    tzIdList = I18nTimeZone::GetTimezoneIdByLocation(95.91, 28.78);
    EXPECT_EQ(tzIdList.size(), 1);
    EXPECT_EQ(tzIdList[0], "Asia/Shanghai");
    tzIdList = I18nTimeZone::GetTimezoneIdByLocation(97.03, 28.75);
    EXPECT_EQ(tzIdList.size(), 1);
    EXPECT_EQ(tzIdList[0], "Asia/Shanghai");
    tzIdList = I18nTimeZone::GetTimezoneIdByLocation(80.15, 30.66);
    EXPECT_EQ(tzIdList.size(), 1);
    EXPECT_EQ(tzIdList[0], "Asia/Shanghai");
    tzIdList = I18nTimeZone::GetTimezoneIdByLocation(79.83, 30.90);
    EXPECT_EQ(tzIdList.size(), 1);
    EXPECT_EQ(tzIdList[0], "Asia/Shanghai");
    tzIdList = I18nTimeZone::GetTimezoneIdByLocation(79.03, 31.05);
    EXPECT_EQ(tzIdList.size(), 1);
    EXPECT_EQ(tzIdList[0], "Asia/Shanghai");
    tzIdList = I18nTimeZone::GetTimezoneIdByLocation(78.45, 32.15);
    EXPECT_EQ(tzIdList.size(), 1);
    EXPECT_EQ(tzIdList[0], "Asia/Shanghai");
    tzIdList = I18nTimeZone::GetTimezoneIdByLocation(79.38, 32.79);
    EXPECT_EQ(tzIdList.size(), 1);
    EXPECT_EQ(tzIdList[0], "Asia/Shanghai");
}

/**
 * @tc.name: TimeZoneLocationTest0004
 * @tc.desc: Test GetTimezoneIdByLocation
 * @tc.type: FUNC
 */
HWTEST_F(TimeZoneLocationTest, TimeZoneLocationTest0004, TestSize.Level1)
{
    printf("==================china and Bhutan=========== \r\n");
    std::vector<std::string> tzIdList = I18nTimeZone::GetTimezoneIdByLocation(89.00, 27.51);
    EXPECT_EQ(tzIdList.size(), 1);
    EXPECT_EQ(tzIdList[0], "Asia/Shanghai");
    tzIdList = I18nTimeZone::GetTimezoneIdByLocation(90.82, 28.05);
    EXPECT_EQ(tzIdList.size(), 1);
    EXPECT_EQ(tzIdList[0], "Asia/Shanghai");
    tzIdList = I18nTimeZone::GetTimezoneIdByLocation(88.90, 27.47);
    EXPECT_EQ(tzIdList.size(), 1);
    EXPECT_EQ(tzIdList[0], "Asia/Shanghai");
    tzIdList = I18nTimeZone::GetTimezoneIdByLocation(91.66, 27.79);
    EXPECT_EQ(tzIdList.size(), 1);
    EXPECT_EQ(tzIdList[0], "Asia/Shanghai");
    tzIdList = I18nTimeZone::GetTimezoneIdByLocation(89.09, 27.62);
    EXPECT_EQ(tzIdList.size(), 1);
    EXPECT_EQ(tzIdList[0], "Asia/Shanghai");
    printf("==================Kyrgyzstan=========== \r\n");
    tzIdList = I18nTimeZone::GetTimezoneIdByLocation(72.16912, 41.1918);
    EXPECT_EQ(tzIdList.size(), 1);
    EXPECT_EQ(tzIdList[0], "Asia/Tashkent");
    printf("==================nanhai area=========== \r\n");
    tzIdList = I18nTimeZone::GetTimezoneIdByLocation(117.74, 15.18);
    EXPECT_EQ(tzIdList.size(), 1);
    EXPECT_EQ(tzIdList[0], "Asia/Shanghai");
    tzIdList = I18nTimeZone::GetTimezoneIdByLocation(116.57, 9.73);
    EXPECT_EQ(tzIdList.size(), 1);
    EXPECT_EQ(tzIdList[0], "Asia/Shanghai");
    tzIdList = I18nTimeZone::GetTimezoneIdByLocation(113.83, 7.38);
    EXPECT_EQ(tzIdList.size(), 1);
    EXPECT_EQ(tzIdList[0], "Asia/Shanghai");
    tzIdList = I18nTimeZone::GetTimezoneIdByLocation(113.23, 6.33);
    EXPECT_EQ(tzIdList.size(), 1);
    EXPECT_EQ(tzIdList[0], "Asia/Shanghai");
    tzIdList = I18nTimeZone::GetTimezoneIdByLocation(116.81, 20.68);
    EXPECT_EQ(tzIdList.size(), 1);
    EXPECT_EQ(tzIdList[0], "Asia/Shanghai");
    printf("==================Nantong Reef=========== \r\n");
    tzIdList = I18nTimeZone::GetTimezoneIdByLocation(113.240785, 6.33258);
    EXPECT_EQ(tzIdList.size(), 1);
    EXPECT_EQ(tzIdList[0], "Asia/Shanghai");
    printf("==================Diaoyu Islands=========== \r\n");
    tzIdList = I18nTimeZone::GetTimezoneIdByLocation(123.47, 25.74);
    EXPECT_EQ(tzIdList.size(), 1);
    EXPECT_EQ(tzIdList[0], "Asia/Shanghai");
    tzIdList = I18nTimeZone::GetTimezoneIdByLocation(123.54, 25.72);
    EXPECT_EQ(tzIdList.size(), 1);
    EXPECT_EQ(tzIdList[0], "Asia/Shanghai");
}

/**
 * @tc.name: TimeZoneLocationTest0005
 * @tc.desc: Test GetTimezoneIdByLocation
 * @tc.type: FUNC
 */
HWTEST_F(TimeZoneLocationTest, TimeZoneLocationTest0005, TestSize.Level1)
{
    std::vector<std::string> tzIdList;
    printf("==================oversea=========== \r\n");
    tzIdList = I18nTimeZone::GetTimezoneIdByLocation(35.14, 31.96);
    EXPECT_TRUE(tzIdList.size() == 2 && tzIdList[0] == "Asia/Jerusalem");
    EXPECT_TRUE(tzIdList.size() == 2 && tzIdList[1] == "Asia/Hebron");
    tzIdList = I18nTimeZone::GetTimezoneIdByLocation(35.20, 31.80);
    EXPECT_TRUE(tzIdList.size() == 1 && tzIdList[0] == "Asia/Jerusalem");
    tzIdList = I18nTimeZone::GetTimezoneIdByLocation(35.35, 32.35);
    EXPECT_TRUE(tzIdList.size() == 1 && tzIdList[0] == "Asia/Hebron");
    tzIdList = I18nTimeZone::GetTimezoneIdByLocation(34.10, 44.94);
    EXPECT_TRUE(tzIdList.size() == 1 && tzIdList[0] == "Europe/Simferopol");
    tzIdList = I18nTimeZone::GetTimezoneIdByLocation(33.52, 44.61);
    EXPECT_TRUE(tzIdList.size() == 1 && tzIdList[0] == "Europe/Simferopol");
    tzIdList = I18nTimeZone::GetTimezoneIdByLocation(-13.19, 27.15);
    EXPECT_TRUE(tzIdList.size() == 1 && tzIdList[0] == "Africa/El_Aaiun");
    tzIdList = I18nTimeZone::GetTimezoneIdByLocation(21.15, 42.65);
    EXPECT_TRUE(tzIdList.size() == 1 && tzIdList[0] == "Europe/Belgrade");
    tzIdList = I18nTimeZone::GetTimezoneIdByLocation(21.43, 42.00);
    EXPECT_TRUE(tzIdList.size() == 1 && tzIdList[0] == "Europe/Skopje");
    tzIdList = I18nTimeZone::GetTimezoneIdByLocation(37.80, 48.01);
    EXPECT_TRUE(tzIdList.size() == 1 && tzIdList[0] == "Europe/Kiev");
    tzIdList = I18nTimeZone::GetTimezoneIdByLocation(39.30, 48.57);
    EXPECT_TRUE(tzIdList.size() == 1 && tzIdList[0] == "Europe/Kiev");
    tzIdList = I18nTimeZone::GetTimezoneIdByLocation(-59.79, -51.72);
    EXPECT_TRUE(tzIdList.size() == 1 && tzIdList[0] == "Atlantic/Stanley");
    tzIdList = I18nTimeZone::GetTimezoneIdByLocation(147.79, 45.08);
    EXPECT_TRUE(tzIdList.size() == 1 && tzIdList[0] == "Asia/Magadan");
    tzIdList = I18nTimeZone::GetTimezoneIdByLocation(131.86, 37.24);
    EXPECT_TRUE(tzIdList.size() == 1 && tzIdList[0] == "Asia/Seoul");
    tzIdList = I18nTimeZone::GetTimezoneIdByLocation(33.34, 35.20);
    EXPECT_TRUE(tzIdList.size() == 1 && tzIdList[0] == "Asia/Famagusta");
    tzIdList = I18nTimeZone::GetTimezoneIdByLocation(32.81, 34.77);
    EXPECT_TRUE(tzIdList.size() == 1 && tzIdList[0] == "Asia/Nicosia");
}

/**
 * @tc.name: TimeZoneLocationTest0006
 * @tc.desc: Test GetTimezoneIdByLocation
 * @tc.type: FUNC
 */
HWTEST_F(TimeZoneLocationTest, TimeZoneLocationTest0006, TestSize.Level1)
{
    std::vector<std::string> tzIdList;
    printf("================China border nearby=========== \r\n");
    tzIdList = I18nTimeZone::GetTimezoneIdByLocation(134.274421, 45.171298);
    EXPECT_EQ(tzIdList[0], "Asia/Vladivostok");
    tzIdList = I18nTimeZone::GetTimezoneIdByLocation(128.134032, 53.553832);
    EXPECT_EQ(tzIdList[0], "Asia/Yakutsk");
    tzIdList = I18nTimeZone::GetTimezoneIdByLocation(117.155382, 51.463265);
    EXPECT_EQ(tzIdList[0], "Asia/Chita");
    tzIdList = I18nTimeZone::GetTimezoneIdByLocation(106.099319, 45.34427);
    EXPECT_EQ(tzIdList[0], "Asia/Ulaanbaatar");
    tzIdList = I18nTimeZone::GetTimezoneIdByLocation(90.737938, 48.422181);
    EXPECT_EQ(tzIdList[0], "Asia/Hovd");
    tzIdList = I18nTimeZone::GetTimezoneIdByLocation(87.479194, 50.141346);
    EXPECT_EQ(tzIdList[0], "Asia/Barnaul");
    tzIdList = I18nTimeZone::GetTimezoneIdByLocation(80.892381, 47.443379);
    EXPECT_EQ(tzIdList[0], "Asia/Almaty");
    tzIdList = I18nTimeZone::GetTimezoneIdByLocation(76.194132, 41.301241);
    EXPECT_EQ(tzIdList[0], "Asia/Bishkek");
    tzIdList = I18nTimeZone::GetTimezoneIdByLocation(74.341692, 38.200328);
    EXPECT_EQ(tzIdList[0], "Asia/Dushanbe");
    tzIdList = I18nTimeZone::GetTimezoneIdByLocation(72.765399, 35.498675);
    EXPECT_EQ(tzIdList[0], "Asia/Karachi");
    printf("================1 kilometer away from the China border=========== \r\n");
    tzIdList = I18nTimeZone::GetTimezoneIdByLocation(99.30555, 22.41134);
    EXPECT_EQ(tzIdList[0], "Asia/Shanghai");
    tzIdList = I18nTimeZone::GetTimezoneIdByLocation(99.34955, 22.70645);
    EXPECT_EQ(tzIdList[0], "Asia/Yangon");
    tzIdList = I18nTimeZone::GetTimezoneIdByLocation(73.92596, 39.92811);
    EXPECT_EQ(tzIdList[0], "Asia/Bishkek");
    tzIdList = I18nTimeZone::GetTimezoneIdByLocation(74.43203, 40.13523);
    EXPECT_EQ(tzIdList[0], "Asia/Shanghai");
}
} // namespace I18n
} // namespace Global
} // namespace OHOS