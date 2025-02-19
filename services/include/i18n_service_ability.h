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

#ifndef OHOS_GLOBAL_I18N_I18N_SERVICE_ABILITY_H
#define OHOS_GLOBAL_I18N_I18N_SERVICE_ABILITY_H

#include "event_handler.h"
#include "i18n_service_ability_stub.h"
#include "system_ability.h"

namespace OHOS {
namespace Global {
namespace I18n {
class I18nServiceAbility : public SystemAbility, public I18nServiceAbilityStub {
    DECLARE_SYSTEM_ABILITY(I18nServiceAbility);
public:
    DISALLOW_COPY_AND_MOVE(I18nServiceAbility);
    I18nServiceAbility(int32_t saId, bool runOnCreate);
    ~I18nServiceAbility();

    // The following are the i18n service implement functions.
    I18nErrorCode SetSystemLanguage(const std::string &language) override;
    I18nErrorCode SetSystemRegion(const std::string &region) override;
    I18nErrorCode SetSystemLocale(const std::string &locale) override;
    I18nErrorCode Set24HourClock(const std::string &flag) override;
    I18nErrorCode SetUsingLocalDigit(bool flag) override;
    I18nErrorCode AddPreferredLanguage(const std::string &language, int32_t index) override;
    I18nErrorCode RemovePreferredLanguage(int32_t index) override;
    /**
     * @brief Trigger unload i18n service after one request finished, but service will not unload
     * until there no request in 10s.
     */
    void UnloadI18nServiceAbility() override;

protected:
    /**
     * @brief Called when i18n service start.
     */
    void OnStart() override;

    /**
     * @brief Called when i18n service stop.
     */
    void OnStop() override;

private:
    // i18n service unload event handler.
    std::shared_ptr<AppExecFwk::EventHandler> handler;
};
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif // OHOS_GLOBAL_I18N_I18N_SERVICE_ABILITY_H