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
#include <set>
#include <vector>

#include "napi/native_api.h"
#include "napi/native_node_api.h"

#include "i18n_hilog.h"

namespace OHOS {
namespace Global {
namespace I18n {
class JSUtils {
public:
    static napi_value DefaultConstructor(napi_env env, napi_callback_info info);
    static napi_value CreateEmptyString(napi_env env);
    static napi_value CreateEmptyArray(napi_env env);
    static napi_value CreateEmptyObject(napi_env env);

    static void GetNumberOptionValues(napi_env env, napi_value options, std::map<std::string, std::string> &map);
    static int64_t GetIntegerOptionValue(napi_env env, napi_value options, const std::string &optionName,
        std::map<std::string, std::string> &map);

    static void SetOptionProperties(napi_env env, napi_value &result,
        std::map<std::string, std::string> &options, const std::string &option);
    static void SetBooleanOptionProperties(napi_env env, napi_value &result,
        std::map<std::string, std::string> &options, const std::string &option);
    static void SetIntegerOptionProperties(napi_env env, napi_value &result,
        std::map<std::string, std::string> &options, const std::string &option);
    static void SetNamedStringProperties(napi_env env, napi_value &result, const std::string &key,
        const std::string &value);
    static void SetNamedIntegerProperties(napi_env env, napi_value &result, const std::string &key, int64_t value);
    static void SetNamedVectorProperties(napi_env env, napi_value &result, const std::string &key,
        const std::vector<std::string> &value);
    static std::string GetString(napi_env env, napi_value value, int32_t& code);
    static std::string GetBigIntStr(napi_env env, napi_value value, int32_t& code);
    static std::vector<std::string> GetStringArray(napi_env env, napi_value value, int32_t& code);
    static std::vector<std::string> GetLocaleArray(napi_env env, napi_value value, int32_t& code);
    static napi_value CreateObject(napi_env env, std::unordered_map<std::string, napi_value>& propertys,
        int32_t& code);
    static napi_value CreateArrayItem(napi_env env, std::unordered_map<std::string, napi_value>& propertys,
        int32_t& code, std::vector<std::string> &keySequence);
    static napi_value CreateArray(napi_env env, const std::vector<std::string>& value);
    static napi_value CreateString(napi_env env, const std::string &str);
    static bool GetPropertyFormObject(napi_env env, napi_value object, const std::string& property,
        napi_valuetype type, std::string& value);
    static napi_value GetNapiPropertyFormObject(napi_env env, napi_value object, const std::string& property,
        int32_t& errCode);
    static double GetNumberValue(napi_env env, napi_value param, int32_t &errorCode);
    static double GetDoubleFromNapiValue(napi_env env, napi_value param, napi_valuetype &valueType, int32_t &errorCode);
    static napi_value GetConstructor(napi_env env, const std::string& className);

    // std::map and std::unordered_map
    template<typename T>
    static void GetOptionValue(napi_env env, napi_value options, const std::string &optionName, T &map)
    {
        napi_value optionValue = nullptr;
        napi_valuetype type = napi_undefined;
        napi_status status = napi_typeof(env, options, &type);
        if (status != napi_ok || type != napi_object) {
            HILOG_ERROR_I18N("Get option failed, option is not an object");
            return;
        }
        bool hasProperty = false;
        status = napi_has_named_property(env, options, optionName.c_str(), &hasProperty);
        if (status != napi_ok) {
            HILOG_ERROR_I18N("GetOptionValue: Has not named property:%{public}s", optionName.c_str());
            return;
        }
        if (!hasProperty) {
            return;
        }
        status = napi_get_named_property(env, options, optionName.c_str(), &optionValue);
        if (status != napi_ok) {
            HILOG_ERROR_I18N("GetOptionValue: Get named property for %{public}s failed.", optionName.c_str());
            return;
        }
        size_t len = 0;
        status = napi_get_value_string_utf8(env, optionValue, nullptr, 0, &len);
        if (status != napi_ok) {
            HILOG_ERROR_I18N("GetOptionValue: get string length failed");
            return;
        }

        std::vector<char> optionBuf(len + 1);
        status = napi_get_value_string_utf8(env, optionValue, optionBuf.data(), len + 1, &len);
        if (status != napi_ok) {
            HILOG_ERROR_I18N("GetOptionValue: get string value failed");
            return;
        }
        auto ret = map.insert(make_pair(optionName, optionBuf.data()));
        if (!ret.second) {
            HILOG_ERROR_I18N("GetOptionValue: map insert failed");
        }
    }

    template<typename T>
    static void GetBoolOptionValue(napi_env env, napi_value options, const std::string &optionName, T &map)
    {
        napi_value optionValue = nullptr;
        napi_valuetype type = napi_undefined;
        napi_status status = napi_typeof(env, options, &type);
        if (status != napi_ok || type != napi_object) {
            HILOG_ERROR_I18N("GetBoolOptionValue: Set option failed, option is not an object");
            return;
        }
        bool hasProperty = false;
        status = napi_has_named_property(env, options, optionName.c_str(), &hasProperty);
        if (status != napi_ok) {
            HILOG_ERROR_I18N("GetBoolOptionValue: Has not named property.");
            return;
        }
        if (!hasProperty) {
            return;
        }
        status = napi_get_named_property(env, options, optionName.c_str(), &optionValue);
        if (status != napi_ok) {
            HILOG_ERROR_I18N("GetBoolOptionValue: Get named property for %{public}s failed.", optionName.c_str());
            return;
        }
        bool boolValue = false;
        status = napi_get_value_bool(env, optionValue, &boolValue);
        if (status != napi_ok) {
            HILOG_ERROR_I18N("GetBoolOptionValue: Get bool value failed.");
            return;
        }
        std::string value = boolValue ? "true" : "false";
        auto ret = map.insert(make_pair(optionName, value));
        if (!ret.second) {
            HILOG_ERROR_I18N("GetBoolOptionValue: map insert failed");
        }
    }

    template<typename T>
    static void GetDoubleOptionValue(napi_env env, napi_value options, const std::string &optionName, T &map)
    {
        napi_value optionValue = nullptr;
        napi_valuetype type = napi_undefined;
        napi_status status = napi_typeof(env, options, &type);
        if (status != napi_ok || type != napi_object) {
            HILOG_ERROR_I18N("GetDoubleOptionValue: Get option failed, option is not an object.");
            return;
        }
        bool hasProperty = false;
        status = napi_has_named_property(env, options, optionName.c_str(), &hasProperty);
        if (status != napi_ok || !hasProperty) {
            HILOG_INFO_I18N("GetDoubleOptionValue: No named %{public}s param.", optionName.c_str());
            return;
        }
        status = napi_get_named_property(env, options, optionName.c_str(), &optionValue);
        if (status != napi_ok) {
            HILOG_ERROR_I18N("GetDoubleOptionValue: get named %{public}s param failed.", optionName.c_str());
            return;
        }
        double option;
        status = napi_get_value_double(env, optionValue, &option);
        if (status != napi_ok) {
            HILOG_ERROR_I18N("GetDoubleOptionValue: param %{public}s get double value failed.", optionName.c_str());
            return;
        }
        try {
            map.insert(make_pair(optionName, std::to_string(option)));
        } catch (const std::bad_alloc& except) {
            HILOG_ERROR_I18N("GetDoubleOptionValue: double to string failed.");
            return;
        }
    }
};
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif