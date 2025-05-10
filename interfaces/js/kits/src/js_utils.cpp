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

std::string JSUtils::GetLocaleTag(napi_env env, napi_value argv)
{
    if (argv == nullptr) {
        return "";
    }
    napi_valuetype valueType = napi_valuetype::napi_undefined;
    napi_typeof(env, argv, &valueType);
    if (valueType != napi_valuetype::napi_string) {
        HILOG_ERROR_I18N("GetLocaleTag: Parameter type does not match");
        return "";
    }
    size_t len = 0;
    napi_status status = napi_get_value_string_utf8(env, argv, nullptr, 0, &len);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("GetLocaleTag -> string: Get locale tag length failed");
        return "";
    }
    std::vector<char> buf;
    buf.resize(len + 1);
    status = napi_get_value_string_utf8(env, argv, buf.data(), len + 1, &len);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("GetLocaleTag: Get locale tag failed");
        return "";
    }
    std::string result(buf.data());
    return result;
}

void JSUtils::GetLocaleTags(napi_env env, napi_value rawLocaleTag, std::vector<std::string> &localeTags)
{
    size_t len = 0;
    napi_status status = napi_get_value_string_utf8(env, rawLocaleTag, nullptr, 0, &len);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("GetLocaleTag -> void: Get locale tag length failed");
        return;
    }
    std::vector<char> buf(len + 1);
    status = napi_get_value_string_utf8(env, rawLocaleTag, buf.data(), len + 1, &len);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("GetLocaleTags: Get locale tag failed");
        return;
    }
    localeTags.push_back(buf.data());
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
        napi_create_string_utf8(env, optionValue.c_str(), NAPI_AUTO_LENGTH, &optionJsValue);
        napi_set_named_property(env, result, option.c_str(), optionJsValue);
    } else {
        napi_value undefined = nullptr;
        napi_get_undefined(env, &undefined);
        napi_set_named_property(env, result, option.c_str(), undefined);
    }
}

void JSUtils::SetBooleanOptionProperties(napi_env env, napi_value &result, std::map<std::string, std::string> &options,
    const std::string &option)
{
    if (options.count(option) > 0) {
        std::string optionValue = options[option];
        bool optionBoolValue = (optionValue == "true");
        napi_value optionJsValue = nullptr;
        napi_get_boolean(env, optionBoolValue, &optionJsValue);
        napi_set_named_property(env, result, option.c_str(), optionJsValue);
    } else {
        napi_value undefined = nullptr;
        napi_get_undefined(env, &undefined);
        napi_set_named_property(env, result, option.c_str(), undefined);
    }
}

void JSUtils::SetIntegerOptionProperties(napi_env env, napi_value &result, std::map<std::string, std::string> &options,
    const std::string &option)
{
    if (options.count(option) > 0) {
        std::string optionValue = options[option];
        napi_value optionJsValue = nullptr;
        int32_t status = 0;
        int64_t integerValue = ConvertString2Int(optionValue, status);
        if (status != -1) {
            napi_create_int64(env, integerValue, &optionJsValue);
            napi_set_named_property(env, result, option.c_str(), optionJsValue);
            return;
        }
    }
    napi_value undefined = nullptr;
    napi_get_undefined(env, &undefined);
    napi_set_named_property(env, result, option.c_str(), undefined);
}
} // namespace I18n
} // namespace Global
} // namespace OHOS