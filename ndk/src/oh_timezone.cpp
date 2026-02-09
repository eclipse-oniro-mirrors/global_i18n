/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "oh_timezone.h"

#include <cstdlib>
#include "securec.h"
#include "unicode/basictz.h"
#include "unicode/timezone.h"
#include "i18n_hilog.h"
#include "utils.h"

static constexpr int32_t MAX_NUM_TRANSITION_RULE = 20;

I18n_ErrorCode WriteString(char** buffer, const std::string& content)
{
    size_t length = (content.length() + 1) * sizeof(char);
    *buffer = static_cast<char*>(malloc(length));
    if (*buffer == nullptr) {
        HILOG_ERROR_I18N("WriteString: malloc failed.");
        return I18n_ErrorCode::UNEXPECTED_ERROR;
    }
    if (memcpy_s(*buffer, length, content.c_str(), length) != EOK) {
        free(*buffer);
        *buffer = nullptr;
        HILOG_ERROR_I18N("WriteString: memcpy_s failed.");
        return I18n_ErrorCode::UNEXPECTED_ERROR;
    }
    return I18n_ErrorCode::SUCCESS;
}

bool IsValidTimeZoneID(const std::string& timeZoneID)
{
    if (timeZoneID.empty()) {
        HILOG_ERROR_I18N("IsValidTimeZoneID: timeZoneID is empty.");
        return false;
    }
    std::set<std::string> availableIDs = OHOS::Global::I18n::GetTimeZoneAvailableIDs();
    if (availableIDs.find(timeZoneID) == availableIDs.end()) {
        HILOG_ERROR_I18N("IsValidTimeZoneID: Invalid timeZoneID %{public}s.", timeZoneID.c_str());
        return false;
    }
    return true;
}

void InitTimeZoneRules(TimeZoneRules* rules)
{
    rules->timeArrayRules = nullptr;
    rules->annualRules = nullptr;
    rules->numTimeArrayRules = 0;
    rules->numAnnualRules = 0;
}

void FreeTimeZoneRules(TimeZoneRules* rules)
{
    if (rules == nullptr) {
        return;
    }
    if (rules->timeArrayRules != nullptr) {
        for (size_t i = 0; i < rules->numTimeArrayRules; i++) {
            if (rules->timeArrayRules[i].name != nullptr) {
                free(rules->timeArrayRules[i].name);
                rules->timeArrayRules[i].name = nullptr;
            }
            if (rules->timeArrayRules[i].startTimes != nullptr) {
                free(rules->timeArrayRules[i].startTimes);
                rules->timeArrayRules[i].startTimes = nullptr;
                rules->timeArrayRules[i].numStartTimes = 0;
            }
        }
        free(rules->timeArrayRules);
        rules->timeArrayRules = nullptr;
        rules->numTimeArrayRules = 0;
    }
    if (rules->annualRules != nullptr) {
        for (size_t i = 0; i < rules->numAnnualRules; i++) {
            if (rules->annualRules[i].name != nullptr) {
                free(rules->annualRules[i].name);
                rules->annualRules[i].name = nullptr;
            }
        }
        free(rules->annualRules);
        rules->annualRules = nullptr;
        rules->numAnnualRules = 0;
    }
}

I18n_ErrorCode ParseInitialRule(const icu::InitialTimeZoneRule* initial, TimeZoneRules* rules)
{
    if (initial == nullptr) {
        HILOG_ERROR_I18N("ParseInitialRule: initial is nullptr.");
        return I18n_ErrorCode::UNEXPECTED_ERROR;
    }
    rules->initial.rawOffset = initial->getRawOffset();
    rules->initial.dstSavings = initial->getDSTSavings();
    return I18n_ErrorCode::SUCCESS;
}

void GetAnnualAndTimeArrayRules(const icu::TimeZoneRule** transitionRule, int32_t numTransitionRule,
    std::vector<const icu::AnnualTimeZoneRule*>& annualRulesArray,
    std::vector<const icu::TimeArrayTimeZoneRule*>& timeArrayRulesArray)
{
    for (int32_t i = 0; i < numTransitionRule; i++) {
        const icu::TimeZoneRule* rule = transitionRule[i];
        const icu::AnnualTimeZoneRule* annualTimeZoneRule = dynamic_cast<const icu::AnnualTimeZoneRule*>(rule);
        if (annualTimeZoneRule != nullptr) {
            annualRulesArray.emplace_back(annualTimeZoneRule);
            continue;
        }
        const icu::TimeArrayTimeZoneRule* timeArrayTimeZoneRule =
            dynamic_cast<const icu::TimeArrayTimeZoneRule*>(rule);
        if (timeArrayTimeZoneRule != nullptr) {
            timeArrayRulesArray.emplace_back(timeArrayTimeZoneRule);
        }
    }
}

I18n_ErrorCode ParseAnnualRules(const std::vector<const icu::AnnualTimeZoneRule*>& annualTimeZoneRulesArray,
    TimeZoneRules* rules)
{
    rules->numAnnualRules = annualTimeZoneRulesArray.size();
    if (rules->numAnnualRules <= 0) {
        return I18n_ErrorCode::SUCCESS;
    }
    size_t sizeOfAnnualRules = sizeof(AnnualTimeZoneRule) * rules->numAnnualRules;
    rules->annualRules = static_cast<AnnualTimeZoneRule*>(malloc(sizeOfAnnualRules));
    if (rules->annualRules == nullptr) {
        HILOG_ERROR_I18N("ParseAnnualRules: malloc failed.");
        return I18n_ErrorCode::UNEXPECTED_ERROR;
    }
    if (memset_s(rules->annualRules, sizeOfAnnualRules, 0, sizeOfAnnualRules) != EOK) {
        free(rules->annualRules);
        rules->annualRules = nullptr;
        HILOG_ERROR_I18N("ParseAnnualRules: memset_s failed.");
        return I18n_ErrorCode::UNEXPECTED_ERROR;
    }
    for (size_t i = 0; i < rules->numAnnualRules; ++i) {
        const icu::AnnualTimeZoneRule* annualTimeZoneRule = annualTimeZoneRulesArray[i];
        icu::UnicodeString ruleName;
        annualTimeZoneRule->getName(ruleName);
        std::string name;
        ruleName.toUTF8String(name);
        I18n_ErrorCode errCode = WriteString(&(rules->annualRules[i].name), name);
        if (errCode != I18n_ErrorCode::SUCCESS) {
            HILOG_ERROR_I18N("ParseAnnualRules: Write string failed.");
            return errCode;
        }
        rules->annualRules[i].startYear = annualTimeZoneRule->getStartYear();
        rules->annualRules[i].endYear = annualTimeZoneRule->getEndYear();
        rules->annualRules[i].rawOffset = annualTimeZoneRule->getRawOffset();
        rules->annualRules[i].dstSavings = annualTimeZoneRule->getDSTSavings();
        const icu::DateTimeRule* dateTimeRule = annualTimeZoneRule->getRule();
        if (dateTimeRule == nullptr) {
            HILOG_ERROR_I18N("ParseAnnualRules:Get rule failed.");
            return I18n_ErrorCode::UNEXPECTED_ERROR;
        }
        rules->annualRules[i].dateTimeRule.month = dateTimeRule->getRuleMonth();
        rules->annualRules[i].dateTimeRule.dayOfMonth = dateTimeRule->getRuleDayOfMonth();
        rules->annualRules[i].dateTimeRule.dayOfWeek = dateTimeRule->getRuleDayOfWeek();
        rules->annualRules[i].dateTimeRule.weekInMonth = dateTimeRule->getRuleWeekInMonth();
        rules->annualRules[i].dateTimeRule.millisInDay = dateTimeRule->getRuleMillisInDay();
        rules->annualRules[i].dateTimeRule.dateRuleType = static_cast<DateRuleType>(dateTimeRule->getDateRuleType());
        rules->annualRules[i].dateTimeRule.timeRuleType = static_cast<TimeRuleType>(dateTimeRule->getTimeRuleType());
    }
    return I18n_ErrorCode::SUCCESS;
}

I18n_ErrorCode ParseTimeArrayRules(const std::vector<const icu::TimeArrayTimeZoneRule*>& timeArrayTimeZoneRulesArray,
    TimeZoneRules* rules)
{
    rules->numTimeArrayRules = timeArrayTimeZoneRulesArray.size();
    if (rules->numTimeArrayRules <= 0) {
        return I18n_ErrorCode::SUCCESS;
    }
    size_t sizeOfTimeArrayRules = sizeof(TimeArrayTimeZoneRule) * rules->numTimeArrayRules;
    rules->timeArrayRules = static_cast<TimeArrayTimeZoneRule*>(malloc(sizeOfTimeArrayRules));
    if (rules->timeArrayRules == nullptr) {
        HILOG_ERROR_I18N("ParseTimeArrayRules: timeArrayRules malloc failed.");
        return I18n_ErrorCode::UNEXPECTED_ERROR;
    }
    if (memset_s(rules->timeArrayRules, sizeOfTimeArrayRules, 0, sizeOfTimeArrayRules) != EOK) {
        free(rules->timeArrayRules);
        rules->timeArrayRules = nullptr;
        HILOG_ERROR_I18N("ParseTimeArrayRules: memset_s failed.");
        return I18n_ErrorCode::UNEXPECTED_ERROR;
    }
    for (size_t i = 0; i < rules->numTimeArrayRules; ++i) {
        const icu::TimeArrayTimeZoneRule* timeArrayTimeZoneRule = timeArrayTimeZoneRulesArray[i];
        icu::UnicodeString ruleName;
        timeArrayTimeZoneRule->getName(ruleName);
        std::string name;
        ruleName.toUTF8String(name);
        I18n_ErrorCode errCode = WriteString(&(rules->timeArrayRules[i].name), name);
        if (errCode != I18n_ErrorCode::SUCCESS) {
            HILOG_ERROR_I18N("ParseTimeArrayRules: Write string failed.");
            return errCode;
        }
        rules->timeArrayRules[i].rawOffset = timeArrayTimeZoneRule->getRawOffset();
        rules->timeArrayRules[i].dstSavings = timeArrayTimeZoneRule->getDSTSavings();
        rules->timeArrayRules[i].numStartTimes = timeArrayTimeZoneRule->countStartTimes();
        rules->timeArrayRules[i].startTimes =
            static_cast<double*>(malloc(sizeof(double) * rules->timeArrayRules[i].numStartTimes));
        if (rules->timeArrayRules[i].startTimes == nullptr) {
            HILOG_ERROR_I18N("ParseTimeArrayRules: startTimes malloc failed.");
            return I18n_ErrorCode::UNEXPECTED_ERROR;
        }
        for (int32_t index = 0; index < rules->timeArrayRules[i].numStartTimes; ++index) {
            timeArrayTimeZoneRule->getStartTimeAt(index, rules->timeArrayRules[i].startTimes[index]);
        }
        rules->timeArrayRules[i].timeRuleType = static_cast<TimeRuleType>(timeArrayTimeZoneRule->getTimeType());
    }
    return I18n_ErrorCode::SUCCESS;
}

I18n_ErrorCode ParseTimeZoneRule(const std::string& timeZoneID, TimeZoneRules* rules)
{
    if (!IsValidTimeZoneID(timeZoneID) || rules == nullptr) {
        HILOG_ERROR_I18N("ParseTimeZoneRule: Invalid parameter.");
        return I18n_ErrorCode::ERROR_INVALID_PARAMETER;
    }
    InitTimeZoneRules(rules);
    icu::TimeZone* icuTimeZone = icu::TimeZone::createTimeZone(timeZoneID.c_str());
    icu::BasicTimeZone* icuBasicTimeZone = dynamic_cast<icu::BasicTimeZone*>(icuTimeZone);
    if (icuBasicTimeZone == nullptr) {
        delete icuTimeZone;
        HILOG_ERROR_I18N("ParseTimeZoneRule: Get icuBasicTimeZone failed.");
        return I18n_ErrorCode::UNEXPECTED_ERROR;
    }
    const icu::InitialTimeZoneRule* initial = nullptr;
    const icu::TimeZoneRule* transitionRule[MAX_NUM_TRANSITION_RULE] = { nullptr };
    int32_t numTransitionRule = MAX_NUM_TRANSITION_RULE;
    UErrorCode status = U_ZERO_ERROR;
    icuBasicTimeZone->getTimeZoneRules(initial, transitionRule, numTransitionRule, status);
    if (U_FAILURE(status)) {
        delete icuTimeZone;
        HILOG_ERROR_I18N("ParseTimeZoneRule: Get time zone rules failed.");
        return I18n_ErrorCode::UNEXPECTED_ERROR;
    }
    I18n_ErrorCode errCode = ParseInitialRule(initial, rules);
    if (errCode != I18n_ErrorCode::SUCCESS) {
        delete icuTimeZone;
        FreeTimeZoneRules(rules);
        HILOG_ERROR_I18N("ParseTimeZoneRule: Parse initial rule failed.");
        return errCode;
    }
    std::vector<const icu::AnnualTimeZoneRule*> annualRulesArray;
    std::vector<const icu::TimeArrayTimeZoneRule*> timeArrayRulesArray;
    GetAnnualAndTimeArrayRules(transitionRule, numTransitionRule, annualRulesArray, timeArrayRulesArray);
    errCode = ParseAnnualRules(annualRulesArray, rules);
    if (errCode != I18n_ErrorCode::SUCCESS) {
        delete icuTimeZone;
        FreeTimeZoneRules(rules);
        HILOG_ERROR_I18N("ParseTimeZoneRule: Parse annual rules failed.");
        return errCode;
    }
    errCode = ParseTimeArrayRules(timeArrayRulesArray, rules);
    if (errCode != I18n_ErrorCode::SUCCESS) {
        delete icuTimeZone;
        FreeTimeZoneRules(rules);
        HILOG_ERROR_I18N("ParseTimeZoneRule: Parse time array rules failed.");
        return errCode;
    }
    delete icuTimeZone;
    return I18n_ErrorCode::SUCCESS;
}

icu::TimeArrayTimeZoneRule* CreateIcuTimeArrayRule(TimeArrayTimeZoneRule* rule)
{
    icu::TimeArrayTimeZoneRule* icuRule = new icu::TimeArrayTimeZoneRule(rule->name, rule->rawOffset, rule->dstSavings,
        rule->startTimes, rule->numStartTimes, static_cast<icu::DateTimeRule::TimeRuleType>(rule->timeRuleType));
    return icuRule;
}

icu::AnnualTimeZoneRule* CreateIcuAnnualRule(AnnualTimeZoneRule* rule)
{
    icu::DateTimeRule* icuDateTimeRule;
    if (rule->dateTimeRule.dateRuleType == DateRuleType::DOM) {
        icuDateTimeRule = new icu::DateTimeRule(rule->dateTimeRule.month, rule->dateTimeRule.dayOfMonth,
            rule->dateTimeRule.millisInDay,
            static_cast<icu::DateTimeRule::TimeRuleType>(rule->dateTimeRule.timeRuleType));
    } else if (rule->dateTimeRule.dateRuleType == DateRuleType::DOW) {
        icuDateTimeRule = new icu::DateTimeRule(rule->dateTimeRule.month, rule->dateTimeRule.weekInMonth,
            rule->dateTimeRule.dayOfWeek, rule->dateTimeRule.millisInDay,
            static_cast<icu::DateTimeRule::TimeRuleType>(rule->dateTimeRule.timeRuleType));
    } else {
        icuDateTimeRule = new icu::DateTimeRule(rule->dateTimeRule.month, rule->dateTimeRule.dayOfMonth,
            rule->dateTimeRule.dayOfWeek, rule->dateTimeRule.dateRuleType == DateRuleType::DOW_GEQ_DOM,
            rule->dateTimeRule.millisInDay,
            static_cast<icu::DateTimeRule::TimeRuleType>(rule->dateTimeRule.timeRuleType));
    }
    icu::AnnualTimeZoneRule* icuRule = new icu::AnnualTimeZoneRule(rule->name, rule->rawOffset, rule->dstSavings,
        icuDateTimeRule, rule->startYear, rule->endYear);
    return icuRule;
}

I18n_ErrorCode GetFirstStartFromTimeArrayTimeZoneRule(TimeArrayTimeZoneRule* rule, TimeZoneRuleQuery* query)
{
    if (rule == nullptr || query == nullptr) {
        HILOG_ERROR_I18N("GetFirstStartFromTimeArrayTimeZoneRule: rule or query is nullptr.");
        return I18n_ErrorCode::ERROR_INVALID_PARAMETER;
    }
    icu::TimeArrayTimeZoneRule* icuRule = CreateIcuTimeArrayRule(rule);
    if (icuRule == nullptr) {
        HILOG_ERROR_I18N("GetFirstStartFromTimeArrayTimeZoneRule: icuRule is nullptr.");
        return I18n_ErrorCode::UNEXPECTED_ERROR;
    }
    if (!icuRule->getFirstStart(query->prevRawOffset, query->prevDSTSavings, query->result)) {
        delete icuRule;
        HILOG_ERROR_I18N("GetFirstStartFromTimeArrayTimeZoneRule: Get start failed.");
        return I18n_ErrorCode::ERROR_INVALID_PARAMETER;
    }
    delete icuRule;
    return I18n_ErrorCode::SUCCESS;
}

I18n_ErrorCode GetFirstStartFromAnnualTimeZoneRule(AnnualTimeZoneRule* rule, TimeZoneRuleQuery* query)
{
    if (rule == nullptr || query == nullptr) {
        HILOG_ERROR_I18N("GetFirstStartFromAnnualTimeZoneRule: rule or query is nullptr.");
        return I18n_ErrorCode::ERROR_INVALID_PARAMETER;
    }
    icu::AnnualTimeZoneRule* icuRule = CreateIcuAnnualRule(rule);
    if (!icuRule->getFirstStart(query->prevRawOffset, query->prevDSTSavings, query->result)) {
        delete icuRule;
        HILOG_ERROR_I18N("GetFirstStartFromAnnualTimeZoneRule: Get start failed.");
        return I18n_ErrorCode::ERROR_INVALID_PARAMETER;
    }
    delete icuRule;
    return I18n_ErrorCode::SUCCESS;
}

I18n_ErrorCode GetFinalStartFromTimeArrayTimeZoneRule(TimeArrayTimeZoneRule* rule, TimeZoneRuleQuery* query)
{
    if (rule == nullptr || query == nullptr) {
        HILOG_ERROR_I18N("GetFinalStartFromTimeArrayTimeZoneRule: rule or query is nullptr.");
        return I18n_ErrorCode::ERROR_INVALID_PARAMETER;
    }
    icu::TimeArrayTimeZoneRule* icuRule = CreateIcuTimeArrayRule(rule);
    if (icuRule == nullptr) {
        HILOG_ERROR_I18N("GetFinalStartFromTimeArrayTimeZoneRule: icuRule is nullptr.");
        return I18n_ErrorCode::UNEXPECTED_ERROR;
    }
    if (!icuRule->getFinalStart(query->prevRawOffset, query->prevDSTSavings, query->result)) {
        delete icuRule;
        HILOG_ERROR_I18N("GetFinalStartFromTimeArrayTimeZoneRule: Get start failed.");
        return I18n_ErrorCode::ERROR_INVALID_PARAMETER;
    }
    delete icuRule;
    return I18n_ErrorCode::SUCCESS;
}

I18n_ErrorCode GetFinalStartFromAnnualTimeZoneRule(AnnualTimeZoneRule* rule, TimeZoneRuleQuery* query)
{
    if (rule == nullptr || query == nullptr) {
        HILOG_ERROR_I18N("GetFinalStartFromAnnualTimeZoneRule: rule or query is nullptr.");
        return I18n_ErrorCode::ERROR_INVALID_PARAMETER;
    }
    icu::AnnualTimeZoneRule* icuRule = CreateIcuAnnualRule(rule);
    if (!icuRule->getFinalStart(query->prevRawOffset, query->prevDSTSavings, query->result)) {
        delete icuRule;
        HILOG_ERROR_I18N("GetFinalStartFromAnnualTimeZoneRule: Get start failed.");
        return I18n_ErrorCode::ERROR_INVALID_PARAMETER;
    }
    delete icuRule;
    return I18n_ErrorCode::SUCCESS;
}

I18n_ErrorCode GetNextStartFromTimeArrayTimeZoneRule(TimeArrayTimeZoneRule* rule, TimeZoneRuleQuery* query)
{
    if (rule == nullptr || query == nullptr) {
        HILOG_ERROR_I18N("GetNextStartFromTimeArrayTimeZoneRule: rule or query is nullptr.");
        return I18n_ErrorCode::ERROR_INVALID_PARAMETER;
    }
    icu::TimeArrayTimeZoneRule* icuRule = CreateIcuTimeArrayRule(rule);
    if (icuRule == nullptr) {
        HILOG_ERROR_I18N("GetNextStartFromTimeArrayTimeZoneRule: icuRule is nullptr.");
        return I18n_ErrorCode::UNEXPECTED_ERROR;
    }
    if (!icuRule->getNextStart(query->base, query->prevRawOffset, query->prevDSTSavings, query->inclusive,
        query->result)) {
        delete icuRule;
        HILOG_ERROR_I18N("GetNextStartFromTimeArrayTimeZoneRule: Get start failed.");
        return I18n_ErrorCode::ERROR_INVALID_PARAMETER;
    }
    delete icuRule;
    return I18n_ErrorCode::SUCCESS;
}

I18n_ErrorCode GetNextStartFromAnnualTimeZoneRule(AnnualTimeZoneRule* rule, TimeZoneRuleQuery* query)
{
    if (rule == nullptr || query == nullptr) {
        HILOG_ERROR_I18N("GetNextStartFromAnnualTimeZoneRule: rule or query is nullptr.");
        return I18n_ErrorCode::ERROR_INVALID_PARAMETER;
    }
    icu::AnnualTimeZoneRule* icuRule = CreateIcuAnnualRule(rule);
    if (!icuRule->getNextStart(query->base, query->prevRawOffset, query->prevDSTSavings, query->inclusive,
        query->result)) {
        delete icuRule;
        HILOG_ERROR_I18N("GetNextStartFromAnnualTimeZoneRule: Get start failed.");
        return I18n_ErrorCode::ERROR_INVALID_PARAMETER;
    }
    delete icuRule;
    return I18n_ErrorCode::SUCCESS;
}

I18n_ErrorCode GetPrevStartFromTimeArrayTimeZoneRule(TimeArrayTimeZoneRule* rule, TimeZoneRuleQuery* query)
{
    if (rule == nullptr || query == nullptr) {
        HILOG_ERROR_I18N("GetPrevStartFromTimeArrayTimeZoneRule: rule or query is nullptr.");
        return I18n_ErrorCode::ERROR_INVALID_PARAMETER;
    }
    icu::TimeArrayTimeZoneRule* icuRule = CreateIcuTimeArrayRule(rule);
    if (icuRule == nullptr) {
        HILOG_ERROR_I18N("GetPrevStartFromTimeArrayTimeZoneRule: icuRule is nullptr.");
        return I18n_ErrorCode::UNEXPECTED_ERROR;
    }
    if (!icuRule->getPreviousStart(query->base, query->prevRawOffset, query->prevDSTSavings, query->inclusive,
        query->result)) {
        delete icuRule;
        HILOG_ERROR_I18N("GetPrevStartFromTimeArrayTimeZoneRule: Get start failed.");
        return I18n_ErrorCode::ERROR_INVALID_PARAMETER;
    }
    delete icuRule;
    return I18n_ErrorCode::SUCCESS;
}

I18n_ErrorCode GetPrevStartFromAnnualTimeZoneRule(AnnualTimeZoneRule* rule, TimeZoneRuleQuery* query)
{
    if (rule == nullptr || query == nullptr) {
        HILOG_ERROR_I18N("GetPrevStartFromAnnualTimeZoneRule: rule or query is nullptr.");
        return I18n_ErrorCode::ERROR_INVALID_PARAMETER;
    }
    icu::AnnualTimeZoneRule* icuRule = CreateIcuAnnualRule(rule);
    if (!icuRule->getPreviousStart(query->base, query->prevRawOffset, query->prevDSTSavings, query->inclusive,
        query->result)) {
        delete icuRule;
        HILOG_ERROR_I18N("GetPrevStartFromAnnualTimeZoneRule: Get start failed.");
        return I18n_ErrorCode::ERROR_INVALID_PARAMETER;
    }
    delete icuRule;
    return I18n_ErrorCode::SUCCESS;
}

I18n_ErrorCode GetStartTimeAt(TimeArrayTimeZoneRule* rule, int32_t index, double* result)
{
    if (rule == nullptr) {
        HILOG_ERROR_I18N("GetStartTimeAt: rule or query is nullptr.");
        return I18n_ErrorCode::ERROR_INVALID_PARAMETER;
    }
    icu::TimeArrayTimeZoneRule* icuRule = CreateIcuTimeArrayRule(rule);
    if (icuRule == nullptr) {
        HILOG_ERROR_I18N("GetStartTimeAt: icuRule is nullptr.");
        return I18n_ErrorCode::UNEXPECTED_ERROR;
    }
    double startTime = 0;
    if (!icuRule->getStartTimeAt(index, startTime)) {
        delete icuRule;
        HILOG_ERROR_I18N("GetStartTimeAt: Get start failed.");
        return I18n_ErrorCode::ERROR_INVALID_PARAMETER;
    }
    *result = startTime;
    delete icuRule;
    return I18n_ErrorCode::SUCCESS;
}

I18n_ErrorCode GetStartInYear(AnnualTimeZoneRule* rule, int32_t year, TimeZoneRuleQuery* query)
{
    if (rule == nullptr || query == nullptr) {
        HILOG_ERROR_I18N("GetStartInYear: rule or query is nullptr.");
        return I18n_ErrorCode::ERROR_INVALID_PARAMETER;
    }
    icu::AnnualTimeZoneRule* icuRule = CreateIcuAnnualRule(rule);
    if (!icuRule->getStartInYear(year, query->prevRawOffset, query->prevDSTSavings, query->result)) {
        delete icuRule;
        HILOG_ERROR_I18N("GetStartInYear: Get start failed.");
        return I18n_ErrorCode::ERROR_INVALID_PARAMETER;
    }
    delete icuRule;
    return I18n_ErrorCode::SUCCESS;
}

#ifdef __cplusplus
extern "C" {
#endif

I18n_ErrorCode OH_i18n_GetTimeZoneRules(const char* timeZoneID, TimeZoneRules* rules)
{
    return ParseTimeZoneRule(timeZoneID, rules);
}

I18n_ErrorCode OH_i18n_GetFirstStartFromTimeArrayTimeZoneRule(TimeArrayTimeZoneRule* rule, TimeZoneRuleQuery* query)
{
    return GetFirstStartFromTimeArrayTimeZoneRule(rule, query);
}

I18n_ErrorCode OH_i18n_GetFirstStartFromAnnualTimeZoneRule(AnnualTimeZoneRule* rule, TimeZoneRuleQuery* query)
{
    return GetFirstStartFromAnnualTimeZoneRule(rule, query);
}

I18n_ErrorCode OH_i18n_GetFinalStartFromTimeArrayTimeZoneRule(TimeArrayTimeZoneRule* rule, TimeZoneRuleQuery* query)
{
    return GetFinalStartFromTimeArrayTimeZoneRule(rule, query);
}

I18n_ErrorCode OH_i18n_GetFinalStartFromAnnualTimeZoneRule(AnnualTimeZoneRule* rule, TimeZoneRuleQuery* query)
{
    return GetFinalStartFromAnnualTimeZoneRule(rule, query);
}

I18n_ErrorCode OH_i18n_GetNextStartFromTimeArrayTimeZoneRule(TimeArrayTimeZoneRule* rule, TimeZoneRuleQuery* query)
{
    return GetNextStartFromTimeArrayTimeZoneRule(rule, query);
}

I18n_ErrorCode OH_i18n_GetNextStartFromAnnualTimeZoneRule(AnnualTimeZoneRule* rule, TimeZoneRuleQuery* query)
{
    return GetNextStartFromAnnualTimeZoneRule(rule, query);
}

I18n_ErrorCode OH_i18n_GetPrevStartFromTimeArrayTimeZoneRule(TimeArrayTimeZoneRule* rule, TimeZoneRuleQuery* query)
{
    return GetPrevStartFromTimeArrayTimeZoneRule(rule, query);
}

I18n_ErrorCode OH_i18n_GetPrevStartFromAnnualTimeZoneRule(AnnualTimeZoneRule* rule, TimeZoneRuleQuery* query)
{
    return GetPrevStartFromAnnualTimeZoneRule(rule, query);
}

I18n_ErrorCode OH_i18n_GetStartTimeAt(TimeArrayTimeZoneRule* rule, int32_t index, double* result)
{
    return GetStartTimeAt(rule, index, result);
}

I18n_ErrorCode OH_i18n_GetStartInYear(AnnualTimeZoneRule* rule, int32_t year, TimeZoneRuleQuery* query)
{
    return GetStartInYear(rule, year, query);
}

#ifdef __cplusplus
};
#endif
