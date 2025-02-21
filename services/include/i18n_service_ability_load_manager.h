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

#ifndef OHOS_GLOBAL_I18N_I18N_SERVICE_ABILITY_LOAD_MANAGER_H
#define OHOS_GLOBAL_I18N_I18N_SERVICE_ABILITY_LOAD_MANAGER_H

#include <mutex>
#include <singleton.h>
#include "iremote_object.h"
#include "iservice_registry.h"

namespace OHOS {
namespace Global {
namespace I18n {
/**
 * @brief Provides i18n service loading and unloading functions.
 */
class I18nServiceAbilityLoadManager : public DelayedSingleton<I18nServiceAbilityLoadManager> {
public:
    I18nServiceAbilityLoadManager();
    ~I18nServiceAbilityLoadManager();

    /**
     * @brief Get i18n service proxy.
     *
     * @param systemAbilityId i18n system ability id.
     * @return sptr<IRemoteObject> i18n service proxy.
     */
    sptr<IRemoteObject> GetI18nServiceAbility(int32_t systemAbilityId);
    bool UnloadI18nService(int32_t systemAbilityId);

    /**
     * @brief Provided for I18nServiceAbilityLoadCallback to Update i18n service loading status to success.
     */
    void LoadSystemAbilitySuccess();

    /**
     * @brief Provided for I18nServiceAbilityLoadCallback to Update i18n service loading status to failed.
     */
    void LoadSystemAbilityFail();

private:
    sptr<ISystemAbilityManager> LoadI18nServiceAbility(int32_t systemAbilityId);
    void InitLoadState();

    /**
     * @brief Wait i18n system ability load.
     *
     * @param systemAbilityId i18n system ability id.
     * @return true Returning true indicates that the loading process has ended.
     * @return false Returning false indicates that the loading process has been overtime.
     */
    bool WaitLoadStateChange(int32_t systemAbilityId);
    std::condition_variable loadStateCondition;
    std::mutex loadStateMutex;
    bool loadState = false;
};
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif // OHOS_GLOBAL_I18N_I18N_SERVICE_ABILITY_LOAD_MANAGER_H