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

#ifndef INTERFACES_CJ_INCLUDE_I18N_CALENDAR_IMPL_H_
#define INTERFACES_CJ_INCLUDE_I18N_CALENDAR_IMPL_H_

#include <string>
#include <vector>
#include "i18n_struct.h"
#include "i18n_calendar.h"
#include "ffi_remote_data.h"

namespace OHOS {
namespace Global {
namespace I18n {
class CCalendar : public OHOS::FFI::FFIData {
    DECL_TYPE(CCalendar, FFIData)
public:
    explicit CCalendar(std::string localeTag, CalendarType type);
    ~CCalendar() = default;
    void SetTime(double time);
    void Set(int32_t year, int32_t month, int32_t date);
    void Set(UCalendarDateFields field, int32_t value);
    std::string GetTimeZone();
    void SetTimeZone(std::string timezoneId);
    int32_t Get(UCalendarDateFields field) const;
    void Add(UCalendarDateFields field, int32_t amount);
    void SetMinimalDaysInFirstWeek(int32_t value);
    void SetFirstDayOfWeek(int32_t value);
    UDate GetTimeInMillis(void);
    int32_t GetMinimalDaysInFirstWeek(void);
    int32_t GetFirstDayOfWeek(void);
    bool IsWeekend(int64_t date, UErrorCode &status);
    bool IsWeekend(void);
    std::string GetDisplayName(std::string &displayLocaleTag);
    int32_t CompareDays(UDate date);

private:
    std::unique_ptr<I18nCalendar> calendar_;
};

}  // namespace I18n
}  // namespace Global
}  // namespace OHOS

#endif  // INTERFACES_CJ_INCLUDE_I18N_CALENDAR_IMPL_H_
