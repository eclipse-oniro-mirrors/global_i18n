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
#include "relative_time_format.h"
#include "relativetimeformat_fuzzer.h"

namespace OHOS {
    const size_t FIRST_PARAM_OFFSET_SIZE = 8;
    const size_t SECOND_PARAM_OFFSET_SIZE = 16;

    bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
    {
        using namespace Global::I18n;
        if (size < 1) {
            return false;
        }
        uint32_t offset = 0;
        std::string input(reinterpret_cast<const char*>(data), size);
        std::vector<std::string> localeTags(1, input);
        std::map<std::string, std::string> options;
        options[input] = input;
        RelativeTimeFormat formatter(localeTags, options);
        if (size >= FIRST_PARAM_OFFSET_SIZE) {
            double number = *(reinterpret_cast<const double*>(data + offset));
            formatter.Format(number, input);
        }

        if (size >= SECOND_PARAM_OFFSET_SIZE) {
            std::vector<std::vector<std::string>> timeVector;
            offset += sizeof(double);
            double number2 = *(reinterpret_cast<const double*>(data + offset));
            formatter.FormatToParts(number2, input, timeVector);
        }
    
        std::map<std::string, std::string> res;
        formatter.GetResolvedOptions(res);
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

