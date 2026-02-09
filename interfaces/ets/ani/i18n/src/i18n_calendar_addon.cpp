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

#include "i18n_calendar_addon.h"

#include <unordered_map>
#include <unordered_set>
#include "error_util.h"
#include "i18n_hilog.h"
#include "variable_converter.h"

using namespace OHOS;
using namespace Global;
using namespace I18n;

static std::unordered_map<std::string, UCalendarDateFields> g_fieldsMap {
    { "era", UCAL_ERA },
    { "year", UCAL_YEAR },
    { "month", UCAL_MONTH },
    { "week_of_year", UCAL_WEEK_OF_YEAR },
    { "week_of_month", UCAL_WEEK_OF_MONTH },
    { "date", UCAL_DATE },
    { "day_of_year", UCAL_DAY_OF_YEAR },
    { "day_of_week", UCAL_DAY_OF_WEEK },
    { "day_of_week_in_month", UCAL_DAY_OF_WEEK_IN_MONTH },
    { "am_pm", UCAL_AM_PM },
    { "hour", UCAL_HOUR },
    { "hour_of_day", UCAL_HOUR_OF_DAY },
    { "minute", UCAL_MINUTE },
    { "second", UCAL_SECOND },
    { "millisecond", UCAL_MILLISECOND },
    { "zone_offset", UCAL_ZONE_OFFSET },
    { "dst_offset", UCAL_DST_OFFSET },
    { "year_woy", UCAL_YEAR_WOY },
    { "dow_local", UCAL_DOW_LOCAL },
    { "extended_year", UCAL_EXTENDED_YEAR },
    { "julian_day", UCAL_JULIAN_DAY },
    { "milliseconds_in_day", UCAL_MILLISECONDS_IN_DAY },
    { "is_leap_month", UCAL_IS_LEAP_MONTH },
};

static std::unordered_set<std::string> g_fieldsInFunctionAdd {
    "year", "month", "date", "hour", "minute", "second", "millisecond",
    "week_of_year", "week_of_month", "day_of_year", "day_of_week",
    "day_of_week_in_month", "hour_of_day", "milliseconds_in_day",
};

static std::unordered_map<std::string, CalendarType> g_typeMap {
    { "buddhist", CalendarType::BUDDHIST },
    { "chinese", CalendarType::CHINESE },
    { "coptic", CalendarType::COPTIC },
    { "ethiopic", CalendarType::ETHIOPIC },
    { "hebrew", CalendarType::HEBREW },
    { "gregory", CalendarType::GREGORY },
    { "indian", CalendarType::INDIAN },
    { "islamic_civil", CalendarType::ISLAMIC_CIVIL },
    { "islamic_tbla", CalendarType::ISLAMIC_TBLA },
    { "islamic_umalqura", CalendarType::ISLAMIC_UMALQURA },
    { "japanese", CalendarType::JAPANESE },
    { "persian", CalendarType::PERSIAN },
};

I18nCalendarAddon* I18nCalendarAddon::UnwrapAddon(ani_env *env, ani_object object)
{
    ani_long ptr;
    if (ANI_OK != env->Object_GetFieldByName_Long(object, "nativeCalendar", &ptr)) {
        HILOG_ERROR_I18N("I18nCalendarAddon::Set: Get filed nativeCalendar to long failed");
        return nullptr;
    }
    return reinterpret_cast<I18nCalendarAddon*>(ptr);
}

ani_object I18nCalendarAddon::GetCalendar(ani_env *env, ani_string locale, ani_string type)
{
    std::unique_ptr<I18nCalendarAddon> obj = std::make_unique<I18nCalendarAddon>();
    ani_boolean isUndefined;
    if (ANI_OK != env->Reference_IsUndefined(type, &isUndefined)) {
        HILOG_ERROR_I18N("I18nCalendarAddon::GetCalendar: Check type reference is undefined failed");
        return nullptr;
    }
    CalendarType calendartype = CalendarType::UNDEFINED;
    if (!isUndefined) {
        std::string typeStr = VariableConverter::AniStrToString(env, type);
        if (g_typeMap.find(typeStr) != g_typeMap.end()) {
            calendartype = g_typeMap[typeStr];
        }
    }

    obj->calendar_ = std::make_unique<I18nCalendar>(VariableConverter::AniStrToString(env, locale), calendartype);
    static const char* className = "@ohos.i18n.i18n.Calendar";
    ani_object calendarObject = VariableConverter::CreateAniObject(env, className, obj.get());
    obj.release();
    return calendarObject;
}

void I18nCalendarAddon::SetTimeByDate(ani_env *env, ani_object object, ani_object date)
{
    I18nCalendarAddon* obj = UnwrapAddon(env, object);
    if (obj == nullptr || obj->calendar_ == nullptr) {
        HILOG_ERROR_I18N("I18nCalendarAddon::SetTimeByDate: Get calendar object failed");
        return;
    }
    ani_long millisecond = VariableConverter::GetDateValue(env, date, "valueOf");
    obj->calendar_->SetTime(static_cast<double>(millisecond));
}

void I18nCalendarAddon::SetTime(ani_env *env, ani_object object, ani_double time)
{
    I18nCalendarAddon* obj = UnwrapAddon(env, object);
    if (obj == nullptr || obj->calendar_ == nullptr) {
        HILOG_ERROR_I18N("I18nCalendarAddon::SetTime: Get calendar object failed");
        return;
    }
    obj->calendar_->SetTime(time);
}

void I18nCalendarAddon::Set(ani_env *env, ani_object object, ani_int year, ani_int month,
    ani_int date, ani_object hour, ani_object minute, ani_object second)
{
    I18nCalendarAddon* obj = UnwrapAddon(env, object);
    if (obj == nullptr || obj->calendar_ == nullptr) {
        HILOG_ERROR_I18N("I18nCalendarAddon::Set: Get calendar object failed");
        return;
    }
    obj->calendar_->Set(year, month, date);

    ani_boolean isUndefined;
    if (ANI_OK != env->Reference_IsUndefined(hour, &isUndefined)) {
        HILOG_ERROR_I18N("I18nCalendarAddon::Set: Check hour reference is undefined failed");
        return;
    }
    if (!isUndefined) {
        ani_int hourValue;
        if (ANI_OK != env->Object_CallMethodByName_Int(hour, "toInt", ":i", &hourValue)) {
            HILOG_ERROR_I18N("I18nCalendarAddon::Set: Unbox hour to int failed");
            HILOG_ERROR_I18N("Unbox Int failed");
            return;
        }
        obj->calendar_->Set(UCalendarDateFields::UCAL_HOUR_OF_DAY, hourValue);
    }

    if (ANI_OK != env->Reference_IsUndefined(minute, &isUndefined)) {
        HILOG_ERROR_I18N("I18nCalendarAddon::Set: Check minute reference is undefined failed");
        return;
    }
    if (!isUndefined) {
        ani_int minuteValue;
        if (ANI_OK != env->Object_CallMethodByName_Int(minute, "toInt", ":i", &minuteValue)) {
            HILOG_ERROR_I18N("I18nCalendarAddon::Set: Unbox minute to int failed");
            return;
        }
        obj->calendar_->Set(UCalendarDateFields::UCAL_MINUTE, minuteValue);
    }

    if (ANI_OK != env->Reference_IsUndefined(second, &isUndefined)) {
        HILOG_ERROR_I18N("I18nCalendarAddon::Set: Check second reference is undefined failed");
        return;
    }
    if (!isUndefined) {
        ani_int secondValue;
        if (ANI_OK != env->Object_CallMethodByName_Int(second, "toInt", ":i", &secondValue)) {
            HILOG_ERROR_I18N("I18nCalendarAddon::Set: Unbox second to int failed");
            return;
        }
        obj->calendar_->Set(UCalendarDateFields::UCAL_SECOND, secondValue);
    }
}

void I18nCalendarAddon::SetTimeZone(ani_env *env, ani_object object, ani_string timezone)
{
    I18nCalendarAddon* obj = UnwrapAddon(env, object);
    if (obj == nullptr || obj->calendar_ == nullptr) {
        HILOG_ERROR_I18N("I18nCalendarAddon::SetTimeZone: Get calendar object failed");
        return;
    }
    obj->calendar_->SetTimeZone(VariableConverter::AniStrToString(env, timezone));
}

ani_string I18nCalendarAddon::GetTimeZone(ani_env *env, ani_object object)
{
    I18nCalendarAddon* obj = UnwrapAddon(env, object);
    if (obj == nullptr || obj->calendar_ == nullptr) {
        HILOG_ERROR_I18N("I18nCalendarAddon::GetTimeZone: Get calendar object failed");
        return nullptr;
    }

    std::string str = obj->calendar_->GetTimeZone();
    return VariableConverter::StringToAniStr(env, str);
}

ani_int I18nCalendarAddon::GetFirstDayOfWeek(ani_env *env, ani_object object)
{
    I18nCalendarAddon* obj = UnwrapAddon(env, object);
    if (obj == nullptr || obj->calendar_ == nullptr) {
        HILOG_ERROR_I18N("I18nCalendarAddon::GetFirstDayOfWeek: Get calendar object failed");
        return -1;
    }
    return obj->calendar_->GetFirstDayOfWeek();
}

void I18nCalendarAddon::SetFirstDayOfWeek(ani_env *env, ani_object object, ani_int value)
{
    I18nCalendarAddon* obj = UnwrapAddon(env, object);
    if (obj == nullptr || obj->calendar_ == nullptr) {
        HILOG_ERROR_I18N("I18nCalendarAddon::SetFirstDayOfWeek: Get calendar object failed");
        return;
    }
    obj->calendar_->SetFirstDayOfWeek(value);
}

ani_int I18nCalendarAddon::GetMinimalDaysInFirstWeek(ani_env *env, ani_object object)
{
    I18nCalendarAddon* obj = UnwrapAddon(env, object);
    if (obj == nullptr || obj->calendar_ == nullptr) {
        HILOG_ERROR_I18N("I18nCalendarAddon::GetMinimalDaysInFirstWeek: Get calendar object failed");
        return -1;
    }
    return obj->calendar_->GetMinimalDaysInFirstWeek();
}

void I18nCalendarAddon::SetMinimalDaysInFirstWeek(ani_env *env, ani_object object, ani_int value)
{
    I18nCalendarAddon* obj = UnwrapAddon(env, object);
    if (obj == nullptr || obj->calendar_ == nullptr) {
        HILOG_ERROR_I18N("I18nCalendarAddon::SetMinimalDaysInFirstWeek: Get calendar object failed");
        return;
    }
    obj->calendar_->SetMinimalDaysInFirstWeek(value);
}

ani_int I18nCalendarAddon::Get(ani_env *env, ani_object object, ani_string field)
{
    std::string str = VariableConverter::AniStrToString(env, field);
    if (g_fieldsMap.find(str) == g_fieldsMap.end()) {
        HILOG_ERROR_I18N("I18nCalendarAddon::Get: Invalid field %{public}s", str.c_str());
        return -1;
    }

    I18nCalendarAddon* obj = UnwrapAddon(env, object);
    if (obj == nullptr || obj->calendar_ == nullptr) {
        HILOG_ERROR_I18N("I18nCalendarAddon::Get: Get calendar object failed");
        return -1;
    }
    return obj->calendar_->Get(g_fieldsMap[str]);
}

ani_string I18nCalendarAddon::GetDisplayName(ani_env *env, ani_object object, ani_string locale)
{
    I18nCalendarAddon* obj = UnwrapAddon(env, object);
    if (obj == nullptr || obj->calendar_ == nullptr) {
        HILOG_ERROR_I18N("I18nCalendarAddon::GetDisplayName: Get calendar object failed");
        return nullptr;
    }

    std::string localeTag = VariableConverter::AniStrToString(env, locale);
    std::string name = obj->calendar_->GetDisplayName(localeTag);
    return VariableConverter::StringToAniStr(env, name);
}

ani_boolean I18nCalendarAddon::IsWeekend(ani_env *env, ani_object object, ani_object date)
{
    I18nCalendarAddon* obj = UnwrapAddon(env, object);
    if (obj == nullptr || obj->calendar_ == nullptr) {
        HILOG_ERROR_I18N("I18nCalendarAddon::IsWeekend: Get calendar object failed");
        return false;
    }

    ani_boolean isUndefined;
    if (ANI_OK != env->Reference_IsUndefined(date, &isUndefined)) {
        HILOG_ERROR_I18N("I18nCalendarAddon::IsWeekend: Check date reference is undefined failed");
        return false;
    }
    if (isUndefined) {
        return obj->calendar_->IsWeekend();
    }
    UErrorCode error = U_ZERO_ERROR;
    ani_long millisecond = VariableConverter::GetDateValue(env, date, "valueOf");
    bool isWeekEnd = obj->calendar_->IsWeekend(static_cast<double>(millisecond), error);
    if (U_FAILURE(error)) {
        HILOG_ERROR_I18N("Call icuCalendar.isWeekend() failed");
        return false;
    }
    return isWeekEnd;
}

void I18nCalendarAddon::Add(ani_env *env, ani_object object, ani_string field, ani_int amount)
{
    std::string str = VariableConverter::AniStrToString(env, field);
    if (g_fieldsInFunctionAdd.find(str) == g_fieldsInFunctionAdd.end()) {
        HILOG_ERROR_I18N("I18nCalendarAddon::Add: Parameter range do not match");
        ErrorUtil::AniThrow(env, I18N_NOT_VALID, "field", "a valid field");
        return;
    }

    I18nCalendarAddon* obj = UnwrapAddon(env, object);
    if (obj == nullptr || obj->calendar_ == nullptr) {
        HILOG_ERROR_I18N("I18nCalendarAddon::Add: Get calendar object failed");
        return;
    }
    obj->calendar_->Add(g_fieldsMap[str], amount);
}

ani_long I18nCalendarAddon::GetTimeInMillis(ani_env *env, ani_object object)
{
    bool flag = true;
    I18nCalendarAddon* obj = UnwrapAddon(env, object);
    if (obj == nullptr || obj->calendar_ == nullptr) {
        HILOG_ERROR_I18N("I18nCalendarAddon::GetTimeInMillis: Get calendar object failed");
        flag = false;
    }

    UDate temp = 0;
    if (flag) {
        temp = obj->calendar_->GetTimeInMillis();
    }
    return temp;
}

ani_int I18nCalendarAddon::CompareDays(ani_env *env, ani_object object, ani_object date)
{
    I18nCalendarAddon* obj = UnwrapAddon(env, object);
    if (obj == nullptr || obj->calendar_ == nullptr) {
        HILOG_ERROR_I18N("I18nCalendarAddon::CompareDays: Get calendar object failed");
        return 0;
    }
    ani_long millisecond = VariableConverter::GetDateValue(env, date, "valueOf");
    return obj->calendar_->CompareDays(static_cast<double>(millisecond));
}

ani_status I18nCalendarAddon::BindContextCalendar(ani_env *env)
{
    static const char *className = "@ohos.i18n.i18n.Calendar";
    ani_class cls;
    if (ANI_OK != env->FindClass(className, &cls)) {
        HILOG_ERROR_I18N("BindContextCalendar: Find class '%{public}s' failed", className);
        return ANI_ERROR;
    }

    std::array methods = {
        ani_native_function { "setTime", "C{std.core.Date}:",
            reinterpret_cast<void *>(I18nCalendarAddon::SetTimeByDate) },
        ani_native_function { "setTime", "d:", reinterpret_cast<void *>(I18nCalendarAddon::SetTime) },
        ani_native_function { "set", nullptr, reinterpret_cast<void *>(I18nCalendarAddon::Set) },
        ani_native_function { "setTimeZone", nullptr, reinterpret_cast<void *>(I18nCalendarAddon::SetTimeZone) },
        ani_native_function { "getTimeZone", nullptr, reinterpret_cast<void *>(I18nCalendarAddon::GetTimeZone) },
        ani_native_function { "getFirstDayOfWeek", nullptr,
            reinterpret_cast<void *>(I18nCalendarAddon::GetFirstDayOfWeek) },
        ani_native_function { "setFirstDayOfWeek", nullptr,
            reinterpret_cast<void *>(I18nCalendarAddon::SetFirstDayOfWeek) },
        ani_native_function { "getMinimalDaysInFirstWeek", nullptr,
            reinterpret_cast<void *>(I18nCalendarAddon::GetMinimalDaysInFirstWeek) },
        ani_native_function { "setMinimalDaysInFirstWeek", nullptr,
            reinterpret_cast<void *>(I18nCalendarAddon::SetMinimalDaysInFirstWeek) },
        ani_native_function { "get", nullptr, reinterpret_cast<void *>(I18nCalendarAddon::Get) },
        ani_native_function { "getDisplayName", nullptr, reinterpret_cast<void *>(I18nCalendarAddon::GetDisplayName) },
        ani_native_function { "isWeekend", nullptr, reinterpret_cast<void *>(I18nCalendarAddon::IsWeekend) },
        ani_native_function { "add", nullptr, reinterpret_cast<void *>(I18nCalendarAddon::Add) },
        ani_native_function { "getTimeInMillis", nullptr,
            reinterpret_cast<void *>(I18nCalendarAddon::GetTimeInMillis) },
        ani_native_function { "compareDays", nullptr, reinterpret_cast<void *>(I18nCalendarAddon::CompareDays) },
    };

    if (ANI_OK != env->Class_BindNativeMethods(cls, methods.data(), methods.size())) {
        HILOG_ERROR_I18N("BindContextCalendar: Cannot bind native methods to '%{public}s'", className);
        return ANI_ERROR;
    };

    return ANI_OK;
}
