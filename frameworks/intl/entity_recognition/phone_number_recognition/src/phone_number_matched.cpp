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

#include <climits>
#include <set>
#include "i18n_hilog.h"
#include "regex_rule.h"
#include "phone_number_matched.h"
#include "utils.h"

namespace OHOS {
namespace Global {
namespace I18n {
using i18n::phonenumbers::PhoneNumber;

const int PhoneNumberMatched::CONTAIN = 9;
const int PhoneNumberMatched::CONTAIN_OR_INTERSECT = 8;
const UChar32 PhoneNumberMatched::REPLACE_CHAR = 'A';

PhoneNumberMatched::PhoneNumberMatched(std::string& country)
{
    phoneNumberRule = new PhoneNumberRule(country);
    phoneNumberUtil = PhoneNumberUtil::GetInstance();
    shortNumberInfo = new ShortNumberInfo();
    this->country = country;
    if (phoneNumberRule != nullptr) {
        phoneNumberRule->Init();
    }
}

PhoneNumberMatched::~PhoneNumberMatched()
{
    delete phoneNumberRule;
    delete shortNumberInfo;
}

std::vector<int> PhoneNumberMatched::GetMatchedPhoneNumber(icu::UnicodeString& message)
{
    icu::UnicodeString messageStr = message;
    if (!phoneNumberRule->isFixed) {
        return DealWithoutFixed(messageStr, country);
    }
    icu::UnicodeString filteredString = HandleNegativeRule(messageStr);
    std::vector<MatchedNumberInfo> matchedNumberInfoList = GetPossibleNumberInfos(country,
        messageStr, filteredString);
    std::vector<MatchedNumberInfo> shortList = FindShortNumbers(country, filteredString);
    if (shortList.size() != 0) {
        matchedNumberInfoList.insert(matchedNumberInfoList.end(), shortList.begin(), shortList.end());
    }
    matchedNumberInfoList = DeleteRepeatedInfo(matchedNumberInfoList);
    for (auto& matchedNumberInfo : matchedNumberInfoList) {
        DealNumberWithOneBracket(matchedNumberInfo);
    }
    return DealResult(matchedNumberInfoList);
}

std::vector<int> PhoneNumberMatched::DealWithoutFixed(icu::UnicodeString& message, std::string& country)
{
    std::vector<PhoneNumberMatch*> matchList = FindNumbers(country, message);
    std::vector<MatchedNumberInfo> result;
    for (auto& match : matchList) {
        if (match == nullptr) continue;
        MatchedNumberInfo info;
        icu::UnicodeString content = match->raw_string().c_str();
        PhoneNumber phoneNumber = match->number();
        if (phoneNumberUtil->IsValidNumber(phoneNumber)) {
            info.SetBegin(match->start());
            info.SetEnd(match->end());
            info.SetContent(content);
            result.push_back(info);
        }
        delete match;
    }
    std::vector<MatchedNumberInfo> shortResult = FindShortNumbers(country, message);
    // Merge result
    if (shortResult.size() != 0) {
        result.insert(result.end(), shortResult.begin(), shortResult.end());
    }
    result = DeleteRepeatedInfo(result);
    for (auto& res: result) {
        DealNumberWithOneBracket(res);
    }
    return DealResult(result);
}

// Filtering text using negative rules
icu::UnicodeString PhoneNumberMatched::HandleNegativeRule(icu::UnicodeString& src)
{
    std::vector<NegativeRule*> rules = phoneNumberRule->GetNegativeRules();
    icu::UnicodeString ret = src;
    for (NegativeRule* rule : rules) {
        if (rule == nullptr) {
            continue;
        }
        ret = rule->Handle(ret);
    }
    return ret;
}

// Replace the characters in the specified area with REPLACE_CHAR
void PhoneNumberMatched::ReplaceSpecifiedPos(icu::UnicodeString& chs, int start, int end)
{
    if (start < end) {
        int len = chs.length();
        for (int i = 0; i < len; i++) {
            if (i >= start && i < end) {
                chs.replace(i, 1, PhoneNumberMatched::REPLACE_CHAR);
            }
        }
    }
}

// Short numbers identification
std::vector<MatchedNumberInfo> PhoneNumberMatched::FindShortNumbers(std::string& country,
    icu::UnicodeString& message)
{
    std::vector<MatchedNumberInfo> matchedNumberInfoList;
    UErrorCode status = U_ZERO_ERROR;
    size_t pos = phoneNumberRule->GetFindRules().size();
    if (pos == 0) {
        HILOG_ERROR_I18N("PhoneNumberRule.findRules is empty.");
        return matchedNumberInfoList;
    }
    // 1 indicates the last position.
    FindRule* shortRegexRule = phoneNumberRule->GetFindRules()[pos - 1];
    icu::RegexPattern* shortPattern = shortRegexRule->GetPattern();
    if (shortPattern == nullptr) {
        HILOG_ERROR_I18N("shortPattern getPattern failed.");
        return matchedNumberInfoList;
    }
    icu::RegexMatcher* shortMatch = shortPattern->matcher(message, status);
    if (shortMatch == nullptr) {
        delete shortPattern;
        HILOG_ERROR_I18N("shortPattern matcher failed.");
        return matchedNumberInfoList;
    }
    while (shortMatch->find(status)) {
        icu::UnicodeString numberToParse = shortMatch->group(status);
        std::string stringParse;
        numberToParse.toUTF8String(stringParse);
        PhoneNumber phoneNumber;
        PhoneNumberUtil::ErrorType errorType =
            phoneNumberUtil->ParseAndKeepRawInput(stringParse, country, &phoneNumber);
        if (errorType != PhoneNumberUtil::NO_PARSING_ERROR) {
            HILOG_ERROR_I18N("PhoneNumberRule: failed to call the ParseAndKeepRawInput.");
            continue;
        }
        // Add the valid short number to the result
        if (shortNumberInfo->IsPossibleShortNumberForRegion(phoneNumber, country)) {
            MatchedNumberInfo matcher;
            matcher.SetBegin(shortMatch->start(status));
            matcher.SetEnd(shortMatch->end(status));
            icu::UnicodeString stringShort = shortMatch->group(status);
            matcher.SetContent(stringShort);
            matchedNumberInfoList.push_back(matcher);
        }
    }
    delete shortMatch;
    delete shortPattern;
    return matchedNumberInfoList;
}

// Add the phone number that may be correct, and return true if successful
bool PhoneNumberMatched::AddPhoneNumber(std::string& number, int start, std::vector<PhoneNumberMatch*>& matchList,
    std::string& country)
{
    PhoneNumber phoneNumber;
    int lenNumber = 5;
    icu::UnicodeString uNumber = number.c_str();
    if (RegexRule::CountDigits(uNumber) < lenNumber) {
        return false;
    }
    PhoneNumberUtil::ErrorType parseStatus = phoneNumberUtil->Parse(number, country, &phoneNumber);
    if (parseStatus != PhoneNumberUtil::NO_PARSING_ERROR) {
        return false;
    }
    UChar32 space = ' ';
    UChar32 slash = '/';
    // Add to matchList if phone number is not delimited, or valid
    if ((uNumber.indexOf(space) == -1 && uNumber.indexOf(slash) == -1) ||
        phoneNumberUtil->IsValidNumber(phoneNumber)) {
        PhoneNumberMatch* match = new PhoneNumberMatch(start, number, phoneNumber);
        matchList.push_back(match);
        return true;
    }
    return false;
}

// Add the valid phone number
std::vector<PhoneNumberMatch*> PhoneNumberMatched::FindNumbers(std::string& country,
    icu::UnicodeString& filteredString)
{
    std::vector<PhoneNumberMatch*> matchList;
    UErrorCode status = U_ZERO_ERROR;
    size_t pos = phoneNumberRule->GetFindRules().size();
    // 2 indicates the penultimate position.
    FindRule* numberRegexRule = phoneNumberRule->GetFindRules()[pos - 2];
    icu::RegexPattern* numberPattern = numberRegexRule->GetPattern();
    if (numberPattern == nullptr) {
        HILOG_ERROR_I18N("numberRegexRule getPattern failed.");
        return matchList;
    }
    icu::RegexMatcher* numberMatcher = numberPattern->matcher(filteredString, status);
    if (numberMatcher == nullptr) {
        HILOG_ERROR_I18N("numberPattern matcher failed.");
        delete numberPattern;
        return matchList;
    }
    while (numberMatcher->find(status)) {
        int32_t start = numberMatcher->start(status);
        int32_t end = numberMatcher->end(status);
        icu::UnicodeString uNumber = filteredString.tempSubString(start, end - start);
        std::string number;
        uNumber.toUTF8String(number);
        // if the entire phone number is invalid, identify each segment that is separated
        if (!AddPhoneNumber(number, start, matchList, country)) {
            int searchStart = 0;
            UChar32 space = ' ';
            UChar32 slash = '/';
            if (uNumber.indexOf(space, searchStart) == -1 && uNumber.indexOf(slash, searchStart) == -1) {
                continue;
            }
            while (uNumber.indexOf(space, searchStart) != -1 || uNumber.indexOf(slash, searchStart) != -1) {
                int phoneStart = searchStart;
                int indexSpace = uNumber.indexOf(space, searchStart);
                int indexSlash = uNumber.indexOf(slash, searchStart);
                int phoneEnd =
                    (indexSpace == -1 || (indexSlash != -1 && indexSlash < indexSpace)) ? indexSlash : indexSpace;
                searchStart = phoneEnd + 1;
                std::string tempNumber = number.substr(phoneStart, phoneEnd - phoneStart);
                AddPhoneNumber(tempNumber, phoneStart + start, matchList, country);
            }
            // identify the last segment
            std::string lastStr = number.substr(searchStart);
            AddPhoneNumber(lastStr, searchStart + start, matchList, country);
        }
    }
    delete numberMatcher;
    delete numberPattern;
    return matchList;
}

// Handing the situation of shortnumber/shortnumber.
bool PhoneNumberMatched::HandleWithShortAndShort(std::vector<MatchedNumberInfo>& result, std::string& country,
    MatchedNumberInfo& info, std::pair<int, int>& pos, icu::UnicodeString& filteredString)
{
    bool flag = false;
    // 3 is the length of findRules.
    size_t length = 3;
    if (phoneNumberRule->GetFindRules().size() == length) {
        UErrorCode status = U_ZERO_ERROR;
        FindRule* regexRule = phoneNumberRule->GetFindRules()[0];
        icu::RegexPattern* pattern = regexRule->GetPattern();
        if (pattern == nullptr) {
            HILOG_ERROR_I18N("regexRule getPattern failed.");
            return flag;
        }
        icu::UnicodeString str = "";
        if (info.GetContent()[0] == '(' || info.GetContent()[0] == '[') {
            str = info.GetContent().tempSubString(1);
        } else {
            str = info.GetContent();
        }
        icu::RegexMatcher* matcher = pattern->matcher(str, status);
        if (matcher == nullptr) {
            delete pattern;
            HILOG_ERROR_I18N("pattern matcher failed.");
            return flag;
        }
        if (!matcher->find(status)) {
            result.push_back(info);
            ReplaceSpecifiedPos(filteredString, pos.first, pos.second);
            flag = true;
        }
        delete matcher;
        delete pattern;
    } else {
        result.push_back(info);
        ReplaceSpecifiedPos(filteredString, pos.first, pos.second);
    }
    return flag;
}

// Get possible phone number
std::vector<MatchedNumberInfo> PhoneNumberMatched::GetPossibleNumberInfos(std::string& country,
    icu::UnicodeString& src, icu::UnicodeString& filteredString)
{
    std::vector<MatchedNumberInfo> result;
    std::vector<PhoneNumberMatch*> matchList = FindNumbers(country, filteredString);
    for (auto match : matchList) {
        if (!HandleBorderRule(match, filteredString)) {
            continue;
        }
        PhoneNumberMatch* delMatch = HandleCodesRule(match, src);
        if (delMatch == nullptr) {
            continue;
        }
        icu::UnicodeString content = delMatch->raw_string().c_str();
        int contentStart = delMatch->start();
        int contentEnd = delMatch->end();
        std::pair<int, int> pos{contentStart, contentEnd};
        if (phoneNumberUtil->IsValidNumber(delMatch->number())) {
            MatchedNumberInfo info;
            info.SetBegin(contentStart);
            info.SetEnd(contentEnd);
            info.SetContent(content);
            bool flag = HandleWithShortAndShort(result, country, info, pos, filteredString);
            if (flag) {
                continue;
            }
        }
        std::vector<MatchedNumberInfo> posList = HandlePositiveRule(delMatch, filteredString);
        if (posList.size() != 0) {
            for (auto& matchInfo : posList) {
                ReplaceSpecifiedPos(filteredString, matchInfo.GetBegin(), matchInfo.GetEnd());
            }
            result.insert(result.end(), posList.begin(), posList.end());
        }
        delete match;
    }
    return result;
}

// Remove duplicate results
std::vector<MatchedNumberInfo> PhoneNumberMatched::DeleteRepeatedInfo(std::vector<MatchedNumberInfo>& list)
{
    std::set<MatchedNumberInfo> set;
    std::vector<MatchedNumberInfo> ret;
    for (auto info : list) {
        if (set.find(info) == set.end()) {
            ret.push_back(info);
        }
        set.insert(info);
    }
    return ret;
}

// Process the case where the phone number starts with one bracket
void PhoneNumberMatched::DealNumberWithOneBracket(MatchedNumberInfo& info)
{
    icu::UnicodeString message = info.GetContent();
    if (IsNumberWithOneBracket(message)) {
        info.SetBegin(info.GetBegin() + 1);
        icu::UnicodeString content = info.GetContent().tempSubString(1);
        info.SetContent(content);
    }
}

// check whether the bracket at the start position are redundant
bool PhoneNumberMatched::IsNumberWithOneBracket(icu::UnicodeString& message)
{
    if (message != "") {
        int numLeft = 0;
        int numRight = 0;
        int len = message.length();
        for (int i = 0; i < len; i++) {
            if (message[i] == '(' || message[i] == '[') {
                numLeft++;
            }
            if (message[i] == ')' || message[i] == ']') {
                numRight++;
            }
        }
        if (numLeft > numRight && (message[0] == '(' || message[0] == '[')) {
            return true;
        }
    }
    return false;
}

std::vector<int> PhoneNumberMatched::DealResult(std::vector<MatchedNumberInfo>& matchedNumberInfoList)
{
    std::vector<int> result;
    size_t length = matchedNumberInfoList.size();
    if (length == 0) {
        result.push_back(0);
    } else {
        size_t posNumber = 2;
        size_t posStart = 1;
        size_t posEnd = 2;
        result.resize(posNumber * length + 1);
        result[0] = static_cast<int>(length);
        for (size_t i = 0; i < length; i++) {
            result[posNumber * i + posStart] = matchedNumberInfoList[i].GetBegin();
            result[posNumber * i + posEnd] = matchedNumberInfoList[i].GetEnd();
        }
    }
    return result;
}

// Filter result based on Border rule
bool PhoneNumberMatched::HandleBorderRule(PhoneNumberMatch* match, icu::UnicodeString& message)
{
    if (match == nullptr) {
        return false;
    }
    std::vector<BorderRule*> rules = phoneNumberRule->GetBorderRules();
    if (rules.size() == 0) {
        return true;
    }
    for (BorderRule* rule : rules) {
        if (!rule->Handle(match, message)) {
            return false;
        }
    }
    return true;
}

// Filter result based on Codes rule
PhoneNumberMatch* PhoneNumberMatched::HandleCodesRule(PhoneNumberMatch* phoneNumberMatch, icu::UnicodeString& message)
{
    PhoneNumberMatch* match = phoneNumberMatch;
    std::vector<CodeRule*> rules = phoneNumberRule->GetCodesRules();
    if (rules.size() == 0) {
        return nullptr;
    }
    for (CodeRule* rule : rules) {
        match = rule->Handle(match, message);
    }
    return match;
}

// Add phone numbers that meet the positive rule to the result
std::vector<MatchedNumberInfo> PhoneNumberMatched::HandlePositiveRule(PhoneNumberMatch* match,
    icu::UnicodeString& message)
{
    std::vector<MatchedNumberInfo> infoList;
    std::vector<PositiveRule*> rules = phoneNumberRule->GetPositiveRules();
    for (PositiveRule* rule : rules) {
        infoList = rule->Handle(match, message);
        if (infoList.size() != 0) {
            break;
        }
    }
    return infoList;
}

icu::UnicodeString PhoneNumberMatched::DealStringWithOneBracket(icu::UnicodeString& message)
{
    if (IsNumberWithOneBracket(message)) {
        return message.tempSubString(1);
    }
    return message;
}
} // namespace I18n
} // namespace Global
} // namespace OHOS