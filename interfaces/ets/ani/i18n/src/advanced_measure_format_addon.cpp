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
#include "advanced_measure_format_addon.h"

#include "error_util.h"
#include "i18n_hilog.h"
#include "styled_number_format_addon.h"
#include "variable_converter.h"

namespace OHOS {
namespace Global {
namespace I18n {
AdvancedMeasureFormatAddon* AdvancedMeasureFormatAddon::UnwrapAddon(ani_env *env, ani_object object)
{
    ani_long ptr;
    if (ANI_OK != env->Object_GetFieldByName_Long(object, "nativeAdvancedMeasureFormat", &ptr)) {
        HILOG_ERROR_I18N("Get Long 'nativeAdvancedMeasureFormat' failed");
        return nullptr;
    }
    return reinterpret_cast<AdvancedMeasureFormatAddon*>(ptr);
}

ani_object AdvancedMeasureFormatAddon::Create(ani_env *env, [[maybe_unused]] ani_object object,
    ani_object numberFormat, ani_object options)
{
    std::unique_ptr<AdvancedMeasureFormatAddon> obj = std::make_unique<AdvancedMeasureFormatAddon>();
    if (!obj->InitContext(env, numberFormat, options)) {
        HILOG_ERROR_I18N("AdvancedMeasureFormatAddon::Create: Init context failed.");
        return nullptr;
    }

    static const char *className = "@ohos.i18n.i18n.AdvancedMeasureFormat";
    ani_object advancedMeasureFormatObject = VariableConverter::CreateAniObject(env, className, obj.get());
    obj.release();
    return advancedMeasureFormatObject;
}

bool AdvancedMeasureFormatAddon::InitContext(ani_env *env, ani_object numberFormat, ani_object options)
{
    std::map<std::string, std::string> numberFormatOptions =
        I18nStyledNumberFormatAddon::ParseIntlOptions(env, numberFormat);
    auto iter = numberFormatOptions.find("locale");
    if (iter == numberFormatOptions.end()) {
        ErrorUtil::AniThrow(env, I18N_NOT_FOUND, "numberFormat", "Intl.NumberFormat");
        return false;
    }
    auto nativeNumberFormat =
        std::make_shared<NumberFormat>(std::vector<std::string>{ iter->second }, numberFormatOptions);

    AdvancedMeasureFormatOptions advancedMeasureOptions = ParseAdvancedMeasureOptions(env, options);
    advancedMeasureFormat_ = std::make_shared<AdvancedMeasureFormat>(nativeNumberFormat, advancedMeasureOptions);
    return advancedMeasureFormat_ != nullptr;
}

AdvancedMeasureFormatOptions AdvancedMeasureFormatAddon::ParseAdvancedMeasureOptions(ani_env *env, ani_object options)
{
    AdvancedMeasureFormatOptions result;
    int32_t unitUsage;
    if (VariableConverter::GetEnumIndexMember(env, options, "unitUsage", unitUsage)) {
        result.unitUsage = static_cast<UnitUsage>(unitUsage);
    }
    return result;
}

ani_string AdvancedMeasureFormatAddon::FormatNumber(ani_env *env, ani_object object, ani_double value)
{
    AdvancedMeasureFormatAddon *obj = UnwrapAddon(env, object);
    if (obj == nullptr || obj->advancedMeasureFormat_ == nullptr) {
        HILOG_ERROR_I18N("AdvancedMeasureFormatAddon::FormatNumber: Unwrap AdvancedMeasureFormatAddon failed.");
        return nullptr;
    }

    std::string formatResult = obj->advancedMeasureFormat_->FormatNumber(value);
    return VariableConverter::StringToAniStr(env, formatResult);
}

ani_status AdvancedMeasureFormatAddon::BindContextAdvancedMeasureFormat(ani_env *env)
{
    static const char *className = "@ohos.i18n.i18n.AdvancedMeasureFormat";
    ani_class cls;
    if (ANI_OK != env->FindClass(className, &cls)) {
        HILOG_ERROR_I18N("BindContextAdvancedMeasureFormat: Find class '%{public}s' failed", className);
        return ANI_ERROR;
    }

    std::array staticMethods = {
        ani_native_function { "create", nullptr, reinterpret_cast<void *>(AdvancedMeasureFormatAddon::Create) },
    };
    if (ANI_OK != env->Class_BindStaticNativeMethods(cls, staticMethods.data(), staticMethods.size())) {
        HILOG_ERROR_I18N("BindContextAdvancedMeasureFormat: Cannot bind static native methods to '%{public}s'",
            className);
        return ANI_ERROR;
    }

    std::array methods = {
        ani_native_function { "formatNumber", nullptr,
            reinterpret_cast<void *>(AdvancedMeasureFormatAddon::FormatNumber) },
    };
    if (ANI_OK != env->Class_BindNativeMethods(cls, methods.data(), methods.size())) {
        HILOG_ERROR_I18N("BindContextAdvancedMeasureFormat: Cannot bind native methods to '%{public}s'", className);
        return ANI_ERROR;
    }
    return ANI_OK;
}
} // namespace I18n
} // namespace Global
} // namespace OHOS
