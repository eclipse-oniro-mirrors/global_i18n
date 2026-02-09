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
#include "i18n_types.h"

namespace OHOS {
template <typename T>
class sptr;

namespace Global {
namespace I18n {
class II18nServiceAbility;

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

    /**
     * @brief Gets collations supported by system locale.
     *
     * @param systemCollations The map will containing the collation's identifier and name.
     * @return I18nErrorCode Return SUCCESS indicates that the getting was successful.
     */
    static I18nErrorCode GetSystemCollations(std::unordered_map<std::string, std::string>& systemCollations);

    /**
     * @brief Gets collation currently used by system locale.
     *
     * @param usingCollation The identifier of the collation model used by system locale will be return.
     * @return I18nErrorCode Return SUCCESS indicates that the getting was successful.
     */
    static I18nErrorCode GetUsingCollation(std::string& usingCollation);

    /**
     * @brief Sets the system collation mode.
     *
     * @param identifier Identifier of the collation mode.
     * @return I18nErrorCode Return SUCCESS indicates that the setting was successful.
     */
    static I18nErrorCode SetSystemCollation(const std::string& identifier);

    /**
     * @brief Gets numbering systems supported by system locale.
     *
     * @param systemNumberingSystems a map will containing the numbering system's identifier and sample.
     * @return I18nErrorCode Return SUCCESS indicates that the getting was successful.
     */
    static I18nErrorCode GetSystemNumberingSystems(
        std::unordered_map<std::string, std::string>& systemNumberingSystems);

    /**
     * @brief Gets numbering system currently used by system locale.
     *
     * @param usingNumberingSystem the numbering systems's identifier
     * @return I18nErrorCode Return SUCCESS indicates that the getting was successful.
     */
    static I18nErrorCode GetUsingNumberingSystem(std::string& usingNumberingSystem);

    /**
     * @brief Sets the numbering system used by the system locale.
     *
     * @param identifier Identifier of the numbering system.
     * @return I18nErrorCode Return SUCCESS indicates that the setting was successful.
     */
    static I18nErrorCode SetSystemNumberingSystem(const std::string& identifier);

    /**
     * @brief Gets commonly used number patterns for system locale.
     *
     * @param systemNumberPatterns a map containing the used number patterns and example of system locale.
     * @return I18nErrorCode Return SUCCESS indicates that the getting was successful.
     */
    static I18nErrorCode GetSystemNumberPatterns(std::unordered_map<std::string, std::string>& systemNumberPatterns);

    /**
     * @brief Gets number pattern used by system locale.
     *
     * @param usingNumberPattern The number pattern identifier used by system locale.
     * @return I18nErrorCode Return SUCCESS indicates that the getting was successful.
     */
    static I18nErrorCode GetUsingNumberPattern(std::string& usingNumberPattern);

    /**
     * @brief Sets the number pattern used by the system locale.
     *
     * @param pattern Identifier of the number pattern.
     * @return I18nErrorCode Return SUCCESS indicates that the setting was successful.
     */
    static I18nErrorCode SetSystemNumberPattern(const std::string& pattern);

    /**
     * @brief Gets measurements supported by system locale.
     *
     * @param systemMeasurements a map will containing identifier and name of measurements supported by system locale.
     * @return I18nErrorCode Return SUCCESS indicates that the setting was successful.
     */
    static I18nErrorCode GetSystemMeasurements(std::unordered_map<std::string, std::string>& systemMeasurements);

    /**
     * @brief Gets measurement currently used by system locale.
     *
     * @param usingMeasurement The identifier of measurement system using by system locale.
     * @return I18nErrorCode Return SUCCESS indicates that the getting was successful.
     */
    static I18nErrorCode GetUsingMeasurement(std::string& identifier);

    /**
     * @brief Sets the measurement system used by the system locale.
     *
     * @param identifier Identifier of the measurement system.
     * @return I18nErrorCode Return SUCCESS indicates that the setting was successful.
     */
    static I18nErrorCode SetSystemMeasurement(const std::string& identifier);

    /**
     * @brief Gets numerical date patterns and examples supported by system locale.
     *
     * @param numericalDatePatterns a map containing the date patterns and examples.
     * @return I18nErrorCode Return SUCCESS indicates that the setting was successful.
     */
    static I18nErrorCode GetSystemNumericalDatePatterns(
        std::unordered_map<std::string, std::string>& numericalDatePatterns);

    /**
     * @brief Gets numerical date pattern currently used by system locale.
     *
     * @param identifier Identifier of numerical date pattern used by system locale
     * @return I18nErrorCode Return SUCCESS indicates that the getting was successful.
     */
    static I18nErrorCode GetUsingNumericalDatePattern(std::string& identifier);

    /**
     * @brief Sets the numerical date pattern used by the system locale.
     *
     * @param identifier Identifier of the numerical date pattern.
     * @return I18nErrorCode Return SUCCESS indicates that the setting was successful.
     */
    static I18nErrorCode SetSystemNumericalDatePattern(const std::string& identifier);

    static std::string GetLanguageFromUserId(int32_t userId);

private:
    /**
     * @brief Construct a new I18nServiceAbilityClient::CallI18nServiceMethod object
     *
     * @tparam Method Method type provided by I18nServiceAbility.
     * @tparam Args Args type for I18nServiceAbility's method.
     * @param method I18nServiceAbility's method pointer.
     * @param methodName I18nServiceAbility's method name.
     * @param args Args for I18nServiceAbility's method .
     * @return I18nErrorCode
     */
    template<typename Method, typename... Args>
    static I18nErrorCode CallI18nServiceMethod(Method method, const char* methodName, Args&&... args);

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