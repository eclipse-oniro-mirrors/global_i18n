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

#include "symbol_date_time_format_addon.h"

#include <cstdint>
#include <unordered_set>
#include "error_util.h"
#include "i18n_hilog.h"
#include "intl_date_time_format_addon.h"
#include "js_lifecycle_managers.h"
#include "js_utils.h"
#include "locale_config.h"
#include "variable_convertor.h"

namespace OHOS {
namespace Global {
namespace I18n {
constexpr int32_t PARSE_PARAM_COUNT = 2;
constexpr int32_t TEXT_PARAM_INDEX = 0;
constexpr int32_t LENIENT_MODE_PARAM_INDEX = 1;

static thread_local ThreadReference* g_SymbolDateTimeFormatConstructor = nullptr;
SymbolDateTimeFormatAddon::SymbolDateTimeFormatAddon()
{
}

SymbolDateTimeFormatAddon::~SymbolDateTimeFormatAddon()
{
}

void SymbolDateTimeFormatAddon::Destructor(napi_env env, void *nativeObject, void *hint)
{
    if (!nativeObject) {
        return;
    }
    delete reinterpret_cast<SymbolDateTimeFormatAddon *>(nativeObject);
    nativeObject = nullptr;
}

napi_value SymbolDateTimeFormatAddon::InitSymbolDateTimeFormat(napi_env env, napi_value exports)
{
    HandleScope scope(env, "InitSymbolDateTimeFormat");
    if (!scope.IsOpen()) {
        return nullptr;
    }

    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("format", Format),
        DECLARE_NAPI_FUNCTION("formatToParts", FormatToParts),
        DECLARE_NAPI_FUNCTION("formatRange", FormatRange),
        DECLARE_NAPI_FUNCTION("formatRangeToParts", FormatRangeToParts),
        DECLARE_NAPI_FUNCTION("toString", ToString),
        DECLARE_NAPI_FUNCTION("resolvedOptions", ResolvedOptions),
        DECLARE_NAPI_FUNCTION("parse", Parse),
    };
    napi_value symbolConstructor = nullptr;
    napi_status status = napi_define_class(env, "SymbolDateTimeFormat", NAPI_AUTO_LENGTH, Constructor, nullptr,
        sizeof(properties) / sizeof(napi_property_descriptor), properties, &symbolConstructor);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("SymbolDateTimeFormatAddon: Define class SymbolDateTimeFormat failed.");
        return nullptr;
    }

    g_SymbolDateTimeFormatConstructor = ThreadReference::CreateInstance(env, symbolConstructor, 1);
    if (!g_SymbolDateTimeFormatConstructor) {
        HILOG_ERROR_I18N("SymbolDateTimeFormatAddon: Failed to create SymbolDateTimeFormat ref at init");
        return nullptr;
    }

    status = napi_set_named_property(env, exports, "SymbolDateTimeFormat", symbolConstructor);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("SymbolDateTimeFormatAddon: Set property SymbolDateTimeFormat failed.");
        return nullptr;
    }
    return exports;
}

napi_value SymbolDateTimeFormatAddon::Constructor(napi_env env, napi_callback_info info)
{
    size_t argc = 2;
    napi_value argv[2] = { nullptr };
    napi_value thisVar = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("SymbolDateTimeFormatAddon::constructor: Get callback info error.");
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

    std::unordered_map<std::string, std::string> options;
    if (argc > 1) {
        ParseOptions(env, argv[1], options);
    }

    std::unique_ptr<SymbolDateTimeFormatAddon> obj = std::make_unique<SymbolDateTimeFormatAddon>();
    status = napi_wrap_s(env, thisVar, reinterpret_cast<void *>(obj.get()), Destructor, nullptr, &TYPE_TAG, nullptr);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("SymbolDateTimeFormatAddon::constructor: napi_wrap error.");
        return nullptr;
    }
    if (!obj->InitSymbolFormatContext(env, localeTags, options)) {
        return nullptr;
    }
    obj.release();
    return thisVar;
}

void SymbolDateTimeFormatAddon::ParseOptions(napi_env env, napi_value optionsValue,
    std::unordered_map<std::string, std::string>& options)
{
    napi_valuetype type = napi_undefined;
    napi_status status = napi_typeof(env, optionsValue, &type);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("SymbolDateTimeFormatAddon::ParseOptions: Get options type failed.");
        return;
    }
    if (type != napi_object) {
        return;
    }
    std::string value;
    std::unordered_set<std::string> keyOfTypeString = { IntlDateTimeFormat::LOCALE_MATCHER_TAG,
        IntlDateTimeFormat::WEEK_DAY_TAG, IntlDateTimeFormat::ERA_TAG, IntlDateTimeFormat::YEAR_TAG,
        IntlDateTimeFormat::MONTH_TAG, IntlDateTimeFormat::DAY_TAG, IntlDateTimeFormat::HOUR_TAG,
        IntlDateTimeFormat::MINUTE_TAG, IntlDateTimeFormat::SECOND_TAG, IntlDateTimeFormat::TIME_ZONE_NAME_TAG,
        IntlDateTimeFormat::FORMAT_MATCHER_TAG, IntlDateTimeFormat::TIME_ZONE_TAG, IntlDateTimeFormat::DATE_STYLE_TAG,
        IntlDateTimeFormat::TIME_STYLE_TAG, IntlDateTimeFormat::HOUR_CYCLE_TAG, IntlDateTimeFormat::DAY_PERIOD_TAG,
        IntlDateTimeFormat::CALENDAR_TAG, IntlDateTimeFormat::NUMBERING_SYSTEM_TAG
    };
    for (const auto& key : keyOfTypeString) {
        if (JSUtils::GetPropertyFormObject(env, optionsValue, key, napi_string, value)) {
            options.insert(std::make_pair(key, value));
        }
    }
    if (JSUtils::GetPropertyFormObject(env, optionsValue, IntlDateTimeFormat::HOUR12_TAG, napi_boolean, value)) {
        options.insert({IntlDateTimeFormat::HOUR12_TAG, value});
    }
    if (JSUtils::GetPropertyFormObject(env, optionsValue, IntlDateTimeFormat::FRACTIONAL_SECOND_DIGITS_TAG, napi_number,
        value)) {
        options.insert({IntlDateTimeFormat::FRACTIONAL_SECOND_DIGITS_TAG, value});
    }
    ParseAmPmSymbol(env, optionsValue, options);
}

bool SymbolDateTimeFormatAddon::InitSymbolFormatContext(napi_env env, const std::vector<std::string>& localeTags,
    const std::unordered_map<std::string, std::string>& options)
{
    std::string errMessage;
    symbolDateTimeFormat_ = std::make_shared<SymbolDateTimeFormat>(localeTags, options, errMessage);
    if (!errMessage.empty()) {
        ErrorUtil::NapiThrowUndefined(env, errMessage);
        return false;
    }
    return symbolDateTimeFormat_ != nullptr;
}

napi_value SymbolDateTimeFormatAddon::Format(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = { nullptr };
    napi_value thisVar = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("SymbolDateTimeFormatAddon::Format: get cb info failed.");
        return VariableConvertor::CreateString(env, "");
    }
    DateType type = DateType::INVALID;
    if (argc >= 1) {
        type = VariableConvertor::GetDateType(env, argv[0]);
        if (type != DateType::DATE_OBJECT && type != DateType::NUMBER) {
            ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, "date", "Date or number", true);
            return nullptr;
        }
    }
    double milliseconds = 0;
    if (argc < 1 || !VariableConvertor::GetTimeFromDate(env, argv[0], type, milliseconds)) {
        std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
        );
        milliseconds = ms.count();
    }

    SymbolDateTimeFormatAddon* obj = nullptr;
    status = napi_unwrap_s(env, thisVar, &TYPE_TAG, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->symbolDateTimeFormat_) {
        HILOG_ERROR_I18N("SymbolDateTimeFormatAddon::Format: unwrap SymbolDateTimeFormatAddon failed.");
        return VariableConvertor::CreateString(env, "");
    }
    std::string result = obj->symbolDateTimeFormat_->Format(milliseconds);
    return VariableConvertor::CreateString(env, result);
}

napi_value SymbolDateTimeFormatAddon::FormatToParts(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = { nullptr };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("SymbolDateTimeFormatAddon::FormatToParts: get cb info failed.");
        return VariableConvertor::CreateString(env, "");
    }
    DateType type = DateType::INVALID;
    if (argc >= 1) {
        type = VariableConvertor::GetDateType(env, argv[0]);
        if (type != DateType::DATE_OBJECT && type != DateType::NUMBER) {
            ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, "date", "Date or number", true);
            return nullptr;
        }
    }
    double milliseconds = 0;
    if (argc < 1 || !VariableConvertor::GetTimeFromDate(env, argv[0], type, milliseconds)) {
        std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
        );
        milliseconds = ms.count();
    }

    SymbolDateTimeFormatAddon* obj = nullptr;
    status = napi_unwrap_s(env, thisVar, &TYPE_TAG, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->symbolDateTimeFormat_) {
        HILOG_ERROR_I18N("SymbolDateTimeFormatAddon::FormatToParts: unwrap SymbolDateTimeFormatAddon failed.");
        return VariableConvertor::CreateString(env, "");
    }
    std::string errMessage;
    auto parts = obj->symbolDateTimeFormat_->FormatToParts(milliseconds, errMessage);
    if (!errMessage.empty()) {
        ErrorUtil::NapiThrowUndefined(env, errMessage);
        return nullptr;
    }
    return IntlDateTimeFormatAddon::CreateFormatPart(env, parts);
}

napi_value SymbolDateTimeFormatAddon::FormatRange(napi_env env, napi_callback_info info)
{
    size_t argc = 2;
    napi_value argv[2] = { nullptr };
    napi_value thisVar = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("SymbolDateTimeFormatAddon::FormatRange: get cb info failed.");
        return VariableConvertor::CreateString(env, "");
    }
    if (argc <= 1) {
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, "startDate or endDate", "", true);
        return VariableConvertor::CreateString(env, "");
    }
    DateType startDateType = VariableConvertor::GetDateType(env, argv[0]);
    if (startDateType != DateType::DATE_OBJECT && startDateType != DateType::NUMBER &&
        startDateType != DateType::BIGINT) {
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, "startDate", "Date, number or bigint", true);
        return nullptr;
    }
    DateType endDateType = VariableConvertor::GetDateType(env, argv[1]);
    if (endDateType != DateType::DATE_OBJECT && endDateType != DateType::NUMBER &&
        endDateType != DateType::BIGINT) {
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, "endDate", "Date, number or bigint", true);
        return nullptr;
    }
    double startDate = 0;
    double endDate = 0;
    if (!VariableConvertor::GetTimeFromDate(env, argv[0], startDateType, startDate) ||
        !VariableConvertor::GetTimeFromDate(env, argv[1], endDateType, endDate)) {
        HILOG_ERROR_I18N("SymbolDateTimeFormatAddon::FormatRange: Get time failed.");
        return VariableConvertor::CreateString(env, "");
    }
    SymbolDateTimeFormatAddon* obj = nullptr;
    status = napi_unwrap_s(env, thisVar, &TYPE_TAG, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->symbolDateTimeFormat_) {
        HILOG_ERROR_I18N("SymbolDateTimeFormatAddon::FormatRange: unwrap SymbolDateTimeFormatAddon failed.");
        return VariableConvertor::CreateString(env, "");
    }
    std::string errMessage;
    std::string result = obj->symbolDateTimeFormat_->FormatRange(startDate, endDate, errMessage);
    if (!errMessage.empty()) {
        ErrorUtil::NapiThrowUndefined(env, errMessage);
        return nullptr;
    }
    return VariableConvertor::CreateString(env, result);
}

napi_value SymbolDateTimeFormatAddon::FormatRangeToParts(napi_env env, napi_callback_info info)
{
    size_t argc = 2;
    napi_value argv[2] = { nullptr };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("SymbolDateTimeFormatAddon::FormatRangeToParts: get cb info failed.");
        return JSUtils::CreateEmptyArray(env);
    }
    if (argc <= 1) {
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, "startDate or endDate", "", true);
        return JSUtils::CreateEmptyArray(env);
    }
    DateType startDateType = VariableConvertor::GetDateType(env, argv[0]);
    if (startDateType != DateType::DATE_OBJECT && startDateType != DateType::NUMBER &&
        startDateType != DateType::BIGINT) {
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, "startDate", "Date, number or bigint", true);
        return nullptr;
    }
    DateType endDateType = VariableConvertor::GetDateType(env, argv[1]);
    if (endDateType != DateType::DATE_OBJECT && endDateType != DateType::NUMBER &&
        endDateType != DateType::BIGINT) {
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, "endDate", "Date, number or bigint", true);
        return nullptr;
    }
    double startDate = 0;
    double endDate = 0;
    if (!VariableConvertor::GetTimeFromDate(env, argv[0], startDateType, startDate) ||
        !VariableConvertor::GetTimeFromDate(env, argv[1], endDateType, endDate)) {
        HILOG_ERROR_I18N("SymbolDateTimeFormatAddon::FormatRangeToParts: Get time failed.");
        return JSUtils::CreateEmptyArray(env);
    }
    SymbolDateTimeFormatAddon* obj = nullptr;
    status = napi_unwrap_s(env, thisVar, &TYPE_TAG, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->symbolDateTimeFormat_) {
        HILOG_ERROR_I18N("SymbolDateTimeFormatAddon::FormatRangeToParts: unwrap SymbolDateTimeFormatAddon failed.");
        return JSUtils::CreateEmptyArray(env);
    }
    std::string errMessage;
    auto parts = obj->symbolDateTimeFormat_->FormatRangeToParts(startDate, endDate, errMessage);
    if (!errMessage.empty()) {
        ErrorUtil::NapiThrowUndefined(env, errMessage);
        return nullptr;
    }
    return IntlDateTimeFormatAddon::CreateFormatPart(env, parts);
}

napi_value SymbolDateTimeFormatAddon::ToString(napi_env env, napi_callback_info info)
{
    return JSUtils::CreateString(env, "[object i18n.SymbolDateTimeFormat]");
}

napi_value SymbolDateTimeFormatAddon::ResolvedOptions(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_status status = napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("SymbolDateTimeFormatAddon::ResolvedOptions: get cb info failed.");
        return JSUtils::CreateEmptyObject(env);
    }
    SymbolDateTimeFormatAddon* obj = nullptr;
    status = napi_unwrap_s(env, thisVar, &TYPE_TAG, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->symbolDateTimeFormat_) {
        HILOG_ERROR_I18N("SymbolDateTimeFormatAddon::ResolvedOptions: unwrap SymbolDateTimeFormatAddon failed.");
        return JSUtils::CreateEmptyObject(env);
    }
    std::unordered_map<std::string, std::string> options;
    obj->symbolDateTimeFormat_->ResolvedOptions(options);
    napi_value amPMSymbol = ResolvedAmPmSymbol(env, options);
    napi_value result = IntlDateTimeFormatAddon::CreateResolvedOptions(env, options);
    status = napi_set_named_property(env, result, "amPMSymbol", amPMSymbol);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("SymbolDateTimeFormatAddon::ResolvedOptions: Set amPMSymbol failed.");
        return result;
    }
    return result;
}

void SymbolDateTimeFormatAddon::ParseAmPmSymbol(napi_env env, napi_value optionsValue,
    std::unordered_map<std::string, std::string>& options)
{
    int32_t code = 0;
    napi_value propertyValue = JSUtils::GetNapiPropertyFormObject(env, optionsValue, "amPMSymbol", code);
    if (code != 0) {
        return;
    }
    std::vector<std::string> symbol;
    if (!VariableConvertor::GetStringArrayFromJsParam(env, propertyValue, "amPMSymbol", symbol)) {
        HILOG_ERROR_I18N("SymbolDateTimeFormatAddon::ParseAmPmSymbol: GetStringArrayFromJsParam failed.");
        return;
    }
    if (symbol.size() < SymbolDateTimeFormat::AM_PM_SYMBOLS_LEN) {
        ErrorUtil::NapiThrow(env, I18N_NOT_VALID, "amPMSymbol", "a string[] with two or more elements", true);
        return;
    }
    options.insert(std::make_pair(SymbolDateTimeFormat::AM_SYMBOL_TAG, symbol[0]));
    options.insert(std::make_pair(SymbolDateTimeFormat::PM_SYMBOL_TAG, symbol[1]));
}

napi_value SymbolDateTimeFormatAddon::ResolvedAmPmSymbol(napi_env env,
    std::unordered_map<std::string, std::string>& options)
{
    auto amIter = options.find(SymbolDateTimeFormat::AM_SYMBOL_TAG);
    if (amIter == options.end()) {
        return nullptr;
    }
    std::string amSymbol = amIter->second;
    options.erase(amIter);
    auto pmIter = options.find(SymbolDateTimeFormat::PM_SYMBOL_TAG);
    if (pmIter == options.end()) {
        return nullptr;
    }
    std::string pmSymbol = pmIter->second;
    options.erase(pmIter);

    napi_value result = nullptr;
    napi_status status = napi_create_array_with_length(env, SymbolDateTimeFormat::AM_PM_SYMBOLS_LEN, &result);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("SymbolDateTimeFormatAddon::ResolvedAmPmSymbol: Create array failed.");
        return nullptr;
    }
    status = napi_set_element(env, result, 0, VariableConvertor::CreateString(env, amSymbol));
    if (status != napi_ok) {
        HILOG_ERROR_I18N("SymbolDateTimeFormatAddon::ResolvedAmPmSymbol: Set am failed.");
        return nullptr;
    }
    status = napi_set_element(env, result, 1, VariableConvertor::CreateString(env, pmSymbol));
    if (status != napi_ok) {
        HILOG_ERROR_I18N("SymbolDateTimeFormatAddon::ResolvedAmPmSymbol: Set pm failed.");
        return nullptr;
    }
    return result;
}

std::shared_ptr<SymbolDateTimeFormat> SymbolDateTimeFormatAddon::GetDateTimeFormat()
{
    return symbolDateTimeFormat_;
}

bool SymbolDateTimeFormatAddon::IsSymbolDateTimeFormat(napi_env env, napi_value argv)
{
    if (g_SymbolDateTimeFormatConstructor == nullptr) {
        HILOG_ERROR_I18N(
            "SymbolDateTimeFormatAddon::IsSymbolDateTimeFormat: g_SymbolDateTimeFormatConstructor is nullptr.");
        return false;
    }
    napi_value constructor = nullptr;
    if (!g_SymbolDateTimeFormatConstructor->GetReferenceValue(env, &constructor)) {
        HILOG_ERROR_I18N("SymbolDateTimeFormatAddon::IsSymbolDateTimeFormat: Failed to create reference.");
        return false;
    }

    bool isSymbolDateTimeFormat = false;
    napi_status status = napi_instanceof(env, argv, constructor, &isSymbolDateTimeFormat);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("SymbolDateTimeFormatAddon::IsSymbolDateTimeFormat: Failed to get instance of argv.");
        return false;
    }
    return isSymbolDateTimeFormat;
}

napi_status SymbolDateTimeFormatAddon::UnwrapDateTimeFormat(napi_env env, napi_value value,
    SymbolDateTimeFormatAddon** dateTimeFormat)
{
    return napi_unwrap_s(env, value, &TYPE_TAG, reinterpret_cast<void **>(dateTimeFormat));
}

bool SymbolDateTimeFormatAddon::GetParseParams(napi_env env, napi_callback_info info,
    std::string& text, bool& lenientMode, napi_value& thisVar)
{
    size_t argc = PARSE_PARAM_COUNT;
    napi_value argv[PARSE_PARAM_COUNT] = { nullptr };
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("GetParseParams: napi_get_cb_info failed, status: %{public}d", status);
        return false;
    }
    if (argc < PARSE_PARAM_COUNT) {
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, "text or lenientMode", "", true);
        return false;
    }
    napi_valuetype valueType = napi_undefined;
    status = napi_typeof(env, argv[TEXT_PARAM_INDEX], &valueType);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("GetParseParams: napi_typeof text failed, status: %{public}d", status);
        return false;
    }
    if (valueType != napi_string) {
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, "text", "string", true);
        return false;
    }
    int32_t code = 0;
    text = VariableConvertor::GetString(env, argv[TEXT_PARAM_INDEX], code);
    if (code != 0) {
        HILOG_ERROR_I18N("GetParseParams: GetString failed, code: %{public}d", code);
        return false;
    }
    status = napi_typeof(env, argv[LENIENT_MODE_PARAM_INDEX], &valueType);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("GetParseParams: napi_typeof lenientMode failed, status: %{public}d", status);
        return false;
    }
    if (valueType != napi_boolean) {
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, "lenientMode", "boolean", true);
        return false;
    }
    status = napi_get_value_bool(env, argv[LENIENT_MODE_PARAM_INDEX], &lenientMode);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("GetParseParams: napi_get_value_bool failed, status: %{public}d", status);
        return false;
    }
    return true;
}

bool SymbolDateTimeFormatAddon::UnwrapParseObject(napi_env env, napi_value thisVar,
    SymbolDateTimeFormatAddon*& obj)
{
    napi_status status = napi_unwrap_s(env, thisVar, &TYPE_TAG, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->symbolDateTimeFormat_) {
        HILOG_ERROR_I18N("UnwrapParseObject: unwrap failed, status: %{public}d", status);
        return false;
    }
    return true;
}

bool SymbolDateTimeFormatAddon::HandleParseResult(napi_env env, I18nErrorCode errCode)
{
    if (errCode == I18nErrorCode::FAILED) {
        HILOG_ERROR_I18N("HandleParseResult: Framework returned FAILED (system error).");
        return false;
    }
    if (errCode == I18nErrorCode::INVALID_PARAM) {
        ErrorUtil::NapiThrow(env, I18N_NOT_VALID_NEW, "text", "a valid localized date string", true);
        return false;
    }
    return true;
}

napi_value SymbolDateTimeFormatAddon::Parse(napi_env env, napi_callback_info info)
{
    std::string text;
    bool lenientMode = false;
    napi_value thisVar = nullptr;
    if (!GetParseParams(env, info, text, lenientMode, thisVar)) {
        return VariableConvertor::CreateInt64Value(env, 0);
    }
    SymbolDateTimeFormatAddon* obj = nullptr;
    if (!UnwrapParseObject(env, thisVar, obj)) {
        return VariableConvertor::CreateInt64Value(env, 0);
    }
    I18nErrorCode errCode = I18nErrorCode::SUCCESS;
    double result = obj->symbolDateTimeFormat_->Parse(text, lenientMode, errCode);
    if (!HandleParseResult(env, errCode)) {
        return VariableConvertor::CreateInt64Value(env, 0);
    }
    return VariableConvertor::CreateInt64Value(env, static_cast<int64_t>(result));
}
} // namespace I18n
} // namespace Global
} // namespace OHOS