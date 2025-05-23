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
#include "i18n_timezone.h"
#include "timezone_fuzzer.h"

namespace OHOS {
    const size_t FIRST_PARAM_OFFSET_SIZE = 8;

    bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
    {
        using namespace Global::I18n;
        if (data == nullptr || size < 1) {
            return false;
        }
        uint32_t offset = 0;
        std::string text(reinterpret_cast<const char*>(data), size);
        bool isDST = (data[0] % 2 == 1);
        I18nTimeZone timezone(text, isDST);
        if (size >= FIRST_PARAM_OFFSET_SIZE) {
            double date = *(reinterpret_cast<const double*>(data + offset));
            timezone.GetOffset(date);
        }

        timezone.GetRawOffset();
        timezone.GetID();
        timezone.GetDisplayName();
        timezone.GetDisplayName(isDST);
        timezone.GetDisplayName(text);
        timezone.GetDisplayName(text, isDST);
        I18nTimeZone::CreateInstance(text, isDST);
        I18nErrorCode errorCode = I18nErrorCode::SUCCESS;
        I18nTimeZone::GetAvailableIDs(errorCode);
        I18nTimeZone::GetAvailableZoneCityIDs();
        I18nTimeZone::GetCityDisplayName(text, text);
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

