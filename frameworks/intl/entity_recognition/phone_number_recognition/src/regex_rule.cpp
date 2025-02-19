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
#include "regex_rule.h"
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

RegexRule::RegexRule(icu::UnicodeString& regex, std::string& isValidType, std::string& handleType,
    std::string& insensitive, std::string& type)
{
    this->regex = regex;
    if (type == "CONTAIN") {
        // 9 indicates a certain execution logic of the border rule.
        this->type = 9;
    } else if (type == "CONTAIN_OR_INTERSECT") {
        // 8 indicates a certain execution logic of the border rule.
        this->type = 8;
    } else {
        this->type = 0;
    }
    this->status = U_ZERO_ERROR;
    this->isValidType = isValidType;
    this->handleType = handleType;
    this->insensitive = insensitive;
    if (regex.length() == 0) {
        return;
    }
    if (U_FAILURE(this->status)) {
        HILOG_ERROR_I18N("member pattern construct failed.");
    }
}

RegexRule::~RegexRule()
{
}

int RegexRule::CountDigits(icu::UnicodeString& str)
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

int RegexRule::GetType()
{
    return type;
}

icu::RegexPattern* RegexRule::GetPattern()
{
    // Sets whether regular expression matching is case sensitive
    if (insensitive == "True") {
        return icu::RegexPattern::compile(this->regex, URegexpFlag::UREGEX_CASE_INSENSITIVE, this->status);
    } else {
        return icu::RegexPattern::compile(this->regex, 0, this->status);
    }
}

PhoneNumberMatch* RegexRule::IsValid(PhoneNumberMatch* possibleNumber, icu::UnicodeString& message)
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
PhoneNumberMatch* RegexRule::IsValidPreSuf(PhoneNumberMatch* possibleNumber, icu::UnicodeString& message)
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
PhoneNumberMatch* RegexRule::IsValidEnd(PhoneNumberMatch* possibleNumber, icu::UnicodeString& message)
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
    if (!isTwo) {
        return possibleNumber;
    } else {
        return nullptr;
    }
}

// check the prefix of possibleNumber
PhoneNumberMatch* RegexRule::IsValidStart(PhoneNumberMatch* possibleNumber, icu::UnicodeString& message)
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
    if (!isTwo) {
        return possibleNumber;
    } else {
        return nullptr;
    }
}

PhoneNumberMatch* RegexRule::IsValidDefault(PhoneNumberMatch* possibleNumber, icu::UnicodeString& message)
{
    return possibleNumber;
}

bool RegexRule::PrefixValid(icu::UnicodeString& number, int length)
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

bool RegexRule::NumberValid(icu::UnicodeString& number)
{
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

PhoneNumberMatch* RegexRule::IsValidCode(PhoneNumberMatch* possibleNumber, icu::UnicodeString& message)
{
    bool isValid = true;
    icu::UnicodeString number = possibleNumber->raw_string().c_str();
    // Processes the ;ext= extention number format
    int32_t ind = number.trim().indexOf(";ext=");
    if (ind != -1) {
        number = number.trim().tempSubString(0, ind);
    }
    if (number[0] == '(' || number[0] == '[') {
        StartWithBrackets(number);
    }
    isValid = NumberValid(number);
    if (isValid) {
        return possibleNumber;
    } else {
        return nullptr;
    }
}

PhoneNumberMatch* RegexRule::IsValidRawstr(PhoneNumberMatch* possibleNumber, icu::UnicodeString& message)
{
    bool isValid = true;
    icu::UnicodeString number = possibleNumber->raw_string().c_str();
    // Processes the ;ext= extention number format
    int32_t ind = number.trim().indexOf(";ext=");
    if (ind != -1) {
        number = number.trim().tempSubString(0, ind);
    }
    if (number[0] == '(' || number[0] == '[') {
        number = number.tempSubString(1);
    }
    // 8 is the number of digits in the phone number.
    if (number[0] != '0' && CountDigits(number) == 8) {
        isValid = false;
    }
    // 4 is the number of digits in the phone number.
    if (CountDigits(number) <= 4) {
        isValid = false;
    }
    if (isValid) {
        return possibleNumber;
    } else {
        return nullptr;
    }
}

std::vector<MatchedNumberInfo> RegexRule::Handle(PhoneNumberMatch *possibleNumber, icu::UnicodeString& message)
{
    if (handleType == "Operator") {
        return HandleOperator(possibleNumber, message);
    } else if (handleType == "Blank") {
        return HandleBlank(possibleNumber, message);
    } else if (handleType == "Slant") {
        return HandleSlant(possibleNumber, message);
    } else if (handleType == "StartWithMobile") {
        return HandleStartWithMobile(possibleNumber, message);
    } else if (handleType == "EndWithMobile") {
        return HandleEndWithMobile(possibleNumber, message);
    }
    return HandleDefault(possibleNumber, message);
}

std::vector<MatchedNumberInfo> RegexRule::HandleDefault(PhoneNumberMatch* possibleNumber, icu::UnicodeString& message)
{
    MatchedNumberInfo matcher;
    matcher.SetBegin(0);
    matcher.SetEnd(1);
    icu::UnicodeString content = "";
    matcher.SetContent(content);
    std::vector<MatchedNumberInfo> matchedNumberInfoList;
    matchedNumberInfoList.push_back(matcher);
    return matchedNumberInfoList;
}

std::vector<MatchedNumberInfo> RegexRule::HandleOperator(PhoneNumberMatch* possibleNumber, icu::UnicodeString& message)
{
    MatchedNumberInfo matcher;
    if (possibleNumber->raw_string()[0] == '(' || possibleNumber->raw_string()[0] == '[') {
        matcher.SetBegin(possibleNumber->start() + 1);
    } else {
        matcher.SetBegin(possibleNumber->start());
    }
    matcher.SetEnd(possibleNumber->end());
    matcher.SetContent(message);
    std::vector<MatchedNumberInfo> matchedNumberInfoList;
    matchedNumberInfoList.push_back(matcher);
    return matchedNumberInfoList;
}

std::vector<MatchedNumberInfo> RegexRule::HandleBlank(PhoneNumberMatch* possibleNumber, icu::UnicodeString& message)
{
    // exclude phone number 5201314
    icu::UnicodeString speString = "5201314";
    MatchedNumberInfo matchedNumberInfo;
    icu::UnicodeString number = possibleNumber->raw_string().c_str();
    icu::RegexPattern* pattern = GetPattern();
    UErrorCode status;
    icu::RegexMatcher* matcher = pattern->matcher(number, status);
    UErrorCode negativeStatus = U_ZERO_ERROR;
    // exclude phone number 2333333
    icu::UnicodeString negativeRegex = "(?<![-\\d])(23{6,7})(?![-\\d])";
    icu::RegexMatcher negativePattern(negativeRegex, 0, negativeStatus);
    negativePattern.reset(number);
    std::vector<MatchedNumberInfo> matchedNumberInfoList;
    if (matcher != nullptr && matcher->find()) {
        if (negativePattern.find() || number == speString) {
            return matchedNumberInfoList;
        }
        if (possibleNumber->raw_string()[0] != '(' && possibleNumber->raw_string()[0] != '[') {
            matchedNumberInfo.SetBegin(matcher->start(status) + possibleNumber->start());
        } else {
            matchedNumberInfo.SetBegin(possibleNumber->start());
        }
        matchedNumberInfo.SetEnd(matcher->end(status) + possibleNumber->start());
        matchedNumberInfo.SetContent(number);
        matchedNumberInfoList.push_back(matchedNumberInfo);
    }
    delete matcher;
    delete pattern;
    return matchedNumberInfoList;
}

std::vector<MatchedNumberInfo> RegexRule::HandleSlant(PhoneNumberMatch* possibleNumber, icu::UnicodeString& message)
{
    MatchedNumberInfo matchedNumberInfo;
    MatchedNumberInfo numberInfo;
    icu::UnicodeString number = possibleNumber->raw_string().c_str();
    icu::RegexPattern* pattern = GetPattern();
    UErrorCode status;
    icu::RegexMatcher* matcher = pattern->matcher(number, status);
    std::vector<MatchedNumberInfo> matchedNumberInfoList;
    if (matcher != nullptr && matcher->find()) {
        int start = matcher->start(status);
        std::vector<MatchedNumberInfo> tempList = GetNumbersWithSlant(number);
        // 2 is the size of tempList.
        if (tempList.size() == 2 && start == 1) {
            start = 0;
        }
        if (tempList.size() > 0) {
            matchedNumberInfo.SetBegin(tempList[0].GetBegin() + start + possibleNumber->start());
            matchedNumberInfo.SetEnd(tempList[0].GetEnd() + possibleNumber->start());
            icu::UnicodeString contentFirst = tempList[0].GetContent();
            matchedNumberInfo.SetContent(contentFirst);
            matchedNumberInfoList.push_back(matchedNumberInfo);
            // 2 is the size of tempList.
            if (tempList.size() == 2) {
                numberInfo.SetBegin(tempList[1].GetBegin() + start + possibleNumber->start());
                numberInfo.SetEnd(tempList[1].GetEnd() + possibleNumber->start());
                icu::UnicodeString contentSecond = tempList[1].GetContent();
                numberInfo.SetContent(contentSecond);
                matchedNumberInfoList.push_back(numberInfo);
            }
        }
    }
    delete matcher;
    delete pattern;
    return matchedNumberInfoList;
}

std::vector<MatchedNumberInfo> RegexRule::HandleStartWithMobile(PhoneNumberMatch* possibleNumber,
    icu::UnicodeString& message)
{
    return HandlePossibleNumberWithPattern(possibleNumber, message, false);
}

std::vector<MatchedNumberInfo> RegexRule::HandleEndWithMobile(PhoneNumberMatch* possibleNumber,
    icu::UnicodeString& message)
{
    return HandlePossibleNumberWithPattern(possibleNumber, message, true);
}

// Handle phone number starting with '(' or '['
void RegexRule::StartWithBrackets(icu::UnicodeString& number)
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
        if ((phoneLength > 4) && (extra == 1 || extra == 2)) {
            number = number.tempSubString(1, neind - 1);
        } else {
            number = number.tempSubString(1);
        }
    } else {
        number = number.tempSubString(1);
    }
}

// identify short number separated by '/'
std::vector<MatchedNumberInfo> RegexRule::GetNumbersWithSlant(icu::UnicodeString& testStr)
{
    std::vector<MatchedNumberInfo> shortList;
    PhoneNumberUtil* pnu = PhoneNumberUtil::GetInstance();
    ShortNumberInfo* shortInfo = new (std::nothrow) ShortNumberInfo();
    if (shortInfo == nullptr) {
        HILOG_ERROR_I18N("ShortNumberInfo construct failed.");
        return shortList;
    }
    std::string numberFisrt = "";
    std::string numberEnd = "";
    int slantIndex = 0;
    for (int i = 0; i < testStr.length(); i++) {
        if (testStr[i] == '/' || testStr[i] == '|') {
            slantIndex = i;
            testStr.tempSubString(0, i).toUTF8String(numberFisrt);
            testStr.tempSubString(i + 1).toUTF8String(numberEnd);
        }
    }
    PhoneNumber phoneNumberFirst;
    PhoneNumber phoneNumberEnd;
    pnu->Parse(numberFisrt, "CN", &phoneNumberFirst);
    pnu->Parse(numberEnd, "CN", &phoneNumberEnd);
    if (shortInfo->IsValidShortNumber(phoneNumberFirst)) {
        MatchedNumberInfo matchedNumberInfoFirst;
        matchedNumberInfoFirst.SetBegin(0);
        matchedNumberInfoFirst.SetEnd(slantIndex);
        icu::UnicodeString contentFirst = numberFisrt.c_str();
        matchedNumberInfoFirst.SetContent(contentFirst);
        shortList.push_back(matchedNumberInfoFirst);
    }
    if (shortInfo->IsValidShortNumber(phoneNumberEnd)) {
        MatchedNumberInfo matchedNumberInfoEnd;
        matchedNumberInfoEnd.SetBegin(slantIndex + 1);
        matchedNumberInfoEnd.SetEnd(testStr.length());
        icu::UnicodeString contentEnd = numberEnd.c_str();
        matchedNumberInfoEnd.SetContent(contentEnd);
        shortList.push_back(matchedNumberInfoEnd);
    }
    delete shortInfo;
    return shortList;
}

std::vector<MatchedNumberInfo> RegexRule::HandlePossibleNumberWithPattern(PhoneNumberMatch* possibleNumber,
    icu::UnicodeString& message, bool isStartsWithNumber)
{
    UErrorCode status = U_ZERO_ERROR;
    std::vector<MatchedNumberInfo> matchedList;
    icu::UnicodeString possible = possibleNumber->raw_string().c_str();
    icu::RegexPattern* pattern = GetPattern();
    icu::RegexMatcher* mat = pattern->matcher(message, status);
    while (mat != nullptr && mat->find(status)) {
        int start = mat->start(status);
        int end = mat->end(status);
        icu::UnicodeString matched = message.tempSubString(start, end - start);
        bool isMatch = isStartsWithNumber ? matched.startsWith(possible) : matched.endsWith(possible);
        if (isMatch) {
            MatchedNumberInfo info;
            info.SetBegin(isStartsWithNumber ? start : end - possible.length());
            info.SetEnd(isStartsWithNumber ? (start + possible.length()) : end);
            info.SetContent(possible);
            matchedList.push_back(info);
        }
    }
    delete mat;
    delete pattern;
    return matchedList;
}
} // namespace I18n
} // namespace Global
} // namespace OHOS