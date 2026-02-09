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
#ifndef OHOS_GLOBAL_I18N_VARIABLE_CONVERTOR_H
#define OHOS_GLOBAL_I18N_VARIABLE_CONVERTOR_H

#include <map>
#include <string>
#include <vector>
#include "i18n_types.h"
#include "locale_info.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"

namespace OHOS {
namespace Global {
namespace I18n {
class VariableConvertor {
public:
    // Check napi variable is null
    static bool CheckNapiIsNull(napi_env env, napi_value value);

    // Convert napi variable to c++ variable
    static void GetOptionValue(napi_env env, napi_value options, const std::string &optionName, std::string &value);
    static bool GetBoolOptionValue(napi_env env, napi_value &options, const std::string &optionName, bool &boolVal);
    static std::string GetString(napi_env env, napi_value value, int32_t &code);
    static double GetDouble(napi_env env, napi_value value, int32_t &code);
    static int32_t GetInt(napi_env env, napi_value value, int32_t &code);
    static bool GetStringArrayFromJsParam(napi_env env, napi_value &jsArray, const std::string& valueName,
        std::vector<std::string> &strArray);

    // Convert c++ variable to napi variable
    static napi_value CreateString(napi_env env, const std::string &str);
    static napi_value CreateNumber(napi_env env, const int32_t &num);
    static napi_value CreateBoolean(napi_env env, bool flag);
    static void VerifyType(napi_env env, const std::string& valueName, napi_valuetype napiType, napi_value argv);
    static napi_status SetEnumValue(napi_env env, napi_value enumObj, const std::string& enumName, int32_t enumVal);
    static LocaleType GetLocaleType(napi_env env, napi_value locale);
    static std::shared_ptr<LocaleInfo> ParseLocaleInfo(napi_env env, napi_value localeInfo);
    static std::string ParseBuiltinsLocale(napi_env env, napi_value intlLocale);
    static napi_value CreateBuiltinsLocaleObject(napi_env env, const std::string& locale);
    static napi_value CreateMap(napi_env env, std::unordered_map<std::string, std::string>& ump);
    static bool GetDateTime(napi_env env, napi_value date, double& milliseconds);
    static napi_value CreateStyledString(napi_env env, const std::string& content, napi_value styleOption);

private:
    static napi_value GetBuiltinsLocaleConstructor(napi_env env);
    static bool IsBuiltinsLocale(napi_env env, napi_value locale);

    static const std::map<napi_valuetype, std::string> NAPI_TYPE_MAP;
};
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif