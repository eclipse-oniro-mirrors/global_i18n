/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#include <map>
#include <vector>

#include "character.h"
#include "collator.h"
#include "date_time_filter.h"
#include "date_time_format.h"
#include "date_time_rule.h"
#include "holiday_manager.h"
#include "i18n_break_iterator.h"
#include "i18n_calendar.h"
#include "i18n_timezone.h"
#include "i18n_types.h"
#include "index_util.h"
#include "locale_compare.h"
#include "locale_config.h"
#include "locale_info.h"
#include "measure_data.h"
#include "number_format.h"
#include "phone_number_format.h"
#include "plural_rules.h"
#include "preferred_language.h"
#include "regex_rule.h"
#include "relative_time_format.h"
#include "system_locale_manager.h"
#include "taboo_utils.h"
#include "taboo.h"
#include "utils.h"
#include "intl_test.h"
#include "generate_ics_file.h"
#include <unistd.h>

using namespace OHOS::Global::I18n;
using testing::ext::TestSize;
using namespace std;

namespace OHOS {
namespace Global {
namespace I18n {
class IntlTest : public testing::Test {
public:
    static string originalLanguage;
    static string originalRegion;
    static string originalLocale;
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

string IntlTest::originalLanguage;
string IntlTest::originalRegion;
string IntlTest::originalLocale;

void IntlTest::SetUpTestCase(void)
{
    originalLanguage = LocaleConfig::GetSystemLanguage();
    originalRegion = LocaleConfig::GetSystemRegion();
    originalLocale = LocaleConfig::GetSystemLocale();
    LocaleConfig::SetSystemLanguage("zh-Hans");
    LocaleConfig::SetSystemRegion("CN");
    LocaleConfig::SetSystemLocale("zh-Hans-CN");
}

void IntlTest::TearDownTestCase(void)
{
    LocaleConfig::SetSystemLanguage(originalLanguage);
    LocaleConfig::SetSystemRegion(originalRegion);
    LocaleConfig::SetSystemLocale(originalLocale);
}

void IntlTest::SetUp(void)
{}

void IntlTest::TearDown(void)
{}

/**
 * @tc.name: IntlFuncTest001
 * @tc.desc: Test Intl DateTimeFormat.format
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest, IntlFuncTest001, TestSize.Level1)
{
    string locale = "zh-CN-u-hc-h12";
    string expects = "公元1970年1月1日星期四 上午8:20:34";
    vector<string> locales;
    locales.push_back("jessie");
    locales.push_back(locale);
    map<string, string> options = { { "year", "numeric" },
                                    { "month", "narrow" },
                                    { "day", "numeric" },
                                    { "hour", "numeric" },
                                    { "minute", "2-digit" },
                                    { "second", "numeric" },
                                    { "weekday", "long" },
                                    { "era", "short"} };
    DateTimeFormat *dateFormat = new (std::nothrow) DateTimeFormat(locales, options);
    if (!dateFormat) {
        EXPECT_TRUE(false);
        return;
    }
    int64_t milliseconds = 1234567;
    string out = dateFormat->Format(milliseconds);
    EXPECT_EQ(out, expects);
    EXPECT_EQ(dateFormat->GetYear(), "numeric");
    EXPECT_EQ(dateFormat->GetMonth(), "narrow");
    EXPECT_EQ(dateFormat->GetDay(), "numeric");
    EXPECT_EQ(dateFormat->GetHour(), "numeric");
    EXPECT_EQ(dateFormat->GetMinute(), "2-digit");
    EXPECT_EQ(dateFormat->GetSecond(), "numeric");
    EXPECT_EQ(dateFormat->GetWeekday(), "long");
    EXPECT_EQ(dateFormat->GetEra(), "short");
    EXPECT_EQ(dateFormat->GetHourCycle(), "h12");
    delete dateFormat;
}

/**
 * @tc.name: IntlFuncTest002
 * @tc.desc: Test Intl LocaleInfo
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest, IntlFuncTest002, TestSize.Level0)
{
    string locale = "ja-Jpan-JP-u-ca-japanese-hc-h12-co-emoji";
    map<string, string> options = { { "hourCycle", "h11" },
                                    { "numeric", "true" },
                                    { "numberingSystem", "jpan" } };
    LocaleInfo *loc = new (std::nothrow) LocaleInfo(locale, options);
    if (!loc) {
        EXPECT_TRUE(false);
        return;
    }
    EXPECT_EQ(loc->GetLanguage(), "ja");
    EXPECT_EQ(loc->GetScript(), "Jpan");
    EXPECT_EQ(loc->GetRegion(), "JP");
    EXPECT_EQ(loc->GetBaseName(), "ja-Jpan-JP");
    EXPECT_EQ(loc->GetCalendar(), "japanese");
    EXPECT_EQ(loc->GetHourCycle(), "h11");
    EXPECT_EQ(loc->GetNumberingSystem(), "jpan");
    EXPECT_EQ(loc->Minimize(), "ja-u-hc-h11-nu-jpan-ca-japanese-co-emoji-kn-true");
    EXPECT_EQ(loc->Maximize(), "ja-Jpan-JP-u-hc-h11-nu-jpan-ca-japanese-co-emoji-kn-true");
    EXPECT_EQ(loc->GetNumeric(), "true");
    EXPECT_EQ(loc->GetCaseFirst(), "");
    delete loc;
}

/**
 * @tc.name: IntlFuncTest003
 * @tc.desc: Test Intl LocaleInfo
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest, IntlFuncTest003, TestSize.Level1)
{
    string locale = "en-GB";
    LocaleInfo *loc = new (std::nothrow) LocaleInfo(locale);
    if (!loc) {
        EXPECT_TRUE(false);
        return;
    }
    string language = "en";
    string script = "";
    string region = "GB";
    string baseName = "en-GB";
    EXPECT_EQ(loc->GetLanguage(), language);
    EXPECT_EQ(loc->GetScript(), script);
    EXPECT_EQ(loc->GetRegion(), region);
    EXPECT_EQ(loc->GetBaseName(), baseName);
    locale = "ja-u-hc-h12-nu-Jpan-JP-kf-japanese-co-emoji-kn-true";
    LocaleInfo *localeInfo = new (std::nothrow) LocaleInfo(locale);
    EXPECT_EQ(localeInfo->GetBaseName(), "ja");
    delete localeInfo;
    delete loc;
}

/**
 * @tc.name: IntlFuncTest004
 * @tc.desc: Test Intl DateTimeFormat.format
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest, IntlFuncTest004, TestSize.Level1)
{
    string locale = "en-GB";
    string expects = "2 January 1970, 18:17 – 12 January 1970, 18:20";
    vector<string> locales;
    locales.push_back(locale);
    string dateStyle = "long";
    string timeStyle = "short";
    map<string, string> options = { { "dateStyle", dateStyle },
                                    { "timeStyle", timeStyle } };
    DateTimeFormat *dateFormat = new (std::nothrow) DateTimeFormat(locales, options);
    if (!dateFormat) {
        EXPECT_TRUE(false);
        return;
    }
    int64_t fromMilliseconds = 123456789;
    int64_t toMilliseconds = 987654321;
    string out = dateFormat->FormatRange(fromMilliseconds, toMilliseconds);
    EXPECT_EQ(out, expects);
    EXPECT_EQ(dateFormat->GetDateStyle(), dateStyle);
    EXPECT_EQ(dateFormat->GetTimeStyle(), timeStyle);
    delete dateFormat;
}

/**
 * @tc.name: IntlFuncTest005
 * @tc.desc: Test Intl DateTimeFormat.format
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest, IntlFuncTest005, TestSize.Level1)
{
    string locale = "ja";
    string expects = "1970年1月2日金曜日";
    vector<string> locales;
    locales.push_back(locale);
    map<string, string> options = { { "year", "numeric" },
                                    { "month", "long" },
                                    { "day", "numeric" },
                                    { "weekday", "long"} };
    DateTimeFormat *dateFormat = new (std::nothrow) DateTimeFormat(locales, options);
    if (!dateFormat) {
        EXPECT_TRUE(false);
        return;
    }
    int64_t milliseconds = 123456789;
    string out = dateFormat->Format(milliseconds);
    EXPECT_EQ(out, expects);
    int64_t fromMilliseconds = 123456789;
    int64_t toMilliseconds = 987654321;
    expects = "1970/01/02(金曜日)～1970/01/12(月曜日)";
    out = dateFormat->FormatRange(fromMilliseconds, toMilliseconds);
    EXPECT_EQ(out, expects);
    delete dateFormat;
}

/**
 * @tc.name: IntlFuncTest006
 * @tc.desc: Test Intl NumberFormat.format
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest, IntlFuncTest006, TestSize.Level1)
{
    string locale = "en-IN";
    string expects = "+1,23,456.79 euros";
    vector<string> locales;
    locales.push_back(locale);
    string useGrouping = "true";
    string minimumIntegerDigits = "7";
    string maximumFractionDigits = "2";
    string style = "currency";
    string currency = "EUR";
    map<string, string> options = { { "useGrouping", useGrouping },
                                    { "style", style },
                                    { "currency", currency },
                                    { "currencyDisplay", "name" },
                                    { "currencySign", "accounting" },
                                    { "signDisplay", "always" } };
    NumberFormat *numFmt = new (std::nothrow) NumberFormat(locales, options);
    if (!numFmt) {
        EXPECT_TRUE(false);
        return;
    }
    string out = numFmt->Format(123456.789);
    EXPECT_EQ(out, expects);
    EXPECT_EQ(numFmt->GetUseGrouping(), useGrouping);
    EXPECT_EQ(numFmt->GetStyle(), style);
    EXPECT_EQ(numFmt->GetCurrency(), currency);
    delete numFmt;
}

/**
 * @tc.name: IntlFuncTest007
 * @tc.desc: Test Intl NumberFormat.format
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest, IntlFuncTest007, TestSize.Level1)
{
    string locale = "zh-CN";
    string expects = "0,123,456.79米";
    vector<string> locales;
    locales.push_back("ban");
    locales.push_back(locale);
    string minimumIntegerDigits = "7";
    string maximumFractionDigits = "2";
    string style = "unit";
    map<string, string> options = { { "style", style },
                                    { "minimumIntegerDigits", minimumIntegerDigits },
                                    { "maximumFractionDigits", maximumFractionDigits },
                                    { "unit", "meter" },
                                    { "unitDisplay", "long"} };
    NumberFormat *numFmt = new (std::nothrow) NumberFormat(locales, options);
    if (!numFmt) {
        EXPECT_TRUE(false);
        return;
    }
    string out = numFmt->Format(123456.789);
    EXPECT_EQ(out, expects);
    EXPECT_EQ(numFmt->GetStyle(), style);
    delete numFmt;
}

/**
 * @tc.name: IntlFuncTest008
 * @tc.desc: Test Intl NumberFormat.format
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest, IntlFuncTest008, TestSize.Level1)
{
    string locale = "en-CN";
    string expects = "12,345,678.9%";
    vector<string> locales;
    locales.push_back(locale);
    string minimumIntegerDigits = "7";
    string maximumFractionDigits = "2";
    string style = "percent";
    map<string, string> options = { { "style", style },
                                    { "minimumIntegerDigits", minimumIntegerDigits },
                                    { "maximumFractionDigits", maximumFractionDigits } };
    NumberFormat *numFmt = new (std::nothrow) NumberFormat(locales, options);
    if (!numFmt) {
        EXPECT_TRUE(false);
        return;
    }
    string out = numFmt->Format(123456.789);
    EXPECT_EQ(out, expects);
    EXPECT_EQ(numFmt->GetStyle(), style);
    delete numFmt;
}

/**
 * @tc.name: IntlFuncTest009
 * @tc.desc: Test Intl NumberFormat.format
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest, IntlFuncTest009, TestSize.Level1)
{
    string locale = "en-CN";
    string expects = "0,123,456.79";
    vector<string> locales;
    locales.push_back(locale);
    string minimumIntegerDigits = "7";
    string maximumFractionDigits = "2";
    string style = "decimal";
    map<string, string> options = { { "style", style },
                                    { "minimumIntegerDigits", minimumIntegerDigits },
                                    { "maximumFractionDigits", maximumFractionDigits } };
    NumberFormat *numFmt = new (std::nothrow) NumberFormat(locales, options);
    if (!numFmt) {
        EXPECT_TRUE(false);
        return;
    }
    string out = numFmt->Format(123456.789);
    EXPECT_EQ(out, expects);
    EXPECT_EQ(numFmt->GetStyle(), style);
    options = { { "style", "unit" },
                { "unit", "meter" },
                { "currency", "USD" },
                { "currencyDisplay", "symbol" },
                { "compactDisplay", "long" },
                { "useGrouping", "true" },
                { "unitUsage", "length-person" },
                { "unitDisplay", "long" } };
    NumberFormat *numFormat = new (std::nothrow) NumberFormat(locales, options);
    map<string, string> res;
    numFormat->GetResolvedOptions(res);
    delete numFormat;
    delete numFmt;
}

/**
 * @tc.name: IntlFuncTest0010
 * @tc.desc: Test Intl NumberFormat.format
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest, IntlFuncTest010, TestSize.Level1)
{
    string locale = "en-CN";
    string expects = "1.234568E5";
    vector<string> locales;
    locales.push_back(locale);
    string style = "decimal";
    map<string, string> options = { { "style", style },
                                    { "notation", "scientific" } };
    NumberFormat *numFmt = new (std::nothrow) NumberFormat(locales, options);
    if (!numFmt) {
        EXPECT_TRUE(false);
        return;
    }
    string out = numFmt->Format(123456.789);
    EXPECT_EQ(out, expects);
    EXPECT_EQ(numFmt->GetStyle(), style);
    delete numFmt;
}

/**
 * @tc.name: IntlFuncTest0011
 * @tc.desc: Test Intl NumberFormat.format
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest, IntlFuncTest011, TestSize.Level1)
{
    string locale = "en-CN";
    string expects = "123 thousand";
    vector<string> locales;
    locales.push_back(locale);
    string style = "decimal";
    map<string, string> options = { { "style", style },
                                    { "notation", "compact" },
                                    { "compactDisplay", "long" } };
    NumberFormat *numFmt = new (std::nothrow) NumberFormat(locales, options);
    if (!numFmt) {
        EXPECT_TRUE(false);
        return;
    }
    string out = numFmt->Format(123456.789);
    EXPECT_EQ(out, expects);
    EXPECT_EQ(numFmt->GetStyle(), style);
    delete numFmt;
}

/**
 * @tc.name: IntlFuncTest0012
 * @tc.desc: Test Intl DateTimeFormat
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest, IntlFuncTest0012, TestSize.Level1)
{
    string locale = "en";
    map<string, string> options = {};
    vector<string> locales;
    locales.push_back(locale);
    std::string expects = "3/11/82";
    DateTimeFormat *dateFormat = new (std::nothrow) DateTimeFormat(locales, options);
    if (!dateFormat) {
        EXPECT_TRUE(false);
        return;
    }
    int64_t milliseconds = 123456789123456;
    string out = dateFormat->Format(milliseconds);
    EXPECT_EQ(out, expects);
    options = {
        { "dateStyle", "long" },
        { "hourCycle", "h11" },
        { "hour12", "fakeValue" }
    };
    DateTimeFormat *dateFormatH11 = new (std::nothrow) DateTimeFormat(locales, options);
    options.insert({ "hourCycle", "h12" });
    DateTimeFormat *dateFormatH12 = new (std::nothrow) DateTimeFormat(locales, options);
    options.insert({ "hourCycle", "h23" });
    DateTimeFormat *dateFormatH23 = new (std::nothrow) DateTimeFormat(locales, options);
    options.insert({ "hourCycle", "h24" });
    DateTimeFormat *dateFormatH24 = new (std::nothrow) DateTimeFormat(locales, options);
    delete dateFormatH11;
    delete dateFormatH12;
    delete dateFormatH23;
    delete dateFormatH24;
    locales.clear();
    DateTimeFormat *dateFormatEmpty = new (std::nothrow) DateTimeFormat(locales, options);
    locales.push_back("@@@&&");
    options = { { "dayPeriod", "short" } };
    DateTimeFormat *dateFormatFake = new (std::nothrow) DateTimeFormat(locales, options);
    options.insert({ "dayPeriod", "long" });
    DateTimeFormat *dateFormatDayPeriod = new (std::nothrow) DateTimeFormat(locales, options);
    delete dateFormatDayPeriod;
    options.insert({ "dayPeriod", "narrow" });
    DateTimeFormat *dateFormatDayPeriod2 = new (std::nothrow) DateTimeFormat(locales, options);
    delete dateFormatDayPeriod2;
    options.insert({ "dateStyle", "long" });
    DateTimeFormat *dateFormatFake2 = new (std::nothrow) DateTimeFormat(locales, options);
    delete dateFormatEmpty;
    delete dateFormatFake;
    delete dateFormatFake2;
    delete dateFormat;
}

/**
 * @tc.name: IntlFuncTest0013
 * @tc.desc: Test Intl LocaleInfo
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest, IntlFuncTest0013, TestSize.Level1)
{
    string locale = "en-CN";
    vector<string> locales;
    locales.push_back(locale);
    map<string, string> options = {};
    std::string expects = "123,456.789";
    NumberFormat *numFmt = new (std::nothrow) NumberFormat(locales, options);
    if (!numFmt) {
        EXPECT_TRUE(false);
        return;
    }
    string out = numFmt->Format(123456.789);
    EXPECT_EQ(out, expects);
    delete numFmt;
}

/**
 * @tc.name: IntlFuncTest0014
 * @tc.desc: Test Intl LocaleInfo
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest, IntlFuncTest0014, TestSize.Level1)
{
    string locale = "zh-CN-u-hc-h12";
    string expects = "北美太平洋标准时间";
    vector<string> locales;
    locales.push_back("jessie");
    locales.push_back(locale);
    map<string, string> options = { { "timeZone", "America/Los_Angeles"  }, { "timeZoneName", "long" } };
    DateTimeFormat *dateFormat = new (std::nothrow) DateTimeFormat(locales, options);
    if (!dateFormat) {
        EXPECT_TRUE(false);
        return;
    }
    int64_t milliseconds = 123456789;
    string out = dateFormat->Format(milliseconds);
    EXPECT_TRUE(out.find(expects) != out.npos);
    delete dateFormat;
}

/**
 * @tc.name: IntlFuncTest0015
 * @tc.desc: Test Intl LocaleInfo
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest, IntlFuncTest0015, TestSize.Level1)
{
    string locale = "zh-CN-u-hc-h12";
    vector<string> locales;
    locales.push_back("jessie");
    locales.push_back(locale);
    map<string, string> options = { { "timeZone", "America/Los_Angeles"  }, { "timeZoneName", "short" } };
    DateTimeFormat *dateFormat = new (std::nothrow) DateTimeFormat(locales, options);
    if (!dateFormat) {
        EXPECT_TRUE(false);
        return;
    }
    EXPECT_EQ(dateFormat->GetTimeZone(), "America/Los_Angeles");
    EXPECT_EQ(dateFormat->GetTimeZoneName(), "short");
    delete dateFormat;
}

/**
 * @tc.name: IntlFuncTest0016
 * @tc.desc: Test Intl Collator
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest, IntlFuncTest0016, TestSize.Level1)
{
    // normal test case
    vector<string> locales;
    locales.push_back("en-US");
    map<string, string> inputOptions;
    Collator *collator = new Collator(locales, inputOptions);
    const string param1 = "abc";
    const string param2 = "cba";
    CompareResult result = collator->Compare(param1, param2);
    EXPECT_EQ(result, CompareResult::SMALLER);
    
    map<string, string> options;
    collator->ResolvedOptions(options);
    EXPECT_EQ(options.size(), 8);
    map<string, string>::iterator it = options.find("localeMatcher");
    if (it != options.end()) {
        EXPECT_EQ(it->second, "best fit");
    }
    it = options.find("locale");
    if (it != options.end()) {
        EXPECT_EQ(it->second, "en-US");
    }
    it = options.find("usage");
    if (it != options.end()) {
        EXPECT_EQ(it->second, "sort");
    }
    it = options.find("sensitivity");
    if (it != options.end()) {
        EXPECT_EQ(it->second, "variant");
    }
    it = options.find("ignorePunctuation");
    if (it != options.end()) {
        EXPECT_EQ(it->second, "false");
    }
    it = options.find("numeric");
    if (it != options.end()) {
        EXPECT_EQ(it->second, "false");
    }
    it = options.find("caseFirst");
    if (it != options.end()) {
        EXPECT_EQ(it->second, "false");
    }
    it = options.find("collation");
    if (it != options.end()) {
        EXPECT_EQ(it->second, "default");
    }
    delete collator;
}

/**
 * @tc.name: IntlFuncTest0017
 * @tc.desc: Test Intl Collator
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest, IntlFuncTest0017, TestSize.Level1)
{
    // normal test case
    vector<string> locales;
    locales.push_back("zh-Hans");
    locales.push_back("en-US");
    map<string, string> inputOptions = {
        { "localeMatcher", "lookup" },
        { "usage", "search"},
        { "sensitivity", "case"},
        { "ignorePunctuation", "true" },
        { "collation", "pinyin"},
        { "numeric", "true"},
        { "caseFirst", "upper"}
    };
    Collator *collator = new Collator(locales, inputOptions);
    const string param1 = "啊";
    const string param2 = "播";
    CompareResult result = collator->Compare(param1, param2);
    EXPECT_EQ(result, CompareResult::SMALLER);
    map<string, string> options;
    collator->ResolvedOptions(options);
    EXPECT_EQ(options.size(), 8);
    map<string, string>::iterator it = options.find("localeMatcher");
    if (it != options.end()) {
        EXPECT_EQ(it->second, "lookup");
    }
    it = options.find("locale");
    if (it != options.end()) {
        EXPECT_EQ(it->second, "zh-Hans");
    }
    it = options.find("usage");
    if (it != options.end()) {
        EXPECT_EQ(it->second, "search");
    }
    it = options.find("sensitivity");
    if (it != options.end()) {
        EXPECT_EQ(it->second, "case");
    }
    delete collator;
}

/**
 * @tc.name: IntlFuncTest0018
 * @tc.desc: Test Intl Collator
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest, IntlFuncTest0018, TestSize.Level1)
{
    // normal test case
    vector<string> locales;
    locales.push_back("zh-Hans");
    locales.push_back("en-US");
    map<string, string> inputOptions = {
        { "ignorePunctuation", "true" },
        { "collation", "pinyin"},
        { "numeric", "true"},
        { "caseFirst", "upper"}
    };
    Collator *collator = new Collator(locales, inputOptions);
    map<string, string> options;
    collator->ResolvedOptions(options);
    map<string, string>::iterator it = options.find("ignorePunctuation");
    if (it != options.end()) {
        EXPECT_EQ(it->second, "true");
    }
    it = options.find("numeric");
    if (it != options.end()) {
        EXPECT_EQ(it->second, "true");
    }
    it = options.find("caseFirst");
    if (it != options.end()) {
        EXPECT_EQ(it->second, "upper");
    }
    it = options.find("collation");
    if (it != options.end()) {
        EXPECT_EQ(it->second, "pinyin");
    }
    inputOptions = {
        { "sensitivity", "base"},
        { "caseFirst", "lower"}
    };
    Collator *collator2 = new Collator(locales, inputOptions);
    inputOptions = {
        { "sensitivity", "accent"},
        { "caseFirst", "lower"}
    };
    Collator *collator3 = new Collator(locales, inputOptions);
    delete collator;
    delete collator2;
    delete collator3;
}

/**
 * @tc.name: IntlFuncTest0019
 * @tc.desc: Test Intl Collator
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest, IntlFuncTest0019, TestSize.Level1)
{
    // abnormal test case
    vector<string> locales;
    locales.push_back("7776@");
    map<string, string> inputOptions = {
        { "localeMatcher", "fake value" },
        { "usage", "fake value"},
        { "sensitivity", "fake value"},
        { "ignorePunctuation", "fake value" },
        { "collation", "fake value"},
        { "numeric", "fake value"},
        { "caseFirst", "fake value"},
        { "fake key", "fake value"}
    };
    Collator *collator = new Collator(locales, inputOptions);
    const string param1 = "abc";
    const string param2 = "cba";
    CompareResult result = collator->Compare(param1, param2);
    EXPECT_EQ(result, CompareResult::SMALLER);
    
    map<string, string> options;
    collator->ResolvedOptions(options);
    EXPECT_EQ(options.size(), 8);
    map<string, string>::iterator it = options.find("localeMatcher");
    if (it != options.end()) {
        EXPECT_EQ(it->second, "fake value");
    }
    std::string systemLocale = LocaleConfig::GetSystemLocale();
    it = options.find("locale");
    if (it != options.end()) {
        EXPECT_EQ(it->second, systemLocale);
    }
    it = options.find("usage");
    if (it != options.end()) {
        EXPECT_EQ(it->second, "fake value");
    }
    it = options.find("sensitivity");
    if (it != options.end()) {
        EXPECT_EQ(it->second, "fake value");
    }
    delete collator;
}

/**
 * @tc.name: IntlFuncTest0020
 * @tc.desc: Test Intl Collator
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest, IntlFuncTest0020, TestSize.Level1)
{
    // abnormal test case
    vector<string> locales;
    locales.push_back("$$##");
    map<string, string> inputOptions = {
        { "ignorePunctuation", "fake value" },
        { "collation", "fake value"},
        { "numeric", "fake value"},
        { "caseFirst", "fake value"},
        { "fake key", "fake value"}
    };
    Collator *collator = new Collator(locales, inputOptions);
    map<string, string> options;
    collator->ResolvedOptions(options);
    map<string, string>::iterator it = options.find("ignorePunctuation");
    if (it != options.end()) {
        EXPECT_EQ(it->second, "fake value");
    }
    it = options.find("numeric");
    if (it != options.end()) {
        EXPECT_EQ(it->second, "fake value");
    }
    it = options.find("caseFirst");
    if (it != options.end()) {
        EXPECT_EQ(it->second, "fake value");
    }
    it = options.find("collation");
    if (it != options.end()) {
        EXPECT_EQ(it->second, "default");
    }
    delete collator;
}

/**
 * @tc.name: IntlFuncTest0021
 * @tc.desc: Test Intl PluralRules
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest, IntlFuncTest0021, TestSize.Level1)
{
    // normal test case
    vector<string> locales;
    locales.push_back("en-US");
    map<string, string> options;
    PluralRules *plurals = new PluralRules(locales, options);
    string res = plurals->Select(0);
    EXPECT_EQ(res, "other");
    res = plurals->Select(1);
    EXPECT_EQ(res, "one");
    res = plurals->Select(2);
    EXPECT_EQ(res, "other");
    res = plurals->Select(5);
    EXPECT_EQ(res, "other");
    res = plurals->Select(20);
    EXPECT_EQ(res, "other");
    res = plurals->Select(200);
    EXPECT_EQ(res, "other");
    res = plurals->Select(12.34);
    EXPECT_EQ(res, "other");
    delete plurals;
}

/**
 * @tc.name: IntlFuncTest0022
 * @tc.desc: Test Intl PluralRules
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest, IntlFuncTest0022, TestSize.Level1)
{
    // normal test case
    vector<string> locales;
    locales.push_back("ar");
    map<string, string> options;
    PluralRules *plurals = new PluralRules(locales, options);
    string res = plurals->Select(0);
    EXPECT_EQ(res, "zero");
    res = plurals->Select(1);
    EXPECT_EQ(res, "one");
    res = plurals->Select(2);
    EXPECT_EQ(res, "two");
    res = plurals->Select(5);
    EXPECT_EQ(res, "few");
    res = plurals->Select(20);
    EXPECT_EQ(res, "many");
    res = plurals->Select(200);
    EXPECT_EQ(res, "other");
    res = plurals->Select(12.34);
    EXPECT_EQ(res, "other");
    options = {
        { "localeMatcher", "best fit" },
        { "type", "cardinal" },
        { "minimumIntegerDigits", "1" },
        { "minimumFractionDigits", "0" },
        { "maximumFractionDigits", "21" },
    };
    PluralRules *pluralRules = new PluralRules(locales, options);
    delete pluralRules;
    delete plurals;
}

/**
 * @tc.name: IntlFuncTest0023
 * @tc.desc: Test Intl PluralRules
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest, IntlFuncTest0023, TestSize.Level1)
{
    // normal test case
    vector<string> locales;
    locales.push_back("ar");
    map<string, string> options = {
        { "localeMatcher", "best fit" },
        { "type", "cardinal" },
        { "minimumIntegerDigits", "1" },
        { "minimumFractionDigits", "0" },
        { "maximumFractionDigits", "21" },
        { "minimumSignificantDigits", "21" },
        { "maximumSignificantDigits", "21" },
    };
    PluralRules *plurals = new PluralRules(locales, options);
    string res = plurals->Select(0);
    EXPECT_EQ(res, "zero");
    res = plurals->Select(1);
    EXPECT_EQ(res, "one");
    res = plurals->Select(2);
    EXPECT_EQ(res, "two");
    res = plurals->Select(5);
    EXPECT_EQ(res, "few");
    res = plurals->Select(20);
    EXPECT_EQ(res, "many");
    res = plurals->Select(200);
    EXPECT_EQ(res, "other");
    res = plurals->Select(12.34);
    EXPECT_EQ(res, "other");
    delete plurals;
}

/**
 * @tc.name: IntlFuncTest0024
 * @tc.desc: Test Intl PluralRules
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest, IntlFuncTest0024, TestSize.Level1)
{
    // abnormal test cast
    // normal test case
    vector<string> locales;
    locales.push_back("$$$###");
    map<string, string> options = {
        { "fake_localeMatcher", "best fit" },
        { "type", "fake_cardinal" },
        { "minimumIntegerDigits", "11111" },
        { "minimumFractionDigits", "-111" },
        { "maximumFractionDigits", "-111" },
        { "minimumSignificantDigits", "11111" },
        { "maximumSignificantDigits", "11111" },
    };
    PluralRules *plurals = new PluralRules(locales, options);
    string res = plurals->Select(0);
    EXPECT_EQ(res, "other");
    res = plurals->Select(1);
    EXPECT_EQ(res, "other");
    res = plurals->Select(2);
    EXPECT_EQ(res, "other");
    res = plurals->Select(5);
    EXPECT_EQ(res, "other");
    res = plurals->Select(20);
    EXPECT_EQ(res, "other");
    res = plurals->Select(200);
    EXPECT_EQ(res, "other");
    res = plurals->Select(12.34);
    EXPECT_EQ(res, "other");
    delete plurals;
}

/**
 * @tc.name: IntlFuncTest0025
 * @tc.desc: Test Intl RelativeTimeFormat
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest, IntlFuncTest0025, TestSize.Level1)
{
    vector<string> locales;
    locales.push_back("en-US");
    map<string, string> options;
    RelativeTimeFormat *formatter = new RelativeTimeFormat(locales, options);
    
    double number = 2022;
    string unit = "year";
    string res = formatter->Format(number, unit);
    EXPECT_EQ(res, "in 2,022 years");
    vector<vector<string>> timeVector;
    formatter->FormatToParts(number, unit, timeVector);
    EXPECT_EQ(timeVector.size(), 5);

    number = 3;
    unit = "quarter";
    res = formatter->Format(number, unit);
    EXPECT_EQ(res, "in 3 quarters");
    formatter->FormatToParts(number, unit, timeVector);
    EXPECT_EQ(timeVector.size(), 8);

    number = 11;
    unit = "month";
    res = formatter->Format(number, unit);
    EXPECT_EQ(res, "in 11 months");
    formatter->FormatToParts(number, unit, timeVector);
    EXPECT_EQ(timeVector.size(), 11);

    number = 2;
    unit = "week";
    res = formatter->Format(number, unit);
    EXPECT_EQ(res, "in 2 weeks");
    formatter->FormatToParts(number, unit, timeVector);
    EXPECT_EQ(timeVector.size(), 14);

    number = 18;
    unit = "day";
    res = formatter->Format(number, unit);
    EXPECT_EQ(res, "in 18 days");
    formatter->FormatToParts(number, unit, timeVector);
    EXPECT_EQ(timeVector.size(), 17);
    delete formatter;
}

/**
 * @tc.name: IntlFuncTest0026
 * @tc.desc: Test Intl RelativeTimeFormat
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest, IntlFuncTest0026, TestSize.Level1)
{
    vector<string> locales;
    locales.push_back("en-US");
    map<string, string> options;
    RelativeTimeFormat *formatter = new RelativeTimeFormat(locales, options);
    
    double number = 23;
    string unit = "hour";
    string res = formatter->Format(number, unit);
    EXPECT_EQ(res, "in 23 hours");
    vector<vector<string>> timeVector;
    formatter->FormatToParts(number, unit, timeVector);
    EXPECT_EQ(timeVector.size(), 3);

    number = 59;
    unit = "minute";
    res = formatter->Format(number, unit);
    EXPECT_EQ(res, "in 59 minutes");
    formatter->FormatToParts(number, unit, timeVector);
    EXPECT_EQ(timeVector.size(), 6);

    number = 1;
    unit = "second";
    res = formatter->Format(number, unit);
    EXPECT_EQ(res, "in 1 second");
    formatter->FormatToParts(number, unit, timeVector);
    EXPECT_EQ(timeVector.size(), 9);

    delete formatter;
}

/**
 * @tc.name: IntlFuncTest0027
 * @tc.desc: Test Intl RelativeTimeFormat
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest, IntlFuncTest0027, TestSize.Level1)
{
    vector<string> locales;
    locales.push_back("zh-Hans");
    map<string, string> options = {
        { "localeMatcher", "best fit" },
        { "numeric", "auto" },
        { "style", "long" }
    };
    RelativeTimeFormat *formatter = new RelativeTimeFormat(locales, options);
    
    double number = 2022;
    string unit = "year";
    string res = formatter->Format(number, unit);
    EXPECT_EQ(res, "2,022年后");
    vector<vector<string>> timeVector;
    formatter->FormatToParts(number, unit, timeVector);
    EXPECT_EQ(timeVector.size(), 4);

    number = 3;
    unit = "quarter";
    res = formatter->Format(number, unit);
    EXPECT_EQ(res, "3个季度后");
    formatter->FormatToParts(number, unit, timeVector);
    EXPECT_EQ(timeVector.size(), 6);

    number = 11;
    unit = "month";
    res = formatter->Format(number, unit);
    EXPECT_EQ(res, "11个月后");
    formatter->FormatToParts(number, unit, timeVector);
    EXPECT_EQ(timeVector.size(), 8);

    number = 2;
    unit = "week";
    res = formatter->Format(number, unit);
    EXPECT_EQ(res, "2周后");
    formatter->FormatToParts(number, unit, timeVector);
    EXPECT_EQ(timeVector.size(), 10);
    delete formatter;
}

/**
 * @tc.name: IntlFuncTest0028
 * @tc.desc: Test Intl RelativeTimeFormat
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest, IntlFuncTest0028, TestSize.Level1)
{
    vector<string> locales;
    locales.push_back("zh-Hans");
    map<string, string> options = {
        { "localeMatcher", "lookup" },
        { "numeric", "auto" },
        { "style", "long" }
    };
    RelativeTimeFormat *formatter = new RelativeTimeFormat(locales, options);
    
    double number = 18;
    string unit = "day";
    string res = formatter->Format(number, unit);
    EXPECT_EQ(res, "18天后");
    vector<vector<string>> timeVector;
    formatter->FormatToParts(number, unit, timeVector);
    EXPECT_EQ(timeVector.size(), 2);

    number = 23;
    unit = "hour";
    res = formatter->Format(number, unit);
    EXPECT_EQ(res, "23小时后");
    formatter->FormatToParts(number, unit, timeVector);
    EXPECT_EQ(timeVector.size(), 4);

    number = 59;
    unit = "minute";
    res = formatter->Format(number, unit);
    EXPECT_EQ(res, "59分钟后");
    formatter->FormatToParts(number, unit, timeVector);
    EXPECT_EQ(timeVector.size(), 6);

    number = 1;
    unit = "second";
    res = formatter->Format(number, unit);
    EXPECT_EQ(res, "1秒钟后");
    formatter->FormatToParts(number, unit, timeVector);
    EXPECT_EQ(timeVector.size(), 8);

    delete formatter;
}

/**
 * @tc.name: IntlFuncTest0029
 * @tc.desc: Test Intl RelativeTimeFormatter
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest, IntlFuncTest0029, TestSize.Level1)
{
    vector<string> locales;
    locales.push_back("####");
    locales.push_back("zh-Hans-u-nu-latn");
    map<string, string> options = {
        { "localeMatcher", "best fit" },
        { "numeric", "auto" },
        { "style", "long" }
    };
    RelativeTimeFormat *formatter = new RelativeTimeFormat(locales, options);
    map<string, string> res;
    formatter->GetResolvedOptions(res);
    EXPECT_EQ(res.size(), 4);

    map<string, string>::iterator it = res.find("locale");
    if (it != res.end()) {
        EXPECT_EQ(it->second, "zh-Hans");
    }
    it = res.find("style");
    if (it != res.end()) {
        EXPECT_EQ(it->second, "long");
    }
    it = res.find("numeric");
    if (it != res.end()) {
        EXPECT_EQ(it->second, "auto");
    }
    it = res.find("numberingSystem");
    if (it != res.end()) {
        EXPECT_EQ(it->second, "latn");
    }
    delete formatter;
}

/**
 * @tc.name: IntlFuncTest0030
 * @tc.desc: Test Intl NumberFormat
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest, IntlFuncTest0030, TestSize.Level1)
{
    vector<string> locales;
    locales.push_back("en-US");
    map<string, string> options = {
        { "unitUsage", "default" }
    };
    NumberFormat *formatter = new NumberFormat(locales, options);
    
    string res = formatter->Format(123);
    EXPECT_EQ(res, "123");
    res = formatter->Format(123.456);
    EXPECT_EQ(res, "123.456");
    locales.clear();
    locales.push_back("$$@@");
    NumberFormat *formatter2 = new NumberFormat(locales, options);
    string res2 = formatter2->Format(123);
    EXPECT_EQ(res2, "123");
    delete formatter;
    delete formatter2;
}

/**
 * @tc.name: IntlFuncTest0031
 * @tc.desc: Test Intl NumberFormat
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest, IntlFuncTest0031, TestSize.Level1)
{
    vector<string> locales;
    locales.push_back("zh-Hans");
    map<string, string> options = {
        { "locale", "zh-Hans" },
        { "currency", "EUR" },
        { "currencySign", "narrowSymbol" },
        { "currencyDisplay", "symbol" },
        { "unit", "meter" },
        { "unitDisplay", "long" },
        { "unitUsage", "length-person" },
        { "signDisplay", "always" },
        { "compactDisplay", "long" },
        { "notation", "standard" },
        { "localeMatcher", "lookup" },
        { "style", "decimal" },
        { "numberingSystem", "latn" },
        { "useGroup", "true" },
        { "minimumIntegerDigits", "1" },
        { "minimumFractionDigits", "0" },
        { "maximumFractionDigits", "20" },
        { "minimumSignificantDigits", "1" },
        { "maximumSignificantDigits", "20" }
    };
    NumberFormat *formatter = new NumberFormat(locales, options);
    
    string formatRes = formatter->Format(123);
    EXPECT_EQ(formatRes, "+123");
    formatRes = formatter->Format(123.456);
    EXPECT_EQ(formatRes, "+123.456");
    
    map<string, string> res;
    formatter->GetResolvedOptions(res);
    EXPECT_EQ(res.size(), 12);
    map<string, string>::iterator it = res.find("locale");
    if (it != res.end()) {
        EXPECT_EQ(it->second, "zh-Hans");
    }
    it = res.find("signDisplay");
    if (it != res.end()) {
        EXPECT_EQ(it->second, "always");
    }
    it = res.find("notation");
    if (it != res.end()) {
        EXPECT_EQ(it->second, "standard");
    }
    delete formatter;
}

/**
 * @tc.name: IntlFuncTest0032
 * @tc.desc: Test Intl NumberFormat
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest, IntlFuncTest0032, TestSize.Level1)
{
    vector<string> locales;
    locales.push_back("zh-Hans");
    map<string, string> options = {
        { "locale", "zh-Hans" },
        { "currency", "CNY" },
        { "currencySign", "symbol" },
        { "currencyDisplay", "symbol" },
        { "unit", "meter" },
        { "unitDisplay", "long" },
        { "unitUsage", "length-person" },
        { "signDisplay", "always" },
        { "compactDisplay", "long" },
        { "notation", "standard" },
        { "localeMatcher", "best fit" },
        { "style", "decimal" },
        { "numberingSystem", "latn" },
        { "useGroup", "true" },
        { "minimumIntegerDigits", "1" },
        { "minimumFractionDigits", "0" },
        { "maximumFractionDigits", "21" },
        { "minimumSignificantDigits", "1" },
        { "maximumSignificantDigits", "21" }
    };
    NumberFormat *formatter = new NumberFormat(locales, options);
    map<string, string> res;
    formatter->GetResolvedOptions(res);
    map<string, string>::iterator it = res.find("style");
    if (it != res.end()) {
        EXPECT_EQ(it->second, "decimal");
    }
    it = res.find("numberingSystem");
    if (it != res.end()) {
        EXPECT_EQ(it->second, "latn");
    }
    it = res.find("useGrouping");
    if (it != res.end()) {
        EXPECT_EQ(it->second, "true");
    }
    it = res.find("minimumIntegerDigits");
    if (it != res.end()) {
        EXPECT_EQ(it->second, "1");
    }
    it = res.find("minimumFractionDigits");
    if (it != res.end()) {
        EXPECT_EQ(it->second, "0");
    }
    delete formatter;
}

/**
 * @tc.name: IntlFuncTest0033
 * @tc.desc: Test Intl NumberFormat
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest, IntlFuncTest0033, TestSize.Level1)
{
    vector<string> locales;
    locales.push_back("zh-Hans");
    map<string, string> options = {
        { "locale", "@@##" },
        { "currency", "fake currency" },
        { "currencySign", "fake sign" },
        { "currencyDisplay", "symbol" },
        { "unit", "meter" },
        { "unitDisplay", "fake value" },
        { "unitUsage", "length-person" },
        { "signDisplay", "always" },
        { "compactDisplay", "long" },
        { "notation", "fake value" },
        { "localeMatcher", "best fit" },
        { "style", "decimal" },
        { "numberingSystem", "latn" },
        { "useGroup", "fake value" },
        { "minimumIntegerDigits", "1" },
        { "minimumFractionDigits", "0" },
        { "maximumFractionDigits", "21" },
        { "minimumSignificantDigits", "1" },
        { "maximumSignificantDigits", "21" }
    };
    NumberFormat *formatter = new NumberFormat(locales, options);
    map<string, string> res;
    formatter->GetResolvedOptions(res);
    map<string, string>::iterator it = res.find("maximumFractionDigits");
    if (it != res.end()) {
        EXPECT_EQ(it->second, "21");
    }
    it = res.find("minimumSignificantDigits");
    if (it != res.end()) {
        EXPECT_EQ(it->second, "1");
    }
    it = res.find("maximumSignificantDigits");
    if (it != res.end()) {
        EXPECT_EQ(it->second, "21");
    }
    delete formatter;
}

/**
 * @tc.name: IntlFuncTest0034
 * @tc.desc: Test Intl NumberFormat
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest, IntlFuncTest0034, TestSize.Level1)
{
    vector<string> locales;
    locales.push_back("en-US");
    map<string, string> options = {
        { "locale", "$$$##43" },
        { "currency", "USD" },
        { "currencySign", "symbol" },
        { "currencyDisplay", "symbol" },
        { "unit", "fake unit" },
        { "unitDisplay", "long" },
        { "unitUsage", "fake usage" },
        { "signDisplay", "always" },
        { "compactDisplay", "long" },
        { "notation", "standard" },
        { "localeMatcher", "lookup" },
        { "style", "currency" },
        { "numberingSystem", "latn" },
        { "useGrouping", "true" },
        { "minimumIntegerDigits", "1" },
        { "minimumFractionDigits", "0" },
        { "maximumFractionDigits", "20" },
        { "minimumSignificantDigits", "1" },
        { "maximumSignificantDigits", "20" }
    };
    NumberFormat *formatter = new NumberFormat(locales, options);
    string res = formatter->Format(123);
    EXPECT_EQ(res, "+$123");
    res = formatter->Format(123.456);
    EXPECT_EQ(res, "+$123.456");
    res = formatter->GetCurrency();
    EXPECT_EQ(res, "USD");
    res = formatter->GetCurrencySign();
    EXPECT_EQ(res, "symbol");
    res = formatter->GetStyle();
    EXPECT_EQ(res, "currency");
    res = formatter->GetNumberingSystem();
    EXPECT_EQ(res, "latn");
    delete formatter;
}

/**
 * @tc.name: IntlFuncTest0035
 * @tc.desc: Test Intl NumberFormat
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest, IntlFuncTest0035, TestSize.Level1)
{
    vector<string> locales;
    locales.push_back("en-US");
    map<string, string> options = {
        { "locale", "fake locale" },
        { "currency", "USD" },
        { "currencySign", "fake sign" },
        { "currencyDisplay", "symbol" },
        { "unit", "fake unit" },
        { "unitDisplay", "long" },
        { "unitUsage", "length-person" },
        { "signDisplay", "fake display" },
        { "compactDisplay", "long" },
        { "notation", "standard" },
        { "localeMatcher", "lookup" },
        { "style", "currency" },
        { "numberingSystem", "latn" },
        { "useGrouping", "false" },
        { "minimumIntegerDigits", "1" },
        { "minimumFractionDigits", "0" },
        { "maximumFractionDigits", "17" },
        { "minimumSignificantDigits", "1" },
        { "maximumSignificantDigits", "17" }
    };
    NumberFormat *formatter = new NumberFormat(locales, options);
    string res = formatter->GetUseGrouping();
    EXPECT_EQ(res, "false");
    res = formatter->GetMinimumIntegerDigits();
    EXPECT_EQ(res, "1");
    res = formatter->GetMinimumFractionDigits();
    EXPECT_EQ(res, "0");
    res = formatter->GetMaximumFractionDigits();
    EXPECT_EQ(res, "17");
    res = formatter->GetMinimumSignificantDigits();
    EXPECT_EQ(res, "1");
    res = formatter->GetMaximumSignificantDigits();
    EXPECT_EQ(res, "17");
    delete formatter;
}

/**
 * @tc.name: IntlFuncTest0036
 * @tc.desc: Test Intl DateTimeFormat
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest, IntlFuncTest0036, TestSize.Level1)
{
    vector<string> locales;
    locales.push_back("en-US");
    map<string, string> options;
    DateTimeFormat *formatter = new DateTimeFormat(locales, options);

    int64_t milliseconds = 123456789;
    string res = formatter->Format(milliseconds);
    EXPECT_EQ(res, "1/2/70");
    
    int64_t milliseconds2 = 987654321;
    res = formatter->FormatRange(milliseconds, milliseconds2);
    EXPECT_EQ(res, "1/2/70 \xE2\x80\x93 1/12/70");
    delete formatter;
}

/**
 * @tc.name: IntlFuncTest0037
 * @tc.desc: Test Intl DateTimeFormat
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest, IntlFuncTest0037, TestSize.Level1)
{
    vector<string> locales;
    locales.push_back("zh-CN");
    map<string, string> options = {
        { "locale", "zh-CN" },
        { "dateStyle", "medium" },
        { "timeStyle", "long" },
        { "hourCycle", "h24" },
        { "timeZone", "Asia/Shanghai" },
        { "numberingSystem", "latn" },
        { "hour12", "false" },
        { "weekday", "long" },
        { "era", "long" },
        { "year", "2-digit" },
        { "month", "2-digit" },
        { "day", "2-digit" },
        { "hour", "2-digit" },
        { "minute", "2-digit" },
        { "second", "2-digit" },
        { "timeZoneName", "long" },
        { "dayPeriod", "long" },
        { "localeMatcher", "lookup" },
        { "formatMatcher", "basic" }
    };
    DateTimeFormat *formatter = new DateTimeFormat(locales, options);

    int64_t milliseconds = 123456789;
    string res = formatter->Format(milliseconds);
    // 2022年12月19日 GMT+8 15:18:24
    EXPECT_TRUE(res.length() > 0);
    
    int64_t milliseconds2 = 987654321;
    res = formatter->FormatRange(milliseconds, milliseconds2);
    // 2022/12/19 GMT+8 15:18:24 \xE2\x80\x93 2023/11/18 GMT+8 14:17:23
    EXPECT_TRUE(res.length() > 0);

    res = formatter->GetDateStyle();
    EXPECT_EQ(res, "medium");
    res = formatter->GetTimeStyle();
    EXPECT_EQ(res, "long");
    res = formatter->GetHourCycle();
    EXPECT_EQ(res, "h24");
    res = formatter->GetTimeZone();
    EXPECT_EQ(res, "Asia/Shanghai");
    res = formatter->GetTimeZoneName();
    EXPECT_EQ(res, "long");
    delete formatter;
}

/**
 * @tc.name: IntlFuncTest0038
 * @tc.desc: Test Intl DateTimeFormat
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest, IntlFuncTest0038, TestSize.Level1)
{
    vector<string> locales;
    locales.push_back("zh-CN");
    map<string, string> options = {
        { "locale", "zh-CN" },
        { "dateStyle", "full" },
        { "timeStyle", "full" },
        { "hourCycle", "h24" },
        { "timeZone", "Asia/Shanghai" },
        { "numberingSystem", "latn" },
        { "hour12", "false" },
        { "weekday", "long" },
        { "era", "long" },
        { "year", "numeric" },
        { "month", "numeric" },
        { "day", "numeric" },
        { "hour", "numeric" },
        { "minute", "numeric" },
        { "second", "numeric" },
        { "timeZoneName", "long" },
        { "dayPeriod", "long" },
        { "localeMatcher", "lookup" },
        { "formatMatcher", "basic" }
    };
    DateTimeFormat *formatter = new DateTimeFormat(locales, options);
    string res = formatter->GetNumberingSystem();
    EXPECT_EQ(res, "latn");
    res = formatter->GetHour12();
    EXPECT_EQ(res, "false");
    res = formatter->GetWeekday();
    EXPECT_EQ(res, "long");
    res = formatter->GetEra();
    EXPECT_EQ(res, "long");
    res = formatter->GetYear();
    EXPECT_EQ(res, "numeric");
    res = formatter->GetMonth();
    EXPECT_EQ(res, "numeric");
    res = formatter->GetDay();
    EXPECT_EQ(res, "numeric");
    res = formatter->GetHour();
    EXPECT_EQ(res, "numeric");
    res = formatter->GetMinute();
    EXPECT_EQ(res, "numeric");
    res = formatter->GetSecond();
    EXPECT_EQ(res, "numeric");
    delete formatter;
}

/**
 * @tc.name: IntlFuncTest0039
 * @tc.desc: Test Intl DateTimeFormat
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest, IntlFuncTest0039, TestSize.Level1)
{
    vector<string> locales;
    locales.push_back("en-US");
    map<string, string> inputOptions = {
        { "locale", "en-US" },
        { "dateStyle", "medium" },
        { "timeStyle", "long" },
        { "hourCycle", "h12" },
        { "numberingSystem", "latn" },
        { "hour12", "true" },
        { "weekday", "long" },
        { "era", "long" },
        { "year", "2-digit" },
        { "month", "2-digit" },
        { "day", "2-digit" },
        { "hour", "2-digit" },
        { "minute", "2-digit" },
        { "second", "2-digit" },
        { "timeZoneName", "long" },
        { "dayPeriod", "long" },
        { "localeMatcher", "lookup" },
        { "formatMatcher", "basic" }
    };
    std::unique_ptr<DateTimeFormat> formatter = DateTimeFormat::CreateInstance(locales, inputOptions);
    int64_t milliseconds = 123456789;
    string res = formatter->Format(milliseconds);
    // Dec 19, 2022, 3:18:24 PM GMT+8
    EXPECT_TRUE(res.length() > 0);
    int64_t milliseconds2 = 987654321;
    res = formatter->FormatRange(milliseconds, milliseconds2);
    // Dec 19, 2022, 3:18:24 PM GMT+8 \xE2\x80\x93 Nov 18, 2023, 2:17:23 PM GMT+8
    EXPECT_TRUE(res.length() > 0);
    map<string, string> options;
    formatter->GetResolvedOptions(options);
    EXPECT_EQ(options.size(), 20);
    map<string, string>::iterator it = options.find("locale");
    if (it != options.end()) {
        EXPECT_EQ(it->second, "en-US");
    }
    it = options.find("dateStyle");
    if (it != options.end()) {
        EXPECT_EQ(it->second, "medium");
    }
    it = options.find("timeStyle");
    if (it != options.end()) {
        EXPECT_EQ(it->second, "long");
    }
}

/**
 * @tc.name: IntlFuncTest0040
 * @tc.desc: Test Intl DateTimeFormat
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest, IntlFuncTest0040, TestSize.Level1)
{
    vector<string> locales;
    locales.push_back("en-GB");
    map<string, string> inputOptions = {
        { "locale", "en-GB" },
        { "dateStyle", "medium" },
        { "timeStyle", "long" },
        { "hourCycle", "h12" },
        { "timeZone", "Asia/Shanghai" },
        { "numberingSystem", "latn" },
        { "hour12", "true" },
        { "weekday", "long" },
        { "era", "long" },
        { "year", "numeric" },
        { "month", "numeric" },
        { "day", "numeric" },
        { "hour", "numeric" },
        { "minute", "numeric" },
        { "second", "numeric" },
        { "timeZoneName", "long" },
        { "dayPeriod", "long" },
        { "localeMatcher", "lookup" },
        { "formatMatcher", "basic" }
    };
    std::unique_ptr<DateTimeFormat> formatter = DateTimeFormat::CreateInstance(locales, inputOptions);
    map<string, string> options;
    formatter->GetResolvedOptions(options);
    map<string, string>::iterator it = options.find("hourCycle");
    if (it != options.end()) {
        EXPECT_EQ(it->second, "h12");
    }
    it = options.find("timeZone");
    if (it != options.end()) {
        EXPECT_EQ(it->second, "Asia/Shanghai");
    }
    it = options.find("numberingSystem");
    if (it != options.end()) {
        EXPECT_EQ(it->second, "latn");
    }
    it = options.find("hour12");
    if (it != options.end()) {
        EXPECT_EQ(it->second, "true");
    }
    it = options.find("weekday");
    if (it != options.end()) {
        EXPECT_EQ(it->second, "long");
    }
}

/**
 * @tc.name: IntlFuncTest0041
 * @tc.desc: Test Intl DateTimeFormat
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest, IntlFuncTest0041, TestSize.Level1)
{
    vector<string> locales;
    locales.push_back("en-US");
    map<string, string> inputOptions = {
        { "locale", "en-US" },
        { "dateStyle", "medium" },
        { "timeStyle", "medium" },
        { "hourCycle", "h24" },
        { "timeZone", "Asia/Shanghai" },
        { "numberingSystem", "latn" },
        { "hour12", "false" },
        { "weekday", "long" },
        { "era", "long" },
        { "year", "2-digit" },
        { "month", "2-digit" },
        { "day", "2-digit" },
        { "hour", "2-digit" },
        { "minute", "2-digit" },
        { "second", "2-digit" },
        { "timeZoneName", "long" },
        { "dayPeriod", "long" },
        { "localeMatcher", "best fit" },
        { "formatMatcher", "best fit" }
    };
    std::unique_ptr<DateTimeFormat> formatter = DateTimeFormat::CreateInstance(locales, inputOptions);
    map<string, string> options;
    formatter->GetResolvedOptions(options);
    map<string, string>::iterator it = options.find("era");
    if (it != options.end()) {
        EXPECT_EQ(it->second, "long");
    }
    it = options.find("year");
    if (it != options.end()) {
        EXPECT_EQ(it->second, "2-digit");
    }
    it = options.find("month");
    if (it != options.end()) {
        EXPECT_EQ(it->second, "2-digit");
    }
    it = options.find("day");
    if (it != options.end()) {
        EXPECT_EQ(it->second, "2-digit");
    }
    it = options.find("hour");
    if (it != options.end()) {
        EXPECT_EQ(it->second, "2-digit");
    }
}

/**
 * @tc.name: IntlFuncTest0042
 * @tc.desc: Test Intl DateTimeFormat
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest, IntlFuncTest0042, TestSize.Level1)
{
    vector<string> locales;
    locales.push_back("en-US");
    map<string, string> inputOptions = {
        { "locale", "en-US" },
        { "dateStyle", "full" },
        { "timeStyle", "long" },
        { "hourCycle", "h12" },
        { "timeZone", "Asia/Singapore" },
        { "numberingSystem", "latn" },
        { "hour12", "true" },
        { "weekday", "long" },
        { "era", "long" },
        { "year", "2-digit" },
        { "month", "2-digit" },
        { "day", "2-digit" },
        { "hour", "numeric" },
        { "minute", "numeric" },
        { "second", "numeric" },
        { "timeZoneName", "long" },
        { "dayPeriod", "long" },
        { "localeMatcher", "lookup" },
        { "formatMatcher", "basic" }
    };
    std::unique_ptr<DateTimeFormat> formatter = DateTimeFormat::CreateInstance(locales, inputOptions);
    map<string, string> options;
    formatter->GetResolvedOptions(options);
    map<string, string>::iterator it = options.find("minute");
    if (it != options.end()) {
        EXPECT_EQ(it->second, "numeric");
    }
    it = options.find("second");
    if (it != options.end()) {
        EXPECT_EQ(it->second, "numeric");
    }
    it = options.find("timeZoneName");
    if (it != options.end()) {
        EXPECT_EQ(it->second, "long");
    }
    it = options.find("dayPeriod");
    if (it != options.end()) {
        EXPECT_EQ(it->second, "long");
    }
}

/**
 * @tc.name: IntlFuncTest0043
 * @tc.desc: Test Intl DateTimeFormat
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest, IntlFuncTest0043, TestSize.Level1)
{
    vector<string> locales;
    locales.push_back("en-US");
    map<string, string> inputOptions = {
        { "locale", "en-US" },
        { "dateStyle", "long" },
        { "timeStyle", "long" },
        { "hourCycle", "h12" },
        { "timeZone", "America/Los_Angeles" },
        { "numberingSystem", "latn" },
        { "hour12", "true" },
        { "weekday", "long" },
        { "era", "long" },
        { "year", "numeric" },
        { "month", "numeric" },
        { "day", "numeric" },
        { "hour", "2-digit" },
        { "minute", "2-digit" },
        { "second", "2-digit" },
        { "timeZoneName", "long" },
        { "dayPeriod", "long" },
        { "localeMatcher", "lookup" },
        { "formatMatcher", "basic" }
    };
    std::unique_ptr<DateTimeFormat> formatter = DateTimeFormat::CreateInstance(locales, inputOptions);
    map<string, string> options;
    formatter->GetResolvedOptions(options);
    map<string, string>::iterator it = options.find("localeMatcher");
    if (it != options.end()) {
        EXPECT_EQ(it->second, "lookup");
    }
    it = options.find("formatMatcher");
    if (it != options.end()) {
        EXPECT_EQ(it->second, "basic");
    }
}

/**
 * @tc.name: IntlFuncTest0044
 * @tc.desc: Test Intl LocaleInfo
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest, IntlFuncTest0044, TestSize.Level1)
{
    string localeTag = "zh-Hans-CN";
    map<string, string> configs = {
        { "calendar", "chinese" },
        { "collation", "pinyin" },
        { "hourCycle", "h12" },
        { "numberingSystem", "latn" },
        { "numeric", "true" },
        { "caseFirst", "upper" }
    };
    LocaleInfo *locale = new LocaleInfo(localeTag, configs);
    string res = locale->GetLanguage();
    EXPECT_EQ(res, "zh");
    res = locale->GetScript();
    EXPECT_EQ(res, "Hans");
    res = locale->GetRegion();
    EXPECT_EQ(res, "CN");
    res = locale->GetBaseName();
    EXPECT_EQ(res, "zh-Hans-CN");
    res = locale->GetCalendar();
    EXPECT_EQ(res, "chinese");
    res = locale->GetCollation();
    EXPECT_EQ(res, "pinyin");
    res = locale->GetHourCycle();
    EXPECT_EQ(res, "h12");
    res = locale->GetNumberingSystem();
    EXPECT_EQ(res, "latn");
    res = locale->GetNumeric();
    EXPECT_EQ(res, "true");
    res = locale->GetCaseFirst();
    EXPECT_EQ(res, "upper");
    icu::Locale icuLocale = locale->GetLocale();
    res = locale->ToString();
    EXPECT_EQ(res, "zh-Hans-CN-u-hc-h12-nu-latn-ca-chinese-co-pinyin-kf-upper-kn-true");
    delete locale;
}

/**
 * @tc.name: IntlFuncTest0045
 * @tc.desc: Test Intl ReadSystemParameter
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest, IntlFuncTest0045, TestSize.Level1)
{
    string paramKey = "const.global.language";
    int paramLength = 128;
    string res = ReadSystemParameter(paramKey.c_str(), paramLength);
    EXPECT_TRUE(res.length() > 0);

    paramKey = "fake system param";
    res = ReadSystemParameter(paramKey.c_str(), paramLength);
    EXPECT_TRUE(res.length() == 0);
}

/**
 * @tc.name: IntlFuncTest0046
 * @tc.desc: Test Intl GetTimezoneIdByLocation
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest, IntlFuncTest0046, TestSize.Level1)
{
    //北京
    vector<std::string> Beijing = I18nTimeZone::GetTimezoneIdByLocation(116.3, 39.5);
    EXPECT_TRUE(Beijing.size() == 1 && Beijing[0] == "Asia/Shanghai");
    //洛杉矶
    vector<std::string> LosAngeles = I18nTimeZone::GetTimezoneIdByLocation(-118.1, 34.0);
    EXPECT_TRUE(LosAngeles.size() == 1 && LosAngeles[0] == "America/Los_Angeles");
    //里约热内卢
    vector<std::string> RIO = I18nTimeZone::GetTimezoneIdByLocation(-43.1, -22.5);
    EXPECT_TRUE(RIO.size() == 1 && RIO[0] == "America/Sao_Paulo");
    //悉尼
    vector<std::string> Sydney = I18nTimeZone::GetTimezoneIdByLocation(150.5, -33.55);
    EXPECT_TRUE(Sydney.size() == 1 && Sydney[0] == "Australia/Sydney");
    //乌鲁木齐
    vector<std::string> Urumqi = I18nTimeZone::GetTimezoneIdByLocation(87.7, 43.8);
    EXPECT_TRUE(Urumqi.size() == 2);
    bool containsShanghai = false;
    bool containsUrumqi = false;
    for (unsigned int i = 0; i < Urumqi.size(); i++) {
        if (Urumqi[i] == "Asia/Shanghai") {
            containsShanghai = true;
        }
        if (Urumqi[i] == "Asia/Urumqi") {
            containsUrumqi = true;
        }
    }
    EXPECT_TRUE(containsShanghai);
    EXPECT_TRUE(containsUrumqi);
}

/**
 * @tc.name: IntlFuncTest0047
 * @tc.desc: Test Intl LocaleConfig GetBlockedLanguages
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest, IntlFuncTest0047, TestSize.Level1)
{
    std::unordered_set<std::string> languageSet = LocaleConfig::GetBlockedLanguages();
    EXPECT_TRUE(languageSet.size() == 1);
}

/**
 * @tc.name: IntlFuncTest0048
 * @tc.desc: Test Intl LocaleConfig GetBlockedRegions
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest, IntlFuncTest0048, TestSize.Level1)
{
    std::unordered_set<std::string> regionSet = LocaleConfig::GetBlockedRegions();
    EXPECT_TRUE(regionSet.size() == 0);
}

/**
 * @tc.name: IntlFuncTest0049
 * @tc.desc: Test Intl LocaleConfig GetLanguageBlockedRegions
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest, IntlFuncTest0049, TestSize.Level1)
{
    std::unordered_set<std::string> blockedRegionSet = LocaleConfig::GetLanguageBlockedRegions();
    EXPECT_EQ(blockedRegionSet.size(), 0);
    I18nErrorCode status = LocaleConfig::SetSystemLanguage("zh-Hans");
    EXPECT_EQ(status, I18nErrorCode::SUCCESS);
    std::string systemLanguage = LocaleConfig::GetSystemLanguage();
    EXPECT_EQ(systemLanguage, "zh-Hans");

    status = LocaleConfig::SetSystemRegion("CN");
    EXPECT_EQ(status, I18nErrorCode::SUCCESS);
    std::string systemRegion = LocaleConfig::GetSystemRegion();
    EXPECT_EQ(systemRegion, "CN");
    std::string systemLocale = LocaleConfig::GetSystemLocale();
    EXPECT_EQ(systemLocale, "zh-Hans-CN");

    const std::string locale = "zh-CN";
    bool isRTL = LocaleConfig::IsRTL(locale);
    EXPECT_TRUE(!isRTL);
    std::string validLocale = LocaleConfig::GetValidLocale(locale);
    EXPECT_EQ(validLocale, "zh-CN");

    status = LocaleConfig::Set24HourClock("false");
    EXPECT_EQ(status, I18nErrorCode::SUCCESS);
    bool is24HourClock = LocaleConfig::Is24HourClock();
    EXPECT_TRUE(!is24HourClock);

    status = LocaleConfig::SetUsingLocalDigit(true);
    EXPECT_EQ(status, I18nErrorCode::UPDATE_LOCAL_DIGIT_FAILED);
    bool usingLocalDigit = LocaleConfig::GetUsingLocalDigit();
    EXPECT_FALSE(usingLocalDigit);
}

/**
 * @tc.name: IntlFuncTest0050
 * @tc.desc: Test Intl PreferredLanguage AddPreferredLanguageExist
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest, IntlFuncTest0050, TestSize.Level1)
{
    // init test environment
    I18nErrorCode status = LocaleConfig::SetSystemLanguage("zh-Hans");
    EXPECT_EQ(status, I18nErrorCode::SUCCESS);
    std::vector<std::string> preferredLanguageList = PreferredLanguage::GetPreferredLanguageList();
    for (auto i = preferredLanguageList.size() - 1; i > 0; --i) {
        status = PreferredLanguage::RemovePreferredLanguage(i);
        EXPECT_EQ(status, I18nErrorCode::SUCCESS);
    }
    // execute test
    const std::string language = "zh-CN";
    const std::string languageDe = "de-DE";
    const std::string fakeLanguage = "1**1";
    status = PreferredLanguage::AddPreferredLanguage(language, 0);
    EXPECT_EQ(status, I18nErrorCode::SUCCESS);
    status = PreferredLanguage::AddPreferredLanguage(languageDe, 1);
    EXPECT_EQ(status, I18nErrorCode::SUCCESS);
    status = PreferredLanguage::AddPreferredLanguage(language, 3);
    EXPECT_EQ(status, I18nErrorCode::SUCCESS);
    PreferredLanguage::AddPreferredLanguage(fakeLanguage, -1);
    PreferredLanguage::AddPreferredLanguage(language, -1);
    std::vector<std::string> list = PreferredLanguage::GetPreferredLanguageList();
    EXPECT_EQ(list.size(), 3);
    std::string firstPreferredLanguage = PreferredLanguage::GetFirstPreferredLanguage();
    EXPECT_EQ(firstPreferredLanguage, "zh-CN");
    std::string preferredLocale = PreferredLanguage::GetPreferredLocale();
    EXPECT_EQ(preferredLocale, "zh-CN");
    status = PreferredLanguage::RemovePreferredLanguage(0);
    EXPECT_EQ(status, I18nErrorCode::SUCCESS);
    // restore environment
    status = LocaleConfig::SetSystemLanguage("zh-Hans");
    EXPECT_EQ(status, I18nErrorCode::SUCCESS);
    preferredLanguageList = PreferredLanguage::GetPreferredLanguageList();
    for (auto i = preferredLanguageList.size() - 1; i > 0; --i) {
        status = PreferredLanguage::RemovePreferredLanguage(i);
        EXPECT_EQ(status, I18nErrorCode::SUCCESS);
    }
}

/**
 * @tc.name: IntlFuncTest0051
 * @tc.desc: Test Intl I18nCalendar
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest, IntlFuncTest0051, TestSize.Level1)
{
    I18nCalendar *calendar = new I18nCalendar("zh-Hans-CN");
    I18nCalendar *calendar2 = new I18nCalendar("zh-Hans-CN", CalendarType::CHINESE);
    calendar->SetTime(1684742124645);
    calendar->Set(1989, 5, 23);
    calendar->SetTimeZone("Asia/Shanghai");
    std::string tzId = calendar->GetTimeZone();
    EXPECT_EQ(tzId, "Asia/Shanghai");
    int32_t minimalDaysInFirstWeek = calendar->GetMinimalDaysInFirstWeek();
    EXPECT_EQ(minimalDaysInFirstWeek, 1);
    int32_t firstDayOfWeek = calendar->GetFirstDayOfWeek();
    EXPECT_EQ(firstDayOfWeek, 1);
    calendar2->Set(2023, 5, 28);
    bool isWeekend = calendar2->IsWeekend();
    EXPECT_TRUE(isWeekend);
    I18nCalendar *calendarFake = new I18nCalendar("123");
    I18nCalendar *calendarInvalid = new I18nCalendar("123", CalendarType::CHINESE);
    delete calendarFake;
    delete calendarInvalid;
    delete calendar;
    delete calendar2;
}

/**
 * @tc.name: IntlFuncTest0052
 * @tc.desc: Test Intl IndexUtil
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest, IntlFuncTest0052, TestSize.Level1)
{
    IndexUtil *indexUtil = new IndexUtil("zh-CN");
    std::vector<std::string> indexList = indexUtil->GetIndexList();
    EXPECT_EQ(indexList.size(), 28);
    indexUtil->AddLocale("en-US");
    std::string indexStr = indexUtil->GetIndex("A");
    EXPECT_EQ(indexStr, "A");
    IndexUtil *indexUtil2 = new IndexUtil("");
    delete indexUtil;
    delete indexUtil2;

    IndexUtil indexUtil3("");
    indexUtil3.AddLocale("en-US");
    indexStr = indexUtil3.GetIndex("A");
    EXPECT_EQ(indexStr, "A");
}

/**
 * @tc.name: IntlFuncTest0053
 * @tc.desc: Test Intl PhoneNumberFormat
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest, IntlFuncTest0053, TestSize.Level1)
{
    map<string, string> options = {
        { "type", "national" }
    };
    PhoneNumberFormat *phoneNumberFormat = new PhoneNumberFormat("zh-CN", options);
    std::string location = phoneNumberFormat->getLocationName("18622350085", "zh-CN");
    EXPECT_EQ(location, "天津市");
    bool flag = phoneNumberFormat->isValidPhoneNumber("+8618622350085");
    EXPECT_TRUE(flag);
    std::string number = "+8618622350085";
    std::string formated = phoneNumberFormat->format(number);
    EXPECT_EQ(formated, "186 2235 0085");
    std::string location2 = phoneNumberFormat->getLocationName("fake-number", "zh-CN");
    EXPECT_EQ(location2, "");
    delete phoneNumberFormat;
}

/**
 * @tc.name: IntlFuncTest0054
 * @tc.desc: Test Intl TabooUtils
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest, IntlFuncTest0054, TestSize.Level1)
{
    TabooUtils *tabooUtils = new TabooUtils();
    std::string res1 = tabooUtils->ReplaceCountryName("CN", "en", "China");
    EXPECT_EQ(res1, "China");
    std::string res2 = tabooUtils->ReplaceLanguageName("zh", "en", "chinese");
    EXPECT_EQ(res2, "chinese");
    std::string res3 = tabooUtils->ReplaceCountryName("TW", "zh-Hans", "中国台湾");
    EXPECT_EQ(res3, "中国台湾");
    delete tabooUtils;
    Taboo* taboo = new Taboo();
    delete taboo;
}

/**
 * @tc.name: IntlFuncTest0055
 * @tc.desc: Test Intl LocaleCompare
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest, IntlFuncTest0055, TestSize.Level1)
{
    int32_t result = LocaleCompare::Compare("zh-CN", "zh-Hans-CN");
    EXPECT_EQ(result, 9);
    I18nBreakIterator *i18nBreakIterator = new I18nBreakIterator("zh-Hans-CN");
    bool isBoundary = i18nBreakIterator->IsBoundary(6);
    EXPECT_TRUE(!isBoundary);
    int32_t current = i18nBreakIterator->Current();
    EXPECT_EQ(current, 0);
    int32_t first = i18nBreakIterator->First();
    EXPECT_EQ(first, 0);
    int32_t last = i18nBreakIterator->Last();
    EXPECT_EQ(last, 0);
    int32_t previous = i18nBreakIterator->Previous();
    EXPECT_EQ(previous, -1);
    int32_t next6 = i18nBreakIterator->Next(6);
    EXPECT_EQ(next6, -1);
    int32_t resultLatn = LocaleCompare::Compare("en-Latn-US", "en-Qaag-US");
    EXPECT_EQ(resultLatn, 9);
    int32_t resultTl = LocaleCompare::Compare("tl-PH", "fil-PH");
    EXPECT_EQ(resultTl, 9);
    int32_t resultFil = LocaleCompare::Compare("fil-PH", "tl-PH");
    EXPECT_EQ(resultFil, 9);
    int32_t resultQaag = LocaleCompare::Compare("en-Qaag-US", "en-Latn-US");
    EXPECT_EQ(resultQaag, 9);
    int32_t resultHashMapZh = LocaleCompare::Compare("zh-MO", "zh-Hant-HK");
    EXPECT_EQ(resultHashMapZh, 8);
    int32_t resultZh = LocaleCompare::Compare("zh-Hant-MO", "zh-Hant-HK");
    EXPECT_EQ(resultZh, 8);
    int32_t resultHashMapEn = LocaleCompare::Compare("en-WS", "en-001");
    EXPECT_EQ(resultHashMapEn, 8);
    int32_t resultHashEn = LocaleCompare::Compare("en-Latn-WS", "en-001");
    EXPECT_EQ(resultHashEn, 8);
    int32_t resultHashQaagEn = LocaleCompare::Compare("en-Qaag-WS", "en-001");
    EXPECT_EQ(resultHashQaagEn, 8);
    I18nBreakIterator *breakIterator = new I18nBreakIterator("2--**");
    breakIterator->Current();
    breakIterator->First();
    breakIterator->Last();
    breakIterator->Previous();
    breakIterator->Next(6);
    breakIterator->Next();
    breakIterator->Following(0);
    breakIterator->IsBoundary(6);
    delete breakIterator;
    delete i18nBreakIterator;
}

/**
 * @tc.name: IntlFuncTest0056
 * @tc.desc: Test Intl SystemLocaleManager
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest, IntlFuncTest0056, TestSize.Level1)
{
    SystemLocaleManager *systemLocaleManager = new SystemLocaleManager();
    std::vector<std::string> languages = {"en", "de", "es", "fr"};
    SortOptions sortOptions = {"en-US", true, true};
    std::vector<LocaleItem> languageInfos = systemLocaleManager->GetLanguageInfoArray(languages, sortOptions);
    EXPECT_EQ(languageInfos.size(), 4);
    const std::vector<std::string> countries = {"US", "GB", "DE", "CA"};
    std::vector<LocaleItem> countryInfos = systemLocaleManager->GetCountryInfoArray(countries, sortOptions);
    EXPECT_EQ(countryInfos.size(), 4);
    std::vector<TimeZoneCityItem> timezoneCityItemList = SystemLocaleManager::GetTimezoneCityInfoArray();
    EXPECT_TRUE(timezoneCityItemList.size() > 0);
    delete systemLocaleManager;
}

/**
 * @tc.name: IntlFuncTest0057
 * @tc.desc: Test Intl Utils
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest, IntlFuncTest0057, TestSize.Level1)
{
    bool isDigit = IsDigit("55");
    EXPECT_TRUE(isDigit);
    bool isSpaceChar = IsSpaceChar(" ");
    EXPECT_TRUE(isSpaceChar);
    bool isWhiteSpace = IsWhiteSpace("  ");
    EXPECT_TRUE(isWhiteSpace);
    bool isRTLCharacter = IsRTLCharacter("^");
    EXPECT_TRUE(!isRTLCharacter);
    isRTLCharacter = IsRTLCharacter("\u0645");
    EXPECT_TRUE(isRTLCharacter);
    bool isIdeoGraphic = IsIdeoGraphic("&&*");
    EXPECT_TRUE(!isIdeoGraphic);
    bool isLetter = IsLetter("cccUt");
    EXPECT_TRUE(isLetter);
    bool isLowerCase = IsLowerCase("abc");
    EXPECT_TRUE(isLowerCase);
    bool isUpperCase = IsUpperCase("AbC");
    EXPECT_TRUE(isUpperCase);
    std::string getType = GetType("$$%");
    EXPECT_EQ(getType, "U_CURRENCY_SYMBOL");
}

/**
 * @tc.name: IntlFuncTest0058
 * @tc.desc: Test Intl MeasureData
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest, IntlFuncTest0058, TestSize.Level1)
{
    std::string timezoneId = "Asia/Shanghai";
    I18nTimeZone *i18nTimeZone = new I18nTimeZone(timezoneId, true);
    int32_t offset = i18nTimeZone->GetOffset(1684742124645);
    EXPECT_EQ(offset, 28800000);
    int32_t rawOffset = i18nTimeZone->GetRawOffset();
    EXPECT_EQ(rawOffset, 28800000);
    std::string tzId = i18nTimeZone->GetID();
    EXPECT_EQ(tzId, "Asia/Shanghai");
    std::string displayName = i18nTimeZone->GetDisplayName();
    EXPECT_EQ(displayName, "中国标准时间");
    std::string displayName2 = i18nTimeZone->GetDisplayName(true);
    EXPECT_EQ(displayName2, "中国标准时间");
    std::string zhCn = "zh-CN";
    std::string displayNameCn = i18nTimeZone->GetDisplayName(zhCn);
    EXPECT_EQ(displayNameCn, "中国标准时间");
    std::string displayName4 = i18nTimeZone->GetDisplayName("zh-CN", true);
    EXPECT_EQ(displayName4, "中国标准时间");
    std::string cityId = "Shanghai";
    std::string localeId = "en-US";
    std::string cityDisplayName = i18nTimeZone->GetCityDisplayName(cityId, localeId);
    EXPECT_EQ(cityDisplayName, "Shanghai (China)");
    std::unique_ptr<I18nTimeZone> timezone = I18nTimeZone::CreateInstance(timezoneId, true);
    I18nErrorCode errorCode = I18nErrorCode::SUCCESS;
    std::set<std::string> set0 = I18nTimeZone::GetAvailableIDs(errorCode);
    EXPECT_EQ(set0.size(), 442);
    std::set<std::string> set1 = I18nTimeZone::GetAvailableZoneCityIDs();
    EXPECT_TRUE(set1.size() > 0);
    std::string empty = "";
    std::string fakeCityId = "fake cityId";
    I18nTimeZone *i18nTimeZoneEmpty = new I18nTimeZone(empty, true);
    I18nTimeZone *i18nTimeZoneFake = new I18nTimeZone(fakeCityId, false);
    I18nTimeZone *i18nTimeZoneCityId = new I18nTimeZone(cityId, false);
    delete i18nTimeZoneEmpty;
    delete i18nTimeZoneFake;
    delete i18nTimeZoneCityId;
    delete i18nTimeZone;
    uint32_t mask = GetMask("CN");
    EXPECT_EQ(mask, 2462);
}

/**
 * @tc.name: IntlFuncTest0059
 * @tc.desc: Test Intl NumberFormat
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest, IntlFuncTest0059, TestSize.Level1)
{
    std::vector<std::string> localeTags = { "$$@@#" };
    std::map<std::string, std::string> configs = {
        {"style", "unit"}, {"unit", "fake unit"}
    };
    NumberFormat formatter(localeTags, configs);
    std::string res = formatter.Format(12);
    EXPECT_EQ(res, "12");
    localeTags = { "zh-Hans-u-nu-latn" };
    NumberFormat* format = new (std::nothrow) NumberFormat(localeTags, configs);
    EXPECT_EQ(res, format->Format(12));
    delete format;
}

/**
 * @tc.name: IntlFuncTest0060
 * @tc.desc: Test Intl HolidayManager
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest, IntlFuncTest0060, TestSize.Level1)
{
    IcsFileWriter icsFileWriter;
    std::string path = icsFileWriter.GenerateFile();
    HolidayManager *holiday_manager = new HolidayManager(path.c_str());
    std::map<std::string, std::vector<HolidayInfoItem>> holidayDataMap;
    std::vector<HolidayInfoItem> infoList;
    std::vector<HolidayLocalName> localNameList1;
    localNameList1.push_back({"tr", "Kurban Bayrami Tatili"});
    std::vector<HolidayLocalName> localNameList2;
    localNameList2.push_back({"tr", "Kurban Bayrami 2. Günü"});
    HolidayInfoItem item1 = {"Sacrifice Feast Holiday", 2022, 6, 25, localNameList1};
    HolidayInfoItem item2 = {"The Second Day of Sacrifice Feast", 2022, 6, 25, localNameList2};
    infoList.push_back(item1);
    infoList.push_back(item2);
    holidayDataMap.insert({"20220625", infoList});
    holiday_manager->SetHolidayData(holidayDataMap);
    std::vector<HolidayInfoItem> list = holiday_manager->GetHolidayInfoItemArray(2022);
    EXPECT_EQ(2, list.size());
    list = holiday_manager->GetHolidayInfoItemArray();
    EXPECT_EQ(0, list.size());
    bool flag = holiday_manager->IsHoliday(2022, 6, 25);
    EXPECT_TRUE(flag);
    flag = holiday_manager->IsHoliday();
    EXPECT_TRUE(!flag);
    delete holiday_manager;
}

/**
 * @tc.name: IntlFuncTest0061
 * @tc.desc: Test Intl NumberFormat.format
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest, IntlFuncTest061, TestSize.Level1)
{
    string locale = "en-CN";
    string expects = "123K";
    vector<string> locales;
    locales.push_back(locale);
    string style = "decimal";
    map<string, string> options = { { "style", style },
                                    { "notation", "compact" } };
    NumberFormat *numFmt = new (std::nothrow) NumberFormat(locales, options);
    if (!numFmt) {
        EXPECT_TRUE(false);
        return;
    }
    string out = numFmt->Format(123456.789);
    EXPECT_EQ(out, expects);
    EXPECT_EQ(numFmt->GetStyle(), style);
    delete numFmt;
}

/**
 * @tc.name: IntlFuncTest0062
 * @tc.desc: Test Intl utils.cpp
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest, IntlFuncTest062, TestSize.Level1)
{
    string emptyStr = "";
    string sep = ";";
    vector<string> dest;
    std::unordered_set<std::string> allValidLocalesLanguageTag;
    Split(emptyStr, sep, dest);
    int32_t status = 0;
    string numberStr = "12345678901234567890123456789012345678901234567890987654321";
    ConvertString2Int(numberStr, status);
    icu::Locale locale("$$$$5%%%");
    bool isValid = IsValidLocaleTag(locale);
    EXPECT_TRUE(!isValid);
    GetAllValidLocalesTag(allValidLocalesLanguageTag);
    EXPECT_TRUE(allValidLocalesLanguageTag.size() == 0);

    std::string localeRule = "zh-Hans";
    DateTimeRule* dateTimeRule = new DateTimeRule(localeRule);
    DateTimeFilter* dateTimeFilter = new DateTimeFilter(localeRule, dateTimeRule);
    delete dateTimeFilter;
    delete dateTimeRule;
}

/**
 * @tc.name: IntlFuncTest0063
 * @tc.desc: Test Intl RegexRule
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest, IntlFuncTest063, TestSize.Level1)
{
    using namespace i18n::phonenumbers;
    string regexStr = "[a-z]1?";
    icu::UnicodeString regex(regexStr.c_str());
    std::string isValidType = "PrexxxxSuf";
    std::string handleType = "EndWithMobile";
    std::string insensitive = "False";
    std::string type = "xxx";
    RegexRule* regexRule = new RegexRule(regex, isValidType, handleType, insensitive, type);
    handleType = "fake";
    isValidType = "Code";
    RegexRule* regexRule2 = new RegexRule(regex, isValidType, handleType, insensitive, type);
    icu::UnicodeString message(type.c_str());

    i18n::phonenumbers::PhoneNumber phoneNumber;
    PhoneNumberUtil* phoneNumberUtil = i18n::phonenumbers::PhoneNumberUtil::GetInstance();
    std::string input = "[010111111111111;ext=0782";
    std::string country = "CN";
    size_t size = input.length();
    phoneNumberUtil->ParseAndKeepRawInput(input, country, &phoneNumber);
    PhoneNumberMatch* possibleNumber = new PhoneNumberMatch(size, input, phoneNumber);
    PhoneNumberMatch* phoneNumberMatch = regexRule2->IsValid(possibleNumber, message);
    EXPECT_TRUE(phoneNumberMatch != nullptr);
    delete phoneNumberMatch;
    delete regexRule;
    delete regexRule2;
}
} // namespace I18n
} // namespace Global
} // namespace OHOS