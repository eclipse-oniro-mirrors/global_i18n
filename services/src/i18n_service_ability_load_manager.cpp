/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "i18n_hilog.h"
#include "i18n_service_ability_load_callback.h"
#include "i18n_service_ability_load_manager.h"

namespace OHOS {
namespace Global {
namespace I18n {
static constexpr int32_t I18N_LOAD_SA_TIMEOUT_MS = 1000;

I18nServiceAbilityLoadManager::I18nServiceAbilityLoadManager()
{
}

I18nServiceAbilityLoadManager::~I18nServiceAbilityLoadManager()
{
}

sptr<IRemoteObject> I18nServiceAbilityLoadManager::GetI18nServiceAbility(int32_t systemAbilityId)
{
    sptr<ISystemAbilityManager> samgr = LoadI18nServiceAbility(systemAbilityId);
    if (samgr == nullptr) {
        // try again
        samgr = LoadI18nServiceAbility(systemAbilityId);
    }
    if (samgr == nullptr) {
        return nullptr;
    }
    return samgr->GetSystemAbility(systemAbilityId, "");
}

sptr<ISystemAbilityManager> I18nServiceAbilityLoadManager::LoadI18nServiceAbility(int32_t systemAbilityId)
{
    InitLoadState();
    sptr<ISystemAbilityManager> samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (samgr == nullptr) {
        HILOG_ERROR_I18N("I18nServiceAbilityLoadManager::LoadAndGetI18nServiceAbility can't get samgr");
        return nullptr;
    }
    sptr<I18nServiceAbilityLoadCallback> i18nSaLoadCallback = new I18nServiceAbilityLoadCallback();
    int32_t ret = samgr->LoadSystemAbility(systemAbilityId, i18nSaLoadCallback);
    if (ret != ERR_OK) {
        HILOG_ERROR_I18N(
            "I18nServiceAbilityLoadManager::LoadAndGetI18nServiceAbility LoadSystemAbility failed.");
        return nullptr;
    }
    bool status = WaitLoadStateChange(systemAbilityId);
    if (!status) {
        HILOG_ERROR_I18N("I18nServiceAbilityLoadManager::LoadAndGetI18nServiceAbility wait overtime.");
        return nullptr;
    }
    return samgr;
}

void I18nServiceAbilityLoadManager::InitLoadState()
{
    std::unique_lock<std::mutex> lock(loadStateMutex);
    loadState = false;
}

bool I18nServiceAbilityLoadManager::WaitLoadStateChange(int32_t systemAbilityId)
{
    std::unique_lock<std::mutex> lock(loadStateMutex);
    auto isLoadSuccess = loadStateCondition.wait_for(lock, std::chrono::milliseconds(I18N_LOAD_SA_TIMEOUT_MS), [this] {
        return loadState;
    });
    if (!isLoadSuccess) {
        HILOG_ERROR_I18N("I18nServiceAbilityLoadManager::WaitLoadStateChange timeout.");
    }
    return isLoadSuccess;
}

bool I18nServiceAbilityLoadManager::UnloadI18nService(int32_t systemAbilityId)
{
    sptr<ISystemAbilityManager> samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (samgr == nullptr) {
        HILOG_ERROR_I18N("I18nServiceAbilityLoadManager::UnloadI18nService can't get samgr.");
        return false;
    }
    int32_t ret = samgr->UnloadSystemAbility(systemAbilityId);
    if (ret != ERR_OK) {
        HILOG_ERROR_I18N("I18nServiceAbilityLoadManager::UnloadI18nService sa unload failed.");
        return false;
    }
    return true;
}

void I18nServiceAbilityLoadManager::LoadSystemAbilitySuccess()
{
    std::unique_lock<std::mutex> lock(loadStateMutex);
    loadState = true;
    loadStateCondition.notify_one();
}

void I18nServiceAbilityLoadManager::LoadSystemAbilityFail()
{
    std::unique_lock<std::mutex> lock(loadStateMutex);
    loadState = false;
    loadStateCondition.notify_one();
}
} // namespace I18n
} // namespace Global
} // namespace OHOS