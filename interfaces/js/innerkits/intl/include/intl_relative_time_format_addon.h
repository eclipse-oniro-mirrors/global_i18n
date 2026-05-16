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

#ifndef OHOS_GLOBAL_I18N_INTL_RELATIVE_TIME_FORMAT_ADDON_H
#define OHOS_GLOBAL_I18N_INTL_RELATIVE_TIME_FORMAT_ADDON_H

#include <memory>

#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include <unordered_map>
#include <vector>

#include "relative_time_format.h"

namespace OHOS {
namespace Global {
namespace I18n {
class IntlRelativeTimeFormatAddon {
public:
    IntlRelativeTimeFormatAddon();
    ~IntlRelativeTimeFormatAddon();
    static void Destructor(napi_env env, void *nativeObject, void *hint);
    static napi_value InitIntlRelativeTimeFormat(napi_env env, napi_value exports);

private:
    static napi_value RelativeTimeFormatConstructor(napi_env env, napi_callback_info info);
    static napi_value Format(napi_env env, napi_callback_info info);
    static napi_value GetNumberResolvedOptions(napi_env env, napi_callback_info info);
    static napi_value FormatToParts(napi_env env, napi_callback_info info);
    static napi_value ToString(napi_env env, napi_callback_info info);
    static napi_value SupportedLocalesOf(napi_env env, napi_callback_info info);
    static napi_value GetFormatToPartsInner(napi_env env, double number, const std::string &unit,
        napi_value &thisVar);
    static napi_value SetNumberFormatParts(napi_env env,
        const std::vector<std::vector<std::string>> &numberParts);
    static void GetRelativeTimeOptionValues(napi_env env, napi_value options, std::map<std::string, std::string> &map);
    static bool ThrowRangeError(napi_env env);
    static void CheckOptionValue(napi_env env, std::map<std::string, std::string> &map, const std::string &optionName);
    static void CheckOptionValue(napi_env env, const std::string &optionValue, const std::string &optionName);

    bool GetParameterLocales(napi_env env, std::vector<std::string> &localeTags,
        const size_t &argc, napi_value* argv);
    bool InitRelativeTimeFormatContext(napi_env env, napi_callback_info info,
        std::vector<std::string> localeTags, std::map<std::string, std::string> &map);

    static std::unordered_map<std::string, std::vector<std::string>> optionAvaliableMap;
    static constexpr napi_type_tag TYPE_TAG = { 0xa4e471ac504a493d, 0x9ddd318ef76b1707 };

    std::shared_ptr<RelativeTimeFormat> relativetimefmt_ = nullptr;
};
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif