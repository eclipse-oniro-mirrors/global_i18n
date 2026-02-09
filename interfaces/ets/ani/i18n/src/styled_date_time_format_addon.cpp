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

#include "styled_date_time_format_addon.h"

#include "error_util.h"
#include "i18n_hilog.h"
#include "simple_date_time_format_addon.h"
#include "variable_converter.h"

namespace OHOS {
namespace Global {
namespace I18n {
I18nStyledDateTimeFormatAddon* I18nStyledDateTimeFormatAddon::UnwrapAddon(ani_env *env, ani_object object)
{
    ani_long ptr;
    if (ANI_OK != env->Object_GetFieldByName_Long(object, "nativeStyledDateTimeFormat", &ptr)) {
        HILOG_ERROR_I18N("Get Long 'nativeStyledDateTimeFormat' failed");
        return nullptr;
    }
    return reinterpret_cast<I18nStyledDateTimeFormatAddon*>(ptr);
}

bool I18nStyledDateTimeFormatAddon::InitStyledFormatContext(ani_env *env, ani_object dateTimeFormat)
{
    ani_class simpleDateTimeFormatClass;
    if (ANI_OK != env->FindClass("@ohos.i18n.i18n.SimpleDateTimeFormat", &simpleDateTimeFormatClass)) {
        HILOG_ERROR_I18N("InitStyledFormatContext Find class '@ohos.i18n.i18n.SimpleDateTimeFormat' failed");
        return false;
    }

    ani_boolean isSimpleDateTimeFormat;
    if (ANI_OK != env->Object_InstanceOf(dateTimeFormat, simpleDateTimeFormatClass, &isSimpleDateTimeFormat)) {
        HILOG_ERROR_I18N("InitStyledFormatContext: Get Instance failed");
        return false;
    }
    if (isSimpleDateTimeFormat) {
        I18nSimpleDateTimeFormatAddon* simpleDateTimeFormatAddon =
            I18nSimpleDateTimeFormatAddon::UnwrapAddon(env, dateTimeFormat);
        if (simpleDateTimeFormatAddon == nullptr) {
            HILOG_ERROR_I18N("InitStyledFormatContext: simpleDateTimeFormatAddon is nullptr.");
            return false;
        }
        styledDateTimeFormat_ = std::make_unique<StyledDateTimeFormat>(simpleDateTimeFormatAddon->GetDateTimeFormat(),
            nullptr);
        return styledDateTimeFormat_ != nullptr;
    }
    ErrorUtil::AniThrow(env, I18N_NOT_FOUND, "dateTimeFormat", "Intl.DateTimeFormat or SimpleDateTimeFormat");
    return false;
}

ani_object I18nStyledDateTimeFormatAddon::Create(ani_env *env, [[maybe_unused]] ani_object object,
    ani_object dateTimeFormat)
{
    std::unique_ptr<I18nStyledDateTimeFormatAddon> obj = std::make_unique<I18nStyledDateTimeFormatAddon>();
    if (!obj) {
        HILOG_ERROR_I18N("I18nStyledDateTimeFormatAddon::Create make unique ptr error");
        return nullptr;
    }

    if (!obj->InitStyledFormatContext(env, dateTimeFormat)) {
        HILOG_ERROR_I18N("I18nStyledDateTimeFormatAddon::Create Init StyledDateTimeFormat error");
        return nullptr;
    }

    static const char *className = "@ohos.i18n.i18n.StyledDateTimeFormat";
    ani_object styledFormatObject = VariableConverter::CreateAniObject(env, className, obj.get());
    obj.release();
    return styledFormatObject;
}

ani_status I18nStyledDateTimeFormatAddon::BindContextStyledDateTimeFormat(ani_env *env)
{
    static const char *className = "@ohos.i18n.i18n.StyledDateTimeFormat";
    ani_class cls;
    if (ANI_OK != env->FindClass(className, &cls)) {
        HILOG_ERROR_I18N("BindContextStyledDateTimeFormat: Find class '%{public}s' failed", className);
        return ANI_ERROR;
    }

    std::array staticMethods = {
        ani_native_function { "create", nullptr, reinterpret_cast<void *>(I18nStyledDateTimeFormatAddon::Create) },
    };
    if (ANI_OK != env->Class_BindStaticNativeMethods(cls, staticMethods.data(), staticMethods.size())) {
        HILOG_ERROR_I18N("BindContextStyledDateTimeFormat: Cannot bind static native methods to '%{public}s'",
            className);
        return ANI_ERROR;
    }

    std::array methods = {
        ani_native_function { "formatToPats", nullptr,
            reinterpret_cast<void *>(I18nStyledDateTimeFormatAddon::FormatToPats) },
    };
    if (ANI_OK != env->Class_BindNativeMethods(cls, methods.data(), methods.size())) {
        HILOG_ERROR_I18N("BindContextStyledDateTimeFormat: Cannot bind native methods to '%{public}s'", className);
        return ANI_ERROR;
    }
    return ANI_OK;
}

static ani_object CreateStyledPart(ani_env *env, DateTimeFormatPart part)
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

    auto status = env->Object_SetPropertyByName_Int(styledPart, "start", static_cast<int32_t>(part.GetStart()));
    if (status != ANI_OK) {
        HILOG_ERROR_I18N("CreateStyledPart: Set property 'start' failed, status: %{public}d", status);
        return nullptr;
    }
    status = env->Object_SetPropertyByName_Int(styledPart, "length", static_cast<int32_t>(part.GetLength()));
    if (status != ANI_OK) {
        HILOG_ERROR_I18N("CreateStyledPart: Set property 'length' failed, status: %{public}d", status);
        return nullptr;
    }
    VariableConverter::SetStringMember(env, styledPart, "type", part.GetPartName());
    return styledPart;
}

static ani_object CreateStyledPartArray(ani_env *env, const std::vector<DateTimeFormatPart>& parts)
{
    static const char *className = "std.core.Array";
    ani_class cls;
    if (ANI_OK != env->FindClass(className, &cls)) {
        HILOG_ERROR_I18N("CreateStyledPartArray: Find class '%{public}s' failed", className);
        return nullptr;
    }

    ani_method ctor;
    if (ANI_OK != env->Class_FindMethod(cls, "<ctor>", "i:", &ctor)) {
        HILOG_ERROR_I18N("CreateStyledPartArray: Find constructor of Array failed");
        return nullptr;
    }

    ani_object ret;
    if (ANI_OK != env->Object_New(cls, ctor, &ret, parts.size())) {
        HILOG_ERROR_I18N("CreateStyledPartArray: New object '%{public}s' failed", className);
        return nullptr;
    }

    ani_method set;
    if (ANI_OK != env->Class_FindMethod(cls, "$_set", "iY:", &set)) {
        HILOG_ERROR_I18N("CreateStyledPartArray: Find method '$_set' of Array failed");
        return nullptr;
    }

    for (size_t index = 0; index < parts.size(); ++index) {
        ani_object styledPart = CreateStyledPart(env, parts[index]);
        if (ANI_OK != env->Object_CallMethod_Void(ret, set, index, styledPart)) {
            HILOG_ERROR_I18N("CreateStyledPartArray: Call method '$_set' failed");
            return nullptr;
        }
    }
    return ret;
}

static ani_object CreateStyledFormatResult(ani_env *env, const std::pair<std::string,
    std::vector<DateTimeFormatPart>>& formatResult)
{
    static const char *className = "@ohos.i18n.i18n.StyledFormatResult";
    ani_class cls;
    if (ANI_OK != env->FindClass(className, &cls)) {
        HILOG_ERROR_I18N("CreateStyledFormatResult: Find class '%{public}s' failed", className);
        return nullptr;
    }

    ani_method ctor;
    if (ANI_OK != env->Class_FindMethod(cls, "<ctor>", nullptr, &ctor)) {
        HILOG_ERROR_I18N("CreateStyledFormatResult: Find method '<ctor>' failed");
        return nullptr;
    }

    ani_object styledFormatResult;
    if (ANI_OK != env->Object_New(cls, ctor, &styledFormatResult)) {
        HILOG_ERROR_I18N("CreateStyledFormatResult: New object '%{public}s' failed", className);
        return nullptr;
    }

    ani_object styledParts = CreateStyledPartArray(env, formatResult.second);
    VariableConverter::SetStringMember(env, styledFormatResult, "content", formatResult.first);
    if (ANI_OK != env->Object_SetPropertyByName_Ref(styledFormatResult, "styledParts", styledParts)) {
        return nullptr;
    }
    return styledFormatResult;
}

ani_object I18nStyledDateTimeFormatAddon::FormatToPats(ani_env *env, ani_object object, ani_object date)
{
    I18nStyledDateTimeFormatAddon* ptr = I18nStyledDateTimeFormatAddon::UnwrapAddon(env, object);
    if (ptr == nullptr || ptr->styledDateTimeFormat_ == nullptr) {
        return nullptr;
    }

    static const char *className = "std.core.Date";
    ani_class cls;
    if (ANI_OK != env->FindClass(className, &cls)) {
        HILOG_ERROR_I18N("I18nSimpleDateTimeFormatAddon::Format: Find class '%{public}s' failed", className);
        return nullptr;
    }

    ani_method getms;
    if (ANI_OK != env->Class_FindMethod(cls, "valueOf", nullptr, &getms)) {
        HILOG_ERROR_I18N("I18nSimpleDateTimeFormatAddon::Format: Find method 'valueOf' failed");
        return nullptr;
    }

    ani_double milliseconds;
    if (ANI_OK != env->Object_CallMethod_Double(date, getms, &milliseconds)) {
        HILOG_ERROR_I18N("I18nSimpleDateTimeFormatAddon::Format: Call method 'valueOf' failed");
        return nullptr;
    }
    auto formatResult = ptr->styledDateTimeFormat_->Format(milliseconds);
    return CreateStyledFormatResult(env, formatResult);
}
} // namespace I18n
} // namespace Global
} // namespace OHOS
