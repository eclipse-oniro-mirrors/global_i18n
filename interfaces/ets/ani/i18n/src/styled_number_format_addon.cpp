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

#include "styled_number_format_addon.h"

#include "error_util.h"
#include "i18n_hilog.h"
#include "variable_converter.h"

namespace OHOS {
namespace Global {
namespace I18n {
const std::unordered_map<std::string, std::string> I18nStyledNumberFormatAddon::PROPERTY_NAME_TO_KEY = {
    { "_locale", "locale" },
    { "_compactDisplay", "compactDisplay" },
    { "_currencySign", "currencySign" },
    { "_currency", "currency" },
    { "_currencyDisplay", "currencyDisplay" },
    { "minFracStr", "minimumFractionDigits" },
    { "maxFracStr", "maximumFractionDigits" },
    { "minSignStr", "minimumSignificantDigits" },
    { "maxSignStr", "maximumSignificantDigits" },
    { "minIntStr", "minimumIntegerDigits" },
    { "_notation", "notation" },
    { "_numberingSystem", "numberingSystem" },
    { "_signDisplay", "signDisplay" },
    { "_style", "style" },
    { "_unit", "unit" },
    { "_unitDisplay", "unitDisplay" },
    { "useGroupingStr", "useGrouping" },
};

I18nStyledNumberFormatAddon* I18nStyledNumberFormatAddon::UnwrapAddon(ani_env *env, ani_object object)
{
    ani_long ptr;
    if (ANI_OK != env->Object_GetFieldByName_Long(object, "nativeStyledNumberFormat", &ptr)) {
        HILOG_ERROR_I18N("Get Long 'nativeStyledNumberFormat' failed");
        return nullptr;
    }
    return reinterpret_cast<I18nStyledNumberFormatAddon*>(ptr);
}

bool I18nStyledNumberFormatAddon::InitStyledNumberFormatContent(ani_env *env, ani_object numberFormat)
{
    ani_class simpleNumberFormatClass;
    if (ANI_OK != env->FindClass("@ohos.i18n.i18n.SimpleNumberFormat", &simpleNumberFormatClass)) {
        HILOG_ERROR_I18N("InitStyledNumberFormatContent Find class '@ohos.i18n.i18n.SimpleNumberFormat' failed");
        return false;
    }

    ani_boolean isSimpleNumberFormat;
    if (ANI_OK != env->Object_InstanceOf(numberFormat, simpleNumberFormatClass, &isSimpleNumberFormat)) {
        HILOG_ERROR_I18N("InitStyledNumberFormatContent: Get Instance failed");
        return false;
    }
    if (isSimpleNumberFormat) {
        I18nSimpleNumberFormatAddon* simpleNumberFormatAddon =
            I18nSimpleNumberFormatAddon::UnwrapAddon(env, numberFormat);
        styledNumberFormat_ = std::make_unique<StyledNumberFormat>(false, nullptr,
            simpleNumberFormatAddon->CopySimpleNumberFormat());
        return styledNumberFormat_ != nullptr;
    }
    std::map<std::string, std::string> options = ParseIntlOptions(env, numberFormat);
    auto iter = options.find("locale");
    if (iter == options.end()) {
        ErrorUtil::AniThrow(env, I18N_NOT_FOUND, "numberFormat", "Intl.NumberFormat or SimpleNumberFormat");
        return false;
    }
    numberFormat_ = std::make_shared<NumberFormat>(std::vector<std::string>{iter->second}, options);
    if (!numberFormat_) {
        HILOG_ERROR_I18N("InitStyledNumberFormatContent: Create NumberFormat failed");
        return false;
    }
    styledNumberFormat_ = std::make_unique<StyledNumberFormat>(true, numberFormat_, nullptr);
    return styledNumberFormat_ != nullptr;
}

ani_object I18nStyledNumberFormatAddon::Create(ani_env *env, [[maybe_unused]] ani_object object,
    ani_object numberFormat)
{
    std::unique_ptr<I18nStyledNumberFormatAddon> obj = std::make_unique<I18nStyledNumberFormatAddon>();
    if (!obj) {
        HILOG_ERROR_I18N("I18nStyledNumberFormatAddon::Create make unique ptr error");
        return nullptr;
    }

    if (!obj->InitStyledNumberFormatContent(env, numberFormat)) {
        HILOG_ERROR_I18N("I18nStyledNumberFormatAddon::Create Init StyledNumberFormat error");
        return nullptr;
    }

    static const char *className = "@ohos.i18n.i18n.StyledNumberFormat";
    ani_object styledNumberFormatObject = VariableConverter::CreateAniObject(env, className, obj.get());
    obj.release();
    return styledNumberFormatObject;
}

static ani_object CreateStyledPart(ani_env *env, StyledNumberFormat::NumberPart numberPart)
{
    static const char *className = "@ohos.i18n.i18n.StyledPartInner";
    ani_class cls;
    if (ANI_OK != env->FindClass(className, &cls)) {
        HILOG_ERROR_I18N("CreateStyledPart: Find class '%{public}s' failed", className);
        return nullptr;
    }

    ani_method ctor;
    if (ANI_OK != env->Class_FindMethod(cls, "<ctor>", ":", &ctor)) {
        HILOG_ERROR_I18N("CreateStyledPart: Find method '<ctor>' failed");
        return nullptr;
    }

    ani_object styledPart;
    if (ANI_OK != env->Object_New(cls, ctor, &styledPart)) {
        HILOG_ERROR_I18N("CreateStyledPart: New object '%{public}s' failed", className);
        return nullptr;
    }

    auto status = env->Object_SetPropertyByName_Int(styledPart, "start", static_cast<int32_t>(numberPart.start));
    if (status != ANI_OK) {
        HILOG_ERROR_I18N("CreateStyledPart: Set property 'start' failed, status: %{public}d", status);
        return nullptr;
    }
    status = env->Object_SetPropertyByName_Int(styledPart, "length", static_cast<int32_t>(numberPart.length));
    if (status != ANI_OK) {
        HILOG_ERROR_I18N("CreateStyledPart: Set property 'length' failed, status: %{public}d", status);
        return nullptr;
    }
    VariableConverter::SetStringMember(env, styledPart, "type", numberPart.part_name);
    return styledPart;
}

static ani_object CreateStyledPartArray(ani_env *env, const std::vector<StyledNumberFormat::NumberPart> &numberParts)
{
    ani_array ret;
    ani_ref undef;
    if (ANI_OK != env->GetUndefined(&undef)) {
        HILOG_ERROR_I18N("CreateStyledPartArray: GetUndefined failed");
        return nullptr;
    }
    if (ANI_OK != env->Array_New(numberParts.size(), undef, &ret)) {
        HILOG_ERROR_I18N("CreateStyledPartArray: Create new array failed");
        return nullptr;
    }
    for (size_t index = 0; index < numberParts.size(); ++index) {
        ani_object styledPart = CreateStyledPart(env, numberParts[index]);
        if (ANI_OK != env->Array_Set(ret, index, styledPart)) {
            HILOG_ERROR_I18N("CreateStyledPartArray: Call method 'Array_Set' failed");
            return nullptr;
        }
    }
    return ret;
}

ani_string I18nStyledNumberFormatAddon::GetFormattedNumber(ani_env *env, ani_object object, ani_double value)
{
    I18nStyledNumberFormatAddon* obj = UnwrapAddon(env, object);
    if (obj == nullptr || obj->styledNumberFormat_  == nullptr) {
        HILOG_ERROR_I18N("I18nStyledNumberFormatAddon::GetFormattedNumber: Get StyledNumberFormat object failed");
        return nullptr;
    }
    return VariableConverter::StringToAniStr(env, obj->styledNumberFormat_->Format(value));
}

ani_object I18nStyledNumberFormatAddon::GetStyle(ani_env *env, ani_object object, ani_double value)
{
    I18nStyledNumberFormatAddon* obj = UnwrapAddon(env, object);
    if (obj == nullptr || obj->styledNumberFormat_  == nullptr) {
        HILOG_ERROR_I18N("I18nStyledNumberFormatAddon::GetStyle: Get StyledNumberFormat object failed");
        return nullptr;
    }
    std::vector<StyledNumberFormat::NumberPart> numberParts = obj->styledNumberFormat_->ParseToParts(value);
    return CreateStyledPartArray(env, numberParts);
}

std::map<std::string, std::string> I18nStyledNumberFormatAddon::ParseIntlOptions(ani_env *env, ani_object obj)
{
    std::map<std::string, std::string> options = {};
    ani_ref optionsRef = nullptr;
    auto status = env->Object_GetFieldByName_Ref(obj, "options", &optionsRef);
    if (status != ANI_OK) {
        HILOG_ERROR_I18N("I18nStyledNumberFormatAddon::ParseIntlOptions: Get options failed");
        return options;
    }
    ani_object optionsObj = static_cast<ani_object>(optionsRef);
    if (!optionsObj) {
        HILOG_ERROR_I18N("I18nStyledNumberFormatAddon::ParseIntlOptions: Static to object failed");
        return options;
    }

    for (auto iter = PROPERTY_NAME_TO_KEY.begin(); iter != PROPERTY_NAME_TO_KEY.end(); iter++) {
        std::string property = "";
        if (!VariableConverter::GetStringMember(env, optionsObj, iter->first, property)) {
            continue;
        }
        if (property.empty()) {
            continue;
        }
        options[iter->second] = property;
    }
    return options;
}

ani_status I18nStyledNumberFormatAddon::BindContextStyledNumberFormat(ani_env *env)
{
    static const char *className = "@ohos.i18n.i18n.StyledNumberFormat";
    ani_class cls;
    if (ANI_OK != env->FindClass(className, &cls)) {
        HILOG_ERROR_I18N("BindContextStyledNumberFormat: Find class '%{public}s' failed", className);
        return ANI_ERROR;
    }

    std::array staticMethods = {
        ani_native_function { "create", nullptr, reinterpret_cast<void *>(I18nStyledNumberFormatAddon::Create) },
    };
    if (ANI_OK != env->Class_BindStaticNativeMethods(cls, staticMethods.data(), staticMethods.size())) {
        HILOG_ERROR_I18N("Cannot bind static native methods to '%{public}s'", className);
        return ANI_ERROR;
    }

    std::array methods = {
        ani_native_function { "getFormattedNumber", nullptr,
            reinterpret_cast<void *>(I18nStyledNumberFormatAddon::GetFormattedNumber) },
        ani_native_function { "getStyle", nullptr, reinterpret_cast<void *>(I18nStyledNumberFormatAddon::GetStyle) },
    };
    if (ANI_OK != env->Class_BindNativeMethods(cls, methods.data(), methods.size())) {
        HILOG_ERROR_I18N("BindContextStyledNumberFormat: Cannot bind native methods to '%{public}s'", className);
        return ANI_ERROR;
    }
    return ANI_OK;
}
} // namespace I18n
} // namespace Global
} // namespace OHOS
