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
        HILOG_ERROR_I18N("JSUtils: create empty string failed.");
    }
    return result;
}

napi_value JSUtils::CreateEmptyArray(napi_env env)
{
    napi_value result = nullptr;
    napi_status status = napi_create_array(env, &result);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("JSUtils::CreateEmptyArray: Failed to create empty array");
    }
    return result;
}

napi_value JSUtils::CreateEmptyObject(napi_env env)
{
    napi_value object = nullptr;
    napi_status status = napi_create_object(env, &object);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("JSUtils::CreateEmptyObject: Create empty object failed.");
    }
    return object;
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

void JSUtils::SetNamedStringProperties(napi_env env, napi_value &result, const std::string &key,
    const std::string &value)
{
    napi_value jsValue = nullptr;
    napi_status status = napi_create_string_utf8(env, value.c_str(), NAPI_AUTO_LENGTH, &jsValue);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("JSUtils::SetNamedStringProperties: string to js failed.");
        return;
    }
    status = napi_set_named_property(env, result, key.c_str(), jsValue);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("JSUtils::SetNamedStringProperties: Set property failed.");
        return;
    }
    return;
}

void JSUtils::SetNamedIntegerProperties(napi_env env, napi_value &result, const std::string &key, int64_t value)
{
    napi_value jsValue = nullptr;
    napi_status status = napi_create_int64(env, value, &jsValue);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("JSUtils::SetNamedIntegerProperties: int64_t to js failed.");
        return;
    }
    status = napi_set_named_property(env, result, key.c_str(), jsValue);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("JSUtils::SetNamedIntegerProperties: Set property failed.");
        return;
    }
    return;
}

void JSUtils::SetNamedVectorProperties(napi_env env, napi_value &result, const std::string &key,
    const std::vector<std::string> &value)
{
    napi_value jsValue = CreateArray(env, value);
    if (!jsValue) {
        HILOG_ERROR_I18N("JSUtils::SetNamedVectorProperties: craete array failed.");
        return;
    }
    napi_status status = napi_set_named_property(env, result, key.c_str(), jsValue);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("JSUtils::SetNamedVectorProperties: Set property failed.");
        return;
    }
    return;
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

std::string JSUtils::GetBigIntStr(napi_env env, napi_value value, int32_t& code)
{
    napi_value global;
    napi_status status = napi_get_global(env, &global);
    if (status != napi_ok) {
        code = -1;
        HILOG_ERROR_I18N("JSUtils::GetBigIntStr: Get global failed.");
        return "";
    }
    napi_value stringClass;
    status = napi_get_named_property(env, global, "String", &stringClass);
    if (status != napi_ok) {
        code = -1;
        HILOG_ERROR_I18N("JSUtils::GetBigIntStr: Get bigint value failed.");
        return "";
    }
    napi_value argv[1] = { value };
    napi_value toStringResult;
    status = napi_call_function(env, global, stringClass, 1, argv, &toStringResult);
    if (status != napi_ok) {
        code = -1;
        HILOG_ERROR_I18N("JSUtils::GetBigIntStr: Get bigint value failed.");
        return "";
    }
    return GetString(env, toStringResult, code);
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
    if (valueType == napi_valuetype::napi_undefined) {
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

napi_value JSUtils::CreateArrayItem(napi_env env, std::unordered_map<std::string, napi_value>& propertys,
    int32_t& code, std::vector<std::string> &keySequence)
{
    napi_value object = nullptr;
    napi_status status = napi_create_object(env, &object);
    if (status != napi_ok) {
        code = 1;
        HILOG_ERROR_I18N("JSUtils::CreateArrayItem: Create object failed.");
        return nullptr;
    }
    for (auto iter = keySequence.begin(); iter != keySequence.end(); ++iter) {
        std::string key = *iter;
        if (propertys.find(key) == propertys.end()) {
            continue;
        }
        napi_value value = propertys[key];
        if (value == nullptr) {
            continue;
        }
        status = napi_set_named_property(env, object, key.c_str(), value);
        if (status != napi_ok) {
            code = 1;
            HILOG_ERROR_I18N("JSUtils::CreateArrayItem: Set property failed, key %{public}s.", key.c_str());
            return nullptr;
        }
    }
    code = 0;
    return object;
}

napi_value JSUtils::CreateArray(napi_env env, const std::vector<std::string>& value)
{
    napi_value result = nullptr;
    napi_status status = napi_create_array_with_length(env, value.size(), &result);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("JSUtils::CreateArray: Failed to create array");
        return nullptr;
    }
    size_t pos = 0;
    for (const auto& language : value) {
        napi_value value = nullptr;
        status = napi_create_string_utf8(env, language.c_str(), NAPI_AUTO_LENGTH, &value);
        if (status != napi_ok) {
            HILOG_ERROR_I18N("JSUtils::CreateArray: Failed to create string item");
            return nullptr;
        }
        status = napi_set_element(env, result, pos, value);
        if (status != napi_ok) {
            HILOG_ERROR_I18N("JSUtils::CreateArray: Failed to set array item");
            return nullptr;
        }
        pos++;
    }
    return result;
}

napi_value JSUtils::CreateString(napi_env env, const std::string &str)
{
    napi_value result;
    napi_status status = napi_create_string_utf8(env, str.c_str(), NAPI_AUTO_LENGTH, &result);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("JSUtils::CreateString : create string js variable failed.");
        return nullptr;
    }
    return result;
}

bool JSUtils::GetPropertyFormObject(napi_env env, napi_value object, const std::string& property,
    napi_valuetype type, std::string& value)
{
    int32_t errCode = 0;
    napi_value propertyValue = JSUtils::GetNapiPropertyFormObject(env, object, property, errCode);
    if (errCode != 0) {
        return false;
    }
    napi_status status = napi_ok;
    switch (type) {
        case napi_valuetype::napi_boolean: {
            bool boolValue = false;
            status = napi_get_value_bool(env, propertyValue, &boolValue);
            if (status != napi_ok) {
                HILOG_ERROR_I18N("JSUtils::GetPropertyFormObject: Get bool failed, property is %{public}s.",
                    property.c_str());
                return false;
            }
            value = boolValue ? "true" : "false";
            break;
        }
        case napi_valuetype::napi_string: {
            int32_t code = 0;
            value = JSUtils::GetString(env, propertyValue, code);
            if (code != 0) {
                HILOG_ERROR_I18N("JSUtils::GetPropertyFormObject: Get string failed, property is %{public}s.",
                    property.c_str());
                return false;
            }
            break;
        }
        case napi_valuetype::napi_number: {
            int32_t intValue = 0;
            status = napi_get_value_int32(env, propertyValue, &intValue);
            if (status != napi_ok) {
                HILOG_ERROR_I18N("JSUtils::GetPropertyFormObject: Get int failed, property is %{public}s.",
                    property.c_str());
                return false;
            }
            value = std::to_string(intValue);
            break;
        }
        default:
            return false;
    }
    return true;
}

napi_value JSUtils::GetNapiPropertyFormObject(napi_env env, napi_value object, const std::string& property,
    int32_t& errCode)
{
    errCode = -1;
    bool hasProperty = false;
    napi_status status = napi_has_named_property(env, object, property.c_str(), &hasProperty);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("JSUtils::GetNapiPropertyFormObject: Call napi_has_named_property failed, "
            "property is %{public}s.", property.c_str());
        return nullptr;
    }
    if (!hasProperty) {
        return nullptr;
    }
    napi_value propertyValue = nullptr;
    status = napi_get_named_property(env, object, property.c_str(), &propertyValue);
    if (status != napi_ok || propertyValue == nullptr) {
        HILOG_ERROR_I18N("JSUtils::GetPropertyFormObject: Call napi_get_named_property failed, "
            "property is %{public}s.", property.c_str());
        return nullptr;
    }
    errCode = 0;
    return propertyValue;
}

double JSUtils::GetNumberValue(napi_env env, napi_value param, int32_t &errorCode)
{
    napi_value global = nullptr;
    napi_status status = napi_get_global(env, &global);
    if (status != napi_ok || global == nullptr) {
        HILOG_ERROR_I18N("JSUtils::GetNumberValue: get global object failed");
        errorCode = -1;
        return 0;
    }
    napi_value numberConstructor;
    status = napi_get_named_property(env, global, "Number", &numberConstructor);
    if (status != napi_ok || numberConstructor == nullptr) {
        HILOG_ERROR_I18N("JSUtils::GetNumberValue: Number constructor failed");
        errorCode = -1;
        return 0;
    }
    size_t argc = 1;
    napi_value instance = nullptr;
    napi_value argv[1] = { param };
    status = napi_new_instance(env, numberConstructor, argc, argv, &instance);
    if (status != napi_ok || instance == nullptr) {
        HILOG_ERROR_I18N("JSUtils::GetNumberValue: Create Number object failed");
        errorCode = -1;
        return 0;
    }
    napi_value valueOfFunc = nullptr;
    status = napi_get_named_property(env, instance, "valueOf", &valueOfFunc);
    if (status != napi_ok || valueOfFunc == nullptr) {
        HILOG_ERROR_I18N("JSUtils::GetNumberValue: Get valueOf function failed");
        errorCode = -1;
        return 0;
    }
    napi_value retValue = nullptr;
    status = napi_call_function(env, instance, valueOfFunc, 0, nullptr, &retValue);
    if (status != napi_ok || retValue == nullptr) {
        HILOG_ERROR_I18N("JSUtils::GetNumberValue: Call valueOf() function failed");
        errorCode = -1;
        return 0;
    }
    double number = 0;
    status = napi_get_value_double(env, retValue, &number);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("JSUtils::GetNumberValue: Get valueOf() double value failed");
        errorCode = -1;
        return 0;
    }
    return number;
}

double JSUtils::GetDoubleFromNapiValue(napi_env env, napi_value param, napi_valuetype &valueType,
    int32_t &errorCode)
{
    if (valueType != napi_valuetype::napi_number) {
        return GetNumberValue(env, param, errorCode);
    }
    double number;
    napi_status status = napi_get_value_double(env, param, &number);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("JSUtils::GetDoubleFromNapiValue: Get double value failed");
        errorCode = -1;
        return 0;
    }
    return number;
}

napi_value JSUtils::GetConstructor(napi_env env, const std::string& className)
{
    napi_value global = nullptr;
    napi_status status = napi_get_global(env, &global);
    if (status != napi_ok || global == nullptr) {
        HILOG_ERROR_I18N("JSUtils::GetConstructor: get global obj failed");
        return nullptr;
    }
    napi_value intlModule;
    status = napi_get_named_property(env, global, "Intl", &intlModule);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("JSUtils::GetConstructor Get Intl module failed");
        return nullptr;
    }
    napi_value constructor = nullptr;
    status = napi_get_named_property(env, intlModule, className.c_str(), &constructor);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("JSUtils::GetConstructor: get constructor failed");
        return nullptr;
    }
    return constructor;
}
} // namespace I18n
} // namespace Global
} // namespace OHOS