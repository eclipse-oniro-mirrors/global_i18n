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
#include "api_version.h"
#include "bundle_mgr_interface.h"
#include "i18n_hilog.h"
#include "ipc_skeleton.h"
#include "iremote_broker.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace Global {
namespace I18n {
const int32_t ApiVersion::API_VERSION_MOD = 1000;
std::mutex ApiVersion::initMutex;
thread_local int32_t sysApiVersion = 0;
std::unordered_map<std::string, int32_t> ApiVersion::versionMap {
    {"isSuggested", 16},
};

ApiCompareResult ApiVersion::CheckApiTargetVersion(const std::string &apiName)
{
    auto iter = versionMap.find(apiName);
    if (iter == versionMap.end()) {
        HILOG_ERROR_I18N("CheckApiTargetVersion: invalid api name: %{public}s", apiName.c_str());
        return ApiCompareResult::INVALID_PARAM;
    }
    int32_t version = iter->second;
    int32_t currentVersion = InitSystemApiVersion();
    if (currentVersion == 0) {
        HILOG_ERROR_I18N("CheckApiTargetVersion: get api target version is 0.");
        return ApiCompareResult::SYS_ERROR;
    }
    return currentVersion >= version ? ApiCompareResult::GREATER_THAN : ApiCompareResult::LESS_THAN;
}

int32_t ApiVersion::InitSystemApiVersion()
{
    if (sysApiVersion > 0) {
        return sysApiVersion;
    }
    std::lock_guard<std::mutex> systemApiLock(initMutex);
    if (sysApiVersion > 0) {
        return sysApiVersion;
    }
    int32_t version = GetApiTargetVersion();
    HILOG_INFO_I18N("InitSystemApiVersion: Current api version is %{public}d.", version);
    if (version > 0) {
        sysApiVersion = version;
    }
    return version;
}

int32_t ApiVersion::GetApiTargetVersion()
{
    auto saManager = OHOS::SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (saManager == nullptr) {
        HILOG_ERROR_I18N("GetApiTargetVersion: saManager is nullptr");
        return 0;
    }
    OHOS::sptr<OHOS::IRemoteObject> remoteObject =
        saManager->GetSystemAbility(OHOS::BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    OHOS::sptr<OHOS::AppExecFwk::IBundleMgr> bundleMgrProxy =
        OHOS::iface_cast<OHOS::AppExecFwk::IBundleMgr>(remoteObject);
    if (bundleMgrProxy == nullptr) {
        HILOG_ERROR_I18N("GetApiTargetVersion: bundleMgrProxy is nullptr");
        return 0;
    }
    OHOS::AppExecFwk::BundleInfo bundleInfo;
    auto ret =
        bundleMgrProxy->GetBundleInfoForSelf(OHOS::AppExecFwk::BundleFlag::GET_BUNDLE_WITH_ABILITIES, bundleInfo);
    if (ret != 0) {
        HILOG_ERROR_I18N("GetApiTargetVersion: GetBundleInfoForSelf failed");
        return 0;
    }
    int32_t hapApiVersion = bundleInfo.applicationInfo.apiTargetVersion % API_VERSION_MOD;
    return hapApiVersion;
}
} // namespace I18n
} // namespace Global
} // namespace OHOS
