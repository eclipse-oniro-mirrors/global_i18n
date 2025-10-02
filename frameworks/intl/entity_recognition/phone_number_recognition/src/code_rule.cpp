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
#include "code_rule.h"
#include "i18n_hilog.h"
#include "phonenumbers/phonenumberutil.h"
#include "phonenumbers/phonenumber.h"
#include "phonenumbers/shortnumberinfo.h"

namespace OHOS {
namespace Global {
namespace I18n {
using i18n::phonenumbers::PhoneNumberMatch;
using i18n::phonenumbers::PhoneNumber;
using i18n::phonenumbers::PhoneNumberUtil;
using i18n::phonenumbers::ShortNumberInfo;

CodeRule::CodeRule(std::string& isValidType)
{
    this->isValidType = isValidType;
}

int CodeRule::CountDigits(icu::UnicodeString& str)
{
    int count = 0;
    int len = str.length();
    for (int i = 0; i < len; i++) {
        if (u_isdigit(str[i])) {
            count++;
        }
    }
    return count;
}
PhoneNumberMatch* CodeRule::IsValid(PhoneNumberMatch* possibleNumber, icu::UnicodeString& message)
{
    if (isValidType == "PreSuf") {
        return IsValidPreSuf(possibleNumber, message);
    } else if (isValidType == "Code") {
        return IsValidCode(possibleNumber, message);
    } else if (isValidType == "Rawstr") {
        return IsValidRawstr(possibleNumber, message);
    }
    return IsValidDefault(possibleNumber, message);
}

// Check the preifx or suffix of possibleNumber
PhoneNumberMatch* CodeRule::IsValidPreSuf(PhoneNumberMatch* possibleNumber, icu::UnicodeString& message)
{
    if (possibleNumber->start() - 1 >= 0) {
        return IsValidStart(possibleNumber, message);
    }
    if (possibleNumber->end() <= message.length() - 1) {
        return IsValidEnd(possibleNumber, message);
    }
    return possibleNumber;
}

// check the suffix of possibleNumber
PhoneNumberMatch* CodeRule::IsValidEnd(PhoneNumberMatch* possibleNumber, icu::UnicodeString& message)
{
    icu::UnicodeString after = message.tempSubString(possibleNumber->end());
    bool isTwo = true;
    int len = after.length();
    // check the 1st and 2nd char of the suffix.
    for (int i = 0; i < len; i++) {
        UChar32 afterChar = after[i];
        if (i == 0 && !u_isUUppercase(afterChar)) {
            isTwo = false;
            break;
        }
        // 2 is the third position in the string.
        if (i < 2 && u_isUAlphabetic(afterChar)) {
            if (u_isUUppercase(afterChar)) {
                continue;
            } else {
                isTwo = false;
                break;
            }
        }
        // 1 and 2 are the second and third position in the string, respectively.
        if (i == 1 || i == 2) {
            if (afterChar == '-' || afterChar == '\'') {
                isTwo = false;
                break;
            } else if (u_isdigit(afterChar) || u_isspace(afterChar)) {
                break;
            } else if (!u_isUAlphabetic(afterChar)) {
                break;
            } else {
                isTwo = false;
                break;
            }
        }
    }
    return isTwo ? nullptr : possibleNumber;
}

// check the prefix of possibleNumber
PhoneNumberMatch* CodeRule::IsValidStart(PhoneNumberMatch* possibleNumber, icu::UnicodeString& message)
{
    icu::UnicodeString before = message.tempSubString(0, possibleNumber->start());
    bool isTwo = true;
    int len = before.length();
    for (int i = 0; i < len; i++) {
        char beforeChar = before[len - 1 - i];
        if (i == 0 && !u_isUUppercase(beforeChar)) {
            isTwo = false;
            break;
        }
        // 2 is the third position in the string.
        if (i < 2 && u_isUAlphabetic(beforeChar)) {
            if (u_isUUppercase(beforeChar)) {
                continue;
            } else {
                isTwo = false;
                break;
            }
        }
        if (beforeChar == '-' || beforeChar == '\'') {
            isTwo = false;
            break;
        } else if (u_isdigit(beforeChar) || u_isspace(beforeChar)) {
            break;
        } else if (!u_isUAlphabetic(beforeChar)) {
            break;
        } else {
            isTwo = false;
            break;
        }
    }
    return isTwo ? nullptr : possibleNumber;
}

PhoneNumberMatch* CodeRule::IsValidDefault(PhoneNumberMatch* possibleNumber, icu::UnicodeString& message)
{
    return possibleNumber;
}

bool CodeRule::PrefixValid(icu::UnicodeString& number, int length)
{
    icu::UnicodeString preNumber = number.tempSubString(0, length);
    if (length == 1) {
        if (number[0] == '0' || number[0] == '1' || number[0] == '+') {
            return true;
        }
    // 3 indicates the first three digits of a phone number.
    } else if (length == 3) {
        if (preNumber == "400" || preNumber == "800") {
            return true;
        }
    // 5 indicates the first five digits of a phone number.
    } else if (length == 5) {
        if (preNumber == "11808" || preNumber == "17909" || preNumber == "12593" ||
            preNumber == "17951" || preNumber == "17911") {
            return true;
        }
    }
    return false;
}

bool CodeRule::NumberValid(icu::UnicodeString& number)
{
    if (number.length() < 1) {
        return false;
    }
    int lengthOne = 1;
    // 3 indicates the first three digits of a phone number.
    int lengthThree = 3;
    // 11 is the number of digits in the phone number.
    if (number[0] == '1' && CountDigits(number) > 11) {
        // 5 indicates the first five digits of a phone number.
        int lengthFive = 5;
        if (!PrefixValid(number, lengthFive)) {
            return false;
        }
    // 12 is the number of digits, 0 and 1 indicate the first and second position, respectively.
    } else if (number[0] == '0' && CountDigits(number) > 12 && number[1] != '0') {
        return false;
    // 10 is the number of digits in the phone number.
    } else if (PrefixValid(number, lengthThree) && CountDigits(number) != 10) {
        return false;
    // 9 is the number of digits in the phone number.
    } else if (!PrefixValid(number, lengthOne) && !PrefixValid(number, lengthThree) && CountDigits(number) >= 9) {
        if (number.trim()[0] != '9' && number.trim()[0] != '1') {
            return false;
        }
    // 4 is the number of digits in the phone number.
    } else if (CountDigits(number) <= 4) {
        return false;
    }
    return true;
}

PhoneNumberMatch* CodeRule::IsValidCode(PhoneNumberMatch* possibleNumber, icu::UnicodeString& message)
{
    if (possibleNumber == nullptr) {
        return nullptr;
    }
    icu::UnicodeString number = possibleNumber->raw_string().c_str();
    // Processes the ;ext= extention number format
    int32_t ind = number.trim().indexOf(";ext=");
    if (ind != -1) {
        number = number.trim().tempSubString(0, ind);
    }
    if (number[0] == '(' || number[0] == '[') {
        StartWithBrackets(number);
    }
    if (!NumberValid(number)) {
        return nullptr;
    }
    return possibleNumber;
}

PhoneNumberMatch* CodeRule::IsValidRawstr(PhoneNumberMatch* possibleNumber, icu::UnicodeString& message)
{
    if (possibleNumber == nullptr) {
        return nullptr;
    }
    icu::UnicodeString number = possibleNumber->raw_string().c_str();
    // Processes the ;ext= extention number format
    int32_t ind = number.trim().indexOf(";ext=");
    if (ind != -1) {
        number = number.trim().tempSubString(0, ind);
    }
    if (number[0] == '(' || number[0] == '[') {
        number = number.tempSubString(1);
    }
    // 8 and 4 is the number of digits in the phone number.
    if ((number[0] != '0' && CountDigits(number) == 8) || CountDigits(number) <= 4) {
        return nullptr;
    }
    return possibleNumber;
}

// Handle phone number starting with '(' or '['
void CodeRule::StartWithBrackets(icu::UnicodeString& number)
{
    icu::UnicodeString right = "";
    if (number[0] == '(') {
        right = ')';
    }
    if (number[0] == '[') {
        right = ']';
    }
    int neind = number.indexOf(right);
    if (neind != -1) {
        icu::UnicodeString phoneStr = number.tempSubString(0, neind);
        int phoneLength = CountDigits(phoneStr);
        icu::UnicodeString extraStr = number.tempSubString(neind);
        int extra = CountDigits(extraStr);
        // 4 is the number of numbers in parentheses, 1 and 2 are the number of numbers outside parentheses.
        bool flag = (phoneLength > 4) && (extra == 1 || extra == 2);
        number = flag ? number.tempSubString(1, neind - 1) : number.tempSubString(1);
    } else {
        number = number.tempSubString(1);
    }
}

PhoneNumberMatch* CodeRule::Handle(PhoneNumberMatch* phoneNumberMatch, icu::UnicodeString& message)
{
    if (phoneNumberMatch == nullptr) {
        return nullptr;
    }
    return IsValid(phoneNumberMatch, message);
}
} // namespace I18n
} // namespace Global
} // namespace OHOS