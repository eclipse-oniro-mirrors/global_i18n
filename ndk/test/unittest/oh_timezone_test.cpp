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

#include <gtest/gtest.h>
#include "oh_timezone.h"
#include "oh_timezone_test.h"

using testing::ext::TestSize;

namespace OHOS {
namespace Global {
namespace I18n {
void OHTimeZoneTest::SetUpTestCase(void)
{
}

void OHTimeZoneTest::TearDownTestCase(void)
{
}

void OHTimeZoneTest::SetUp(void)
{
}

void OHTimeZoneTest::TearDown(void)
{
}

/**
 * @tc.name: OHTimeZoneFuncTest001
 * @tc.desc: Test timezone native api
 * @tc.type: FUNC
 */
HWTEST_F(OHTimeZoneTest, OHTimeZoneFuncTest001, TestSize.Level1)
{
    TimeZoneRules rules;
    I18n_ErrorCode errCode = OH_i18n_GetTimeZoneRules("Asia/Shanghai", &rules);
    EXPECT_EQ(errCode, I18n_ErrorCode::SUCCESS);
    EXPECT_EQ(rules.initial.dstSavings, 0);
    EXPECT_EQ(rules.initial.rawOffset, 29143000);
    EXPECT_EQ(rules.numAnnualRules, 0);
    EXPECT_EQ(rules.numTimeArrayRules, 2);

    TimeArrayTimeZoneRule rule = rules.timeArrayRules[0];
    std::string ruleName = rule.name;
    EXPECT_EQ(ruleName, "Asia/Shanghai(STD)");
    EXPECT_EQ(rule.rawOffset, 28800000);
    EXPECT_EQ(rule.dstSavings, 0);
    EXPECT_EQ(rule.numStartTimes, 15);
    EXPECT_EQ(rule.timeRuleType, TimeRuleType::UTC_TIME);
    TimeZoneRuleQuery query;
    errCode = OH_i18n_GetFirstStartFromTimeArrayTimeZoneRule(&rule, &query);
    EXPECT_EQ(errCode, I18n_ErrorCode::SUCCESS);
    EXPECT_EQ(query.result, -2177481943000); // 1900-12-31 23:54:17
    errCode = OH_i18n_GetFinalStartFromTimeArrayTimeZoneRule(&rule, &query);
    EXPECT_EQ(errCode, I18n_ErrorCode::SUCCESS);
    EXPECT_EQ(query.result, 684867600000); // 1991-09-15 01:00:00
    errCode = OH_i18n_GetNextStartFromTimeArrayTimeZoneRule(&rule, &query);
    EXPECT_EQ(errCode, I18n_ErrorCode::SUCCESS);
    EXPECT_EQ(query.result, 527014800000); // 1986-09-14 01:00:00
    errCode = OH_i18n_GetPrevStartFromTimeArrayTimeZoneRule(&rule, &query);
    EXPECT_EQ(errCode, I18n_ErrorCode::SUCCESS);
    EXPECT_EQ(query.result, -650019600000); // 1949-05-27 23:00:00
    double result;
    errCode = OH_i18n_GetStartTimeAt(&rule, 0, &result);
    EXPECT_EQ(errCode, I18n_ErrorCode::SUCCESS);
    EXPECT_EQ(result, -2177481943000); // 1900-12-31 23:54:17
}

/**
 * @tc.name: OHTimeZoneFuncTest002
 * @tc.desc: Test timezone native api
 * @tc.type: FUNC
 */
HWTEST_F(OHTimeZoneTest, OHTimeZoneFuncTest002, TestSize.Level1)
{
    TimeZoneRules rules;
    I18n_ErrorCode errCode = OH_i18n_GetTimeZoneRules("America/Santiago", &rules);
    EXPECT_EQ(errCode, I18n_ErrorCode::SUCCESS);
    EXPECT_EQ(rules.initial.dstSavings, 0);
    EXPECT_EQ(rules.initial.rawOffset, -16965000);
    EXPECT_EQ(rules.numAnnualRules, 2);
    EXPECT_EQ(rules.numTimeArrayRules, 5);

    AnnualTimeZoneRule rule = rules.annualRules[0];
    std::string ruleName = rule.name;
    EXPECT_EQ(ruleName, "(STD)");
    EXPECT_EQ(rule.startYear, 2024);
    EXPECT_EQ(rule.endYear, MAX_YEAR_IN_ANNUAL_TIMEZONE_RULE);
    EXPECT_EQ(rule.rawOffset, -14400000);
    EXPECT_EQ(rule.dstSavings, 0);
    EXPECT_EQ(rule.dateTimeRule.month, 3);
    EXPECT_EQ(rule.dateTimeRule.dayOfMonth, 2);
    EXPECT_EQ(rule.dateTimeRule.dayOfWeek, 1);
    EXPECT_EQ(rule.dateTimeRule.weekInMonth, 0);
    EXPECT_EQ(rule.dateTimeRule.millisInDay, 10800000);
    EXPECT_EQ(rule.dateTimeRule.dateRuleType, DateRuleType::DOW_GEQ_DOM);
    EXPECT_EQ(rule.dateTimeRule.timeRuleType, TimeRuleType::UTC_TIME);

    TimeZoneRuleQuery query;
    query.base = 1712458800000; // 2024-04-07 11:00:00
    query.prevRawOffset = -14400000;
    query.prevDSTSavings = 0;
    query.inclusive = false;
    errCode = OH_i18n_GetFirstStartFromAnnualTimeZoneRule(&rule, &query);
    EXPECT_EQ(errCode, I18n_ErrorCode::SUCCESS);
    EXPECT_EQ(query.result, 1712458800000); // 2024-04-07 11:00:00
    errCode = OH_i18n_GetFinalStartFromAnnualTimeZoneRule(&rule, &query);
    EXPECT_EQ(errCode, I18n_ErrorCode::ERROR_INVALID_PARAMETER);
    errCode = OH_i18n_GetNextStartFromAnnualTimeZoneRule(&rule, &query);
    EXPECT_EQ(errCode, I18n_ErrorCode::SUCCESS);
    EXPECT_EQ(query.result, 1743908400000); // 2025-04-06 11:00:00
    query.base = 1743908400000; // 2025-04-06 11:00:00
    errCode = OH_i18n_GetPrevStartFromAnnualTimeZoneRule(&rule, &query);
    EXPECT_EQ(errCode, I18n_ErrorCode::SUCCESS);
    EXPECT_EQ(query.result, 1712458800000); // 2024-04-07 11:00:00
    errCode = OH_i18n_GetStartInYear(&rule, 2024, &query);
    EXPECT_EQ(errCode, I18n_ErrorCode::SUCCESS);
    EXPECT_EQ(query.result, 1712458800000); // 2024-04-07 11:00:00
}
} // namespace I18n
} // namespace Global
} // namespace OHOS