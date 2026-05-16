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
#include "intl_date_time_format.h"

#include "intl_date_time_format_test.h"

using namespace OHOS::Global::I18n;
using testing::ext::TestSize;
using namespace std;

namespace OHOS {
namespace Global {
namespace I18n {
class IntlDateTimeFormatTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void IntlDateTimeFormatTest::SetUpTestCase(void)
{
}

void IntlDateTimeFormatTest::TearDownTestCase(void)
{
}

void IntlDateTimeFormatTest::SetUp(void)
{
}

void IntlDateTimeFormatTest::TearDown(void)
{}

/**
 * @tc.name: IntlDateTimeFormatFuncTest001
 * @tc.desc: Test Intl date time format
 * @tc.type: FUNC
 */
HWTEST_F(IntlDateTimeFormatTest, IntlDateTimeFormatFuncTest001, TestSize.Level1)
{
    std::vector<std::string> localeTags = { "zh-Hans-CN" };
    std::unordered_map<std::string, std::string> configs;
    std::string errMessage;
    std::unique_ptr<IntlDateTimeFormat> intlDateTimeFormat = std::make_unique<IntlDateTimeFormat>(localeTags,
        configs, errMessage);
    double startMilliseconds = 987654321;
    double endMilliseconds = 1987987654;

    std::string formatResult = intlDateTimeFormat->Format(startMilliseconds);
    EXPECT_EQ(formatResult, "1970/1/12");
    std::vector<std::pair<std::string, std::string>> formatToPartsResult =
        intlDateTimeFormat->FormatToParts(startMilliseconds, errMessage);
    EXPECT_EQ(formatToPartsResult.size(), 5);
    EXPECT_EQ(formatToPartsResult[0].first, "year");
    EXPECT_EQ(formatToPartsResult[0].second, "1970");
    EXPECT_EQ(formatToPartsResult[1].first, "literal");
    EXPECT_EQ(formatToPartsResult[1].second, "/");
    EXPECT_EQ(formatToPartsResult[2].first, "month");
    EXPECT_EQ(formatToPartsResult[2].second, "1");
    EXPECT_EQ(formatToPartsResult[3].first, "literal");
    EXPECT_EQ(formatToPartsResult[3].second, "/");
    EXPECT_EQ(formatToPartsResult[4].first, "day");
    EXPECT_EQ(formatToPartsResult[4].second, "12");
    EXPECT_EQ(errMessage, "");

    std::string formatRangeResult = intlDateTimeFormat->FormatRange(startMilliseconds, endMilliseconds, errMessage);
    EXPECT_EQ(formatRangeResult, "1970/1/12 \xE2\x80\x93 1970/1/24");
    EXPECT_EQ(errMessage, "");
    std::vector<std::pair<std::string, std::string>> formatRangeToPartsResult =
        intlDateTimeFormat->FormatRangeToParts(startMilliseconds, endMilliseconds, errMessage);
    EXPECT_EQ(formatRangeToPartsResult.size(), 11);
    std::string result;
    for (auto part : formatRangeToPartsResult) {
        result += part.second;
    }
    EXPECT_EQ(result, "1970/1/12 \xE2\x80\x93 1970/1/24");
    EXPECT_EQ(errMessage, "");
}

/**
 * @tc.name: IntlDateTimeFormatFuncTest002
 * @tc.desc: Test Intl date time format
 * @tc.type: FUNC
 */
HWTEST_F(IntlDateTimeFormatTest, IntlDateTimeFormatFuncTest002, TestSize.Level1)
{
    std::vector<std::string> localeTags = { "zh-Hans-CN" };
    std::unordered_map<std::string, std::string> configs;
    std::string errMessage;
    std::unique_ptr<IntlDateTimeFormat> intlDateTimeFormat = std::make_unique<IntlDateTimeFormat>(localeTags,
        configs, errMessage);
    std::unordered_map<std::string, std::string> resolvedConfigs;
    intlDateTimeFormat->ResolvedOptions(resolvedConfigs);
    EXPECT_EQ(resolvedConfigs[IntlDateTimeFormat::LOCALE_TAG], "zh-Hans-CN");
    EXPECT_EQ(resolvedConfigs[IntlDateTimeFormat::NUMBERING_SYSTEM_TAG], "latn");
    EXPECT_EQ(resolvedConfigs[IntlDateTimeFormat::HOUR_CYCLE_TAG], "");
    EXPECT_EQ(resolvedConfigs[IntlDateTimeFormat::HOUR12_TAG], "");
    EXPECT_EQ(resolvedConfigs[IntlDateTimeFormat::CALENDAR_TAG], "gregory");
    EXPECT_EQ(resolvedConfigs[IntlDateTimeFormat::TIME_ZONE_TAG], "America/Chicago");
    EXPECT_EQ(resolvedConfigs[IntlDateTimeFormat::DATE_STYLE_TAG], "");
    EXPECT_EQ(resolvedConfigs[IntlDateTimeFormat::TIME_STYLE_TAG], "");
    EXPECT_EQ(resolvedConfigs[IntlDateTimeFormat::FRACTIONAL_SECOND_DIGITS_TAG], "");

    I18nErrorCode status = I18nErrorCode::SUCCESS;
    std::map<std::string, std::string> supportedConfigs;
    std::vector<std::string> supportedLocales =
        IntlDateTimeFormat::SupportedLocalesOf(localeTags, supportedConfigs, status);
    EXPECT_EQ(supportedLocales.size(), 1);
    EXPECT_EQ(supportedLocales[0], "zh-Hans-CN");
    EXPECT_EQ(status, I18nErrorCode::SUCCESS);
}

/**
 * @tc.name: IntlDateTimeFormatFuncTest003
 * @tc.desc: Test Intl date time format
 * @tc.type: FUNC
 */
HWTEST_F(IntlDateTimeFormatTest, IntlDateTimeFormatFuncTest003, TestSize.Level1)
{
    std::vector<std::string> localeTags = { "zh-Hans-CN" };
    std::unordered_map<std::string, std::string> configs = {
        { IntlDateTimeFormat::LOCALE_MATCHER_TAG, "lookup" },
        { IntlDateTimeFormat::WEEK_DAY_TAG, "short" },
        { IntlDateTimeFormat::ERA_TAG, "narrow" },
        { IntlDateTimeFormat::YEAR_TAG, "2-digit" },
        { IntlDateTimeFormat::MONTH_TAG, "2-digit" },
        { IntlDateTimeFormat::DAY_TAG, "2-digit" },
        { IntlDateTimeFormat::HOUR_TAG, "2-digit" },
        { IntlDateTimeFormat::MINUTE_TAG, "2-digit" },
        { IntlDateTimeFormat::SECOND_TAG, "2-digit" },
        { IntlDateTimeFormat::TIME_ZONE_NAME_TAG, "short" },
        { IntlDateTimeFormat::FORMAT_MATCHER_TAG, "basic" },
        { IntlDateTimeFormat::HOUR12_TAG, "false" },
        { IntlDateTimeFormat::TIME_ZONE_TAG, "Asia/Shanghai" },
        { IntlDateTimeFormat::DAY_PERIOD_TAG, "narrow" },
        { IntlDateTimeFormat::NUMBERING_SYSTEM_TAG, "latn" }
    };
    std::string errMessage;
    std::unique_ptr<IntlDateTimeFormat> intlDateTimeFormat = std::make_unique<IntlDateTimeFormat>(localeTags,
        configs, errMessage);
    double startMilliseconds = 987654321;
    double endMilliseconds = 1987987654;

    std::string formatResult = intlDateTimeFormat->Format(startMilliseconds);
    EXPECT_EQ(formatResult, "公元70年1月12日周一 GMT+8 18:20:54");

    std::string formatRangeResult = intlDateTimeFormat->FormatRange(startMilliseconds, endMilliseconds, errMessage);
    EXPECT_EQ(formatRangeResult, "公元70年1月12日周一 GMT+8 18:20:54 – 公元70年1月24日周六 GMT+8 08:13:07");
    std::unordered_map<std::string, std::string> resolvedConfigs;
    intlDateTimeFormat->ResolvedOptions(resolvedConfigs);
    EXPECT_EQ(resolvedConfigs[IntlDateTimeFormat::LOCALE_TAG], "zh-Hans-CN-u-nu-latn");
    EXPECT_EQ(resolvedConfigs[IntlDateTimeFormat::NUMBERING_SYSTEM_TAG], "latn");
    EXPECT_EQ(resolvedConfigs[IntlDateTimeFormat::HOUR_CYCLE_TAG], "h24");
    EXPECT_EQ(resolvedConfigs[IntlDateTimeFormat::HOUR12_TAG], "false");
    EXPECT_EQ(resolvedConfigs[IntlDateTimeFormat::CALENDAR_TAG], "gregory");
    EXPECT_EQ(resolvedConfigs[IntlDateTimeFormat::TIME_ZONE_TAG], "Asia/Shanghai");
    EXPECT_EQ(resolvedConfigs[IntlDateTimeFormat::DATE_STYLE_TAG], "");
    EXPECT_EQ(resolvedConfigs[IntlDateTimeFormat::TIME_STYLE_TAG], "");
    EXPECT_EQ(resolvedConfigs[IntlDateTimeFormat::FRACTIONAL_SECOND_DIGITS_TAG], "");
}

/**
 * @tc.name: IntlDateTimeFormatFuncTest004
 * @tc.desc: Test Intl date time format
 * @tc.type: FUNC
 */
HWTEST_F(IntlDateTimeFormatTest, IntlDateTimeFormatFuncTest004, TestSize.Level1)
{
    std::vector<std::string> fakeLocaleTags = { "test_locale" };
    std::unordered_map<std::string, std::string> configs;
    std::string errMessage;
    std::unique_ptr<IntlDateTimeFormat> intlDateTimeFormat = std::make_unique<IntlDateTimeFormat>(fakeLocaleTags,
        configs, errMessage);
    EXPECT_EQ(errMessage, "invalid locale");

    std::vector<std::string> weekDayLocale = { "zh-Hans-CN" };
    std::unordered_map<std::string, std::string> weekDayConfigs = {
        { IntlDateTimeFormat::WEEK_DAY_TAG, "faker" }
    };
    std::unique_ptr<IntlDateTimeFormat> weekDayFormatter = std::make_unique<IntlDateTimeFormat>(weekDayLocale,
        weekDayConfigs, errMessage);
    EXPECT_EQ(errMessage, "Value out of range for locale options property");
}

/**
 * @tc.name: IntlDateTimeFormatFuncTest005
 * @tc.desc: Test Intl date time format
 * @tc.type: FUNC
 */
HWTEST_F(IntlDateTimeFormatTest, IntlDateTimeFormatFuncTest005, TestSize.Level1)
{
    std::string errMessage;
    std::vector<std::string> timeZoneLocale = { "zh-Hans-CN" };
    std::unordered_map<std::string, std::string> timeZoneConfigs = {
        { IntlDateTimeFormat::TIME_ZONE_TAG, "timezone" }
    };
    std::unique_ptr<IntlDateTimeFormat> timeZoneFormatter =
        std::make_unique<IntlDateTimeFormat>(timeZoneLocale, timeZoneConfigs, errMessage);
    EXPECT_EQ(errMessage, "invalid timeZone");

    std::vector<std::string> timeStyleLocale = { "zh-Hans-CN" };
    std::unordered_map<std::string, std::string> timeStyleConfigs = {
        { IntlDateTimeFormat::TIME_STYLE_TAG, "test" }
    };
    std::unique_ptr<IntlDateTimeFormat> timeStyleFormatter =
        std::make_unique<IntlDateTimeFormat>(timeStyleLocale, timeStyleConfigs, errMessage);
    EXPECT_EQ(errMessage, "Value out of range for locale options property");
}
} // namespace I18n
} // namespace Global
} // namespace OHOS