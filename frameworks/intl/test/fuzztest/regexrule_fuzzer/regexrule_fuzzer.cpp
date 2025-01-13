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
#include "locale_config.h"
#include "phone_number_matched.h"
#include "regex_rule.h"
#include "regexrule_fuzzer.h"

namespace OHOS {
    bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
    {
        using namespace Global::I18n;
        using namespace i18n::phonenumbers;
        if (size < 1) {
            return false;
        }
        std::string input(reinterpret_cast<const char*>(data), size);
        icu::UnicodeString regex(input.c_str());
        RegexRule* regexRule = new RegexRule(regex, input, input, input, input);

        regexRule->GetType();
        icu::RegexPattern* pattern = regexRule->GetPattern();
        i18n::phonenumbers::PhoneNumber phoneNumber;
        PhoneNumberUtil* phoneNumberUtil = i18n::phonenumbers::PhoneNumberUtil::GetInstance();
        std::string country = LocaleConfig::GetSystemRegion();
        phoneNumberUtil->Parse(input, country, &phoneNumber);

        PhoneNumberMatch* possibleNumber = new PhoneNumberMatch(size, input, phoneNumber);
        regexRule->Handle(possibleNumber, regex);
        regexRule->IsValid(possibleNumber, regex);
        regexRule->CountDigits(regex);
        delete possibleNumber;
        delete pattern;
        delete regexRule;
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

