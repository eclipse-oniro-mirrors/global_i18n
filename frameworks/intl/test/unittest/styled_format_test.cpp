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
#include <sys/stat.h>
#include "date_time_format_part.h"
#include "styled_date_time_format.h"

#include "styled_format_test.h"

using namespace OHOS::Global::I18n;
using testing::ext::TestSize;
using namespace testing;

namespace OHOS {
namespace Global {
namespace I18n {
void StyledFormatTest::SetUpTestCase(void)
{
}

void StyledFormatTest::TearDownTestCase(void)
{
}

void StyledFormatTest::SetUp(void)
{}

void StyledFormatTest::TearDown(void)
{}

/**
 * @tc.name: StyledFormatTest001
 * @tc.desc: Test DateTimeFormatPart
 * @tc.type: FUNC
 */
HWTEST_F(StyledFormatTest, StyledFormatTest001, TestSize.Level1)
{
    std::unordered_set<std::string> supportedTypes = StyledDateTimeFormat::GetSupportedTypes();
    EXPECT_TRUE(supportedTypes.find("year") != supportedTypes.end());
    EXPECT_TRUE(supportedTypes.find("month") != supportedTypes.end());
    EXPECT_TRUE(supportedTypes.find("day") != supportedTypes.end());
    EXPECT_TRUE(supportedTypes.find("hour") != supportedTypes.end());
    EXPECT_TRUE(supportedTypes.find("minute") != supportedTypes.end());
    EXPECT_TRUE(supportedTypes.find("second") != supportedTypes.end());
    EXPECT_TRUE(supportedTypes.find("weekday") != supportedTypes.end());
    EXPECT_TRUE(supportedTypes.find("era") != supportedTypes.end());
    EXPECT_TRUE(supportedTypes.find("timeZoneName") != supportedTypes.end());
    EXPECT_TRUE(supportedTypes.find("dayPeriod") != supportedTypes.end());
}

/**
 * @tc.name: StyledFormatTest002
 * @tc.desc: Test DateTimeFormatPart
 * @tc.type: FUNC
 */
HWTEST_F(StyledFormatTest, StyledFormatTest002, TestSize.Level1)
{
    I18nErrorCode errCode = I18nErrorCode::SUCCESS;
    std::shared_ptr<SimpleDateTimeFormat> simpleFormatter =
        std::make_shared<SimpleDateTimeFormat>("y'年'M'月'dhmsazEG", "zh-Hans-CN", true, errCode);
    EXPECT_EQ(errCode, I18nErrorCode::SUCCESS);
    
    std::shared_ptr<StyledDateTimeFormat> formatter
        = std::make_shared<StyledDateTimeFormat>(simpleFormatter, nullptr);
    double date = 1761984846000;
    std::pair<std::string, std::vector<DateTimeFormatPart>> result = formatter->Format(date);
    EXPECT_EQ(result.first, "2025年11月14146下午GMT+8周六公元");
    EXPECT_EQ(result.second.size(), 12);
    EXPECT_EQ(result.second[0].GetPartName(), "year");
    EXPECT_EQ(result.second[0].GetStart(), 0);
    result.second[0].SetStart(3);
    EXPECT_EQ(result.second[0].GetStart(), 3);
    EXPECT_EQ(result.second[0].GetLength(), 4);
    EXPECT_EQ(result.second[1].GetPartName(), "literal");
    EXPECT_EQ(result.second[1].GetStart(), 4);
    EXPECT_EQ(result.second[1].GetLength(), 1);
    EXPECT_EQ(result.second[2].GetPartName(), "month");
    EXPECT_EQ(result.second[2].GetStart(), 5);
    EXPECT_EQ(result.second[2].GetLength(), 2);
}

/**
 * @tc.name: StyledFormatTest003
 * @tc.desc: Test DateTimeFormatPart
 * @tc.type: FUNC
 */
HWTEST_F(StyledFormatTest, StyledFormatTest003, TestSize.Level1)
{
    std::vector<std::string> localeTags = { "en-GB" };
    std::unordered_map<std::string, std::string> configs {
        { "dateStyle", "full" },
        { "timeStyle", "full" },
    };
    std::string errMessage;
    std::shared_ptr<IntlDateTimeFormat> intlDateFormatter =
        std::make_shared<IntlDateTimeFormat>(localeTags, configs, errMessage);
    EXPECT_EQ(errMessage, "");
    
    std::shared_ptr<StyledDateTimeFormat> formatter
        = std::make_shared<StyledDateTimeFormat>(nullptr, intlDateFormatter);
    double date = 1761984846000;
    std::pair<std::string, std::vector<DateTimeFormatPart>> result = formatter->Format(date);
    EXPECT_EQ(result.first, "Saturday 1 November 2025 at 16:14:06 Central Standard Time");
    EXPECT_EQ(result.second.size(), 15);
    EXPECT_EQ(result.second[0].GetPartName(), "weekday");
    EXPECT_EQ(result.second[0].GetStart(), 0);
    EXPECT_EQ(result.second[0].GetLength(), 8);
    EXPECT_EQ(result.second[1].GetPartName(), "literal");
    EXPECT_EQ(result.second[1].GetStart(), 8);
    EXPECT_EQ(result.second[1].GetLength(), 1);
    EXPECT_EQ(result.second[2].GetPartName(), "day");
    EXPECT_EQ(result.second[2].GetStart(), 9);
    EXPECT_EQ(result.second[2].GetLength(), 1);
}

} // namespace I18n
} // namespace Global
} // namespace OHOS