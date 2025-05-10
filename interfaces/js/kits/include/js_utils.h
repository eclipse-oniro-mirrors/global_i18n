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
#ifndef JS_UTILS_H
#define JS_UTILS_H

#include <map>
#include "napi/native_api.h"
#include "napi/native_node_api.h"

namespace OHOS {
namespace Global {
namespace I18n {
class JSUtils {
public:
    static napi_value DefaultConstructor(napi_env env, napi_callback_info info);
    static napi_value CreateEmptyString(napi_env env);
    static void GetOptionValue(napi_env env, napi_value options, const std::string &optionName,
        std::map<std::string, std::string> &map);
    static void GetBoolOptionValue(napi_env env, napi_value options, const std::string &optionName,
        std::map<std::string, std::string> &map);
    static std::string GetLocaleTag(napi_env env, napi_value argv);
    static void GetLocaleTags(napi_env env, napi_value rawLocaleTag, std::vector<std::string> &localeTags);

    static void GetNumberOptionValues(napi_env env, napi_value options, std::map<std::string, std::string> &map);
    static int64_t GetIntegerOptionValue(napi_env env, napi_value options, const std::string &optionName,
        std::map<std::string, std::string> &map);

    static void SetOptionProperties(napi_env env, napi_value &result,
        std::map<std::string, std::string> &options, const std::string &option);
    static void SetBooleanOptionProperties(napi_env env, napi_value &result,
        std::map<std::string, std::string> &options, const std::string &option);
    static void SetIntegerOptionProperties(napi_env env, napi_value &result,
        std::map<std::string, std::string> &options, const std::string &option);
};
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif