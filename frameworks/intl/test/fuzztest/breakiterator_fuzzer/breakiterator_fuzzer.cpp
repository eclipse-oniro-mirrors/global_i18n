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
#include "breakiterator_fuzzer.h"
#include "i18n_break_iterator.h"

namespace OHOS {
    const int VALUE_NUMBER = 3;
    bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
    {
        using namespace Global::I18n;

        if (size < sizeof(int32_t) * VALUE_NUMBER) {
            return false;
        }

        uint32_t offset = 0;

        std::string text(reinterpret_cast<const char*>(data + offset), size - sizeof(int32_t) * 3);
        offset += size - sizeof(int32_t) * VALUE_NUMBER;

        int32_t number = *(reinterpret_cast<const int32_t*>(data + offset));
        offset += sizeof(int32_t);

        int32_t next = *(reinterpret_cast<const int32_t*>(data + offset));
        offset += sizeof(int32_t);

        int32_t following = *(reinterpret_cast<const int32_t*>(data + offset));

        I18nBreakIterator iterator(text);
        iterator.SetText(text.c_str());

        std::string str;
        iterator.GetText(str);

        iterator.IsBoundary(number);
        iterator.Current();
        iterator.First();
        iterator.Last();

        iterator.Next(next);
        iterator.Next();
        iterator.Previous();

        iterator.Following(following);

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

