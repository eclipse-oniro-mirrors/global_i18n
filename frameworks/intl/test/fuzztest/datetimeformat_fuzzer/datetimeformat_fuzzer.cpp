/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#include "date_time_format.h"
#include "datetimeformat_fuzzer.h"

namespace OHOS {
    void DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
    {
        using namespace Global::I18n;

        std::string input(reinterpret_cast<const char*>(data), size);

        std::vector<std::string> localeTags;
        localeTags.push_back(input);
        std::map<std::string, std::string> configs;
        configs[input] = input;
        DateTimeFormat dateTimeFormat(localeTags, configs);
        dateTimeFormat.Format(size);
        dateTimeFormat.FormatRange(size, size);
        dateTimeFormat.GetResolvedOptions(configs);
        dateTimeFormat.GetDateStyle();
        dateTimeFormat.GetTimeStyle();
        dateTimeFormat.GetHourCycle();
        dateTimeFormat.GetTimeZone();
        dateTimeFormat.GetTimeZoneName();
        dateTimeFormat.GetNumberingSystem();
        dateTimeFormat.GetHour12();
        dateTimeFormat.GetWeekday();
        dateTimeFormat.GetEra();
        dateTimeFormat.GetYear();
        dateTimeFormat.GetMonth();
        dateTimeFormat.GetDay();
        dateTimeFormat.GetHour();
        dateTimeFormat.GetMinute();
        dateTimeFormat.GetSecond();
        std::unique_ptr<DateTimeFormat> dtFormat =
                DateTimeFormat::CreateInstance(localeTags, configs);
        dtFormat->Format(size);
    }
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}

