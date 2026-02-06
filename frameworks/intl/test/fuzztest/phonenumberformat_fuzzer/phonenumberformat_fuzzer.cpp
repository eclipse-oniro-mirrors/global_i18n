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
#include "phone_number_format.h"
#include "phonenumberformat_fuzzer.h"

namespace OHOS {
    const static size_t INPUT_DATA_LENGTH = 2;

    bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
    {
        using namespace Global::I18n;
        if (size < INPUT_DATA_LENGTH) {
            return false;
        }
        std::string input(reinterpret_cast<const char*>(data), INPUT_DATA_LENGTH);
        std::map<std::string, std::string> options;
        options[input] = input;
        PhoneNumberFormat formatter(input, options);
        formatter.isValidPhoneNumber(input);
        formatter.format(input);
        formatter.getLocationName(input, input);
        formatter.getPhoneLocationName(input, input, input);
        formatter.getBlockedRegionName(input, input);
        formatter.getCityName(input, input, input);
        PhoneNumberFormat::CreateInstance(input, options);
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

