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

#ifndef OHOS_GLOBAL_I18N_SYSTEM_ADDON_H
#define OHOS_GLOBAL_I18N_SYSTEM_ADDON_H

#include "ani.h"
#include <vector>

namespace OHOS {
namespace Global {
namespace I18n {
class I18nSystemAddon {
public:
    static ani_string GetDisplayCountry(ani_env *env, [[maybe_unused]] ani_object object,
        ani_string country, ani_string locale, ani_object sentenceCase);
    static ani_string GetDisplayLanguage(ani_env *env, [[maybe_unused]] ani_object object,
        ani_string language, ani_string locale, ani_object sentenceCase);
    static ani_object GetSystemLanguages(ani_env *env, [[maybe_unused]] ani_object object);
    static ani_object GetSystemCountries(ani_env *env, [[maybe_unused]] ani_object object, ani_string language);
    static ani_boolean IsSuggested(ani_env *env, [[maybe_unused]] ani_object object,
        ani_string language, ani_string region);
    static ani_string GetSystemLanguage(ani_env *env, [[maybe_unused]] ani_object object);
    static void SetSystemLanguage(ani_env *env, [[maybe_unused]] ani_object object, ani_string language);
    static ani_string GetSystemRegion(ani_env *env, [[maybe_unused]] ani_object object);
    static void SetSystemRegion(ani_env *env, [[maybe_unused]] ani_object object, ani_string region);
    static ani_string GetSystemLocale(ani_env *env, [[maybe_unused]] ani_object object);
    static void SetSystemLocale(ani_env *env, [[maybe_unused]] ani_object object, ani_string locale);
    static ani_boolean Is24HourClock([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object object);
    static void Set24HourClock([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object object, ani_boolean option);
    static void AddPreferredLanguage(ani_env *env, [[maybe_unused]] ani_object object,
        ani_string language, ani_object number);
    static void RemovePreferredLanguage([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object object,
        ani_int index);
    static ani_object GetPreferredLanguageList(ani_env *env, [[maybe_unused]] ani_object object);
    static ani_string GetFirstPreferredLanguage(ani_env *env, [[maybe_unused]] ani_object object);
    static void SetAppPreferredLanguage(ani_env *env, [[maybe_unused]] ani_object object, ani_string language);
    static ani_string GetAppPreferredLanguage(ani_env *env, [[maybe_unused]] ani_object object);
    static void SetUsingLocalDigit([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object object, ani_boolean flag);
    static ani_boolean GetUsingLocalDigit([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object object);
    static ani_string GetSimplifiedLanguage([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object object,
        ani_string language);
    static void SetTemperatureType([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object object,
        ani_enum_item type);
    static ani_enum_item GetTemperatureType([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object object);
    static ani_string GetTemperatureName([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object object,
        ani_enum_item type);
    static void SetFirstDayOfWeek([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object object,
        ani_enum_item type);
    static ani_enum_item GetFirstDayOfWeek([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object object);
    static ani_object GetSystemCollations(ani_env *env, [[maybe_unused]] ani_object object);
    static ani_string GetUsingCollation(ani_env *env, [[maybe_unused]] ani_object object);
    static void SetSystemCollation(ani_env *env, [[maybe_unused]] ani_object object, ani_string aniIdentifier);
    static ani_object GetSystemMeasurements([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object object);
    static ani_string GetUsingMeasurement([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object object);
    static void SetSystemMeasurement([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object object,
        ani_string aniIdentifier);
    static ani_string GetUsingNumberingSystem([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object object);
    static void SetSystemNumberingSystem([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object object,
        ani_string aniIdentifier);
    static ani_object GetSystemNumberingSystems([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object object);
    static ani_object GetSystemNumberPatterns([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object object);
    static void SetSystemNumberPattern([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object object,
        ani_string aniPattern);
    static ani_string GetUsingNumberPattern([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object object);
    static ani_object GetSystemNumericalDatePatterns([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object object);
    static void SetSystemNumericalDatePattern([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object object,
        ani_string aniIdentifier);
    static ani_string GetUsingNumericalDatePattern([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object object);
    static ani_object GetSystemLocaleInstance([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object object);
    static ani_status BindContextSystem(ani_env *env);

private:
    static const std::vector<ani_native_function> METHODS;
};
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif
