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
 * See the License for the specific locale governing permissions and
 * limitations under the License.
 */
#include "date_rule_init.h"
#include "i18n_hilog.h"

namespace OHOS {
namespace Global {
namespace I18n {
DateRuleInit::DateRuleInit(std::string& locale)
{
    dateTimeRule = new DateTimeRule(locale);
    if (dateTimeRule == nullptr) {
        HILOG_ERROR_I18N("DateTimeRule construct failed.");
        return;
    }
    this->locale = dateTimeRule->GetLocale();
    filter = new DateTimeFilter(this->locale, dateTimeRule);
    if (filter == nullptr) {
        HILOG_ERROR_I18N("DateTimeFilter construct failed.");
    }
    Init();
}

DateRuleInit::~DateRuleInit()
{
    if (dateTimeRule != nullptr) {
        delete dateTimeRule;
    }
    if (filter != nullptr) {
        delete filter;
    }
}

std::vector<MatchedDateTimeInfo> DateRuleInit::Detect(icu::UnicodeString& message)
{
    std::vector<MatchedDateTimeInfo> matches = GetMatches(message);
    std::vector<MatchedDateTimeInfo> clearMatches = ClearFind(message);
    std::vector<MatchedDateTimeInfo> pastMatches = PastFind(message);
    if (filter != nullptr) {
        // Filter results that don't meet rules
        matches = filter->Filter(message, matches, clearMatches, pastMatches);
    }
    return matches;
}

// Obtains result that meet the clear rules
std::vector<MatchedDateTimeInfo> DateRuleInit::ClearFind(icu::UnicodeString& message)
{
    return this->clearRulesEngine.Match(message);
}

// Obtains result that meet the past rules
std::vector<MatchedDateTimeInfo> DateRuleInit::PastFind(icu::UnicodeString& message)
{
    return this->pastRulesEngine.Match(message);
}

void DateRuleInit::Init()
{
    // create rulesEngine based on the UniverseRules.
    RulesSet rulesSet1(dateTimeRule->GetUniverseRules(), dateTimeRule->GetSubRules(), dateTimeRule->GetParam(),
        dateTimeRule->GetParamBackup());
    RulesEngine rulesEngine1(dateTimeRule, rulesSet1);
    universalAndLocaleRules.push_back(rulesEngine1);

    std::unordered_map<std::string, std::string> nullMap;
    // create rulesEngine based on the LocalesRules.
    if (dateTimeRule->GetLocalesRules().size() != 0) {
        RulesSet rulesSet2(dateTimeRule->GetLocalesRules(), dateTimeRule->GetSubRules(), dateTimeRule->GetParam(),
            nullMap);
        RulesEngine rulesEngine2(dateTimeRule, rulesSet2);
        universalAndLocaleRules.push_back(rulesEngine2);
    }

    // create rulesEngine based on the LocalesRulesBackup.
    if (dateTimeRule->GetLocalesRulesBackup().size() != 0) {
        RulesSet rulesSet3(dateTimeRule->GetLocalesRulesBackup(), dateTimeRule->GetSubRules(),
            dateTimeRule->GetParamBackup(), nullMap);
        RulesEngine rulesEngine3(dateTimeRule, rulesSet3);
        universalAndLocaleRules.push_back(rulesEngine3);
    }

    // create rulesEngine based on the SubRules.
    for (auto& kv : dateTimeRule->GetSubRulesMap()) {
        RulesSet rulesSet4(kv.second, dateTimeRule->GetSubRules(), dateTimeRule->GetParam(),
            dateTimeRule->GetParamBackup());
        RulesEngine rulesEngine4(dateTimeRule, rulesSet4);
        subDetectsMap[kv.first] = rulesEngine4;
    }

    // create rulesEngine based on the FilterRules.
    RulesSet rulesSet5(dateTimeRule->GetFilterRules(), dateTimeRule->GetSubRules(), dateTimeRule->GetParam(),
        dateTimeRule->GetParamBackup());
    RulesEngine rulesEngine5(dateTimeRule, rulesSet5);
    this->clearRulesEngine = rulesEngine5;

    // create rulesEngine based on the PastRules.
    RulesSet rulesSet6(dateTimeRule->GetPastRules(), nullMap, dateTimeRule->GetParam(),
        dateTimeRule->GetParamBackup());
    RulesEngine rulesEngine6(dateTimeRule, rulesSet6);
    this->pastRulesEngine = rulesEngine6;
}

// obtains the date and time information in the message.
std::vector<MatchedDateTimeInfo> DateRuleInit::GetMatches(icu::UnicodeString& message)
{
    std::vector<MatchedDateTimeInfo> matches;
    for (auto& detect : universalAndLocaleRules) {
        std::vector<MatchedDateTimeInfo> tempMatches = detect.Match(message);
        for (MatchedDateTimeInfo& match : tempMatches) {
            GetMatchedInfo(matches, match, message);
        }
    }
    return matches;
}

void DateRuleInit::GetMatchedInfo(std::vector<MatchedDateTimeInfo>& matches, MatchedDateTimeInfo& match,
    icu::UnicodeString& message)
{
    // splitting results based on subRules.
    if (subDetectsMap.find(match.GetRegex()) != subDetectsMap.end()) {
        RulesEngine subRulesEngine = subDetectsMap[match.GetRegex()];
        icu::UnicodeString subMessage = message.tempSubString(match.GetBegin(), match.GetEnd() - match.GetBegin());
        std::vector<MatchedDateTimeInfo> subMatches = subRulesEngine.Match(subMessage);
        for (auto& subMatch : subMatches) {
            subMatch.SetBegin(subMatch.GetBegin() + match.GetBegin());
            subMatch.SetEnd(subMatch.GetEnd() + match.GetBegin());
        }
        matches.insert(matches.end(), subMatches.begin(), subMatches.end());
    } else {
        matches.push_back(match);
    }
}
} // namespace I18n
} // namespace Global
} // namespace OHOS