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
#include "locale_config.h"
#include "variable_converter.h"

namespace OHOS {
namespace Global {
namespace I18n {
I18nSymbolNumberFormatAddon* I18nSymbolNumberFormatAddon::UnwrapAddon(ani_env *env, ani_object object)
{
    ani_long ptr;
    if (ANI_OK != env->Object_GetFieldByName_Long(object, "nativeSymbolNumberFormat", &ptr)) {
        HILOG_ERROR_I18N("Get Long 'nativeSymbolNumberFormat' failed");
        return nullptr;
    }
    return reinterpret_cast<I18nSymbolNumberFormatAddon*>(ptr);
}

ani_object I18nSymbolNumberFormatAddon::Create(ani_env *env, [[maybe_unused]] ani_object object,
    ani_object locale, ani_object options)
{
    std::unique_ptr<I18nSymbolNumberFormatAddon> obj = std::make_unique<I18nSymbolNumberFormatAddon>();
    if (!obj->InitContext(env, locale, options)) {
        HILOG_ERROR_I18N("I18nSymbolNumberFormatAddon::Create: Init context failed.");
        return nullptr;
    }

    static const char *className = "@ohos.i18n.i18n.SymbolNumberFormat";
    ani_object symbolNumberFormatObject = VariableConverter::CreateAniObject(env, className, obj.get());
    obj.release();
    return symbolNumberFormatObject;
}

bool I18nSymbolNumberFormatAddon::InitContext(ani_env *env, ani_object locale, ani_object options)
{
    ani_boolean isUndefined;
    if (ANI_OK != env->Reference_IsUndefined(locale, &isUndefined)) {
        HILOG_ERROR_I18N("I18nSymbolNumberFormatAddon::InitContext: Check locale is undefined failed.");
        return false;
    }
    std::string localeTag;
    if (!isUndefined) {
        localeTag = VariableConverter::GetLocaleTagFromBuiltinLocale(env, locale);
    }
    if (localeTag.empty()) {
        localeTag = LocaleConfig::GetEffectiveLocale();
    }

    auto symbolOptions = I18nSymbolNumberFormatAddon::ParseOptions(env, options);

    std::string errMessage;
    int32_t code = 0;
    symbolNumberFormat_ =
        std::make_shared<SymbolNumberFormat>(std::vector<std::string>{ localeTag }, symbolOptions, errMessage, code);
    if (!errMessage.empty() || code != 0) {
        ErrorUtil::AniThrowUndefined(env, errMessage);
        return false;
    }
    return symbolNumberFormat_ != nullptr;
}

std::map<std::string, std::string> I18nSymbolNumberFormatAddon::ParseOptions(ani_env *env,
    ani_object options)
{
    std::map<std::string, std::string> result;

    ani_boolean isUndefined;
    if (ANI_OK != env->Reference_IsUndefined(options, &isUndefined)) {
        HILOG_ERROR_I18N("I18nSymbolNumberFormatAddon::ParseOptions: Check options is undefined failed.");
        return result;
    }
    if (isUndefined) {
        return result;
    }

    std::string valueString;
    std::unordered_set<std::string> keyOfTypeString = { "currency", "currencySign", "currencyDisplay", "unit",
        "unitDisplay", "compactDisplay", "signDisplay", "localeMatcher", "style", "notation",
        "groupingSeparator", "infinity", "plusSign", "minusSign",
        "nan", "zero" };

    for (const auto& key : keyOfTypeString) {
        if (VariableConverter::GetStringMember(env, options, key, valueString)) {
            result.insert(std::make_pair(key, valueString));
        }
    }

    bool useGrouping = false;
    if (VariableConverter::GetBooleanMember(env, options, "useGrouping", useGrouping)) {
        result.insert(std::make_pair("useGrouping", useGrouping ? "true" : "false"));
    }

    int valueNumber = 0;
    std::unordered_set<std::string> keyOfTypeNumber = { "minimumIntegerDigits", "minimumFractionDigits",
        "maximumFractionDigits", "minimumSignificantDigits", "maximumSignificantDigits" };
    for (const auto& key : keyOfTypeNumber) {
        if (VariableConverter::GetNumberMember(env, options, key, valueNumber)) {
            result.insert(std::make_pair(key, std::to_string(valueNumber)));
        }
    }
    return result;
}

ani_string I18nSymbolNumberFormatAddon::FormatDoubleImpl(ani_env *env, ani_object object, ani_double value)
{
    I18nSymbolNumberFormatAddon* obj = UnwrapAddon(env, object);
    if (obj == nullptr || obj->symbolNumberFormat_ == nullptr) {
        HILOG_ERROR_I18N("I18nSymbolNumberFormatAddon::FormatDoubleImpl: UnwrapAddon object failed");
        return VariableConverter::StringToAniStr(env, "");
    }
    std::string result = obj->symbolNumberFormat_->Format(value);
    return VariableConverter::StringToAniStr(env, result);
}

ani_string I18nSymbolNumberFormatAddon::FormatLongImpl(ani_env *env, ani_object object, ani_long value)
{
    I18nSymbolNumberFormatAddon* obj = UnwrapAddon(env, object);
    if (obj == nullptr || obj->symbolNumberFormat_ == nullptr) {
        HILOG_ERROR_I18N("I18nSymbolNumberFormatAddon::FormatLongImpl: UnwrapAddon object failed");
        return VariableConverter::StringToAniStr(env, "");
    }
    std::string result = obj->symbolNumberFormat_->FormatLong(value);
    return VariableConverter::StringToAniStr(env, result);
}

ani_string I18nSymbolNumberFormatAddon::FormatDecStrImpl(ani_env *env, ani_object object, ani_string value)
{
    I18nSymbolNumberFormatAddon* obj = UnwrapAddon(env, object);
    if (obj == nullptr || obj->symbolNumberFormat_ == nullptr) {
        HILOG_ERROR_I18N("I18nSymbolNumberFormatAddon::FormatDecStrImpl: UnwrapAddon object failed");
        return VariableConverter::StringToAniStr(env, "");
    }
    std::string result = obj->symbolNumberFormat_->FormatBigInt(VariableConverter::AniStrToString(env, value));
    return VariableConverter::StringToAniStr(env, result);
}

ani_object I18nSymbolNumberFormatAddon::FormatToPartsDoubleImpl(ani_env *env, ani_object object, ani_double value)
{
    I18nSymbolNumberFormatAddon* obj = UnwrapAddon(env, object);
    if (obj == nullptr || obj->symbolNumberFormat_ == nullptr) {
        HILOG_ERROR_I18N("I18nSymbolNumberFormatAddon::FormatDecStrImpl: UnwrapAddon object failed");
        return nullptr;
    }
    auto parts = obj->symbolNumberFormat_->FormatToPartsDouble(value);
    return CreateNumberFormatPartArray(env, parts, false);
}

ani_object I18nSymbolNumberFormatAddon::FormatToPartsDecStrImpl(ani_env *env, ani_object object, ani_string value)
{
    I18nSymbolNumberFormatAddon* obj = UnwrapAddon(env, object);
    if (obj == nullptr || obj->symbolNumberFormat_ == nullptr) {
        HILOG_ERROR_I18N("I18nSymbolNumberFormatAddon::FormatDecStrImpl: UnwrapAddon object failed");
        return nullptr;
    }
    auto parts = obj->symbolNumberFormat_->FormatToPartsDecStr(VariableConverter::AniStrToString(env, value));
    return CreateNumberFormatPartArray(env, parts, false);
}

ani_string I18nSymbolNumberFormatAddon::FormatRangeDoubleDoubleImpl(ani_env *env, ani_object object,
    ani_double start, ani_double end)
{
    I18nSymbolNumberFormatAddon* obj = UnwrapAddon(env, object);
    if (obj == nullptr || obj->symbolNumberFormat_ == nullptr) {
        HILOG_ERROR_I18N("I18nSymbolNumberFormatAddon::FormatRangeDoubleDoubleImpl: UnwrapAddon object failed");
        return VariableConverter::StringToAniStr(env, "");
    }
    std::string result = obj->symbolNumberFormat_->FormatRangeDoubleDouble(start, end);
    return VariableConverter::StringToAniStr(env, result);
}

ani_string I18nSymbolNumberFormatAddon::FormatRangeDoubleDecStrImpl(ani_env *env, ani_object object,
    ani_double start, ani_string end)
{
    I18nSymbolNumberFormatAddon* obj = UnwrapAddon(env, object);
    if (obj == nullptr || obj->symbolNumberFormat_ == nullptr) {
        HILOG_ERROR_I18N("I18nSymbolNumberFormatAddon::FormatRangeDoubleDecStrImpl: UnwrapAddon object failed");
        return VariableConverter::StringToAniStr(env, "");
    }
    std::string result = obj->symbolNumberFormat_->FormatRangeDoubleDecStr(start,
        VariableConverter::AniStrToString(env, end));
    return VariableConverter::StringToAniStr(env, result);
}

ani_string I18nSymbolNumberFormatAddon::FormatRangeDecStrDoubleImpl(ani_env *env, ani_object object,
    ani_string start, ani_double end)
{
    I18nSymbolNumberFormatAddon* obj = UnwrapAddon(env, object);
    if (obj == nullptr || obj->symbolNumberFormat_ == nullptr) {
        HILOG_ERROR_I18N("I18nSymbolNumberFormatAddon::FormatRangeDoubleDecStrImpl: UnwrapAddon object failed");
        return VariableConverter::StringToAniStr(env, "");
    }
    std::string result =
        obj->symbolNumberFormat_->FormatRangeDecStrDouble(VariableConverter::AniStrToString(env, start), end);
    return VariableConverter::StringToAniStr(env, result);
}

ani_string I18nSymbolNumberFormatAddon::FormatRangeDecStrDecStrImpl(ani_env *env, ani_object object,
    ani_string start, ani_string end)
{
    I18nSymbolNumberFormatAddon* obj = UnwrapAddon(env, object);
    if (obj == nullptr || obj->symbolNumberFormat_ == nullptr) {
        HILOG_ERROR_I18N("I18nSymbolNumberFormatAddon::FormatRangeDoubleDecStrImpl: UnwrapAddon object failed");
        return VariableConverter::StringToAniStr(env, "");
    }
    std::string result = obj->symbolNumberFormat_->FormatRangeDecStrDecStr(
        VariableConverter::AniStrToString(env, start), VariableConverter::AniStrToString(env, end));
    return VariableConverter::StringToAniStr(env, result);
}

ani_object I18nSymbolNumberFormatAddon::FormatToRangePartsDoubleDoubleImpl(ani_env *env, ani_object object,
    ani_double start, ani_double end)
{
    I18nSymbolNumberFormatAddon* obj = UnwrapAddon(env, object);
    if (obj == nullptr || obj->symbolNumberFormat_ == nullptr) {
        HILOG_ERROR_I18N("I18nSymbolNumberFormatAddon::FormatDecStrImpl: UnwrapAddon object failed");
        return nullptr;
    }
    auto parts = obj->symbolNumberFormat_->FormatToRangePartsDoubleDouble(start, end);
    return CreateNumberFormatPartArray(env, parts, true);
}

ani_object I18nSymbolNumberFormatAddon::FormatToRangePartsDoubleDecStrImpl(ani_env *env, ani_object object,
    ani_double start, ani_string end)
{
    I18nSymbolNumberFormatAddon* obj = UnwrapAddon(env, object);
    if (obj == nullptr || obj->symbolNumberFormat_ == nullptr) {
        HILOG_ERROR_I18N("I18nSymbolNumberFormatAddon::FormatDecStrImpl: UnwrapAddon object failed");
        return nullptr;
    }
    auto parts = obj->symbolNumberFormat_->FormatToRangePartsDoubleDecStr(start,
        VariableConverter::AniStrToString(env, end));
    return CreateNumberFormatPartArray(env, parts, true);
}

ani_object I18nSymbolNumberFormatAddon::FormatToRangePartsDecStrDoubleImpl(ani_env *env, ani_object object,
    ani_string start, ani_double end)
{
    I18nSymbolNumberFormatAddon* obj = UnwrapAddon(env, object);
    if (obj == nullptr || obj->symbolNumberFormat_ == nullptr) {
        HILOG_ERROR_I18N("I18nSymbolNumberFormatAddon::FormatDecStrImpl: UnwrapAddon object failed");
        return nullptr;
    }
    auto parts = obj->symbolNumberFormat_->FormatToRangePartsDecStrDouble(
        VariableConverter::AniStrToString(env, start), end);
    return CreateNumberFormatPartArray(env, parts, true);
}

ani_object I18nSymbolNumberFormatAddon::FormatToRangePartsDecStrDecStrImpl(ani_env *env, ani_object object,
    ani_string start, ani_string end)
{
    I18nSymbolNumberFormatAddon* obj = UnwrapAddon(env, object);
    if (obj == nullptr || obj->symbolNumberFormat_ == nullptr) {
        HILOG_ERROR_I18N("I18nSymbolNumberFormatAddon::FormatDecStrImpl: UnwrapAddon object failed");
        return nullptr;
    }
    auto parts = obj->symbolNumberFormat_->FormatToRangePartsDecStrDecStr(
        VariableConverter::AniStrToString(env, start), VariableConverter::AniStrToString(env, end));
    return CreateNumberFormatPartArray(env, parts, true);
}

ani_object I18nSymbolNumberFormatAddon::ResolvedOptions(ani_env *env, ani_object object)
{
    I18nSymbolNumberFormatAddon* obj = UnwrapAddon(env, object);
    if (obj == nullptr || obj->symbolNumberFormat_ == nullptr) {
        HILOG_ERROR_I18N("I18nSymbolNumberFormatAddon::ResolvedOptions: UnwrapAddon object failed");
        return nullptr;
    }
    std::map<std::string, std::string> options;
    obj->symbolNumberFormat_->ResolvedOptions(options);
    return CreateResolvedOptions(env, options);
}

ani_double I18nSymbolNumberFormatAddon::Parse(ani_env *env, ani_object object, ani_string text,
    ani_boolean lenientMode)
{
    I18nSymbolNumberFormatAddon* obj = UnwrapAddon(env, object);
    if (obj == nullptr) {
        HILOG_ERROR_I18N("I18nSymbolNumberFormatAddon::Parse: UnwrapAddon failed.");
        return 0;
    }
    if (obj->symbolNumberFormat_ == nullptr) {
        HILOG_ERROR_I18N("I18nSymbolNumberFormatAddon::Parse: symbolNumberFormat_ is nullptr.");
        return 0;
    }
    std::string textStr = VariableConverter::AniStrToString(env, text);
    I18nErrorCode status = I18nErrorCode::SUCCESS;
    double result = obj->symbolNumberFormat_->Parse(textStr, lenientMode, status);
    if (status == I18nErrorCode::FAILED) {
        HILOG_ERROR_I18N("I18nSymbolNumberFormatAddon::Parse: Framework returned FAILED (system error).");
        return 0;
    }
    if (status == I18nErrorCode::INVALID_PARAM) {
        ErrorUtil::AniThrow(env, I18N_NOT_VALID_NEW, "text", "a valid localized number string");
        return 0;
    }
    return result;
}

ani_string I18nSymbolNumberFormatAddon::ToString(ani_env *env, [[maybe_unused]] ani_object object)
{
    return VariableConverter::StringToAniStr(env, "[object i18n.SymbolNumberFormat]");
}

ani_object I18nSymbolNumberFormatAddon::CreateNumberFormatPartArray(ani_env *env,
    const NumberFormatParts &parts, bool isRange)
{
    ani_ref undefined {};
    ani_array result {};
    if (ANI_OK != env->GetUndefined(&undefined)) {
        HILOG_ERROR_I18N("CreateNumberFormatPartArray: GetUndefined failed.");
        return result;
    }
    if (ANI_OK != env->Array_New(parts.size(), undefined, &result)) {
        HILOG_ERROR_I18N("CreateNumberFormatPartArray: Create array failed.");
        return result;
    }

    for (size_t index = 0; index < parts.size(); ++index) {
        ani_object part;
        if (isRange) {
            part = CreateNumberRangeFormatPart(env, parts[index]);
        } else {
            part = CreateNumberFormatPart(env, parts[index]);
        }
        if (part == nullptr || ANI_OK != env->Array_Set(result, index, part)) {
            HILOG_ERROR_I18N("CreateNumberFormatPartArray: Set array failed.");
            return result;
        }
    }
    return result;
}

ani_object I18nSymbolNumberFormatAddon::CreateNumberFormatPart(ani_env *env,
    const std::unordered_map<std::string, std::string> &part)
{
    static const char *className = "std.core.Intl.NumberFormatPart";
    ani_class cls;
    if (ANI_OK != env->FindClass(className, &cls)) {
        HILOG_ERROR_I18N("CreateNumberFormatPart: Find class failed");
        return nullptr;
    }

    static const char *ctorSignature = "C{std.core.String}C{std.core.String}:";
    ani_method ctor;
    if (ANI_OK != env->Class_FindMethod(cls, "<ctor>", ctorSignature, &ctor)) {
        HILOG_ERROR_I18N("CreateNumberFormatPart: Find method '<ctor>' failed.");
        return nullptr;
    }

    auto typeIter = part.find(NumberFormat::PART_TYPE_TAG);
    auto valueIter = part.find(NumberFormat::PART_VALUE_TAG);
    if (typeIter == part.end() || valueIter == part.end()) {
        HILOG_ERROR_I18N("CreateNumberFormatPart: Invalid part.");
        return nullptr;
    }
    ani_string type = VariableConverter::StringToAniStr(env, typeIter->second);
    ani_string value = VariableConverter::StringToAniStr(env, valueIter->second);

    ani_object numberFormatPart;
    if (ANI_OK != env->Object_New(cls, ctor, &numberFormatPart, type, value)) {
        HILOG_ERROR_I18N("CreateNumberFormatPart: New object '%{public}s' failed.", className);
        return nullptr;
    }
    return numberFormatPart;
}

ani_object I18nSymbolNumberFormatAddon::CreateNumberRangeFormatPart(ani_env *env,
    const std::unordered_map<std::string, std::string> &part)
{
    static const char *className = "std.core.Intl.NumberRangeFormatPart";
    ani_class cls;
    if (ANI_OK != env->FindClass(className, &cls)) {
        HILOG_ERROR_I18N("CreateNumberRangeFormatPart: Find class failed");
        return nullptr;
    }

    static const char *ctorSignature = "C{std.core.String}C{std.core.String}C{std.core.String}:";
    ani_method ctor;
    if (ANI_OK != env->Class_FindMethod(cls, "<ctor>", ctorSignature, &ctor)) {
        HILOG_ERROR_I18N("CreateNumberRangeFormatPart: Find method '<ctor>' failed.");
        return nullptr;
    }

    auto typeIter = part.find(NumberFormat::PART_TYPE_TAG);
    auto valueIter = part.find(NumberFormat::PART_VALUE_TAG);
    auto sourceIter = part.find(NumberFormat::PART_SOURCE_TAG);
    if (typeIter == part.end() || valueIter == part.end() || sourceIter == part.end()) {
        HILOG_ERROR_I18N("CreateNumberFormatPart: Invalid part.");
        return nullptr;
    }
    ani_string type = VariableConverter::StringToAniStr(env, typeIter->second);
    ani_string value = VariableConverter::StringToAniStr(env, valueIter->second);
    ani_string source = VariableConverter::StringToAniStr(env, sourceIter->second);

    ani_object numberRangeFormatPart;
    if (ANI_OK != env->Object_New(cls, ctor, &numberRangeFormatPart, type, value, source)) {
        HILOG_ERROR_I18N("CreateNumberFormatPart: New object '%{public}s' failed.", className);
        return nullptr;
    }
    return numberRangeFormatPart;
}

ani_object I18nSymbolNumberFormatAddon::CreateResolvedOptions(ani_env *env,
    const std::map<std::string, std::string> &options)
{
    static const char *className = "@ohos.i18n.i18n.ResolvedSymbolNumberFormatOptionsInner";
    ani_class cls;
    if (ANI_OK != env->FindClass(className, &cls)) {
        HILOG_ERROR_I18N("I18nSymbolNumberFormatAddon::CreateResolvedOptions: Find class '%{public}s' failed.",
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
    std::unordered_set<std::string> keyOfTypeNumber = { "minimumIntegerDigits", "minimumFractionDigits",
        "maximumFractionDigits", "minimumSignificantDigits", "maximumSignificantDigits" };
    
    for (const auto& option : options) {
        if (keyOfTypeNumber.find(option.first) != keyOfTypeNumber.end()) {
            VariableConverter::SetObjectNumberMember(env, resolvedOptions, option.first,
                std::atoi(option.second.c_str()));
        } else if (option.first == "useGrouping") {
            VariableConverter::SetObjectBooleanMember(env, resolvedOptions, option.first, option.second == "true");
        } else {
            VariableConverter::SetStringMember(env, resolvedOptions, option.first, option.second);
        }
    }
    return resolvedOptions;
}

ani_status I18nSymbolNumberFormatAddon::BindContextSymbolNumberFormat(ani_env *env)
{
    static const char *className = "@ohos.i18n.i18n.SymbolNumberFormat";
    ani_class cls;
    if (ANI_OK != env->FindClass(className, &cls)) {
        HILOG_ERROR_I18N("BindContextSymbolNumberFormat: Find class '%{public}s' failed", className);
        return ANI_ERROR;
    }

    std::array methods = {
        ani_native_function { "formatDoubleImpl", nullptr,
            reinterpret_cast<void *>(I18nSymbolNumberFormatAddon::FormatDoubleImpl) },
        ani_native_function { "formatLongImpl", nullptr,
            reinterpret_cast<void *>(I18nSymbolNumberFormatAddon::FormatLongImpl) },
        ani_native_function { "formatDecStrImpl", nullptr,
            reinterpret_cast<void *>(I18nSymbolNumberFormatAddon::FormatDecStrImpl) },
        ani_native_function { "formatToPartsDoubleImpl", nullptr,
            reinterpret_cast<void *>(I18nSymbolNumberFormatAddon::FormatToPartsDoubleImpl) },
        ani_native_function { "formatToPartsDecStrImpl", nullptr,
            reinterpret_cast<void *>(I18nSymbolNumberFormatAddon::FormatToPartsDecStrImpl) },
        ani_native_function { "formatRangeDoubleDoubleImpl", nullptr,
            reinterpret_cast<void *>(I18nSymbolNumberFormatAddon::FormatRangeDoubleDoubleImpl) },
        ani_native_function { "formatRangeDoubleDecStrImpl", nullptr,
            reinterpret_cast<void *>(I18nSymbolNumberFormatAddon::FormatRangeDoubleDecStrImpl) },
        ani_native_function { "formatRangeDecStrDoubleImpl", nullptr,
            reinterpret_cast<void *>(I18nSymbolNumberFormatAddon::FormatRangeDecStrDoubleImpl) },
        ani_native_function { "formatRangeDecStrDecStrImpl", nullptr,
            reinterpret_cast<void *>(I18nSymbolNumberFormatAddon::FormatRangeDecStrDecStrImpl) },
        ani_native_function { "formatToRangePartsDoubleDoubleImpl", nullptr,
            reinterpret_cast<void *>(I18nSymbolNumberFormatAddon::FormatToRangePartsDoubleDoubleImpl) },
        ani_native_function { "formatToRangePartsDoubleDecStrImpl", nullptr,
            reinterpret_cast<void *>(I18nSymbolNumberFormatAddon::FormatToRangePartsDoubleDecStrImpl) },
        ani_native_function { "formatToRangePartsDecStrDoubleImpl", nullptr,
            reinterpret_cast<void *>(I18nSymbolNumberFormatAddon::FormatToRangePartsDecStrDoubleImpl) },
        ani_native_function { "formatToRangePartsDecStrDecStrImpl", nullptr,
            reinterpret_cast<void *>(I18nSymbolNumberFormatAddon::FormatToRangePartsDecStrDecStrImpl) },
        ani_native_function { "resolvedOptions", nullptr,
            reinterpret_cast<void *>(I18nSymbolNumberFormatAddon::ResolvedOptions) },
        ani_native_function { "parse", nullptr, reinterpret_cast<void *>(I18nSymbolNumberFormatAddon::Parse) },
        ani_native_function { "toString", nullptr, reinterpret_cast<void *>(I18nSymbolNumberFormatAddon::ToString) },
    };
    if (ANI_OK != env->Class_BindNativeMethods(cls, methods.data(), methods.size())) {
        HILOG_ERROR_I18N("BindContextSymbolNumberFormat: Cannot bind native methods to '%{public}s'", className);
        return ANI_ERROR;
    }

    std::array staticMethods = {
        ani_native_function { "create", nullptr, reinterpret_cast<void *>(I18nSymbolNumberFormatAddon::Create) },
    };
    if (ANI_OK != env->Class_BindStaticNativeMethods(cls, staticMethods.data(), staticMethods.size())) {
        HILOG_ERROR_I18N("BindContextSymbolNumberFormat: Cannot bind static native methods to '%{public}s'", className);
        return ANI_ERROR;
    }
    return ANI_OK;
}

bool I18nSymbolNumberFormatAddon::IsSymbolNumberFormat(ani_env *env, ani_object numberFormat)
{
    static const char *className = "@ohos.i18n.i18n.SymbolNumberFormat";
    ani_class cls;
    if (ANI_OK != env->FindClass(className, &cls)) {
        HILOG_ERROR_I18N("I18nSymbolNumberFormatAddon::IsSymbolNumberFormat: Find class failed.");
        return false;
    }

    ani_boolean isSymbolNumberFormat;
    if (ANI_OK != env->Object_InstanceOf(numberFormat, cls, &isSymbolNumberFormat)) {
        HILOG_ERROR_I18N("I18nSymbolNumberFormatAddon::IsSymbolNumberFormat: Get Instance failed");
        return false;
    }
    return isSymbolNumberFormat;
}

std::shared_ptr<SymbolNumberFormat> I18nSymbolNumberFormatAddon::GetNumberFormat()
{
    return symbolNumberFormat_;
}
} // namespace I18n
} // namespace Global
} // namespace OHOS
