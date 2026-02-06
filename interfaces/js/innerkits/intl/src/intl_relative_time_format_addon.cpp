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
#include "intl_relative_time_format_addon.h"

#include <cmath>
#include "error_util.h"
#include "i18n_hilog.h"
#include "i18n_types.h"
#include "js_lifecycle_managers.h"
#include "js_utils.h"
#include "locale_helper.h"
#include "utils.h"

namespace OHOS {
namespace Global {
namespace I18n {

std::unordered_map<std::string, std::vector<std::string>> IntlRelativeTimeFormatAddon::optionAvaliableMap {
    {"localeMatcher", {"lookup", "best fit"}},
    {"numeric", {"always", "auto"}},
    {"style", {"long", "short", "narrow"}},
    {"unit", {"year", "years", "quarter", "quarters", "month", "months", "week", "weeks",
            "day", "days", "hour", "hours", "minute", "minutes", "second", "seconds"}},
};

IntlRelativeTimeFormatAddon::IntlRelativeTimeFormatAddon()
{
}

IntlRelativeTimeFormatAddon::~IntlRelativeTimeFormatAddon()
{
}

void IntlRelativeTimeFormatAddon::Destructor(napi_env env, void *nativeObject, void *hint)
{
    if (!nativeObject) {
        return;
    }
    delete reinterpret_cast<IntlRelativeTimeFormatAddon *>(nativeObject);
    nativeObject = nullptr;
}

napi_value IntlRelativeTimeFormatAddon::InitIntlRelativeTimeFormat(napi_env env, napi_value exports)
{
    HandleScope scope(env, "InitIntlRelativeTimeFormat");
    if (!scope.IsOpen()) {
        return nullptr;
    }

    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("format", Format),
        DECLARE_NAPI_FUNCTION("formatToParts", FormatToParts),
        DECLARE_NAPI_FUNCTION("resolvedOptions", GetNumberResolvedOptions),
        DECLARE_NAPI_STATIC_FUNCTION("supportedLocalesOf", SupportedLocalesOf),
    };
    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, "RelativeTimeFormat", NAPI_AUTO_LENGTH,
        RelativeTimeFormatConstructor, nullptr,
        sizeof(properties) / sizeof(napi_property_descriptor), properties, &constructor);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("InitIntlRelativeTimeFormat: Define class failed when init JsRelativeTimeFormat");
        return nullptr;
    }
    status = napi_set_named_property(env, exports, "RelativeTimeFormat", constructor);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("InitIntlRelativeTimeFormat: Set property failed when init JsRelativeTimeFormat");
        return nullptr;
    }
    return exports;
}

napi_value IntlRelativeTimeFormatAddon::RelativeTimeFormatConstructor(napi_env env, napi_callback_info info)
{
    size_t argc = 2;
    napi_value argv[2] = { nullptr };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("RelativeTimeFormatConstructor: Get parameter info failed");
        return nullptr;
    }
    napi_value newTarget;
    status = napi_get_new_target(env, info, &newTarget);
    if (status != napi_ok  || newTarget == nullptr) {
        HILOG_ERROR_I18N("RelativeTimeFormatConstructor: Intl.RelativeTimeFormat requires new");
        napi_throw_type_error(env, nullptr, "newTarget is undefined");
        return nullptr;
    }
    IntlRelativeTimeFormatAddon* obj = new (std::nothrow) IntlRelativeTimeFormatAddon();
    if (obj == nullptr) {
        HILOG_ERROR_I18N("RelativeTimeFormatConstructor: Create IntlRelativeTimeFormatAddon failed");
        return nullptr;
    }
    std::vector<std::string> localeTags;
    if (!obj->GetParameterLocales(env, localeTags, argc, argv)) {
        delete obj;
        return nullptr;
    }
    std::map<std::string, std::string> map = {};
    if (argc > 1) {
        GetRelativeTimeOptionValues(env, argv[1], map);
    }
    status = napi_wrap(env, thisVar, reinterpret_cast<void *>(obj), IntlRelativeTimeFormatAddon::Destructor,
        nullptr, nullptr);
    if (status != napi_ok) {
        delete obj;
        HILOG_ERROR_I18N("RelativeTimeFormatConstructor: Wrap IntlRelativeTimeFormatAddon failed");
        return nullptr;
    }
    if (!obj->InitRelativeTimeFormatContext(env, info, localeTags, map)) {
        HILOG_ERROR_I18N("Init RelativeTimeFormat failed");
        return nullptr;
    }
    return thisVar;
}

bool IntlRelativeTimeFormatAddon::GetParameterLocales(napi_env env,
    std::vector<std::string> &localeTags, const size_t &argc, napi_value* argv)
{
    if (argc < 1) {
        localeTags.push_back("en-US");
        return true;
    }
    int32_t code = 0;
    std::vector<std::string> localeArray = JSUtils::GetLocaleArray(env, argv[0], code);
    if (localeArray.empty()) {
        localeTags.push_back("en-US");
        return true;
    }
    std::string checkResult = LocaleHelper::CheckParamLocales(localeArray);
    if (!checkResult.empty()) {
        ErrorUtil::NapiThrowUndefined(env, checkResult);
        return false;
    }
    ErrorMessage errorMsg;
    localeArray = RelativeTimeFormat::CanonicalizeLocales(localeArray, errorMsg);
    if (errorMsg.type != ErrorType::NO_ERROR) {
        napi_throw_range_error(env, nullptr, errorMsg.message.c_str());
        return false;
    }
    localeTags.assign(localeArray.begin(), localeArray.end());
    return true;
}

void IntlRelativeTimeFormatAddon::GetRelativeTimeOptionValues(napi_env env,
    napi_value options, std::map<std::string, std::string> &map)
{
    JSUtils::GetOptionValue(env, options, "localeMatcher", map);
    CheckOptionValue(env, map, "localeMatcher");
    JSUtils::GetOptionValue(env, options, "numeric", map);
    CheckOptionValue(env, map, "numeric");
    JSUtils::GetOptionValue(env, options, "style", map);
    CheckOptionValue(env, map, "style");
}

void IntlRelativeTimeFormatAddon::CheckOptionValue(napi_env env, std::map<std::string, std::string> &map,
    const std::string &optionName)
{
    if (optionAvaliableMap.find(optionName) == optionAvaliableMap.end()) {
        return;
    }
    if (map.find(optionName) != map.end()) {
        std::string optionValue = map[optionName];
        CheckOptionValue(env, optionValue, optionName);
    }
}

void IntlRelativeTimeFormatAddon::CheckOptionValue(napi_env env, const std::string &optionValue,
    const std::string &optionName)
{
    if (optionAvaliableMap.find(optionName) == optionAvaliableMap.end()) {
        return;
    }
    std::vector<std::string> aviliableValues = optionAvaliableMap[optionName];
    auto it = std::find(aviliableValues.begin(), aviliableValues.end(), optionValue);
    if (it != aviliableValues.end()) {
        return;
    }
    if (!ThrowRangeError(env)) {
        HILOG_ERROR_I18N("IntlRelativeTimeFormatAddon::CheckOptionValue:"
            "throw error for %{public}s failed", optionName.c_str());
        return;
    }
}

bool IntlRelativeTimeFormatAddon::ThrowRangeError(napi_env env)
{
    const char* msg = "Value out of range for locale options property";
    napi_status status = napi_throw_range_error(env, nullptr, msg);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("IntlRelativeTimeFormatAddon::ThrowRangeError: throw range error failed");
        return false;
    }
    return true;
}

bool IntlRelativeTimeFormatAddon::InitRelativeTimeFormatContext(napi_env env, napi_callback_info info,
    std::vector<std::string> localeTags, std::map<std::string, std::string> &map)
{
    relativetimefmt_ = std::make_shared<RelativeTimeFormat>(localeTags, map, true);
    return relativetimefmt_ != nullptr;
}

napi_value IntlRelativeTimeFormatAddon::Format(napi_env env, napi_callback_info info)
{
    size_t argc = 2;
    napi_value argv[2] = { nullptr };
    napi_value thisVar = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("IntlRelativeTimeFormatAddon::Format: Get parameter info failed");
        return JSUtils::CreateEmptyString(env);
    }
    if (argc < 2) { // 2 is required parameter count
        HILOG_ERROR_I18N("IntlRelativeTimeFormatAddon::Format: parameter less then required");
        return JSUtils::CreateEmptyString(env);
    }
    napi_valuetype valueType = napi_valuetype::napi_undefined;
    status = napi_typeof(env, argv[0], &valueType);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("IntlRelativeTimeFormatAddon::Format: Get param type failed.");
        return JSUtils::CreateEmptyString(env);
    }
    int32_t code = 0;
    double number = JSUtils::GetDoubleFromNapiValue(env, argv[0], valueType, code);
    if (code != 0) {
        HILOG_ERROR_I18N("IntlRelativeTimeFormatAddon::Format: Get number failed");
        return JSUtils::CreateEmptyString(env);
    }
    std::string unit = JSUtils::GetString(env, argv[1], code);
    if (code != 0) {
        return JSUtils::CreateEmptyString(env);
    }
    CheckOptionValue(env, unit, "unit");
    IntlRelativeTimeFormatAddon *obj = nullptr;
    status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->relativetimefmt_) {
        HILOG_ERROR_I18N("IntlRelativeTimeFormatAddon::Format: Get RelativeTimeFormat object failed");
        return JSUtils::CreateEmptyString(env);
    }
    std::string value = obj->relativetimefmt_->Format(number, unit);
    return JSUtils::CreateString(env, value);
}

napi_value IntlRelativeTimeFormatAddon::GetNumberResolvedOptions(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, &data);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("IntlRelativeTimeFormatAddon::GetNumberResolvedOptions: Get parameter info failed");
        return JSUtils::CreateEmptyObject(env);
    }
    IntlRelativeTimeFormatAddon *obj = nullptr;
    status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->relativetimefmt_) {
        HILOG_ERROR_I18N("GetNumberResolvedOptions: Get RelativeTimeFormat object failed");
        return JSUtils::CreateEmptyObject(env);
    }
    napi_value result = nullptr;
    status = napi_create_object(env, &result);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("IntlRelativeTimeFormatAddon::GetNumberResolvedOptions: napi_create_object failed");
        return JSUtils::CreateEmptyObject(env);
    }
    std::map<std::string, std::string> options = {};
    obj->relativetimefmt_->GetResolvedOptions(options);
    JSUtils::SetOptionProperties(env, result, options, "locale");
    JSUtils::SetOptionProperties(env, result, options, "style");
    JSUtils::SetOptionProperties(env, result, options, "numeric");
    JSUtils::SetOptionProperties(env, result, options, "numberingSystem");
    return result;
}

napi_value IntlRelativeTimeFormatAddon::FormatToParts(napi_env env, napi_callback_info info)
{
    size_t argc = 2;
    const size_t argsCount = 2; // 2 is required parameter count
    napi_value argv[argsCount] = { nullptr };
    napi_value thisVar = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("IntlRelativeTimeFormatAddon::FormatToParts: Get parameter info failed");
        return JSUtils::CreateEmptyArray(env);
    }
    if (argc < argsCount) {
        HILOG_ERROR_I18N("IntlRelativeTimeFormatAddon::FormatToParts: parameter count less then required.");
        return JSUtils::CreateEmptyArray(env);
    }
    napi_valuetype valueType = napi_valuetype::napi_undefined;
    status = napi_typeof(env, argv[0], &valueType);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("IntlRelativeTimeFormatAddon::FormatToParts: Get param type failed.");
        return JSUtils::CreateEmptyArray(env);
    }
    int32_t code = 0;
    double number = JSUtils::GetDoubleFromNapiValue(env, argv[0], valueType, code);
    if (code != 0) {
        HILOG_ERROR_I18N("IntlRelativeTimeFormatAddon::FormatToParts: Get parameter number failed");
        return JSUtils::CreateEmptyArray(env);
    }
    std::string unit = JSUtils::GetString(env, argv[argsCount - 1], code);
    if (code != 0) {
        HILOG_ERROR_I18N("IntlRelativeTimeFormatAddon::FormatToParts: Get second parameter failed");
        return JSUtils::CreateEmptyArray(env);
    }
    return GetFormatToPartsInner(env, number, unit, thisVar);
}

napi_value IntlRelativeTimeFormatAddon::GetFormatToPartsInner(napi_env env, double number,
    const std::string &unit, napi_value &thisVar)
{
    IntlRelativeTimeFormatAddon *obj = nullptr;
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->relativetimefmt_) {
        HILOG_ERROR_I18N("GetFormatToPartsInner: Get NumberFormat object failed");
        return JSUtils::CreateEmptyArray(env);
    }
    std::vector<std::vector<std::string>> relativeTimeParts;
    obj->relativetimefmt_->FormatToParts(number, unit, relativeTimeParts);
    return SetNumberFormatParts(env, relativeTimeParts);
}

napi_value IntlRelativeTimeFormatAddon::SupportedLocalesOf(napi_env env, napi_callback_info info)
{
    size_t argc = 2;
    napi_value argv[2] = { nullptr };
    napi_value thisVar = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("IntlRelativeTimeFormatAddon::SupportedLocalesOf: Get parameter info failed");
        return JSUtils::CreateEmptyArray(env);
    }
    std::vector<std::string> localeTags;
    if (argc < 1) {
        return JSUtils::CreateArray(env, localeTags);
    }
    int32_t code = 0;
    std::vector<std::string> localeArray = JSUtils::GetLocaleArray(env, argv[0], code);
    localeTags.assign(localeArray.begin(), localeArray.end());
    std::map<std::string, std::string> map = {};
    if (argc > 1) {
        JSUtils::GetOptionValue(env, argv[1], "localeMatcher", map);
        CheckOptionValue(env, map, "localeMatcher");
    }
    I18nErrorCode i18nStatus = I18nErrorCode::SUCCESS;
    std::vector<std::string> resultLocales = RelativeTimeFormat::SupportedLocalesOf(localeTags, map, i18nStatus);
    if (i18nStatus == I18nErrorCode::INVALID_LOCALE_TAG) {
        HILOG_ERROR_I18N("IntlRelativeTimeFormatAddon::SupportedLocalesOf: SupportedLocalesOf status fail");
        ErrorUtil::NapiThrowUndefined(env, "invalid locale");
        return nullptr;
    } else if (i18nStatus != I18nErrorCode::SUCCESS) {
        ErrorUtil::NapiThrowUndefined(env, "getStringOption failed");
        return nullptr;
    }
    return JSUtils::CreateArray(env, resultLocales);
}

napi_value IntlRelativeTimeFormatAddon::SetNumberFormatParts(napi_env env,
    const std::vector<std::vector<std::string>> &numberParts)
{
    napi_value result = nullptr;
    napi_status status = napi_create_array_with_length(env, numberParts.size(), &result);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("SetNumberFormatParts: Failed to create array");
        return JSUtils::CreateEmptyArray(env);
    }
    for (size_t i = 0; i < numberParts.size(); i++) {
        if (numberParts[i].size() < 2) { // 2 is minimum vector's size
            HILOG_ERROR_I18N("SetNumberFormatParts: numberParts's item in %{public}zu lack key&value pair", i);
            return JSUtils::CreateEmptyArray(env);
        }
        napi_value value = JSUtils::CreateString(env, numberParts[i][1].c_str());
        if (value == nullptr) {
            HILOG_ERROR_I18N("Failed to create string item numberParts[i][1].");
            return JSUtils::CreateEmptyArray(env);
        }
        napi_value type = JSUtils::CreateString(env, numberParts[i][0].c_str());
        if (type == nullptr) {
            HILOG_ERROR_I18N("Failed to create string item numberParts[i][0].");
            return JSUtils::CreateEmptyArray(env);
        }
        napi_value unit = nullptr;
        if (numberParts[i].size() > 2) { // 2 is vector's size which not contains unit
            unit = JSUtils::CreateString(env, numberParts[i][2].c_str());
            if (unit == nullptr) {
                HILOG_ERROR_I18N("Failed to create string item numberParts[i][2].");
                return JSUtils::CreateEmptyArray(env);
            }
        }
        std::unordered_map<std::string, napi_value> propertys {
            { "type", type },
            { "value", value },
            { "unit", unit }
        };
        std::vector<std::string> keyVect = { "type", "value", "unit" };
        int32_t code = 0;
        napi_value formatInfo = JSUtils::CreateArrayItem(env, propertys, code, keyVect);
        if (code != 0) {
            return JSUtils::CreateEmptyArray(env);
        }
        status = napi_set_element(env, result, i, formatInfo);
        if (status != napi_ok) {
            HILOG_ERROR_I18N("Failed to set array item");
            return JSUtils::CreateEmptyArray(env);
        }
    }
    return result;
}

} // namespace I18n
} // namespace Global
} // namespace OHOS