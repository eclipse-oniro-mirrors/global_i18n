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

#include "iso8601_date_time_format_addon.h"

#include "error_util.h"
#include "i18n_hilog.h"
#include "variable_converter.h"

using namespace OHOS;
using namespace Global;
using namespace I18n;

ISO8601DateTimeFormatAddon* ISO8601DateTimeFormatAddon::UnwrapAddon(ani_env *env, ani_object object)
{
    ani_long ptr;
    if (ANI_OK != env->Object_GetFieldByName_Long(object, "nativeISO8601DateTimeFormat", &ptr)) {
        HILOG_ERROR_I18N("Get Long 'nativeISO8601DateTimeFormat' failed");
        return nullptr;
    }
    return reinterpret_cast<ISO8601DateTimeFormatAddon*>(ptr);
}

std::string ISO8601DateTimeFormatAddon::ParseTimezone(ani_env *env, ani_object timeZone)
{
    if (timeZone == nullptr) {
        return "UTC";
    }
    ani_class cls;
    if (ANI_OK != env->FindClass("@ohos.i18n.i18n.TimeZone", &cls)) {
        HILOG_ERROR_I18N("ParseTimezone: Find class 'TimeZone' failed");
        return "UTC";
    }

    ani_method getms;
    if (ANI_OK != env->Class_FindMethod(cls, "getID", ":C{std.core.String}", &getms)) {
        HILOG_ERROR_I18N("ParseTimezone: Find method 'getID' failed");
        return "UTC";
    }

    ani_ref id;
    if (ANI_OK != env->Object_CallMethod_Ref(timeZone, getms, &id)) {
        HILOG_ERROR_I18N("ParseTimezone: Call method 'getID' failed");
        return "UTC";
    }
    return VariableConverter::AniStrToString(env, id);
}

ani_object ISO8601DateTimeFormatAddon::Create(ani_env *env, [[maybe_unused]] ani_object object, ani_object options)
{
    ani_boolean isUndefined;
    if (ANI_OK != env->Reference_IsUndefined(options, &isUndefined)) {
        HILOG_ERROR_I18N("ISO8601DateTimeFormat::Create: Check options reference is undefined failed");
        return nullptr;
    }

    // Default values
    std::string dateFormat = "calendar";
    std::string timePrecision = "seconds";
    std::string separatorStyle = "extended";
    std::string timeZone = "UTC";
    bool displayTimeZone = true;

    if (!isUndefined) {
        // Get dateFormat, timePrecision, separatorStyle, timeZone
        VariableConverter::GetStringMember(env, options, "dateFormat", dateFormat);
        VariableConverter::GetStringMember(env, options, "timePrecision", timePrecision);
        VariableConverter::GetStringMember(env, options, "separatorStyle", separatorStyle);
        ani_object timeZoneValue;
        VariableConverter::GetObjectMember(env, options, "timeZone", timeZoneValue);
        timeZone = ParseTimezone(env, timeZoneValue);

        VariableConverter::GetBooleanMember(env, options, "displayTimeZone",  displayTimeZone);
    }

    // Convert string values to enum
    ISO8601DateTimeFormat::DateFormat dateFormatEnum = ISO8601DateTimeFormat::DateFormat::CALENDAR;
    if (dateFormat == "ordinal") {
        dateFormatEnum = ISO8601DateTimeFormat::DateFormat::ORDINAL;
    } else if (dateFormat == "week") {
        dateFormatEnum = ISO8601DateTimeFormat::DateFormat::WEEK;
    }

    ISO8601DateTimeFormat::TimePrecision timePrecisionEnum = ISO8601DateTimeFormat::TimePrecision::DATE_ONLY;
    if (timePrecision == "hours") {
        timePrecisionEnum = ISO8601DateTimeFormat::TimePrecision::HOURS;
    } else if (timePrecision == "minutes") {
        timePrecisionEnum = ISO8601DateTimeFormat::TimePrecision::MINUTES;
    } else if (timePrecision == "seconds") {
        timePrecisionEnum = ISO8601DateTimeFormat::TimePrecision::SECONDS;
    } else if (timePrecision == "milliSeconds") {
        timePrecisionEnum = ISO8601DateTimeFormat::TimePrecision::MILLISECONDS;
    }

    ISO8601DateTimeFormat::SeparatorStyle separatorStyleEnum = ISO8601DateTimeFormat::SeparatorStyle::EXTENDED;
    if (separatorStyle == "basic") {
        separatorStyleEnum = ISO8601DateTimeFormat::SeparatorStyle::BASIC;
    }

    std::unique_ptr<ISO8601DateTimeFormatAddon> obj = std::make_unique<ISO8601DateTimeFormatAddon>();
    if (obj == nullptr) {
        HILOG_ERROR_I18N("ISO8601DateTimeFormat::Create: Create ISO8601DateTimeFormatAddon failed.");
        return nullptr;
    }

    I18nErrorCode errCode = I18nErrorCode::SUCCESS;
    obj->iso8601DateTimeFormat_ = std::make_shared<ISO8601DateTimeFormat>(
        dateFormatEnum, timePrecisionEnum, separatorStyleEnum, timeZone, displayTimeZone, errCode);

    if (!obj->iso8601DateTimeFormat_ || errCode != I18nErrorCode::SUCCESS) {
        HILOG_ERROR_I18N("ISO8601DateTimeFormat::Create: Construct ISO8601DateTimeFormat failed.");
        return nullptr;
    }

    static const char* className = "@ohos.i18n.i18n.ISO8601DateTimeFormat";
    ani_object iso8601DateTimeFormatObject = VariableConverter::CreateAniObject(env, className, obj.get());
    obj.release();
    return iso8601DateTimeFormatObject;
}

ani_string ISO8601DateTimeFormatAddon::Format(ani_env *env, ani_object object, ani_object date)
{
    ISO8601DateTimeFormatAddon* obj = UnwrapAddon(env, object);
    if (obj == nullptr || obj->iso8601DateTimeFormat_ == nullptr) {
        HILOG_ERROR_I18N("ISO8601DateTimeFormat::Format: Get ISO8601DateTimeFormat object failed");
        return nullptr;
    }

    int64_t milliseconds = VariableConverter::GetDateValue(env, date, "valueOf");
    std::string result = obj->iso8601DateTimeFormat_->Format(milliseconds);
    return VariableConverter::StringToAniStr(env, result);
}

ani_status ISO8601DateTimeFormatAddon::BindContextISO8601DateTimeFormat(ani_env *env)
{
    ani_class clazz;
    if (ANI_OK != env->FindClass("@ohos.i18n.i18n.ISO8601DateTimeFormat", &clazz)) {
        HILOG_ERROR_I18N("BindContextISO8601DateTimeFormat: Find class failed");
        return ANI_ERROR;
    }

    std::array methods = {
        ani_native_function { "format", nullptr, reinterpret_cast<void *>(ISO8601DateTimeFormatAddon::Format) }
    };

    if (ANI_OK != env->Class_BindNativeMethods(clazz, methods.data(), methods.size())) {
        HILOG_ERROR_I18N("BindContextSimpleDateTimeFormat: Cannot bind native methods to ISO8601DateTimeFormat");
        return ANI_ERROR;
    };

    std::array staticMethods = {
        ani_native_function { "create", nullptr, reinterpret_cast<void *>(ISO8601DateTimeFormatAddon::Create) },
    };
    if (ANI_OK != env->Class_BindStaticNativeMethods(clazz, staticMethods.data(), staticMethods.size())) {
        HILOG_ERROR_I18N("BindContextSimpleDateTimeFormat: Cannot bind static native methods");
        return ANI_ERROR;
    }

    return ANI_OK;
}