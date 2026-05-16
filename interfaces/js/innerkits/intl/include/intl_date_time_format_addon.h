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
#ifndef OHOS_GLOBAL_I18N_INTL_DATE_TIME_FORMAT_ADDON_H
#define OHOS_GLOBAL_I18N_INTL_DATE_TIME_FORMAT_ADDON_H

#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "intl_date_time_format.h"

namespace OHOS {
namespace Global {
namespace I18n {

class IntlDateTimeFormatAddon {
public:
    IntlDateTimeFormatAddon();
    virtual ~IntlDateTimeFormatAddon();
    std::shared_ptr<IntlDateTimeFormat> GetDateTimeFormat();
    static napi_value InitIntlDateTimeFormat(napi_env env, napi_value exports);
    static bool IsIntlDateTimeFormat(napi_env env, napi_value argv);
    static napi_status UnwrapIntlDateTimeFormat(napi_env env, napi_value value,
        IntlDateTimeFormatAddon** dateTimeFormat);

private:
    static void Destructor(napi_env env, void* nativeObject, void* hint);
    static napi_value IntlDateTimeFormatConstructor(napi_env env, napi_callback_info info);
    static void ParseConfigs(napi_env env, napi_value options, std::unordered_map<std::string, std::string>& configs);
    bool InitIntlDateTimeFormatContext(napi_env env, const std::vector<std::string>& localeTags,
        std::unordered_map<std::string, std::string>& configs);
    static napi_value CreateDateTimeFormatWithoutNew(napi_env env, size_t argc, napi_value *argv);

    static napi_value Format(napi_env env, napi_callback_info info);
    static napi_value FormatToParts(napi_env env, napi_callback_info info);
    static napi_value FormatRange(napi_env env, napi_callback_info info);
    static napi_value FormatRangeToParts(napi_env env, napi_callback_info info);
    static napi_value ToString(napi_env env, napi_callback_info info);
    static napi_value ResolvedOptions(napi_env env, napi_callback_info info);
    static napi_value SupportedLocalesOf(napi_env env, napi_callback_info info);

    static bool GetDateTime(napi_env env, napi_value time, double& milliseconds);
    static napi_value CreateFormatPart(napi_env env,
        const std::vector<std::pair<std::string, std::string>>& formatResult);
    static napi_value CreateResolvedOptions(napi_env env, const std::unordered_map<std::string, std::string>& configs);

    static constexpr napi_type_tag TYPE_TAG = { 0xd4d19fabc19f4cc0, 0x8294da532c5c4104 };

    std::shared_ptr<IntlDateTimeFormat> intlDateTimeFormat = nullptr;
};
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif