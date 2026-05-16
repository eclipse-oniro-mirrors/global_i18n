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
#ifndef OHOS_GLOBAL_I18N_SYSTEM_ADDON_H
#define OHOS_GLOBAL_I18N_SYSTEM_ADDON_H

#include <memory>
#include <vector>
#include "i18n_timezone.h"
#include "i18n_types.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"

namespace OHOS {
namespace Global {
namespace I18n {
class I18nSystemAddon {
public:
    I18nSystemAddon();
    ~I18nSystemAddon();
    static void Destructor(napi_env env, void *nativeObject, void *finalize_hint);
    static napi_value InitI18nSystem(napi_env env, napi_value exports);

    static napi_value GetSystemLanguage(napi_env env, napi_callback_info info);
    static napi_value GetSystemCountriesWithError(napi_env env, napi_callback_info info);
    static napi_value GetSystemRegion(napi_env env, napi_callback_info info);
    static napi_value GetSystemLocale(napi_env env, napi_callback_info info);
    static napi_value GetPreferredLanguageList(napi_env env, napi_callback_info info);
    static napi_value GetFirstPreferredLanguage(napi_env env, napi_callback_info info);
    static napi_value Is24HourClock(napi_env env, napi_callback_info info);
    static napi_value Set24HourClock(napi_env env, napi_callback_info info);
    static napi_value AddPreferredLanguage(napi_env env, napi_callback_info info);
    static napi_value RemovePreferredLanguage(napi_env env, napi_callback_info info);
    static napi_value GetDisplayLanguage(napi_env env, napi_callback_info info);
    static napi_value GetDisplayCountry(napi_env env, napi_callback_info info);

private:
    static std::vector<napi_property_descriptor> GetI18nSystemProperty();
    static napi_value GetSystemCountries(napi_env env, napi_callback_info info);
    static napi_value IsSuggested(napi_env env, napi_callback_info info);
    static napi_value SetSystemLanguage(napi_env env, napi_callback_info info);
    static napi_value GetDisplayCountryWithError(napi_env env, napi_callback_info info);
    static napi_value GetDisplayLanguageWithError(napi_env env, napi_callback_info info);
    static napi_value GetSystemLanguages(napi_env env, napi_callback_info info);
    static napi_value IsSuggestedWithError(napi_env env, napi_callback_info info);
    static napi_value SetSystemLanguageWithError(napi_env env, napi_callback_info info);
    static napi_value SetSystemRegion(napi_env env, napi_callback_info info);
    static napi_value SetSystemRegionWithError(napi_env env, napi_callback_info info);
    static napi_value SetSystemLocale(napi_env env, napi_callback_info info);
    static napi_value SetSystemLocaleWithError(napi_env env, napi_callback_info info);
    static napi_value Set24HourClockWithError(napi_env env, napi_callback_info info);
    static napi_value AddPreferredLanguageWithError(napi_env env, napi_callback_info info);
    static napi_value RemovePreferredLanguageWithError(napi_env env, napi_callback_info info);
    static napi_value SetAppPreferredLanguage(napi_env env, napi_callback_info info);
    static napi_value GetAppPreferredLanguage(napi_env env, napi_callback_info info);
    static napi_value SetUsingLocalDigitAddon(napi_env env, napi_callback_info info);
    static napi_value SetUsingLocalDigitAddonWithError(napi_env env, napi_callback_info info);
    static napi_value GetUsingLocalDigitAddon(napi_env env, napi_callback_info info);
    static napi_value GetSimplifiedLanguage(napi_env env, napi_callback_info info);

    static napi_value GetDisplayCountryImpl(napi_env env, napi_callback_info info, bool throwError);
    static napi_value GetDisplayLanguageImpl(napi_env env, napi_callback_info info, bool throwError);
    static napi_value GetSystemCountriesImpl(napi_env env, napi_callback_info info, bool throwError);
    static napi_value IsSuggestedImpl(napi_env env, napi_callback_info info, bool throwError);
    static bool GetApiCheckResult();
    static bool IsSuggestedByApiVersion(const std::string &language, bool newApi);
    static bool IsSuggestedByApiVersion(const std::string &language,
        const std::string &region, bool newApi);
    static napi_value SetSystemLanguageImpl(napi_env env, napi_callback_info info, bool throwError);
    static napi_value SetSystemRegionImpl(napi_env env, napi_callback_info info, bool throwError);
    static napi_value SetSystemLocaleImpl(napi_env env, napi_callback_info info, bool throwError);
    static napi_value Set24HourClockImpl(napi_env env, napi_callback_info info, bool throwError);
    static napi_value AddPreferredLanguageImpl(napi_env env, napi_callback_info info, bool throwError);
    static napi_value RemovePreferredLanguageImpl(napi_env env, napi_callback_info info, bool throwError);
    static bool CheckPreferredLanguageIndex(int index);
    static napi_value SetUsingLocalDigitAddonImpl(napi_env env, napi_callback_info info, bool throwError);

    static bool ParseStringParam(napi_env env, napi_value argv, bool throwError, std::string &strParam);
    static napi_value SetTemperatureType(napi_env env, napi_callback_info info);
    static napi_value GetTemperatureType(napi_env env, napi_callback_info info);
    static napi_value GetTemperatureName(napi_env env, napi_callback_info info);
    static bool ParseTemperatureType(napi_env env, napi_callback_info info, TemperatureType& type);
    static napi_value SetFirstDayOfWeek(napi_env env, napi_callback_info info);
    static napi_value GetFirstDayOfWeek(napi_env env, napi_callback_info info);
    static napi_value GetSystemLocaleInstance(napi_env env, napi_callback_info info);
    static napi_value GetSystemCollations(napi_env env, napi_callback_info info);
    static napi_value GetUsingCollation(napi_env env, napi_callback_info info);
    static napi_value SetSystemCollation(napi_env env, napi_callback_info info);
    static napi_value GetSystemNumberingSystems(napi_env env, napi_callback_info info);
    static napi_value GetUsingNumberingSystem(napi_env env, napi_callback_info info);
    static napi_value SetSystemNumberingSystem(napi_env env, napi_callback_info info);
    static napi_value GetSystemNumberPatterns(napi_env env, napi_callback_info info);
    static napi_value GetUsingNumberPattern(napi_env env, napi_callback_info info);
    static napi_value SetSystemNumberPattern(napi_env env, napi_callback_info info);
    static napi_value GetSystemMeasurements(napi_env env, napi_callback_info info);
    static napi_value GetUsingMeasurement(napi_env env, napi_callback_info info);
    static napi_value SetSystemMeasurement(napi_env env, napi_callback_info info);
    static napi_value GetSystemNumericalDatePatterns(napi_env env, napi_callback_info info);
    static napi_value GetUsingNumericalDatePattern(napi_env env, napi_callback_info info);
    static napi_value SetSystemNumericalDatePattern(napi_env env, napi_callback_info info);
    static const std::string NAME_OF_TEMPERATURE;
    static const std::string TYPE_OF_TEMPERATURE;
    static const std::string NAME_OF_WEEKDAY;
    static const std::string TYPE_OF_WEEKDAY;
};
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif