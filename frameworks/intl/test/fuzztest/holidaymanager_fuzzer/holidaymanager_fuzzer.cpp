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
#include "holiday_manager.h"
#include "holidaymanager_fuzzer.h"

namespace OHOS {
    bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
    {
        using namespace Global::I18n;
        if (size < 1) {
            return false;
        }
        std::string input(reinterpret_cast<const char*>(data), size);
        HolidayManager* holidayManager = new HolidayManager(input.c_str());

        std::map<std::string, std::vector<HolidayInfoItem>> holidayDataMap;
        std::vector<HolidayInfoItem> infoList;
        std::vector<HolidayLocalName> localNameList1;
        localNameList1.push_back({input, input});
        HolidayInfoItem item1 = {input, size, size, size, localNameList1};
        infoList.push_back(item1);
        holidayDataMap.insert({input, infoList});
        holidayManager->SetHolidayData(holidayDataMap);

        holidayManager->IsHoliday();
        holidayManager->IsHoliday(size, size, size);
        holidayManager->GetHolidayInfoItemArray();
        holidayManager->GetHolidayInfoItemArray(size);
        delete holidayManager;
        return true;
    }
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}

