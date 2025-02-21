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
#ifndef OHOS_GLOBAL_I18N_SYSTEM_LOCAEL_MANAGER_H
#define OHOS_GLOBAL_I18N_SYSTEM_LOCAEL_MANAGER_H

#include "system_locale_manager.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"

namespace OHOS {
namespace Global {
namespace I18n {
class SystemLocaleManagerAddon {
public:
    SystemLocaleManagerAddon();
    ~SystemLocaleManagerAddon();
    static void Destructor(napi_env env, void *nativeObject, void *hint);
    static napi_value InitSystemLocaleManager(napi_env env, napi_value exports);

private:
    static napi_value SystemLocaleManagerConstructor(napi_env env, napi_callback_info info);
    bool InitSystemLocaleManagerContext(napi_env env, napi_callback_info info);
    
    // process js function call
    static napi_value GetLanguageInfoArray(napi_env env, napi_callback_info info);
    static napi_value GetCountryInfoArray(napi_env env, napi_callback_info info);
    static napi_value GetTimeZoneCityInfoArray(napi_env env, napi_callback_info info);

    // convert napi variable to c++ variable
    static void GetSortOptionsFromJsParam(napi_env env, napi_value &jsOptions, SortOptions &options, bool isRegion);

    // convert c++ variable to napi variable
    static napi_value CreateTimeZoneCityItem(napi_env env, const TimeZoneCityItem &timezoneCityItem);
    static napi_value CreateLocaleItemArray(napi_env env, const std::vector<LocaleItem> &localeItemList);
    static napi_value CreateLocaleItem(napi_env env, const LocaleItem &localeItem);
    static napi_value CreateSuggestionType(napi_env env, SuggestionType suggestionType);

    napi_env env_;
    std::unique_ptr<SystemLocaleManager> systemLocaleManager_ = nullptr;
};
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif