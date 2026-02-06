/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#ifndef OHOS_GLOBAL_I18N_LOCALE_INFO_ADDON_H
#define OHOS_GLOBAL_I18N_LOCALE_INFO_ADDON_H

#include <vector>

#include "napi/native_api.h"
#include "napi/native_node_api.h"

#include "locale_info.h"

namespace OHOS {
namespace Global {
namespace I18n {
typedef std::string (*GetLocaleInfoProperty)(std::shared_ptr<LocaleInfo>);

class LocaleInfoAddon {
public:
    LocaleInfoAddon();
    ~LocaleInfoAddon();
    static void Destructor(napi_env env, void *nativeObject, void *hint);
    static napi_value InitLocale(napi_env env, napi_value exports);
    std::shared_ptr<LocaleInfo> GetLocaleInfo();
    static bool IsLocaleInfo(napi_env env, napi_value argv);

private:
    static napi_value LocaleConstructor(napi_env env, napi_callback_info info);
    bool InitLocaleContext(napi_env env, napi_callback_info info, const std::string localeTag,
        std::map<std::string, std::string> &map);
    static napi_value GetLanguage(napi_env env, napi_callback_info info);
    static napi_value GetScript(napi_env env, napi_callback_info info);
    static napi_value GetRegion(napi_env env, napi_callback_info info);
    static napi_value GetBaseName(napi_env env, napi_callback_info info);
    static napi_value GetCalendar(napi_env env, napi_callback_info info);
    static napi_value GetCollation(napi_env env, napi_callback_info info);
    static napi_value GetHourCycle(napi_env env, napi_callback_info info);
    static napi_value GetNumberingSystem(napi_env env, napi_callback_info info);
    static napi_value GetNumeric(napi_env env, napi_callback_info info);
    static napi_value GetCaseFirst(napi_env env, napi_callback_info info);
    static napi_value ToString(napi_env env, napi_callback_info info);
    static napi_value Maximize(napi_env env, napi_callback_info info);
    static napi_value Minimize(napi_env env, napi_callback_info info);
    static std::string GetLanguageInner(std::shared_ptr<LocaleInfo> localeInfo);
    static std::string GetScriptInner(std::shared_ptr<LocaleInfo> localeInfo);
    static std::string GetRegionInner(std::shared_ptr<LocaleInfo> localeInfo);
    static std::string GetBaseNameInner(std::shared_ptr<LocaleInfo> localeInfo);
    static std::string GetCalendarInner(std::shared_ptr<LocaleInfo> localeInfo);
    static std::string GetCollationInner(std::shared_ptr<LocaleInfo> localeInfo);
    static std::string GetHourCycleInner(std::shared_ptr<LocaleInfo> localeInfo);
    static std::string GetNumberingSystemInner(std::shared_ptr<LocaleInfo> localeInfo);
    static std::string GetCaseFirstInner(std::shared_ptr<LocaleInfo> localeInfo);
    static std::string ToStringInner(std::shared_ptr<LocaleInfo> localeInfo);
    static std::string MaximizeInner(std::shared_ptr<LocaleInfo> localeInfo);
    static std::string MinimizeInner(std::shared_ptr<LocaleInfo> localeInfo);
    static napi_value GetLocaleInfoAttribute(napi_env env, napi_callback_info info, GetLocaleInfoProperty func);
    static napi_value GetMaximizeOrMinimize(napi_env env, napi_callback_info info, GetLocaleInfoProperty func);
    
    static napi_value SetProperty(napi_env env, napi_callback_info info);

    napi_env env_;
    std::shared_ptr<LocaleInfo> locale_ = nullptr;
};
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif