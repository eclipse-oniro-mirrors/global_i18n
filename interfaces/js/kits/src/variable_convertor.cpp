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

#include "error_util.h"
#include "i18n_hilog.h"
#include "intl_date_time_format_addon.h"
#include "intl_number_format_addon.h"
#include "locale_info_addon.h"
#include "number_format_addon.h"
#include "simple_date_time_format_addon.h"
#include "simple_number_format_addon.h"
#include "symbol_date_time_format_addon.h"
#include "symbol_number_format_addon.h"
#include "variable_convertor.h"

namespace OHOS {
namespace Global {
namespace I18n {
const std::map<napi_valuetype, std::string> VariableConvertor::NAPI_TYPE_MAP {
    {napi_valuetype::napi_boolean, "boolean"},
    {napi_valuetype::napi_number, "number"},
    {napi_valuetype::napi_string, "string"},
    {napi_valuetype::napi_bigint, "bigint"},
    {napi_valuetype::napi_symbol, "symbol"},
    {napi_valuetype::napi_object, "object"},
    {napi_valuetype::napi_null, "null"},
    {napi_valuetype::napi_undefined, "undefined"},
};

const std::string GET_TIME_NAME = "getTime";

bool VariableConvertor::CheckNapiIsNull(napi_env env, napi_value value)
{
    if (value != nullptr) {
        napi_valuetype valueType = napi_valuetype::napi_undefined;
        napi_status status = napi_typeof(env, value, &valueType);
        if (status != napi_ok) {
            return false;
        }
        if (valueType != napi_valuetype::napi_undefined && valueType != napi_valuetype::napi_null) {
            return true;
        }
    }
    return false;
}

void VariableConvertor::GetOptionValue(napi_env env, napi_value options, const std::string &optionName,
    std::string &value)
{
    napi_value optionValue = nullptr;
    napi_valuetype type = napi_undefined;
    napi_status status = napi_typeof(env, options, &type);
    if (status != napi_ok && type != napi_object) {
        HILOG_ERROR_I18N("GetOptionValue: Get option failed, option is not an object");
        return;
    }
    bool hasProperty = false;
    napi_status propStatus = napi_has_named_property(env, options, optionName.c_str(), &hasProperty);
    if (propStatus == napi_ok && hasProperty) {
        status = napi_get_named_property(env, options, optionName.c_str(), &optionValue);
        if (status == napi_ok) {
            size_t len;
            status = napi_get_value_string_utf8(env, optionValue, nullptr, 0, &len);
            if (status != napi_ok) {
                return;
            }
            std::vector<char> optionBuf(len + 1);
            status = napi_get_value_string_utf8(env, optionValue, optionBuf.data(), len + 1, &len);
            if (status != napi_ok) {
                HILOG_ERROR_I18N("GetOptionValue: Failed to get string item");
                return;
            }
            value = optionBuf.data();
        }
    }
}

bool VariableConvertor::GetBoolOptionValue(napi_env env, napi_value &options, const std::string &optionName,
    bool &boolVal)
{
    napi_valuetype type = napi_undefined;
    napi_status status = napi_typeof(env, options, &type);
    if (status != napi_ok && type != napi_object) {
        HILOG_ERROR_I18N("option is not an object");
        ErrorUtil::NapiThrow(env, I18N_NOT_VALID, optionName, "a valid object", true);
        return false;
    }
    bool hasProperty = false;
    status = napi_has_named_property(env, options, optionName.c_str(), &hasProperty);
    if (status != napi_ok || !hasProperty) {
        HILOG_INFO_I18N("option don't have property %{public}s", optionName.c_str());
        return false;
    }
    napi_value optionValue = nullptr;
    status = napi_get_named_property(env, options, optionName.c_str(), &optionValue);
    if (status != napi_ok) {
        HILOG_INFO_I18N("get option %{public}s failed", optionName.c_str());
        return false;
    }
    status = napi_get_value_bool(env, optionValue, &boolVal);
    if (status != napi_ok) {
        return false;
    }
    return true;
}

double VariableConvertor::GetDouble(napi_env env, napi_value value, int32_t &code)
{
    napi_valuetype valueType = napi_valuetype::napi_undefined;
    napi_status status = napi_typeof(env, value, &valueType);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("VariableConvertor::GetDouble Get napi_value type failed");
        code = 1;
        return 0;
    }
    if (valueType != napi_valuetype::napi_number) {
        HILOG_ERROR_I18N("VariableConvertor::GetDouble check napi_type_number failed");
        code = 1;
        return 0;
    }
    double result = 0;
    status = napi_get_value_double(env, value, &result);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("VariableConvertor::GetDouble Get double failed");
        code = 1;
        return 0;
    }
    return result;
}

int32_t VariableConvertor::GetInt32(napi_env env, napi_value value, int32_t& code)
{
    napi_valuetype valueType = napi_valuetype::napi_undefined;
    napi_status status = napi_typeof(env, value, &valueType);
    if (status != napi_ok) {
        code = I18N_NAPI_ERROR;
        HILOG_ERROR_I18N("GetInt32: Get type failed.");
        return 0;
    }
    if (valueType != napi_valuetype::napi_number) {
        code = I18N_NOT_FOUND;
        HILOG_ERROR_I18N("GetInt32: Check type failed.");
        return 0;
    }
    int32_t result = 0;
    status = napi_get_value_int32(env, value, &result);
    if (status != napi_ok) {
        code = I18N_NAPI_ERROR;
        HILOG_ERROR_I18N("GetInt32: Get confidence failed");
        return 0;
    }
    return result;
}

std::string VariableConvertor::GetString(napi_env env, napi_value value, int32_t &code)
{
    size_t len = 0;
    napi_status status = napi_get_value_string_utf8(env, value, nullptr, 0, &len);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("Get string failed");
        code = 1;
        return "";
    }
    std::vector<char> buf(len + 1);
    status = napi_get_value_string_utf8(env, value, buf.data(), len + 1, &len);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("Create string failed");
        code = 1;
        return "";
    }
    std::string result(buf.data());
    return result;
}

bool VariableConvertor::GetStringArrayFromJsParam(napi_env env, napi_value &jsArray, const std::string& valueName,
    std::vector<std::string> &strArray)
{
    if (jsArray == nullptr) {
        ErrorUtil::NapiNotFoundError(env, I18N_NOT_FOUND, valueName, true);
        return false;
    }
    bool isArray = false;
    napi_status status = napi_is_array(env, jsArray, &isArray);
    if (status != napi_ok) {
        return false;
    } else if (!isArray) {
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, valueName, "an Array", true);
        return false;
    }
    uint32_t arrayLength = 0;
    status = napi_get_array_length(env, jsArray, &arrayLength);
    if (status != napi_ok) {
        return false;
    }
    napi_value element = nullptr;
    int32_t code = 0;
    for (uint32_t i = 0; i < arrayLength; ++i) {
        status = napi_get_element(env, jsArray, i, &element);
        if (status != napi_ok) {
            return false;
        }
        std::string str = GetString(env, element, code);
        if (code != 0) {
            HILOG_ERROR_I18N("GetStringArrayFromJsParam: Failed to obtain the parameter.");
            return false;
        }
        strArray.push_back(str);
    }
    return true;
}

napi_value VariableConvertor::CreateString(napi_env env, const std::string &str)
{
    napi_value result;
    napi_status status = napi_create_string_utf8(env, str.c_str(), NAPI_AUTO_LENGTH, &result);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("create string js variable failed.");
        return nullptr;
    }
    return result;
}

napi_value VariableConvertor::CreateNumber(napi_env env, const int32_t &num)
{
    napi_value result;
    napi_status status = napi_create_int32(env, num, &result);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("create number js variable failed.");
        return nullptr;
    }
    return result;
}

napi_value VariableConvertor::CreateInt64Value(napi_env env, int64_t value)
{
    napi_value result = nullptr;
    napi_status status = napi_create_int64(env, value, &result);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("VariableConvertor::CreateInt64Value: napi_create_int64 failed, status: %{public}d", status);
        return nullptr;
    }
    return result;
}

napi_value VariableConvertor::CreateDoubleValue(napi_env env, double value)
{
    napi_value result = nullptr;
    napi_status status = napi_create_double(env, value, &result);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("VariableConvertor::CreateDoubleValue: napi_create_double failed, status: %{public}d", status);
        return nullptr;
    }
    return result;
}

napi_value VariableConvertor::CreateBoolean(napi_env env, bool flag)
{
    napi_value result = nullptr;
    napi_status status = napi_get_boolean(env, flag, &result);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("create boolean js variable failed.");
        return nullptr;
    }
    return result;
}

void VariableConvertor::VerifyType(napi_env env, const std::string& valueName, napi_valuetype napiType,
    napi_value argv)
{
    napi_valuetype valueType = napi_valuetype::napi_undefined;
    napi_status status = napi_typeof(env, argv, &valueType);
    if (status != napi_ok || valueType != napiType) {
        auto iter = NAPI_TYPE_MAP.find(napiType);
        std::string type = iter != NAPI_TYPE_MAP.end() ? iter->second : "";
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, valueName, type, true);
    }
}

napi_status VariableConvertor::SetEnumValue(napi_env env, napi_value enumObj, const std::string& enumName,
    int32_t enumVal)
{
    napi_value name = nullptr;
    napi_status status = napi_create_string_utf8(env, enumName.c_str(), NAPI_AUTO_LENGTH, &name);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("VariableConvertor::SetEnumValue: create string %{public}s failed.", enumName.c_str());
        return status;
    }
    napi_value value = nullptr;
    status = napi_create_int32(env, enumVal, &value);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("VariableConvertor::SetEnumValue: create int32 %{public}d failed.", enumVal);
        return status;
    }
    status = napi_set_property(env, enumObj, name, value);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("VariableConvertor::SetEnumValue: set property %{public}s failed.", enumName.c_str());
        return status;
    }
    status = napi_set_property(env, enumObj, value, name);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("VariableConvertor::SetEnumValue: set property %{public}d failed.", enumVal);
        return status;
    }
    return napi_ok;
}

LocaleType VariableConvertor::GetLocaleType(napi_env env, napi_value locale)
{
    napi_valuetype valueType = napi_valuetype::napi_undefined;
    napi_status status = napi_typeof(env, locale, &valueType);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("VariableConvertor::GetLocaleType: Failed to get type of intlLocale.");
        return LocaleType::INVALID;
    } else if (valueType != napi_valuetype::napi_object) {
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, "locale", "Locale", true);
        return LocaleType::INVALID;
    }
    bool isBuiltinsLocale = VariableConvertor::IsBuiltinsLocale(env, locale);
    if (isBuiltinsLocale) {
        return LocaleType::BUILTINS_LOCALE;
    }
    bool isLocaleInfo = LocaleInfoAddon::IsLocaleInfo(env, locale);
    if (isLocaleInfo) {
        return LocaleType::LOCALE_INFO;
    }
    ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, "locale", "Locale", true);
    return LocaleType::INVALID;
}

std::shared_ptr<LocaleInfo> VariableConvertor::ParseLocaleInfo(napi_env env, napi_value localeInfo)
{
    LocaleInfoAddon *localeInfoAddon = nullptr;
    napi_status status = LocaleInfoAddon::UnwrapLocaleInfoAddon(env, localeInfo, &localeInfoAddon);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("VariableConvertor::ParseLocaleInfo: Failed to unwrap localeInfo.");
        return nullptr;
    } else if (!localeInfoAddon) {
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, "locale", "intl.Locale", true);
        return nullptr;
    } else if (!localeInfoAddon->GetLocaleInfo()) {
        ErrorUtil::NapiThrow(env, I18N_NOT_VALID, "locale", "a valid intl.Locale", true);
        return nullptr;
    }
    return localeInfoAddon->GetLocaleInfo();
}

std::string VariableConvertor::ParseBuiltinsLocale(napi_env env, napi_value builtinsLocale)
{
    napi_value func = nullptr;
    napi_status status = napi_get_named_property(env, builtinsLocale, "toString", &func);
    if (status != napi_ok || func == nullptr) {
        HILOG_ERROR_I18N("VariableConvertor::ParseBuiltinsLocale: Get function toString failed.");
        return "";
    }
    napi_value locale = nullptr;
    status = napi_call_function(env, builtinsLocale, func, 0, nullptr, &locale);
    if (status != napi_ok || locale == nullptr) {
        HILOG_ERROR_I18N("VariableConvertor::ParseBuiltinsLocale: Call function failed.");
        return "";
    }
    int32_t code = 0;
    std::string localeTag = VariableConvertor::GetString(env, locale, code);
    if (code != 0) {
        HILOG_ERROR_I18N("VariableConvertor::ParseBuiltinsLocale: Get string failed.");
        return "";
    }
    return localeTag;
}

napi_value VariableConvertor::CreateBuiltinsLocaleObject(napi_env env, const std::string& locale)
{
    napi_value param = nullptr;
    napi_status status = napi_create_string_utf8(env, locale.c_str(), NAPI_AUTO_LENGTH, &param);
    if (status != napi_ok || param == nullptr) {
        HILOG_ERROR_I18N("VariableConvertor::CreateBuiltinsLocaleObject: Create string failed.");
        return nullptr;
    }

    napi_value localeConstructor = VariableConvertor::GetBuiltinsLocaleConstructor(env);
    if (localeConstructor == nullptr) {
        HILOG_ERROR_I18N("VariableConvertor::CreateBuiltinsLocaleObject: Get constructor failed.");
        return nullptr;
    }

    size_t argc = 1;
    napi_value argv[1] = { param };
    napi_value builtinsLocale = nullptr;
    status = napi_new_instance(env, localeConstructor, argc, argv, &builtinsLocale);
    if (status != napi_ok || builtinsLocale == nullptr) {
        HILOG_ERROR_I18N("VariableConvertor::CreateBuiltinsLocaleObject: Create locale failed.");
        return nullptr;
    }
    return builtinsLocale;
}

napi_value VariableConvertor::CreateMap(napi_env env, std::unordered_map<std::string, std::string>& ump)
{
    napi_value result = nullptr;
    napi_status status = napi_create_map(env, &result);
    if (status != napi_ok || result == nullptr) {
        HILOG_ERROR_I18N("VariableConvertor::CreateMap: Create map failed.");
        return nullptr;
    }
    for (const auto& item : ump) {
        std::string key = item.first;
        napi_value JSKey = nullptr;
        status = napi_create_string_utf8(env, key.c_str(), NAPI_AUTO_LENGTH, &JSKey);
        if (status != napi_ok || JSKey == nullptr) {
            HILOG_ERROR_I18N("VariableConvertor::CreateMap: Create JSKey failed.");
            return nullptr;
        }
        std::string value = item.second;
        napi_value JSValue = nullptr;
        status = napi_create_string_utf8(env, value.c_str(), NAPI_AUTO_LENGTH, &JSValue);
        if (status != napi_ok || JSValue == nullptr) {
            HILOG_ERROR_I18N("VariableConvertor::CreateMap: Create JSValue failed.");
            return nullptr;
        }
        status = napi_map_set_property(env, result, JSKey, JSValue);
        if (status != napi_ok) {
            HILOG_ERROR_I18N("VariableConvertor::CreateMap: Map set property failed.");
            return nullptr;
        }
    }
    return result;
}

napi_value VariableConvertor::GetBuiltinsLocaleConstructor(napi_env env)
{
    napi_value global = nullptr;
    napi_status status = napi_get_global(env, &global);
    if (status != napi_ok || global == nullptr) {
        HILOG_ERROR_I18N("VariableConvertor::GetBuiltinsLocaleConstructor: Get global failed.");
        return nullptr;
    }

    napi_value intl = nullptr;
    status = napi_get_named_property(env, global, "Intl", &intl);
    if (status != napi_ok || intl == nullptr) {
        HILOG_ERROR_I18N("VariableConvertor::GetBuiltinsLocaleConstructor: Load intl failed.");
        return nullptr;
    }

    napi_value localeConstructor = nullptr;
    status = napi_get_named_property(env, intl, "Locale", &localeConstructor);
    if (status != napi_ok || localeConstructor == nullptr) {
        HILOG_ERROR_I18N("VariableConvertor::GetBuiltinsLocaleConstructor: Get locale constructor failed.");
        return nullptr;
    }
    return localeConstructor;
}

bool VariableConvertor::IsBuiltinsLocale(napi_env env, napi_value locale)
{
    napi_value localeConstructor = VariableConvertor::GetBuiltinsLocaleConstructor(env);
    if (localeConstructor == nullptr) {
        HILOG_ERROR_I18N("VariableConvertor::IsBuiltinsLocale: Get constructor failed.");
        return false;
    }

    bool isBuiltinsLocale = false;
    napi_status status = napi_instanceof(env, locale, localeConstructor, &isBuiltinsLocale);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("VariableConvertor::IsBuiltinsLocale: Get instance of locale failed.");
        return false;
    }
    return isBuiltinsLocale;
}

bool VariableConvertor::GetDateTime(napi_env env, napi_value date, double& milliseconds)
{
    bool isDate = false;
    napi_status status = napi_is_date(env, date, &isDate);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("VariableConvertor::GetDateTime: Failed to call napi_is_date.");
        return false;
    } else if (!isDate) {
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, "date", "Date", true);
        return false;
    }

    napi_value funcGetTime = nullptr;
    status = napi_get_named_property(env, date, GET_TIME_NAME.c_str(), &funcGetTime);
    if (status != napi_ok || funcGetTime == nullptr) {
        HILOG_ERROR_I18N("VariableConvertor::GetDateTime: Get named property failed.");
        return false;
    }

    napi_value value = nullptr;
    status = napi_call_function(env, date, funcGetTime, 0, nullptr, &value);
    if (status != napi_ok || value == nullptr) {
        HILOG_ERROR_I18N("VariableConvertor::GetDateTime: Call getTime failed.");
        return false;
    }

    status = napi_get_value_double(env, value, &milliseconds);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("VariableConvertor::GetDateTime: Get milliseconds failed.");
        return false;
    }
    return true;
}

napi_value VariableConvertor::CreateStyledString(napi_env env, const std::string &content,
    napi_value styleOption)
{
    size_t argc = 2;    // StyledString need 2 args
    napi_value argv[2] = {nullptr};
    argv[0] = VariableConvertor::CreateString(env, content);
    argv[1] = styleOption;

    napi_value global = nullptr;
    napi_status status = napi_get_global(env, &global);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("VariableConvertor::CreateStyledString: get global object failed");
        return nullptr;
    }
    napi_value constructor = nullptr;
    status = napi_get_named_property(env, global, "StyledString", &constructor);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("VariableConvertor::CreateStyledString: get StyledString failed");
        return nullptr;
    }

    napi_value styleString = nullptr;
    status = napi_new_instance(env, constructor, argc, argv, &styleString);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("VariableConvertor::CreateStyledString: napi_new_instance failed");
        return nullptr;
    }
    return styleString;
}

std::pair<uint8_t*, size_t> VariableConvertor::GetUint8Array(napi_env env, napi_value value, int32_t& code)
{
    bool isTypedArray = false;
    napi_status status = napi_is_typedarray(env, value, &isTypedArray);
    if (status != napi_ok) {
        code = I18N_NAPI_ERROR;
        HILOG_ERROR_I18N("VariableConvertor::IsUint8Array: napi_is_typedarray failed.");
        return { nullptr, 0 };
    }
    if (!isTypedArray) {
        code = I18N_NOT_FOUND;
        return { nullptr, 0 };
    }
    napi_typedarray_type type;
    size_t length = 0;
    void* data = nullptr;
    status = napi_get_typedarray_info(env, value, &type, &length, &data, nullptr, nullptr);
    if (status != napi_ok) {
        code = I18N_NAPI_ERROR;
        HILOG_ERROR_I18N("VariableConvertor::IsUint8Array: napi_get_typedarray_info failed.");
        return { nullptr, 0 };
    }
    if (type != napi_typedarray_type::napi_uint8_array) {
        code = I18N_NOT_FOUND;
        return { nullptr, 0 };
    }
    return { static_cast<uint8_t*>(data), length };
}

bool VariableConvertor::IsUndefined(napi_env env, napi_value value)
{
    napi_valuetype valueType = napi_valuetype::napi_undefined;
    napi_status status = napi_typeof(env, value, &valueType);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("VariableConvertor::IsUndefined: Get type failed.");
        return false;
    }
    return valueType == napi_valuetype::napi_undefined;
}

DateType VariableConvertor::GetDateType(napi_env env, napi_value date)
{
    bool isDate = false;
    napi_status status = napi_is_date(env, date, &isDate);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("VariableConvertor::GetDateType: Failed to call napi_is_date.");
        return DateType::INVALID;
    }
    if (isDate) {
        return DateType::DATE_OBJECT;
    }
    napi_valuetype valueType = napi_valuetype::napi_undefined;
    status = napi_typeof(env, date, &valueType);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("VariableConvertor::GetDateType: Get type failed.");
        return DateType::INVALID;
    }
    if (valueType == napi_valuetype::napi_number) {
        return DateType::NUMBER;
    } else if (valueType == napi_valuetype::napi_bigint) {
        return DateType::BIGINT;
    }
    return DateType::INVALID;
}

bool VariableConvertor::GetTimeFromDate(napi_env env, napi_value date, DateType type, double& milliseconds)
{
    napi_status status = napi_ok;
    switch (type) {
        case DateType::DATE_OBJECT:
            status = napi_get_date_value(env, date, &milliseconds);
            break;
        case DateType::NUMBER:
            status = napi_get_value_double(env, date, &milliseconds);
            break;
        case DateType::BIGINT: {
            int64_t ms = 0;
            bool lossless = false;
            status = napi_get_value_bigint_int64(env, date, &ms, &lossless);
            milliseconds = static_cast<double>(ms);
            break;
        }
        default:
            HILOG_ERROR_I18N("VariableConvertor::GetTimeFromDate: Invalid type.");
            return false;
    }
    if (status != napi_ok) {
        HILOG_ERROR_I18N("VariableConvertor::GetTimeFromDate: Get milliseconds failed.");
        return false;
    }
    return true;
}

NumberFormatType VariableConvertor::GetNumberFormatType(napi_env env, napi_value value)
{
    napi_valuetype valueType = napi_valuetype::napi_undefined;
    napi_status status = napi_typeof(env, value, &valueType);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("VariableConvertor::GetNumberFormatType: Get type failed.");
        return NumberFormatType::INVALID;
    } else if (valueType != napi_valuetype::napi_object) {
        HILOG_ERROR_I18N("VariableConvertor::GetNumberFormatType: Type != object.");
        return NumberFormatType::INVALID;
    }
    if (IntlNumberFormatAddon::IsIntlNumberFormat(env, value)) {
        return NumberFormatType::BUILTINS_NUMBER_FORMAT;
    } else if (NumberFormatAddon::IsNumberFormat(env, value)) {
        return NumberFormatType::NUMBER_FORMAT;
    } else if (SimpleNumberFormatAddon::IsSimpleNumberFormat(env, value)) {
        return NumberFormatType::SIMPLE_NUMBER_FORMAT;
    } else if (SymbolNumberFormatAddon::IsSymbolNumberFormat(env, value)) {
        return NumberFormatType::SYMBOL_NUMBER_FORMAT;
    }
    return NumberFormatType::INVALID;
}

DateTimeFormatType VariableConvertor::GetDateTimeFormatType(napi_env env, napi_value value)
{
    napi_valuetype valueType = napi_valuetype::napi_undefined;
    napi_status status = napi_typeof(env, value, &valueType);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("VariableConvertor::GetDateTimeFormatType: Get type failed.");
        return DateTimeFormatType::INVALID;
    } else if (valueType != napi_valuetype::napi_object) {
        HILOG_ERROR_I18N("VariableConvertor::GetDateTimeFormatType: Type != object.");
        return DateTimeFormatType::INVALID;
    }

    if (IntlDateTimeFormatAddon::IsIntlDateTimeFormat(env, value)) {
        return DateTimeFormatType::BUILTINS_DATE_TIME_FORMAT;
    } else if (SimpleDateTimeFormatAddon::IsSimpleDateTimeFormat(env, value)) {
        return DateTimeFormatType::SIMPLE_DATE_TIME_FORMAT;
    } else if (SymbolDateTimeFormatAddon::IsSymbolDateTimeFormat(env, value)) {
        return DateTimeFormatType::SYMBOL_DATE_TIME_FORMAT;
    }
    return DateTimeFormatType::INVALID;
}
} // namespace I18n
} // namespace Global
} // namespace OHOS