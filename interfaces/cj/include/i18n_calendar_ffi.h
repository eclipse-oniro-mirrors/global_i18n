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
#ifndef INTERFACES_CJ_INCLUDE_I18N_CALENDAR_FFI_H_
#define INTERFACES_CJ_INCLUDE_I18N_CALENDAR_FFI_H_

#include <cstdint>
#include "i18n_struct.h"

#define FFI_EXPORT __attribute__((visibility("default")))

extern "C" {
    FFI_EXPORT void FfiOHOSCalendarSetTime(int64_t id, double milliseconds);
    FFI_EXPORT void FfiOHOSCalendarSetOfDay(int64_t id, int32_t hour, int32_t minute, int32_t second);
    FFI_EXPORT void FfiOHOSCalendarSetTimeOfDay(int64_t id, int32_t hour, int32_t minute, int32_t second,
        uint32_t flag);
    FFI_EXPORT void FfiOHOSCalendarSetDate(int64_t id, int32_t year, int32_t month, int32_t day);
    FFI_EXPORT void FfiOHOSCalendarSetTimeZone(int64_t id, const char* timeZone);
    FFI_EXPORT char* FfiOHOSCalendarGetTimeZone(int64_t id);
    FFI_EXPORT void FfiOHOSCalendarSetFirstDayOfWeek(int64_t id, int32_t firstDayOfWeek);
    FFI_EXPORT int32_t FfiOHOSCalendarGetFirstDayOfWeek(int64_t id);
    FFI_EXPORT void FfiOHOSCalendarSetMinimalDaysInFirstWeek(int64_t id, int32_t minimalDaysInFirstWeek);
    FFI_EXPORT int32_t FfiOHOSCalendarGetMinimalDaysInFirstWeek(int64_t id);
    FFI_EXPORT int32_t FfiOHOSCalendarGet(int64_t id, const char* field);
    FFI_EXPORT char* FfiOHOSCalendarGetDisplayName(int64_t id, const char* locale);
    FFI_EXPORT bool FfiOHOSCalendarIsWeekend(int64_t id, CDate date);
    FFI_EXPORT void FfiOHOSCalendarAdd(int64_t id, const char* field, int32_t amount, int32_t* errcode);
    FFI_EXPORT double FfiOHOSCalendarGetTimeInMillis(int64_t id);
    FFI_EXPORT int32_t FfiOHOSCalendarCompareDays(int64_t id, CDate date);
    FFI_EXPORT int64_t FfiOHOSGetCalendar(const char* locale, const char* type);
}

#endif  // INTERFACES_CJ_INCLUDE_I18N_CALENDAR_FFI_H_
