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
#include "date_time_filter.h"
#include <algorithm>
#include "i18n_hilog.h"
#include "utils.h"

namespace OHOS {
namespace Global {
namespace I18n {
DateTimeFilter::DateTimeFilter(std::string& locale, DateTimeRule* dateTimeRule)
{
    this->locale = locale;
    this->dateTimeRule = dateTimeRule;
}

DateTimeFilter::~DateTimeFilter()
{
}

// type of the time calculated based on the rule number.
int DateTimeFilter::GetType(std::string& name)
{
    int32_t status = 0;
    int key = ConvertString2Int(name, status);
    if (status == -1) {
        HILOG_ERROR_I18N("DateTimeFilter::GetType: convert %{public}s to Int failed.", name.c_str());
        return FilterType::TYPE_NULL;
    }
    int type;
    // 19999 and 30000 are the matching rule numbers.
    if (key > 19999 && key < 30000) {
        // 20009, 20011 and 21026 are the TYPE_WEEK rule numbers.
        if (key == 20009 || key == 20011 || key == 21026) {
            type = FilterType::TYPE_WEEK;
        // 20010 is the TYPE_TODAY rule numbers.
        } else if (key == 20010) {
            type = FilterType::TYPE_TODAY;
        } else {
            type = FilterType::TYPE_DATE;
        }
    // 29999 - 40000 is the range of the rule number for TYPE_TIME.
    } else if (key > 29999 && key < 40000) {
        type = FilterType::TYPE_TIME;
    // 9999 - 20000 is the range of the rule number for TYPE_DATETIME.
    } else if (key > 9999 && key < 20000) {
        type = FilterType::TYPE_DATETIME;
    } else {
        type = FilterType::TYPE_TIME_PERIOD;
    }
    return type;
}

std::vector<MatchedDateTimeInfo> DateTimeFilter::Filter(icu::UnicodeString& content,
    std::vector<MatchedDateTimeInfo>& matches, std::vector<MatchedDateTimeInfo>& clearMatches,
    std::vector<MatchedDateTimeInfo>& pastMatches)
{
    matches = FilterOverlay(matches);
    matches = FilterDatePeriod(content, matches);
    matches = FilterByRules(content, matches, clearMatches);
    matches = FilterByPast(content, matches, pastMatches);
    return matches;
}

std::vector<MatchedDateTimeInfo> DateTimeFilter::FilterOverlay(std::vector<MatchedDateTimeInfo>& matches)
{
    if (matches.size() == 0) {
        return matches;
    }
    matches = FilterOverlayFirst(matches);
    matches = FilterOverlaySecond(matches);
    std::sort(matches.begin(), matches.end());
    return matches;
}

// filtering results based on the clear rule.
std::vector<MatchedDateTimeInfo> DateTimeFilter::FilterByRules(icu::UnicodeString& content,
    std::vector<MatchedDateTimeInfo>& matches, std::vector<MatchedDateTimeInfo>& clears)
{
    if (clears.size() == 0) {
        return matches;
    }
    auto matchIterator = matches.begin();
    while (matchIterator != matches.end()) {
        MatchedDateTimeInfo match =  (*matchIterator);
        bool isDelete = false;
        for (auto& clearMatch : clears) {
            // remove the time within the filter range.
            if (match.GetBegin() >= clearMatch.GetBegin() && match.GetEnd() <= clearMatch.GetEnd()) {
                matchIterator = matches.erase(matchIterator);
                isDelete = true;
                break;
            }
        }
        if (!isDelete) {
            matchIterator++;
        }
    }
    return matches;
}

// filtering results based on the past rule.
std::vector<MatchedDateTimeInfo> DateTimeFilter::FilterByPast(icu::UnicodeString& content,
    std::vector<MatchedDateTimeInfo>& matches, std::vector<MatchedDateTimeInfo>& pasts)
{
    if (pasts.size() == 0) {
        return matches;
    }
    int32_t status = 0;
    for (auto& pastMatch : pasts) {
        std::string name = pastMatch.GetRegex();
        int key = ConvertString2Int(name, status);
        auto matchIterator = matches.begin();
        while (matchIterator != matches.end()) {
            MatchedDateTimeInfo match =  (*matchIterator);
            // fields with number < 200 are placed before the time.
            if ((key < 200 && pastMatch.GetEnd() == match.GetBegin()) ||
                // fields with number >= 200 are placed after the time.
                (key >= 200 && pastMatch.GetBegin() == match.GetEnd())) {
                matchIterator = matches.erase(matchIterator);
                break;
            }
            matchIterator++;
        }
    }
    return matches;
}

std::vector<MatchedDateTimeInfo> DateTimeFilter::FilterOverlayFirst(std::vector<MatchedDateTimeInfo>& matches)
{
    std::vector<MatchedDateTimeInfo> matchList;
    for (MatchedDateTimeInfo& match : matches) {
        bool valid = true;
        auto matchIterator = matchList.begin();
        while (matchIterator != matchList.end()) {
            MatchedDateTimeInfo currentMatch =  (*matchIterator);
            if (!(currentMatch.GetBegin() == match.GetBegin() && currentMatch.GetEnd() == match.GetEnd()) &&
                !(currentMatch.GetBegin() < match.GetBegin() && match.GetBegin() < currentMatch.GetEnd() &&
                currentMatch.GetEnd() < match.GetEnd()) && !(match.GetBegin() < currentMatch.GetBegin() &&
                currentMatch.GetBegin() < match.GetEnd() && match.GetEnd() < currentMatch.GetEnd())) {
                matchIterator++;
                continue;
            }
            std::string currentRegex = currentMatch.GetRegex();
            std::string matchRegex = match.GetRegex();
            if (this->dateTimeRule == nullptr) {
                HILOG_ERROR_I18N("FilterOverlayFirst failed because this->dateTimeRule is nullptr.");
                return matchList;
            }
            // if the matched time segments overlap, retain the high-priority.
            if (this->dateTimeRule->CompareLevel(currentRegex, matchRegex) > -1) {
                valid = false;
                matchIterator++;
            } else {
                matchIterator = matchList.erase(matchIterator);
            }
        }
        if (valid) {
            matchList.push_back(match);
        }
    }
    return matchList;
}

std::vector<MatchedDateTimeInfo> DateTimeFilter::FilterOverlaySecond(std::vector<MatchedDateTimeInfo>& matches)
{
    std::vector<MatchedDateTimeInfo> matchList;
    for (MatchedDateTimeInfo& match : matches) {
        bool valid = true;
        auto matchIterator = matchList.begin();
        while (matchIterator != matchList.end()) {
            MatchedDateTimeInfo currentMatch =  (*matchIterator);
            // if one time within another time, the larger one is retained.
            if ((currentMatch.GetBegin() > match.GetBegin() && currentMatch.GetEnd() <= match.GetEnd()) ||
                (currentMatch.GetBegin() == match.GetBegin() && currentMatch.GetEnd() < match.GetEnd())) {
                matchIterator = matchList.erase(matchIterator);
                continue;
            } else if (currentMatch.GetBegin() <= match.GetBegin() && currentMatch.GetEnd() >= match.GetEnd()) {
                valid = false;
                matchIterator++;
                continue;
            } else {
                matchIterator++;
                continue;
            }
        }
        if (valid) {
            matchList.push_back(match);
        }
    }
    return matchList;
}

std::vector<MatchedDateTimeInfo> DateTimeFilter::FilterDatePeriod(icu::UnicodeString& content,
    std::vector<MatchedDateTimeInfo>& matches)
{
    std::vector<MatchedDateTimeInfo> matchList = matches;
    matchList = FilterDate(content, matchList);
    matchList = FilterDateTime(content, matchList);
    matchList = FilterPeriod(content, matchList);
    matchList = FilterDateTimePunc(content, matchList);
    return matchList;
}

std::vector<MatchedDateTimeInfo> DateTimeFilter::FilterDate(icu::UnicodeString& content,
    std::vector<MatchedDateTimeInfo>& matches)
{
    std::vector<MatchedDateTimeInfo> result;
    size_t lengthMatches = matches.size();
    unsigned int arrayIndexLessValue = 1;
    for (size_t i = 0; i < lengthMatches; i++) {
        MatchedDateTimeInfo match = matches[i];
        std::string matchRegex = match.GetRegex();
        int type = GetType(matchRegex);
        match.SetType(type);
        if (type != FilterType::TYPE_DATE && type != FilterType::TYPE_TODAY && type != FilterType::TYPE_WEEK) {
            result.push_back(match);
            continue;
        }
        size_t hasNum = matches.size() - arrayIndexLessValue - i;
        std::vector<MatchedDateTimeInfo> sub;
        if (hasNum > 1) {
            // 1 and 3 indicate the relative positions of the intercepted matches.
            sub.assign(matches.begin() + i + 1, matches.begin() + i + 3);
        } else if (hasNum == 1) {
            // 1 and 2 indicate the relative positions of the intercepted matches.
            sub.assign(matches.begin() + i + 1, matches.begin() + i + 2);
        }
        if (hasNum == 0 || sub.size() == 0) {
            match.SetType(FilterType::TYPE_DATE);
            result.push_back(match);
            continue;
        }
        int status = NestDealDate(content, match, sub, -1);
        match.SetType(FilterType::TYPE_DATE);
        if (status == DateCombine::NOT_COMBINE) {
            result.push_back(match);
            continue;
        } else if (status == DateCombine::TWO_COMBINE) {
            i++;
        } else {
            // 2 indicates that the subsequent two time points and the current time point can be combined.
            i = i + 2;
        }
        DealMatchE(content, matches[i], match);
        result.push_back(match);
    }
    return result;
}

// process time that can be merged
void DateTimeFilter::DealMatchE(icu::UnicodeString& content, MatchedDateTimeInfo& nextMatch,
    MatchedDateTimeInfo& match)
{
    int add = 0;
    int leftIndex = content.indexOf('(', match.GetEnd());
    if (leftIndex != -1 && leftIndex < nextMatch.GetBegin()) {
        int end = content.indexOf(')', nextMatch.GetEnd());
        if (end != -1) {
            icu::UnicodeString right = content.tempSubString(nextMatch.GetEnd(), end + 1 - nextMatch.GetEnd());
            if (right.trim() == ')') {
                add = end - nextMatch.GetEnd() + 1;
            }
        }
    }
    match.SetEnd(nextMatch.GetEnd() + add);
}

// process two times connected with keywords
std::vector<MatchedDateTimeInfo> DateTimeFilter::FilterDateTime(icu::UnicodeString& content,
    std::vector<MatchedDateTimeInfo>& matches)
{
    if (matches.size() == 0) {
        return matches;
    }
    UErrorCode status = U_ZERO_ERROR;
    icu::RegexPattern* pattern = dateTimeRule->GetPatternByKey("datetime");
    if (pattern == nullptr) {
        HILOG_ERROR_I18N("FilterDateTime failed because pattern is nullptr.");
        return matches;
    }
    std::string::size_type matchIndex = 1;
    std::string::size_type lastMatchIndex = 0;
    while (matchIndex < matches.size()) {
        bool isDelete = false;
        MatchedDateTimeInfo match =  matches[matchIndex];
        MatchedDateTimeInfo lastMatch = matches[lastMatchIndex];
        int lType = lastMatch.GetType();
        int cType = match.GetType();
        if ((lType == FilterType::TYPE_DATE && cType == FilterType::TYPE_TIME) ||
            (lType == FilterType::TYPE_DATE && cType == FilterType::TYPE_TIME_PERIOD && match.IsTimePeriod()) ||
            (lType == FilterType::TYPE_TIME && cType == FilterType::TYPE_DATE) ||
            (lType == FilterType::TYPE_TIME_PERIOD && lastMatch.IsTimePeriod() && cType == FilterType::TYPE_DATE)) {
            // truncate the substring between two times.
            icu::UnicodeString joiner = content.tempSubString(lastMatch.GetEnd(),
                match.GetBegin() - lastMatch.GetEnd());
            icu::RegexMatcher* matcher = pattern->matcher(joiner, status);
            if (matcher == nullptr) {
                HILOG_ERROR_I18N("FilterDateTime failed because pattern matcher failed.");
                return matches;
            }
            bool isJoiner = (joiner.trim().isEmpty()) ? true : matcher->matches(status);
            // the substring meets the rule, the two times are combined.
            if (isJoiner) {
                matches[lastMatchIndex].SetEnd(match.GetEnd());
                int lastType = ((lType == FilterType::TYPE_DATE && cType == FilterType::TYPE_TIME) ||
                (lType == FilterType::TYPE_TIME && cType == FilterType::TYPE_DATE)) ?
                FilterType::TYPE_DATETIME : FilterType::TYPE_TIME_PERIOD;
                matches[lastMatchIndex].SetType(lastType);
                matches.erase(matches.begin() + matchIndex);
                isDelete = true;
            } else {
                isDelete = DealBrackets(content, matches, matchIndex, matches[lastMatchIndex], match);
            }
            delete matcher;
        }
        if (!isDelete) {
            lastMatchIndex = matchIndex;
            matchIndex++;
        }
    }
    return matches;
}

// process the time period consisting of two times.
std::vector<MatchedDateTimeInfo> DateTimeFilter::FilterPeriod(icu::UnicodeString& content,
    std::vector<MatchedDateTimeInfo>& matches)
{
    if (matches.size() == 0) {
        return matches;
    }
    UErrorCode status = U_ZERO_ERROR;
    icu::RegexPattern* pattern = dateTimeRule->GetPatternByKey("period");
    if (pattern == nullptr) {
        HILOG_ERROR_I18N("FilterPeriod failed because pattern is nullptr.");
        return matches;
    }
    std::string::size_type matchIndex = 1;
    std::string::size_type currentMatchIndex = 0;
    while (matchIndex < matches.size()) {
        MatchedDateTimeInfo match =  matches[matchIndex];
        MatchedDateTimeInfo currentMatch =  matches[currentMatchIndex];
        int cType = currentMatch.GetType();
        int nType = match.GetType();
        if ((cType == nType && (cType == FilterType::TYPE_DATE || cType == FilterType::TYPE_TIME ||
            cType == FilterType::TYPE_DATETIME)) || (cType == FilterType::TYPE_DATETIME &&
            nType == FilterType::TYPE_TIME)) {
            icu::UnicodeString matchContent = content.tempSubString(currentMatch.GetEnd(),
                match.GetBegin() - currentMatch.GetEnd());
            icu::RegexMatcher* matcher = pattern->matcher(matchContent, status);
            if (matcher == nullptr) {
                HILOG_ERROR_I18N("FilterPeriod failed because pattern matcher failed.");
                return matches;
            }
            if (matcher->matches(status)) {
                matches[currentMatchIndex].SetEnd(match.GetEnd());
                matches[currentMatchIndex].SetType(FilterType::TYPE_TIME_PERIOD);
                bool isTimePeriod = (cType == FilterType::TYPE_TIME)? true : false;
                matches[currentMatchIndex].SetIsTimePeriod(isTimePeriod);
                matches.erase(matches.begin() + matchIndex);
            } else {
                currentMatchIndex = matchIndex;
                matchIndex++;
            }
            delete matcher;
        } else {
            currentMatchIndex = matchIndex;
            matchIndex++;
        }
    }
    return matches;
}

// process the time separated by punctuation
std::vector<MatchedDateTimeInfo> DateTimeFilter::FilterDateTimePunc(icu::UnicodeString& content,
    std::vector<MatchedDateTimeInfo>& matches)
{
    if (matches.size() == 0) {
        return matches;
    }
    std::string::size_type matchIndex = 1;
    std::string::size_type currentMatchIndex = 0;
    while (matchIndex < matches.size()) {
        MatchedDateTimeInfo match =  matches[matchIndex];
        MatchedDateTimeInfo currentMatch =  matches[currentMatchIndex];
        int cType = currentMatch.GetType();
        int lType = match.GetType();
        if ((cType == FilterType::TYPE_DATE && lType == FilterType::TYPE_TIME) ||
            (cType == FilterType::TYPE_DATE && lType == FilterType::TYPE_TIME_PERIOD && match.IsTimePeriod()) ||
            (cType == FilterType::TYPE_TIME && lType == FilterType::TYPE_DATE) ||
            (cType == FilterType::TYPE_TIME_PERIOD && currentMatch.IsTimePeriod() && lType == FilterType::TYPE_DATE)) {
            bool isPunc = false;
            icu::UnicodeString ss = content.tempSubString(currentMatch.GetEnd(),
                match.GetBegin() - currentMatch.GetEnd());
            if (ss.trim() == ',' || ss.trim() == "，") {
                isPunc = true;
            }
            if (isPunc) {
                matches[currentMatchIndex].SetEnd(match.GetEnd());
                int currentType = ((cType == FilterType::TYPE_DATE && lType == FilterType::TYPE_TIME) ||
                    cType == FilterType::TYPE_TIME) ? FilterType::TYPE_DATETIME : FilterType::TYPE_TIME_PERIOD;
                matches[currentMatchIndex].SetType(currentType);
                matches.erase(matches.begin() + matchIndex);
            } else {
                currentMatchIndex = matchIndex;
                matchIndex++;
            }
        } else {
            currentMatchIndex = matchIndex;
            matchIndex++;
        }
    }
    return matches;
}

bool DateTimeFilter::DealBrackets(icu::UnicodeString& content, std::vector<MatchedDateTimeInfo>& matches,
    int matchIndex, MatchedDateTimeInfo& lastMatch, MatchedDateTimeInfo& currentMatch)
{
    int lType = lastMatch.GetType();
    int cType = currentMatch.GetType();
    if (lType == FilterType::TYPE_TIME) {
        int add = 0;
        icu::UnicodeString endStr = content.tempSubString(lastMatch.GetEnd());
        UErrorCode status = U_ZERO_ERROR;
        icu::RegexPattern* pattern = dateTimeRule->GetPatternByKey("brackets");
        if (pattern == nullptr) {
            HILOG_ERROR_I18N("DealBrackets failed because pattern is nullptr.");
            return false;
        }
        icu::RegexMatcher* matcher = pattern->matcher(endStr, status);
        if (matcher == nullptr) {
            HILOG_ERROR_I18N("DealBrackets failed because pattern matcher failed.");
            return false;
        }
        icu::UnicodeString groupStr;
        if (matcher->find(status)) {
            groupStr = matcher->group(1, status);
            add = matcher->group(status).length();
        }
        delete matcher;
        if (!groupStr.isEmpty() && groupStr.trim() == content.tempSubString(currentMatch.GetBegin(),
            currentMatch.GetEnd() - currentMatch.GetBegin()).trim()) {
            lastMatch.SetEnd(lastMatch.GetEnd() + add);
            lastMatch.SetType(FilterType::TYPE_DATETIME);
            matches.erase(matches.begin() + matchIndex);
            return true;
        }
    } else if (lType == FilterType::TYPE_DATE && cType == FilterType::TYPE_TIME) {
        icu::UnicodeString beginStr = content.tempSubString(0, lastMatch.GetBegin());
        icu::UnicodeString endStr = content.tempSubString(lastMatch.GetEnd(),
            currentMatch.GetBegin() - lastMatch.GetEnd());
        if (beginStr.trim().endsWith('(') && endStr.trim() == ')') {
            int bracketLastIndex = beginStr.lastIndexOf('(');
            lastMatch.SetBegin(bracketLastIndex);
            lastMatch.SetEnd(currentMatch.GetEnd());
            lastMatch.SetType(FilterType::TYPE_DATETIME);
            matches.erase(matches.begin() + matchIndex);
            return true;
        }
    }
    return false;
}

int DateTimeFilter::NestDealDate(icu::UnicodeString& content, MatchedDateTimeInfo& current,
    std::vector<MatchedDateTimeInfo>& matches, int preType)
{
    int result = 0;
    MatchedDateTimeInfo nextMatch = matches[0];
    std::string nextRegex = nextMatch.GetRegex();
    int nextType = GetType(nextRegex);
    if (nextType != FilterType::TYPE_DATE && nextType != FilterType::TYPE_TODAY && nextType != FilterType::TYPE_WEEK) {
        return result;
    }
    std::string currentRegex = current.GetRegex();
    if (nextType == GetType(currentRegex) || nextType == preType) {
        return result;
    }
    result = GetResult(content, current, nextMatch, matches);
    return result;
}

int DateTimeFilter::GetResult(icu::UnicodeString& content, MatchedDateTimeInfo& current,
    MatchedDateTimeInfo& nextMatch, std::vector<MatchedDateTimeInfo>& matches)
{
    int result = 0;
    icu::UnicodeString ss = content.tempSubString(current.GetEnd(), nextMatch.GetBegin() - current.GetEnd());
    if (this->dateTimeRule == nullptr) {
        HILOG_ERROR_I18N("GetResult failed because this->dateTimeRule is nullptr.");
        return result;
    }
    if (this->dateTimeRule->IsRelDates(ss, this->locale) || ss.trim() == '(') {
        bool isThree = false;
        if (matches.size() > 1) {
            std::vector<MatchedDateTimeInfo> nextMatches(matches.begin() + 1, matches.end());
            int connect = NestDealDate(content, nextMatch, nextMatches, current.GetType());
            if (connect == DateCombine::TWO_COMBINE) {
                isThree = true;
            }
        }
        bool isBrackets = false;
        if (ss.trim() == '(') {
            icu::UnicodeString endStr = content.tempSubString(current.GetEnd());
            UErrorCode status = U_ZERO_ERROR;
            icu::RegexPattern* pattern = dateTimeRule->GetPatternByKey("brackets");
            if (pattern == nullptr) {
                HILOG_ERROR_I18N("GetResult failed because pattern is nullptr.");
                return result;
            }
            icu::RegexMatcher* matcher = pattern->matcher(endStr, status);
            if (matcher == nullptr) {
                HILOG_ERROR_I18N("GetResult failed because pattern matcher failed.");
                return result;
            }
            icu::UnicodeString str = matcher->find(status) ? matcher->group(1, status) : "";
            int end = isThree ? matches[1].GetEnd() : nextMatch.GetEnd();
            if (!str.isEmpty() && str.trim() == content.tempSubString(nextMatch.GetBegin(),
                end - nextMatch.GetBegin()).trim()) {
                isBrackets = true;
            }
            delete matcher;
        }
        if (this->dateTimeRule->IsRelDates(ss, this->locale) || isBrackets) {
            result = isThree ? DateCombine::ALL_COMBINE : DateCombine::TWO_COMBINE;
        }
    }
    return result;
}
} // namespace I18n
} // namespace Global
} // namespace OHOS