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
#include "displaynames_test.h"

#include <map>
#include <vector>

#include "displaynames.h"

using namespace OHOS::Global::I18n;
using testing::ext::TestSize;
using namespace std;

namespace OHOS {
namespace Global {
namespace I18n {

void DisplaynamesTest::SetUpTestCase(void)
{}

void DisplaynamesTest::TearDownTestCase(void)
{}

void DisplaynamesTest::SetUp(void)
{}

void DisplaynamesTest::TearDown(void)
{}

/**
 * @tc.name: DisplaynamesTest0001
 * @tc.desc: Test Intl DisplayNames.SupportedLocalesOf
 * @tc.type: FUNC
 */
HWTEST_F(DisplaynamesTest, DisplaynamesTest0001, TestSize.Level1)
{
    std::vector<std::string> localeTags = {};
    std::map<std::string, std::string> options = {{"type", "language"}};
    DisplayNames displayNames(localeTags, options);
    I18nErrorCode status = displayNames.GetError();
    EXPECT_EQ(status, I18nErrorCode::SUCCESS);
    std::map<std::string, std::string> resolvedOptions = displayNames.ResolvedOptions();
    auto it = resolvedOptions.find("locale");
    ASSERT_TRUE(it != resolvedOptions.end());
    std::string defaultLocale = it->second;

    std::vector<std::string> requestLocales = {"zxx"};
    requestLocales.push_back(defaultLocale);
    status = I18nErrorCode::SUCCESS;
    std::vector<std::string> resultLocales = DisplayNames::SupportedLocalesOf(requestLocales, options, status);
    EXPECT_EQ(status, I18nErrorCode::SUCCESS);
    EXPECT_EQ(resultLocales.size(), 1);
    EXPECT_EQ(resultLocales[0], defaultLocale);
}

/**
 * @tc.name: DisplaynamesTest0002
 * @tc.desc: Test Intl DisplayNames.SupportedLocalesOf
 * @tc.type: FUNC
 */
HWTEST_F(DisplaynamesTest, DisplaynamesTest0002, TestSize.Level1)
{
    std::vector<std::string> localeTags = {"ban", "id-u-co-pinyin", "de-ID"};
    std::map<std::string, std::string> options = {{"localeMatcher", "lookup"}};
    I18nErrorCode status = I18nErrorCode::SUCCESS;
    std::vector<std::string> resultLocales = DisplayNames::SupportedLocalesOf(localeTags, options, status);
    EXPECT_EQ(status, I18nErrorCode::SUCCESS);
    EXPECT_EQ(resultLocales.size(), 2);
    EXPECT_EQ(resultLocales[0], "id-u-co-pinyin");
    EXPECT_EQ(resultLocales[1], "de-ID");
    std::map<std::string, std::string> optionsBogus = {{"localeMatcher", "bogus"}};
    std::vector<std::string> resultLocalesBogus = DisplayNames::SupportedLocalesOf(localeTags, optionsBogus, status);
    EXPECT_EQ(status, I18nErrorCode::INVALID_PARAM);
    EXPECT_EQ(resultLocalesBogus.size(), 0);
}

/**
 * @tc.name: DisplaynamesTest0003
 * @tc.desc: Test Intl DisplayNames.Display
 * @tc.type: FUNC
 */
HWTEST_F(DisplaynamesTest, DisplaynamesTest0003, TestSize.Level1)
{
    std::vector<std::string> localeTags = {"ban", "zxx", "zh"};
    std::map<std::string, std::string> options = {{"type", "calendar"}};
    DisplayNames displayNames(localeTags, options);
    I18nErrorCode status = displayNames.GetError();
    EXPECT_EQ(status, I18nErrorCode::SUCCESS);
    const std::vector<std::string> bogusCalendarCode = {"00", "000000000", "-00000000", "_00000000", " abcdef",
        "abcdef ", "abc def", "123_abc", "123_abc_ABC", "12345678_abcdefgh_ABCDEFGH"};
    std::string result = "";
    std::string errorMessage = "";
    for (const std::string& code : bogusCalendarCode) {
        result = displayNames.Display(code);
        status = displayNames.GetError();
        errorMessage = displayNames.GetErrorMessage();
        EXPECT_EQ(status, I18nErrorCode::INVALID_PARAM);
        EXPECT_EQ(errorMessage, "invalid calendar");
        EXPECT_EQ(result, "");
    }
    const std::vector<std::string> validCalendarCode = {"01234567", "899", "abcdefgh", "ijklmnop",
        "qrstuvwx", "yzz", "ABCDEFGH", "IJKLMNOP", "QRSTUVWX", "YZZ", "123-abc", "12345678-abcdefgh",
        "123-abc-ABC", "12345678-abcdefgh-ABCDEFGH"};
    for (const std::string& code : validCalendarCode) {
        result = displayNames.Display(code);
        status = displayNames.GetError();
        errorMessage = displayNames.GetErrorMessage();
        EXPECT_EQ(status, I18nErrorCode::SUCCESS);
        EXPECT_EQ(errorMessage, "");
        EXPECT_EQ(result, code);
    }
}

/**
 * @tc.name: DisplaynamesTest0004
 * @tc.desc: Test Intl DisplayNames.Display
 * @tc.type: FUNC
 */
HWTEST_F(DisplaynamesTest, DisplaynamesTest0004, TestSize.Level1)
{
    std::vector<std::string> localeTags = {"ban", "zxx", "en"};
    std::map<std::string, std::string> options = {{"type", "dateTimeField"}};
    DisplayNames displayNames(localeTags, options);
    I18nErrorCode status = displayNames.GetError();
    EXPECT_EQ(status, I18nErrorCode::SUCCESS);
    const std::vector<std::string> bogusCode = {"", "timezoneName", "timezonename",
        "millisecond", "seconds", " year", "year "};
    std::string result = "";
    std::string errorMessage = "";
    for (const std::string& code : bogusCode) {
        result = displayNames.Display(code);
        status = displayNames.GetError();
        errorMessage = displayNames.GetErrorMessage();
        EXPECT_EQ(status, I18nErrorCode::INVALID_PARAM);
        EXPECT_EQ(errorMessage, "invalid datetimefield");
        EXPECT_EQ(result, "");
    }
    std::vector<std::string> validCode = {"era", "year", "quarter", "month", "weekOfYear",
        "weekday", "day", "dayPeriod", "hour", "minute", "second", "timeZoneName"};
    std::vector<std::string> validCodeResult = {"era", "year", "quarter", "month", "week",
            "day of the week", "day", "AM/PM", "hour", "minute", "second", "time zone"};
    for (auto index = 0; index < validCode.size(); ++index) {
        result = displayNames.Display(validCode[index]);
        status = displayNames.GetError();
        errorMessage = displayNames.GetErrorMessage();
        EXPECT_EQ(status, I18nErrorCode::SUCCESS);
        EXPECT_EQ(errorMessage, "");
        EXPECT_EQ(result, validCodeResult[index]);
    }
}

/**
 * @tc.name: DisplaynamesTest0005
 * @tc.desc: Test Intl DisplayNames.ResolvedOptions deafault
 * @tc.type: FUNC
 */
HWTEST_F(DisplaynamesTest, DisplaynamesTest0005, TestSize.Level1)
{
    std::vector<std::string> localeTags = {"en-US"};
    std::map<std::string, std::string> options = {{"type", "language"}};
    DisplayNames displayNames(localeTags, options);
    I18nErrorCode status = displayNames.GetError();
    EXPECT_EQ(status, I18nErrorCode::SUCCESS);
    std::map<std::string, std::string> resolvedOptions = displayNames.ResolvedOptions();
    EXPECT_EQ(resolvedOptions["style"], "long");
    EXPECT_EQ(resolvedOptions["type"], "language");
    EXPECT_EQ(resolvedOptions["fallback"], "code");
}

/**
 * @tc.name: DisplaynamesTest0006
 * @tc.desc: Test Intl DisplayNames.ResolvedOptions fallback
 * @tc.type: FUNC
 */
HWTEST_F(DisplaynamesTest, DisplaynamesTest0006, TestSize.Level1)
{
    std::vector<std::string> localeTags = {"en-US"};
    const std::vector<std::string> fallbackOptions = { "code", "none" };
    for (auto fallback : fallbackOptions) {
        std::map<std::string, std::string> options = {{"type", "language"}, { "fallback", fallback }};
        DisplayNames displayNames(localeTags, options);
        I18nErrorCode status = displayNames.GetError();
        EXPECT_EQ(status, I18nErrorCode::SUCCESS);
        std::map<std::string, std::string> resolvedOptions = displayNames.ResolvedOptions();
        EXPECT_EQ(resolvedOptions["style"], "long");
        EXPECT_EQ(resolvedOptions["type"], "language");
        EXPECT_EQ(resolvedOptions["fallback"], fallback);
    }
    std::map<std::string, std::string> bogusOptions = {{"type", "language"}, { "fallback", "bogus" }};
    DisplayNames displayNames(localeTags, bogusOptions);
    I18nErrorCode status = displayNames.GetError();
    EXPECT_EQ(status, I18nErrorCode::INVALID_PARAM);
}

/**
 * @tc.name: DisplaynamesTest0007
 * @tc.desc: Test Intl DisplayNames.ResolvedOptions languageDisplay
 * @tc.type: FUNC
 */
HWTEST_F(DisplaynamesTest, DisplaynamesTest0007, TestSize.Level1)
{
    std::vector<std::string> localeTags = {"en-US"};
    const std::vector<std::string> languageDisplayOptions = { "dialect", "standard" };
    for (auto languageDisplay : languageDisplayOptions) {
        std::map<std::string, std::string> options = {{"type", "language"}, { "languageDisplay", languageDisplay }};
        DisplayNames displayNames(localeTags, options);
        I18nErrorCode status = displayNames.GetError();
        EXPECT_EQ(status, I18nErrorCode::SUCCESS);
        std::map<std::string, std::string> resolvedOptions = displayNames.ResolvedOptions();
        EXPECT_EQ(resolvedOptions["languageDisplay"], languageDisplay);
    }
    std::map<std::string, std::string> bogusOptions = {{"type", "language"}, { "languageDisplay", "bogus" }};
    DisplayNames displayNames(localeTags, bogusOptions);
    I18nErrorCode status = displayNames.GetError();
    EXPECT_EQ(status, I18nErrorCode::INVALID_PARAM);
}

/**
 * @tc.name: DisplaynamesTest0008
 * @tc.desc: Test Intl DisplayNames.ResolvedOptions style
 * @tc.type: FUNC
 */
HWTEST_F(DisplaynamesTest, DisplaynamesTest0008, TestSize.Level1)
{
    std::vector<std::string> localeTags = {"en-US"};
    const std::vector<std::string> styleOptions = { "narrow", "short", "long" };
    for (auto style : styleOptions) {
        std::map<std::string, std::string> options = {{"type", "language"}, { "style", style }};
        DisplayNames displayNames(localeTags, options);
        I18nErrorCode status = displayNames.GetError();
        EXPECT_EQ(status, I18nErrorCode::SUCCESS);
        std::map<std::string, std::string> resolvedOptions = displayNames.ResolvedOptions();
        EXPECT_EQ(resolvedOptions["style"], style);
    }
    std::map<std::string, std::string> bogusOptions = {{"type", "language"}, { "style", "bogus" }};
    DisplayNames displayNames(localeTags, bogusOptions);
    I18nErrorCode status = displayNames.GetError();
    EXPECT_EQ(status, I18nErrorCode::INVALID_PARAM);
}

/**
 * @tc.name: DisplaynamesTest0009
 * @tc.desc: Test Intl DisplayNames.ResolvedOptions type
 * @tc.type: FUNC
 */
HWTEST_F(DisplaynamesTest, DisplaynamesTest0009, TestSize.Level1)
{
    std::vector<std::string> localeTags = {"en-US"};
    const std::vector<std::string> typeOptions = { "language", "region", "script", "currency",
        "calendar", "dateTimeField" };
    for (auto type : typeOptions) {
        std::map<std::string, std::string> options = {{"type", type}};
        DisplayNames displayNames(localeTags, options);
        I18nErrorCode status = displayNames.GetError();
        EXPECT_EQ(status, I18nErrorCode::SUCCESS);
        std::map<std::string, std::string> resolvedOptions = displayNames.ResolvedOptions();
        EXPECT_EQ(resolvedOptions["type"], type);
    }
    std::map<std::string, std::string> bogusOptions = {{"type", "bogus"}};
    DisplayNames displayNames(localeTags, bogusOptions);
    I18nErrorCode status = displayNames.GetError();
    EXPECT_EQ(status, I18nErrorCode::INVALID_PARAM);

    std::map<std::string, std::string> missOptions = {{"localeMatcher", "lookup"}};
    DisplayNames missTypeDisplayNames(localeTags, missOptions);
    status = missTypeDisplayNames.GetError();
    EXPECT_EQ(status, I18nErrorCode::MISSING_PARAM);
}
} // namespace I18n
} // namespace Global
} // namespace OHOS