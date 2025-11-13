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
        HILOG_ERROR_I18N("Get Long 'nativeCalendar' failed");
        return nullptr;
    }
    return reinterpret_cast<I18nCalendarAddon*>(ptr);
}

ani_object I18nCalendarAddon::GetCalendar(ani_env *env, ani_string locale, ani_string type)
{
    std::unique_ptr<I18nCalendarAddon> obj = std::make_unique<I18nCalendarAddon>();
    ani_boolean isUndefined;
    if (ANI_OK != env->Reference_IsUndefined(type, &isUndefined)) {
        HILOG_ERROR_I18N("Reference IsUndefined failed");
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
    static const char* className = "L@ohos/i18n/i18n/Calendar;";
    ani_object calendarObject = VariableConverter::CreateAniObject(env, className, obj.get());
    obj.release();
    return calendarObject;
}

void I18nCalendarAddon::SetTimeByDate(ani_env *env, ani_object object, ani_object date)
{
    I18nCalendarAddon* obj = UnwrapAddon(env, object);
    if (obj == nullptr || obj->calendar_ == nullptr) {
        HILOG_ERROR_I18N("SetTime: Get calendar object failed");
        return;
    }
    obj->calendar_->SetTime(VariableConverter::GetDateValue(env, date, "valueOf"));
}

void I18nCalendarAddon::SetTime(ani_env *env, ani_object object, ani_double time)
{
    I18nCalendarAddon* obj = UnwrapAddon(env, object);
    if (obj == nullptr || obj->calendar_ == nullptr) {
        HILOG_ERROR_I18N("SetTime: Get calendar object failed");
        return;
    }
    obj->calendar_->SetTime(time);
}

void I18nCalendarAddon::Set(ani_env *env, ani_object object, ani_double year, ani_double month,
    ani_double date, ani_object hour, ani_object minute, ani_object second)
{
    I18nCalendarAddon* obj = UnwrapAddon(env, object);
    if (obj == nullptr || obj->calendar_ == nullptr) {
        HILOG_ERROR_I18N("Set: Get calendar object failed");
        return;
    }
    obj->calendar_->Set(year, month, date);

    ani_boolean isUndefined;
    if (ANI_OK != env->Reference_IsUndefined(hour, &isUndefined)) {
        HILOG_ERROR_I18N("Reference IsUndefined failed");
        return;
    }
    if (!isUndefined) {
        ani_double hourValue;
        if (ANI_OK != env->Object_CallMethodByName_Double(hour, "unboxed", ":D", &hourValue)) {
            HILOG_ERROR_I18N("Unboxed Double failed");
            return;
        }
        obj->calendar_->Set(UCalendarDateFields::UCAL_HOUR_OF_DAY, hourValue);
    }

    if (ANI_OK != env->Reference_IsUndefined(minute, &isUndefined)) {
        HILOG_ERROR_I18N("Reference IsUndefined failed");
        return;
    }
    if (!isUndefined) {
        ani_double minuteValue;
        if (ANI_OK != env->Object_CallMethodByName_Double(minute, "unboxed", ":D", &minuteValue)) {
            HILOG_ERROR_I18N("Unboxed Double failed");
            return;
        }
        obj->calendar_->Set(UCalendarDateFields::UCAL_MINUTE, minuteValue);
    }

    if (ANI_OK != env->Reference_IsUndefined(second, &isUndefined)) {
        HILOG_ERROR_I18N("Reference IsUndefined failed");
        return;
    }
    if (!isUndefined) {
        ani_double secondValue;
        if (ANI_OK != env->Object_CallMethodByName_Double(second, "unboxed", ":D", &secondValue)) {
            HILOG_ERROR_I18N("Unboxed Double failed");
            return;
        }
        obj->calendar_->Set(UCalendarDateFields::UCAL_SECOND, secondValue);
    }
}

void I18nCalendarAddon::SetTimeZone(ani_env *env, ani_object object, ani_string timezone)
{
    I18nCalendarAddon* obj = UnwrapAddon(env, object);
    if (obj == nullptr || obj->calendar_ == nullptr) {
        HILOG_ERROR_I18N("SetTimeZone: Get calendar object failed");
        return;
    }
    obj->calendar_->SetTimeZone(VariableConverter::AniStrToString(env, timezone));
}

ani_string I18nCalendarAddon::GetTimeZone(ani_env *env, ani_object object)
{
    I18nCalendarAddon* obj = UnwrapAddon(env, object);
    if (obj == nullptr || obj->calendar_ == nullptr) {
        HILOG_ERROR_I18N("GetTimeZone: Get calendar object failed");
        return nullptr;
    }

    std::string str = obj->calendar_->GetTimeZone();
    return VariableConverter::StringToAniStr(env, str);
}

ani_double I18nCalendarAddon::GetFirstDayOfWeek(ani_env *env, ani_object object)
{
    I18nCalendarAddon* obj = UnwrapAddon(env, object);
    if (obj == nullptr || obj->calendar_ == nullptr) {
        HILOG_ERROR_I18N("GetFirstDayOfWeek: Get calendar object failed");
        return -1;
    }
    return obj->calendar_->GetFirstDayOfWeek();
}

void I18nCalendarAddon::SetFirstDayOfWeek(ani_env *env, ani_object object, ani_double value)
{
    I18nCalendarAddon* obj = UnwrapAddon(env, object);
    if (obj == nullptr || obj->calendar_ == nullptr) {
        HILOG_ERROR_I18N("SetFirstDayOfWeek: Get calendar object failed");
        return;
    }
    obj->calendar_->SetFirstDayOfWeek(value);
}

ani_double I18nCalendarAddon::GetMinimalDaysInFirstWeek(ani_env *env, ani_object object)
{
    I18nCalendarAddon* obj = UnwrapAddon(env, object);
    if (obj == nullptr || obj->calendar_ == nullptr) {
        HILOG_ERROR_I18N("GetMinimalDaysInFirstWeek: Get calendar object failed");
        return -1;
    }
    return obj->calendar_->GetMinimalDaysInFirstWeek();
}

void I18nCalendarAddon::SetMinimalDaysInFirstWeek(ani_env *env, ani_object object, ani_double value)
{
    I18nCalendarAddon* obj = UnwrapAddon(env, object);
    if (obj == nullptr || obj->calendar_ == nullptr) {
        HILOG_ERROR_I18N("SetMinimalDaysInFirstWeek: Get calendar object failed");
        return;
    }
    obj->calendar_->SetMinimalDaysInFirstWeek(value);
}

ani_double I18nCalendarAddon::Get(ani_env *env, ani_object object, ani_string field)
{
    std::string str = VariableConverter::AniStrToString(env, field);
    if (g_fieldsMap.find(str) == g_fieldsMap.end()) {
        HILOG_ERROR_I18N("Invalid field");
        return -1;
    }

    I18nCalendarAddon* obj = UnwrapAddon(env, object);
    if (obj == nullptr || obj->calendar_ == nullptr) {
        HILOG_ERROR_I18N("Get: Get calendar object failed");
        return -1;
    }
    return obj->calendar_->Get(g_fieldsMap[str]);
}

ani_string I18nCalendarAddon::GetDisplayName(ani_env *env, ani_object object, ani_string locale)
{
    I18nCalendarAddon* obj = UnwrapAddon(env, object);
    if (obj == nullptr || obj->calendar_ == nullptr) {
        HILOG_ERROR_I18N("GetDisplayName: Get calendar object failed");
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
        HILOG_ERROR_I18N("IsWeekend: Get calendar object failed");
        return false;
    }

    bool isWeekEnd = false;
    ani_boolean isUndefined;
    if (ANI_OK != env->Reference_IsUndefined(date, &isUndefined)) {
        HILOG_ERROR_I18N("Reference IsUndefined failed");
        return false;
    }
    if (isUndefined) {
        isWeekEnd = obj->calendar_->IsWeekend();
    } else {
        UErrorCode error = U_ZERO_ERROR;
        isWeekEnd = obj->calendar_->IsWeekend(VariableConverter::GetDateValue(env, date, "valueOf"), error);
    }
    return isWeekEnd;
}

void I18nCalendarAddon::Add(ani_env *env, ani_object object, ani_string field, ani_double amount)
{
    std::string str = VariableConverter::AniStrToString(env, field);
    if (g_fieldsInFunctionAdd.find(str) == g_fieldsInFunctionAdd.end()) {
        HILOG_ERROR_I18N("Parameter range do not match");
        ErrorUtil::AniThrow(env, I18N_NOT_VALID, "field", "a valid field");
        return;
    }

    I18nCalendarAddon* obj = UnwrapAddon(env, object);
    if (obj == nullptr || obj->calendar_ == nullptr) {
        HILOG_ERROR_I18N("Add: Get calendar object failed");
        return;
    }
    obj->calendar_->Add(g_fieldsMap[str], amount);
}

ani_double I18nCalendarAddon::GetTimeInMillis(ani_env *env, ani_object object)
{
    bool flag = true;
    I18nCalendarAddon* obj = UnwrapAddon(env, object);
    if (obj == nullptr || obj->calendar_ == nullptr) {
        HILOG_ERROR_I18N("GetTimeInMillis: Get calendar object failed");
        flag = false;
    }

    UDate temp = 0;
    if (flag) {
        temp = obj->calendar_->GetTimeInMillis();
    }
    return temp;
}

ani_double I18nCalendarAddon::CompareDays(ani_env *env, ani_object object, ani_object date)
{
    I18nCalendarAddon* obj = UnwrapAddon(env, object);
    if (obj == nullptr || obj->calendar_ == nullptr) {
        HILOG_ERROR_I18N("CompareDays: Get calendar object failed");
        return 0;
    }
    return obj->calendar_->CompareDays(VariableConverter::GetDateValue(env, date, "valueOf"));
}

ani_status I18nCalendarAddon::BindContextCalendar(ani_env *env)
{
    static const char *className = "L@ohos/i18n/i18n/Calendar;";
    ani_class cls;
    if (ANI_OK != env->FindClass(className, &cls)) {
        HILOG_ERROR_I18N("Find class '%{public}s' failed", className);
        return ANI_ERROR;
    }

    std::array methods = {
        ani_native_function { "setTime", "Lescompat/Date;:V",
            reinterpret_cast<void *>(I18nCalendarAddon::SetTimeByDate) },
        ani_native_function { "setTime", "D:V", reinterpret_cast<void *>(I18nCalendarAddon::SetTime) },
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
        HILOG_ERROR_I18N("Cannot bind native methods to '%{public}s'", className);
        return ANI_ERROR;
    };

    return ANI_OK;
}
