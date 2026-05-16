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

#include "date_time_format_test.h"
#include "intl_date_time_format.h"
#include "locale_config.h"
#include "styled_date_time_format.h"
#include "symbol_date_time_format.h"

using namespace OHOS::Global::I18n;
using testing::ext::TestSize;
using namespace std;

namespace OHOS {
namespace Global {
namespace I18n {
class DateTimeFormatTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void DateTimeFormatTest::SetUpTestCase(void)
{
}

void DateTimeFormatTest::TearDownTestCase(void)
{
}

void DateTimeFormatTest::SetUp(void)
{
}

void DateTimeFormatTest::TearDown(void)
{}

/**
 * @tc.name: DateTimeFormatFuncTest001
 * @tc.desc: Test Intl date time format
 * @tc.type: FUNC
 */
HWTEST_F(DateTimeFormatTest, DateTimeFormatFuncTest001, TestSize.Level1)
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
    auto formatToPartsResult = intlDateTimeFormat->FormatToParts(startMilliseconds, errMessage);
    EXPECT_EQ(formatToPartsResult.size(), 5);
    EXPECT_EQ(formatToPartsResult[0][IntlDateTimeFormat::PART_TYPE_TAG], "year");
    EXPECT_EQ(formatToPartsResult[0][IntlDateTimeFormat::PART_VALUE_TAG], "1970");
    EXPECT_EQ(formatToPartsResult[1][IntlDateTimeFormat::PART_TYPE_TAG], "literal");
    EXPECT_EQ(formatToPartsResult[1][IntlDateTimeFormat::PART_VALUE_TAG], "/");
    EXPECT_EQ(formatToPartsResult[2][IntlDateTimeFormat::PART_TYPE_TAG], "month");
    EXPECT_EQ(formatToPartsResult[2][IntlDateTimeFormat::PART_VALUE_TAG], "1");
    EXPECT_EQ(formatToPartsResult[3][IntlDateTimeFormat::PART_TYPE_TAG], "literal");
    EXPECT_EQ(formatToPartsResult[3][IntlDateTimeFormat::PART_VALUE_TAG], "/");
    EXPECT_EQ(formatToPartsResult[4][IntlDateTimeFormat::PART_TYPE_TAG], "day");
    EXPECT_EQ(formatToPartsResult[4][IntlDateTimeFormat::PART_VALUE_TAG], "12");
    EXPECT_EQ(errMessage, "");

    std::string formatRangeResult = intlDateTimeFormat->FormatRange(startMilliseconds, endMilliseconds, errMessage);
    EXPECT_EQ(formatRangeResult, "1970/1/12 \xE2\x80\x93 1970/1/24");
    EXPECT_EQ(errMessage, "");
    auto formatRangeToPartsResult =
        intlDateTimeFormat->FormatRangeToParts(startMilliseconds, endMilliseconds, errMessage);
    EXPECT_EQ(formatRangeToPartsResult.size(), 11);
    std::string result;
    for (auto part : formatRangeToPartsResult) {
        result += part[IntlDateTimeFormat::PART_VALUE_TAG];
    }
    EXPECT_EQ(result, "1970/1/12 \xE2\x80\x93 1970/1/24");
    EXPECT_EQ(errMessage, "");
}

/**
 * @tc.name: DateTimeFormatFuncTest002
 * @tc.desc: Test Intl date time format
 * @tc.type: FUNC
 */
HWTEST_F(DateTimeFormatTest, DateTimeFormatFuncTest002, TestSize.Level1)
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
 * @tc.name: DateTimeFormatFuncTest003
 * @tc.desc: Test Intl date time format
 * @tc.type: FUNC
 */
HWTEST_F(DateTimeFormatTest, DateTimeFormatFuncTest003, TestSize.Level1)
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
 * @tc.name: DateTimeFormatFuncTest004
 * @tc.desc: Test Intl date time format
 * @tc.type: FUNC
 */
HWTEST_F(DateTimeFormatTest, DateTimeFormatFuncTest004, TestSize.Level1)
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
 * @tc.name: DateTimeFormatFuncTest005
 * @tc.desc: Test Intl date time format
 * @tc.type: FUNC
 */
HWTEST_F(DateTimeFormatTest, DateTimeFormatFuncTest005, TestSize.Level1)
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

/**
 * @tc.name: DateTimeFormatFuncTest006
 * @tc.desc: Test DateTimeFormatPart
 * @tc.type: FUNC
 */
HWTEST_F(DateTimeFormatTest, DateTimeFormatFuncTest006, TestSize.Level1)
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
 * @tc.name: DateTimeFormatFuncTest007
 * @tc.desc: Test DateTimeFormatPart
 * @tc.type: FUNC
 */
HWTEST_F(DateTimeFormatTest, DateTimeFormatFuncTest007, TestSize.Level1)
{
    I18nErrorCode errCode = I18nErrorCode::SUCCESS;
    std::shared_ptr<SimpleDateTimeFormat> simpleFormatter =
        std::make_shared<SimpleDateTimeFormat>("y'年'M'月'dhmsazEG", "zh-Hans-CN", true, errCode);
    EXPECT_EQ(errCode, I18nErrorCode::SUCCESS);

    std::shared_ptr<StyledDateTimeFormat> formatter
        = std::make_shared<StyledDateTimeFormat>(simpleFormatter);
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
 * @tc.name: DateTimeFormatFuncTest008
 * @tc.desc: Test DateTimeFormatPart
 * @tc.type: FUNC
 */
HWTEST_F(DateTimeFormatTest, DateTimeFormatFuncTest008, TestSize.Level1)
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
        = std::make_shared<StyledDateTimeFormat>(intlDateFormatter);
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

/**
 * @tc.name: DateTimeFormatFuncTest009
 * @tc.desc: Test SymbolDateTimeFormat
 * @tc.type: FUNC
 */
HWTEST_F(DateTimeFormatTest, DateTimeFormatFuncTest009, TestSize.Level1)
{
    std::vector<std::string> localeTags = { "en-GB" };
    std::unordered_map<std::string, std::string> configs {
        { "timeStyle", "short" },
        { "hour12", "true" },
        { "pm", "**" },
        { "am", "am" },
    };
    std::string errMessage;
    auto symbolDateFormatter = std::make_shared<SymbolDateTimeFormat>(localeTags, configs, errMessage);
    EXPECT_EQ(errMessage, "");

    double amDate = 1761958846000;
    double pmDate = 1761984846000;
    std::string formatResult = symbolDateFormatter->Format(amDate);
    EXPECT_EQ(formatResult, "09:00\xE2\x80\xAF""am");
    formatResult = symbolDateFormatter->Format(pmDate);
    EXPECT_EQ(formatResult, "04:14\xE2\x80\xAF**");

    auto parts = symbolDateFormatter->FormatToParts(pmDate, errMessage);
    EXPECT_EQ(errMessage, "");
    EXPECT_EQ(parts.size(), 5);

    std::unordered_map<std::string, std::string> options;
    symbolDateFormatter->ResolvedOptions(options);
    EXPECT_EQ(options["am"], "am");
    EXPECT_EQ(options["pm"], "**");
    EXPECT_EQ(options["timeStyle"], "short");
    EXPECT_EQ(options["hour12"], "true");
}

/**
 * @tc.name: DateTimeFormatFuncTest010
 * @tc.desc: Test SymbolDateTimeFormat
 * @tc.type: FUNC
 */
HWTEST_F(DateTimeFormatTest, DateTimeFormatFuncTest010, TestSize.Level1)
{
    std::vector<std::string> localeTags = { "en-GB" };
    std::unordered_map<std::string, std::string> configs {
        { "timeStyle", "short" },
        { "hour12", "true" },
    };
    std::string errMessage;
    std::shared_ptr<IntlDateTimeFormat> intlDateFormatter =
        std::make_shared<IntlDateTimeFormat>(localeTags, configs, errMessage);
    EXPECT_EQ(errMessage, "");
    configs.insert(std::make_pair("am", "++"));
    configs.insert(std::make_pair("pm", "--"));
    auto symbolDateFormatter = std::make_shared<SymbolDateTimeFormat>(localeTags, configs, errMessage);
    EXPECT_EQ(errMessage, "");

    double startDate = 1761958846000;
    double endDate = 1761984846000;
    std::string rangeResult = intlDateFormatter->FormatRange(startDate, endDate, errMessage);
    EXPECT_EQ(errMessage, "");
    std::string symbolRangeResult = symbolDateFormatter->FormatRange(startDate, endDate, errMessage);
    EXPECT_EQ(errMessage, "");
    EXPECT_EQ(rangeResult, symbolRangeResult);

    auto rangeParts = intlDateFormatter->FormatRangeToParts(startDate, endDate, errMessage);
    EXPECT_EQ(errMessage, "");
    auto symbolRangeParts = symbolDateFormatter->FormatRangeToParts(startDate, endDate, errMessage);
    EXPECT_EQ(errMessage, "");
    ASSERT_TRUE(rangeParts.size() == symbolRangeParts.size());
    for (size_t i = 0; i < rangeParts.size(); i++) {
        EXPECT_EQ(rangeParts[i][IntlDateTimeFormat::PART_TYPE_TAG],
            symbolRangeParts[i][IntlDateTimeFormat::PART_TYPE_TAG]);
        EXPECT_EQ(rangeParts[i][IntlDateTimeFormat::PART_VALUE_TAG],
            symbolRangeParts[i][IntlDateTimeFormat::PART_VALUE_TAG]);
        EXPECT_EQ(rangeParts[i][IntlDateTimeFormat::PART_SOURCE_TAG],
            symbolRangeParts[i][IntlDateTimeFormat::PART_SOURCE_TAG]);
    }
}

/**
 * @tc.name: DateTimeFormatFuncTest011
 * @tc.desc: Test SymbolDateTimeFormat
 * @tc.type: FUNC
 */
HWTEST_F(DateTimeFormatTest, DateTimeFormatFuncTest011, TestSize.Level1)
{
    std::vector<std::string> localeTags = { "en-GB" };
    std::unordered_map<std::string, std::string> configs {
        { "timeStyle", "full" },
        { "hour12", "true" },
        { "pm", "**" },
        { "am", "--" },
    };
    std::string errMessage;
    std::shared_ptr<SymbolDateTimeFormat> symbolDateFormatter =
        std::make_shared<SymbolDateTimeFormat>(localeTags, configs, errMessage);
    EXPECT_EQ(errMessage, "");

    std::shared_ptr<StyledDateTimeFormat> formatter
        = std::make_shared<StyledDateTimeFormat>(symbolDateFormatter);
    double date = 1761984846000;
    std::pair<std::string, std::vector<DateTimeFormatPart>> result = formatter->Format(date);
    EXPECT_EQ(result.first, "04:14:06\xE2\x80\xAF** Central Standard Time");
    EXPECT_EQ(result.second.size(), 9);
    EXPECT_EQ(result.second[0].GetPartName(), "hour");
    EXPECT_EQ(result.second[0].GetStart(), 0);
    EXPECT_EQ(result.second[0].GetLength(), 2);
    EXPECT_EQ(result.second[1].GetPartName(), "literal");
    EXPECT_EQ(result.second[1].GetStart(), 2);
    EXPECT_EQ(result.second[1].GetLength(), 1);
}

/**
 * @tc.name: DateTimeFormatFuncTest012
 * @tc.desc: Test SymbolDateTimeFormat Parse with valid date string
 * @tc.type: FUNC
 */
HWTEST_F(DateTimeFormatTest, DateTimeFormatFuncTest012, TestSize.Level1)
{
    std::vector<std::string> localeTags = { "zh-Hans-CN" };
    std::unordered_map<std::string, std::string> configs;
    std::string errMessage;
    std::unique_ptr<SymbolDateTimeFormat> symbolDateTimeFormat =
        std::make_unique<SymbolDateTimeFormat>(localeTags, configs, errMessage);
    EXPECT_EQ(errMessage, "");

    I18nErrorCode status = I18nErrorCode::SUCCESS;
    double result = symbolDateTimeFormat->Parse("1970/1/12", false, status);
    EXPECT_EQ(status, I18nErrorCode::SUCCESS);
    EXPECT_NEAR(result, 921600000, 1000);
}

/**
 * @tc.name: DateTimeFormatFuncTest013
 * @tc.desc: Test SymbolDateTimeFormat Parse with invalid date string
 * @tc.type: FUNC
 */
HWTEST_F(DateTimeFormatTest, DateTimeFormatFuncTest013, TestSize.Level1)
{
    std::vector<std::string> localeTags = { "zh-Hans-CN" };
    std::unordered_map<std::string, std::string> configs;
    std::string errMessage;
    std::unique_ptr<SymbolDateTimeFormat> symbolDateTimeFormat =
        std::make_unique<SymbolDateTimeFormat>(localeTags, configs, errMessage);
    EXPECT_EQ(errMessage, "");

    I18nErrorCode status = I18nErrorCode::SUCCESS;
    double result = symbolDateTimeFormat->Parse("invalid-date", false, status);
    EXPECT_EQ(status, I18nErrorCode::INVALID_PARAM);
    EXPECT_EQ(result, 0);
}

/**
 * @tc.name: DateTimeFormatFuncTest014
 * @tc.desc: Test SymbolDateTimeFormat Parse with lenient mode
 * @tc.type: FUNC
 */
HWTEST_F(DateTimeFormatTest, DateTimeFormatFuncTest014, TestSize.Level1)
{
    std::vector<std::string> localeTags = { "en-US" };
    std::unordered_map<std::string, std::string> configs;
    std::string errMessage;
    std::unique_ptr<SymbolDateTimeFormat> symbolDateTimeFormat =
        std::make_unique<SymbolDateTimeFormat>(localeTags, configs, errMessage);
    EXPECT_EQ(errMessage, "");

    I18nErrorCode status = I18nErrorCode::SUCCESS;
    double result = symbolDateTimeFormat->Parse("1/12/70", true, status);
    EXPECT_EQ(status, I18nErrorCode::SUCCESS);
    EXPECT_NEAR(result, 921600000, 1000);
}

/**
 * @tc.name: DateTimeFormatFuncTest015
 * @tc.desc: Test SymbolDateTimeFormat Parse with empty string
 * @tc.type: FUNC
 */
HWTEST_F(DateTimeFormatTest, DateTimeFormatFuncTest015, TestSize.Level1)
{
    std::vector<std::string> localeTags = { "zh-Hans-CN" };
    std::unordered_map<std::string, std::string> configs;
    std::string errMessage;
    std::unique_ptr<SymbolDateTimeFormat> symbolDateTimeFormat =
        std::make_unique<SymbolDateTimeFormat>(localeTags, configs, errMessage);
    EXPECT_EQ(errMessage, "");

    I18nErrorCode status = I18nErrorCode::SUCCESS;
    double result = symbolDateTimeFormat->Parse("", false, status);
    EXPECT_EQ(status, I18nErrorCode::INVALID_PARAM);
    EXPECT_EQ(result, 0);
}
} // namespace I18n
} // namespace Global
} // namespace OHOS