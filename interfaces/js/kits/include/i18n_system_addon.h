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
#ifndef OHOS_GLOBAL_I18N_SYSTEM_ADDON_H
#define OHOS_GLOBAL_I18N_SYSTEM_ADDON_H

#include <memory>
#include "i18n_timezone.h"
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

    static napi_value GetDisplayCountryImpl(napi_env env, napi_callback_info info, bool throwError);
    static napi_value GetDisplayLanguageImpl(napi_env env, napi_callback_info info, bool throwError);
    static napi_value GetSystemCountriesImpl(napi_env env, napi_callback_info info, bool throwError);
    static napi_value IsSuggestedImpl(napi_env env, napi_callback_info info, bool throwError);
    static napi_value SetSystemLanguageImpl(napi_env env, napi_callback_info info, bool throwError);
    static napi_value SetSystemRegionImpl(napi_env env, napi_callback_info info, bool throwError);
    static napi_value SetSystemLocaleImpl(napi_env env, napi_callback_info info, bool throwError);
    static napi_value Set24HourClockImpl(napi_env env, napi_callback_info info, bool throwError);
    static napi_value AddPreferredLanguageImpl(napi_env env, napi_callback_info info, bool throwError);
    static napi_value RemovePreferredLanguageImpl(napi_env env, napi_callback_info info, bool throwError);
    static napi_value SetUsingLocalDigitAddonImpl(napi_env env, napi_callback_info info, bool throwError);
    static bool GetNapiStringValueWithError(napi_env env, napi_value napiValue, size_t len, char* valueBuf,
        std::string index);
    static bool GetCountryNapiValueWithError(napi_env env, napi_value napiValue, size_t len, char* valueBuf,
        std::string index);

    static bool ParseStringParam(napi_env env, napi_value argv, bool throwError, std::string &strParam);
};
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif