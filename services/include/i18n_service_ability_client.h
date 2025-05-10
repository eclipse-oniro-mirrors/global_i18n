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

#ifndef OHOS_GLOBAL_I18N_I18N_SERVICE_ABILITY_CLIENT_H
#define OHOS_GLOBAL_I18N_I18N_SERVICE_ABILITY_CLIENT_H

#include <string>
#include "ii18n_service_ability.h"
#include "i18n_types.h"

namespace OHOS {
namespace Global {
namespace I18n {
/**
 * @brief I18nServiceAbilityClient is i18n service entry class, provides i18n service entry function,
 * such as SetSystemLanguage.
 */
class I18nServiceAbilityClient {
public:
    /**
     * @brief I18n service entry for setting system language.
     *
     * @param language language tag to set.
     * @return I18nErrorCode Return SUCCESS indicates that the setting was successful.
     */
    static I18nErrorCode SetSystemLanguage(const std::string &language);

    /**
     * @brief I18n service entry for setting system region.
     *
     * @param region region tag to set.
     * @return I18nErrorCode Return SUCCESS indicates that the setting was successful.
     */
    static I18nErrorCode SetSystemRegion(const std::string &region);

    /**
     * @brief I18n service entry for setting system locale.
     *
     * @param locale locale tag to set.
     * @return I18nErrorCode Return SUCCESS indicates that the setting was successful.
     */
    static I18nErrorCode SetSystemLocale(const std::string &locale);

    /**
     * @brief I18n service entry for setting the system to use 12 or 24-hour clock.
     *
     * @param flag When flag is true, the system adopts 24-hour clock;
     * When flag is false, the system adopts 12-hour clock; When flag is empty string, the system adopts
     * system locale default behavior.
     * @return I18nErrorCode Return SUCCESS indicates that the setting was successful.
     */
    static I18nErrorCode Set24HourClock(const std::string &flag);

    /**
     * @brief I18n service entry for setting whether the system uses local numbers.
     *
     * @param flag When flag is True, the system adopts local digit;
     * When flag is False, the system doesn't adopt local digit;
     * @return I18nErrorCode Return SUCCESS indicates that the setting was successful.
     */
    static I18nErrorCode SetUsingLocalDigit(bool flag);

    /**
     * @brief I18n service entry for add system preferred language.
     *
     * @param language preferred language tag to add.
     * @param index preferred language'index to add.
     * @return I18nErrorCode Return SUCCESS indicates that the setting was successful.
     */
    static I18nErrorCode AddPreferredLanguage(const std::string &language, int32_t index);

    /**
     * @brief I18n service entry for remove system preferred language.
     *
     * @param index preferred language's index to remove.
     * @return I18nErrorCode Return SUCCESS indicates that the setting was successful.
     */
    static I18nErrorCode RemovePreferredLanguage(int32_t index);
    
    /**
     * @brief Set user's preferrence for temperature type.
     *
     * @param type temperature type to set.
     * @return I18nErrorCode Return SUCCESS indicates that the setting was successful.
     */
    static I18nErrorCode SetTemperatureType(TemperatureType type);

    /**
     * @brief Set user's preferrence for first day of week.
     *
     * @param type one of week days.
     * @return I18nErrorCode Return SUCCESS indicates that the setting was successful.
     */
    static I18nErrorCode SetFirstDayOfWeek(WeekDay type);

private:
    /**
     * @brief Get the proxy of i18n service to access service.
     *
     * @param err Indicates whether the acquisition was successful.
     * @return sptr<II18nServiceAbility> I18n service proxy.
     */
    static sptr<II18nServiceAbility> GetProxy(I18nErrorCode &err);
};
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif // OHOS_GLOBAL_I18N_I18N_SERVICE_ABILITY_CLIENT_H