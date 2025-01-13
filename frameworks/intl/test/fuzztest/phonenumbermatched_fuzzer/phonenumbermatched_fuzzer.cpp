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
#include "matched_number_info.h"
#include "phone_number_matched.h"
#include "phone_number_rule.h"
#include "phonenumbermatched_fuzzer.h"

namespace OHOS {
    bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
    {
        using namespace Global::I18n;
        if (size < 1) {
            return false;
        }
        std::string input(reinterpret_cast<const char*>(data), size);

        PhoneNumberMatched* phoneNumberMatched = new PhoneNumberMatched(input);
        icu::UnicodeString message(input.c_str());
        phoneNumberMatched->GetMatchedPhoneNumber(message);

        PhoneNumberRule phoneNumberRule(input);
        phoneNumberRule.Init();
        phoneNumberRule.GetBorderRules();
        phoneNumberRule.GetCodesRules();
        phoneNumberRule.GetPositiveRules();
        phoneNumberRule.GetNegativeRules();
        phoneNumberRule.GetFindRules();
        MatchedNumberInfo* matchedNumberInfo = new MatchedNumberInfo();
        icu::UnicodeString content(input.c_str());
        MatchedNumberInfo* matchedNumberInfo2 = new MatchedNumberInfo(size, size, content);

        matchedNumberInfo2->SetBegin(size);
        matchedNumberInfo2->GetBegin();
        matchedNumberInfo2->SetEnd(size);
        matchedNumberInfo2->GetEnd();
        matchedNumberInfo2->SetContent(content);
        matchedNumberInfo2->GetContent();
        bool flag = *matchedNumberInfo < *matchedNumberInfo2;
        delete phoneNumberMatched;
        delete matchedNumberInfo2;
        delete matchedNumberInfo;
        return flag;
    }
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}

