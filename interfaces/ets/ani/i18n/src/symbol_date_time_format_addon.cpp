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

#include <unordered_set>
#include "error_util.h"
#include "i18n_hilog.h"
#include "locale_config.h"
#include "variable_converter.h"

namespace OHOS {
namespace Global {
namespace I18n {
I18nSymbolDateTimeFormatAddon* I18nSymbolDateTimeFormatAddon::UnwrapAddon(ani_env *env, ani_object object)
{
    ani_long ptr;
    if (ANI_OK != env->Object_GetFieldByName_Long(object, "nativeSymbolDateTimeFormat", &ptr)) {
        HILOG_ERROR_I18N("Get Long 'nativeSymbolDateTimeFormat' failed");
        return nullptr;
    }
    return reinterpret_cast<I18nSymbolDateTimeFormatAddon*>(ptr);
}

ani_object I18nSymbolDateTimeFormatAddon::Create(ani_env *env, [[maybe_unused]] ani_object object,
    ani_object locale, ani_object options)
{
    std::unique_ptr<I18nSymbolDateTimeFormatAddon> obj = std::make_unique<I18nSymbolDateTimeFormatAddon>();
    if (!obj->InitContext(env, locale, options)) {
        HILOG_ERROR_I18N("I18nSymbolDateTimeFormatAddon::Create: Init context failed.");
        return nullptr;
    }

    static const char *className = "@ohos.i18n.i18n.SymbolDateTimeFormat";
    ani_object symbolDateTimeFormatObject = VariableConverter::CreateAniObject(env, className, obj.get());
    obj.release();
    return symbolDateTimeFormatObject;
}

bool I18nSymbolDateTimeFormatAddon::InitContext(ani_env *env, ani_object locale, ani_object options)
{
    ani_boolean isUndefined;
    if (ANI_OK != env->Reference_IsUndefined(locale, &isUndefined)) {
        HILOG_ERROR_I18N("I18nSymbolDateTimeFormatAddon::InitContext: Check locale is undefined failed.");
        return false;
    }
    std::string localeTag;
    if (!isUndefined) {
        localeTag = VariableConverter::GetLocaleTagFromBuiltinLocale(env, locale);
    }
    if (localeTag.empty()) {
        localeTag = LocaleConfig::GetEffectiveLocale();
    }

    auto symbolOptions = I18nSymbolDateTimeFormatAddon::ParseOptions(env, options);

    std::string errMessage;
    symbolDateTimeFormat_ =
        std::make_shared<SymbolDateTimeFormat>(std::vector<std::string>{ localeTag }, symbolOptions, errMessage);
    if (!errMessage.empty()) {
        ErrorUtil::AniThrowUndefined(env, errMessage);
        return false;
    }
    return symbolDateTimeFormat_ != nullptr;
}

std::unordered_map<std::string, std::string> I18nSymbolDateTimeFormatAddon::ParseOptions(ani_env *env,
    ani_object options)
{
    std::unordered_map<std::string, std::string> result;

    ani_boolean isUndefined;
    if (ANI_OK != env->Reference_IsUndefined(options, &isUndefined)) {
        HILOG_ERROR_I18N("I18nSymbolDateTimeFormatAddon::ParseOptions: Check options is undefined failed.");
        return result;
    }
    if (isUndefined) {
        return result;
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
        if (VariableConverter::GetStringMember(env, options, key, value)) {
            result.insert(std::make_pair(key, value));
        }
    }

    bool hour12 = false;
    if (VariableConverter::GetBooleanMember(env, options, IntlDateTimeFormat::HOUR12_TAG, hour12)) {
        result.insert({IntlDateTimeFormat::HOUR12_TAG, hour12 ? "true" : "false"});
    }

    int fractionalSecondDigits = 0;
    if (VariableConverter::GetNumberMember(env, options, IntlDateTimeFormat::FRACTIONAL_SECOND_DIGITS_TAG,
        fractionalSecondDigits)) {
        result.insert({IntlDateTimeFormat::FRACTIONAL_SECOND_DIGITS_TAG, std::to_string(fractionalSecondDigits)});
    }
    ParseAmPmSymbol(env, options, result);
    return result;
}

ani_string I18nSymbolDateTimeFormatAddon::FormatImpl(ani_env *env, ani_object object, ani_object date)
{
    ani_boolean isUndefined;
    if (ANI_OK != env->Reference_IsUndefined(date, &isUndefined)) {
        HILOG_ERROR_I18N("I18nSymbolDateTimeFormatAddon::FormatImpl: Check date is undefined failed");
        return VariableConverter::StringToAniStr(env, "");
    }
    double milliseconds;
    if (isUndefined || ANI_OK != env->Object_CallMethodByName_Double(date, "toDouble", ":d", &milliseconds)) {
        std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
        );
        milliseconds = ms.count();
    }

    I18nSymbolDateTimeFormatAddon* obj = UnwrapAddon(env, object);
    if (obj == nullptr || obj->symbolDateTimeFormat_  == nullptr) {
        HILOG_ERROR_I18N("I18nSymbolDateTimeFormatAddon::FormatImpl: UnwrapAddon object failed");
        return VariableConverter::StringToAniStr(env, "");
    }
    return VariableConverter::StringToAniStr(env, obj->symbolDateTimeFormat_->Format(milliseconds));
}

ani_object I18nSymbolDateTimeFormatAddon::FormatToPartsImpl(ani_env *env, ani_object object, ani_object date)
{
    ani_boolean isUndefined;
    if (ANI_OK != env->Reference_IsUndefined(date, &isUndefined)) {
        HILOG_ERROR_I18N("I18nSymbolDateTimeFormatAddon::InitContext: Check locale reference is undefined failed");
        return nullptr;
    }
    double milliseconds;
    if (isUndefined || ANI_OK != env->Object_CallMethodByName_Double(date, "toDouble", ":d", &milliseconds)) {
        std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
        );
        milliseconds = ms.count();
    }

    I18nSymbolDateTimeFormatAddon* obj = UnwrapAddon(env, object);
    if (obj == nullptr || obj->symbolDateTimeFormat_  == nullptr) {
        HILOG_ERROR_I18N("I18nSymbolDateTimeFormatAddon::FormatToPartsImpl: UnwrapAddon object failed");
        return nullptr;
    }
    std::string errMessage;
    auto parts = obj->symbolDateTimeFormat_->FormatToParts(milliseconds, errMessage);
    if (!errMessage.empty()) {
        ErrorUtil::AniThrowUndefined(env, errMessage);
        return nullptr;
    }
    return CreateDateTimeFormatPartArray(env, parts);
}

ani_string I18nSymbolDateTimeFormatAddon::FormatRangeImpl(ani_env *env, ani_object object, ani_double startDate,
    ani_double endDate)
{
    I18nSymbolDateTimeFormatAddon* obj = UnwrapAddon(env, object);
    if (obj == nullptr || obj->symbolDateTimeFormat_  == nullptr) {
        HILOG_ERROR_I18N("I18nSymbolDateTimeFormatAddon::FormatRangeImpl: UnwrapAddon object failed.");
        return VariableConverter::StringToAniStr(env, "");
    }
    std::string errMessage;
    std::string result = obj->symbolDateTimeFormat_->FormatRange(startDate, endDate, errMessage);
    if (!errMessage.empty()) {
        ErrorUtil::AniThrowUndefined(env, errMessage);
        return VariableConverter::StringToAniStr(env, "");
    }
    return VariableConverter::StringToAniStr(env, result);
}

ani_object I18nSymbolDateTimeFormatAddon::FormatRangeToPartsImpl(ani_env *env, ani_object object, ani_double startDate,
    ani_double endDate)
{
    I18nSymbolDateTimeFormatAddon* obj = UnwrapAddon(env, object);
    if (obj == nullptr || obj->symbolDateTimeFormat_  == nullptr) {
        HILOG_ERROR_I18N("I18nSymbolDateTimeFormatAddon::FormatRangeToPartsImpl: UnwrapAddon object failed.");
        return nullptr;
    }
    std::string errMessage;
    auto parts = obj->symbolDateTimeFormat_->FormatRangeToParts(startDate, endDate, errMessage);
    if (!errMessage.empty()) {
        ErrorUtil::AniThrowUndefined(env, errMessage);
        return  nullptr;
    }
    return CreateDateTimeRangeFormatPartArray(env, parts);
}

ani_string I18nSymbolDateTimeFormatAddon::ToString(ani_env *env, [[maybe_unused]] ani_object object)
{
    return VariableConverter::StringToAniStr(env, "[object i18n.SymbolDateTimeFormat]");
}

ani_object I18nSymbolDateTimeFormatAddon::ResolvedOptions(ani_env *env, ani_object object)
{
    I18nSymbolDateTimeFormatAddon* obj = UnwrapAddon(env, object);
    if (obj == nullptr || obj->symbolDateTimeFormat_  == nullptr) {
        HILOG_ERROR_I18N("I18nSymbolDateTimeFormatAddon::ResolvedOptions: UnwrapAddon object failed.");
        return nullptr;
    }
    std::unordered_map<std::string, std::string> options;
    obj->symbolDateTimeFormat_->ResolvedOptions(options);
    return CreateResolvedOptions(env, options);
}

ani_array I18nSymbolDateTimeFormatAddon::CreateDateTimeFormatPartArray(ani_env *env,
    const std::vector<std::unordered_map<std::string, std::string>> &parts)
{
    ani_ref undefined {};
    ani_array result {};
    if (ANI_OK != env->GetUndefined(&undefined)) {
        HILOG_ERROR_I18N("CreateDateTimeFormatPartArray: GetUndefined failed.");
        return result;
    }
    if (ANI_OK != env->Array_New(parts.size(), undefined, &result)) {
        HILOG_ERROR_I18N("CreateDateTimeFormatPartArray: Create array failed.");
        return result;
    }

    for (size_t index = 0; index < parts.size(); ++index) {
        ani_object part = CreateDateTimeFormatPart(env, parts[index]);
        if (part == nullptr || ANI_OK != env->Array_Set(result, index, part)) {
            HILOG_ERROR_I18N("CreateDateTimeFormatPartArray: Set array failed.");
            return result;
        }
    }
    return result;
}

void I18nSymbolDateTimeFormatAddon::ParseAmPmSymbol(ani_env *env, ani_object options,
    std::unordered_map<std::string, std::string>& result)
{
    ani_ref ref;
    if (ANI_OK != env->Object_GetPropertyByName_Ref(options, "amPMSymbol", &ref)) {
        HILOG_ERROR_I18N("I18nSymbolDateTimeFormatAddon::ParseAmPmSymbol: Get property amPMSymbol failed.");
        return;
    }

    ani_boolean isUndefined;
    if (ANI_OK != env->Reference_IsUndefined(ref, &isUndefined)) {
        HILOG_ERROR_I18N("I18nSymbolDateTimeFormatAddon::ParseAmPmSymbol: Check reference is undefined failed");
        return;
    }
    if (isUndefined) {
        return;
    }

    std::vector<std::string> symbol;
    if (!VariableConverter::ParseStringArray(env, static_cast<ani_object>(ref), symbol)) {
        HILOG_ERROR_I18N("I18nSymbolDateTimeFormatAddon::ParseAmPmSymbol: ParseStringArray failed.");
        return;
    }
    if (symbol.size() < SymbolDateTimeFormat::AM_PM_SYMBOLS_LEN) {
        ErrorUtil::AniThrow(env, I18N_NOT_VALID, "amPMSymbol", "a string[] with two or more elements");
        return;
    }
    result.insert(std::make_pair(SymbolDateTimeFormat::AM_SYMBOL_TAG, symbol[0]));
    result.insert(std::make_pair(SymbolDateTimeFormat::PM_SYMBOL_TAG, symbol[1]));
}

ani_object I18nSymbolDateTimeFormatAddon::CreateDateTimeFormatPart(ani_env *env,
    const std::unordered_map<std::string, std::string> &part)
{
    static const char *className = "std.core.Intl.DateTimeFormatPartImpl";
    ani_class cls;
    if (ANI_OK != env->FindClass(className, &cls)) {
        HILOG_ERROR_I18N("CreateDateTimeFormatPart: Find class '%{public}s' failed", className);
        return nullptr;
    }

    static const char *ctorSignature = "C{std.core.String}C{std.core.String}:";
    ani_method ctor;
    if (ANI_OK != env->Class_FindMethod(cls, "<ctor>", ctorSignature, &ctor)) {
        HILOG_ERROR_I18N("CreateDateTimeFormatPart: Find method '<ctor>' failed.");
        return nullptr;
    }

    auto typeIter = part.find(IntlDateTimeFormat::PART_TYPE_TAG);
    auto valueIter = part.find(IntlDateTimeFormat::PART_VALUE_TAG);
    if (typeIter == part.end() || valueIter == part.end()) {
        HILOG_ERROR_I18N("CreateDateTimeFormatPart: Invalid part.");
        return nullptr;
    }
    ani_string type = VariableConverter::StringToAniStr(env, typeIter->second);
    ani_string value = VariableConverter::StringToAniStr(env, valueIter->second);

    ani_object dateTimeFormatPart;
    if (ANI_OK != env->Object_New(cls, ctor, &dateTimeFormatPart, type, value)) {
        HILOG_ERROR_I18N("CreateDateTimeFormatPart: New object '%{public}s' failed.", className);
        return nullptr;
    }
    return dateTimeFormatPart;
}

ani_array I18nSymbolDateTimeFormatAddon::CreateDateTimeRangeFormatPartArray(ani_env *env,
    const std::vector<std::unordered_map<std::string, std::string>> &parts)
{
    ani_ref undefined {};
    ani_array result {};
    if (ANI_OK != env->GetUndefined(&undefined)) {
        HILOG_ERROR_I18N("CreateDateTimeRangeFormatPartArray: GetUndefined failed.");
        return result;
    }
    if (ANI_OK != env->Array_New(parts.size(), undefined, &result)) {
        HILOG_ERROR_I18N("CreateDateTimeRangeFormatPartArray: Create array failed.");
        return result;
    }

    for (size_t index = 0; index < parts.size(); ++index) {
        ani_object part = CreateDateTimeRangeFormatPart(env, parts[index]);
        if (part == nullptr || ANI_OK != env->Array_Set(result, index, part)) {
            HILOG_ERROR_I18N("CreateDateTimeRangeFormatPartArray: Set array failed.");
            return result;
        }
    }
    return result;
}

ani_object I18nSymbolDateTimeFormatAddon::CreateDateTimeRangeFormatPart(ani_env *env,
    const std::unordered_map<std::string, std::string> &part)
{
    static const char *className = "std.core.Intl.DateTimeRangeFormatPartImpl";
    ani_class cls;
    if (ANI_OK != env->FindClass(className, &cls)) {
        HILOG_ERROR_I18N("CreateDateTimeRangeFormatPart: Find class '%{public}s' failed", className);
        return nullptr;
    }

    static const char *ctorSignature = "C{std.core.String}C{std.core.String}C{std.core.String}:";
    ani_method ctor;
    if (ANI_OK != env->Class_FindMethod(cls, "<ctor>", ctorSignature, &ctor)) {
        HILOG_ERROR_I18N("CreateDateTimeRangeFormatPart: Find method '<ctor>' failed.");
        return nullptr;
    }

    auto typeIter = part.find(IntlDateTimeFormat::PART_TYPE_TAG);
    auto valueIter = part.find(IntlDateTimeFormat::PART_VALUE_TAG);
    auto sourceIter = part.find(IntlDateTimeFormat::PART_SOURCE_TAG);
    if (typeIter == part.end() || valueIter == part.end() || sourceIter == part.end()) {
        HILOG_ERROR_I18N("CreateDateTimeRangeFormatPart: Invalid part.");
        return nullptr;
    }
    ani_string type = VariableConverter::StringToAniStr(env, typeIter->second);
    ani_string value = VariableConverter::StringToAniStr(env, valueIter->second);
    ani_string source = VariableConverter::StringToAniStr(env, sourceIter->second);

    ani_object dateTimeRangeFormatPart;
    if (ANI_OK != env->Object_New(cls, ctor, &dateTimeRangeFormatPart, type, value, source)) {
        HILOG_ERROR_I18N("CreateDateTimeRangeFormatPart: New object '%{public}s' failed.", className);
        return nullptr;
    }
    return dateTimeRangeFormatPart;
}

ani_object I18nSymbolDateTimeFormatAddon::CreateResolvedOptions(ani_env *env,
    std::unordered_map<std::string, std::string> &options)
{
    static const char *className = "@ohos.i18n.i18n.ResolvedSymbolDateTimeFormatOptionsInner";
    ani_class cls;
    if (ANI_OK != env->FindClass(className, &cls)) {
        HILOG_ERROR_I18N("I18nSymbolDateTimeFormatAddon::CreateResolvedOptions: Find class '%{public}s' failed.",
            className);
        return nullptr;
    }

    ani_method ctor;
    if (ANI_OK != env->Class_FindMethod(cls, "<ctor>", ":", &ctor)) {
        HILOG_ERROR_I18N("I18nSymbolDateTimeFormatAddon::CreateResolvedOptions: Find method '<ctor>' failed.");
        return nullptr;
    }

    ani_object resolvedOptions;
    if (ANI_OK != env->Object_New(cls, ctor, &resolvedOptions)) {
        HILOG_ERROR_I18N("I18nSymbolDateTimeFormatAddon::CreateResolvedOptions: New object '%{public}s' failed",
            className);
        return nullptr;
    }

    ResolvedAmPmSymbol(env, resolvedOptions, options);
    for (const auto& option : options) {
        if (option.first == IntlDateTimeFormat::HOUR12_TAG) {
            VariableConverter::SetBooleanMember(env, resolvedOptions, option.first, option.second);
        } else if (option.first == IntlDateTimeFormat::FRACTIONAL_SECOND_DIGITS_TAG) {
            VariableConverter::SetNumberMember(env, resolvedOptions, option.first, std::atoi(option.second.c_str()));
        } else {
            VariableConverter::SetStringMember(env, resolvedOptions, option.first, option.second);
        }
    }
    return resolvedOptions;
}

void I18nSymbolDateTimeFormatAddon::ResolvedAmPmSymbol(ani_env *env, ani_object resolvedOptions,
    std::unordered_map<std::string, std::string> &options)
{
    std::vector<std::string> amPMSymbol(SymbolDateTimeFormat::AM_PM_SYMBOLS_LEN, "");
    auto amIter = options.find(SymbolDateTimeFormat::AM_SYMBOL_TAG);
    if (amIter == options.end()) {
        return;
    }
    amPMSymbol[0] = amIter->second;
    options.erase(amIter);
    auto pmIter = options.find(SymbolDateTimeFormat::PM_SYMBOL_TAG);
    if (pmIter == options.end()) {
        return;
    }
    amPMSymbol[1] = pmIter->second;
    options.erase(pmIter);
    ani_object amPMSymbolArray = VariableConverter::CreateArray(env, amPMSymbol);
    if (ANI_OK != env->Object_SetPropertyByName_Ref(resolvedOptions, "amPMSymbol", amPMSymbolArray)) {
        HILOG_ERROR_I18N("I18nSymbolDateTimeFormatAddon::ResolvedAmPmSymbol: Set amPMSymbol failed.");
        return;
    }
}

ani_status I18nSymbolDateTimeFormatAddon::BindContextSymbolDateTimeFormat(ani_env *env)
{
    static const char *className = "@ohos.i18n.i18n.SymbolDateTimeFormat";
    ani_class cls;
    if (ANI_OK != env->FindClass(className, &cls)) {
        HILOG_ERROR_I18N("BindContextSymbolDateTimeFormat: Find class '%{public}s' failed", className);
        return ANI_ERROR;
    }

    std::array staticMethods = {
        ani_native_function { "create", nullptr, reinterpret_cast<void *>(I18nSymbolDateTimeFormatAddon::Create) },
    };
    if (ANI_OK != env->Class_BindStaticNativeMethods(cls, staticMethods.data(), staticMethods.size())) {
        HILOG_ERROR_I18N("BindContextSymbolDateTimeFormat: Cannot bind static native methods to '%{public}s'",
            className);
        return ANI_ERROR;
    }

    std::array methods = {
        ani_native_function { "formatImpl", nullptr,
            reinterpret_cast<void *>(I18nSymbolDateTimeFormatAddon::FormatImpl) },
        ani_native_function { "formatToPartsImpl", nullptr,
            reinterpret_cast<void *>(I18nSymbolDateTimeFormatAddon::FormatToPartsImpl) },
        ani_native_function { "formatRangeImpl", nullptr,
            reinterpret_cast<void *>(I18nSymbolDateTimeFormatAddon::FormatRangeImpl) },
        ani_native_function { "formatRangeToPartsImpl", nullptr,
            reinterpret_cast<void *>(I18nSymbolDateTimeFormatAddon::FormatRangeToPartsImpl) },
        ani_native_function { "toString", nullptr,
            reinterpret_cast<void *>(I18nSymbolDateTimeFormatAddon::ToString) },
        ani_native_function { "resolvedOptions", nullptr,
            reinterpret_cast<void *>(I18nSymbolDateTimeFormatAddon::ResolvedOptions) },
        ani_native_function { "parse", nullptr,
            reinterpret_cast<void *>(I18nSymbolDateTimeFormatAddon::Parse) },
    };
    if (ANI_OK != env->Class_BindNativeMethods(cls, methods.data(), methods.size())) {
        HILOG_ERROR_I18N("BindContextSymbolDateTimeFormat: Cannot bind native methods to '%{public}s'", className);
        return ANI_ERROR;
    }
    return ANI_OK;
}

bool I18nSymbolDateTimeFormatAddon::IsSymbolDateTimeFormat(ani_env *env, ani_object dateTimeFormat)
{
    static const char *className = "@ohos.i18n.i18n.SymbolDateTimeFormat";
    ani_class cls;
    if (ANI_OK != env->FindClass(className, &cls)) {
        HILOG_ERROR_I18N("IsSymbolDateTimeFormat: Find class failed.");
        return false;
    }

    ani_boolean isSymbolDateTimeFormat;
    if (ANI_OK != env->Object_InstanceOf(dateTimeFormat, cls, &isSymbolDateTimeFormat)) {
        HILOG_ERROR_I18N("IsSymbolDateTimeFormat: Get Instance failed.");
        return false;
    }
    return isSymbolDateTimeFormat;
}

std::shared_ptr<SymbolDateTimeFormat> I18nSymbolDateTimeFormatAddon::GetDateTimeFormat()
{
    return symbolDateTimeFormat_;
}

ani_long I18nSymbolDateTimeFormatAddon::Parse(ani_env *env, ani_object object, ani_string text,
    ani_boolean lenientMode)
{
    // Step 1: Unwrap object - ANI internal failure: log only
    I18nSymbolDateTimeFormatAddon* obj = UnwrapAddon(env, object);
    if (obj == nullptr) {
        HILOG_ERROR_I18N("I18nSymbolDateTimeFormatAddon::Parse: UnwrapAddon failed.");
        return 0;
    }
    if (obj->symbolDateTimeFormat_ == nullptr) {
        HILOG_ERROR_I18N("I18nSymbolDateTimeFormatAddon::Parse: symbolDateTimeFormat_ is nullptr.");
        return 0;
    }

    // Step 2: Convert text - ANI internal failure: log only
    std::string textStr = VariableConverter::AniStrToString(env, text);

    // Step 3: Call framework and handle different error codes
    I18nErrorCode status = I18nErrorCode::SUCCESS;
    double result = obj->symbolDateTimeFormat_->Parse(textStr, lenientMode, status);

    // System error (FAILED) - log only, NO error code thrown
    if (status == I18nErrorCode::FAILED) {
        HILOG_ERROR_I18N("I18nSymbolDateTimeFormatAddon::Parse: Framework returned FAILED (system error).");
        return 0;
    }

    // Input error (INVALID_PARAM) - throw declared error code 8900001
    if (status == I18nErrorCode::INVALID_PARAM) {
        ErrorUtil::AniThrow(env, I18N_NOT_VALID_NEW, "text", "a valid localized date string");
        return 0;
    }

    // Step 4: Return value type conversion - interface defines 'long'
    // Framework returns double (UDate), convert to ani_long per interface definition
    return static_cast<ani_long>(result);
}
} // namespace I18n
} // namespace Global
} // namespace OHOS
