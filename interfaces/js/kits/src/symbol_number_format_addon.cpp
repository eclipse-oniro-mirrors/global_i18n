/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "symbol_number_format_addon.h"

#include <unordered_set>
#include "error_util.h"
#include "i18n_hilog.h"
#include "intl_date_time_format_addon.h"
#include "intl_number_format_addon.h"
#include "js_lifecycle_managers.h"
#include "js_utils.h"
#include "locale_config.h"
#include "utils.h"
#include "variable_convertor.h"

namespace OHOS {
namespace Global {
namespace I18n {
static thread_local ThreadReference* g_SymbolNumberFormatConstructor = nullptr;

SymbolNumberFormatAddon::SymbolNumberFormatAddon()
{
}

SymbolNumberFormatAddon::~SymbolNumberFormatAddon()
{
}

void SymbolNumberFormatAddon::Destructor(napi_env env, void *nativeObject, void *hint)
{
    if (!nativeObject) {
        return;
    }
    delete reinterpret_cast<SymbolNumberFormatAddon *>(nativeObject);
    nativeObject = nullptr;
}

napi_value SymbolNumberFormatAddon::InitSymbolNumberFormat(napi_env env, napi_value exports)
{
    HandleScope scope(env, "InitSymbolNumberFormat");
    if (!scope.IsOpen()) {
        return nullptr;
    }

    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("format", Format),
        DECLARE_NAPI_FUNCTION("formatToParts", FormatToParts),
        DECLARE_NAPI_FUNCTION("formatRange", FormatRange),
        DECLARE_NAPI_FUNCTION("formatRangeToParts", FormatRangeToParts),
        DECLARE_NAPI_FUNCTION("parse", Parse),
        DECLARE_NAPI_FUNCTION("toString", ToString),
        DECLARE_NAPI_FUNCTION("resolvedOptions", ResolvedOptions),
    };
    napi_value symbolConstructor = nullptr;
    napi_status status = napi_define_class(env, "SymbolNumberFormat", NAPI_AUTO_LENGTH, Constructor, nullptr,
        sizeof(properties) / sizeof(napi_property_descriptor), properties, &symbolConstructor);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("SymbolNumberFormatAddon: Define class SymbolNumberFormat failed.");
        return nullptr;
    }

    g_SymbolNumberFormatConstructor = ThreadReference::CreateInstance(env, symbolConstructor, 1);
    if (!g_SymbolNumberFormatConstructor) {
        HILOG_ERROR_I18N("SymbolNumberFormatAddon: Failed to create SymbolNumberFormat ref at init");
        return nullptr;
    }

    status = napi_set_named_property(env, exports, "SymbolNumberFormat", symbolConstructor);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("SymbolNumberFormatAddon: Set property SymbolNumberFormat failed.");
        return nullptr;
    }
    return exports;
}

napi_value SymbolNumberFormatAddon::Constructor(napi_env env, napi_callback_info info)
{
    size_t argc = 2;
    napi_value argv[2] = { nullptr };
    napi_value thisVar = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("SymbolNumberFormatAddon::constructor: Get callback info error.");
        return nullptr;
    }

    std::vector<std::string> localeTags;
    if (argc >= 1 && !VariableConvertor::IsUndefined(env, argv[0])) {
        if (VariableConvertor::GetLocaleType(env, argv[0]) != LocaleType::BUILTINS_LOCALE) {
            ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, "locale", "Locale", true);
            return nullptr;
        }
        std::string localeTag = VariableConvertor::ParseBuiltinsLocale(env, argv[0]);
        if (!localeTag.empty()) {
            localeTags.emplace_back(localeTag);
        }
    }
    if (localeTags.size() == 0) {
        localeTags.emplace_back(LocaleConfig::GetEffectiveLocale());
    }

    std::map<std::string, std::string> options;
    if (argc > 1) {
        ParseOptions(env, argv[1], options);
    }

    std::unique_ptr<SymbolNumberFormatAddon> obj = std::make_unique<SymbolNumberFormatAddon>();
    status = napi_wrap_s(env, thisVar, reinterpret_cast<void *>(obj.get()), Destructor, nullptr, &TYPE_TAG, nullptr);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("SymbolNumberFormatAddon::constructor: napi_wrap error.");
        return nullptr;
    }
    if (!obj->InitSymbolFormatContext(env, localeTags, options)) {
        return nullptr;
    }
    obj.release();
    return thisVar;
}

void SymbolNumberFormatAddon::ParseOptions(napi_env env, napi_value optionsValue,
    std::map<std::string, std::string>& options)
{
    napi_valuetype type = napi_undefined;
    napi_status status = napi_typeof(env, optionsValue, &type);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("SymbolNumberFormatAddon::ParseOptions: Get options type failed.");
        return;
    }
    if (type != napi_object) {
        return;
    }

    std::string value;
    std::unordered_set<std::string> keyOfTypeString = { "currency", "currencySign", "currencyDisplay", "unit",
        "unitDisplay", "compactDisplay", "signDisplay", "localeMatcher", "style", "numberingSystem", "notation",
        "unitUsage", "roundingPriority", "roundingMode", "groupingSeparator", "infinity", "plusSign", "minusSign",
        "nan", "zero" };
    for (const auto& key : keyOfTypeString) {
        if (JSUtils::GetPropertyFormObject(env, optionsValue, key, napi_string, value)) {
            options.insert(std::make_pair(key, value));
        }
    }

    if (JSUtils::GetPropertyFormObject(env, optionsValue, "useGrouping", napi_boolean, value)) {
        options.insert(std::make_pair("useGrouping", value));
    }

    std::unordered_set<std::string> keyOfTypeNumber = { "minimumIntegerDigits", "minimumFractionDigits",
        "maximumFractionDigits", "minimumSignificantDigits", "maximumSignificantDigits", "roundingIncrement" };
    for (const auto& key : keyOfTypeNumber) {
        if (JSUtils::GetPropertyFormObject(env, optionsValue, key, napi_number, value)) {
            options.insert(std::make_pair(key, value));
        }
    }
}

bool SymbolNumberFormatAddon::InitSymbolFormatContext(napi_env env, const std::vector<std::string>& localeTags,
    std::map<std::string, std::string>& options)
{
    std::string errMessage;
    int32_t code = 0;
    symbolNumberFormat_ = std::make_shared<SymbolNumberFormat>(localeTags, options, errMessage, code);
    if (!errMessage.empty() || code != 0) {
        ErrorUtil::NapiThrowUndefined(env, errMessage);
        return false;
    }
    return true;
}

napi_value SymbolNumberFormatAddon::Format(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = { nullptr };
    napi_value thisVar = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("SymbolNumberFormatAddon::Format: get cb info failed.");
        return VariableConvertor::CreateString(env, "");
    }
    if (argc < 1) {
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, "value", "", true);
        return VariableConvertor::CreateString(env, "");
    }
    NumberDataType type;
    NumberData data = ParseNumberData(env, argv[0], type);

    SymbolNumberFormatAddon* obj = nullptr;
    status = napi_unwrap_s(env, thisVar, &TYPE_TAG, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->symbolNumberFormat_) {
        HILOG_ERROR_I18N("SymbolNumberFormatAddon::Format: unwrap SymbolNumberFormatAddon failed.");
        return VariableConvertor::CreateString(env, "");
    }

    std::string result;
    switch (type) {
        case NumberDataType::NUMBER_TYPE:
            result = obj->symbolNumberFormat_->Format(data.valueNumber);
            break;
        case NumberDataType::BIGINT_TYPE:
            result = obj->symbolNumberFormat_->FormatBigInt(data.valueBigint);
            break;
        default:
            HILOG_ERROR_I18N("SymbolNumberFormatAddon::Format: Invalid data.");
    }
    return VariableConvertor::CreateString(env, result);
}

napi_value SymbolNumberFormatAddon::FormatToParts(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = { nullptr };
    napi_value thisVar = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("SymbolNumberFormatAddon::FormatToParts: get cb info failed.");
        return nullptr;
    }
    
    SymbolNumberFormatAddon* obj = nullptr;
    status = napi_unwrap_s(env, thisVar, &TYPE_TAG, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->symbolNumberFormat_) {
        HILOG_ERROR_I18N("SymbolNumberFormatAddon::FormatToParts: unwrap SymbolNumberFormatAddon failed.");
        return nullptr;
    }
    NumberFormatParts parts;
    if (argc < 1 || VariableConvertor::IsUndefined(env, argv[0])) {
        parts = obj->symbolNumberFormat_->FormatToPartsDouble(uprv_getNaN());
        return IntlDateTimeFormatAddon::CreateFormatPart(env, parts);
    }
    NumberDataType type;
    NumberData data = ParseNumberData(env, argv[0], type);
    switch (type) {
        case NumberDataType::NUMBER_TYPE:
            parts = obj->symbolNumberFormat_->FormatToPartsDouble(data.valueNumber);
            break;
        case NumberDataType::BIGINT_TYPE:
            parts = obj->symbolNumberFormat_->FormatToPartsDecStr(data.valueBigint);
            break;
        default:
            HILOG_ERROR_I18N("SymbolNumberFormatAddon::FormatToParts: Invalid data.");
    }
    return IntlDateTimeFormatAddon::CreateFormatPart(env, parts);
}

napi_value SymbolNumberFormatAddon::FormatRange(napi_env env, napi_callback_info info)
{
    size_t argc = 2;
    napi_value argv[2] = { nullptr };
    napi_value thisVar = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("SymbolNumberFormatAddon::FormatRange: get cb info failed.");
        return nullptr;
    }
    if (argc <= 1) {
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, "startRange or endRange", "", true);
        return VariableConvertor::CreateString(env, "");
    }
    NumberDataType startRangeType;
    NumberData startRangeData = ParseNumberData(env, argv[0], startRangeType);
    NumberDataType endRangeType;
    NumberData endRangeData = ParseNumberData(env, argv[1], endRangeType);
    if (startRangeType != NumberDataType::NUMBER_TYPE || endRangeType != NumberDataType::NUMBER_TYPE) {
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, "startRange or endRange", "number", true);
        return VariableConvertor::CreateString(env, "");
    }

    SymbolNumberFormatAddon* obj = nullptr;
    status = napi_unwrap_s(env, thisVar, &TYPE_TAG, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->symbolNumberFormat_) {
        HILOG_ERROR_I18N("SymbolNumberFormatAddon::FormatRange: unwrap SymbolNumberFormatAddon failed.");
        return nullptr;
    }

    std::string result =
        obj->symbolNumberFormat_->FormatRangeDoubleDouble(startRangeData.valueNumber, endRangeData.valueNumber);
    return JSUtils::CreateString(env, result);
}

napi_value SymbolNumberFormatAddon::FormatRangeToParts(napi_env env, napi_callback_info info)
{
    size_t argc = 2;
    napi_value argv[2] = { nullptr };
    napi_value thisVar = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("SymbolNumberFormatAddon::FormatRangeToParts: get cb info failed.");
        return nullptr;
    }
    if (argc <= 1) {
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, "startRange or endRange", "", true);
        return VariableConvertor::CreateString(env, "");
    }
    NumberDataType startRangeType;
    NumberData startRangeData = ParseNumberData(env, argv[0], startRangeType);
    NumberDataType endRangeType;
    NumberData endRangeData = ParseNumberData(env, argv[1], endRangeType);
    if (startRangeType != NumberDataType::NUMBER_TYPE || endRangeType != NumberDataType::NUMBER_TYPE) {
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, "startRange or endRange", "number", true);
        return VariableConvertor::CreateString(env, "");
    }

    SymbolNumberFormatAddon* obj = nullptr;
    status = napi_unwrap_s(env, thisVar, &TYPE_TAG, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->symbolNumberFormat_) {
        HILOG_ERROR_I18N("SymbolNumberFormatAddon::FormatRange: unwrap SymbolNumberFormatAddon failed.");
        return nullptr;
    }
    NumberFormatParts parts = obj->symbolNumberFormat_->FormatToRangePartsDoubleDouble(
        startRangeData.valueNumber, endRangeData.valueNumber);
    return IntlDateTimeFormatAddon::CreateFormatPart(env, parts);
}

napi_value SymbolNumberFormatAddon::ToString(napi_env env, napi_callback_info info)
{
    return JSUtils::CreateString(env, "[object i18n.SymbolNumberFormat]");
}

napi_value SymbolNumberFormatAddon::ResolvedOptions(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_status status = napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("SymbolNumberFormatAddon::ResolvedOptions: get cb info failed.");
        return nullptr;
    }

    SymbolNumberFormatAddon* obj = nullptr;
    status = napi_unwrap_s(env, thisVar, &TYPE_TAG, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->symbolNumberFormat_) {
        HILOG_ERROR_I18N("SymbolNumberFormatAddon::ResolvedOptions: unwrap SymbolNumberFormatAddon failed.");
        return nullptr;
    }

    std::map<std::string, std::string> options;
    obj->symbolNumberFormat_->ResolvedOptions(options);
    return CreateResolvedOptions(env, options);
}

NumberData SymbolNumberFormatAddon::ParseNumberData(napi_env env, napi_value value, NumberDataType& type)
{
    NumberData data;
    napi_valuetype valueType = napi_valuetype::napi_undefined;
    napi_status status = napi_typeof(env, value, &valueType);
    if (status != napi_ok) {
        type = NumberDataType::INVALID_DATA;
        return data;
    }
    if (valueType == napi_valuetype::napi_bigint) {
        int32_t code = 0;
        data.valueBigint = JSUtils::GetBigIntStr(env, value, code);
        if (code != 0) {
            type = NumberDataType::INVALID_DATA;
            HILOG_ERROR_I18N("SymbolNumberFormatAddon::ParseNumberData: Get bigint value failed.");
            return data;
        }
        type = NumberDataType::BIGINT_TYPE;
    } else if (valueType == napi_valuetype::napi_number) {
        status = napi_get_value_double(env, value, &data.valueNumber);
        if (status != napi_ok) {
            type = NumberDataType::INVALID_DATA;
            HILOG_ERROR_I18N("SymbolNumberFormatAddon::ParseNumberData: Get double value failed.");
            return data;
        }
        type = NumberDataType::NUMBER_TYPE;
    } else {
        int32_t code = 0;
        data.valueNumber = JSUtils::GetNumberValue(env, value, code);
        if (code != 0) {
            type = NumberDataType::INVALID_DATA;
            HILOG_ERROR_I18N("SymbolNumberFormatAddon::ParseNumberData: Get Number value failed");
            return data;
        }
        type = NumberDataType::NUMBER_TYPE;
    }
    return data;
}

napi_value SymbolNumberFormatAddon::CreateResolvedOptions(napi_env env,
    const std::map<std::string, std::string>& options)
{
    napi_value object = nullptr;
    napi_status status = napi_create_object(env, &object);
    if (status != napi_ok || object == nullptr) {
        HILOG_ERROR_I18N("SymbolNumberFormatAddon::CreateResolvedOptions: Create object failed.");
        return nullptr;
    }

    std::unordered_set<std::string> keyOfTypeNumber = { "minimumIntegerDigits", "minimumFractionDigits",
        "maximumFractionDigits", "minimumSignificantDigits", "maximumSignificantDigits", "roundingIncrement" };

    for (auto& option : options) {
        std::string key = option.first;
        std::string value = option.second;
        napi_value propertyValue = nullptr;
        if (key.compare("useGrouping") == 0) {
            bool boolValue = (value.compare("true") == 0) ? true : false;
            status = napi_get_boolean(env, boolValue, &propertyValue);
        } else if (keyOfTypeNumber.find(key) != keyOfTypeNumber.end()) {
            int32_t code = 0;
            int32_t intValue = ConvertString2Int(value, code);
            if (code != 0) {
                HILOG_ERROR_I18N("IntlDateTimeFormatAddon::CreateResolvedOptions: Convert string failed.");
                continue;
            }
            status = napi_create_int32(env, intValue, &propertyValue);
        } else {
            status = napi_create_string_utf8(env, value.c_str(), NAPI_AUTO_LENGTH, &propertyValue);
        }
        if (status != napi_ok || propertyValue == nullptr) {
            HILOG_ERROR_I18N("IntlDateTimeFormatAddon::CreateResolvedOptions: Create propertyValue failed.");
            continue;
        }
        status = napi_set_named_property(env, object, key.c_str(), propertyValue);
        if (status != napi_ok) {
            HILOG_ERROR_I18N("IntlDateTimeFormatAddon::CreateResolvedOptions: Set property failed.");
        }
    }
    return object;
}

std::shared_ptr<SymbolNumberFormat> SymbolNumberFormatAddon::GetNumberFormat()
{
    return symbolNumberFormat_;
}

bool SymbolNumberFormatAddon::IsSymbolNumberFormat(napi_env env, napi_value argv)
{
    if (g_SymbolNumberFormatConstructor == nullptr) {
        HILOG_ERROR_I18N("SymbolNumberFormatAddon::IsSymbolNumberFormat: "
            "g_SymbolNumberFormatConstructor is nullptr.");
        return false;
    }

    napi_value constructor = nullptr;
    if (!g_SymbolNumberFormatConstructor->GetReferenceValue(env, &constructor)) {
        HILOG_ERROR_I18N("SymbolNumberFormatAddon::IsSymbolNumberFormat: Failed to create reference.");
        return false;
    }

    bool isSymbolNumberFormat = false;
    napi_status status = napi_instanceof(env, argv, constructor, &isSymbolNumberFormat);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("SymbolNumberFormatAddon::IsSymbolNumberFormat: Failed to get instance of argv.");
        return false;
    }

    return isSymbolNumberFormat;
}

napi_status SymbolNumberFormatAddon::UnwrapNumberFormat(napi_env env, napi_value value,
    SymbolNumberFormatAddon** numberFormat)
{
    return napi_unwrap_s(env, value, &TYPE_TAG, reinterpret_cast<void **>(numberFormat));
}

namespace {
constexpr int32_t MIN_PARSE_PARAMS = 2;  // text and lenientMode
}

bool SymbolNumberFormatAddon::GetParseParams(napi_env env, napi_callback_info info,
    std::string& text, bool& lenientMode, napi_value& thisVar)
{
    size_t argc = MIN_PARSE_PARAMS;
    napi_value argv[MIN_PARSE_PARAMS] = { nullptr };
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("GetParseParams: napi_get_cb_info failed, status: %{public}d", status);
        return false;
    }
    if (argc < MIN_PARSE_PARAMS) {
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, "text or lenientMode", "", true);
        return false;
    }
    
    napi_valuetype valueType = napi_undefined;
    status = napi_typeof(env, argv[0], &valueType);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("GetParseParams: napi_typeof text failed, status: %{public}d", status);
        return false;
    }
    if (valueType != napi_string) {
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, "text", "string", true);
        return false;
    }
    
    int32_t code = 0;
    text = VariableConvertor::GetString(env, argv[0], code);
    if (code != 0) {
        HILOG_ERROR_I18N("GetParseParams: GetString failed, code: %{public}d", code);
        return false;
    }
    
    status = napi_typeof(env, argv[1], &valueType);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("GetParseParams: napi_typeof lenientMode failed, status: %{public}d", status);
        return false;
    }
    if (valueType != napi_boolean) {
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, "lenientMode", "boolean", true);
        return false;
    }
    
    status = napi_get_value_bool(env, argv[1], &lenientMode);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("GetParseParams: napi_get_value_bool failed, status: %{public}d", status);
        return false;
    }
    
    return true;
}

bool SymbolNumberFormatAddon::UnwrapParseObject(napi_env env, napi_value thisVar,
    SymbolNumberFormatAddon*& obj)
{
    napi_status status = napi_unwrap_s(env, thisVar, &TYPE_TAG, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->symbolNumberFormat_) {
        HILOG_ERROR_I18N("UnwrapParseObject: unwrap failed, status: %{public}d", status);
        return false;
    }
    return true;
}

bool SymbolNumberFormatAddon::HandleParseResult(napi_env env, I18nErrorCode errCode)
{
    if (errCode == I18nErrorCode::FAILED) {
        HILOG_ERROR_I18N("HandleParseResult: Framework returned FAILED (system error).");
        return false;
    }
    if (errCode == I18nErrorCode::INVALID_PARAM) {
        ErrorUtil::NapiThrow(env, I18N_NOT_VALID_NEW, "text", "a valid localized number string", true);
        return false;
    }
    return true;
}

napi_value SymbolNumberFormatAddon::Parse(napi_env env, napi_callback_info info)
{
    std::string text;
    bool lenientMode = false;
    napi_value thisVar = nullptr;
    
    if (!GetParseParams(env, info, text, lenientMode, thisVar)) {
        return VariableConvertor::CreateDoubleValue(env, 0);
    }
    
    SymbolNumberFormatAddon* obj = nullptr;
    if (!UnwrapParseObject(env, thisVar, obj)) {
        return VariableConvertor::CreateDoubleValue(env, 0);
    }
    
    I18nErrorCode errCode = I18nErrorCode::SUCCESS;
    double result = obj->symbolNumberFormat_->Parse(text, lenientMode, errCode);
    
    if (!HandleParseResult(env, errCode)) {
        return VariableConvertor::CreateDoubleValue(env, 0);
    }
    
    return VariableConvertor::CreateDoubleValue(env, result);
}
} // namespace I18n
} // namespace Global
} // namespace OHOS
