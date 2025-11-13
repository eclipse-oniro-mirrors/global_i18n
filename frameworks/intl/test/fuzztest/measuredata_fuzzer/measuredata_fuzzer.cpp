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
#include "measure_data.h"
#include "measuredata_fuzzer.h"

namespace OHOS {
    const size_t FIRST_PARAM_OFFSET_SIZE = 8;
    bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
    {
        using namespace Global::I18n;
        if (size < 1) {
            return false;
        }
        std::string input(reinterpret_cast<const char*>(data), size);
        std::vector<std::string> units;
        units.push_back(input);
        GetMask(input);
        GetDefaultPreferredUnit(input, input, units);
        GetFallbackPreferredUnit(input, input, units);
        GetRestPreferredUnit(input, input, units);
        GetPreferredUnit(input, input, units);
        ComputeSIPrefixValue(input);
        if (size >= FIRST_PARAM_OFFSET_SIZE) {
            double number = *(reinterpret_cast<const double*>(data));
            std::vector<double> factors;
            factors.push_back(number);
            factors.push_back(number);
            ComputeFactorValue(input, input, factors);
            ComputePowerValue(input, input, factors);
            ComputeValue(input, input, factors);
            ConvertByte(number, input);
            ConvertDate(number, input);
            Convert(number, input, input, input, input);
        }
        return false;
    }
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}

