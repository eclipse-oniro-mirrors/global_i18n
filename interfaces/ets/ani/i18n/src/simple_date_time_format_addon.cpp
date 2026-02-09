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

#include "simple_date_time_format_addon.h"

#include "error_util.h"
#include "i18n_hilog.h"
#include "variable_converter.h"

using namespace OHOS;
using namespace Global;
using namespace I18n;

I18nSimpleDateTimeFormatAddon* I18nSimpleDateTimeFormatAddon::UnwrapAddon(ani_env *env, ani_object object)
{
    ani_long ptr;
    if (ANI_OK != env->Object_GetFieldByName_Long(object, "nativeSimpleDateTimeFormat", &ptr)) {
        HILOG_ERROR_I18N("Get Long 'nativeSimpleDateTimeFormat' failed");
        return nullptr;
    }
    return reinterpret_cast<I18nSimpleDateTimeFormatAddon*>(ptr);
}

ani_object I18nSimpleDateTimeFormatAddon::GetSimpleDateTimeFormatByPattern(ani_env *env,
    ani_string pattern, ani_object locale)
{
    ani_boolean isUndefined;
    if (ANI_OK != env->Reference_IsUndefined(locale, &isUndefined)) {
        HILOG_ERROR_I18N("GetSimpleDateTimeFormatByPattern: Check locale reference is undefined failed");
        return nullptr;
    }

    std::string localeTag = "";
    if (!isUndefined) {
        localeTag = VariableConverter::GetLocaleTagFromBuiltinLocale(env, locale);
    }
    std::shared_ptr<LocaleInfo> localeInfo = localeTag.empty() ? nullptr : std::make_shared<LocaleInfo>(localeTag);

    std::unique_ptr<I18nSimpleDateTimeFormatAddon> obj = std::make_unique<I18nSimpleDateTimeFormatAddon>();
    if (obj == nullptr) {
        HILOG_ERROR_I18N("GetSimpleDateTimeFormatByPattern: Create I18nSimpleDateTimeFormatAddon failed.");
        return nullptr;
    }

    I18nErrorCode errCode = I18nErrorCode::SUCCESS;
    obj->simpleDateTimeFormat_ = std::make_shared<SimpleDateTimeFormat>(
        VariableConverter::AniStrToString(env, pattern), localeInfo, true, errCode);
    if (errCode == I18nErrorCode::INVALID_DATE_TIME_FORMAT_SKELETON) {
        ErrorUtil::AniThrow(env, I18N_NOT_VALID, "skeleton", "a valid date time format skeleton");
        return nullptr;
    } else if (errCode == I18nErrorCode::INVALID_DATE_TIME_FORMAT_PATTERN) {
        ErrorUtil::AniThrow(env, I18N_NOT_VALID, "pattern", "a valid date time format pattern");
        return nullptr;
    } else if (!obj->simpleDateTimeFormat_ || errCode != I18nErrorCode::SUCCESS) {
        HILOG_ERROR_I18N("GetSimpleDateTimeFormatByPattern: Construct SimpleDateTimeFormat failed.");
        return nullptr;
    }

    static const char *className = "@ohos.i18n.i18n.SimpleDateTimeFormat";
    ani_object simpleDateTimeFormatObject = VariableConverter::CreateAniObject(env, className, obj.get());
    obj.release();
    return simpleDateTimeFormatObject;
}

ani_object I18nSimpleDateTimeFormatAddon::GetSimpleDateTimeFormatBySkeleton(ani_env *env,
    ani_string skeleton, ani_object locale)
{
    ani_boolean isUndefined;
    if (ANI_OK != env->Reference_IsUndefined(locale, &isUndefined)) {
        HILOG_ERROR_I18N("GetSimpleDateTimeFormatBySkeleton: Check locale reference is undefined failed");
        return nullptr;
    }
    std::string localeTag = "";
    if (!isUndefined) {
        localeTag = VariableConverter::GetLocaleTagFromBuiltinLocale(env, locale);
    }
    std::shared_ptr<LocaleInfo> localeInfo = localeTag.empty() ? nullptr : std::make_shared<LocaleInfo>(localeTag);

    std::unique_ptr<I18nSimpleDateTimeFormatAddon> obj = std::make_unique<I18nSimpleDateTimeFormatAddon>();
    if (obj == nullptr) {
        HILOG_ERROR_I18N("GetSimpleDateTimeFormatBySkeleton: Create I18nSimpleDateTimeFormatAddon failed.");
        return nullptr;
    }

    I18nErrorCode errCode = I18nErrorCode::SUCCESS;
    obj->simpleDateTimeFormat_ = std::make_shared<SimpleDateTimeFormat>(
        VariableConverter::AniStrToString(env, skeleton), localeInfo, false, errCode);
    if (errCode == I18nErrorCode::INVALID_DATE_TIME_FORMAT_SKELETON) {
        ErrorUtil::AniThrow(env, I18N_NOT_VALID, "skeleton", "a valid date time format skeleton");
        return nullptr;
    } else if (errCode == I18nErrorCode::INVALID_DATE_TIME_FORMAT_PATTERN) {
        ErrorUtil::AniThrow(env, I18N_NOT_VALID, "pattern", "a valid date time format pattern");
        return nullptr;
    } else if (!obj->simpleDateTimeFormat_ || errCode != I18nErrorCode::SUCCESS) {
        HILOG_ERROR_I18N("GetSimpleDateTimeFormatBySkeleton: Construct SimpleDateTimeFormat failed.");
        return nullptr;
    }

    static const char *className = "@ohos.i18n.i18n.SimpleDateTimeFormat";
    ani_object simpleDateTimeFormatObject = VariableConverter::CreateAniObject(env, className, obj.get());
    obj.release();
    return simpleDateTimeFormatObject;
}

ani_string I18nSimpleDateTimeFormatAddon::Format(ani_env *env, ani_object object, ani_object date)
{
    I18nSimpleDateTimeFormatAddon* obj = UnwrapAddon(env, object);
    if (obj == nullptr || obj->simpleDateTimeFormat_ == nullptr) {
        HILOG_ERROR_I18N("I18nSimpleDateTimeFormatAddon::Format: unwrap SimpleDateTimeFormatAddon failed.");
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

    ani_long milliseconds;
    if (ANI_OK != env->Object_CallMethod_Long(date, getms, &milliseconds)) {
        HILOG_ERROR_I18N("I18nSimpleDateTimeFormatAddon::Format: Call method 'valueOf' failed");
        return nullptr;
    }

    std::string formatResult = obj->simpleDateTimeFormat_->Format(
        static_cast<double>(milliseconds));
    if (formatResult.empty()) {
        HILOG_ERROR_I18N("I18nSimpleDateTimeFormatAddon::Format: format result is empty.");
    }
    return VariableConverter::StringToAniStr(env, formatResult);
}

ani_status I18nSimpleDateTimeFormatAddon::BindContextSimpleDateTimeFormat(ani_env *env)
{
    static const char *className = "@ohos.i18n.i18n.SimpleDateTimeFormat";
    ani_class cls;
    if (ANI_OK != env->FindClass(className, &cls)) {
        HILOG_ERROR_I18N("BindContextSimpleDateTimeFormat: Find class '%{public}s' failed", className);
        return ANI_ERROR;
    }

    std::array methods = {
        ani_native_function { "format", nullptr, reinterpret_cast<void *>(I18nSimpleDateTimeFormatAddon::Format) },
    };

    if (ANI_OK != env->Class_BindNativeMethods(cls, methods.data(), methods.size())) {
        HILOG_ERROR_I18N("BindContextSimpleDateTimeFormat: Cannot bind native methods to '%{public}s'", className);
        return ANI_ERROR;
    };

    return ANI_OK;
}

std::shared_ptr<SimpleDateTimeFormat> I18nSimpleDateTimeFormatAddon::GetDateTimeFormat()
{
    return simpleDateTimeFormat_;
}
