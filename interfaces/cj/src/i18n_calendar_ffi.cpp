/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <ctime>
#include "i18n_calendar_ffi.h"
#include "i18n_calendar_impl.h"
#include "unicode/ucal.h"
#include "i18n_ffi.h"
#include "i18n_hilog.h"

namespace {
using namespace OHOS::Global::I18n;
std::unordered_map<std::string, UCalendarDateFields> g_fieldsMap {
    { "era", UCAL_ERA },
    { "year", UCAL_YEAR },
    { "month", UCAL_MONTH },
    { "week_of_year", UCAL_WEEK_OF_YEAR },
    { "week_of_month", UCAL_WEEK_OF_MONTH },
    { "date", UCAL_DATE },
    { "day_of_year", UCAL_DAY_OF_YEAR },
    { "day_of_week", UCAL_DAY_OF_WEEK },
    { "day_of_week_in_month", UCAL_DAY_OF_WEEK_IN_MONTH },
    { "ap_pm", UCAL_AM_PM },
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

std::unordered_set<std::string> g_fieldsInFunctionAdd {
    "year", "month", "date", "hour", "minute", "second", "millisecond",
    "week_of_year", "week_of_month", "day_of_year", "day_of_week",
    "day_of_week_in_month", "hour_of_day", "milliseconds_in_day",
};

std::unordered_map<std::string, CalendarType> g_typeMap {
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
    { "persion", CalendarType::PERSIAN },
};
}

namespace OHOS {
namespace Global {
namespace I18n {
using namespace OHOS::FFI;

extern "C" {
CalendarType GetCalendarType(const std::string& calendarType)
{
    CalendarType type = CalendarType::UNDEFINED;
    if (g_typeMap.find(calendarType) != g_typeMap.end()) {
        type = g_typeMap[calendarType];
    }
    return type;
}

int64_t FfiOHOSGetCalendar(const char* locale, const char* type)
{
    CalendarType calendarType = CalendarType::UNDEFINED;
    if (type != nullptr) {
        calendarType = GetCalendarType(std::string(type));
    }
    std::string localStr(locale);
    auto nativeCalendar = FFIData::Create<CCalendar>(localStr, calendarType);
    if (nativeCalendar == nullptr) {
        HILOG_ERROR_I18N("Create CCalendar fail");
        return -1;
    }
    return nativeCalendar->GetID();
}

void FfiOHOSCalendarSetTime(int64_t id, double time)
{
    auto instance = FFIData::GetData<CCalendar>(id);
    if (!instance) {
        HILOG_ERROR_I18N("The CCalendar instance is nullptr");
        return;
    }
    instance->SetTime(time);
}

void FfiOHOSCalendarSetDate(int64_t id, int32_t year, int32_t month, int32_t day)
{
    auto instance = FFIData::GetData<CCalendar>(id);
    if (!instance) {
        HILOG_ERROR_I18N("The CCalendar instance is nullptr");
        return;
    }
    instance->Set(year, month, day);
}

void FfiOHOSCalendarSetOfDay(int64_t id, int32_t hour, int32_t minute, int32_t second)
{
    auto instance = FFIData::GetData<CCalendar>(id);
    if (!instance) {
        HILOG_ERROR_I18N("The CCalendar instance is nullptr");
        return;
    }
    std::time_t t = std::time(nullptr);
    std::tm* now = std::localtime(&t);

    int32_t currentHour = 0;
    int32_t currentMinute = 0;
    int32_t currentSecond = 0;

    if (now != nullptr) {
        currentHour = now->tm_hour;
        currentMinute = now->tm_min;
        currentSecond = now->tm_sec;
    }
    
    if (hour == -1) {
        hour = currentHour;
    }
    if (minute == -1) {
        minute = currentMinute;
    }
    if (second == -1) {
        second = currentSecond;
    }
    
    instance->Set(UCalendarDateFields::UCAL_HOUR_OF_DAY, hour);
    instance->Set(UCalendarDateFields::UCAL_MINUTE, minute);
    instance->Set(UCalendarDateFields::UCAL_SECOND, second);
}

void FfiOHOSCalendarSetTimeZone(int64_t id, const char* timeZone)
{
    auto instance = FFIData::GetData<CCalendar>(id);
    if (!instance) {
        HILOG_ERROR_I18N("The CCalendar instance is nullptr");
        return;
    }
    instance->SetTimeZone(std::string(timeZone));
}

char* FfiOHOSCalendarGetTimeZone(int64_t id)
{
    auto instance = FFIData::GetData<CCalendar>(id);
    if (!instance) {
        HILOG_ERROR_I18N("The CCalendar instance is nullptr");
        return nullptr;
    }
    std::string res = instance->GetTimeZone();
    return MallocCString(res);
}

void FfiOHOSCalendarSetFirstDayOfWeek(int64_t id, int32_t firstDayOfWeek)
{
    auto instance = FFIData::GetData<CCalendar>(id);
    if (!instance) {
        HILOG_ERROR_I18N("The CCalendar instance is nullptr");
        return;
    }
    instance->SetFirstDayOfWeek(firstDayOfWeek);
}

int32_t FfiOHOSCalendarGetFirstDayOfWeek(int64_t id)
{
    auto instance = FFIData::GetData<CCalendar>(id);
    if (!instance) {
        HILOG_ERROR_I18N("The CCalendar instance is nullptr");
        return UCAL_SUNDAY;
    }
    return instance->GetFirstDayOfWeek();
}

void FfiOHOSCalendarSetMinimalDaysInFirstWeek(int64_t id, int32_t minimalDays)
{
    auto instance = FFIData::GetData<CCalendar>(id);
    if (!instance) {
        HILOG_ERROR_I18N("The CCalendar instance is nullptr");
        return;
    }
    instance->SetMinimalDaysInFirstWeek(minimalDays);
}

int32_t FfiOHOSCalendarGetMinimalDaysInFirstWeek(int64_t id)
{
    auto instance = FFIData::GetData<CCalendar>(id);
    if (!instance) {
        HILOG_ERROR_I18N("The CCalendar instance is nullptr");
        return 1;
    }
    return instance->GetMinimalDaysInFirstWeek();
}

int32_t FfiOHOSCalendarGet(int64_t id, char* field)
{
    auto instance = FFIData::GetData<CCalendar>(id);
    if (!instance) {
        HILOG_ERROR_I18N("The CCalendar instance is nullptr");
        return 0;
    }
    std::string fieldName(field);
    return instance->Get(g_fieldsMap[fieldName]);
}

char* FfiOHOSCalendarGetDisplayName(int64_t id, char* locale)
{
    std::string localeStr(locale);
    auto instance = FFIData::GetData<CCalendar>(id);
    if (!instance) {
        HILOG_ERROR_I18N("The CCalendar instance is nullptr");
        return nullptr;
    }
    std::string res = instance->GetDisplayName(localeStr);
    return MallocCString(res);
}

bool FfiOHOSCalendarIsWeekend(int64_t id, CDate date)
{
    auto instance = FFIData::GetData<CCalendar>(id);
    if (!instance) {
        HILOG_ERROR_I18N("The CCalendar instance is nullptr");
        return false;
    }
    // -1 indicates that the date is not provided
    if (date.isNull) {
        return instance->IsWeekend();
    } else {
        UErrorCode error = U_ZERO_ERROR;
        return instance->IsWeekend(date.icuUdate, error);
    }
}

void FfiOHOSCalendarAdd(int64_t id, char* field, int32_t amount, int32_t* errcode)
{
    auto instance = FFIData::GetData<CCalendar>(id);
    if (!instance) {
        HILOG_ERROR_I18N("The CCalendar instance is nullptr");
        return;
    }
    std::string fieldStr(field);
    if (g_fieldsInFunctionAdd.find(fieldStr) == g_fieldsInFunctionAdd.end()) {
        *errcode = I18N_NOT_VALID;
    }
    instance->Add(g_fieldsMap[fieldStr], amount);
}

double FfiOHOSCalendarGetTimeInMillis(int64_t id)
{
    auto instance = FFIData::GetData<CCalendar>(id);
    if (!instance) {
        HILOG_ERROR_I18N("The CCalendar instance is nullptr");
        return 0;
    }
    return instance->GetTimeInMillis();
}

int32_t FfiOHOSCalendarCompareDays(int64_t id, CDate date)
{
    auto instance = FFIData::GetData<CCalendar>(id);
    if (!instance) {
        HILOG_ERROR_I18N("The CCalendar instance is nullptr");
        return 0;
    }
    return instance->CompareDays(date.icuUdate);
}
}

}  // namespace I18n
}  // namespace Global
}  // namespace OHOS
