/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
#include "iso8601_date_time_format_test.h"
#include "iso8601_date_time_format.h"

using namespace OHOS::Global::I18n;
using testing::ext::TestSize;
using namespace std;

namespace OHOS {
namespace Global {
namespace I18n {
class ISO8601DateTimeFormatTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void ISO8601DateTimeFormatTest::SetUpTestCase(void)
{
}

void ISO8601DateTimeFormatTest::TearDownTestCase(void)
{
}

void ISO8601DateTimeFormatTest::SetUp(void)
{
}

void ISO8601DateTimeFormatTest::TearDown(void)
{
}

/**
 * @tc.name: ISO8601DateTimeFormatFuncTest001
 * @tc.desc: Test ISO8601 date time format with Calendar format + DateOnly + Extended separator
 * @tc.type: FUNC
 */
HWTEST_F(ISO8601DateTimeFormatTest, ISO8601DateTimeFormatFuncTest001, TestSize.Level1)
{
    I18nErrorCode errCode = I18nErrorCode::SUCCESS;
    ISO8601DateTimeFormat formatter(ISO8601DateTimeFormat::DateFormat::CALENDAR,
        ISO8601DateTimeFormat::TimePrecision::DATE_ONLY,
        ISO8601DateTimeFormat::SeparatorStyle::EXTENDED, "", false, errCode);
    EXPECT_EQ(errCode, I18nErrorCode::SUCCESS);

    int64_t milliseconds = 1736899200000; // 2025-01-15 00:00:00 UTC
    std::string result = formatter.Format(milliseconds);
    EXPECT_EQ(result, "2025-01-15");
}

/**
 * @tc.name: ISO8601DateTimeFormatFuncTest002
 * @tc.desc: Test ISO8601 date time format with Calendar format + Hours precision
 * @tc.type: FUNC
 */
HWTEST_F(ISO8601DateTimeFormatTest, ISO8601DateTimeFormatFuncTest002, TestSize.Level1)
{
    I18nErrorCode errCode = I18nErrorCode::SUCCESS;
    ISO8601DateTimeFormat formatter(ISO8601DateTimeFormat::DateFormat::CALENDAR,
        ISO8601DateTimeFormat::TimePrecision::HOURS,
        ISO8601DateTimeFormat::SeparatorStyle::EXTENDED, "", false, errCode);
    EXPECT_EQ(errCode, I18nErrorCode::SUCCESS);

    int64_t milliseconds = 1736899200000; // 2025-01-15 00:00:00 UTC
    std::string result = formatter.Format(milliseconds);
    EXPECT_EQ(result, "2025-01-15T00");
}

/**
 * @tc.name: ISO8601DateTimeFormatFuncTest003
 * @tc.desc: Test ISO8601 date time format with Calendar format + Minutes precision
 * @tc.type: FUNC
 */
HWTEST_F(ISO8601DateTimeFormatTest, ISO8601DateTimeFormatFuncTest003, TestSize.Level1)
{
    I18nErrorCode errCode = I18nErrorCode::SUCCESS;
    ISO8601DateTimeFormat formatter(ISO8601DateTimeFormat::DateFormat::CALENDAR,
        ISO8601DateTimeFormat::TimePrecision::MINUTES,
        ISO8601DateTimeFormat::SeparatorStyle::EXTENDED, "", false, errCode);
    EXPECT_EQ(errCode, I18nErrorCode::SUCCESS);

    int64_t milliseconds = 1736899200000; // 2025-01-15 00:00:00 UTC
    std::string result = formatter.Format(milliseconds);
    EXPECT_EQ(result, "2025-01-15T00:00");
}

/**
 * @tc.name: ISO8601DateTimeFormatFuncTest004
 * @tc.desc: Test ISO8601 date time format with Calendar format + Seconds precision
 * @tc.type: FUNC
 */
HWTEST_F(ISO8601DateTimeFormatTest, ISO8601DateTimeFormatFuncTest004, TestSize.Level1)
{
    I18nErrorCode errCode = I18nErrorCode::SUCCESS;
    ISO8601DateTimeFormat formatter(ISO8601DateTimeFormat::DateFormat::CALENDAR,
        ISO8601DateTimeFormat::TimePrecision::SECONDS,
        ISO8601DateTimeFormat::SeparatorStyle::EXTENDED, "", false, errCode);
    EXPECT_EQ(errCode, I18nErrorCode::SUCCESS);

    int64_t milliseconds = 1736899200000; // 2025-01-15 00:00:00 UTC
    std::string result = formatter.Format(milliseconds);
    EXPECT_EQ(result, "2025-01-15T00:00:00");
}

/**
 * @tc.name: ISO8601DateTimeFormatFuncTest005
 * @tc.desc: Test ISO8601 date time format with Calendar format + Milliseconds precision
 * @tc.type: FUNC
 */
HWTEST_F(ISO8601DateTimeFormatTest, ISO8601DateTimeFormatFuncTest005, TestSize.Level1)
{
    I18nErrorCode errCode = I18nErrorCode::SUCCESS;
    ISO8601DateTimeFormat formatter(ISO8601DateTimeFormat::DateFormat::CALENDAR,
        ISO8601DateTimeFormat::TimePrecision::MILLISECONDS,
        ISO8601DateTimeFormat::SeparatorStyle::EXTENDED, "", false, errCode);
    EXPECT_EQ(errCode, I18nErrorCode::SUCCESS);

    int64_t milliseconds = 1736899200123; // 2025-01-15 00:00:00.123 UTC
    std::string result = formatter.Format(milliseconds);
    EXPECT_EQ(result, "2025-01-15T00:00:00.123");
}

/**
 * @tc.name: ISO8601DateTimeFormatFuncTest006
 * @tc.desc: Test ISO8601 date time format with Ordinal format
 * @tc.type: FUNC
 */
HWTEST_F(ISO8601DateTimeFormatTest, ISO8601DateTimeFormatFuncTest006, TestSize.Level1)
{
    I18nErrorCode errCode = I18nErrorCode::SUCCESS;
    ISO8601DateTimeFormat formatter(ISO8601DateTimeFormat::DateFormat::ORDINAL,
        ISO8601DateTimeFormat::TimePrecision::DATE_ONLY,
        ISO8601DateTimeFormat::SeparatorStyle::EXTENDED, "", false, errCode);
    EXPECT_EQ(errCode, I18nErrorCode::SUCCESS);

    int64_t milliseconds = 1736899200000; // 2025-01-15 00:00:00 UTC (day 15 of the year)
    std::string result = formatter.Format(milliseconds);
    EXPECT_EQ(result, "2025-015");
}

/**
 * @tc.name: ISO8601DateTimeFormatFuncTest007
 * @tc.desc: Test ISO8601 date time format with Week format
 * @tc.type: FUNC
 */
HWTEST_F(ISO8601DateTimeFormatTest, ISO8601DateTimeFormatFuncTest007, TestSize.Level1)
{
    I18nErrorCode errCode = I18nErrorCode::SUCCESS;
    ISO8601DateTimeFormat formatter(ISO8601DateTimeFormat::DateFormat::WEEK,
        ISO8601DateTimeFormat::TimePrecision::DATE_ONLY,
        ISO8601DateTimeFormat::SeparatorStyle::EXTENDED, "", false, errCode);
    EXPECT_EQ(errCode, I18nErrorCode::SUCCESS);

    int64_t milliseconds = 1736899200000; // 2025-01-15 00:00:00 UTC (Wednesday, week 3)
    std::string result = formatter.Format(milliseconds);
    EXPECT_EQ(result, "2025-W03-4");
}

/**
 * @tc.name: ISO8601DateTimeFormatFuncTest008
 * @tc.desc: Test ISO8601 date time format with Basic separator
 * @tc.type: FUNC
 */
HWTEST_F(ISO8601DateTimeFormatTest, ISO8601DateTimeFormatFuncTest008, TestSize.Level1)
{
    I18nErrorCode errCode = I18nErrorCode::SUCCESS;
    ISO8601DateTimeFormat formatter(ISO8601DateTimeFormat::DateFormat::CALENDAR,
        ISO8601DateTimeFormat::TimePrecision::SECONDS,
        ISO8601DateTimeFormat::SeparatorStyle::BASIC, "", false, errCode);
    EXPECT_EQ(errCode, I18nErrorCode::SUCCESS);

    int64_t milliseconds = 1736899200000; // 2025-01-15 00:00:00 UTC
    std::string result = formatter.Format(milliseconds);
    EXPECT_EQ(result, "20250115T000000");
}

/**
 * @tc.name: ISO8601DateTimeFormatFuncTest009
 * @tc.desc: Test ISO8601 date time format with display time zone
 * @tc.type: FUNC
 */
HWTEST_F(ISO8601DateTimeFormatTest, ISO8601DateTimeFormatFuncTest009, TestSize.Level1)
{
    I18nErrorCode errCode = I18nErrorCode::SUCCESS;
    ISO8601DateTimeFormat formatter(ISO8601DateTimeFormat::DateFormat::CALENDAR,
        ISO8601DateTimeFormat::TimePrecision::SECONDS,
        ISO8601DateTimeFormat::SeparatorStyle::EXTENDED, "", true, errCode);
    EXPECT_EQ(errCode, I18nErrorCode::SUCCESS);

    int64_t milliseconds = 1736899200000; // 2025-01-15 00:00:00 UTC
    std::string result = formatter.Format(milliseconds);
    EXPECT_EQ(result, "2025-01-15T00:00:00Z");
}

/**
 * @tc.name: ISO8601DateTimeFormatFuncTest010
 * @tc.desc: Test ISO8601 date time format with custom time zone (Asia/Shanghai)
 * @tc.type: FUNC
 */
HWTEST_F(ISO8601DateTimeFormatTest, ISO8601DateTimeFormatFuncTest010, TestSize.Level1)
{
    I18nErrorCode errCode = I18nErrorCode::SUCCESS;
    ISO8601DateTimeFormat formatter(ISO8601DateTimeFormat::DateFormat::CALENDAR,
        ISO8601DateTimeFormat::TimePrecision::SECONDS,
        ISO8601DateTimeFormat::SeparatorStyle::EXTENDED, "Asia/Shanghai", true, errCode);
    EXPECT_EQ(errCode, I18nErrorCode::SUCCESS);

    int64_t milliseconds = 1736899200000; // 2025-01-15 00:00:00 UTC
    std::string result = formatter.Format(milliseconds);
    EXPECT_EQ(result, "2025-01-15T08:00:00+08:00");
}

/**
 * @tc.name: ISO8601DateTimeFormatFuncTest011
 * @tc.desc: Test ISO8601 date time format with default UTC time zone
 * @tc.type: FUNC
 */
HWTEST_F(ISO8601DateTimeFormatTest, ISO8601DateTimeFormatFuncTest011, TestSize.Level1)
{
    I18nErrorCode errCode = I18nErrorCode::SUCCESS;
    // Empty time zone should default to UTC
    ISO8601DateTimeFormat formatter(ISO8601DateTimeFormat::DateFormat::CALENDAR,
        ISO8601DateTimeFormat::TimePrecision::SECONDS,
        ISO8601DateTimeFormat::SeparatorStyle::EXTENDED, "", true, errCode);
    EXPECT_EQ(errCode, I18nErrorCode::SUCCESS);

    int64_t milliseconds = 1736899200000; // 2025-01-15 00:00:00 UTC
    std::string result = formatter.Format(milliseconds);
    EXPECT_EQ(result, "2025-01-15T00:00:00Z");
}
} // namespace I18n
} // namespace Global
} // namespace OHOS