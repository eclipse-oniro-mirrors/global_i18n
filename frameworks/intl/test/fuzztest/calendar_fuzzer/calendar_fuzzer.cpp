/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except", "in compliance with the License.
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

#include <cstddef>
#include <cstdint>
#include <string>
#include "i18n_calendar.h"
#include "calendar_fuzzer.h"
#include "unicode/ucal.h"
#include "unicode/utypes.h"

namespace OHOS {
    const size_t FIRST_PARAM_OFFSET_SIZE = 8;
    const size_t SECOND_PARAM_OFFSET_SIZE = 16;

    void DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
    {
        using namespace Global::I18n;

        uint32_t offset = 0;
        std::string input(reinterpret_cast<const char*>(data), size);
        I18nCalendar calendar(input, CalendarType::GREGORY);
        if (size >= FIRST_PARAM_OFFSET_SIZE) {
            double number = *(reinterpret_cast<const double*>(data + offset));
            calendar.SetTime(number);
        }

        calendar.Set(size, size, size);
        calendar.Set(UCalendarDateFields::UCAL_YEAR, size);
        calendar.GetTimeZone();
        calendar.SetTimeZone(input);
        calendar.GetTimeInMillis();
        calendar.GetMinimalDaysInFirstWeek();
        calendar.GetFirstDayOfWeek();
        UErrorCode status = U_ZERO_ERROR;
        calendar.IsWeekend(size, status);
        calendar.IsWeekend();
        calendar.GetDisplayName(input);
        if (size < SECOND_PARAM_OFFSET_SIZE) {
            return;
        }
        offset += sizeof(double);
        double compareDay = *(reinterpret_cast<const double*>(data + offset));
        calendar.CompareDays(compareDay);
    }
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}

