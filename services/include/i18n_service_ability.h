/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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
#include "i18n_service_event.h"
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
    ErrCode SetSystemLanguage(const std::string& language, int32_t& code) override;
    ErrCode SetSystemRegion(const std::string& region, int32_t& code) override;
    ErrCode SetSystemLocale(const std::string& locale, int32_t& code) override;
    ErrCode Set24HourClock(const std::string& flag, int32_t& code) override;
    ErrCode SetUsingLocalDigit(bool flag, int32_t& code) override;
    ErrCode AddPreferredLanguage(const std::string& language, int32_t index, int32_t& code) override;
    ErrCode RemovePreferredLanguage(int32_t index, int32_t& code) override;
    ErrCode SetTemperatureType(int32_t type, int32_t& code) override;
    ErrCode SetFirstDayOfWeek(int32_t type, int32_t& code) override;

protected:
    /**
     * @brief Called when i18n service start.
     */
    void OnStart(const SystemAbilityOnDemandReason& startReason) override;

    /**
     * @brief Called when i18n service stop.
     */
    void OnStop() override;

    /**
     * @brief Called when i18n service unload.
     */
    int32_t OnIdle(const SystemAbilityOnDemandReason& idleReason) override;

private:
    enum class ModifyTaskNumber {
        DECREASE = -1,
        QUERY = 0,
        INCREASE = 1
    };
    /**
     * @brief Trigger unload i18n service after one request finished, but service will not unload
     * until there no request in 10s.
     */
    void DelayUnloadI18nServiceAbility();
    int32_t UpdateTaskNumber(ModifyTaskNumber action);
    void StartTask(int32_t pid);
    void FinishTask(int32_t pid);
    int32_t GetCallingUserId();
    I18nErrorCode CheckPermission();
    // i18n service unload event handler.
    std::shared_ptr<AppExecFwk::EventHandler> handler = nullptr;
    std::unique_ptr<I18nServiceEvent> i18nServiceEvent = nullptr;
    std::mutex taskNumberMutex;
    int32_t taskNumber;
};
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif // OHOS_GLOBAL_I18N_I18N_SERVICE_ABILITY_H