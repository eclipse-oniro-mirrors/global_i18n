/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "number_format_test.h"

#include "locale_config.h"
#include "number_format.h"
#include "simple_number_format.h"
#include "styled_number_format.h"
#include "unicode/locid.h"
#include "utils.h"

using namespace OHOS::Global::I18n;
using testing::ext::TestSize;
using namespace std;

namespace OHOS {
namespace Global {
namespace I18n {
string NumberFormatTest::originalLanguage;
string NumberFormatTest::originalRegion;
string NumberFormatTest::originalLocale;
string NumberFormatTest::deviceType;

void NumberFormatTest::SetUpTestCase(void)
{
    originalLanguage = LocaleConfig::GetSystemLanguage();
    originalRegion = LocaleConfig::GetSystemRegion();
    originalLocale = LocaleConfig::GetSystemLocale();
    LocaleConfig::SetSystemLanguage("zh-Hans");
    LocaleConfig::SetSystemRegion("CN");
    LocaleConfig::SetSystemLocale("zh-Hans-CN");
    NumberFormatTest::deviceType = ReadSystemParameter("const.product.devicetype", LocaleConfig::CONFIG_LEN);
}

void NumberFormatTest::TearDownTestCase(void)
{
    LocaleConfig::SetSystemLanguage(originalLanguage);
    LocaleConfig::SetSystemRegion(originalRegion);
    LocaleConfig::SetSystemLocale(originalLocale);
}

void NumberFormatTest::SetUp(void)
{}

void NumberFormatTest::TearDown(void)
{}

/**
 * @tc.name: NumberFormatFuncTest001
 * @tc.desc: Test Intl NumberFormat.format
 * @tc.type: FUNC
 */
HWTEST_F(NumberFormatTest, NumberFormatFuncTest001, TestSize.Level1)
{
    string locale = "en-IN";
    string expects = "+1,23,456.79 euros";
    vector<string> locales{locale};
    string useGrouping = "true";
    string minimumIntegerDigits = "7";
    string maximumFractionDigits = "2";
    string style = "currency";
    string currency = "978";
    map<string, string> options = { { "useGrouping", useGrouping },
                                    { "style", style },
                                    { "currency", currency },
                                    { "currencyDisplay", "name" },
                                    { "currencySign", "accounting" },
                                    { "signDisplay", "always" } };
    std::unique_ptr<NumberFormat> numFmt = std::make_unique<NumberFormat>(locales, options);
    ASSERT_TRUE(numFmt != nullptr);
    string out = numFmt->Format(123456.789);
    EXPECT_EQ(out, expects);
    EXPECT_EQ(numFmt->GetUseGrouping(), useGrouping);
    EXPECT_EQ(numFmt->GetStyle(), style);
    EXPECT_EQ(numFmt->GetCurrency(), "EUR");
}

/**
 * @tc.name: NumberFormatFuncTest002
 * @tc.desc: Test Intl NumberFormat.format
 * @tc.type: FUNC
 */
HWTEST_F(NumberFormatTest, NumberFormatFuncTest002, TestSize.Level1)
{
    string locale = "en-IN";
    string expects = "+1,23,456.789";
    vector<string> locales{locale};
    string useGrouping = "true";
    string minimumIntegerDigits = "7";
    string maximumFractionDigits = "2";
    string style = "currency";
    string currency = "111";
    map<string, string> options = { { "useGrouping", useGrouping },
                                    { "style", style },
                                    { "currency", currency },
                                    { "currencyDisplay", "name" },
                                    { "currencySign", "accounting" },
                                    { "signDisplay", "always" } };
    std::unique_ptr<NumberFormat> numFmt = std::make_unique<NumberFormat>(locales, options);
    ASSERT_TRUE(numFmt != nullptr);
    string out = numFmt->Format(123456.789);
    EXPECT_EQ(out, expects);
    EXPECT_EQ(numFmt->GetUseGrouping(), useGrouping);
    EXPECT_EQ(numFmt->GetStyle(), style);
    EXPECT_EQ(numFmt->GetCurrency(), "");
}

/**
 * @tc.name: NumberFormatFuncTest003
 * @tc.desc: Test Intl NumberFormat.format
 * @tc.type: FUNC
 */
HWTEST_F(NumberFormatTest, NumberFormatFuncTest003, TestSize.Level1)
{
    string locale = "en-IN";
    string expects = "+1,23,456.789";
    vector<string> locales{locale};
    string useGrouping = "true";
    string minimumIntegerDigits = "7";
    string maximumFractionDigits = "2";
    string style = "currency";
    string currency = "a1b";
    map<string, string> options = { { "useGrouping", useGrouping },
                                    { "style", style },
                                    { "currency", currency },
                                    { "currencyDisplay", "name" },
                                    { "currencySign", "accounting" },
                                    { "signDisplay", "always" } };
    std::unique_ptr<NumberFormat> numFmt = std::make_unique<NumberFormat>(locales, options);
    ASSERT_TRUE(numFmt != nullptr);
    string out = numFmt->Format(123456.789);
    EXPECT_EQ(out, expects);
    EXPECT_EQ(numFmt->GetUseGrouping(), useGrouping);
    EXPECT_EQ(numFmt->GetStyle(), style);
    EXPECT_EQ(numFmt->GetCurrency(), "");
}

/**
 * @tc.name: NumberFormatFuncTest004
 * @tc.desc: Test Intl NumberFormat.format
 * @tc.type: FUNC
 */
HWTEST_F(NumberFormatTest, NumberFormatFuncTest004, TestSize.Level1)
{
    string locale = "en-IN";
    string expects = "+1,23,456.789";
    vector<string> locales{locale};
    string useGrouping = "true";
    string minimumIntegerDigits = "7";
    string maximumFractionDigits = "2";
    string style = "currency";
    string currency = "a#b";
    map<string, string> options = { { "useGrouping", useGrouping },
                                    { "style", style },
                                    { "currency", currency },
                                    { "currencyDisplay", "name" },
                                    { "currencySign", "accounting" },
                                    { "signDisplay", "always" } };
    std::unique_ptr<NumberFormat> numFmt = std::make_unique<NumberFormat>(locales, options);
    ASSERT_TRUE(numFmt != nullptr);
    string out = numFmt->Format(123456.789);
    EXPECT_EQ(out, expects);
    EXPECT_EQ(numFmt->GetUseGrouping(), useGrouping);
    EXPECT_EQ(numFmt->GetStyle(), style);
    EXPECT_EQ(numFmt->GetCurrency(), "");
}

/**
 * @tc.name: NumberFormatFuncTest005
 * @tc.desc: Test Intl NumberFormat.format
 * @tc.type: FUNC
 */
HWTEST_F(NumberFormatTest, NumberFormatFuncTest005, TestSize.Level1)
{
    string locale = "en-IN";
    string expects = "+1,23,456.789";
    vector<string> locales{locale};
    string useGrouping = "true";
    string minimumIntegerDigits = "7";
    string maximumFractionDigits = "2";
    string style = "currency";
    string currency = "ab";
    map<string, string> options = { { "useGrouping", useGrouping },
                                    { "style", style },
                                    { "currency", currency },
                                    { "currencyDisplay", "name" },
                                    { "currencySign", "accounting" },
                                    { "signDisplay", "always" } };
    std::unique_ptr<NumberFormat> numFmt = std::make_unique<NumberFormat>(locales, options);
    ASSERT_TRUE(numFmt != nullptr);
    string out = numFmt->Format(123456.789);
    EXPECT_EQ(out, expects);
    EXPECT_EQ(numFmt->GetUseGrouping(), useGrouping);
    EXPECT_EQ(numFmt->GetStyle(), style);
    EXPECT_EQ(numFmt->GetCurrency(), "");
}

/**
 * @tc.name: NumberFormatFuncTest006
 * @tc.desc: Test Intl NumberFormat.format
 * @tc.type: FUNC
 */
HWTEST_F(NumberFormatTest, NumberFormatFuncTest006, TestSize.Level1)
{
    string locale = "en-IN";
    string expects = "+1,23,456.789";
    vector<string> locales{locale};
    string useGrouping = "true";
    string minimumIntegerDigits = "7";
    string maximumFractionDigits = "2";
    string style = "currency";
    string currency = "abcd";
    map<string, string> options = { { "useGrouping", useGrouping },
                                    { "style", style },
                                    { "currency", currency },
                                    { "currencyDisplay", "name" },
                                    { "currencySign", "accounting" },
                                    { "signDisplay", "always" } };
    std::unique_ptr<NumberFormat> numFmt = std::make_unique<NumberFormat>(locales, options);
    ASSERT_TRUE(numFmt != nullptr);
    string out = numFmt->Format(123456.789);
    EXPECT_EQ(out, expects);
    EXPECT_EQ(numFmt->GetUseGrouping(), useGrouping);
    EXPECT_EQ(numFmt->GetStyle(), style);
    EXPECT_EQ(numFmt->GetCurrency(), "");
}

/**
 * @tc.name: NumberFormatFuncTest007
 * @tc.desc: Test Intl NumberFormat.format
 * @tc.type: FUNC
 */
HWTEST_F(NumberFormatTest, NumberFormatFuncTest007, TestSize.Level1)
{
    string localeCN = "zh-CN";
    vector<string> localesCN{localeCN};
    string localeUS = "en-US";
    vector<string> localesUS{localeUS};
    string localeGB = "en-GB";
    vector<string> localesGB{localeGB};
    string localeBO = "bo";
    vector<string> localesBO{localeBO};
    string localeUG = "ug";
    vector<string> localesUG{localeUG};
    string localeHK = "zh-HK";
    vector<string> localesHK{localeHK};
    string localeTW = "zh-TW";
    vector<string> localesTW{localeTW};
    string style = "unit";
    string unit = "beat-per-minute";
    string unitStyle = "long";
    map<string, string> options = { { "style", style},
                                    { "unit", unit },
                                    { "unitStyle", unitStyle } };
    std::unique_ptr<NumberFormat> numFmtCN = std::make_unique<NumberFormat>(localesCN, options);
    ASSERT_TRUE(numFmtCN != nullptr);
    EXPECT_EQ(numFmtCN->Format(1234567.89), "1,234,567.89 次/分钟");
    std::unique_ptr<NumberFormat> numFmtUS = std::make_unique<NumberFormat>(localesUS, options);
    ASSERT_TRUE(numFmtUS != nullptr);
    EXPECT_EQ(numFmtUS->Format(1), "1 bpm");
    EXPECT_EQ(numFmtUS->Format(1234567.89), "1,234,567.89 bpm");
    std::unique_ptr<NumberFormat> numFmtGB = std::make_unique<NumberFormat>(localesGB, options);
    ASSERT_TRUE(numFmtGB != nullptr);
    EXPECT_EQ(numFmtGB->Format(1), "1 bpm");
    EXPECT_EQ(numFmtGB->Format(1234567.89), "1,234,567.89 bpm");
    std::unique_ptr<NumberFormat> numFmtBO = std::make_unique<NumberFormat>(localesBO, options);
    ASSERT_TRUE(numFmtBO != nullptr);
    EXPECT_EQ(numFmtBO->Format(1234567.89), "སྐར་མ་རེར་ཐེངས་ 1,234,567.89");
    std::unique_ptr<NumberFormat> numFmtUG = std::make_unique<NumberFormat>(localesUG, options);
    ASSERT_TRUE(numFmtUG != nullptr);
    EXPECT_EQ(numFmtUG->Format(1), "1 ق/م");
    EXPECT_EQ(numFmtUG->Format(1234567.89), "1,234,567.89 ق/م");
    std::unique_ptr<NumberFormat> numFmtHK = std::make_unique<NumberFormat>(localesHK, options);
    ASSERT_TRUE(numFmtHK != nullptr);
    EXPECT_EQ(numFmtHK->Format(1234567.89), "1,234,567.89 下/分鐘");
    std::unique_ptr<NumberFormat> numFmtTW = std::make_unique<NumberFormat>(localesTW, options);
    ASSERT_TRUE(numFmtTW != nullptr);
    EXPECT_EQ(numFmtTW->Format(1234567.89), "1,234,567.89 次/分鐘");
}

/**
 * @tc.name: NumberFormatFuncTest008
 * @tc.desc: Test Intl NumberFormat.format
 * @tc.type: FUNC
 */
HWTEST_F(NumberFormatTest, NumberFormatFuncTest008, TestSize.Level1)
{
    string localeCN = "zh-CN";
    vector<string> localesCN{localeCN};
    string localeUS = "en-US";
    vector<string> localesUS{localeUS};
    string localeGB = "en-GB";
    vector<string> localesGB{localeGB};
    string localeBO = "bo";
    vector<string> localesBO{localeBO};
    string localeUG = "ug";
    vector<string> localesUG{localeUG};
    string localeHK = "zh-HK";
    vector<string> localesHK{localeHK};
    string localeTW = "zh-TW";
    vector<string> localesTW{localeTW};
    string style = "unit";
    string unit = "beat-per-minute";
    string unitStyle = "short";
    map<string, string> options = { { "style", style},
                                    { "unit", unit },
                                    { "unitStyle", unitStyle } };
    std::unique_ptr<NumberFormat> numFmtCN = std::make_unique<NumberFormat>(localesCN, options);
    ASSERT_TRUE(numFmtCN != nullptr);
    EXPECT_EQ(numFmtCN->Format(1234567.89), "1,234,567.89 次/分钟");
    std::unique_ptr<NumberFormat> numFmtUS = std::make_unique<NumberFormat>(localesUS, options);
    ASSERT_TRUE(numFmtUS != nullptr);
    EXPECT_EQ(numFmtUS->Format(1), "1 bpm");
    EXPECT_EQ(numFmtUS->Format(1234567.89), "1,234,567.89 bpm");
    std::unique_ptr<NumberFormat> numFmtGB = std::make_unique<NumberFormat>(localesGB, options);
    ASSERT_TRUE(numFmtGB != nullptr);
    EXPECT_EQ(numFmtGB->Format(1), "1 bpm");
    EXPECT_EQ(numFmtGB->Format(1234567.89), "1,234,567.89 bpm");
    std::unique_ptr<NumberFormat> numFmtBO = std::make_unique<NumberFormat>(localesBO, options);
    ASSERT_TRUE(numFmtBO != nullptr);
    EXPECT_EQ(numFmtBO->Format(1234567.89), "སྐར་མ་རེར་ཐེངས་ 1,234,567.89");
    std::unique_ptr<NumberFormat> numFmtUG = std::make_unique<NumberFormat>(localesUG, options);
    ASSERT_TRUE(numFmtUG != nullptr);
    EXPECT_EQ(numFmtUG->Format(1), "1 ق/م");
    EXPECT_EQ(numFmtUG->Format(1234567.89), "1,234,567.89 ق/م");
    std::unique_ptr<NumberFormat> numFmtHK = std::make_unique<NumberFormat>(localesHK, options);
    ASSERT_TRUE(numFmtHK != nullptr);
    EXPECT_EQ(numFmtHK->Format(1234567.89), "1,234,567.89 下/分鐘");
    std::unique_ptr<NumberFormat> numFmtTW = std::make_unique<NumberFormat>(localesTW, options);
    ASSERT_TRUE(numFmtTW != nullptr);
    EXPECT_EQ(numFmtTW->Format(1234567.89), "1,234,567.89 次/分鐘");
}

/**
 * @tc.name: NumberFormatFuncTest009
 * @tc.desc: Test Intl NumberFormat.format
 * @tc.type: FUNC
 */
HWTEST_F(NumberFormatTest, NumberFormatFuncTest009, TestSize.Level1)
{
    string localeCN = "zh-CN";
    vector<string> localesCN{localeCN};
    string localeUS = "en-US";
    vector<string> localesUS{localeUS};
    string localeGB = "en-GB";
    vector<string> localesGB{localeGB};
    string localeBO = "bo";
    vector<string> localesBO{localeBO};
    string localeUG = "ug";
    vector<string> localesUG{localeUG};
    string localeHK = "zh-HK";
    vector<string> localesHK{localeHK};
    string localeTW = "zh-TW";
    vector<string> localesTW{localeTW};
    string style = "unit";
    string unit = "beat-per-minute";
    string unitStyle = "narrow";
    map<string, string> options = { { "style", style},
                                    { "unit", unit },
                                    { "unitStyle", unitStyle } };
    std::unique_ptr<NumberFormat> numFmtCN = std::make_unique<NumberFormat>(localesCN, options);
    ASSERT_TRUE(numFmtCN != nullptr);
    EXPECT_EQ(numFmtCN->Format(1234567.89), "1,234,567.89 次/分钟");
    std::unique_ptr<NumberFormat> numFmtUS = std::make_unique<NumberFormat>(localesUS, options);
    ASSERT_TRUE(numFmtUS != nullptr);
    EXPECT_EQ(numFmtUS->Format(1), "1 bpm");
    EXPECT_EQ(numFmtUS->Format(1234567.89), "1,234,567.89 bpm");
    std::unique_ptr<NumberFormat> numFmtGB = std::make_unique<NumberFormat>(localesGB, options);
    ASSERT_TRUE(numFmtGB != nullptr);
    EXPECT_EQ(numFmtGB->Format(1), "1 bpm");
    EXPECT_EQ(numFmtGB->Format(1234567.89), "1,234,567.89 bpm");
    std::unique_ptr<NumberFormat> numFmtBO = std::make_unique<NumberFormat>(localesBO, options);
    ASSERT_TRUE(numFmtBO != nullptr);
    EXPECT_EQ(numFmtBO->Format(1234567.89), "སྐར་མ་རེར་ཐེངས་ 1,234,567.89");
    std::unique_ptr<NumberFormat> numFmtUG = std::make_unique<NumberFormat>(localesUG, options);
    ASSERT_TRUE(numFmtUG != nullptr);
    EXPECT_EQ(numFmtUG->Format(1), "1 ق/م");
    EXPECT_EQ(numFmtUG->Format(1234567.89), "1,234,567.89 ق/م");
    std::unique_ptr<NumberFormat> numFmtHK = std::make_unique<NumberFormat>(localesHK, options);
    ASSERT_TRUE(numFmtHK != nullptr);
    EXPECT_EQ(numFmtHK->Format(1234567.89), "1,234,567.89 下/分鐘");
    std::unique_ptr<NumberFormat> numFmtTW = std::make_unique<NumberFormat>(localesTW, options);
    ASSERT_TRUE(numFmtTW != nullptr);
    EXPECT_EQ(numFmtTW->Format(1234567.89), "1,234,567.89 次/分鐘");
}

/**
 * @tc.name: NumberFormatFuncTest0010
 * @tc.desc: Test Intl NumberFormat.format
 * @tc.type: FUNC
 */
HWTEST_F(NumberFormatTest, NumberFormatFuncTest0010, TestSize.Level1)
{
    string localeCN = "zh-CN";
    vector<string> localesCN{localeCN};
    string localeUS = "en-US";
    vector<string> localesUS{localeUS};
    string localeGB = "en-GB";
    vector<string> localesGB{localeGB};
    string localeBO = "bo";
    vector<string> localesBO{localeBO};
    string localeUG = "ug";
    vector<string> localesUG{localeUG};
    string localeHK = "zh-HK";
    vector<string> localesHK{localeHK};
    string localeTW = "zh-TW";
    vector<string> localesTW{localeTW};
    string style = "unit";
    string unit = "body-weight-per-second";
    string unitStyle = "long";
    map<string, string> options = { { "style", style},
                                    { "unit", unit },
                                    { "unitStyle", unitStyle } };
    std::unique_ptr<NumberFormat> numFmtCN = std::make_unique<NumberFormat>(localesCN, options);
    ASSERT_TRUE(numFmtCN != nullptr);
    EXPECT_EQ(numFmtCN->Format(1234567.89), "1,234,567.89 BW/s");
    std::unique_ptr<NumberFormat> numFmtUS = std::make_unique<NumberFormat>(localesUS, options);
    ASSERT_TRUE(numFmtUS != nullptr);
    EXPECT_EQ(numFmtUS->Format(1), "1 BW/s");
    EXPECT_EQ(numFmtUS->Format(1234567.89), "1,234,567.89 BW/s");
    std::unique_ptr<NumberFormat> numFmtGB = std::make_unique<NumberFormat>(localesGB, options);
    ASSERT_TRUE(numFmtGB != nullptr);
    EXPECT_EQ(numFmtGB->Format(1), "1 BW/s");
    EXPECT_EQ(numFmtGB->Format(1234567.89), "1,234,567.89 BW/s");
    std::unique_ptr<NumberFormat> numFmtBO = std::make_unique<NumberFormat>(localesBO, options);
    ASSERT_TRUE(numFmtBO != nullptr);
    EXPECT_EQ(numFmtBO->Format(1234567.89), "1,234,567.89 BW/s");
    std::unique_ptr<NumberFormat> numFmtUG = std::make_unique<NumberFormat>(localesUG, options);
    ASSERT_TRUE(numFmtUG != nullptr);
    EXPECT_EQ(numFmtUG->Format(1), "1 BW/s");
    EXPECT_EQ(numFmtUG->Format(1234567.89), "1,234,567.89 BW/s");
    std::unique_ptr<NumberFormat> numFmtHK = std::make_unique<NumberFormat>(localesHK, options);
    ASSERT_TRUE(numFmtHK != nullptr);
    EXPECT_EQ(numFmtHK->Format(1234567.89), "1,234,567.89 體重/秒");
    std::unique_ptr<NumberFormat> numFmtTW = std::make_unique<NumberFormat>(localesTW, options);
    ASSERT_TRUE(numFmtTW != nullptr);
    EXPECT_EQ(numFmtTW->Format(1234567.89), "1,234,567.89 BW/s");
}

/**
 * @tc.name: NumberFormatFuncTest0011
 * @tc.desc: Test Intl NumberFormat.format
 * @tc.type: FUNC
 */
HWTEST_F(NumberFormatTest, NumberFormatFuncTest0011, TestSize.Level1)
{
    string localeCN = "zh-CN";
    vector<string> localesCN{localeCN};
    string localeUS = "en-US";
    vector<string> localesUS{localeUS};
    string localeGB = "en-GB";
    vector<string> localesGB{localeGB};
    string localeBO = "bo";
    vector<string> localesBO{localeBO};
    string localeUG = "ug";
    vector<string> localesUG{localeUG};
    string localeHK = "zh-HK";
    vector<string> localesHK{localeHK};
    string localeTW = "zh-TW";
    vector<string> localesTW{localeTW};
    string style = "unit";
    string unit = "body-weight-per-second";
    string unitStyle = "short";
    map<string, string> options = { { "style", style},
                                    { "unit", unit },
                                    { "unitStyle", unitStyle } };
    std::unique_ptr<NumberFormat> numFmtCN = std::make_unique<NumberFormat>(localesCN, options);
    ASSERT_TRUE(numFmtCN != nullptr);
    EXPECT_EQ(numFmtCN->Format(1234567.89), "1,234,567.89 BW/s");
    std::unique_ptr<NumberFormat> numFmtUS = std::make_unique<NumberFormat>(localesUS, options);
    ASSERT_TRUE(numFmtUS != nullptr);
    EXPECT_EQ(numFmtUS->Format(1), "1 BW/s");
    EXPECT_EQ(numFmtUS->Format(1234567.89), "1,234,567.89 BW/s");
    std::unique_ptr<NumberFormat> numFmtGB = std::make_unique<NumberFormat>(localesGB, options);
    ASSERT_TRUE(numFmtGB != nullptr);
    EXPECT_EQ(numFmtGB->Format(1), "1 BW/s");
    EXPECT_EQ(numFmtGB->Format(1234567.89), "1,234,567.89 BW/s");
    std::unique_ptr<NumberFormat> numFmtBO = std::make_unique<NumberFormat>(localesBO, options);
    ASSERT_TRUE(numFmtBO != nullptr);
    EXPECT_EQ(numFmtBO->Format(1234567.89), "1,234,567.89 BW/s");
    std::unique_ptr<NumberFormat> numFmtUG = std::make_unique<NumberFormat>(localesUG, options);
    ASSERT_TRUE(numFmtUG != nullptr);
    EXPECT_EQ(numFmtUG->Format(1), "1 BW/s");
    EXPECT_EQ(numFmtUG->Format(1234567.89), "1,234,567.89 BW/s");
    std::unique_ptr<NumberFormat> numFmtHK = std::make_unique<NumberFormat>(localesHK, options);
    ASSERT_TRUE(numFmtHK != nullptr);
    EXPECT_EQ(numFmtHK->Format(1234567.89), "1,234,567.89 體重/秒");
    std::unique_ptr<NumberFormat> numFmtTW = std::make_unique<NumberFormat>(localesTW, options);
    ASSERT_TRUE(numFmtTW != nullptr);
    EXPECT_EQ(numFmtTW->Format(1234567.89), "1,234,567.89 BW/s");
}

/**
 * @tc.name: NumberFormatFuncTest0012
 * @tc.desc: Test Intl NumberFormat.format
 * @tc.type: FUNC
 */
HWTEST_F(NumberFormatTest, NumberFormatFuncTest0012, TestSize.Level1)
{
    string localeCN = "zh-CN";
    vector<string> localesCN{localeCN};
    string localeUS = "en-US";
    vector<string> localesUS{localeUS};
    string localeGB = "en-GB";
    vector<string> localesGB{localeGB};
    string localeBO = "bo";
    vector<string> localesBO{localeBO};
    string localeUG = "ug";
    vector<string> localesUG{localeUG};
    string localeHK = "zh-HK";
    vector<string> localesHK{localeHK};
    string localeTW = "zh-TW";
    vector<string> localesTW{localeTW};
    string style = "unit";
    string unit = "body-weight-per-second";
    string unitStyle = "narrow";
    map<string, string> options = { { "style", style},
                                    { "unit", unit },
                                    { "unitStyle", unitStyle } };
    std::unique_ptr<NumberFormat> numFmtCN = std::make_unique<NumberFormat>(localesCN, options);
    ASSERT_TRUE(numFmtCN != nullptr);
    EXPECT_EQ(numFmtCN->Format(1234567.89), "1,234,567.89 BW/s");
    std::unique_ptr<NumberFormat> numFmtUS = std::make_unique<NumberFormat>(localesUS, options);
    ASSERT_TRUE(numFmtUS != nullptr);
    EXPECT_EQ(numFmtUS->Format(1), "1 BW/s");
    EXPECT_EQ(numFmtUS->Format(1234567.89), "1,234,567.89 BW/s");
    std::unique_ptr<NumberFormat> numFmtGB = std::make_unique<NumberFormat>(localesGB, options);
    ASSERT_TRUE(numFmtGB != nullptr);
    EXPECT_EQ(numFmtGB->Format(1), "1 BW/s");
    EXPECT_EQ(numFmtGB->Format(1234567.89), "1,234,567.89 BW/s");
    std::unique_ptr<NumberFormat> numFmtBO = std::make_unique<NumberFormat>(localesBO, options);
    ASSERT_TRUE(numFmtBO != nullptr);
    EXPECT_EQ(numFmtBO->Format(1234567.89), "1,234,567.89 BW/s");
    std::unique_ptr<NumberFormat> numFmtUG = std::make_unique<NumberFormat>(localesUG, options);
    ASSERT_TRUE(numFmtUG != nullptr);
    EXPECT_EQ(numFmtUG->Format(1), "1 BW/s");
    EXPECT_EQ(numFmtUG->Format(1234567.89), "1,234,567.89 BW/s");
    std::unique_ptr<NumberFormat> numFmtHK = std::make_unique<NumberFormat>(localesHK, options);
    ASSERT_TRUE(numFmtHK != nullptr);
    EXPECT_EQ(numFmtHK->Format(1234567.89), "1,234,567.89 體重/秒");
    std::unique_ptr<NumberFormat> numFmtTW = std::make_unique<NumberFormat>(localesTW, options);
    ASSERT_TRUE(numFmtTW != nullptr);
    EXPECT_EQ(numFmtTW->Format(1234567.89), "1,234,567.89 BW/s");
}

/**
 * @tc.name: NumberFormatFuncTest0013
 * @tc.desc: Test Intl NumberFormat.format
 * @tc.type: FUNC
 */
HWTEST_F(NumberFormatTest, NumberFormatFuncTest0013, TestSize.Level1)
{
    string localeCN = "zh-CN";
    vector<string> localesCN{localeCN};
    string localeUS = "en-US";
    vector<string> localesUS{localeUS};
    string localeGB = "en-GB";
    vector<string> localesGB{localeGB};
    string localeBO = "bo";
    vector<string> localesBO{localeBO};
    string localeUG = "ug";
    vector<string> localesUG{localeUG};
    string localeHK = "zh-HK";
    vector<string> localesHK{localeHK};
    string localeTW = "zh-TW";
    vector<string> localesTW{localeTW};
    string style = "unit";
    string unit = "breath-per-minute";
    string unitStyle = "long";
    map<string, string> options = { { "style", style},
                                    { "unit", unit },
                                    { "unitStyle", unitStyle } };
    std::unique_ptr<NumberFormat> numFmtCN = std::make_unique<NumberFormat>(localesCN, options);
    ASSERT_TRUE(numFmtCN != nullptr);
    EXPECT_EQ(numFmtCN->Format(1234567.89), "1,234,567.89 次/分");
    std::unique_ptr<NumberFormat> numFmtUS = std::make_unique<NumberFormat>(localesUS, options);
    ASSERT_TRUE(numFmtUS != nullptr);
    EXPECT_EQ(numFmtUS->Format(1), "1 brpm");
    EXPECT_EQ(numFmtUS->Format(1234567.89), "1,234,567.89 brpm");
    std::unique_ptr<NumberFormat> numFmtGB = std::make_unique<NumberFormat>(localesGB, options);
    ASSERT_TRUE(numFmtGB != nullptr);
    EXPECT_EQ(numFmtGB->Format(1), "1 brpm");
    EXPECT_EQ(numFmtGB->Format(1234567.89), "1,234,567.89 brpm");
    std::unique_ptr<NumberFormat> numFmtBO = std::make_unique<NumberFormat>(localesBO, options);
    ASSERT_TRUE(numFmtBO != nullptr);
    EXPECT_EQ(numFmtBO->Format(1234567.89), "སྐར་མ་རེར་ཐེངས་ 1,234,567.89");
    std::unique_ptr<NumberFormat> numFmtUG = std::make_unique<NumberFormat>(localesUG, options);
    ASSERT_TRUE(numFmtUG != nullptr);
    EXPECT_EQ(numFmtUG->Format(1), "1 ق/م");
    EXPECT_EQ(numFmtUG->Format(1234567.89), "1,234,567.89 ق/م");
    std::unique_ptr<NumberFormat> numFmtHK = std::make_unique<NumberFormat>(localesHK, options);
    ASSERT_TRUE(numFmtHK != nullptr);
    EXPECT_EQ(numFmtHK->Format(1234567.89), "1,234,567.89 下/分鐘");
    std::unique_ptr<NumberFormat> numFmtTW = std::make_unique<NumberFormat>(localesTW, options);
    ASSERT_TRUE(numFmtTW != nullptr);
    EXPECT_EQ(numFmtTW->Format(1234567.89), "1,234,567.89 次/分");
}

/**
 * @tc.name: NumberFormatFuncTest0014
 * @tc.desc: Test Intl NumberFormat.format
 * @tc.type: FUNC
 */
HWTEST_F(NumberFormatTest, NumberFormatFuncTest0014, TestSize.Level1)
{
    string localeCN = "zh-CN";
    vector<string> localesCN{localeCN};
    string localeUS = "en-US";
    vector<string> localesUS{localeUS};
    string localeGB = "en-GB";
    vector<string> localesGB{localeGB};
    string localeBO = "bo";
    vector<string> localesBO{localeBO};
    string localeUG = "ug";
    vector<string> localesUG{localeUG};
    string localeHK = "zh-HK";
    vector<string> localesHK{localeHK};
    string localeTW = "zh-TW";
    vector<string> localesTW{localeTW};
    string style = "unit";
    string unit = "breath-per-minute";
    string unitStyle = "short";
    map<string, string> options = { { "style", style},
                                    { "unit", unit },
                                    { "unitStyle", unitStyle } };
    std::unique_ptr<NumberFormat> numFmtCN = std::make_unique<NumberFormat>(localesCN, options);
    ASSERT_TRUE(numFmtCN != nullptr);
    EXPECT_EQ(numFmtCN->Format(1234567.89), "1,234,567.89 次/分");
    std::unique_ptr<NumberFormat> numFmtUS = std::make_unique<NumberFormat>(localesUS, options);
    ASSERT_TRUE(numFmtUS != nullptr);
    EXPECT_EQ(numFmtUS->Format(1), "1 brpm");
    EXPECT_EQ(numFmtUS->Format(1234567.89), "1,234,567.89 brpm");
    std::unique_ptr<NumberFormat> numFmtGB = std::make_unique<NumberFormat>(localesGB, options);
    ASSERT_TRUE(numFmtGB != nullptr);
    EXPECT_EQ(numFmtGB->Format(1), "1 brpm");
    EXPECT_EQ(numFmtGB->Format(1234567.89), "1,234,567.89 brpm");
    std::unique_ptr<NumberFormat> numFmtBO = std::make_unique<NumberFormat>(localesBO, options);
    ASSERT_TRUE(numFmtBO != nullptr);
    EXPECT_EQ(numFmtBO->Format(1234567.89), "སྐར་མ་རེར་ཐེངས་ 1,234,567.89");
    std::unique_ptr<NumberFormat> numFmtUG = std::make_unique<NumberFormat>(localesUG, options);
    ASSERT_TRUE(numFmtUG != nullptr);
    EXPECT_EQ(numFmtUG->Format(1), "1 ق/م");
    EXPECT_EQ(numFmtUG->Format(1234567.89), "1,234,567.89 ق/م");
    std::unique_ptr<NumberFormat> numFmtHK = std::make_unique<NumberFormat>(localesHK, options);
    ASSERT_TRUE(numFmtHK != nullptr);
    EXPECT_EQ(numFmtHK->Format(1234567.89), "1,234,567.89 下/分鐘");
    std::unique_ptr<NumberFormat> numFmtTW = std::make_unique<NumberFormat>(localesTW, options);
    ASSERT_TRUE(numFmtTW != nullptr);
    EXPECT_EQ(numFmtTW->Format(1234567.89), "1,234,567.89 次/分");
}

/**
 * @tc.name: NumberFormatFuncTest0015
 * @tc.desc: Test Intl NumberFormat.format
 * @tc.type: FUNC
 */
HWTEST_F(NumberFormatTest, NumberFormatFuncTest0015, TestSize.Level1)
{
    string localeCN = "zh-CN";
    vector<string> localesCN{localeCN};
    string localeUS = "en-US";
    vector<string> localesUS{localeUS};
    string localeGB = "en-GB";
    vector<string> localesGB{localeGB};
    string localeBO = "bo";
    vector<string> localesBO{localeBO};
    string localeUG = "ug";
    vector<string> localesUG{localeUG};
    string localeHK = "zh-HK";
    vector<string> localesHK{localeHK};
    string localeTW = "zh-TW";
    vector<string> localesTW{localeTW};
    string style = "unit";
    string unit = "breath-per-minute";
    string unitStyle = "narrow";
    map<string, string> options = { { "style", style},
                                    { "unit", unit },
                                    { "unitStyle", unitStyle } };
    std::unique_ptr<NumberFormat> numFmtCN = std::make_unique<NumberFormat>(localesCN, options);
    ASSERT_TRUE(numFmtCN != nullptr);
    EXPECT_EQ(numFmtCN->Format(1234567.89), "1,234,567.89 次/分");
    std::unique_ptr<NumberFormat> numFmtUS = std::make_unique<NumberFormat>(localesUS, options);
    ASSERT_TRUE(numFmtUS != nullptr);
    EXPECT_EQ(numFmtUS->Format(1), "1 brpm");
    EXPECT_EQ(numFmtUS->Format(1234567.89), "1,234,567.89 brpm");
    std::unique_ptr<NumberFormat> numFmtGB = std::make_unique<NumberFormat>(localesGB, options);
    ASSERT_TRUE(numFmtGB != nullptr);
    EXPECT_EQ(numFmtGB->Format(1), "1 brpm");
    EXPECT_EQ(numFmtGB->Format(1234567.89), "1,234,567.89 brpm");
    std::unique_ptr<NumberFormat> numFmtBO = std::make_unique<NumberFormat>(localesBO, options);
    ASSERT_TRUE(numFmtBO != nullptr);
    EXPECT_EQ(numFmtBO->Format(1234567.89), "སྐར་མ་རེར་ཐེངས་ 1,234,567.89");
    std::unique_ptr<NumberFormat> numFmtUG = std::make_unique<NumberFormat>(localesUG, options);
    ASSERT_TRUE(numFmtUG != nullptr);
    EXPECT_EQ(numFmtUG->Format(1), "1 ق/م");
    EXPECT_EQ(numFmtUG->Format(1234567.89), "1,234,567.89 ق/م");
    std::unique_ptr<NumberFormat> numFmtHK = std::make_unique<NumberFormat>(localesHK, options);
    ASSERT_TRUE(numFmtHK != nullptr);
    EXPECT_EQ(numFmtHK->Format(1234567.89), "1,234,567.89 下/分鐘");
    std::unique_ptr<NumberFormat> numFmtTW = std::make_unique<NumberFormat>(localesTW, options);
    ASSERT_TRUE(numFmtTW != nullptr);
    EXPECT_EQ(numFmtTW->Format(1234567.89), "1,234,567.89 次/分");
}

/**
 * @tc.name: NumberFormatFuncTest0016
 * @tc.desc: Test Intl NumberFormat.format
 * @tc.type: FUNC
 */
HWTEST_F(NumberFormatTest, NumberFormatFuncTest0016, TestSize.Level1)
{
    string localeCN = "zh-CN";
    vector<string> localesCN{localeCN};
    string localeUS = "en-US";
    vector<string> localesUS{localeUS};
    string localeGB = "en-GB";
    vector<string> localesGB{localeGB};
    string localeBO = "bo";
    vector<string> localesBO{localeBO};
    string localeUG = "ug";
    vector<string> localesUG{localeUG};
    string localeHK = "zh-HK";
    vector<string> localesHK{localeHK};
    string localeTW = "zh-TW";
    vector<string> localesTW{localeTW};
    string style = "unit";
    string unit = "foot-per-hour";
    string unitStyle = "long";
    map<string, string> options = { { "style", style},
                                    { "unit", unit },
                                    { "unitStyle", unitStyle } };
    std::unique_ptr<NumberFormat> numFmtCN = std::make_unique<NumberFormat>(localesCN, options);
    ASSERT_TRUE(numFmtCN != nullptr);
    EXPECT_EQ(numFmtCN->Format(1234567.89), "1,234,567.89 英尺/小时");
    std::unique_ptr<NumberFormat> numFmtUS = std::make_unique<NumberFormat>(localesUS, options);
    ASSERT_TRUE(numFmtUS != nullptr);
    EXPECT_EQ(numFmtUS->Format(1), "1 ft/h");
    EXPECT_EQ(numFmtUS->Format(1234567.89), "1,234,567.89 ft/h");
    std::unique_ptr<NumberFormat> numFmtGB = std::make_unique<NumberFormat>(localesGB, options);
    ASSERT_TRUE(numFmtGB != nullptr);
    EXPECT_EQ(numFmtGB->Format(1), "1 ft/h");
    EXPECT_EQ(numFmtGB->Format(1234567.89), "1,234,567.89 ft/h");
    std::unique_ptr<NumberFormat> numFmtBO = std::make_unique<NumberFormat>(localesBO, options);
    ASSERT_TRUE(numFmtBO != nullptr);
    EXPECT_EQ(numFmtBO->Format(1234567.89), "1,234,567.89 ft/h");
    std::unique_ptr<NumberFormat> numFmtUG = std::make_unique<NumberFormat>(localesUG, options);
    ASSERT_TRUE(numFmtUG != nullptr);
    EXPECT_EQ(numFmtUG->Format(1), "1 ft/h");
    EXPECT_EQ(numFmtUG->Format(1234567.89), "1,234,567.89 ft/h");
    std::unique_ptr<NumberFormat> numFmtHK = std::make_unique<NumberFormat>(localesHK, options);
    ASSERT_TRUE(numFmtHK != nullptr);
    EXPECT_EQ(numFmtHK->Format(1234567.89), "1,234,567.89 呎/小時");
    std::unique_ptr<NumberFormat> numFmtTW = std::make_unique<NumberFormat>(localesTW, options);
    ASSERT_TRUE(numFmtTW != nullptr);
    EXPECT_EQ(numFmtTW->Format(1234567.89), "1,234,567.89 英尺/小時");
}

/**
 * @tc.name: NumberFormatFuncTest0017
 * @tc.desc: Test Intl NumberFormat.format
 * @tc.type: FUNC
 */
HWTEST_F(NumberFormatTest, NumberFormatFuncTest0017, TestSize.Level1)
{
    string localeCN = "zh-CN";
    vector<string> localesCN{localeCN};
    string localeUS = "en-US";
    vector<string> localesUS{localeUS};
    string localeGB = "en-GB";
    vector<string> localesGB{localeGB};
    string localeBO = "bo";
    vector<string> localesBO{localeBO};
    string localeUG = "ug";
    vector<string> localesUG{localeUG};
    string localeHK = "zh-HK";
    vector<string> localesHK{localeHK};
    string localeTW = "zh-TW";
    vector<string> localesTW{localeTW};
    string style = "unit";
    string unit = "foot-per-hour";
    string unitStyle = "short";
    map<string, string> options = { { "style", style},
                                    { "unit", unit },
                                    { "unitStyle", unitStyle } };
    std::unique_ptr<NumberFormat> numFmtCN = std::make_unique<NumberFormat>(localesCN, options);
    ASSERT_TRUE(numFmtCN != nullptr);
    EXPECT_EQ(numFmtCN->Format(1234567.89), "1,234,567.89 英尺/小时");
    std::unique_ptr<NumberFormat> numFmtUS = std::make_unique<NumberFormat>(localesUS, options);
    ASSERT_TRUE(numFmtUS != nullptr);
    EXPECT_EQ(numFmtUS->Format(1), "1 ft/h");
    EXPECT_EQ(numFmtUS->Format(1234567.89), "1,234,567.89 ft/h");
    std::unique_ptr<NumberFormat> numFmtGB = std::make_unique<NumberFormat>(localesGB, options);
    ASSERT_TRUE(numFmtGB != nullptr);
    EXPECT_EQ(numFmtGB->Format(1), "1 ft/h");
    EXPECT_EQ(numFmtGB->Format(1234567.89), "1,234,567.89 ft/h");
    std::unique_ptr<NumberFormat> numFmtBO = std::make_unique<NumberFormat>(localesBO, options);
    ASSERT_TRUE(numFmtBO != nullptr);
    EXPECT_EQ(numFmtBO->Format(1234567.89), "1,234,567.89 ft/h");
    std::unique_ptr<NumberFormat> numFmtUG = std::make_unique<NumberFormat>(localesUG, options);
    ASSERT_TRUE(numFmtUG != nullptr);
    EXPECT_EQ(numFmtUG->Format(1), "1 ft/h");
    EXPECT_EQ(numFmtUG->Format(1234567.89), "1,234,567.89 ft/h");
    std::unique_ptr<NumberFormat> numFmtHK = std::make_unique<NumberFormat>(localesHK, options);
    ASSERT_TRUE(numFmtHK != nullptr);
    EXPECT_EQ(numFmtHK->Format(1234567.89), "1,234,567.89 呎/小時");
    std::unique_ptr<NumberFormat> numFmtTW = std::make_unique<NumberFormat>(localesTW, options);
    ASSERT_TRUE(numFmtTW != nullptr);
    EXPECT_EQ(numFmtTW->Format(1234567.89), "1,234,567.89 英尺/小時");
}

/**
 * @tc.name: NumberFormatFuncTest0018
 * @tc.desc: Test Intl NumberFormat.format
 * @tc.type: FUNC
 */
HWTEST_F(NumberFormatTest, NumberFormatFuncTest0018, TestSize.Level1)
{
    string localeCN = "zh-CN";
    vector<string> localesCN{localeCN};
    string localeUS = "en-US";
    vector<string> localesUS{localeUS};
    string localeGB = "en-GB";
    vector<string> localesGB{localeGB};
    string localeBO = "bo";
    vector<string> localesBO{localeBO};
    string localeUG = "ug";
    vector<string> localesUG{localeUG};
    string localeHK = "zh-HK";
    vector<string> localesHK{localeHK};
    string localeTW = "zh-TW";
    vector<string> localesTW{localeTW};
    string style = "unit";
    string unit = "foot-per-hour";
    string unitStyle = "narrow";
    map<string, string> options = { { "style", style},
                                    { "unit", unit },
                                    { "unitStyle", unitStyle } };
    std::unique_ptr<NumberFormat> numFmtCN = std::make_unique<NumberFormat>(localesCN, options);
    ASSERT_TRUE(numFmtCN != nullptr);
    EXPECT_EQ(numFmtCN->Format(1234567.89), "1,234,567.89 英尺/小时");
    std::unique_ptr<NumberFormat> numFmtUS = std::make_unique<NumberFormat>(localesUS, options);
    ASSERT_TRUE(numFmtUS != nullptr);
    EXPECT_EQ(numFmtUS->Format(1), "1 ft/h");
    EXPECT_EQ(numFmtUS->Format(1234567.89), "1,234,567.89 ft/h");
    std::unique_ptr<NumberFormat> numFmtGB = std::make_unique<NumberFormat>(localesGB, options);
    ASSERT_TRUE(numFmtGB != nullptr);
    EXPECT_EQ(numFmtGB->Format(1), "1 ft/h");
    EXPECT_EQ(numFmtGB->Format(1234567.89), "1,234,567.89 ft/h");
    std::unique_ptr<NumberFormat> numFmtBO = std::make_unique<NumberFormat>(localesBO, options);
    ASSERT_TRUE(numFmtBO != nullptr);
    EXPECT_EQ(numFmtBO->Format(1234567.89), "1,234,567.89 ft/h");
    std::unique_ptr<NumberFormat> numFmtUG = std::make_unique<NumberFormat>(localesUG, options);
    ASSERT_TRUE(numFmtUG != nullptr);
    EXPECT_EQ(numFmtUG->Format(1), "1 ft/h");
    EXPECT_EQ(numFmtUG->Format(1234567.89), "1,234,567.89 ft/h");
    std::unique_ptr<NumberFormat> numFmtHK = std::make_unique<NumberFormat>(localesHK, options);
    ASSERT_TRUE(numFmtHK != nullptr);
    EXPECT_EQ(numFmtHK->Format(1234567.89), "1,234,567.89 呎/小時");
    std::unique_ptr<NumberFormat> numFmtTW = std::make_unique<NumberFormat>(localesTW, options);
    ASSERT_TRUE(numFmtTW != nullptr);
    EXPECT_EQ(numFmtTW->Format(1234567.89), "1,234,567.89 英尺/小時");
}

/**
 * @tc.name: NumberFormatFuncTest0019
 * @tc.desc: Test Intl NumberFormat.format
 * @tc.type: FUNC
 */
HWTEST_F(NumberFormatTest, NumberFormatFuncTest0019, TestSize.Level1)
{
    string localeCN = "zh-CN";
    vector<string> localesCN{localeCN};
    string localeUS = "en-US";
    vector<string> localesUS{localeUS};
    string localeGB = "en-GB";
    vector<string> localesGB{localeGB};
    string localeBO = "bo";
    vector<string> localesBO{localeBO};
    string localeUG = "ug";
    vector<string> localesUG{localeUG};
    string localeHK = "zh-HK";
    vector<string> localesHK{localeHK};
    string localeTW = "zh-TW";
    vector<string> localesTW{localeTW};
    string style = "unit";
    string unit = "jump-rope-per-minute";
    string unitStyle = "long";
    map<string, string> options = { { "style", style},
                                    { "unit", unit },
                                    { "unitStyle", unitStyle } };
    std::unique_ptr<NumberFormat> numFmtCN = std::make_unique<NumberFormat>(localesCN, options);
    ASSERT_TRUE(numFmtCN != nullptr);
    EXPECT_EQ(numFmtCN->Format(1234567.89), "1,234,567.89 个/分钟");
    std::unique_ptr<NumberFormat> numFmtUS = std::make_unique<NumberFormat>(localesUS, options);
    ASSERT_TRUE(numFmtUS != nullptr);
    EXPECT_EQ(numFmtUS->Format(1), "1 jump/minute");
    EXPECT_EQ(numFmtUS->Format(1234567.89), "1,234,567.89 jumps/minute");
    std::unique_ptr<NumberFormat> numFmtGB = std::make_unique<NumberFormat>(localesGB, options);
    ASSERT_TRUE(numFmtGB != nullptr);
    EXPECT_EQ(numFmtGB->Format(1), "1 skip/minute");
    EXPECT_EQ(numFmtGB->Format(1234567.89), "1,234,567.89 skips/minute");
    std::unique_ptr<NumberFormat> numFmtBO = std::make_unique<NumberFormat>(localesBO, options);
    ASSERT_TRUE(numFmtBO != nullptr);
    EXPECT_EQ(numFmtBO->Format(1234567.89), "སྐར་མ་རེར་ 1,234,567.89");
    std::unique_ptr<NumberFormat> numFmtUG = std::make_unique<NumberFormat>(localesUG, options);
    ASSERT_TRUE(numFmtUG != nullptr);
    EXPECT_EQ(numFmtUG->Format(1), "1 ئاتلام/مىنۇت");
    EXPECT_EQ(numFmtUG->Format(1234567.89), "1,234,567.89 ئاتلام/مىنۇت");
    std::unique_ptr<NumberFormat> numFmtHK = std::make_unique<NumberFormat>(localesHK, options);
    ASSERT_TRUE(numFmtHK != nullptr);
    EXPECT_EQ(numFmtHK->Format(1234567.89), "1,234,567.89 下/分鐘");
    std::unique_ptr<NumberFormat> numFmtTW = std::make_unique<NumberFormat>(localesTW, options);
    ASSERT_TRUE(numFmtTW != nullptr);
    EXPECT_EQ(numFmtTW->Format(1234567.89), "1,234,567.89 個/分鐘");
}

/**
 * @tc.name: NumberFormatFuncTest0020
 * @tc.desc: Test Intl NumberFormat.format
 * @tc.type: FUNC
 */
HWTEST_F(NumberFormatTest, NumberFormatFuncTest0020, TestSize.Level1)
{
    string localeCN = "zh-CN";
    vector<string> localesCN{localeCN};
    string localeUS = "en-US";
    vector<string> localesUS{localeUS};
    string localeGB = "en-GB";
    vector<string> localesGB{localeGB};
    string localeBO = "bo";
    vector<string> localesBO{localeBO};
    string localeUG = "ug";
    vector<string> localesUG{localeUG};
    string localeHK = "zh-HK";
    vector<string> localesHK{localeHK};
    string localeTW = "zh-TW";
    vector<string> localesTW{localeTW};
    string style = "unit";
    string unit = "jump-rope-per-minute";
    string unitStyle = "short";
    map<string, string> options = { { "style", style},
                                    { "unit", unit },
                                    { "unitStyle", unitStyle } };
    std::unique_ptr<NumberFormat> numFmtCN = std::make_unique<NumberFormat>(localesCN, options);
    ASSERT_TRUE(numFmtCN != nullptr);
    EXPECT_EQ(numFmtCN->Format(1234567.89), "1,234,567.89 个/分钟");
    std::unique_ptr<NumberFormat> numFmtUS = std::make_unique<NumberFormat>(localesUS, options);
    ASSERT_TRUE(numFmtUS != nullptr);
    EXPECT_EQ(numFmtUS->Format(1), "1 jump/minute");
    EXPECT_EQ(numFmtUS->Format(1234567.89), "1,234,567.89 jumps/minute");
    std::unique_ptr<NumberFormat> numFmtGB = std::make_unique<NumberFormat>(localesGB, options);
    ASSERT_TRUE(numFmtGB != nullptr);
    EXPECT_EQ(numFmtGB->Format(1), "1 skip/minute");
    EXPECT_EQ(numFmtGB->Format(1234567.89), "1,234,567.89 skips/minute");
    std::unique_ptr<NumberFormat> numFmtBO = std::make_unique<NumberFormat>(localesBO, options);
    ASSERT_TRUE(numFmtBO != nullptr);
    EXPECT_EQ(numFmtBO->Format(1234567.89), "སྐར་མ་རེར་ 1,234,567.89");
    std::unique_ptr<NumberFormat> numFmtUG = std::make_unique<NumberFormat>(localesUG, options);
    ASSERT_TRUE(numFmtUG != nullptr);
    EXPECT_EQ(numFmtUG->Format(1), "1 ئاتلام/مىنۇت");
    EXPECT_EQ(numFmtUG->Format(1234567.89), "1,234,567.89 ئاتلام/مىنۇت");
    std::unique_ptr<NumberFormat> numFmtHK = std::make_unique<NumberFormat>(localesHK, options);
    ASSERT_TRUE(numFmtHK != nullptr);
    EXPECT_EQ(numFmtHK->Format(1234567.89), "1,234,567.89 下/分鐘");
    std::unique_ptr<NumberFormat> numFmtTW = std::make_unique<NumberFormat>(localesTW, options);
    ASSERT_TRUE(numFmtTW != nullptr);
    EXPECT_EQ(numFmtTW->Format(1234567.89), "1,234,567.89 個/分鐘");
}

/**
 * @tc.name: NumberFormatFuncTest0021
 * @tc.desc: Test Intl NumberFormat.format
 * @tc.type: FUNC
 */
HWTEST_F(NumberFormatTest, NumberFormatFuncTest0021, TestSize.Level1)
{
    string localeCN = "zh-CN";
    vector<string> localesCN{localeCN};
    string localeUS = "en-US";
    vector<string> localesUS{localeUS};
    string localeGB = "en-GB";
    vector<string> localesGB{localeGB};
    string localeBO = "bo";
    vector<string> localesBO{localeBO};
    string localeUG = "ug";
    vector<string> localesUG{localeUG};
    string localeHK = "zh-HK";
    vector<string> localesHK{localeHK};
    string localeTW = "zh-TW";
    vector<string> localesTW{localeTW};
    string style = "unit";
    string unit = "jump-rope-per-minute";
    string unitStyle = "narrow";
    map<string, string> options = { { "style", style},
                                    { "unit", unit },
                                    { "unitStyle", unitStyle } };
    std::unique_ptr<NumberFormat> numFmtCN = std::make_unique<NumberFormat>(localesCN, options);
    ASSERT_TRUE(numFmtCN != nullptr);
    EXPECT_EQ(numFmtCN->Format(1234567.89), "1,234,567.89 个/分钟");
    std::unique_ptr<NumberFormat> numFmtUS = std::make_unique<NumberFormat>(localesUS, options);
    ASSERT_TRUE(numFmtUS != nullptr);
    EXPECT_EQ(numFmtUS->Format(1), "1 jump/minute");
    EXPECT_EQ(numFmtUS->Format(1234567.89), "1,234,567.89 jumps/minute");
    std::unique_ptr<NumberFormat> numFmtGB = std::make_unique<NumberFormat>(localesGB, options);
    ASSERT_TRUE(numFmtGB != nullptr);
    EXPECT_EQ(numFmtGB->Format(1), "1 skip/minute");
    EXPECT_EQ(numFmtGB->Format(1234567.89), "1,234,567.89 skips/minute");
    std::unique_ptr<NumberFormat> numFmtBO = std::make_unique<NumberFormat>(localesBO, options);
    ASSERT_TRUE(numFmtBO != nullptr);
    EXPECT_EQ(numFmtBO->Format(1234567.89), "སྐར་མ་རེར་ 1,234,567.89");
    std::unique_ptr<NumberFormat> numFmtUG = std::make_unique<NumberFormat>(localesUG, options);
    ASSERT_TRUE(numFmtUG != nullptr);
    EXPECT_EQ(numFmtUG->Format(1), "1 ئاتلام/مىنۇت");
    EXPECT_EQ(numFmtUG->Format(1234567.89), "1,234,567.89 ئاتلام/مىنۇت");
    std::unique_ptr<NumberFormat> numFmtHK = std::make_unique<NumberFormat>(localesHK, options);
    ASSERT_TRUE(numFmtHK != nullptr);
    EXPECT_EQ(numFmtHK->Format(1234567.89), "1,234,567.89 下/分鐘");
    std::unique_ptr<NumberFormat> numFmtTW = std::make_unique<NumberFormat>(localesTW, options);
    ASSERT_TRUE(numFmtTW != nullptr);
    EXPECT_EQ(numFmtTW->Format(1234567.89), "1,234,567.89 個/分鐘");
}

/**
 * @tc.name: NumberFormatFuncTest0022
 * @tc.desc: Test Intl NumberFormat.format
 * @tc.type: FUNC
 */
HWTEST_F(NumberFormatTest, NumberFormatFuncTest0022, TestSize.Level1)
{
    string localeCN = "zh-CN";
    vector<string> localesCN{localeCN};
    string localeUS = "en-US";
    vector<string> localesUS{localeUS};
    string localeGB = "en-GB";
    vector<string> localesGB{localeGB};
    string localeBO = "bo";
    vector<string> localesBO{localeBO};
    string localeUG = "ug";
    vector<string> localesUG{localeUG};
    string localeHK = "zh-HK";
    vector<string> localesHK{localeHK};
    string localeTW = "zh-TW";
    vector<string> localesTW{localeTW};
    string style = "unit";
    string unit = "meter-per-hour";
    string unitStyle = "long";
    map<string, string> options = { { "style", style},
                                    { "unit", unit },
                                    { "unitStyle", unitStyle } };
    std::unique_ptr<NumberFormat> numFmtCN = std::make_unique<NumberFormat>(localesCN, options);
    ASSERT_TRUE(numFmtCN != nullptr);
    EXPECT_EQ(numFmtCN->Format(1234567.89), "1,234,567.89 米/小时");
    std::unique_ptr<NumberFormat> numFmtUS = std::make_unique<NumberFormat>(localesUS, options);
    ASSERT_TRUE(numFmtUS != nullptr);
    EXPECT_EQ(numFmtUS->Format(1), "1 m/h");
    EXPECT_EQ(numFmtUS->Format(1234567.89), "1,234,567.89 m/h");
    std::unique_ptr<NumberFormat> numFmtGB = std::make_unique<NumberFormat>(localesGB, options);
    ASSERT_TRUE(numFmtGB != nullptr);
    EXPECT_EQ(numFmtGB->Format(1), "1 m/h");
    EXPECT_EQ(numFmtGB->Format(1234567.89), "1,234,567.89 m/h");
    std::unique_ptr<NumberFormat> numFmtBO = std::make_unique<NumberFormat>(localesBO, options);
    ASSERT_TRUE(numFmtBO != nullptr);
    EXPECT_EQ(numFmtBO->Format(1234567.89), "1,234,567.89 m/h");
    std::unique_ptr<NumberFormat> numFmtUG = std::make_unique<NumberFormat>(localesUG, options);
    ASSERT_TRUE(numFmtUG != nullptr);
    EXPECT_EQ(numFmtUG->Format(1), "1 m/h");
    EXPECT_EQ(numFmtUG->Format(1234567.89), "1,234,567.89 m/h");
    std::unique_ptr<NumberFormat> numFmtHK = std::make_unique<NumberFormat>(localesHK, options);
    ASSERT_TRUE(numFmtHK != nullptr);
    EXPECT_EQ(numFmtHK->Format(1234567.89), "1,234,567.89 米/小時");
    std::unique_ptr<NumberFormat> numFmtTW = std::make_unique<NumberFormat>(localesTW, options);
    ASSERT_TRUE(numFmtTW != nullptr);
    EXPECT_EQ(numFmtTW->Format(1234567.89), "1,234,567.89 公尺/小時");
}

/**
 * @tc.name: NumberFormatFuncTest0023
 * @tc.desc: Test Intl NumberFormat.format
 * @tc.type: FUNC
 */
HWTEST_F(NumberFormatTest, NumberFormatFuncTest0023, TestSize.Level1)
{
    string localeCN = "zh-CN";
    vector<string> localesCN{localeCN};
    string localeUS = "en-US";
    vector<string> localesUS{localeUS};
    string localeGB = "en-GB";
    vector<string> localesGB{localeGB};
    string localeBO = "bo";
    vector<string> localesBO{localeBO};
    string localeUG = "ug";
    vector<string> localesUG{localeUG};
    string localeHK = "zh-HK";
    vector<string> localesHK{localeHK};
    string localeTW = "zh-TW";
    vector<string> localesTW{localeTW};
    string style = "unit";
    string unit = "meter-per-hour";
    string unitStyle = "short";
    map<string, string> options = { { "style", style},
                                    { "unit", unit },
                                    { "unitStyle", unitStyle } };
    std::unique_ptr<NumberFormat> numFmtCN = std::make_unique<NumberFormat>(localesCN, options);
    ASSERT_TRUE(numFmtCN != nullptr);
    EXPECT_EQ(numFmtCN->Format(1234567.89), "1,234,567.89 米/小时");
    std::unique_ptr<NumberFormat> numFmtUS = std::make_unique<NumberFormat>(localesUS, options);
    ASSERT_TRUE(numFmtUS != nullptr);
    EXPECT_EQ(numFmtUS->Format(1), "1 m/h");
    EXPECT_EQ(numFmtUS->Format(1234567.89), "1,234,567.89 m/h");
    std::unique_ptr<NumberFormat> numFmtGB = std::make_unique<NumberFormat>(localesGB, options);
    ASSERT_TRUE(numFmtGB != nullptr);
    EXPECT_EQ(numFmtGB->Format(1), "1 m/h");
    EXPECT_EQ(numFmtGB->Format(1234567.89), "1,234,567.89 m/h");
    std::unique_ptr<NumberFormat> numFmtBO = std::make_unique<NumberFormat>(localesBO, options);
    ASSERT_TRUE(numFmtBO != nullptr);
    EXPECT_EQ(numFmtBO->Format(1234567.89), "1,234,567.89 m/h");
    std::unique_ptr<NumberFormat> numFmtUG = std::make_unique<NumberFormat>(localesUG, options);
    ASSERT_TRUE(numFmtUG != nullptr);
    EXPECT_EQ(numFmtUG->Format(1), "1 m/h");
    EXPECT_EQ(numFmtUG->Format(1234567.89), "1,234,567.89 m/h");
    std::unique_ptr<NumberFormat> numFmtHK = std::make_unique<NumberFormat>(localesHK, options);
    ASSERT_TRUE(numFmtHK != nullptr);
    EXPECT_EQ(numFmtHK->Format(1234567.89), "1,234,567.89 米/小時");
    std::unique_ptr<NumberFormat> numFmtTW = std::make_unique<NumberFormat>(localesTW, options);
    ASSERT_TRUE(numFmtTW != nullptr);
    EXPECT_EQ(numFmtTW->Format(1234567.89), "1,234,567.89 公尺/小時");
}

/**
 * @tc.name: NumberFormatFuncTest0024
 * @tc.desc: Test Intl NumberFormat.format
 * @tc.type: FUNC
 */
HWTEST_F(NumberFormatTest, NumberFormatFuncTest0024, TestSize.Level1)
{
    string localeCN = "zh-CN";
    vector<string> localesCN{localeCN};
    string localeUS = "en-US";
    vector<string> localesUS{localeUS};
    string localeGB = "en-GB";
    vector<string> localesGB{localeGB};
    string localeBO = "bo";
    vector<string> localesBO{localeBO};
    string localeUG = "ug";
    vector<string> localesUG{localeUG};
    string localeHK = "zh-HK";
    vector<string> localesHK{localeHK};
    string localeTW = "zh-TW";
    vector<string> localesTW{localeTW};
    string style = "unit";
    string unit = "meter-per-hour";
    string unitStyle = "narrow";
    map<string, string> options = { { "style", style},
                                    { "unit", unit },
                                    { "unitStyle", unitStyle } };
    std::unique_ptr<NumberFormat> numFmtCN = std::make_unique<NumberFormat>(localesCN, options);
    ASSERT_TRUE(numFmtCN != nullptr);
    EXPECT_EQ(numFmtCN->Format(1234567.89), "1,234,567.89 米/小时");
    std::unique_ptr<NumberFormat> numFmtUS = std::make_unique<NumberFormat>(localesUS, options);
    ASSERT_TRUE(numFmtUS != nullptr);
    EXPECT_EQ(numFmtUS->Format(1), "1 m/h");
    EXPECT_EQ(numFmtUS->Format(1234567.89), "1,234,567.89 m/h");
    std::unique_ptr<NumberFormat> numFmtGB = std::make_unique<NumberFormat>(localesGB, options);
    ASSERT_TRUE(numFmtGB != nullptr);
    EXPECT_EQ(numFmtGB->Format(1), "1 m/h");
    EXPECT_EQ(numFmtGB->Format(1234567.89), "1,234,567.89 m/h");
    std::unique_ptr<NumberFormat> numFmtBO = std::make_unique<NumberFormat>(localesBO, options);
    ASSERT_TRUE(numFmtBO != nullptr);
    EXPECT_EQ(numFmtBO->Format(1234567.89), "1,234,567.89 m/h");
    std::unique_ptr<NumberFormat> numFmtUG = std::make_unique<NumberFormat>(localesUG, options);
    ASSERT_TRUE(numFmtUG != nullptr);
    EXPECT_EQ(numFmtUG->Format(1), "1 m/h");
    EXPECT_EQ(numFmtUG->Format(1234567.89), "1,234,567.89 m/h");
    std::unique_ptr<NumberFormat> numFmtHK = std::make_unique<NumberFormat>(localesHK, options);
    ASSERT_TRUE(numFmtHK != nullptr);
    EXPECT_EQ(numFmtHK->Format(1234567.89), "1,234,567.89 米/小時");
    std::unique_ptr<NumberFormat> numFmtTW = std::make_unique<NumberFormat>(localesTW, options);
    ASSERT_TRUE(numFmtTW != nullptr);
    EXPECT_EQ(numFmtTW->Format(1234567.89), "1,234,567.89 公尺/小時");
}

/**
 * @tc.name: NumberFormatFuncTest0025
 * @tc.desc: Test Intl NumberFormat.format
 * @tc.type: FUNC
 */
HWTEST_F(NumberFormatTest, NumberFormatFuncTest0025, TestSize.Level1)
{
    string localeCN = "zh-CN";
    vector<string> localesCN{localeCN};
    string localeUS = "en-US";
    vector<string> localesUS{localeUS};
    string localeGB = "en-GB";
    vector<string> localesGB{localeGB};
    string localeBO = "bo";
    vector<string> localesBO{localeBO};
    string localeUG = "ug";
    vector<string> localesUG{localeUG};
    string localeHK = "zh-HK";
    vector<string> localesHK{localeHK};
    string localeTW = "zh-TW";
    vector<string> localesTW{localeTW};
    string style = "unit";
    string unit = "milliliter-per-minute-per-kilogram";
    string unitStyle = "long";
    map<string, string> options = { { "style", style},
                                    { "unit", unit },
                                    { "unitStyle", unitStyle } };
    std::unique_ptr<NumberFormat> numFmtCN = std::make_unique<NumberFormat>(localesCN, options);
    ASSERT_TRUE(numFmtCN != nullptr);
    EXPECT_EQ(numFmtCN->Format(1234567.89), "1,234,567.89 ml/kg/min");
    std::unique_ptr<NumberFormat> numFmtUS = std::make_unique<NumberFormat>(localesUS, options);
    ASSERT_TRUE(numFmtUS != nullptr);
    EXPECT_EQ(numFmtUS->Format(1), "1 ml/kg/min");
    EXPECT_EQ(numFmtUS->Format(1234567.89), "1,234,567.89 ml/kg/min");
    std::unique_ptr<NumberFormat> numFmtGB = std::make_unique<NumberFormat>(localesGB, options);
    ASSERT_TRUE(numFmtGB != nullptr);
    EXPECT_EQ(numFmtGB->Format(1), "1 ml/kg/min");
    EXPECT_EQ(numFmtGB->Format(1234567.89), "1,234,567.89 ml/kg/min");
    std::unique_ptr<NumberFormat> numFmtBO = std::make_unique<NumberFormat>(localesBO, options);
    ASSERT_TRUE(numFmtBO != nullptr);
    EXPECT_EQ(numFmtBO->Format(1234567.89), "1,234,567.89 ml/kg/min");
    std::unique_ptr<NumberFormat> numFmtUG = std::make_unique<NumberFormat>(localesUG, options);
    ASSERT_TRUE(numFmtUG != nullptr);
    EXPECT_EQ(numFmtUG->Format(1), "1 ml/kg/min");
    EXPECT_EQ(numFmtUG->Format(1234567.89), "1,234,567.89 ml/kg/min");
    std::unique_ptr<NumberFormat> numFmtHK = std::make_unique<NumberFormat>(localesHK, options);
    ASSERT_TRUE(numFmtHK != nullptr);
    EXPECT_EQ(numFmtHK->Format(1234567.89), "1,234,567.89 ml/kg/min");
    std::unique_ptr<NumberFormat> numFmtTW = std::make_unique<NumberFormat>(localesTW, options);
    ASSERT_TRUE(numFmtTW != nullptr);
    EXPECT_EQ(numFmtTW->Format(1234567.89), "1,234,567.89 ml/kg/min");
}

/**
 * @tc.name: NumberFormatFuncTest0026
 * @tc.desc: Test Intl NumberFormat.format
 * @tc.type: FUNC
 */
HWTEST_F(NumberFormatTest, NumberFormatFuncTest0026, TestSize.Level1)
{
    string localeCN = "zh-CN";
    vector<string> localesCN{localeCN};
    string localeUS = "en-US";
    vector<string> localesUS{localeUS};
    string localeGB = "en-GB";
    vector<string> localesGB{localeGB};
    string localeBO = "bo";
    vector<string> localesBO{localeBO};
    string localeUG = "ug";
    vector<string> localesUG{localeUG};
    string localeHK = "zh-HK";
    vector<string> localesHK{localeHK};
    string localeTW = "zh-TW";
    vector<string> localesTW{localeTW};
    string style = "unit";
    string unit = "milliliter-per-minute-per-kilogram";
    string unitStyle = "short";
    map<string, string> options = { { "style", style},
                                    { "unit", unit },
                                    { "unitStyle", unitStyle } };
    std::unique_ptr<NumberFormat> numFmtCN = std::make_unique<NumberFormat>(localesCN, options);
    ASSERT_TRUE(numFmtCN != nullptr);
    EXPECT_EQ(numFmtCN->Format(1234567.89), "1,234,567.89 ml/kg/min");
    std::unique_ptr<NumberFormat> numFmtUS = std::make_unique<NumberFormat>(localesUS, options);
    ASSERT_TRUE(numFmtUS != nullptr);
    EXPECT_EQ(numFmtUS->Format(1), "1 ml/kg/min");
    EXPECT_EQ(numFmtUS->Format(1234567.89), "1,234,567.89 ml/kg/min");
    std::unique_ptr<NumberFormat> numFmtGB = std::make_unique<NumberFormat>(localesGB, options);
    ASSERT_TRUE(numFmtGB != nullptr);
    EXPECT_EQ(numFmtGB->Format(1), "1 ml/kg/min");
    EXPECT_EQ(numFmtGB->Format(1234567.89), "1,234,567.89 ml/kg/min");
    std::unique_ptr<NumberFormat> numFmtBO = std::make_unique<NumberFormat>(localesBO, options);
    ASSERT_TRUE(numFmtBO != nullptr);
    EXPECT_EQ(numFmtBO->Format(1234567.89), "1,234,567.89 ml/kg/min");
    std::unique_ptr<NumberFormat> numFmtUG = std::make_unique<NumberFormat>(localesUG, options);
    ASSERT_TRUE(numFmtUG != nullptr);
    EXPECT_EQ(numFmtUG->Format(1), "1 ml/kg/min");
    EXPECT_EQ(numFmtUG->Format(1234567.89), "1,234,567.89 ml/kg/min");
    std::unique_ptr<NumberFormat> numFmtHK = std::make_unique<NumberFormat>(localesHK, options);
    ASSERT_TRUE(numFmtHK != nullptr);
    EXPECT_EQ(numFmtHK->Format(1234567.89), "1,234,567.89 ml/kg/min");
    std::unique_ptr<NumberFormat> numFmtTW = std::make_unique<NumberFormat>(localesTW, options);
    ASSERT_TRUE(numFmtTW != nullptr);
    EXPECT_EQ(numFmtTW->Format(1234567.89), "1,234,567.89 ml/kg/min");
}

/**
 * @tc.name: NumberFormatFuncTest0027
 * @tc.desc: Test Intl NumberFormat.format
 * @tc.type: FUNC
 */
HWTEST_F(NumberFormatTest, NumberFormatFuncTest0027, TestSize.Level1)
{
    string localeCN = "zh-CN";
    vector<string> localesCN{localeCN};
    string localeUS = "en-US";
    vector<string> localesUS{localeUS};
    string localeGB = "en-GB";
    vector<string> localesGB{localeGB};
    string localeBO = "bo";
    vector<string> localesBO{localeBO};
    string localeUG = "ug";
    vector<string> localesUG{localeUG};
    string localeHK = "zh-HK";
    vector<string> localesHK{localeHK};
    string localeTW = "zh-TW";
    vector<string> localesTW{localeTW};
    string style = "unit";
    string unit = "milliliter-per-minute-per-kilogram";
    string unitStyle = "narrow";
    map<string, string> options = { { "style", style},
                                    { "unit", unit },
                                    { "unitStyle", unitStyle } };
    std::unique_ptr<NumberFormat> numFmtCN = std::make_unique<NumberFormat>(localesCN, options);
    ASSERT_TRUE(numFmtCN != nullptr);
    EXPECT_EQ(numFmtCN->Format(1234567.89), "1,234,567.89 ml/kg/min");
    std::unique_ptr<NumberFormat> numFmtUS = std::make_unique<NumberFormat>(localesUS, options);
    ASSERT_TRUE(numFmtUS != nullptr);
    EXPECT_EQ(numFmtUS->Format(1), "1 ml/kg/min");
    EXPECT_EQ(numFmtUS->Format(1234567.89), "1,234,567.89 ml/kg/min");
    std::unique_ptr<NumberFormat> numFmtGB = std::make_unique<NumberFormat>(localesGB, options);
    ASSERT_TRUE(numFmtGB != nullptr);
    EXPECT_EQ(numFmtGB->Format(1), "1 ml/kg/min");
    EXPECT_EQ(numFmtGB->Format(1234567.89), "1,234,567.89 ml/kg/min");
    std::unique_ptr<NumberFormat> numFmtBO = std::make_unique<NumberFormat>(localesBO, options);
    ASSERT_TRUE(numFmtBO != nullptr);
    EXPECT_EQ(numFmtBO->Format(1234567.89), "1,234,567.89 ml/kg/min");
    std::unique_ptr<NumberFormat> numFmtUG = std::make_unique<NumberFormat>(localesUG, options);
    ASSERT_TRUE(numFmtUG != nullptr);
    EXPECT_EQ(numFmtUG->Format(1), "1 ml/kg/min");
    EXPECT_EQ(numFmtUG->Format(1234567.89), "1,234,567.89 ml/kg/min");
    std::unique_ptr<NumberFormat> numFmtHK = std::make_unique<NumberFormat>(localesHK, options);
    ASSERT_TRUE(numFmtHK != nullptr);
    EXPECT_EQ(numFmtHK->Format(1234567.89), "1,234,567.89 ml/kg/min");
    std::unique_ptr<NumberFormat> numFmtTW = std::make_unique<NumberFormat>(localesTW, options);
    ASSERT_TRUE(numFmtTW != nullptr);
    EXPECT_EQ(numFmtTW->Format(1234567.89), "1,234,567.89 ml/kg/min");
}

/**
 * @tc.name: NumberFormatFuncTest0028
 * @tc.desc: Test Intl NumberFormat.format
 * @tc.type: FUNC
 */
HWTEST_F(NumberFormatTest, NumberFormatFuncTest0028, TestSize.Level1)
{
    string localeCN = "zh-CN";
    vector<string> localesCN{localeCN};
    string localeUS = "en-US";
    vector<string> localesUS{localeUS};
    string localeGB = "en-GB";
    vector<string> localesGB{localeGB};
    string localeBO = "bo";
    vector<string> localesBO{localeBO};
    string localeUG = "ug";
    vector<string> localesUG{localeUG};
    string localeHK = "zh-HK";
    vector<string> localesHK{localeHK};
    string localeTW = "zh-TW";
    vector<string> localesTW{localeTW};
    string style = "unit";
    string unit = "rotation-per-minute";
    string unitStyle = "long";
    map<string, string> options = { { "style", style},
                                    { "unit", unit },
                                    { "unitStyle", unitStyle } };
    std::unique_ptr<NumberFormat> numFmtCN = std::make_unique<NumberFormat>(localesCN, options);
    ASSERT_TRUE(numFmtCN != nullptr);
    EXPECT_EQ(numFmtCN->Format(1234567.89), "1,234,567.89 转/分钟");
    std::unique_ptr<NumberFormat> numFmtUS = std::make_unique<NumberFormat>(localesUS, options);
    ASSERT_TRUE(numFmtUS != nullptr);
    EXPECT_EQ(numFmtUS->Format(1), "1 rpm");
    EXPECT_EQ(numFmtUS->Format(1234567.89), "1,234,567.89 rpm");
    std::unique_ptr<NumberFormat> numFmtGB = std::make_unique<NumberFormat>(localesGB, options);
    ASSERT_TRUE(numFmtGB != nullptr);
    EXPECT_EQ(numFmtGB->Format(1), "1 rpm");
    EXPECT_EQ(numFmtGB->Format(1234567.89), "1,234,567.89 rpm");
    std::unique_ptr<NumberFormat> numFmtBO = std::make_unique<NumberFormat>(localesBO, options);
    ASSERT_TRUE(numFmtBO != nullptr);
    EXPECT_EQ(numFmtBO->Format(1234567.89), "སྐར་མ་རེར་སྐོར་བ་ 1,234,567.89");
    std::unique_ptr<NumberFormat> numFmtUG = std::make_unique<NumberFormat>(localesUG, options);
    ASSERT_TRUE(numFmtUG != nullptr);
    EXPECT_EQ(numFmtUG->Format(1), "1 ق/م");
    EXPECT_EQ(numFmtUG->Format(1234567.89), "1,234,567.89 ق/م");
    std::unique_ptr<NumberFormat> numFmtHK = std::make_unique<NumberFormat>(localesHK, options);
    ASSERT_TRUE(numFmtHK != nullptr);
    EXPECT_EQ(numFmtHK->Format(1234567.89), "1,234,567.89 轉/分鐘");
    std::unique_ptr<NumberFormat> numFmtTW = std::make_unique<NumberFormat>(localesTW, options);
    ASSERT_TRUE(numFmtTW != nullptr);
    EXPECT_EQ(numFmtTW->Format(1234567.89), "1,234,567.89 轉/分鐘");
}

/**
 * @tc.name: NumberFormatFuncTest0029
 * @tc.desc: Test Intl NumberFormat.format
 * @tc.type: FUNC
 */
HWTEST_F(NumberFormatTest, NumberFormatFuncTest0029, TestSize.Level1)
{
    string localeCN = "zh-CN";
    vector<string> localesCN{localeCN};
    string localeUS = "en-US";
    vector<string> localesUS{localeUS};
    string localeGB = "en-GB";
    vector<string> localesGB{localeGB};
    string localeBO = "bo";
    vector<string> localesBO{localeBO};
    string localeUG = "ug";
    vector<string> localesUG{localeUG};
    string localeHK = "zh-HK";
    vector<string> localesHK{localeHK};
    string localeTW = "zh-TW";
    vector<string> localesTW{localeTW};
    string style = "unit";
    string unit = "rotation-per-minute";
    string unitStyle = "short";
    map<string, string> options = { { "style", style},
                                    { "unit", unit },
                                    { "unitStyle", unitStyle } };
    std::unique_ptr<NumberFormat> numFmtCN = std::make_unique<NumberFormat>(localesCN, options);
    ASSERT_TRUE(numFmtCN != nullptr);
    EXPECT_EQ(numFmtCN->Format(1234567.89), "1,234,567.89 转/分钟");
    std::unique_ptr<NumberFormat> numFmtUS = std::make_unique<NumberFormat>(localesUS, options);
    ASSERT_TRUE(numFmtUS != nullptr);
    EXPECT_EQ(numFmtUS->Format(1), "1 rpm");
    EXPECT_EQ(numFmtUS->Format(1234567.89), "1,234,567.89 rpm");
    std::unique_ptr<NumberFormat> numFmtGB = std::make_unique<NumberFormat>(localesGB, options);
    ASSERT_TRUE(numFmtGB != nullptr);
    EXPECT_EQ(numFmtGB->Format(1), "1 rpm");
    EXPECT_EQ(numFmtGB->Format(1234567.89), "1,234,567.89 rpm");
    std::unique_ptr<NumberFormat> numFmtBO = std::make_unique<NumberFormat>(localesBO, options);
    ASSERT_TRUE(numFmtBO != nullptr);
    EXPECT_EQ(numFmtBO->Format(1234567.89), "སྐར་མ་རེར་སྐོར་བ་ 1,234,567.89");
    std::unique_ptr<NumberFormat> numFmtUG = std::make_unique<NumberFormat>(localesUG, options);
    ASSERT_TRUE(numFmtUG != nullptr);
    EXPECT_EQ(numFmtUG->Format(1), "1 ق/م");
    EXPECT_EQ(numFmtUG->Format(1234567.89), "1,234,567.89 ق/م");
    std::unique_ptr<NumberFormat> numFmtHK = std::make_unique<NumberFormat>(localesHK, options);
    ASSERT_TRUE(numFmtHK != nullptr);
    EXPECT_EQ(numFmtHK->Format(1234567.89), "1,234,567.89 轉/分鐘");
    std::unique_ptr<NumberFormat> numFmtTW = std::make_unique<NumberFormat>(localesTW, options);
    ASSERT_TRUE(numFmtTW != nullptr);
    EXPECT_EQ(numFmtTW->Format(1234567.89), "1,234,567.89 轉/分鐘");
}

/**
 * @tc.name: NumberFormatFuncTest0030
 * @tc.desc: Test Intl NumberFormat.format
 * @tc.type: FUNC
 */
HWTEST_F(NumberFormatTest, NumberFormatFuncTest0030, TestSize.Level1)
{
    string localeCN = "zh-CN";
    vector<string> localesCN{localeCN};
    string localeUS = "en-US";
    vector<string> localesUS{localeUS};
    string localeGB = "en-GB";
    vector<string> localesGB{localeGB};
    string localeBO = "bo";
    vector<string> localesBO{localeBO};
    string localeUG = "ug";
    vector<string> localesUG{localeUG};
    string localeHK = "zh-HK";
    vector<string> localesHK{localeHK};
    string localeTW = "zh-TW";
    vector<string> localesTW{localeTW};
    string style = "unit";
    string unit = "rotation-per-minute";
    string unitStyle = "narrow";
    map<string, string> options = { { "style", style},
                                    { "unit", unit },
                                    { "unitStyle", unitStyle } };
    std::unique_ptr<NumberFormat> numFmtCN = std::make_unique<NumberFormat>(localesCN, options);
    ASSERT_TRUE(numFmtCN != nullptr);
    EXPECT_EQ(numFmtCN->Format(1234567.89), "1,234,567.89 转/分钟");
    std::unique_ptr<NumberFormat> numFmtUS = std::make_unique<NumberFormat>(localesUS, options);
    ASSERT_TRUE(numFmtUS != nullptr);
    EXPECT_EQ(numFmtUS->Format(1), "1 rpm");
    EXPECT_EQ(numFmtUS->Format(1234567.89), "1,234,567.89 rpm");
    std::unique_ptr<NumberFormat> numFmtGB = std::make_unique<NumberFormat>(localesGB, options);
    ASSERT_TRUE(numFmtGB != nullptr);
    EXPECT_EQ(numFmtGB->Format(1), "1 rpm");
    EXPECT_EQ(numFmtGB->Format(1234567.89), "1,234,567.89 rpm");
    std::unique_ptr<NumberFormat> numFmtBO = std::make_unique<NumberFormat>(localesBO, options);
    ASSERT_TRUE(numFmtBO != nullptr);
    EXPECT_EQ(numFmtBO->Format(1234567.89), "སྐར་མ་རེར་སྐོར་བ་ 1,234,567.89");
    std::unique_ptr<NumberFormat> numFmtUG = std::make_unique<NumberFormat>(localesUG, options);
    ASSERT_TRUE(numFmtUG != nullptr);
    EXPECT_EQ(numFmtUG->Format(1), "1 ق/م");
    EXPECT_EQ(numFmtUG->Format(1234567.89), "1,234,567.89 ق/م");
    std::unique_ptr<NumberFormat> numFmtHK = std::make_unique<NumberFormat>(localesHK, options);
    ASSERT_TRUE(numFmtHK != nullptr);
    EXPECT_EQ(numFmtHK->Format(1234567.89), "1,234,567.89 轉/分鐘");
    std::unique_ptr<NumberFormat> numFmtTW = std::make_unique<NumberFormat>(localesTW, options);
    ASSERT_TRUE(numFmtTW != nullptr);
    EXPECT_EQ(numFmtTW->Format(1234567.89), "1,234,567.89 轉/分鐘");
}

/**
 * @tc.name: NumberFormatFuncTest0031
 * @tc.desc: Test Intl NumberFormat.format
 * @tc.type: FUNC
 */
HWTEST_F(NumberFormatTest, NumberFormatFuncTest0031, TestSize.Level1)
{
    string localeCN = "zh-CN";
    vector<string> localesCN{localeCN};
    string localeUS = "en-US";
    vector<string> localesUS{localeUS};
    string localeGB = "en-GB";
    vector<string> localesGB{localeGB};
    string localeBO = "bo";
    vector<string> localesBO{localeBO};
    string localeUG = "ug";
    vector<string> localesUG{localeUG};
    string localeHK = "zh-HK";
    vector<string> localesHK{localeHK};
    string localeTW = "zh-TW";
    vector<string> localesTW{localeTW};
    string style = "unit";
    string unit = "step-per-minute";
    string unitStyle = "long";
    map<string, string> options = { { "style", style},
                                    { "unit", unit },
                                    { "unitStyle", unitStyle } };
    std::unique_ptr<NumberFormat> numFmtCN = std::make_unique<NumberFormat>(localesCN, options);
    ASSERT_TRUE(numFmtCN != nullptr);
    EXPECT_EQ(numFmtCN->Format(1234567.89), "1,234,567.89 步/分钟");
    std::unique_ptr<NumberFormat> numFmtUS = std::make_unique<NumberFormat>(localesUS, options);
    ASSERT_TRUE(numFmtUS != nullptr);
    EXPECT_EQ(numFmtUS->Format(1), "1 step/min");
    EXPECT_EQ(numFmtUS->Format(1234567.89), "1,234,567.89 steps/min");
    std::unique_ptr<NumberFormat> numFmtGB = std::make_unique<NumberFormat>(localesGB, options);
    ASSERT_TRUE(numFmtGB != nullptr);
    EXPECT_EQ(numFmtGB->Format(1), "1 step/min");
    EXPECT_EQ(numFmtGB->Format(1234567.89), "1,234,567.89 steps/min");
    std::unique_ptr<NumberFormat> numFmtBO = std::make_unique<NumberFormat>(localesBO, options);
    ASSERT_TRUE(numFmtBO != nullptr);
    EXPECT_EQ(numFmtBO->Format(1234567.89), "སྐར་མ་རེར་གོམ་པ་ 1,234,567.89");
    std::unique_ptr<NumberFormat> numFmtUG = std::make_unique<NumberFormat>(localesUG, options);
    ASSERT_TRUE(numFmtUG != nullptr);
    EXPECT_EQ(numFmtUG->Format(1), "1 قەدەم/مىنۇت");
    EXPECT_EQ(numFmtUG->Format(1234567.89), "1,234,567.89 قەدەم/مىنۇت");
    std::unique_ptr<NumberFormat> numFmtHK = std::make_unique<NumberFormat>(localesHK, options);
    ASSERT_TRUE(numFmtHK != nullptr);
    EXPECT_EQ(numFmtHK->Format(1234567.89), "1,234,567.89 步/分鐘");
    std::unique_ptr<NumberFormat> numFmtTW = std::make_unique<NumberFormat>(localesTW, options);
    ASSERT_TRUE(numFmtTW != nullptr);
    EXPECT_EQ(numFmtTW->Format(1234567.89), "1,234,567.89 步/分鐘");
}

/**
 * @tc.name: NumberFormatFuncTest0032
 * @tc.desc: Test Intl NumberFormat.format
 * @tc.type: FUNC
 */
HWTEST_F(NumberFormatTest, NumberFormatFuncTest0032, TestSize.Level1)
{
    string localeCN = "zh-CN";
    vector<string> localesCN{localeCN};
    string localeUS = "en-US";
    vector<string> localesUS{localeUS};
    string localeGB = "en-GB";
    vector<string> localesGB{localeGB};
    string localeBO = "bo";
    vector<string> localesBO{localeBO};
    string localeUG = "ug";
    vector<string> localesUG{localeUG};
    string localeHK = "zh-HK";
    vector<string> localesHK{localeHK};
    string localeTW = "zh-TW";
    vector<string> localesTW{localeTW};
    string style = "unit";
    string unit = "step-per-minute";
    string unitStyle = "short";
    map<string, string> options = { { "style", style},
                                    { "unit", unit },
                                    { "unitStyle", unitStyle } };
    std::unique_ptr<NumberFormat> numFmtCN = std::make_unique<NumberFormat>(localesCN, options);
    ASSERT_TRUE(numFmtCN != nullptr);
    EXPECT_EQ(numFmtCN->Format(1234567.89), "1,234,567.89 步/分钟");
    std::unique_ptr<NumberFormat> numFmtUS = std::make_unique<NumberFormat>(localesUS, options);
    ASSERT_TRUE(numFmtUS != nullptr);
    EXPECT_EQ(numFmtUS->Format(1), "1 step/min");
    EXPECT_EQ(numFmtUS->Format(1234567.89), "1,234,567.89 steps/min");
    std::unique_ptr<NumberFormat> numFmtGB = std::make_unique<NumberFormat>(localesGB, options);
    ASSERT_TRUE(numFmtGB != nullptr);
    EXPECT_EQ(numFmtGB->Format(1), "1 step/min");
    EXPECT_EQ(numFmtGB->Format(1234567.89), "1,234,567.89 steps/min");
    std::unique_ptr<NumberFormat> numFmtBO = std::make_unique<NumberFormat>(localesBO, options);
    ASSERT_TRUE(numFmtBO != nullptr);
    EXPECT_EQ(numFmtBO->Format(1234567.89), "སྐར་མ་རེར་གོམ་པ་ 1,234,567.89");
    std::unique_ptr<NumberFormat> numFmtUG = std::make_unique<NumberFormat>(localesUG, options);
    ASSERT_TRUE(numFmtUG != nullptr);
    EXPECT_EQ(numFmtUG->Format(1), "1 قەدەم/مىنۇت");
    EXPECT_EQ(numFmtUG->Format(1234567.89), "1,234,567.89 قەدەم/مىنۇت");
    std::unique_ptr<NumberFormat> numFmtHK = std::make_unique<NumberFormat>(localesHK, options);
    ASSERT_TRUE(numFmtHK != nullptr);
    EXPECT_EQ(numFmtHK->Format(1234567.89), "1,234,567.89 步/分鐘");
    std::unique_ptr<NumberFormat> numFmtTW = std::make_unique<NumberFormat>(localesTW, options);
    ASSERT_TRUE(numFmtTW != nullptr);
    EXPECT_EQ(numFmtTW->Format(1234567.89), "1,234,567.89 步/分鐘");
}

/**
 * @tc.name: NumberFormatFuncTest0033
 * @tc.desc: Test Intl NumberFormat.format
 * @tc.type: FUNC
 */
HWTEST_F(NumberFormatTest, NumberFormatFuncTest0033, TestSize.Level1)
{
    string localeCN = "zh-CN";
    vector<string> localesCN{localeCN};
    string localeUS = "en-US";
    vector<string> localesUS{localeUS};
    string localeGB = "en-GB";
    vector<string> localesGB{localeGB};
    string localeBO = "bo";
    vector<string> localesBO{localeBO};
    string localeUG = "ug";
    vector<string> localesUG{localeUG};
    string localeHK = "zh-HK";
    vector<string> localesHK{localeHK};
    string localeTW = "zh-TW";
    vector<string> localesTW{localeTW};
    string style = "unit";
    string unit = "step-per-minute";
    string unitStyle = "narrow";
    map<string, string> options = { { "style", style},
                                    { "unit", unit },
                                    { "unitStyle", unitStyle } };
    std::unique_ptr<NumberFormat> numFmtCN = std::make_unique<NumberFormat>(localesCN, options);
    ASSERT_TRUE(numFmtCN != nullptr);
    EXPECT_EQ(numFmtCN->Format(1234567.89), "1,234,567.89 步/分钟");
    std::unique_ptr<NumberFormat> numFmtUS = std::make_unique<NumberFormat>(localesUS, options);
    ASSERT_TRUE(numFmtUS != nullptr);
    EXPECT_EQ(numFmtUS->Format(1), "1 step/min");
    EXPECT_EQ(numFmtUS->Format(1234567.89), "1,234,567.89 steps/min");
    std::unique_ptr<NumberFormat> numFmtGB = std::make_unique<NumberFormat>(localesGB, options);
    ASSERT_TRUE(numFmtGB != nullptr);
    EXPECT_EQ(numFmtGB->Format(1), "1 step/min");
    EXPECT_EQ(numFmtGB->Format(1234567.89), "1,234,567.89 steps/min");
    std::unique_ptr<NumberFormat> numFmtBO = std::make_unique<NumberFormat>(localesBO, options);
    ASSERT_TRUE(numFmtBO != nullptr);
    EXPECT_EQ(numFmtBO->Format(1234567.89), "སྐར་མ་རེར་གོམ་པ་ 1,234,567.89");
    std::unique_ptr<NumberFormat> numFmtUG = std::make_unique<NumberFormat>(localesUG, options);
    ASSERT_TRUE(numFmtUG != nullptr);
    EXPECT_EQ(numFmtUG->Format(1), "1 قەدەم/مىنۇت");
    EXPECT_EQ(numFmtUG->Format(1234567.89), "1,234,567.89 قەدەم/مىنۇت");
    std::unique_ptr<NumberFormat> numFmtHK = std::make_unique<NumberFormat>(localesHK, options);
    ASSERT_TRUE(numFmtHK != nullptr);
    EXPECT_EQ(numFmtHK->Format(1234567.89), "1,234,567.89 步/分鐘");
    std::unique_ptr<NumberFormat> numFmtTW = std::make_unique<NumberFormat>(localesTW, options);
    ASSERT_TRUE(numFmtTW != nullptr);
    EXPECT_EQ(numFmtTW->Format(1234567.89), "1,234,567.89 步/分鐘");
}

/**
 * @tc.name: NumberFormatFuncTest0034
 * @tc.desc: Test Intl NumberFormat.format
 * @tc.type: FUNC
 */
HWTEST_F(NumberFormatTest, NumberFormatFuncTest0034, TestSize.Level1)
{
    string localeCN = "zh-CN";
    vector<string> localesCN{localeCN};
    string localeUS = "en-US";
    vector<string> localesUS{localeUS};
    string localeGB = "en-GB";
    vector<string> localesGB{localeGB};
    string localeBO = "bo";
    vector<string> localesBO{localeBO};
    string localeUG = "ug";
    vector<string> localesUG{localeUG};
    string localeHK = "zh-HK";
    vector<string> localesHK{localeHK};
    string localeTW = "zh-TW";
    vector<string> localesTW{localeTW};
    string style = "unit";
    string unit = "stroke-per-minute";
    string unitStyle = "long";
    map<string, string> options = { { "style", style},
                                    { "unit", unit },
                                    { "unitStyle", unitStyle } };
    std::unique_ptr<NumberFormat> numFmtCN = std::make_unique<NumberFormat>(localesCN, options);
    ASSERT_TRUE(numFmtCN != nullptr);
    EXPECT_EQ(numFmtCN->Format(1234567.89), "1,234,567.89 次/分钟");
    std::unique_ptr<NumberFormat> numFmtUS = std::make_unique<NumberFormat>(localesUS, options);
    ASSERT_TRUE(numFmtUS != nullptr);
    EXPECT_EQ(numFmtUS->Format(1), "1 stroke/min");
    EXPECT_EQ(numFmtUS->Format(1234567.89), "1,234,567.89 strokes/min");
    std::unique_ptr<NumberFormat> numFmtGB = std::make_unique<NumberFormat>(localesGB, options);
    ASSERT_TRUE(numFmtGB != nullptr);
    EXPECT_EQ(numFmtGB->Format(1), "1 stroke/min");
    EXPECT_EQ(numFmtGB->Format(1234567.89), "1,234,567.89 strokes/min");
    std::unique_ptr<NumberFormat> numFmtBO = std::make_unique<NumberFormat>(localesBO, options);
    ASSERT_TRUE(numFmtBO != nullptr);
    EXPECT_EQ(numFmtBO->Format(1234567.89), "སྐར་མ་རེར་ཐེངས་ 1,234,567.89");
    std::unique_ptr<NumberFormat> numFmtUG = std::make_unique<NumberFormat>(localesUG, options);
    ASSERT_TRUE(numFmtUG != nullptr);
    EXPECT_EQ(numFmtUG->Format(1), "1 پالاق/مىنۇت");
    EXPECT_EQ(numFmtUG->Format(1234567.89), "1,234,567.89 پالاق/مىنۇت");
    std::unique_ptr<NumberFormat> numFmtHK = std::make_unique<NumberFormat>(localesHK, options);
    ASSERT_TRUE(numFmtHK != nullptr);
    EXPECT_EQ(numFmtHK->Format(1234567.89), "1,234,567.89 下/分鐘");
    std::unique_ptr<NumberFormat> numFmtTW = std::make_unique<NumberFormat>(localesTW, options);
    ASSERT_TRUE(numFmtTW != nullptr);
    EXPECT_EQ(numFmtTW->Format(1234567.89), "1,234,567.89 次/分鐘");
}

/**
 * @tc.name: NumberFormatFuncTest0035
 * @tc.desc: Test Intl NumberFormat.format
 * @tc.type: FUNC
 */
HWTEST_F(NumberFormatTest, NumberFormatFuncTest0035, TestSize.Level1)
{
    string localeCN = "zh-CN";
    vector<string> localesCN{localeCN};
    string localeUS = "en-US";
    vector<string> localesUS{localeUS};
    string localeGB = "en-GB";
    vector<string> localesGB{localeGB};
    string localeBO = "bo";
    vector<string> localesBO{localeBO};
    string localeUG = "ug";
    vector<string> localesUG{localeUG};
    string localeHK = "zh-HK";
    vector<string> localesHK{localeHK};
    string localeTW = "zh-TW";
    vector<string> localesTW{localeTW};
    string style = "unit";
    string unit = "stroke-per-minute";
    string unitStyle = "short";
    map<string, string> options = { { "style", style},
                                    { "unit", unit },
                                    { "unitStyle", unitStyle } };
    std::unique_ptr<NumberFormat> numFmtCN = std::make_unique<NumberFormat>(localesCN, options);
    ASSERT_TRUE(numFmtCN != nullptr);
    EXPECT_EQ(numFmtCN->Format(1234567.89), "1,234,567.89 次/分钟");
    std::unique_ptr<NumberFormat> numFmtUS = std::make_unique<NumberFormat>(localesUS, options);
    ASSERT_TRUE(numFmtUS != nullptr);
    EXPECT_EQ(numFmtUS->Format(1), "1 stroke/min");
    EXPECT_EQ(numFmtUS->Format(1234567.89), "1,234,567.89 strokes/min");
    std::unique_ptr<NumberFormat> numFmtGB = std::make_unique<NumberFormat>(localesGB, options);
    ASSERT_TRUE(numFmtGB != nullptr);
    EXPECT_EQ(numFmtGB->Format(1), "1 stroke/min");
    EXPECT_EQ(numFmtGB->Format(1234567.89), "1,234,567.89 strokes/min");
    std::unique_ptr<NumberFormat> numFmtBO = std::make_unique<NumberFormat>(localesBO, options);
    ASSERT_TRUE(numFmtBO != nullptr);
    EXPECT_EQ(numFmtBO->Format(1234567.89), "སྐར་མ་རེར་ཐེངས་ 1,234,567.89");
    std::unique_ptr<NumberFormat> numFmtUG = std::make_unique<NumberFormat>(localesUG, options);
    ASSERT_TRUE(numFmtUG != nullptr);
    EXPECT_EQ(numFmtUG->Format(1), "1 پالاق/مىنۇت");
    EXPECT_EQ(numFmtUG->Format(1234567.89), "1,234,567.89 پالاق/مىنۇت");
    std::unique_ptr<NumberFormat> numFmtHK = std::make_unique<NumberFormat>(localesHK, options);
    ASSERT_TRUE(numFmtHK != nullptr);
    EXPECT_EQ(numFmtHK->Format(1234567.89), "1,234,567.89 下/分鐘");
    std::unique_ptr<NumberFormat> numFmtTW = std::make_unique<NumberFormat>(localesTW, options);
    ASSERT_TRUE(numFmtTW != nullptr);
    EXPECT_EQ(numFmtTW->Format(1234567.89), "1,234,567.89 次/分鐘");
}

/**
 * @tc.name: NumberFormatFuncTest0036
 * @tc.desc: Test Intl NumberFormat.format
 * @tc.type: FUNC
 */
HWTEST_F(NumberFormatTest, NumberFormatFuncTest0036, TestSize.Level1)
{
    string localeCN = "zh-CN";
    vector<string> localesCN{localeCN};
    string localeUS = "en-US";
    vector<string> localesUS{localeUS};
    string localeGB = "en-GB";
    vector<string> localesGB{localeGB};
    string localeBO = "bo";
    vector<string> localesBO{localeBO};
    string localeUG = "ug";
    vector<string> localesUG{localeUG};
    string localeHK = "zh-HK";
    vector<string> localesHK{localeHK};
    string localeTW = "zh-TW";
    vector<string> localesTW{localeTW};
    string style = "unit";
    string unit = "stroke-per-minute";
    string unitStyle = "narrow";
    map<string, string> options = { { "style", style},
                                    { "unit", unit },
                                    { "unitStyle", unitStyle } };
    std::unique_ptr<NumberFormat> numFmtCN = std::make_unique<NumberFormat>(localesCN, options);
    ASSERT_TRUE(numFmtCN != nullptr);
    EXPECT_EQ(numFmtCN->Format(1234567.89), "1,234,567.89 次/分钟");
    std::unique_ptr<NumberFormat> numFmtUS = std::make_unique<NumberFormat>(localesUS, options);
    ASSERT_TRUE(numFmtUS != nullptr);
    EXPECT_EQ(numFmtUS->Format(1), "1 stroke/min");
    EXPECT_EQ(numFmtUS->Format(1234567.89), "1,234,567.89 strokes/min");
    std::unique_ptr<NumberFormat> numFmtGB = std::make_unique<NumberFormat>(localesGB, options);
    ASSERT_TRUE(numFmtGB != nullptr);
    EXPECT_EQ(numFmtGB->Format(1), "1 stroke/min");
    EXPECT_EQ(numFmtGB->Format(1234567.89), "1,234,567.89 strokes/min");
    std::unique_ptr<NumberFormat> numFmtBO = std::make_unique<NumberFormat>(localesBO, options);
    ASSERT_TRUE(numFmtBO != nullptr);
    EXPECT_EQ(numFmtBO->Format(1234567.89), "སྐར་མ་རེར་ཐེངས་ 1,234,567.89");
    std::unique_ptr<NumberFormat> numFmtUG = std::make_unique<NumberFormat>(localesUG, options);
    ASSERT_TRUE(numFmtUG != nullptr);
    EXPECT_EQ(numFmtUG->Format(1), "1 پالاق/مىنۇت");
    EXPECT_EQ(numFmtUG->Format(1234567.89), "1,234,567.89 پالاق/مىنۇت");
    std::unique_ptr<NumberFormat> numFmtHK = std::make_unique<NumberFormat>(localesHK, options);
    ASSERT_TRUE(numFmtHK != nullptr);
    EXPECT_EQ(numFmtHK->Format(1234567.89), "1,234,567.89 下/分鐘");
    std::unique_ptr<NumberFormat> numFmtTW = std::make_unique<NumberFormat>(localesTW, options);
    ASSERT_TRUE(numFmtTW != nullptr);
    EXPECT_EQ(numFmtTW->Format(1234567.89), "1,234,567.89 次/分鐘");
}

/**
 * @tc.name: NumberFormatFuncTest0037
 * @tc.desc: Test Intl NumberFormat.formatRange
 * @tc.type: FUNC
 */
HWTEST_F(NumberFormatTest, NumberFormatFuncTest0037, TestSize.Level1)
{
    string locale = "en-US";
    vector<string> locales{locale};
    string unit = "meter";
    string style = "unit";
    map<string, string> options = { { "style", style},
                                    { "unit", unit } };
    std::unique_ptr<NumberFormat> rangeFormatter = std::make_unique<NumberFormat>(locales, options);
    ASSERT_TRUE(rangeFormatter != nullptr);
    if (NumberFormatTest::deviceType == "wearable" || NumberFormatTest::deviceType == "liteWearable" ||
        NumberFormatTest::deviceType == "watch") {
        EXPECT_EQ(rangeFormatter->FormatRange(1, 5), "1–5m");
        EXPECT_EQ(rangeFormatter->FormatRange(4.9999999, 5.0000001), "~5m");
        EXPECT_EQ(rangeFormatter->FormatRange(5, 5), "~5m");
        EXPECT_EQ(rangeFormatter->FormatRange(0, 3), "0–3m");
        EXPECT_EQ(rangeFormatter->FormatRange(0, 0), "~0m");
        EXPECT_EQ(rangeFormatter->FormatRange(3, 3000), "3–3,000m");
        EXPECT_EQ(rangeFormatter->FormatRange(3000, 5000), "3,000–5,000m");
        EXPECT_EQ(rangeFormatter->FormatRange(4999, 5001), "4,999–5,001m");
        EXPECT_EQ(rangeFormatter->FormatRange(5000, 5000), "~5,000m");
        EXPECT_EQ(rangeFormatter->FormatRange(5e3, 5e6), "5,000–5,000,000m");
    } else if (NumberFormatTest::deviceType == "tablet" || NumberFormatTest::deviceType == "2in1" ||
        NumberFormatTest::deviceType == "tv" || NumberFormatTest::deviceType == "pc") {
        EXPECT_EQ(rangeFormatter->FormatRange(1, 5), "1–5 meters");
        EXPECT_EQ(rangeFormatter->FormatRange(4.9999999, 5.0000001), "~5 meters");
        EXPECT_EQ(rangeFormatter->FormatRange(5, 5), "~5 meters");
        EXPECT_EQ(rangeFormatter->FormatRange(0, 3), "0–3 meters");
        EXPECT_EQ(rangeFormatter->FormatRange(0, 0), "~0 meters");
        EXPECT_EQ(rangeFormatter->FormatRange(3, 3000), "3–3,000 meters");
        EXPECT_EQ(rangeFormatter->FormatRange(3000, 5000), "3,000–5,000 meters");
        EXPECT_EQ(rangeFormatter->FormatRange(4999, 5001), "4,999–5,001 meters");
        EXPECT_EQ(rangeFormatter->FormatRange(5000, 5000), "~5,000 meters");
        EXPECT_EQ(rangeFormatter->FormatRange(5e3, 5e6), "5,000–5,000,000 meters");
    } else {
        EXPECT_EQ(rangeFormatter->FormatRange(1, 5), "1–5 m");
        EXPECT_EQ(rangeFormatter->FormatRange(4.9999999, 5.0000001), "~5 m");
        EXPECT_EQ(rangeFormatter->FormatRange(5, 5), "~5 m");
        EXPECT_EQ(rangeFormatter->FormatRange(0, 3), "0–3 m");
        EXPECT_EQ(rangeFormatter->FormatRange(0, 0), "~0 m");
        EXPECT_EQ(rangeFormatter->FormatRange(3, 3000), "3–3,000 m");
        EXPECT_EQ(rangeFormatter->FormatRange(3000, 5000), "3,000–5,000 m");
        EXPECT_EQ(rangeFormatter->FormatRange(4999, 5001), "4,999–5,001 m");
        EXPECT_EQ(rangeFormatter->FormatRange(5000, 5000), "~5,000 m");
        EXPECT_EQ(rangeFormatter->FormatRange(5e3, 5e6), "5,000–5,000,000 m");
    }
}

/**
 * @tc.name: NumberFormatFuncTest0038
 * @tc.desc: Test Intl NumberFormat.formatRange
 * @tc.type: FUNC
 */
HWTEST_F(NumberFormatTest, NumberFormatFuncTest0038, TestSize.Level1)
{
    string locale = "ja";
    vector<string> locales{locale};
    map<string, string> options = {};
    std::unique_ptr<NumberFormat> rangeFormatter = std::make_unique<NumberFormat>(locales, options);
    ASSERT_TRUE(rangeFormatter != nullptr);
    EXPECT_EQ(rangeFormatter->FormatRange(1, 5), "1～5");
    EXPECT_EQ(rangeFormatter->FormatRange(4.9999999, 5.0000001), "約5");
    EXPECT_EQ(rangeFormatter->FormatRange(5, 5), "約5");
    EXPECT_EQ(rangeFormatter->FormatRange(0, 3), "0～3");
    EXPECT_EQ(rangeFormatter->FormatRange(0, 0), "約0");
    EXPECT_EQ(rangeFormatter->FormatRange(3, 3000), "3～3,000");
    EXPECT_EQ(rangeFormatter->FormatRange(3000, 5000), "3,000～5,000");
    EXPECT_EQ(rangeFormatter->FormatRange(4999, 5001), "4,999～5,001");
    EXPECT_EQ(rangeFormatter->FormatRange(5000, 5000), "約5,000");
    EXPECT_EQ(rangeFormatter->FormatRange(5e3, 5e6), "5,000～5,000,000");
}

/**
 * @tc.name: NumberFormatFuncTest0039
 * @tc.desc: Test I18n StyledNumberFormat.format
 * @tc.type: FUNC
 */
HWTEST_F(NumberFormatTest, NumberFormatFuncTest0039, TestSize.Level1)
{
    string locale = "en-IN";
    string expects = "+1,23,456.79 euros";
    vector<string> locales{locale};
    string useGrouping = "true";
    string minimumIntegerDigits = "7";
    string maximumFractionDigits = "2";
    string style = "currency";
    string currency = "978";
    map<string, string> options = { { "useGrouping", useGrouping },
                                    { "style", style },
                                    { "currency", currency },
                                    { "currencyDisplay", "name" },
                                    { "currencySign", "accounting" },
                                    { "signDisplay", "always" } };
    std::shared_ptr<NumberFormat> numFmt = std::make_shared<NumberFormat>(locales, options);
    ASSERT_TRUE(numFmt != nullptr);
    StyledNumberFormat styledNumFmt(true, numFmt, nullptr);
    string out = styledNumFmt.Format(123456.789);
    EXPECT_EQ(out, expects);
}

/**
 * @tc.name: NumberFormatFuncTest0040
 * @tc.desc: Test I18n StyledNumberFormat.ParseToParts
 * @tc.type: FUNC
 */
HWTEST_F(NumberFormatTest, NumberFormatFuncTest0040, TestSize.Level1)
{
    string locale = "en-IN";
    vector<string> locales{locale};
    string useGrouping = "true";
    string minimumIntegerDigits = "7";
    string maximumFractionDigits = "2";
    string style = "currency";
    string currency = "978";
    map<string, string> options = { { "useGrouping", useGrouping },
                                    { "style", style },
                                    { "currency", currency },
                                    { "currencyDisplay", "name" },
                                    { "currencySign", "accounting" },
                                    { "signDisplay", "always" } };
    std::shared_ptr<NumberFormat> numFmt = std::make_shared<NumberFormat>(locales, options);
    ASSERT_TRUE(numFmt != nullptr);
    StyledNumberFormat styledNumFmt(true, numFmt, nullptr);
    std::vector<StyledNumberFormat::NumberPart> out = styledNumFmt.ParseToParts(123456.789);
    EXPECT_EQ(out.size(), 5);
    EXPECT_EQ(out[0].part_name, "integer");
    EXPECT_EQ(out[1].part_name, "integer");
    EXPECT_EQ(out[2].part_name, "decimal");
    EXPECT_EQ(out[3].part_name, "fraction");
    EXPECT_EQ(out[4].part_name, "unit");
}

/**
 * @tc.name: NumberFormatFuncTest0041
 * @tc.desc: Test I18n StyledNumberFormat.format
 * @tc.type: FUNC
 */
HWTEST_F(NumberFormatTest, NumberFormatFuncTest0041, TestSize.Level1)
{
    I18nErrorCode err = I18nErrorCode::SUCCESS;
    std::string skeleton = "percent";
    std::shared_ptr<LocaleInfo> localeInfo = std::make_shared<LocaleInfo>("zh-Hans-CN");
    ASSERT_TRUE(localeInfo != nullptr);
    std::shared_ptr<SimpleNumberFormat> formatter = std::make_shared<SimpleNumberFormat>(skeleton, localeInfo, err);
    EXPECT_EQ(err, I18nErrorCode::SUCCESS);
    ASSERT_TRUE(formatter != nullptr);
    StyledNumberFormat styledNumFmt(false, nullptr, formatter);
    std::string result = styledNumFmt.Format(10);
    EXPECT_EQ(result, "10%");
}

/**
 * @tc.name: NumberFormatFuncTest0042
 * @tc.desc: Test I18n StyledNumberFormat.ParseToParts and StyledNumberFormat.Format
 * @tc.type: FUNC
 */
HWTEST_F(NumberFormatTest, NumberFormatFuncTest0042, TestSize.Level1)
{
    string locale = "en";
    vector<string> locales{locale};
    map<string, string> options = { { "style", "unit" },
                                    { "unit", "minute" },
                                    { "unitUsage", "elapsed-time-second" } };
    std::shared_ptr<NumberFormat> numFmt = std::make_shared<NumberFormat>(locales, options);
    ASSERT_TRUE(numFmt != nullptr);
    StyledNumberFormat styledNumFmt(true, numFmt, nullptr);
    if (NumberFormatTest::deviceType == "wearable" || NumberFormatTest::deviceType == "liteWearable" ||
        NumberFormatTest::deviceType == "watch") {
        EXPECT_EQ(styledNumFmt.Format(1234.5678), "20 hr. ago");
    } else if (NumberFormatTest::deviceType == "tablet" || NumberFormatTest::deviceType == "2in1" ||
        NumberFormatTest::deviceType == "tv" || NumberFormatTest::deviceType == "pc") {
        EXPECT_EQ(styledNumFmt.Format(1234.5678), "20 hours ago");
    } else {
        EXPECT_EQ(styledNumFmt.Format(1234.5678), "20 hours ago");
    }
    std::vector<StyledNumberFormat::NumberPart> out = styledNumFmt.ParseToParts(123456.789);
    EXPECT_EQ(out.size(), 2);
    EXPECT_EQ(out[0].part_name, "integer");
    EXPECT_EQ(out[1].part_name, "unit");
}

/**
 * @tc.name: NumberFormatFuncTest0043
 * @tc.desc: Test Intl NumberFormat.format
 * @tc.type: FUNC
 */
HWTEST_F(NumberFormatTest, NumberFormatFuncTest0043, TestSize.Level1)
{
    string locale = "en";
    vector<string> locales{locale};
    map<string, string> options = {
        {"maximumFractionDigits", "3"},
    };
    std::unique_ptr<NumberFormat> formatter = std::make_unique<NumberFormat>(locales, options);
    ASSERT_TRUE(formatter != nullptr);
    EXPECT_EQ(formatter->Format(1.23456), "1.235");
    options = {
        {"maximumSignificantDigits", "3"},
    };
    std::unique_ptr<NumberFormat> formatter2 = std::make_unique<NumberFormat>(locales, options);
    EXPECT_EQ(formatter2->Format(1.23456), "1.23");
    options = {
        {"maximumFractionDigits", "3"},
        {"maximumSignificantDigits", "3"},
    };
    std::unique_ptr<NumberFormat> formatter3 = std::make_unique<NumberFormat>(locales, options);
    EXPECT_EQ(formatter3->Format(1.23456), "1.23");
    options = {
        {"roundingPriority", "lessPrecision"},
        {"maximumFractionDigits", "3"},
        {"maximumSignificantDigits", "3"},
    };
    std::unique_ptr<NumberFormat> formatter4 = std::make_unique<NumberFormat>(locales, options);
    EXPECT_EQ(formatter4->Format(1.23456), "1.23");
    options = {
        {"roundingPriority", "morePrecision"},
        {"maximumFractionDigits", "3"},
        {"maximumSignificantDigits", "3"},
    };
    std::unique_ptr<NumberFormat> formatter5 = std::make_unique<NumberFormat>(locales, options);
    EXPECT_EQ(formatter5->Format(1.23456), "1.235");
    options = {
        {"roundingPriority", "lessPrecision"},
        {"minimumFractionDigits", "2"},
        {"minimumSignificantDigits", "2"},
    };
    std::unique_ptr<NumberFormat> formatter6 = std::make_unique<NumberFormat>(locales, options);
    EXPECT_EQ(formatter6->Format(1), "1.00");
    options = {
        {"roundingPriority", "morePrecision"},
        {"minimumFractionDigits", "2"},
        {"minimumSignificantDigits", "2"},
    };
    std::unique_ptr<NumberFormat> formatter7 = std::make_unique<NumberFormat>(locales, options);
    EXPECT_EQ(formatter7->Format(1), "1.0");
}

/**
 * @tc.name: NumberFormatFuncTest0044
 * @tc.desc: Test Intl NumberFormat.format
 * @tc.type: FUNC
 */
HWTEST_F(NumberFormatTest, NumberFormatFuncTest0044, TestSize.Level1)
{
    string locale = "en";
    vector<string> locales{locale};
    map<string, string> options = {
        {"roundingMode", "ceil"},
        {"maximumSignificantDigits", "2"},
    };
    std::unique_ptr<NumberFormat> formatter = std::make_unique<NumberFormat>(locales, options);
    ASSERT_TRUE(formatter != nullptr);
    EXPECT_EQ(formatter->Format(2.23), "2.3");
    EXPECT_EQ(formatter->Format(2.25), "2.3");
    EXPECT_EQ(formatter->Format(2.28), "2.3");
    EXPECT_EQ(formatter->Format(-2.23), "-2.2");
    EXPECT_EQ(formatter->Format(-2.25), "-2.2");
    EXPECT_EQ(formatter->Format(-2.28), "-2.2");

    options = {
        {"roundingMode", "halfCeil"},
        {"maximumSignificantDigits", "2"},
    };
    std::unique_ptr<NumberFormat> numFmt = std::make_unique<NumberFormat>(locales, options);
    ASSERT_TRUE(numFmt != nullptr);
    EXPECT_EQ(numFmt->Format(2.23), "2.2");
    EXPECT_EQ(numFmt->Format(2.25), "2.3");
    EXPECT_EQ(numFmt->Format(2.28), "2.3");
    EXPECT_EQ(numFmt->Format(-2.23), "-2.2");
    EXPECT_EQ(numFmt->Format(-2.25), "-2.2");
    EXPECT_EQ(numFmt->Format(-2.28), "-2.3");
}

/**
 * @tc.name: NumberFormatFuncTest0045
 * @tc.desc: Test Intl NumberFormat.format
 * @tc.type: FUNC
 */
HWTEST_F(NumberFormatTest, NumberFormatFuncTest0045, TestSize.Level1)
{
    string locale = "en";
    vector<string> locales{locale};
    map<string, string> options = {
        {"roundingMode", "floor"},
        {"maximumSignificantDigits", "2"},
    };
    std::unique_ptr<NumberFormat> formatter = std::make_unique<NumberFormat>(locales, options);
    ASSERT_TRUE(formatter != nullptr);
    EXPECT_EQ(formatter->Format(2.23), "2.2");
    EXPECT_EQ(formatter->Format(2.25), "2.2");
    EXPECT_EQ(formatter->Format(2.28), "2.2");
    EXPECT_EQ(formatter->Format(-2.23), "-2.3");
    EXPECT_EQ(formatter->Format(-2.25), "-2.3");
    EXPECT_EQ(formatter->Format(-2.28), "-2.3");

    options = {
        {"roundingMode", "halfFloor"},
        {"maximumSignificantDigits", "2"},
    };
    std::unique_ptr<NumberFormat> formatter2 = std::make_unique<NumberFormat>(locales, options);
    ASSERT_TRUE(formatter2 != nullptr);
    EXPECT_EQ(formatter2->Format(2.23), "2.2");
    EXPECT_EQ(formatter2->Format(2.25), "2.2");
    EXPECT_EQ(formatter2->Format(2.28), "2.3");
    EXPECT_EQ(formatter2->Format(-2.23), "-2.2");
    EXPECT_EQ(formatter2->Format(-2.25), "-2.3");
    EXPECT_EQ(formatter2->Format(-2.28), "-2.3");
}

/**
 * @tc.name: NumberFormatFuncTest0046
 * @tc.desc: Test Intl NumberFormat.format
 * @tc.type: FUNC
 */
HWTEST_F(NumberFormatTest, NumberFormatFuncTest0046, TestSize.Level1)
{
    string locale = "en";
    vector<string> locales{locale};
    map<string, string> options = {
        {"roundingMode", "expand"},
        {"maximumSignificantDigits", "2"},
    };
    std::unique_ptr<NumberFormat> formatter = std::make_unique<NumberFormat>(locales, options);
    ASSERT_TRUE(formatter != nullptr);
    EXPECT_EQ(formatter->Format(2.23), "2.3");
    EXPECT_EQ(formatter->Format(2.25), "2.3");
    EXPECT_EQ(formatter->Format(2.28), "2.3");
    EXPECT_EQ(formatter->Format(-2.23), "-2.3");
    EXPECT_EQ(formatter->Format(-2.25), "-2.3");
    EXPECT_EQ(formatter->Format(-2.28), "-2.3");

    options = {
        {"roundingMode", "halfExpand"},
        {"maximumSignificantDigits", "2"},
    };
    std::unique_ptr<NumberFormat> numberFormat = std::make_unique<NumberFormat>(locales, options);
    ASSERT_TRUE(numberFormat != nullptr);
    EXPECT_EQ(numberFormat->Format(2.23), "2.2");
    EXPECT_EQ(numberFormat->Format(2.25), "2.3");
    EXPECT_EQ(numberFormat->Format(2.28), "2.3");
    EXPECT_EQ(numberFormat->Format(-2.23), "-2.2");
    EXPECT_EQ(numberFormat->Format(-2.25), "-2.3");
    EXPECT_EQ(numberFormat->Format(-2.28), "-2.3");
}

/**
 * @tc.name: NumberFormatFuncTest0047
 * @tc.desc: Test Intl NumberFormat.format
 * @tc.type: FUNC
 */
HWTEST_F(NumberFormatTest, NumberFormatFuncTest0047, TestSize.Level1)
{
    string locale = "en";
    vector<string> locales{locale};
    map<string, string> options = {
        {"roundingMode", "trunc"},
        {"maximumSignificantDigits", "2"},
    };
    std::unique_ptr<NumberFormat> formatter = std::make_unique<NumberFormat>(locales, options);
    ASSERT_TRUE(formatter != nullptr);
    EXPECT_EQ(formatter->Format(2.23), "2.2");
    EXPECT_EQ(formatter->Format(2.25), "2.2");
    EXPECT_EQ(formatter->Format(2.28), "2.2");
    EXPECT_EQ(formatter->Format(-2.23), "-2.2");
    EXPECT_EQ(formatter->Format(-2.25), "-2.2");
    EXPECT_EQ(formatter->Format(-2.28), "-2.2");

    options = {
        {"roundingMode", "halfTrunc"},
        {"maximumSignificantDigits", "2"},
    };
    std::unique_ptr<NumberFormat> numberFormat = std::make_unique<NumberFormat>(locales, options);
    ASSERT_TRUE(numberFormat != nullptr);
    EXPECT_EQ(numberFormat->Format(2.23), "2.2");
    EXPECT_EQ(numberFormat->Format(2.25), "2.2");
    EXPECT_EQ(numberFormat->Format(2.28), "2.3");
    EXPECT_EQ(numberFormat->Format(-2.23), "-2.2");
    EXPECT_EQ(numberFormat->Format(-2.25), "-2.2");
    EXPECT_EQ(numberFormat->Format(-2.28), "-2.3");
}

/**
 * @tc.name: NumberFormatFuncTest0048
 * @tc.desc: Test Intl NumberFormat.format
 * @tc.type: FUNC
 */
HWTEST_F(NumberFormatTest, NumberFormatFuncTest0048, TestSize.Level1)
{
    string locale = "en";
    vector<string> locales{locale};
    map<string, string> options = {
        {"roundingMode", "halfEven"},
        {"maximumSignificantDigits", "2"},
    };
    std::unique_ptr<NumberFormat> formatter = std::make_unique<NumberFormat>(locales, options);
    ASSERT_TRUE(formatter != nullptr);
    EXPECT_EQ(formatter->Format(2.23), "2.2");
    EXPECT_EQ(formatter->Format(2.25), "2.2");
    EXPECT_EQ(formatter->Format(2.28), "2.3");
    EXPECT_EQ(formatter->Format(-2.23), "-2.2");
    EXPECT_EQ(formatter->Format(-2.25), "-2.2");
    EXPECT_EQ(formatter->Format(-2.28), "-2.3");
}

/**
 * @tc.name: NumberFormatFuncTest0049
 * @tc.desc: Test Intl NumberFormat.format
 * @tc.type: FUNC
 */
HWTEST_F(NumberFormatTest, NumberFormatFuncTest0049, TestSize.Level1)
{
    string locale = "en-US";
    vector<string> locales{locale};
    map<string, string> options = {
        {"style", "currency"},
        {"currency", "USD"},
        {"roundingIncrement", "5"},
        {"maximumFractionDigits", "2"},
    };
    std::unique_ptr<NumberFormat> formatter = std::make_unique<NumberFormat>(locales, options);
    ASSERT_TRUE(formatter != nullptr);
    EXPECT_EQ(formatter->Format(11.29), "$11.30");
    EXPECT_EQ(formatter->Format(11.25), "$11.25");
    EXPECT_EQ(formatter->Format(11.22), "$11.20");

    options = {
        {"style", "currency"},
        {"currency", "USD"},
        {"roundingIncrement", "5"},
        {"maximumFractionDigits", "2"},
        {"roundingMode", "halfCeil"},
    };
    std::unique_ptr<NumberFormat> numberFmt = std::make_unique<NumberFormat>(locales, options);
    ASSERT_TRUE(numberFmt != nullptr);
    EXPECT_EQ(numberFmt->Format(11.21), "$11.20");
    EXPECT_EQ(numberFmt->Format(11.22), "$11.20");
    EXPECT_EQ(numberFmt->Format(11.224), "$11.20");
    EXPECT_EQ(numberFmt->Format(11.225), "$11.25");
    EXPECT_EQ(numberFmt->Format(11.23), "$11.25");
}
} // namespace I18n
} // namespace Global
} // namespace OHOS