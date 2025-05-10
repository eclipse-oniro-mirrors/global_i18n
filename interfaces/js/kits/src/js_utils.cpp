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
#include "js_utils.h"
#include "i18n_hilog.h"
#include "utils.h"

namespace OHOS {
namespace Global {
namespace I18n {
napi_value JSUtils::DefaultConstructor(napi_env env, napi_callback_info info)
{
    return nullptr;
}

napi_value JSUtils::CreateEmptyString(napi_env env)
{
    napi_value result;
    std::string emptyStr = "";
    napi_status status = napi_create_string_utf8(env, emptyStr.c_str(), NAPI_AUTO_LENGTH, &result);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("JSUtils: create string js variable failed.");
    }
    return result;
}

void JSUtils::GetOptionValue(napi_env env, napi_value options, const std::string &optionName,
    std::map<std::string, std::string> &map)
{
    napi_value optionValue = nullptr;
    napi_valuetype type = napi_undefined;
    napi_status status = napi_typeof(env, options, &type);
    if (status != napi_ok && type != napi_object) {
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

void JSUtils::GetBoolOptionValue(napi_env env, napi_value options, const std::string &optionName,
    std::map<std::string, std::string> &map)
{
    napi_value optionValue = nullptr;
    napi_valuetype type = napi_undefined;
    napi_status status = napi_typeof(env, options, &type);
    if (status != napi_ok && type != napi_object) {
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

void JSUtils::GetNumberOptionValues(napi_env env, napi_value options, std::map<std::string, std::string> &map)
{
    JSUtils::GetOptionValue(env, options, "currency", map);
    JSUtils::GetOptionValue(env, options, "currencySign", map);
    JSUtils::GetOptionValue(env, options, "currencyDisplay", map);
    JSUtils::GetOptionValue(env, options, "unit", map);
    JSUtils::GetOptionValue(env, options, "unitDisplay", map);
    JSUtils::GetOptionValue(env, options, "compactDisplay", map);
    JSUtils::GetOptionValue(env, options, "signDisplay", map);
    JSUtils::GetOptionValue(env, options, "localeMatcher", map);
    JSUtils::GetOptionValue(env, options, "style", map);
    JSUtils::GetOptionValue(env, options, "numberingSystem", map);
    JSUtils::GetOptionValue(env, options, "notation", map);
    JSUtils::GetOptionValue(env, options, "unitUsage", map);
    JSUtils::GetOptionValue(env, options, "roundingPriority", map);
    JSUtils::GetOptionValue(env, options, "roundingMode", map);
    JSUtils::GetBoolOptionValue(env, options, "useGrouping", map);
    GetIntegerOptionValue(env, options, "minimumIntegerDigits", map);
    int64_t minFd = GetIntegerOptionValue(env, options, "minimumFractionDigits", map);
    int64_t maxFd = GetIntegerOptionValue(env, options, "maximumFractionDigits", map);
    if (minFd != -1 && maxFd != -1 && minFd > maxFd) {
        HILOG_ERROR_I18N(
            "GetNumberOptionValues: Invalid parameter value: minimumFractionDigits > maximumFractionDigits");
    }
    GetIntegerOptionValue(env, options, "minimumSignificantDigits", map);
    GetIntegerOptionValue(env, options, "maximumSignificantDigits", map);
    GetIntegerOptionValue(env, options, "roundingIncrement", map);
}

int64_t JSUtils::GetIntegerOptionValue(napi_env env, napi_value options, const std::string &optionName,
    std::map<std::string, std::string> &map)
{
    napi_value optionValue = nullptr;
    int64_t integerValue = -1;
    napi_valuetype type = napi_undefined;
    napi_status status = napi_typeof(env, options, &type);
    if (status != napi_ok && type != napi_object) {
        HILOG_ERROR_I18N("GetIntegerOptionValue: Set option failed, option is not an object");
        return integerValue;
    }
    bool hasProperty = false;
    napi_status propStatus = napi_has_named_property(env, options, optionName.c_str(), &hasProperty);
    if (propStatus == napi_ok && hasProperty) {
        status = napi_get_named_property(env, options, optionName.c_str(), &optionValue);
        if (status == napi_ok) {
            status = napi_get_value_int64(env, optionValue, &integerValue);
            if (status == napi_ok) {
                map.insert(make_pair(optionName, std::to_string(integerValue)));
            }
        }
    }
    return integerValue;
}

void JSUtils::SetOptionProperties(napi_env env, napi_value &result, std::map<std::string, std::string> &options,
    const std::string &option)
{
    if (options.count(option) > 0) {
        std::string optionValue = options[option];
        napi_value optionJsValue = nullptr;
        napi_status status = napi_create_string_utf8(env, optionValue.c_str(), NAPI_AUTO_LENGTH, &optionJsValue);
        if (status != napi_ok) {
            return;
        }
        status = napi_set_named_property(env, result, option.c_str(), optionJsValue);
        if (status != napi_ok) {
            HILOG_ERROR_I18N("JSUtils::SetOptionProperties: Set property failed.");
            return;
        }
    } else {
        napi_value undefined = nullptr;
        napi_status status = napi_get_undefined(env, &undefined);
        if (status != napi_ok) {
            return;
        }
        status = napi_set_named_property(env, result, option.c_str(), undefined);
        if (status != napi_ok) {
            HILOG_ERROR_I18N("JSUtils::SetOptionProperties: Set property failed.");
            return;
        }
    }
}

void JSUtils::SetBooleanOptionProperties(napi_env env, napi_value &result, std::map<std::string, std::string> &options,
    const std::string &option)
{
    if (options.count(option) > 0) {
        std::string optionValue = options[option];
        bool optionBoolValue = (optionValue == "true");
        napi_value optionJsValue = nullptr;
        napi_status status = napi_get_boolean(env, optionBoolValue, &optionJsValue);
        if (status != napi_ok) {
            return;
        }
        status = napi_set_named_property(env, result, option.c_str(), optionJsValue);
        if (status != napi_ok) {
            HILOG_ERROR_I18N("JSUtils::SetBooleanOptionProperties: Set property failed.");
            return;
        }
    } else {
        napi_value undefined = nullptr;
        napi_status status = napi_get_undefined(env, &undefined);
        if (status != napi_ok) {
            return;
        }
        status = napi_set_named_property(env, result, option.c_str(), undefined);
        if (status != napi_ok) {
            HILOG_ERROR_I18N("JSUtils::SetBooleanOptionProperties: Set property failed.");
            return;
        }
    }
}

void JSUtils::SetIntegerOptionProperties(napi_env env, napi_value &result, std::map<std::string, std::string> &options,
    const std::string &option)
{
    napi_status status;
    if (options.count(option) > 0) {
        std::string optionValue = options[option];
        napi_value optionJsValue = nullptr;
        int32_t status = 0;
        int64_t integerValue = ConvertString2Int(optionValue, status);
        if (status != -1) {
            status = napi_create_int64(env, integerValue, &optionJsValue);
            if (status != napi_ok) {
                return;
            }
            status = napi_set_named_property(env, result, option.c_str(), optionJsValue);
            if (status != napi_ok) {
                HILOG_ERROR_I18N("JSUtils::SetIntegerOptionProperties: Set property failed.");
                return;
            }
            return;
        }
    }
    napi_value undefined = nullptr;
    status = napi_get_undefined(env, &undefined);
    if (status != napi_ok) {
        return;
    }
    status = napi_set_named_property(env, result, option.c_str(), undefined);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("JSUtils::SetIntegerOptionProperties: Set property failed.");
        return;
    }
}

std::string JSUtils::GetString(napi_env env, napi_value value, int32_t& code)
{
    size_t len = 0;
    napi_status status = napi_get_value_string_utf8(env, value, nullptr, 0, &len);
    if (status != napi_ok) {
        code = 1;
        HILOG_ERROR_I18N("JSUtils::GetString: Get string len failed.");
        return "";
    }
    std::vector<char> buffer(len + 1);
    status = napi_get_value_string_utf8(env, value, buffer.data(), len + 1, &len);
    if (status != napi_ok) {
        code = 1;
        HILOG_ERROR_I18N("JSUtils::GetString: Get string value failed.");
        return "";
    }
    std::string result(buffer.data());
    code = 0;
    return result;
}

std::vector<std::string> JSUtils::GetStringArray(napi_env env, napi_value value, int32_t& code)
{
    std::vector<std::string> result;
    uint32_t arrayLength = 0;
    napi_status status = napi_get_array_length(env, value, &arrayLength);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("JSUtils::GetStringArray: Get array length failed.");
        code = 1;
        return result;
    }
    result.resize(arrayLength);
    for (uint32_t i = 0; i < arrayLength; i++) {
        napi_value element = nullptr;
        status = napi_get_element(env, value, i, &element);
        if (status != napi_ok) {
            HILOG_ERROR_I18N("JSUtils::GetStringArray: Get element %{public}d failed.", i);
            code = 1;
            return result;
        }
        std::string locale = JSUtils::GetString(env, element, code);
        if (code != 0) {
            HILOG_ERROR_I18N("JSUtils::GetStringArray: Get string failed.");
            return result;
        }
        result[i] = locale;
    }
    code = 0;
    return result;
}

std::vector<std::string> JSUtils::GetLocaleArray(napi_env env, napi_value value, int32_t& code)
{
    std::vector<std::string> result;
    napi_valuetype valueType = napi_valuetype::napi_undefined;
    napi_status status = napi_typeof(env, value, &valueType);
    if (status != napi_ok) {
        code = 1;
        HILOG_ERROR_I18N("JSUtils::GetLocaleVector: Get type of value failed.");
        return result;
    }
    if (valueType == napi_valuetype::napi_string) {
        std::string locale = JSUtils::GetString(env, value, code);
        if (code != 0) {
            HILOG_ERROR_I18N("JSUtils::GetLocaleVector: Get string failed.");
            return result;
        }
        result.emplace_back(locale);
        code = 0;
        return result;
    }
    bool isArray = false;
    status = napi_is_array(env, value, &isArray);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("JSUtils::GetLocaleVector: Check value is array failed.");
        code = 1;
        return result;
    }
    if (!isArray) {
        HILOG_ERROR_I18N("JSUtils::GetLocaleVector: Type of value is invalid.");
        code = 1;
        return result;
    }
    return JSUtils::GetStringArray(env, value, code);
}

napi_value JSUtils::CreateObject(napi_env env, std::unordered_map<std::string, napi_value>& propertys, int32_t& code)
{
    napi_value object = nullptr;
    napi_status status = napi_create_object(env, &object);
    if (status != napi_ok) {
        code = 1;
        HILOG_ERROR_I18N("JSUtils::CreateObject: Create object failed.");
        return nullptr;
    }
    for (const auto& property : propertys) {
        std::string key = property.first;
        napi_value value = property.second;
        status = napi_set_named_property(env, object, key.c_str(), value);
        if (status != napi_ok) {
            code = 1;
            HILOG_ERROR_I18N("JSUtils::CreateObject: Set property failed, key %{public}s.", key.c_str());
            return nullptr;
        }
    }
    code = 0;
    return object;
}
} // namespace I18n
} // namespace Global
} // namespace OHOS