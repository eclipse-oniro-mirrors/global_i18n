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
#ifndef OHOS_GLOBAL_I18N_INTL_LOCALE_ADDON_H
#define OHOS_GLOBAL_I18N_INTL_LOCALE_ADDON_H

#include <unordered_map>
#include "intl_locale.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"

namespace OHOS {
namespace Global {
namespace I18n {
typedef std::string (*GetIntlLocaleProperty)(std::shared_ptr<IntlLocale>);

class IntlLocaleAddon {
public:
    IntlLocaleAddon();
    virtual ~IntlLocaleAddon();

    std::shared_ptr<IntlLocale> GetIntlLocale();
    static napi_value InitIntlLocale(napi_env env, napi_value exports);
    static bool IsIntlLocale(napi_env env, napi_value argv);
    static napi_value CreateIntlLocaleObject(napi_env env, napi_value argv);

private:
    static void Destructor(napi_env env, void *nativeObject, void *hint);
    static napi_value SetProperty(napi_env env, napi_callback_info info);
    static napi_value IntlLocaleConstructor(napi_env env, napi_callback_info info);
    static std::string LocaleToString(napi_env env, napi_value argv);
    bool InitLocaleContext(napi_env env, const std::string localeTag,
        std::unordered_map<std::string, std::string>& map);

    static napi_value GetLanguage(napi_env env, napi_callback_info info);
    static napi_value GetBaseName(napi_env env, napi_callback_info info);
    static napi_value GetRegion(napi_env env, napi_callback_info info);
    static napi_value GetScript(napi_env env, napi_callback_info info);
    static napi_value GetCalendar(napi_env env, napi_callback_info info);
    static napi_value GetCollation(napi_env env, napi_callback_info info);
    static napi_value GetHourCycle(napi_env env, napi_callback_info info);
    static napi_value GetNumberingSystem(napi_env env, napi_callback_info info);
    static napi_value GetNumeric(napi_env env, napi_callback_info info);
    static napi_value GetCaseFirst(napi_env env, napi_callback_info info);
    static napi_value ToString(napi_env env, napi_callback_info info);
    static napi_value Minimize(napi_env env, napi_callback_info info);
    static napi_value Maximize(napi_env env, napi_callback_info info);

    static std::string GetLanguageInner(std::shared_ptr<IntlLocale> intlLocale);
    static std::string GetBaseNameInner(std::shared_ptr<IntlLocale> intlLocale);
    static std::string GetRegionInner(std::shared_ptr<IntlLocale> intlLocale);
    static std::string GetScriptInner(std::shared_ptr<IntlLocale> intlLocale);
    static std::string GetCalendarInner(std::shared_ptr<IntlLocale> intlLocale);
    static std::string GetCollationInner(std::shared_ptr<IntlLocale> intlLocale);
    static std::string GetHourCycleInner(std::shared_ptr<IntlLocale> intlLocale);
    static std::string GetNumberingSystemInner(std::shared_ptr<IntlLocale> intlLocale);
    static std::string GetNumericInner(std::shared_ptr<IntlLocale> intlLocale);
    static std::string GetCaseFirstInner(std::shared_ptr<IntlLocale> intlLocale);
    static std::string MaximizeInner(std::shared_ptr<IntlLocale> intlLocale);
    static std::string MinimizeInner(std::shared_ptr<IntlLocale> intlLocale);
    static napi_value GetIntlLocaleAttribute(napi_env env, napi_callback_info info, GetIntlLocaleProperty func,
        napi_valuetype type);
    static std::string ParseLocaleTag(napi_env env, napi_value value, napi_valuetype type);
    static void ParseConfigs(napi_env env, napi_value options, std::unordered_map<std::string, std::string>& configs);

    static constexpr napi_type_tag TYPE_TAG = { 0xf8a4eb747b854c8f, 0xae58d824ccf59c05 };

    std::shared_ptr<IntlLocale> intlLocale = nullptr;
};
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif