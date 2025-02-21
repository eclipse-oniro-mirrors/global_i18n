/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
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
#ifndef OHOS_GLOBAL_REGEX_RULE_H
#define OHOS_GLOBAL_REGEX_RULE_H

#include <unicode/regex.h>
#include <vector>
#include <string>
#include "matched_number_info.h"
#include "phonenumbers/phonenumbermatch.h"

namespace OHOS {
namespace Global {
namespace I18n {
using i18n::phonenumbers::PhoneNumberMatch;
class RegexRule {
public:
    RegexRule(icu::UnicodeString& regex, std::string& IsValidType, std::string& HandleType,
        std::string& insensitive, std::string& type);
    ~RegexRule();

    int GetType(); // border rules
    icu::RegexPattern* GetPattern(); // positive Rules, negative rules, border rules, find rules

    std::vector<MatchedNumberInfo> Handle(PhoneNumberMatch* possibleNumber, icu::UnicodeString& message);
    PhoneNumberMatch* IsValid(PhoneNumberMatch* possibleNumber, icu::UnicodeString& message); // codes rules
    static int CountDigits(icu::UnicodeString& str);

private:
    void StartWithBrackets(icu::UnicodeString& phone); // codes rules
    static std::vector<MatchedNumberInfo> GetNumbersWithSlant(icu::UnicodeString& testStr); // positive Rules
    std::vector<MatchedNumberInfo> HandlePossibleNumberWithPattern(PhoneNumberMatch* possibleNumber,
        icu::UnicodeString& message, bool isStartsWithNumber); // positive Rules
    bool NumberValid(icu::UnicodeString& number); // codes rules
    bool PrefixValid(icu::UnicodeString& number, int length); // codes rules

    PhoneNumberMatch* IsValidEnd(PhoneNumberMatch* possibleNumber, icu::UnicodeString& message); // codes rules
    PhoneNumberMatch* IsValidStart(PhoneNumberMatch* possibleNumber, icu::UnicodeString& message); // codes rules
    PhoneNumberMatch* IsValidDefault(PhoneNumberMatch* possibleNumber, icu::UnicodeString& message); // codes rules
    PhoneNumberMatch* IsValidPreSuf(PhoneNumberMatch* possibleNumber, icu::UnicodeString& message); // codes rules
    PhoneNumberMatch* IsValidCode(PhoneNumberMatch* possibleNumber, icu::UnicodeString& message); // codes rules
    PhoneNumberMatch* IsValidRawstr(PhoneNumberMatch* possibleNumber, icu::UnicodeString& message); // codes rules

    std::vector<MatchedNumberInfo> HandleDefault(PhoneNumberMatch* possibleNumber, icu::UnicodeString& message);
    std::vector<MatchedNumberInfo> HandleOperator(PhoneNumberMatch* possibleNumber, icu::UnicodeString& message);
    std::vector<MatchedNumberInfo> HandleBlank(PhoneNumberMatch* possibleNumber, icu::UnicodeString& message);
    std::vector<MatchedNumberInfo> HandleSlant(PhoneNumberMatch* possibleNumber, icu::UnicodeString& message);
    std::vector<MatchedNumberInfo> HandleStartWithMobile(PhoneNumberMatch* possibleNumber, icu::UnicodeString& message);
    std::vector<MatchedNumberInfo> HandleEndWithMobile(PhoneNumberMatch* possibleNumber, icu::UnicodeString& message);

    int type; // border rules
    icu::UnicodeString regex; // positive Rules, negative rules, border rules, find rules

    UErrorCode status;
    // Decide which IsValid function to use
    std::string isValidType; // codes rules
    // Decide which Handle function to use
    std::string handleType; // positive Rules
    std::string insensitive; // positive Rules, negative rules, border rules, find rules
};
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif