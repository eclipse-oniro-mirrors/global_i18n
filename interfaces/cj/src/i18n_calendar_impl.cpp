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

#include "i18n_calendar_impl.h"
#include "i18n_calendar.h"
#include "i18n_hilog.h"

namespace OHOS {
namespace Global {
namespace I18n {
CCalendar::CCalendar(std::string localeTag, CalendarType type)
{
    calendar_ = std::make_unique<I18nCalendar>(localeTag, type);
    if (calendar_ == nullptr) {
        HILOG_ERROR_I18N("Create I18nCalendar fail");
    }
}

void CCalendar::SetTime(double time)
{
    calendar_->SetTime(time);
}

void CCalendar::Set(UCalendarDateFields field, int32_t value)
{
    calendar_->Set(field, value);
}

void CCalendar::Set(int32_t year, int32_t month, int32_t date)
{
    calendar_->Set(year, month, date);
}

std::string CCalendar::GetTimeZone(void)
{
    return calendar_->GetTimeZone();
}

void CCalendar::SetTimeZone(std::string timezoneId)
{
    calendar_->SetTimeZone(timezoneId);
}

int32_t CCalendar::Get(UCalendarDateFields field) const
{
    return calendar_->Get(field);
}

void CCalendar::Add(UCalendarDateFields field, int32_t amount)
{
    return calendar_->Add(field, amount);
}

void CCalendar::SetMinimalDaysInFirstWeek(int32_t value)
{
    calendar_->SetMinimalDaysInFirstWeek(value);
}

void CCalendar::SetFirstDayOfWeek(int32_t value)
{
    calendar_->SetFirstDayOfWeek(value);
}

UDate CCalendar::GetTimeInMillis(void)
{
    return calendar_->GetTimeInMillis();
}

int32_t CCalendar::CompareDays(UDate date)
{
    return calendar_->CompareDays(date);
}

bool CCalendar::IsWeekend(void)
{
    return calendar_->IsWeekend();
}

bool CCalendar::IsWeekend(int64_t date, UErrorCode &status)
{
    return calendar_->IsWeekend(date, status);
}

std::string CCalendar::GetDisplayName(std::string &displayLocaleTag)
{
    return calendar_->GetDisplayName(displayLocaleTag);
}

int32_t CCalendar::GetMinimalDaysInFirstWeek(void)
{
    return calendar_->GetMinimalDaysInFirstWeek();
}

int32_t CCalendar::GetFirstDayOfWeek(void)
{
    return calendar_->GetFirstDayOfWeek();
}

}  // namespace I18n
}  // namespace Global
}  // namespace OHOS
