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
#include "intl_number_format_addon.h"

#include <cmath>
#include "error_util.h"
#include "i18n_hilog.h"
#include "js_lifecycle_managers.h"
#include "js_utils.h"
#include "locale_helper.h"
#include "utils.h"

namespace OHOS {
namespace Global {
namespace I18n {

IntlNumberFormatAddon::IntlNumberFormatAddon()
{
}

IntlNumberFormatAddon::~IntlNumberFormatAddon()
{
}

void IntlNumberFormatAddon::Destructor(napi_env env, void *nativeObject, void *hint)
{
    if (!nativeObject) {
        return;
    }
    delete reinterpret_cast<IntlNumberFormatAddon *>(nativeObject);
    nativeObject = nullptr;
}

napi_value IntlNumberFormatAddon::InitIntlNumberFormat(napi_env env, napi_value exports)
{
    HandleScope scope(env, "InitIntlNumberFormat");
    if (!scope.IsOpen()) {
        return nullptr;
    }

    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("format", FormatNumber),
        DECLARE_NAPI_FUNCTION("formatToParts", FormatToParts),
        DECLARE_NAPI_FUNCTION("formatRange", FormatRangeNumber),
        DECLARE_NAPI_FUNCTION("formatRangeToParts", FormatRangeToParts),
        DECLARE_NAPI_FUNCTION("resolvedOptions", GetNumberResolvedOptions),
        DECLARE_NAPI_FUNCTION("toString", ToString),
        DECLARE_NAPI_STATIC_FUNCTION("supportedLocalesOf", SupportedLocalesOf),
    };

    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, "NumberFormat", NAPI_AUTO_LENGTH, NumberFormatConstructor, nullptr,
        sizeof(properties) / sizeof(napi_property_descriptor), properties, &constructor);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("InitIntlNumberFormat: Define class failed when InitNumberFormat");
        return nullptr;
    }
    status = napi_set_named_property(env, exports, "NumberFormat", constructor);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("InitIntlNumberFormat: Set property failed when InitNumberFormat");
        return nullptr;
    }
    return exports;
}

napi_value IntlNumberFormatAddon::NumberFormatConstructor(napi_env env, napi_callback_info info)
{
    size_t argc = 2;
    napi_value argv[2] = { nullptr };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        return nullptr;
    }
    napi_value new_target;
    status = napi_get_new_target(env, info, &new_target);
    if (status == napi_pending_exception || new_target == nullptr) {
        return CreateNumberFormatWithoutNew(env, argc, argv);
    }
    IntlNumberFormatAddon* obj = new (std::nothrow) IntlNumberFormatAddon();
    if (obj == nullptr) {
        HILOG_ERROR_I18N("NumberFormatConstructor: Create IntlNumberFormatAddon object failed");
        return nullptr;
    }
    std::vector<std::string> localeTags;
    if (!obj->HandleArguments(env, argv[0], argc, localeTags)) {
        delete obj;
        return nullptr;
    }
    std::map<std::string, std::string> map = {};
    if (argc > 1) {
        JSUtils::GetNumberOptionValues(env, argv[1], map);
    }
    status = napi_wrap_s(env, thisVar, reinterpret_cast<void *>(obj), IntlNumberFormatAddon::Destructor, nullptr,
        &TYPE_TAG, nullptr);
    if (status != napi_ok) {
        delete obj;
        HILOG_ERROR_I18N("NumberFormatConstructor: Wrap IntlNumberFormatAddon failed");
        return nullptr;
    }
    if (!obj->InitNumberFormatContext(env, localeTags, map)) {
        HILOG_ERROR_I18N("Init NumberFormat failed");
        return nullptr;
    }
    return thisVar;
}

bool IntlNumberFormatAddon::HandleArguments(napi_env env, napi_value argVal, size_t argc,
    std::vector<std::string> &localeTags)
{
    if (argc < 1) {
        std::string defaultLocale = NumberFormat::GetIcuDefaultLocale();
        localeTags.push_back(defaultLocale);
        return true;
    }
    int32_t code = 0;
    std::vector<std::string> localeArray = JSUtils::GetLocaleArray(env, argVal, code);
    if (localeArray.empty()) {
        std::string defaultLocale = NumberFormat::GetIcuDefaultLocale();
        localeTags.push_back(defaultLocale);
        return true;
    }
    std::string checkResult = LocaleHelper::CheckParamLocales(localeArray);
    if (!checkResult.empty()) {
        ErrorUtil::NapiThrowUndefined(env, checkResult);
        return false;
    }
    localeTags.assign(localeArray.begin(), localeArray.end());
    return true;
}

bool IntlNumberFormatAddon::InitNumberFormatContext(napi_env env,
    const std::vector<std::string> &localeTags, std::map<std::string, std::string> &map)
{
    int32_t code = 0;
    std::string message = NumberFormat::CheckNumberFormatOptions(map, code);
    if (code != 0) {
        ErrorUtil::NapiThrowUndefined(env, message);
        return false;
    }
    numberfmt_ = std::make_shared<NumberFormat>(localeTags, map, true);
    return numberfmt_ != nullptr;
}

napi_value IntlNumberFormatAddon::CreateNumberFormatWithoutNew(napi_env env, size_t argc,
    napi_value *argv)
{
    napi_value exception;
    napi_status status = napi_get_and_clear_last_exception(env, &exception);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("CreateNumberFormatWithoutNew: napi_get_and_clear_last_exception failed");
        return nullptr;
    }
    napi_value constructor = JSUtils::GetConstructor(env, "NumberFormat");
    if (constructor == nullptr) {
        HILOG_ERROR_I18N("CreateNumberFormatWithoutNew: get constructor failed");
        return nullptr;
    }
    napi_value instance = nullptr;
    status = napi_new_instance(env, constructor, argc, argv, &instance);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("CreateNumberFormatWithoutNew: napi_new_instance failed");
        return nullptr;
    }
    return instance;
}

napi_value IntlNumberFormatAddon::FormatNumber(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = { nullptr };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("IntlNumberFormatAddon::FormatNumber: Get patameter info failed");
        return JSUtils::CreateEmptyString(env);
    }
    if (argc < 1) {
        HILOG_ERROR_I18N("IntlNumberFormatAddon::FormatNumber: Param count less then required");
        ErrorUtil::NapiThrowUndefined(env, "parameter is empty");
        return nullptr;
    }
    napi_valuetype valueType = napi_valuetype::napi_undefined;
    status = napi_typeof(env, argv[0], &valueType);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("IntlNumberFormatAddon::FormatNumber: Get parameter type failed");
        return JSUtils::CreateEmptyString(env);
    }
    return FormatWithDiffParamType(env, argv[0], valueType, thisVar);
}

napi_value IntlNumberFormatAddon::FormatRangeNumber(napi_env env, napi_callback_info info)
{
    size_t argc = 2;
    napi_value argv[2] = { nullptr };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("FormatRangeNumber: Get parameter info failed");
        return JSUtils::CreateEmptyString(env);
    }
    if (argc != 2) { // 2 is parameter count
        HILOG_ERROR_I18N("FormatRangeNumber: Insufficient parameters");
        return JSUtils::CreateEmptyString(env);
    }
    int32_t errorCode = 0;
    double start = GetFormatParam(env, argv[0], errorCode);
    if (errorCode != 0) {
        HILOG_ERROR_I18N("FormatRangeNumber: Get first param value failed");
        return JSUtils::CreateEmptyString(env);
    }
    double end = GetFormatParam(env, argv[1], errorCode);
    if (errorCode != 0) {
        HILOG_ERROR_I18N("FormatRangeNumber: Get second param value failed");
        return JSUtils::CreateEmptyString(env);
    }
    IntlNumberFormatAddon *obj = nullptr;
    status = napi_unwrap_s(env, thisVar, &TYPE_TAG, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->numberfmt_) {
        HILOG_ERROR_I18N("FormatRangeNumber: Get NumberFormat object failed");
        return JSUtils::CreateEmptyString(env);
    }
    std::string value = obj->numberfmt_->FormatJsRange(start, end);
    napi_value result;
    status = napi_create_string_utf8(env, value.c_str(), NAPI_AUTO_LENGTH, &result);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("FormatRangeNumber: create string js variable failed.");
        return JSUtils::CreateEmptyString(env);
    }
    return result;
}

napi_value IntlNumberFormatAddon::FormatRangeToParts(napi_env env, napi_callback_info info)
{
    size_t argc = 2;
    napi_value argv[2] = { nullptr };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("FormatRangeToParts: Get parameter info failed");
        return JSUtils::CreateEmptyArray(env);
    }
    if (argc != 2) { // 2 is parameter count
        HILOG_ERROR_I18N("FormatRangeToParts: Insufficient parameters");
        return JSUtils::CreateEmptyArray(env);
    }
    int32_t code = 0;
    double start = GetFormatParam(env, argv[0], code);
    if (code != 0) {
        HILOG_ERROR_I18N("FormatRangeToParts: Get first param value failed");
        return JSUtils::CreateEmptyArray(env);
    }
    double end = GetFormatParam(env, argv[1], code);
    if (code != 0) {
        HILOG_ERROR_I18N("FormatRangeToParts: Get second param value failed");
        return JSUtils::CreateEmptyArray(env);
    }
    IntlNumberFormatAddon *obj = nullptr;
    status = napi_unwrap_s(env, thisVar, &TYPE_TAG, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->numberfmt_) {
        HILOG_ERROR_I18N("FormatRangeToParts: Get NumberFormat object failed");
        return JSUtils::CreateEmptyArray(env);
    }
    std::vector<std::vector<std::string>> result;
    obj->numberfmt_->FormatRangeToParts(start, end, result);
    return SetNumberFormatParts(env, status, result);
}

double IntlNumberFormatAddon::GetFormatParam(napi_env env, napi_value param, int32_t &code)
{
    napi_valuetype valueType = napi_valuetype::napi_undefined;
    napi_status status = napi_typeof(env, param, &valueType);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("IntlNumberFormatAddon::GetFormatParam: Get parameter type failed");
        code = -1;
        return 0;
    }
    return JSUtils::GetDoubleFromNapiValue(env, param, valueType, code);
}

napi_value IntlNumberFormatAddon::GetNumberResolvedOptions(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, &data);
    if (status != napi_ok) {
        return JSUtils::CreateEmptyObject(env);
    }
    IntlNumberFormatAddon *obj = nullptr;
    status = napi_unwrap_s(env, thisVar, &TYPE_TAG, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->numberfmt_) {
        HILOG_ERROR_I18N("GetNumberResolvedOptions: Get NumberFormat object failed");
        return JSUtils::CreateEmptyObject(env);
    }
    napi_value result = nullptr;
    status = napi_create_object(env, &result);
    if (status != napi_ok) {
        return JSUtils::CreateEmptyObject(env);
    }
    std::map<std::string, std::string> options = {};
    obj->numberfmt_->GetResolvedOptions(options, true);
    JSUtils::SetOptionProperties(env, result, options, "locale");
    JSUtils::SetOptionProperties(env, result, options, "currency");
    JSUtils::SetOptionProperties(env, result, options, "currencySign");
    JSUtils::SetOptionProperties(env, result, options, "currencyDisplay");
    JSUtils::SetOptionProperties(env, result, options, "unit");
    JSUtils::SetOptionProperties(env, result, options, "unitDisplay");
    JSUtils::SetOptionProperties(env, result, options, "signDisplay");
    JSUtils::SetOptionProperties(env, result, options, "compactDisplay");
    JSUtils::SetOptionProperties(env, result, options, "notation");
    JSUtils::SetOptionProperties(env, result, options, "style");
    JSUtils::SetOptionProperties(env, result, options, "numberingSystem");
    JSUtils::SetOptionProperties(env, result, options, "unitUsage");
    JSUtils::SetBooleanOptionProperties(env, result, options, "useGrouping");
    JSUtils::SetIntegerOptionProperties(env, result, options, "minimumIntegerDigits");
    JSUtils::SetIntegerOptionProperties(env, result, options, "minimumFractionDigits");
    JSUtils::SetIntegerOptionProperties(env, result, options, "maximumFractionDigits");
    JSUtils::SetIntegerOptionProperties(env, result, options, "minimumSignificantDigits");
    JSUtils::SetIntegerOptionProperties(env, result, options, "maximumSignificantDigits");
    JSUtils::SetIntegerOptionProperties(env, result, options, "roundingIncrement");
    JSUtils::SetOptionProperties(env, result, options, "localeMatcher");
    JSUtils::SetOptionProperties(env, result, options, "roundingMode");
    JSUtils::SetOptionProperties(env, result, options, "roundingPriority");
    return result;
}

napi_value IntlNumberFormatAddon::FormatToParts(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = { nullptr };
    napi_value thisVar = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (status != napi_ok) {
        return JSUtils::CreateEmptyArray(env);
    }
    napi_valuetype valueType = napi_valuetype::napi_undefined;
    if (argc < 1) {
        return GetFormatToPartsInner(env, nullptr, valueType, thisVar);
    }
    status = napi_typeof(env, argv[0], &valueType);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("IntlNumberFormatAddon::FormatToParts: Get param type failed.");
        return JSUtils::CreateEmptyArray(env);
    }
    return GetFormatToPartsInner(env, argv[0], valueType, thisVar);
}

napi_value IntlNumberFormatAddon::ToString(napi_env env, napi_callback_info info)
{
    return JSUtils::CreateString(env, "[object Intl.NumberFormat]");
}

napi_value IntlNumberFormatAddon::FormatWithDiffParamType(napi_env env, napi_value param,
    napi_valuetype &valueType, napi_value &thisVar)
{
    IntlNumberFormatAddon *obj = nullptr;
    napi_status status = napi_unwrap_s(env, thisVar, &TYPE_TAG, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->numberfmt_) {
        HILOG_ERROR_I18N("FormatWithDiffParamType: Get NumberFormat object failed");
        return JSUtils::CreateEmptyString(env);
    }
    std::string formatedValue;
    if (valueType == napi_valuetype::napi_bigint) {
        int32_t code = 0;
        std::string bigintValue = JSUtils::GetBigIntStr(env, param, code);
        if (code != 0) {
            HILOG_ERROR_I18N("FormatWithDiffParamType: Get bigint parameter value failed");
            return JSUtils::CreateEmptyString(env);
        }
        formatedValue = obj->numberfmt_->FormatBigInt(bigintValue);
    } else if (valueType == napi_valuetype::napi_number) {
        double number = 0;
        status = napi_get_value_double(env, param, &number);
        if (status != napi_ok) {
            HILOG_ERROR_I18N("FormatWithDiffParamType: Get double parameter value failed");
            return JSUtils::CreateEmptyString(env);
        }
        formatedValue = obj->numberfmt_->Format(number);
    } else {
        int32_t errorCode = 0;
        double paramValue = JSUtils::GetNumberValue(env, param, errorCode);
        if (errorCode != 0) {
            HILOG_ERROR_I18N("FormatWithDiffParamType: Get Number object value failed");
            return JSUtils::CreateEmptyString(env);
        }
        formatedValue = obj->numberfmt_->Format(paramValue);
    }
    return JSUtils::CreateString(env, formatedValue);
}

napi_value IntlNumberFormatAddon::GetFormatToPartsInner(napi_env env, napi_value param,
    napi_valuetype &valueType, napi_value &thisVar)
{
    IntlNumberFormatAddon *obj = nullptr;
    napi_status status = napi_unwrap_s(env, thisVar, &TYPE_TAG, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->numberfmt_) {
        HILOG_ERROR_I18N("GetFormatToPartsInner: Get NumberFormat object failed");
        return JSUtils::CreateEmptyArray(env);
    }
    std::vector<std::vector<std::string>> numberParts;
    if (param == nullptr) {
        obj->numberfmt_->FormatToParts(numberParts);
        return SetNumberFormatParts(env, status, numberParts);
    }
    if (valueType == napi_valuetype::napi_bigint) {
        int32_t code = 0;
        std::string bigintValue = JSUtils::GetBigIntStr(env, param, code);
        if (code != 0) {
            HILOG_ERROR_I18N("GetFormatToPartsInner: Get bigint parameter value failed");
            return JSUtils::CreateEmptyArray(env);
        }
        obj->numberfmt_->FormatBigIntToParts(bigintValue, numberParts);
    } else if (valueType == napi_valuetype::napi_number) {
        double number = 0;
        status = napi_get_value_double(env, param, &number);
        if (status != napi_ok) {
            HILOG_ERROR_I18N("GetFormatToPartsInner: Get double parameter value failed");
            return JSUtils::CreateEmptyArray(env);
        }
        obj->numberfmt_->FormatToParts(number, numberParts);
    } else {
        int32_t errorCode = 0;
        double paramValue = JSUtils::GetNumberValue(env, param, errorCode);
        if (errorCode != 0) {
            HILOG_ERROR_I18N("GetFormatToPartsInner: Get Number object value failed");
            return JSUtils::CreateEmptyArray(env);
        }
        obj->numberfmt_->FormatToParts(paramValue, numberParts);
    }
    return SetNumberFormatParts(env, status, numberParts);
}

napi_value IntlNumberFormatAddon::SupportedLocalesOf(napi_env env, napi_callback_info info)
{
    size_t argc = 2;
    napi_value argv[2] = { nullptr };
    napi_value thisVar = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("IntlNumberFormatAddon::SupportedLocalesOf: Get parameter info failed");
        return JSUtils::CreateEmptyArray(env);
    }
    std::vector<std::string> localeTags;
    if (argc > 0) {
        int32_t code = 0;
        std::vector<std::string> localeArray = JSUtils::GetLocaleArray(env, argv[0], code);
        localeTags.assign(localeArray.begin(), localeArray.end());
    }
    std::map<std::string, std::string> map = {};
    if (argc > 1) {
        JSUtils::GetNumberOptionValues(env, argv[1], map);
    }
    I18nErrorCode i18nStatus = I18nErrorCode::SUCCESS;
    std::vector<std::string> resultLocales = NumberFormat::SupportedLocalesOf(localeTags, map, i18nStatus);
    if (i18nStatus == I18nErrorCode::INVALID_LOCALE_TAG) {
        HILOG_ERROR_I18N("IntlNumberFormatAddon::SupportedLocalesOf: SupportedLocalesOf status fail");
        ErrorUtil::NapiThrowUndefined(env, "invalid locale");
        return nullptr;
    } else if (i18nStatus != I18nErrorCode::SUCCESS) {
        ErrorUtil::NapiThrowUndefined(env, "getStringOption failed");
        return nullptr;
    }
    return JSUtils::CreateArray(env, resultLocales);
}

napi_value IntlNumberFormatAddon::SetNumberFormatParts(napi_env env, napi_status &status,
    const std::vector<std::vector<std::string>> &numberParts)
{
    napi_value result = nullptr;
    status = napi_create_array_with_length(env, numberParts.size(), &result);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("SetNumberFormatParts: Failed to create array");
        return JSUtils::CreateEmptyArray(env);
    }
    for (size_t i = 0; i < numberParts.size(); i++) {
        napi_value value = nullptr;
        status = napi_create_string_utf8(env, numberParts[i][1].c_str(), NAPI_AUTO_LENGTH, &value);
        if (status != napi_ok) {
            HILOG_ERROR_I18N("Failed to create string item numberParts[i][1].");
            return JSUtils::CreateEmptyArray(env);
        }
        napi_value type = nullptr;
        status = napi_create_string_utf8(env, numberParts[i][0].c_str(), NAPI_AUTO_LENGTH, &type);
        if (status != napi_ok) {
            HILOG_ERROR_I18N("Failed to create string item numberParts[i][0].");
            return JSUtils::CreateEmptyArray(env);
        }
        std::unordered_map<std::string, napi_value> propertys {
            { "type", type },
            { "value", value }
        };
        std::vector<std::string> keyVect = { "type", "value" };
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

napi_status IntlNumberFormatAddon::UnwrapIntlNumberFormat(napi_env env, napi_value value,
    IntlNumberFormatAddon** numberFormat)
{
    return napi_unwrap_s(env, value, &TYPE_TAG, reinterpret_cast<void **>(numberFormat));
}

std::shared_ptr<NumberFormat> IntlNumberFormatAddon::GetNumberFormat()
{
    return numberfmt_;
}
} // namespace I18n
} // namespace Global
} // namespace OHOS