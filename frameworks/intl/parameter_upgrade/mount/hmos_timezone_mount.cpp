/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except", "in compliance with the License.
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
#include <string>
#include <vector>
#include "i18n_hilog.h"
#include "parameter.h"
#include "upgrade_utils.h"

namespace OHOS {
namespace Global {
namespace I18n {
const std::string CFG_PATH =
    "/data/service/el1/public/update/param_service/install/system/etc/TIMEZONE/generic/current/";
const std::string LOCAL_VERSION_PATH = "/system/etc/TIMEZONE/generic/current/version.txt";
const std::string VERSION_FILE = "version.txt";
const std::string PUBKEY_PATH = "/system/etc/i18n/upgrade/hota_i18n_upgrade_v1.pem";
const std::string SAFE_PATH = "/data/service/el1/public/i18n/timezone/";
const std::string MOUNT_DIR = "/system/etc/tzdata_distro/";
const std::string COTA_PARAM_TIMEZONE_KEY = "persist.global.tz_override";

void UpdateTimeZone()
{
    if (SetParameter(COTA_PARAM_TIMEZONE_KEY.c_str(), "false") != 0) {
        HILOG_ERROR_I18N("UpdateTimeZone: Set COTA_PARAM_TIMEZONE_KEY false failed.");
    }
    std::vector<std::string> filesList;
    if (!UpgradeUtils::CheckIfUpdateNecessary(CFG_PATH + VERSION_FILE, LOCAL_VERSION_PATH)) {
        HILOG_ERROR_I18N("UpdateTimeZone: CheckIfUpdateNecessary error, no need to update.");
        return;
    }

    if (!UpgradeUtils::CheckFileIntegrity(CFG_PATH, PUBKEY_PATH, filesList)) {
        HILOG_ERROR_I18N("UpdateTimeZone: CheckFileIntegrity error, no need to update.");
        return;
    }

    if (!UpgradeUtils::CopyDataFile(CFG_PATH, SAFE_PATH, filesList)) {
        HILOG_ERROR_I18N("UpdateTimeZone: Copy data file failed.");
        return;
    }

    if (!UpgradeUtils::FileDirMount(SAFE_PATH, MOUNT_DIR)) {
        HILOG_ERROR_I18N("UpdateTimeZone: Mount error.");
        return;
    }
    if (SetParameter(COTA_PARAM_TIMEZONE_KEY.c_str(), "true") != 0) {
        HILOG_ERROR_I18N("UpdateTimeZone: Set COTA_PARAM_TIMEZONE_KEY true failed.");
    }
    HILOG_INFO_I18N("UpdateTimeZone: Update success.");
}
}
}
}

int main(int argc, char *argv[])
{
    HILOG_INFO_I18N("hmos_timezone_mount: UpdateTimeZone start.");
    OHOS::Global::I18n::UpdateTimeZone();
    return 0;
}