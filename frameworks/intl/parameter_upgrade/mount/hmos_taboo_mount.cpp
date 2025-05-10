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
#include "upgrade_utils.h"

namespace OHOS {
namespace Global {
namespace I18n {
const std::string CFG_PATH =
    "/data/service/el1/public/update/param_service/install/system/etc/TABOO/generic/";
const std::string LOCAL_VERSION_PATH = "/system/etc/TABOO/generic/version.txt";
const std::string VERSION_FILE = "version.txt";
const std::string PUBKEY_PATH = "/system/etc/i18n/upgrade/hota_i18n_upgrade_v1.pem";
const std::string SAFE_PATH = "/data/service/el1/public/i18n/taboo/";
const std::string MOUNT_DIR = "/system/etc/taboo_res/";

void UpdateTaboo()
{
    std::vector<std::string> filesList;
    if (!UpgradeUtils::CheckIfUpdateNecessary(CFG_PATH + VERSION_FILE, LOCAL_VERSION_PATH)) {
        HILOG_ERROR_I18N("UpdateTaboo: CheckIfUpdateNecessary error, no need to update.");
        return;
    }

    if (!UpgradeUtils::CheckFileIntegrity(CFG_PATH, PUBKEY_PATH, filesList)) {
        HILOG_ERROR_I18N("UpdateTaboo: CheckFileIntegrity error, no need to update.");
        return;
    }

    if (!UpgradeUtils::CopyDataFile(CFG_PATH, SAFE_PATH, filesList)) {
        HILOG_ERROR_I18N("UpdateTaboo: Copy data file failed.");
        return;
    }

    if (!UpgradeUtils::FileDirMount(SAFE_PATH, MOUNT_DIR)) {
        HILOG_ERROR_I18N("UpdateTaboo: Mount taboo_res error.");
        return;
    }

    HILOG_INFO_I18N("UpdateTaboo: Update success.");
}
}
}
}

int main(int argc, char *argv[])
{
    HILOG_INFO_I18N("hmos_taboo_mount: UpdateTaboo start.");
    OHOS::Global::I18n::UpdateTaboo();
    return 0;
}