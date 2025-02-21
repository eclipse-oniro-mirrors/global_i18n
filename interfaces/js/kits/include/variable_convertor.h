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
#ifndef OHOS_GLOBAL_I18N_VARIABLE_CONVERTOR_H
#define OHOS_GLOBAL_I18N_VARIABLE_CONVERTOR_H

#include <string>
#include <vector>
#include "napi/native_api.h"
#include "napi/native_node_api.h"

namespace OHOS {
namespace Global {
namespace I18n {
class VariableConvertor {
public:
    // Check napi variable is null
    static bool CheckNapiValueType(napi_env env, napi_value value);

    // Convert napi variable to c++ variable
    static void GetOptionValue(napi_env env, napi_value options, const std::string &optionName, std::string &value);
    static bool GetBoolOptionValue(napi_env env, napi_value &options, const std::string &optionName, bool &boolVal);
    static std::string GetString(napi_env &env, napi_value &value, int32_t &code);
    static bool GetStringArrayFromJsParam(napi_env env, napi_value &jsArray, const std::string& valueName,
        std::vector<std::string> &strArray);

    // Convert c++ variable to napi variable
    static napi_value CreateString(napi_env env, const std::string &str);
    static napi_value CreateNumber(napi_env env, const int32_t &num);
    static void VerifyType(napi_env env, const std::string& valueName, const std::string& type, napi_value argv);
};
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif