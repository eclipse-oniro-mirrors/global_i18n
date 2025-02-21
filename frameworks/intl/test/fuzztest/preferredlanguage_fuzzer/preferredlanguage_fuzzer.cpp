/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#include <vector>
#include "preferred_language.h"

namespace OHOS {
    const size_t FIRST_PARAM_OFFSET_SIZE = 4;
    const size_t SECOND_PARAM_OFFSET_SIZE = 8;

    void DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
    {
        using namespace Global::I18n;
        std::string input(reinterpret_cast<const char*>(data), size);
        if (size < FIRST_PARAM_OFFSET_SIZE) {
            return;
        }
        uint32_t offset = 0;
        int32_t index = *(reinterpret_cast<const int32_t*>(data + offset));
        PreferredLanguage::AddPreferredLanguage(input, index);
        if (size < SECOND_PARAM_OFFSET_SIZE) {
            return;
        }
        offset += sizeof(int32_t);
        int32_t index2 = *(reinterpret_cast<const int32_t*>(data + offset));
        PreferredLanguage::RemovePreferredLanguage(index2);
    }
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}

