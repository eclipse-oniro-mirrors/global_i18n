/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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
#include "entity_recognizer.h"
#include "i18n_break_iterator.h"
#include "i18n_calendar.h"
#include "i18n_normalizer.h"
#include "i18n_timezone_mock.h"
#include "locale_config.h"
#include "locale_info.h"
#include "locale_matcher.h"
#include "locale_util.h"
#include "lunar_calendar.h"
#include "measure_data.h"
#include "phone_number_format_mock.h"
#include "preferred_language.h"
#include "simple_date_time_format.h"
#include "simple_number_format.h"
#include "utils.h"

using namespace OHOS::Global::I18n;
using testing::ext::TestSize;
using namespace std;

namespace OHOS {
namespace Global {
namespace I18n {
class I18nTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();

    static std::string originalLanguage;
    static std::string originalRegion;
    static std::string originalLocale;
};

std::string I18nTest::originalLanguage;
std::string I18nTest::originalRegion;
std::string I18nTest::originalLocale;

void I18nTest::SetUpTestCase(void)
{
    LocaleConfig::SetSystemLanguage("zh-Hans");
    LocaleConfig::SetSystemRegion("CN");
    LocaleConfig::SetSystemLocale("zh-Hans-CN");
}

void I18nTest::TearDownTestCase(void)
{
    LocaleConfig::SetSystemLanguage(originalLanguage);
    LocaleConfig::SetSystemRegion(originalRegion);
    LocaleConfig::SetSystemLocale(originalLocale);
}

void I18nTest::SetUp(void)
{
    originalLanguage = LocaleConfig::GetSystemLanguage();
    originalRegion = LocaleConfig::GetSystemRegion();
    originalLocale = LocaleConfig::GetSystemLocale();
}

void I18nTest::TearDown(void)
{}

/**
 * @tc.name: I18nFuncTest001
 * @tc.desc: Test I18n PreferredLanguage GetPreferredLocale
 * @tc.type: FUNC
 */
HWTEST_F(I18nTest, I18nFuncTest001, TestSize.Level1)
{
    string preferredLocale = PreferredLanguage::GetPreferredLocale();
    EXPECT_TRUE(preferredLocale.size() > 0);
    string systemLocale = "zh-Hans-CN";
    I18nErrorCode status = LocaleConfig::SetSystemLocale(systemLocale);
    EXPECT_EQ(status, I18nErrorCode::SUCCESS);
    status = PreferredLanguage::AddPreferredLanguage("en-US", 0);
    EXPECT_EQ(status, I18nErrorCode::SUCCESS);
    preferredLocale = PreferredLanguage::GetPreferredLocale();
    EXPECT_EQ(preferredLocale, "en-CN");
    PreferredLanguage::RemovePreferredLanguage(0);
#ifdef SUPPORT_APP_PREFERRED_LANGUAGE
    I18nErrorCode errCode = I18nErrorCode::SUCCESS;
    PreferredLanguage::SetAppPreferredLanguage("en-US", errCode);
    EXPECT_EQ(errCode, I18nErrorCode::SUCCESS);
    std::string appPreferLanguage = PreferredLanguage::GetAppPreferredLanguage();
    EXPECT_EQ(appPreferLanguage, "en-US");
#endif
}

/**
 * @tc.name: I18nFuncTest002
 * @tc.desc: Test I18n Normalizer
 * @tc.type: FUNC
 */
HWTEST_F(I18nTest, I18nFuncTest002, TestSize.Level1)
{
    I18nErrorCode errorCode = I18nErrorCode::SUCCESS;
    I18nNormalizer normalizer(I18nNormalizerMode::NFD, errorCode);
    EXPECT_EQ(errorCode, I18nErrorCode::SUCCESS);

    string text = "\uFB01"; // \uFB01 is ﬁ
    string normalizedText = normalizer.Normalize(text.c_str(), text.length(), errorCode);
    EXPECT_EQ(errorCode, I18nErrorCode::SUCCESS);
    EXPECT_EQ(normalizedText.length(), 3); // 3 is the NFD normalized length of ﬁ.
    EXPECT_EQ(normalizedText, "\uFB01");

    text = "\u0032\u2075";
    normalizedText = normalizer.Normalize(text.c_str(), text.length(), errorCode);
    EXPECT_EQ(errorCode, I18nErrorCode::SUCCESS);
    EXPECT_EQ(normalizedText.length(), 4); // 4 is the expected normalized text length.
    EXPECT_EQ(normalizedText, "\u0032\u2075");

    text = "\u1E9B\u0323";
    normalizedText = normalizer.Normalize(text.c_str(), text.length(), errorCode);
    EXPECT_EQ(errorCode, I18nErrorCode::SUCCESS);
    EXPECT_EQ(normalizedText.length(), 6); // 6 is the expected normalized text length.
    EXPECT_EQ(normalizedText, "\u017F\u0323\u0307");
    std::string result = normalizer.Normalize(nullptr, 0, errorCode);
    EXPECT_EQ(result, "");
}

/**
 * @tc.name: I18nFuncTest003
 * @tc.desc: Test I18n Normalizer
 * @tc.type: FUNC
 */
HWTEST_F(I18nTest, I18nFuncTest003, TestSize.Level1)
{
    I18nErrorCode errorCode = I18nErrorCode::SUCCESS;
    I18nNormalizer normalizer(I18nNormalizerMode::NFC, errorCode);
    EXPECT_EQ(errorCode, I18nErrorCode::SUCCESS);

    string text = "\uFB01"; // \uFB01 is ﬁ
    string normalizedText = normalizer.Normalize(text.c_str(), text.length(), errorCode);
    EXPECT_EQ(errorCode, I18nErrorCode::SUCCESS);
    EXPECT_EQ(normalizedText.length(), 3); // 3 is the NFC normalized length of ﬁ.
    EXPECT_EQ(normalizedText, "\uFB01");

    text = "\u0032\u2075";
    normalizedText = normalizer.Normalize(text.c_str(), text.length(), errorCode);
    EXPECT_EQ(errorCode, I18nErrorCode::SUCCESS);
    EXPECT_EQ(normalizedText.length(), 4); // 4 is the expected normalized text length.
    EXPECT_EQ(normalizedText, "\u0032\u2075");

    text = "\u1E9B\u0323";
    normalizedText = normalizer.Normalize(text.c_str(), text.length(), errorCode);
    EXPECT_EQ(errorCode, I18nErrorCode::SUCCESS);
    EXPECT_EQ(normalizedText.length(), 5); // 5 is the expected normalized text length.
    EXPECT_EQ(normalizedText, "\u1E9B\u0323");
}

/**
 * @tc.name: I18nFuncTest004
 * @tc.desc: Test I18n Normalizer
 * @tc.type: FUNC
 */
HWTEST_F(I18nTest, I18nFuncTest004, TestSize.Level1)
{
    I18nErrorCode errorCode = I18nErrorCode::SUCCESS;
    I18nNormalizer normalizer(I18nNormalizerMode::NFKD, errorCode);
    EXPECT_EQ(errorCode, I18nErrorCode::SUCCESS);

    string text = "\uFB01"; // \uFB01 is ﬁ
    string normalizedText = normalizer.Normalize(text.c_str(), text.length(), errorCode);
    EXPECT_EQ(errorCode, I18nErrorCode::SUCCESS);
    EXPECT_EQ(normalizedText.length(), 2); // 2 is the NFKD normalized length of ﬁ.
    EXPECT_EQ(normalizedText, "\u0066\u0069");

    text = "\u0032\u2075";
    normalizedText = normalizer.Normalize(text.c_str(), text.length(), errorCode);
    EXPECT_EQ(errorCode, I18nErrorCode::SUCCESS);
    EXPECT_EQ(normalizedText.length(), 2); // 2 is the expected normalized text length.
    EXPECT_EQ(normalizedText, "\u0032\u0035");

    text = "\u1E9B\u0323";
    normalizedText = normalizer.Normalize(text.c_str(), text.length(), errorCode);
    EXPECT_EQ(errorCode, I18nErrorCode::SUCCESS);
    EXPECT_EQ(normalizedText.length(), 5); // 5 is the expected normalized text length.
    EXPECT_EQ(normalizedText, "\u0073\u0323\u0307");
}

/**
 * @tc.name: I18nFuncTest005
 * @tc.desc: Test I18n Normalizer
 * @tc.type: FUNC
 */
HWTEST_F(I18nTest, I18nFuncTest005, TestSize.Level1)
{
    I18nErrorCode errorCode = I18nErrorCode::SUCCESS;
    I18nNormalizer normalizer(I18nNormalizerMode::NFKC, errorCode);
    EXPECT_EQ(errorCode, I18nErrorCode::SUCCESS);

    string text = "\uFB01"; // \uFB01 is ﬁ
    string normalizedText = normalizer.Normalize(text.c_str(), text.length(), errorCode);
    EXPECT_EQ(errorCode, I18nErrorCode::SUCCESS);
    EXPECT_EQ(normalizedText.length(), 2); // 2 is the NFKC normalized length of ﬁ.
    EXPECT_EQ(normalizedText, "\u0066\u0069");

    text = "\u0032\u2075";
    normalizedText = normalizer.Normalize(text.c_str(), text.length(), errorCode);
    EXPECT_EQ(errorCode, I18nErrorCode::SUCCESS);
    EXPECT_EQ(normalizedText.length(), 2); // 2 is the expected normalized text length.
    EXPECT_EQ(normalizedText, "\u0032\u0035");

    text = "\u1E9B\u0323";
    normalizedText = normalizer.Normalize(text.c_str(), text.length(), errorCode);
    EXPECT_EQ(errorCode, I18nErrorCode::SUCCESS);
    EXPECT_EQ(normalizedText.length(), 3); // 3 is the expected normalized text length.
    EXPECT_EQ(normalizedText, "\u1E69");
}

/**
 * @tc.name: I18nFuncTest006
 * @tc.desc: Test I18n I18nCalendar
 * @tc.type: FUNC
 */
HWTEST_F(I18nTest, I18nFuncTest006, TestSize.Level1)
{
    I18nCalendar buddhistCalendar("zh-Hans", CalendarType::BUDDHIST);
    buddhistCalendar.Set(UCalendarDateFields::UCAL_YEAR, 2023);
    int32_t year = buddhistCalendar.Get(UCalendarDateFields::UCAL_YEAR);
    EXPECT_EQ(year, 2023);
    I18nCalendar chineseCalendar("zh-Hans", CalendarType::CHINESE);
    chineseCalendar.SetMinimalDaysInFirstWeek(3);
    int32_t minimalDaysInFirstWeek = chineseCalendar.GetMinimalDaysInFirstWeek();
    EXPECT_EQ(minimalDaysInFirstWeek, 3);
    I18nCalendar copticCalendar("zh-Hans", CalendarType::COPTIC);
    copticCalendar.SetFirstDayOfWeek(2);
    int32_t firstDayOfWeek = copticCalendar.GetFirstDayOfWeek();
    EXPECT_EQ(firstDayOfWeek, 2);
    I18nCalendar ethiopicCalendar("zh-Hans", CalendarType::ETHIOPIC);
    int64_t date = 1687244448234;
    UErrorCode status = U_ZERO_ERROR;
    bool isWeekend = ethiopicCalendar.IsWeekend(date, status);
    EXPECT_EQ(isWeekend, false);
    I18nCalendar hebrewCalendar("zh-Hans", CalendarType::HEBREW);
    std::string displayLocaleTag = "en-US";
    std::string displayName = hebrewCalendar.GetDisplayName(displayLocaleTag);
    EXPECT_EQ(displayName, "Hebrew Calendar");
    I18nCalendar gregoryCalendar("zh-Hans", CalendarType::GREGORY);
    gregoryCalendar.Set(UCalendarDateFields::UCAL_MONTH, 2);
    int32_t month = gregoryCalendar.Get(UCalendarDateFields::UCAL_MONTH);
    EXPECT_EQ(month, 2);
    I18nCalendar indianCalendar("zh-Hans", CalendarType::INDIAN);
    indianCalendar.Set(UCalendarDateFields::UCAL_WEEK_OF_YEAR, 10);
    int32_t weekOfYear = indianCalendar.Get(UCalendarDateFields::UCAL_WEEK_OF_YEAR);
    EXPECT_EQ(weekOfYear, 10);
    I18nCalendar islamicCivilCalendar("zh-Hans", CalendarType::ISLAMIC_CIVIL);
    islamicCivilCalendar.Set(UCalendarDateFields::UCAL_WEEK_OF_MONTH, 2);
    int32_t weekOfMonth = islamicCivilCalendar.Get(UCalendarDateFields::UCAL_WEEK_OF_MONTH);
    EXPECT_EQ(weekOfMonth, 2);
    I18nCalendar islamicTblaCalendar("zh-Hans", CalendarType::ISLAMIC_TBLA);
    islamicTblaCalendar.Set(UCalendarDateFields::UCAL_DATE, 3);
    int32_t dateValue = islamicTblaCalendar.Get(UCalendarDateFields::UCAL_DATE);
    EXPECT_EQ(dateValue, 3);
    I18nCalendar islamicUmalquraCalendar("zh-Hans", CalendarType::ISLAMIC_UMALQURA);
    islamicUmalquraCalendar.Set(UCalendarDateFields::UCAL_DAY_OF_YEAR, 123);
    int32_t dayOfYear = islamicUmalquraCalendar.Get(UCalendarDateFields::UCAL_DAY_OF_YEAR);
    EXPECT_EQ(dayOfYear, 123);
    I18nCalendar japaneseCalendar("zh-Hans", CalendarType::JAPANESE);
    japaneseCalendar.Set(UCalendarDateFields::UCAL_DAY_OF_WEEK, 3);
    int32_t dayOfWeek = japaneseCalendar.Get(UCalendarDateFields::UCAL_DAY_OF_WEEK);
    EXPECT_EQ(dayOfWeek, 3);
}

/**
 * @tc.name: I18nFuncTest007
 * @tc.desc: Test I18n I18nCalendar
 * @tc.type: FUNC
 */
HWTEST_F(I18nTest, I18nFuncTest007, TestSize.Level1)
{
    I18nCalendar persianTblaCalendar("zh-Hans", CalendarType::PERSIAN);
    persianTblaCalendar.Set(UCalendarDateFields::UCAL_DAY_OF_WEEK_IN_MONTH, 1);
    int32_t dayOfWeekInMonth = persianTblaCalendar.Get(UCalendarDateFields::UCAL_DAY_OF_WEEK_IN_MONTH);
    EXPECT_EQ(dayOfWeekInMonth, 1);
    I18nCalendar defaultCalendar("zh-Hans", CalendarType::UNDEFINED);
    defaultCalendar.Set(UCalendarDateFields::UCAL_HOUR, 12);
    int32_t hour = defaultCalendar.Get(UCalendarDateFields::UCAL_HOUR);
    EXPECT_EQ(hour, 0);
    defaultCalendar.Set(UCalendarDateFields::UCAL_MILLISECOND, 1000);
    defaultCalendar.Add(UCalendarDateFields::UCAL_MILLISECOND, 165);
    EXPECT_EQ(defaultCalendar.Get(UCalendarDateFields::UCAL_MILLISECOND), 165);
    defaultCalendar.Set(UCalendarDateFields::UCAL_SECOND, 21);
    defaultCalendar.Add(UCalendarDateFields::UCAL_SECOND, 51);
    EXPECT_EQ(defaultCalendar.Get(UCalendarDateFields::UCAL_SECOND), 12);
    defaultCalendar.Set(UCalendarDateFields::UCAL_MINUTE, 31);
    defaultCalendar.Add(UCalendarDateFields::UCAL_MINUTE, 31);
    EXPECT_EQ(defaultCalendar.Get(UCalendarDateFields::UCAL_MINUTE), 2);
    defaultCalendar.Set(UCalendarDateFields::UCAL_HOUR, 10);
    defaultCalendar.Add(UCalendarDateFields::UCAL_HOUR, 11);
    EXPECT_EQ(defaultCalendar.Get(UCalendarDateFields::UCAL_HOUR), 9);
    defaultCalendar.Set(UCalendarDateFields::UCAL_DATE, 20);
    defaultCalendar.Add(UCalendarDateFields::UCAL_DATE, 5);
    EXPECT_EQ(defaultCalendar.Get(UCalendarDateFields::UCAL_DATE), 25);
    defaultCalendar.Set(UCalendarDateFields::UCAL_MONTH, 10);
    defaultCalendar.Add(UCalendarDateFields::UCAL_MONTH, 2);
    EXPECT_EQ(defaultCalendar.Get(UCalendarDateFields::UCAL_MONTH), 0);
    defaultCalendar.Set(UCalendarDateFields::UCAL_YEAR, 100);
    defaultCalendar.Add(UCalendarDateFields::UCAL_YEAR, 100);
    EXPECT_EQ(defaultCalendar.Get(UCalendarDateFields::UCAL_YEAR), 200);
    defaultCalendar.Set(2023, 8, 2);
    UDate millis = defaultCalendar.GetTimeInMillis();
    I18nCalendar checkCalendar("zh-Hans", CalendarType::UNDEFINED);
    checkCalendar.SetTime(millis);
    EXPECT_EQ(checkCalendar.Get(UCalendarDateFields::UCAL_YEAR), 2023);
    EXPECT_EQ(checkCalendar.Get(UCalendarDateFields::UCAL_MONTH), 8);
    EXPECT_EQ(checkCalendar.Get(UCalendarDateFields::UCAL_DATE), 2);
    defaultCalendar.SetTime(1684742124645);
    int32_t compareDays = defaultCalendar.CompareDays(1684742124650);
    EXPECT_EQ(compareDays, 1);
}

/**
 * @tc.name: I18nFuncTest008
 * @tc.desc: Test I18n I18nBreakIterator
 * @tc.type: FUNC
 */
HWTEST_F(I18nTest, I18nFuncTest008, TestSize.Level1)
{
    std::string fakeLocaleTag = "FakeLocaleTag";
    I18nBreakIterator brkIterator(fakeLocaleTag);
    int32_t res = brkIterator.Current();
    EXPECT_EQ(res, 0);
    res = brkIterator.First();
    EXPECT_EQ(res, 0);
    res = brkIterator.Last();
    EXPECT_EQ(res, 0);
    res = brkIterator.Previous();
    EXPECT_EQ(res, -1);
    int32_t offset = 1;
    res = brkIterator.Next(offset);
    EXPECT_EQ(res, -1);
    res = brkIterator.Next();
    EXPECT_EQ(res, -1);
    res = brkIterator.Following(offset);
    EXPECT_EQ(res, -1);
    bool status = brkIterator.IsBoundary(offset);
    EXPECT_FALSE(status);

    std::string localeTag = "en-Latn-US";
    I18nBreakIterator enBrkIterator(localeTag);
    std::string text = "Test I18nBreakIterator";
    enBrkIterator.SetText(text.c_str());
    std::string resText;
    enBrkIterator.GetText(resText);
    EXPECT_EQ(resText, text);
    res = enBrkIterator.Next();
    EXPECT_EQ(res, 5);
}

/**
 * @tc.name: I18nFuncTest009
 * @tc.desc: Test I18n MeasureData GetDefaultPreferredUnit
 * @tc.type: FUNC
 */
HWTEST_F(I18nTest, I18nFuncTest009, TestSize.Level1)
{
    std::string regionTags[] = {"GB", "US", "CN"};
    std::string usage = "length";
    std::vector<std::string> units;
    for (size_t i = 0; i < sizeof(regionTags) / sizeof(std::string); ++i) {
        GetDefaultPreferredUnit(regionTags[i], usage, units);
        EXPECT_EQ(units.size(), 3);
        units.clear();
    }
}

/**
 * @tc.name: I18nFuncTest010
 * @tc.desc: Test I18n MeasureData GetFallbackPreferredUnit
 * @tc.type: FUNC
 */
HWTEST_F(I18nTest, I18nFuncTest010, TestSize.Level1)
{
    std::string regionTags1[] = {"MX", "NL", "NO", "PL", "RU"};
    std::string usage = "length-person-informal";
    std::vector<std::string> units;
    for (size_t i = 0; i < sizeof(regionTags1) / sizeof(std::string); ++i) {
        GetFallbackPreferredUnit(regionTags1[i], usage, units);
        EXPECT_EQ(units.size(), 2);
        units.clear();
    }

    std::string regionTag2 = "SE";
    usage = "length-person";
    GetFallbackPreferredUnit(regionTag2, usage, units);
    EXPECT_EQ(units.size(), 2);
    units.clear();

    std::string regionTags3[] = {"US", "CN"};
    for (size_t i = 0; i < sizeof(regionTags3) / sizeof(std::string); ++i) {
        GetFallbackPreferredUnit(regionTags3[i], usage, units);
        EXPECT_EQ(units.size(), 1);
        units.clear();
    }
}

/**
 * @tc.name: I18nFuncTest011
 * @tc.desc: Test I18n MeasureData GetRestPreferredUnit
 * @tc.type: FUNC
 */
HWTEST_F(I18nTest, I18nFuncTest011, TestSize.Level1)
{
    std::string regionTags1[] = {"CA", "IN"};
    std::string usage = "length-person";
    std::vector<std::string> units;
    for (size_t i = 0; i < sizeof(regionTags1) / sizeof(std::string); ++i) {
        GetRestPreferredUnit(regionTags1[i], usage, units);
        EXPECT_EQ(units.size(), 1);
        units.clear();
    }

    std::string regionTags2[] = {"CN", "DK", "PT", "DE", "GB"};
    usage = "length-person-informal";
    for (size_t i = 0; i < sizeof(regionTags2) / sizeof(std::string); ++i) {
        GetRestPreferredUnit(regionTags2[i], usage, units);
        EXPECT_EQ(units.size(), 2);
        units.clear();
    }

    std::string regionTag3 = "KR";
    usage = "speed-wind";
    GetRestPreferredUnit(regionTag3, usage, units);
    EXPECT_EQ(units.size(), 1);
    units.clear();

    std::string regionTag4 = "XX";
    usage = "fake usage";
    GetRestPreferredUnit(regionTag4, usage, units);
    EXPECT_EQ(units.size(), 0);
}

/**
 * @tc.name: I18nFuncTest012
 * @tc.desc: Test I18n MeasureData GetPreferredUnit
 * @tc.type: FUNC
 */
HWTEST_F(I18nTest, I18nFuncTest012, TestSize.Level1)
{
    std::string regionTags1[] = {
        "AT", "BE", "DZ", "EG", "ES", "FR", "HK", "ID", "IL", "IT", "JO", "MY", "SA", "TR", "VN"
    };
    std::string usage = "length-person";
    std::vector<std::string> units;
    for (size_t i = 0; i < sizeof(regionTags1) / sizeof(std::string); ++i) {
        GetPreferredUnit(regionTags1[i], usage, units);
        EXPECT_EQ(units.size(), 2);
        units.clear();
    }

    std::string regionTag2 = "BR";
    usage = "length-person-informal";
    GetPreferredUnit(regionTag2, usage, units);
    EXPECT_EQ(units.size(), 2);
    units.clear();

    std::string regionTags3[] = {"BS", "BZ", "PR", "PW"};
    usage = "temperature-weather";
    for (size_t i = 0; i < sizeof(regionTags3) / sizeof(std::string); i++) {
        GetPreferredUnit(regionTags3[i], usage, units);
        EXPECT_EQ(units.size(), 1);
        units.clear();
    }

    std::string regionTag4 = "XX";
    usage = "fake usage";
    GetPreferredUnit(regionTag4, usage, units);
    EXPECT_EQ(units.size(), 0);
}

/**
 * @tc.name: I18nFuncTest013
 * @tc.desc: Test I18n MeasureData
 * @tc.type: FUNC
 */
HWTEST_F(I18nTest, I18nFuncTest013, TestSize.Level1)
{
    double value = 10.0;
    std::string fromUnit = "acre";
    std::string fromMeasSys = "US";
    std::string toUnit = "foot";
    std::string toMeasSys = "US";
    int status = Convert(value, fromUnit, fromMeasSys, toUnit, toMeasSys);
    EXPECT_EQ(status, 0);

    std::string fakeUnit = "fake unit";
    status = Convert(value, fakeUnit, fromMeasSys, toUnit, toMeasSys);
    EXPECT_EQ(status, 0);
    status = Convert(value, fromUnit, fromMeasSys, fakeUnit, toMeasSys);
    EXPECT_EQ(status, 0);

    toUnit = "hectare";
    status = Convert(value, fromUnit, fromMeasSys, toUnit, toMeasSys);
    EXPECT_EQ(status, 1);
    EXPECT_TRUE(Eq(value, 4.0468564));

    std::string unit = "tablespoon";
    std::string measSys = "UK";
    std::vector<double> factors = {0.0, 0.0};
    ComputeFactorValue(unit, measSys, factors);
    EXPECT_EQ(factors.size(), 2);
    EXPECT_TRUE(Eq(factors[0], 1.77582e-05));
    unit = "acre";
    factors = {0.0, 0.0};
    ComputeFactorValue(unit, measSys, factors);
    EXPECT_EQ(factors.size(), 2);
    EXPECT_TRUE(Eq(factors[0], 4046.856422));

    double res = ComputeSIPrefixValue(unit);
    EXPECT_TRUE(Eq(res, 0));
    unit = "deci";
    res = ComputeSIPrefixValue(unit);
    EXPECT_TRUE(Eq(res, 0.1));

    unit = "square-acre";
    factors = {0.0, 0.0};
    ComputePowerValue(unit, measSys, factors);
    EXPECT_EQ(factors.size(), 2);

    unit = "fake-per-hour";
    factors = {0.0, 0.0};
    status = ComputeValue(unit, measSys, factors);
    EXPECT_EQ(status, 0);
    unit = "kilometer-per-fake";
    factors = {0.0, 0.0};
    status = ComputeValue(unit, measSys, factors);
    EXPECT_EQ(status, 0);
}

/**
 * @tc.name: I18nFuncTest014
 * @tc.desc: Test I18n PhoneNumberFormat
 * @tc.type: FUNC
 */
HWTEST_F(I18nTest, I18nFuncTest014, TestSize.Level1)
{
    std::string countryTag = "";
    std::map<std::string, std::string> options = {
        {"type", "INTERNATIONAL"}
    };
    std::unique_ptr<PhoneNumberFormat> formatter = PhoneNumberFormat::CreateInstance(countryTag, options);
    std::string fakePhoneNumber = "93827393872723482";
    bool isValid = formatter->isValidPhoneNumber(fakePhoneNumber);
    EXPECT_FALSE(isValid);
    std::string formattedPhoneNumber = formatter->format(fakePhoneNumber);
    EXPECT_EQ(formattedPhoneNumber, "");
    std::unique_ptr<PhoneNumberFormat> formatter1 = std::make_unique<PhoneNumberFormatMock>("XK", options);
    std::string blocklRegion = formatter1->getLocationName("0038312345678", "zh-CN");
    EXPECT_EQ(blocklRegion, "");
    std::unique_ptr<PhoneNumberFormat> formatter2 = std::make_unique<PhoneNumberFormatMock>("CN", options);
    std::string blocklCity = formatter2->getLocationName("13731630016", "zh-CN");
    EXPECT_EQ(blocklCity, "");
    std::string replaceCity = formatter2->getLocationName("13731930016", "zh-CN");
    EXPECT_EQ(replaceCity, "安徽省宣城市2");
    std::string number192 = "19200707087";
    std::string formattedNumber = formatter2->format(number192);
    EXPECT_EQ(formattedNumber, "+86 192 0070 7087");
}

/**
 * @tc.name: I18nFuncTest015
 * @tc.desc: Test I18n I18nTimeZone
 * @tc.type: FUNC
 */
HWTEST_F(I18nTest, I18nFuncTest015, TestSize.Level1)
{
    std::string id = "Asia/Shanghai";
    bool isZoneID = true;
    I18nTimeZone timezone1(id, isZoneID);
    std::string timezoneId = timezone1.GetID();
    EXPECT_EQ(timezoneId, "Asia/Shanghai");

    id = "Shanghai";
    isZoneID = false;
    I18nTimeZone timezone2(id, isZoneID);
    timezoneId = timezone2.GetID();
    EXPECT_EQ(timezoneId, "Asia/Shanghai");

    id = "Auckland";
    I18nTimeZone timezone3(id, isZoneID);
    timezoneId = timezone3.GetID();
    EXPECT_EQ(timezoneId, "Pacific/Auckland");
    std::string localeTag = "en-Latn-US";
    std::string displayName = timezone3.GetDisplayName(localeTag);
    EXPECT_EQ(timezoneId, "Pacific/Auckland");

    std::string fakeId = "fake city id";
    std::string cityDisplayName = I18nTimeZone::GetCityDisplayName(fakeId, localeTag);
    EXPECT_EQ(cityDisplayName, "");

    cityDisplayName = I18nTimeZone::GetCityDisplayName(id, localeTag);
    EXPECT_EQ(cityDisplayName, "Auckland (New Zealand)");

    std::string fakeLocale = "fake locale tag";
    cityDisplayName = I18nTimeZone::GetCityDisplayName(id, fakeLocale);
    EXPECT_EQ(cityDisplayName, "");

    id = "Pacific/Enderbury";
    std::unique_ptr<I18nTimeZone> timezone4 = std::make_unique<I18nTimeZoneMock>(id, true);
    std::string cityName = timezone4->GetDisplayName("zh-CN", false);
    EXPECT_EQ(cityName, "菲尼克斯群岛标准时间");
}

/**
 * @tc.name: I18nFuncTest016
 * @tc.desc: Test I18n EntityRecognizer
 * @tc.type: FUNC
 */
HWTEST_F(I18nTest, I18nFuncTest016, TestSize.Level1)
{
    std::string localeStr = "zh-CN";
    UErrorCode status = U_ZERO_ERROR;
    icu::Locale locale = icu::Locale::forLanguageTag(localeStr, status);
    std::string message = "您好，您的包裹已送至指定地点，请尽快签收：快递员：刘某某，手机：15912345678，QQ：123456789。";
    std::unique_ptr<EntityRecognizer> m = std::make_unique<EntityRecognizer>(locale);
    std::vector<std::vector<int>> res = m->FindEntityInfo(message);
    EXPECT_EQ(res[0][0], 1);
    EXPECT_EQ(res[0][1], 32);
    EXPECT_EQ(res[0][2], 43);
}

/**
 * @tc.name: I18nFuncTest017
 * @tc.desc: Test I18n EntityRecognizer
 * @tc.type: FUNC
 */
HWTEST_F(I18nTest, I18nFuncTest017, TestSize.Level1)
{
    std::string localeStr = "zh-CN";
    UErrorCode status = U_ZERO_ERROR;
    icu::Locale locale = icu::Locale::forLanguageTag(localeStr, status);
    std::string message = "您好，您的包裹已送至指定地点，请尽快签收：快递员：刘某某，手机：15912345678 15512345678。";
    std::unique_ptr<EntityRecognizer> m = std::make_unique<EntityRecognizer>(locale);
    std::vector<std::vector<int>> res = m->FindEntityInfo(message);
    EXPECT_EQ(res[0][0], 2);
    EXPECT_EQ(res[0][1], 32);
    EXPECT_EQ(res[0][2], 43);
    EXPECT_EQ(res[0][3], 44);
    EXPECT_EQ(res[0][4], 55);
}

/**
 * @tc.name: I18nFuncTest018
 * @tc.desc: Test I18n EntityRecognizer
 * @tc.type: FUNC
 */
HWTEST_F(I18nTest, I18nFuncTest018, TestSize.Level1)
{
    std::string localeStr = "en-GB";
    UErrorCode status = U_ZERO_ERROR;
    icu::Locale locale = icu::Locale::forLanguageTag(localeStr, status);
    std::string message = "The stunning xxxxxx xxx in Verbier is availble for Christmas - call +44 (0)20 1234 5678.";
    std::unique_ptr<EntityRecognizer> m = std::make_unique<EntityRecognizer>(locale);
    std::vector<std::vector<int>> res = m->FindEntityInfo(message);
    EXPECT_EQ(res[0][0], 1);
    EXPECT_EQ(res[0][1], 68);
    EXPECT_EQ(res[0][2], 87);
}

/**
 * @tc.name: I18nFuncTest019
 * @tc.desc: Test I18n EntityRecognizer
 * @tc.type: FUNC
 */
HWTEST_F(I18nTest, I18nFuncTest019, TestSize.Level1)
{
    std::string localeStr = "en-GB";
    UErrorCode status = U_ZERO_ERROR;
    icu::Locale locale = icu::Locale::forLanguageTag(localeStr, status);
    std::string message = "RT @missingpeople: RT 32 yo missing since 26/09/2013 from Newry, NI. Seen him? Call 116000.";
    std::unique_ptr<EntityRecognizer> m = std::make_unique<EntityRecognizer>(locale);
    std::vector<std::vector<int>> res = m->FindEntityInfo(message);
    EXPECT_EQ(res[0][0], 1);
    EXPECT_EQ(res[0][1], 84);
    EXPECT_EQ(res[0][2], 90);
}

/**
 * @tc.name: I18nFuncTest020
 * @tc.desc: Test I18n EntityRecognizer
 * @tc.type: FUNC
 */
HWTEST_F(I18nTest, I18nFuncTest020, TestSize.Level1)
{
    std::string localeStr = "zh-CN";
    UErrorCode status = U_ZERO_ERROR;
    icu::Locale locale = icu::Locale::forLanguageTag(localeStr, status);
    std::string message = "xxxx海滩 xxxxx xxxxxx街上的饰品店，皮革花朵戒指10000印尼盾，不到10块人民币。";
    std::unique_ptr<EntityRecognizer> m = std::make_unique<EntityRecognizer>(locale);
    std::vector<std::vector<int>> res = m->FindEntityInfo(message);
    EXPECT_EQ(res[0][0], 0);
}

/**
 * @tc.name: I18nFuncTest021
 * @tc.desc: Test I18n EntityRecognizer
 * @tc.type: FUNC
 */
HWTEST_F(I18nTest, I18nFuncTest021, TestSize.Level1)
{
    std::string localeStr = "en-GB";
    UErrorCode status = U_ZERO_ERROR;
    icu::Locale locale = icu::Locale::forLanguageTag(localeStr, status);
    std::string message = "New Job: Java Developer, Dublin, Republic of Ireland, $350000.00 - $45000 per annum.";
    std::unique_ptr<EntityRecognizer> m = std::make_unique<EntityRecognizer>(locale);
    std::vector<std::vector<int>> res = m->FindEntityInfo(message);
    EXPECT_EQ(res[0][0], 0);
}

/**
 * @tc.name: I18nFuncTest022
 * @tc.desc: Test I18n EntityRecognizer
 * @tc.type: FUNC
 */
HWTEST_F(I18nTest, I18nFuncTest022, TestSize.Level1)
{
    std::string localeStr = "zh-CN";
    UErrorCode status = U_ZERO_ERROR;
    icu::Locale locale = icu::Locale::forLanguageTag(localeStr, status);
    std::string message = "您好，关于您的问题，可以拨打(0511) 8812 1234咨询，如果还有其他疑问，可联系刘某某（15512345678）";
    std::unique_ptr<EntityRecognizer> m = std::make_unique<EntityRecognizer>(locale);
    std::vector<std::vector<int>> res = m->FindEntityInfo(message);
    EXPECT_EQ(res[0][0], 2);
    EXPECT_EQ(res[0][1], 14);
    EXPECT_EQ(res[0][2], 30);
    EXPECT_EQ(res[0][3], 49);
    EXPECT_EQ(res[0][4], 60);
}

/**
 * @tc.name: I18nFuncTest023
 * @tc.desc: Test I18n EntityRecognizer
 * @tc.type: FUNC
 */
HWTEST_F(I18nTest, I18nFuncTest023, TestSize.Level1)
{
    std::string localeStr = "zh-CN";
    UErrorCode status = U_ZERO_ERROR;
    icu::Locale locale = icu::Locale::forLanguageTag(localeStr, status);
    std::string message = "给10086/10010发了一条短信：“我爱你”，收到了回复：尊敬的用户，我也爱您。";
    std::unique_ptr<EntityRecognizer> m = std::make_unique<EntityRecognizer>(locale);
    std::vector<std::vector<int>> res = m->FindEntityInfo(message);
    EXPECT_EQ(res[0][0], 2);
    EXPECT_EQ(res[0][1], 1);
    EXPECT_EQ(res[0][2], 6);
    EXPECT_EQ(res[0][3], 7);
    EXPECT_EQ(res[0][4], 12);
}

/**
 * @tc.name: I18nFuncTest024
 * @tc.desc: Test I18n EntityRecognizer
 * @tc.type: FUNC
 */
HWTEST_F(I18nTest, I18nFuncTest024, TestSize.Level1)
{
    std::string localeStr = "zh-CN";
    UErrorCode status = U_ZERO_ERROR;
    icu::Locale locale = icu::Locale::forLanguageTag(localeStr, status);
    std::string message = "您好，您的包裹已送至指定地点，请尽快签收：快递员：刘某某，手机：159/1234/5678，QQ：123456789。";
    std::unique_ptr<EntityRecognizer> m = std::make_unique<EntityRecognizer>(locale);
    std::vector<std::vector<int>> res = m->FindEntityInfo(message);
    EXPECT_EQ(res[0][0], 1);
    EXPECT_EQ(res[0][1], 32);
    EXPECT_EQ(res[0][2], 45);
}

/**
 * @tc.name: I18nFuncTest025
 * @tc.desc: Test I18n EntityRecognizer
 * @tc.type: FUNC
 */
HWTEST_F(I18nTest, I18nFuncTest025, TestSize.Level1)
{
    std::string localeStr = "zh-CN";
    UErrorCode status = U_ZERO_ERROR;
    icu::Locale locale = icu::Locale::forLanguageTag(localeStr, status);
    std::string message = "您好，您的包裹已送至指定地点，请尽快签收：快递员：刘某某，手机：15912345678/15512345678。";
    std::unique_ptr<EntityRecognizer> m = std::make_unique<EntityRecognizer>(locale);
    std::vector<std::vector<int>> res = m->FindEntityInfo(message);
    EXPECT_EQ(res[0][0], 2);
    EXPECT_EQ(res[0][1], 32);
    EXPECT_EQ(res[0][2], 43);
    EXPECT_EQ(res[0][3], 44);
    EXPECT_EQ(res[0][4], 55);
}

/**
 * @tc.name: I18nFuncTest026
 * @tc.desc: Test I18n EntityRecognizer
 * @tc.type: FUNC
 */
HWTEST_F(I18nTest, I18nFuncTest026, TestSize.Level1)
{
    std::string localeStr = "zh-CN";
    UErrorCode status = U_ZERO_ERROR;
    icu::Locale locale = icu::Locale::forLanguageTag(localeStr, status);
    std::string message = "今天一起去看的那个电影太搞笑了，2333333";
    std::unique_ptr<EntityRecognizer> m = std::make_unique<EntityRecognizer>(locale);
    std::vector<std::vector<int>> res = m->FindEntityInfo(message);
    EXPECT_EQ(res[0][0], 0);
}

/**
 * @tc.name: I18nFuncTest027
 * @tc.desc: Test I18n EntityRecognizer
 * @tc.type: FUNC
 */
HWTEST_F(I18nTest, I18nFuncTest027, TestSize.Level1)
{
    std::string localeStr = "pt-PT";
    UErrorCode status = U_ZERO_ERROR;
    icu::Locale locale = icu::Locale::forLanguageTag(localeStr, status);
    std::string message = "Se você tiver alguma dúvida, ligue para 912345678 ou 1820 para consulta";
    std::unique_ptr<EntityRecognizer> m = std::make_unique<EntityRecognizer>(locale);
    std::vector<std::vector<int>> res = m->FindEntityInfo(message);
    EXPECT_EQ(res[0][0], 2);
    EXPECT_EQ(res[0][1], 40);
    EXPECT_EQ(res[0][2], 49);
    EXPECT_EQ(res[0][3], 53);
    EXPECT_EQ(res[0][4], 57);
}

/**
 * @tc.name: I18nFuncTest028
 * @tc.desc: Test I18n EntityRecognizer
 * @tc.type: FUNC
 */
HWTEST_F(I18nTest, I18nFuncTest028, TestSize.Level1)
{
    std::string localeStr = "en-GB";
    UErrorCode status = U_ZERO_ERROR;
    icu::Locale locale = icu::Locale::forLanguageTag(localeStr, status);
    std::string message = "+44 (0)20 1234 5678 is my phone number. If you need anything, please contact me.";
    std::unique_ptr<EntityRecognizer> m = std::make_unique<EntityRecognizer>(locale);
    std::vector<std::vector<int>> res = m->FindEntityInfo(message);
    EXPECT_EQ(res[0][0], 1);
    EXPECT_EQ(res[0][1], 0);
    EXPECT_EQ(res[0][2], 19);
}

/**
 * @tc.name: I18nFuncTest029
 * @tc.desc: Test I18n EntityRecognizer
 * @tc.type: FUNC
 */
HWTEST_F(I18nTest, I18nFuncTest029, TestSize.Level1)
{
    std::string localeStr = "en-GB";
    UErrorCode status = U_ZERO_ERROR;
    icu::Locale locale = icu::Locale::forLanguageTag(localeStr, status);
    std::string message = "To book a room, please call (+44 (0)20 1234 5678;ext=588)";
    std::unique_ptr<EntityRecognizer> m = std::make_unique<EntityRecognizer>(locale);
    std::vector<std::vector<int>> res = m->FindEntityInfo(message);
    EXPECT_EQ(res[0][0], 1);
    EXPECT_EQ(res[0][1], 29);
    EXPECT_EQ(res[0][2], 48);
}

/**
 * @tc.name: I18nFuncTest030
 * @tc.desc: Test I18n EntityRecognizer
 * @tc.type: FUNC
 */
HWTEST_F(I18nTest, I18nFuncTest030, TestSize.Level1)
{
    std::string localeStr = "zh-CN";
    UErrorCode status = U_ZERO_ERROR;
    icu::Locale locale = icu::Locale::forLanguageTag(localeStr, status);
    std::string message = "我们提供23 24两种尺寸的屏幕，如有需要，请拨打11808 15512345678。";
    std::unique_ptr<EntityRecognizer> m = std::make_unique<EntityRecognizer>(locale);
    std::vector<std::vector<int>> res = m->FindEntityInfo(message);
    EXPECT_EQ(res[0][0], 1);
    EXPECT_EQ(res[0][1], 25);
    EXPECT_EQ(res[0][2], 42);
}

/**
 * @tc.name: I18nFuncTest031
 * @tc.desc: Test I18n EntityRecognizer
 * @tc.type: FUNC
 */
HWTEST_F(I18nTest, I18nFuncTest031, TestSize.Level1)
{
    std::string localeStr = "zh-CN";
    UErrorCode status = U_ZERO_ERROR;
    icu::Locale locale = icu::Locale::forLanguageTag(localeStr, status);
    std::string message = "售后问题请拨打95528|95188。";
    std::unique_ptr<EntityRecognizer> m = std::make_unique<EntityRecognizer>(locale);
    std::vector<std::vector<int>> res = m->FindEntityInfo(message);
    EXPECT_EQ(res[0][0], 2);
    EXPECT_EQ(res[0][1], 7);
    EXPECT_EQ(res[0][2], 12);
    EXPECT_EQ(res[0][3], 13);
    EXPECT_EQ(res[0][4], 18);
}

/**
 * @tc.name: I18nFuncTest032
 * @tc.desc: Test I18n EntityRecognizer
 * @tc.type: FUNC
 */
HWTEST_F(I18nTest, I18nFuncTest032, TestSize.Level1)
{
    std::string localeStr = "en-GB";
    UErrorCode status = U_ZERO_ERROR;
    icu::Locale locale = icu::Locale::forLanguageTag(localeStr, status);
    std::string message = "If you need anything, please contact mob:(0)20 1234 5678.";
    std::unique_ptr<EntityRecognizer> m = std::make_unique<EntityRecognizer>(locale);
    std::vector<std::vector<int>> res = m->FindEntityInfo(message);
    EXPECT_EQ(res[0][0], 1);
    EXPECT_EQ(res[0][1], 41);
    EXPECT_EQ(res[0][2], 56);
}

/**
 * @tc.name: I18nFuncTest033
 * @tc.desc: Test I18n EntityRecognizer
 * @tc.type: FUNC
 */
HWTEST_F(I18nTest, I18nFuncTest033, TestSize.Level1)
{
    std::string localeStr = "zh-CN";
    UErrorCode status = U_ZERO_ERROR;
    icu::Locale locale = icu::Locale::forLanguageTag(localeStr, status);
    std::string message = "尊敬的客户您好！4G套餐火热申办中，每月最高可获得20G手机上网流量，升级4G套餐享更多优惠。咨询及办理请致电10086。中国移动";
    std::unique_ptr<EntityRecognizer> m = std::make_unique<EntityRecognizer>(locale);
    std::vector<std::vector<int>> res = m->FindEntityInfo(message);
    EXPECT_EQ(res[0][0], 1);
    EXPECT_EQ(res[0][1], 55);
    EXPECT_EQ(res[0][2], 60);
}

/**
 * @tc.name: I18nFuncTest034
 * @tc.desc: Test I18n EntityRecognizer
 * @tc.type: FUNC
 */
HWTEST_F(I18nTest, I18nFuncTest034, TestSize.Level1)
{
    std::string localeStr = "zh-CN";
    UErrorCode status = U_ZERO_ERROR;
    icu::Locale locale = icu::Locale::forLanguageTag(localeStr, status);
    std::string message = "今天晚上10点，我们商量一下10月1日至7日的旅游安排。";
    std::unique_ptr<EntityRecognizer> m = std::make_unique<EntityRecognizer>(locale);
    std::vector<std::vector<int>> res = m->FindEntityInfo(message);
    EXPECT_EQ(res[1][0], 2);
    EXPECT_EQ(res[1][1], 0);
    EXPECT_EQ(res[1][2], 7);
    EXPECT_EQ(res[1][3], 14);
    EXPECT_EQ(res[1][4], 22);
}

/**
 * @tc.name: I18nFuncTest035
 * @tc.desc: Test I18n EntityRecognizer
 * @tc.type: FUNC
 */
HWTEST_F(I18nTest, I18nFuncTest035, TestSize.Level1)
{
    std::string localeStr = "zh-CN";
    UErrorCode status = U_ZERO_ERROR;
    icu::Locale locale = icu::Locale::forLanguageTag(localeStr, status);
    std::string message = "7月1日周一到7月5日周五都是工作日，所以在2023年7月6日下午17:00到19:00聚餐。";
    std::unique_ptr<EntityRecognizer> m = std::make_unique<EntityRecognizer>(locale);
    std::vector<std::vector<int>> res = m->FindEntityInfo(message);
    EXPECT_EQ(res[1][0], 2);
    EXPECT_EQ(res[1][1], 0);
    EXPECT_EQ(res[1][2], 13);
    EXPECT_EQ(res[1][3], 22);
    EXPECT_EQ(res[1][4], 44);
}

/**
 * @tc.name: I18nFuncTest036
 * @tc.desc: Test I18n EntityRecognizer
 * @tc.type: FUNC
 */
HWTEST_F(I18nTest, I18nFuncTest036, TestSize.Level1)
{
    std::string localeStr = "zh-CN";
    UErrorCode status = U_ZERO_ERROR;
    icu::Locale locale = icu::Locale::forLanguageTag(localeStr, status);
    std::string message = "昨天8：30：00的会议没有讨论出结果，我们2023年11月11日的8：30：00再开一次会吧。";
    std::unique_ptr<EntityRecognizer> m = std::make_unique<EntityRecognizer>(locale);
    std::vector<std::vector<int>> res = m->FindEntityInfo(message);
    EXPECT_EQ(res[1][0], 1);
    EXPECT_EQ(res[1][1], 22);
    EXPECT_EQ(res[1][2], 41);
}

/**
 * @tc.name: I18nFuncTest037
 * @tc.desc: Test I18n EntityRecognizer
 * @tc.type: FUNC
 */
HWTEST_F(I18nTest, I18nFuncTest037, TestSize.Level1)
{
    std::string localeStr = "zh-CN";
    UErrorCode status = U_ZERO_ERROR;
    icu::Locale locale = icu::Locale::forLanguageTag(localeStr, status);
    std::string message = "我们2023年10月23日（周六）一起完成作业，并且2023年10月24日周天 晚上8：00上交。";
    std::unique_ptr<EntityRecognizer> m = std::make_unique<EntityRecognizer>(locale);
    std::vector<std::vector<int>> res = m->FindEntityInfo(message);
    EXPECT_EQ(res[1][0], 2);
    EXPECT_EQ(res[1][1], 2);
    EXPECT_EQ(res[1][2], 17);
    EXPECT_EQ(res[1][3], 26);
    EXPECT_EQ(res[1][4], 46);
}

/**
 * @tc.name: I18nFuncTest038
 * @tc.desc: Test I18n EntityRecognizer
 * @tc.type: FUNC
 */
HWTEST_F(I18nTest, I18nFuncTest038, TestSize.Level1)
{
    std::string localeStr = "zh-CN";
    UErrorCode status = U_ZERO_ERROR;
    icu::Locale locale = icu::Locale::forLanguageTag(localeStr, status);
    std::string message = "2023/10/1是国庆节，我们可以在GMT+0800 上午9時30分00秒去参观博物馆，后续星期六 晚上 7：00我们一起吃饭。";
    std::unique_ptr<EntityRecognizer> m = std::make_unique<EntityRecognizer>(locale);
    std::vector<std::vector<int>> res = m->FindEntityInfo(message);
    EXPECT_EQ(res[1][0], 3);
    EXPECT_EQ(res[1][1], 0);
    EXPECT_EQ(res[1][2], 9);
    EXPECT_EQ(res[1][3], 19);
    EXPECT_EQ(res[1][4], 38);
    EXPECT_EQ(res[1][5], 47);
    EXPECT_EQ(res[1][6], 58);
}

/**
 * @tc.name: I18nFuncTest039
 * @tc.desc: Test I18n EntityRecognizer
 * @tc.type: FUNC
 */
HWTEST_F(I18nTest, I18nFuncTest039, TestSize.Level1)
{
    std::string localeStr = "zh-CN";
    UErrorCode status = U_ZERO_ERROR;
    icu::Locale locale = icu::Locale::forLanguageTag(localeStr, status);
    std::string message = "我们上午 7：30：00 （GMT+8:30）可以去看日出，下午5：00-晚上7：00看日落。";
    std::unique_ptr<EntityRecognizer> m = std::make_unique<EntityRecognizer>(locale);
    std::vector<std::vector<int>> res = m->FindEntityInfo(message);
    EXPECT_EQ(res[1][0], 2);
    EXPECT_EQ(res[1][1], 2);
    EXPECT_EQ(res[1][2], 23);
    EXPECT_EQ(res[1][3], 30);
    EXPECT_EQ(res[1][4], 43);
}

/**
 * @tc.name: I18nFuncTest040
 * @tc.desc: Test I18n EntityRecognizer
 * @tc.type: FUNC
 */
HWTEST_F(I18nTest, I18nFuncTest040, TestSize.Level1)
{
    std::string localeStr = "en-GB";
    UErrorCode status = U_ZERO_ERROR;
    icu::Locale locale = icu::Locale::forLanguageTag(localeStr, status);
    std::string message = "We'll have dinner at GMT-8:15 PM 17:30:00, and go shopping at tomorrow 8:00.";
    std::unique_ptr<EntityRecognizer> m = std::make_unique<EntityRecognizer>(locale);
    std::vector<std::vector<int>> res = m->FindEntityInfo(message);
    EXPECT_EQ(res[1][0], 2);
    EXPECT_EQ(res[1][1], 21);
    EXPECT_EQ(res[1][2], 41);
    EXPECT_EQ(res[1][3], 62);
    EXPECT_EQ(res[1][4], 75);
}

/**
 * @tc.name: I18nFuncTest041
 * @tc.desc: Test I18n EntityRecognizer
 * @tc.type: FUNC
 */
HWTEST_F(I18nTest, I18nFuncTest041, TestSize.Level1)
{
    std::string localeStr = "en-GB";
    UErrorCode status = U_ZERO_ERROR;
    icu::Locale locale = icu::Locale::forLanguageTag(localeStr, status);
    std::string message = "Our time zone is GMT+12:00.";
    std::unique_ptr<EntityRecognizer> m = std::make_unique<EntityRecognizer>(locale);
    std::vector<std::vector<int>> res = m->FindEntityInfo(message);
    EXPECT_EQ(res[1][0], 0);
}

/**
 * @tc.name: I18nFuncTest042
 * @tc.desc: Test I18n EntityRecognizer
 * @tc.type: FUNC
 */
HWTEST_F(I18nTest, I18nFuncTest042, TestSize.Level1)
{
    std::string localeStr = "en-GB";
    UErrorCode status = U_ZERO_ERROR;
    icu::Locale locale = icu::Locale::forLanguageTag(localeStr, status);
    std::string message = "The festivities will take place on Sunday, jan 1, 2023, and run until Wed, 1/4/2023";
    std::unique_ptr<EntityRecognizer> m = std::make_unique<EntityRecognizer>(locale);
    std::vector<std::vector<int>> res = m->FindEntityInfo(message);
    EXPECT_EQ(res[1][0], 2);
    EXPECT_EQ(res[1][1], 35);
    EXPECT_EQ(res[1][2], 54);
    EXPECT_EQ(res[1][3], 70);
    EXPECT_EQ(res[1][4], 83);
}

/**
 * @tc.name: I18nFuncTest043
 * @tc.desc: Test I18n EntityRecognizer
 * @tc.type: FUNC
 */
HWTEST_F(I18nTest, I18nFuncTest043, TestSize.Level1)
{
    std::string localeStr = "zh-CN";
    UErrorCode status = U_ZERO_ERROR;
    icu::Locale locale = icu::Locale::forLanguageTag(localeStr, status);
    // std::string message = "2023年1月2日至3日周三是一个好日子，上午可以11：00：00（周三）去逛街";
    std::string message = "2023年1月2日至3日是一个好日子,12：00：00（2023年1月3日）一起吃饭。";
    std::unique_ptr<EntityRecognizer> m = std::make_unique<EntityRecognizer>(locale);
    std::vector<std::vector<int>> res = m->FindEntityInfo(message);
    EXPECT_EQ(res[1][0], 2);
    EXPECT_EQ(res[1][1], 0);
    EXPECT_EQ(res[1][2], 12);
    EXPECT_EQ(res[1][3], 19);
    EXPECT_EQ(res[1][4], 38);
}

/**
 * @tc.name: I18nFuncTest044
 * @tc.desc: Test I18n EntityRecognizer
 * @tc.type: FUNC
 */
HWTEST_F(I18nTest, I18nFuncTest044, TestSize.Level1)
{
    std::string localeStr = "zh-CN";
    UErrorCode status = U_ZERO_ERROR;
    icu::Locale locale = icu::Locale::forLanguageTag(localeStr, status);
    std::string message = "(2023年1月1日)12:00:00聚会,2023年1月2日，16：00：00返回深圳。";
    std::unique_ptr<EntityRecognizer> m = std::make_unique<EntityRecognizer>(locale);
    std::vector<std::vector<int>> res = m->FindEntityInfo(message);
    EXPECT_EQ(res[1][0], 2);
    EXPECT_EQ(res[1][1], 0);
    EXPECT_EQ(res[1][2], 19);
    EXPECT_EQ(res[1][3], 22);
    EXPECT_EQ(res[1][4], 40);
}


/**
 * @tc.name: I18nFuncTest045
 * @tc.desc: Test I18n EntityRecognizer
 * @tc.type: FUNC
 */
HWTEST_F(I18nTest, I18nFuncTest045, TestSize.Level1)
{
    std::string localeStr = "zh-CN";
    UErrorCode status = U_ZERO_ERROR;
    icu::Locale locale = icu::Locale::forLanguageTag(localeStr, status);
    std::string message = "2023年1月3日(今天(本周五))和2023年1月3日(12：00：00)是指同一天";
    std::unique_ptr<EntityRecognizer> m = std::make_unique<EntityRecognizer>(locale);
    std::vector<std::vector<int>> res = m->FindEntityInfo(message);
    EXPECT_EQ(res[1][0], 3);
    EXPECT_EQ(res[1][1], 0);
    EXPECT_EQ(res[1][2], 17);
    EXPECT_EQ(res[1][3], 19);
    EXPECT_EQ(res[1][4], 28);
    EXPECT_EQ(res[1][5], 29);
    EXPECT_EQ(res[1][6], 37);
}

/**
 * @tc.name: I18nFuncTest046
 * @tc.desc: Test I18n EntityRecognizer
 * @tc.type: FUNC
 */
HWTEST_F(I18nTest, I18nFuncTest046, TestSize.Level1)
{
    std::string localeStr = "zh-CN";
    UErrorCode status = U_ZERO_ERROR;
    icu::Locale locale = icu::Locale::forLanguageTag(localeStr, status);
    std::string message = "2023年1月3日今天(本周五)在7：00：00的2023年1月3日可以看日出";
    std::unique_ptr<EntityRecognizer> m = std::make_unique<EntityRecognizer>(locale);
    std::vector<std::vector<int>> res = m->FindEntityInfo(message);
    EXPECT_EQ(res[1][0], 2);
    EXPECT_EQ(res[1][1], 0);
    EXPECT_EQ(res[1][2], 16);
    EXPECT_EQ(res[1][3], 17);
    EXPECT_EQ(res[1][4], 34);
}

/**
 * @tc.name: I18nFuncTest047
 * @tc.desc: Test I18n LocaleUtil
 * @tc.type: FUNC
 */
HWTEST_F(I18nTest, I18nFuncTest047, TestSize.Level1)
{
    LocaleInfo *locale = new LocaleInfo("zh-Hans-CN");
    uint16_t encodedLanguage = LocaleUtil::EncodeLanguageByLocaleInfo(locale);
    EXPECT_EQ(encodedLanguage, 31336);
    delete locale;
}

/**
 * @tc.name: I18nFuncTest048
 * @tc.desc: Test I18n LocaleUtil
 * @tc.type: FUNC
 */
HWTEST_F(I18nTest, I18nFuncTest048, TestSize.Level1)
{
    LocaleInfo *locale = new LocaleInfo("zh-Hans-CN");
    uint16_t encodedScript = LocaleUtil::EncodeScriptByLocaleInfo(locale);
    EXPECT_EQ(encodedScript, 28275);
    delete locale;
}

/**
 * @tc.name: I18nFuncTest049
 * @tc.desc: Test I18n LocaleUtil
 * @tc.type: FUNC
 */
HWTEST_F(I18nTest, I18nFuncTest049, TestSize.Level1)
{
    LocaleInfo *locale = new LocaleInfo("zh-Hans-CN");
    uint16_t encodedRegion = LocaleUtil::EncodeRegionByLocaleInfo(locale);
    EXPECT_EQ(encodedRegion, 17230);
    uint16_t result = LocaleUtil::EncodeRegionByLocaleInfo(nullptr);
    EXPECT_EQ(result, 0);
    delete locale;
}

/**
 * @tc.name: I18nFuncTest050
 * @tc.desc: Test I18n LocaleUtil
 * @tc.type: FUNC
 */
HWTEST_F(I18nTest, I18nFuncTest050, TestSize.Level1)
{
    LocaleInfo locale("zh-Hans-CN");
    uint16_t encodedLanguage = LocaleUtil::EncodeLanguage(locale.GetLanguage().c_str());
    EXPECT_EQ(encodedLanguage, 31336);
}

/**
 * @tc.name: I18nFuncTest051
 * @tc.desc: Test I18n LocaleUtil
 * @tc.type: FUNC
 */
HWTEST_F(I18nTest, I18nFuncTest051, TestSize.Level1)
{
    LocaleInfo locale("zh-Hans-CN");
    uint16_t encodedScript = LocaleUtil::EncodeScript(locale.GetScript().c_str());
    EXPECT_EQ(encodedScript, 28275);
}

/**
 * @tc.name: I18nFuncTest052
 * @tc.desc: Test I18n LocaleUtil
 * @tc.type: FUNC
 */
HWTEST_F(I18nTest, I18nFuncTest052, TestSize.Level1)
{
    LocaleInfo locale("zh-Hans-CN");
    uint16_t encodedRegion = LocaleUtil::EncodeRegion(locale.GetRegion().c_str());
    EXPECT_EQ(encodedRegion, 17230);
}

/**
 * @tc.name: I18nFuncTest053
 * @tc.desc: Test I18n LocaleUtil
 * @tc.type: FUNC
 */
HWTEST_F(I18nTest, I18nFuncTest053, TestSize.Level1)
{
    LocaleInfo locale("zh-Hans-CN");
    uint16_t encodedLocale = LocaleUtil::EncodeLocale(locale.GetLanguage().c_str(), locale.GetScript().c_str(),
        locale.GetRegion().c_str());
    EXPECT_EQ(encodedLocale, 17230);
}

/**
 * @tc.name: I18nFuncTest054
 * @tc.desc: Test I18n LocaleUtil
 * @tc.type: FUNC
 */
HWTEST_F(I18nTest, I18nFuncTest054, TestSize.Level1)
{
    std::string str1 = "non-empty";
    std::string str2 = "";
    bool isEmpty = LocaleUtil::IsStrEmpty(str1.c_str());
    EXPECT_FALSE(isEmpty);
    isEmpty = LocaleUtil::IsStrEmpty(str2.c_str());
    EXPECT_TRUE(isEmpty);
}

/**
 * @tc.name: I18nFuncTest055
 * @tc.desc: Test I18n LocaleUtil
 * @tc.type: FUNC
 */
HWTEST_F(I18nTest, I18nFuncTest055, TestSize.Level1)
{
    LocaleInfo locale("zh-Hans-CN");
    uint32_t encodedScript = LocaleUtil::EncodeScript(locale.GetScript().c_str());
    char decodedScript[5] = { 0 };
    LocaleUtil::DecodeScript(encodedScript, decodedScript);
    std::string originScript = "Hans";
    EXPECT_EQ(originScript.compare(decodedScript), 0);
    LocaleUtil::DecodeScript(encodedScript, nullptr);
}

/**
 * @tc.name: I18nFuncTest056
 * @tc.desc: Test I18n LocaleUtil
 * @tc.type: FUNC
 */
HWTEST_F(I18nTest, I18nFuncTest056, TestSize.Level1)
{
    std::string alphaString = "abc";
    std::string nonAlphaString = "abc123abc";
    bool isAlpha = LocaleUtil::IsAlphaString(alphaString.c_str(), alphaString.length());
    EXPECT_TRUE(isAlpha);
    isAlpha = LocaleUtil::IsAlphaString(nonAlphaString.c_str(), nonAlphaString.length());
    EXPECT_FALSE(isAlpha);
    isAlpha = LocaleUtil::IsAlphaString(nullptr, nonAlphaString.length());
    EXPECT_FALSE(isAlpha);
}

/**
 * @tc.name: I18nFuncTest057
 * @tc.desc: Test I18n LocaleUtil
 * @tc.type: FUNC
 */
HWTEST_F(I18nTest, I18nFuncTest057, TestSize.Level1)
{
    std::string numericString = "123";
    std::string nonNumericString = "123abc123";
    bool isNumeric = LocaleUtil::IsNumericString(numericString.c_str(), numericString.length());
    EXPECT_TRUE(isNumeric);
    isNumeric = LocaleUtil::IsNumericString(nonNumericString.c_str(), nonNumericString.length());
    EXPECT_FALSE(isNumeric);
    isNumeric = LocaleUtil::IsNumericString(nullptr, nonNumericString.length());
    EXPECT_FALSE(isNumeric);
    std::string locale = "ug-CN";
    bool rtl = LocaleUtil::IsRTL(locale);
    EXPECT_TRUE(rtl);
}

/**
 * @tc.name: I18nFuncTest058
 * @tc.desc: Test I18n LocaleUtil
 * @tc.type: FUNC
 */
HWTEST_F(I18nTest, I18nFuncTest058, TestSize.Level1)
{
    LocaleInfo *locale1 = new LocaleInfo("zh-Hans-CN");
    LocaleInfo *locale2 = new LocaleInfo("en-Latn-US");
    bool isMatched = LocaleMatcher::Match(locale1, locale2);
    EXPECT_FALSE(isMatched);
    delete locale1;
    delete locale2;

    locale1 = new LocaleInfo("zh-Hans-CN");
    locale2 = new LocaleInfo("zh-Hant-TW");
    isMatched = LocaleMatcher::Match(locale1, locale2);
    EXPECT_FALSE(isMatched);
    delete locale1;
    delete locale2;

    locale1 = new LocaleInfo("zh-Hans-CN");
    locale2 = new LocaleInfo("zh-Hant-TW");
    isMatched = LocaleMatcher::Match(locale1, locale2);
    EXPECT_FALSE(isMatched);
    delete locale1;
    delete locale2;

    locale1 = new LocaleInfo("zh-Hans-CN");
    locale2 = new LocaleInfo("zh-Hans-MO");
    isMatched = LocaleMatcher::Match(locale1, locale2);
    EXPECT_TRUE(isMatched);
    delete locale1;
    delete locale2;
}

/**
 * @tc.name: I18nFuncTest059
 * @tc.desc: Test I18n LocaleUtil
 * @tc.type: FUNC
 */
HWTEST_F(I18nTest, I18nFuncTest059, TestSize.Level1)
{
    LocaleInfo *request = nullptr;
    LocaleInfo *current = new LocaleInfo("zh-Hans-CN");
    LocaleInfo *other = nullptr;
    int8_t res = LocaleMatcher::IsMoreSuitable(current, other, request);
    EXPECT_TRUE(res < 0);
    delete current;
    current = nullptr;

    res = LocaleMatcher::IsMoreSuitable(current, other, request);
    EXPECT_TRUE(res == 0);

    request = new LocaleInfo("en-GB");
    current = new LocaleInfo("en-AU");
    res = LocaleMatcher::IsMoreSuitable(current, other, request);
    EXPECT_TRUE(res > 0);

    other = new LocaleInfo("en-GB");
    res = LocaleMatcher::IsMoreSuitable(current, other, request);
    EXPECT_TRUE(res < 0);

    delete request;
    delete current;
    delete other;
    request = new LocaleInfo("iw-Lant-GB");
    current = new LocaleInfo("iw-Lant-AU");
    other = new LocaleInfo("he-Latn-AU");
    res = LocaleMatcher::IsMoreSuitable(current, other, request);
    EXPECT_TRUE(res > 0);
    delete other;

    other = new LocaleInfo("iw-Latn-AG");
    res = LocaleMatcher::IsMoreSuitable(current, other, request);
    EXPECT_TRUE(res < 0);
    delete request;
    delete current;
    delete other;
}

/**
 * @tc.name: I18nFuncTest061
 * @tc.desc: Test I18n GetISO3Language
 * @tc.type: FUNC
 */
HWTEST_F(I18nTest, I18nFuncTest061, TestSize.Level1)
{
    std::string language = GetISO3Language("zh");
    EXPECT_EQ(language, "zho");
    language = GetISO3Language("en");
    EXPECT_EQ(language, "eng");
    language = GetISO3Language("zh-CN");
    EXPECT_EQ(language, "zho");
    language = GetISO3Language("en-US");
    EXPECT_EQ(language, "eng");
    language = GetISO3Language("zz");
    EXPECT_EQ(language, "");
    language = GetISO3Language("sdfsdf");
    EXPECT_EQ(language, "");
}

/**
 * @tc.name: I18nFuncTest062
 * @tc.desc: Test I18n GetISO3Country
 * @tc.type: FUNC
 */
HWTEST_F(I18nTest, I18nFuncTest062, TestSize.Level1)
{
    std::string country = GetISO3Country("CN");
    EXPECT_EQ(country, "CHN");
    country = GetISO3Country("US");
    EXPECT_EQ(country, "USA");
    country = GetISO3Country("zh-CN");
    EXPECT_EQ(country, "CHN");
    country = GetISO3Country("en-US");
    EXPECT_EQ(country, "USA");
    country = GetISO3Country("ZX");
    EXPECT_EQ(country, "");
    country = GetISO3Country("zh");
    EXPECT_EQ(country, "");
    country = GetISO3Country("sdfsdf");
    EXPECT_EQ(country, "");
}

/**
 * @tc.name: I18nFuncTest063
 * @tc.desc: Test I18n lunar calendar
 * @tc.type: FUNC
 */
HWTEST_F(I18nTest, I18nFuncTest063, TestSize.Level1)
{
    std::unique_ptr<LunarCalendar> ptr = std::make_unique<LunarCalendar>();
    ASSERT_TRUE(ptr != nullptr);

    ptr->SetGregorianDate(1900, 1, 31);
    int32_t year = ptr->GetLunarYear();
    int32_t month = ptr->GetLunarMonth();
    int32_t day = ptr->GetLunarDay();
    bool isLeap = ptr->IsLeapMonth();
    EXPECT_EQ(year, 1900);
    EXPECT_EQ(month, 1);
    EXPECT_EQ(day, 1);
    EXPECT_EQ(isLeap, false);

    ptr->SetGregorianDate(1900, 6, 15);
    year = ptr->GetLunarYear();
    month = ptr->GetLunarMonth();
    day = ptr->GetLunarDay();
    isLeap = ptr->IsLeapMonth();
    EXPECT_EQ(year, 1900);
    EXPECT_EQ(month, 5);
    EXPECT_EQ(day, 19);
    EXPECT_EQ(isLeap, false);

    ptr->SetGregorianDate(2100, 2, 15);
    year = ptr->GetLunarYear();
    month = ptr->GetLunarMonth();
    day = ptr->GetLunarDay();
    isLeap = ptr->IsLeapMonth();
    EXPECT_EQ(year, 2100);
    EXPECT_EQ(month, 1);
    EXPECT_EQ(day, 7);
    EXPECT_EQ(isLeap, false);

    ptr->SetGregorianDate(2100, 12, 31);
    year = ptr->GetLunarYear();
    month = ptr->GetLunarMonth();
    day = ptr->GetLunarDay();
    isLeap = ptr->IsLeapMonth();
    EXPECT_EQ(year, 2100);
    EXPECT_EQ(month, 12);
    EXPECT_EQ(day, 1);
    EXPECT_EQ(isLeap, false);
}

/**
 * @tc.name: I18nFuncTest064
 * @tc.desc: Test I18n lunar calendar
 * @tc.type: FUNC
 */
HWTEST_F(I18nTest, I18nFuncTest064, TestSize.Level1)
{
    std::unique_ptr<LunarCalendar> ptr = std::make_unique<LunarCalendar>();
    ASSERT_TRUE(ptr != nullptr);

    ptr->SetGregorianDate(2087, 1, 11);
    int32_t year = ptr->GetLunarYear();
    int32_t month = ptr->GetLunarMonth();
    int32_t day = ptr->GetLunarDay();
    bool isLeap = ptr->IsLeapMonth();
    EXPECT_EQ(year, 2086);
    EXPECT_EQ(month, 12);
    EXPECT_EQ(day, 7);
    EXPECT_EQ(isLeap, false);

    ptr->SetGregorianDate(2024, 10, 11);
    year = ptr->GetLunarYear();
    month = ptr->GetLunarMonth();
    day = ptr->GetLunarDay();
    isLeap = ptr->IsLeapMonth();
    EXPECT_EQ(year, 2024);
    EXPECT_EQ(month, 9);
    EXPECT_EQ(day, 9);
    EXPECT_EQ(isLeap, false);

    ptr->SetGregorianDate(1963, 6, 15);
    year = ptr->GetLunarYear();
    month = ptr->GetLunarMonth();
    day = ptr->GetLunarDay();
    isLeap = ptr->IsLeapMonth();
    EXPECT_EQ(year, 1963);
    EXPECT_EQ(month, 4);
    EXPECT_EQ(day, 24);
    EXPECT_EQ(isLeap, true);

    ptr->SetGregorianDate(1923, 1, 11);
    year = ptr->GetLunarYear();
    month = ptr->GetLunarMonth();
    day = ptr->GetLunarDay();
    isLeap = ptr->IsLeapMonth();
    EXPECT_EQ(year, 1922);
    EXPECT_EQ(month, 11);
    EXPECT_EQ(day, 25);
    EXPECT_EQ(isLeap, false);
}

/**
 * @tc.name: I18nFuncTest065
 * @tc.desc: Test I18n lunar calendar
 * @tc.type: FUNC
 */
HWTEST_F(I18nTest, I18nFuncTest065, TestSize.Level1)
{
    std::unique_ptr<LunarCalendar> ptr = std::make_unique<LunarCalendar>();
    ASSERT_TRUE(ptr != nullptr);

    ptr->SetGregorianDate(2024, 8, 51);
    int32_t year = ptr->GetLunarYear();
    int32_t month = ptr->GetLunarMonth();
    int32_t day = ptr->GetLunarDay();
    bool isLeap = ptr->IsLeapMonth();
    EXPECT_EQ(year, 2024);
    EXPECT_EQ(month, 8);
    EXPECT_EQ(day, 18);
    EXPECT_EQ(isLeap, false);

    ptr->SetGregorianDate(2024, 12, 43);
    year = ptr->GetLunarYear();
    month = ptr->GetLunarMonth();
    day = ptr->GetLunarDay();
    isLeap = ptr->IsLeapMonth();
    EXPECT_EQ(year, 2024);
    EXPECT_EQ(month, 12);
    EXPECT_EQ(day, 13);
    EXPECT_EQ(isLeap, false);

    ptr->SetGregorianDate(2000, 7, 13);
    EXPECT_EQ(ptr->GetLunarDay(), 12);

    ptr->SetGregorianDate(2024, 8, -11);
    EXPECT_EQ(ptr->GetLunarDay(), 15);
    ptr->SetGregorianDate(2024, -3, -11);
    EXPECT_EQ(ptr->GetLunarMonth(), 7);
    ptr->SetGregorianDate(-24, -3, -11);
    EXPECT_EQ(ptr->GetLunarYear(), -1);
    EXPECT_FALSE(ptr->IsLeapMonth());
}

/**
 * @tc.name: I18nFuncTest066
 * @tc.desc: Test I18n set ext param
 * @tc.type: FUNC
 */
HWTEST_F(I18nTest, I18nFuncTest066, TestSize.Level1)
{
    I18nErrorCode err = LocaleConfig::SetTemperatureType(TemperatureType::KELVIN);
    EXPECT_EQ(err, I18nErrorCode::SUCCESS);

    TemperatureType type = LocaleConfig::GetTemperatureType();
    EXPECT_EQ(type, TemperatureType::KELVIN);

    std::string typeName = LocaleConfig::GetTemperatureName(type);
    EXPECT_EQ(typeName, "kelvin");

    type = LocaleConfig::GetTemperatureTypeFromLocale("zh-Hans-CN-u-mu-kelvin");
    EXPECT_EQ(type, TemperatureType::KELVIN);

    WeekDay weekDayType = LocaleConfig::GetFirstDayOfWeek();
    EXPECT_EQ(weekDayType, WeekDay::SUN);

    err = LocaleConfig::SetFirstDayOfWeek(WeekDay::FRI);
    EXPECT_EQ(err, I18nErrorCode::SUCCESS);

    weekDayType = LocaleConfig::GetFirstDayOfWeek();
    EXPECT_EQ(weekDayType, WeekDay::FRI);
}

/**
 * @tc.name: I18nFuncTest067
 * @tc.desc: Test I18n simplified language
 * @tc.type: FUNC
 */
HWTEST_F(I18nTest, I18nFuncTest067, TestSize.Level1)
{
    LocaleConfig::SetSystemLanguage("en-Latn");
    LocaleConfig::SetSystemRegion("US");
    EXPECT_EQ(LocaleConfig::GetSimplifiedSystemLanguage(), "en");
    LocaleConfig::SetSystemRegion("GB");
    EXPECT_EQ(LocaleConfig::GetSimplifiedSystemLanguage(), "en-GB");
    LocaleConfig::SetSystemLanguage("zh-Hant");
    LocaleConfig::SetSystemRegion("CN");
    EXPECT_EQ(LocaleConfig::GetSimplifiedSystemLanguage(), "zh-Hant");
    LocaleConfig::SetSystemRegion("HK");
    EXPECT_EQ(LocaleConfig::GetSimplifiedSystemLanguage(), "zh-Hant-HK");
    LocaleConfig::SetSystemLanguage("es");
    LocaleConfig::SetSystemRegion("ES");
    EXPECT_EQ(LocaleConfig::GetSimplifiedSystemLanguage(), "es");
    LocaleConfig::SetSystemRegion("US");
    EXPECT_EQ(LocaleConfig::GetSimplifiedSystemLanguage(), "es");
    LocaleConfig::SetSystemLanguage("es-US");
    LocaleConfig::SetSystemRegion("ES");
    EXPECT_EQ(LocaleConfig::GetSimplifiedSystemLanguage(), "es-US");
    LocaleConfig::SetSystemRegion("US");
    EXPECT_EQ(LocaleConfig::GetSimplifiedSystemLanguage(), "es-US");
    LocaleConfig::SetSystemLanguage("pt");
    LocaleConfig::SetSystemRegion("PT");
    EXPECT_EQ(LocaleConfig::GetSimplifiedSystemLanguage(), "pt");
    LocaleConfig::SetSystemRegion("BR");
    EXPECT_EQ(LocaleConfig::GetSimplifiedSystemLanguage(), "pt");
    LocaleConfig::SetSystemLanguage("pt-PT");
    LocaleConfig::SetSystemRegion("PT");
    EXPECT_EQ(LocaleConfig::GetSimplifiedSystemLanguage(), "pt-PT");
    LocaleConfig::SetSystemRegion("BR");
    EXPECT_EQ(LocaleConfig::GetSimplifiedSystemLanguage(), "pt-PT");
    LocaleConfig::SetSystemLanguage("ug-Arab");
    LocaleConfig::SetSystemRegion("CN");
    EXPECT_EQ(LocaleConfig::GetSimplifiedSystemLanguage(), "ug");

    int32_t code = 0;
    EXPECT_EQ(LocaleConfig::GetSimplifiedLanguage("zh-Hant-CN", code), "zh-Hant-CN");
    EXPECT_EQ(code, 0);
    EXPECT_EQ(LocaleConfig::GetSimplifiedLanguage("cr-Cans-CA", code), "cr");
    EXPECT_EQ(code, 0);
    LocaleConfig::SetSystemLanguage("zh-Hans");
}

/**
 * @tc.name: I18nFuncTest068
 * @tc.desc: Test I18n simple number format
 * @tc.type: FUNC
 */
HWTEST_F(I18nTest, I18nFuncTest068, TestSize.Level1)
{
    I18nErrorCode err = I18nErrorCode::SUCCESS;
    std::string skeleton = "percent";
    std::shared_ptr<LocaleInfo> localeInfo = std::make_shared<LocaleInfo>("zh-Hans-CN");
    ASSERT_TRUE(localeInfo != nullptr);
    std::unique_ptr<SimpleNumberFormat> formatter = std::make_unique<SimpleNumberFormat>(skeleton, localeInfo, err);
    EXPECT_EQ(err, I18nErrorCode::SUCCESS);
    ASSERT_TRUE(formatter != nullptr);
    std::string result = formatter->Format(10);
    EXPECT_EQ(result, "10%");

    skeleton = "%";
    std::shared_ptr<LocaleInfo> nullLocaleInfo = nullptr;
    std::unique_ptr<SimpleNumberFormat> formatterWithoutLocale =
        std::make_unique<SimpleNumberFormat>(skeleton, nullLocaleInfo, err);
    EXPECT_EQ(err, I18nErrorCode::SUCCESS);
    ASSERT_TRUE(formatterWithoutLocale != nullptr);
    result = formatterWithoutLocale->Format(10);
    EXPECT_EQ(result, "10%");

    skeleton = "5%";
    std::unique_ptr<SimpleNumberFormat> formatterWithError =
        std::make_unique<SimpleNumberFormat>(skeleton, nullLocaleInfo, err);
    EXPECT_EQ(err, I18nErrorCode::INVALID_NUMBER_FORMAT_SKELETON);

    std::unique_ptr<SimpleNumberFormat> formatterWithEmpty =
        std::make_unique<SimpleNumberFormat>("", nullLocaleInfo, err);
    EXPECT_EQ(err, I18nErrorCode::INVALID_NUMBER_FORMAT_SKELETON);
}

/**
 * @tc.name: I18nFuncTest069
 * @tc.desc: Test I18n simple date time format
 * @tc.type: FUNC
 */
HWTEST_F(I18nTest, I18nFuncTest069, TestSize.Level1)
{
    I18nErrorCode err = I18nErrorCode::SUCCESS;
    std::shared_ptr<LocaleInfo> localeInfo = std::make_shared<LocaleInfo>("zh-Hans-CN");
    ASSERT_TRUE(localeInfo != nullptr);

    std::string skeleton = "yMd";
    std::unique_ptr<SimpleDateTimeFormat> skeletonFormatter =
        std::make_unique<SimpleDateTimeFormat>(skeleton, localeInfo, false, err);
    EXPECT_EQ(err, I18nErrorCode::SUCCESS);
    ASSERT_TRUE(skeletonFormatter != nullptr);

    int64_t milliseconds = 98765432100;
    std::string result = skeletonFormatter->Format(milliseconds);
    EXPECT_EQ(result, "1973/2/17");
    std::shared_ptr<LocaleInfo> nullLocaleInfo = nullptr;
    std::unique_ptr<SimpleDateTimeFormat> formatterWithoutLocale =
        std::make_unique<SimpleDateTimeFormat>(skeleton, nullLocaleInfo, false, err);
    EXPECT_EQ(err, I18nErrorCode::SUCCESS);

    skeleton = "y/M/d";
    std::unique_ptr<SimpleDateTimeFormat> formatterInvalidSkeleton =
        std::make_unique<SimpleDateTimeFormat>(skeleton, localeInfo, false, err);
    EXPECT_EQ(err, I18nErrorCode::INVALID_DATE_TIME_FORMAT_SKELETON);

    std::unique_ptr<SimpleDateTimeFormat> formatterWithEmptySkeleton =
        std::make_unique<SimpleDateTimeFormat>("", localeInfo, false, err);
    EXPECT_EQ(err, I18nErrorCode::INVALID_DATE_TIME_FORMAT_SKELETON);
}

/**
 * @tc.name: I18nFuncTest070
 * @tc.desc: Test I18n simple date time format
 * @tc.type: FUNC
 */
HWTEST_F(I18nTest, I18nFuncTest070, TestSize.Level1)
{
    I18nErrorCode err = I18nErrorCode::SUCCESS;
    std::shared_ptr<LocaleInfo> localeInfo = std::make_shared<LocaleInfo>("zh-Hans-CN");
    ASSERT_TRUE(localeInfo != nullptr);

    std::string pattern = "yMd";
    std::unique_ptr<SimpleDateTimeFormat> patternFormatter =
        std::make_unique<SimpleDateTimeFormat>(pattern, localeInfo, true, err);
    EXPECT_EQ(err, I18nErrorCode::SUCCESS);
    ASSERT_TRUE(patternFormatter != nullptr);

    int64_t milliseconds = 98765432100;
    std::string result = patternFormatter->Format(milliseconds);
    EXPECT_EQ(result, "1973217");

    pattern = "'year('y')month('M')day('d')'";
    std::unique_ptr<SimpleDateTimeFormat> formatterWithContent =
        std::make_unique<SimpleDateTimeFormat>(pattern, localeInfo, true, err);
    EXPECT_EQ(err, I18nErrorCode::SUCCESS);
    ASSERT_TRUE(formatterWithContent != nullptr);
    result = formatterWithContent->Format(milliseconds);
    EXPECT_EQ(result, "year(1973)month(2)day(17)");

    pattern = "y''Md";
    std::unique_ptr<SimpleDateTimeFormat> formatterInvalidPattern =
        std::make_unique<SimpleDateTimeFormat>(pattern, localeInfo, true, err);
    EXPECT_EQ(err, I18nErrorCode::INVALID_DATE_TIME_FORMAT_PATTERN);

    std::unique_ptr<SimpleDateTimeFormat> formatterWithEmptyPattern =
        std::make_unique<SimpleDateTimeFormat>("", localeInfo, true, err);
    EXPECT_EQ(err, I18nErrorCode::INVALID_DATE_TIME_FORMAT_PATTERN);
}

/**
 * @tc.name: I18nFuncTest071
 * @tc.desc: Test I18n simple number format
 * @tc.type: FUNC
 */
HWTEST_F(I18nTest, I18nFuncTest071, TestSize.Level1)
{
    I18nErrorCode err = I18nErrorCode::SUCCESS;
    std::string skeleton = "percent";
    std::string localeTag = "zh-Hans-CN";
    std::unique_ptr<SimpleNumberFormat> formatter = std::make_unique<SimpleNumberFormat>(skeleton, localeTag, err);
    EXPECT_EQ(err, I18nErrorCode::SUCCESS);
    ASSERT_TRUE(formatter != nullptr);
    std::string result = formatter->Format(10);
    EXPECT_EQ(result, "10%");

    skeleton = "%";
    std::unique_ptr<SimpleNumberFormat> formatterWithoutLocale =
        std::make_unique<SimpleNumberFormat>(skeleton, "", err);
    EXPECT_EQ(err, I18nErrorCode::SUCCESS);
    ASSERT_TRUE(formatterWithoutLocale != nullptr);
    result = formatterWithoutLocale->Format(10);
    EXPECT_EQ(result, "10%");

    skeleton = "5%";
    std::unique_ptr<SimpleNumberFormat> formatterWithError =
        std::make_unique<SimpleNumberFormat>(skeleton, "", err);
    EXPECT_EQ(err, I18nErrorCode::INVALID_NUMBER_FORMAT_SKELETON);

    std::unique_ptr<SimpleNumberFormat> formatterWithEmpty =
        std::make_unique<SimpleNumberFormat>("", "", err);
    EXPECT_EQ(err, I18nErrorCode::INVALID_NUMBER_FORMAT_SKELETON);
}

/**
 * @tc.name: I18nFuncTest072
 * @tc.desc: Test I18n simple date time format
 * @tc.type: FUNC
 */
HWTEST_F(I18nTest, I18nFuncTest072, TestSize.Level1)
{
    I18nErrorCode err = I18nErrorCode::SUCCESS;
    std::string localeTag = "zh-Hans-CN";
    std::string pattern = "yMd";
    std::unique_ptr<SimpleDateTimeFormat> patternFormatter =
        std::make_unique<SimpleDateTimeFormat>(pattern, localeTag, true, err);
    EXPECT_EQ(err, I18nErrorCode::SUCCESS);
    ASSERT_TRUE(patternFormatter != nullptr);

    int64_t milliseconds = 98765432100;
    std::string result = patternFormatter->Format(milliseconds);
    EXPECT_EQ(result, "1973217");

    pattern = "'year('y')month('M')day('d')'";
    std::unique_ptr<SimpleDateTimeFormat> formatterWithContent =
        std::make_unique<SimpleDateTimeFormat>(pattern, localeTag, true, err);
    EXPECT_EQ(err, I18nErrorCode::SUCCESS);
    ASSERT_TRUE(formatterWithContent != nullptr);
    result = formatterWithContent->Format(milliseconds);
    EXPECT_EQ(result, "year(1973)month(2)day(17)");

    pattern = "y''Md";
    std::unique_ptr<SimpleDateTimeFormat> formatterInvalidPattern =
        std::make_unique<SimpleDateTimeFormat>(pattern, localeTag, true, err);
    EXPECT_EQ(err, I18nErrorCode::INVALID_DATE_TIME_FORMAT_PATTERN);

    std::unique_ptr<SimpleDateTimeFormat> formatterWithEmptyPattern =
        std::make_unique<SimpleDateTimeFormat>("", localeTag, true, err);
    EXPECT_EQ(err, I18nErrorCode::INVALID_DATE_TIME_FORMAT_PATTERN);
}

/**
 * @tc.name: I18nFuncTest073
 * @tc.desc: Test I18n GetUnicodeWrappedFilePath
 * @tc.type: FUNC
 */
HWTEST_F(I18nTest, I18nFuncTest073, TestSize.Level1)
{
    std::string localeTag = "ar";
    std::string path = "data>log>faultlog";
    char delimiter = '>';
    std::string errorCode;
    std::string result = LocaleConfig::GetUnicodeWrappedFilePath(path, delimiter, localeTag, errorCode);
    EXPECT_EQ(result, "\u200f\u200edata\u200e\u200f>\u200elog\u200e\u200f>\u200efaultlog\u200e");
}

/**
 * @tc.name: I18nFuncTest074
 * @tc.desc: Test I18n calendar
 * @tc.type: FUNC
 */
HWTEST_F(I18nTest, I18nFuncTest074, TestSize.Level1)
{
    std::string systemLanguage = LocaleConfig::GetSystemLanguage();
    std::string systemLocale = LocaleConfig::GetSystemLocale();
    LocaleConfig::SetFirstDayOfWeek(WeekDay::THU);

    I18nCalendar defaultCalendar("zh-Hans-CN", CalendarType::CHINESE);
    int32_t firstDayOfWeek = defaultCalendar.GetFirstDayOfWeek();
    EXPECT_EQ(firstDayOfWeek, 1);
    defaultCalendar.SetFirstDayOfWeek(6);
    firstDayOfWeek = defaultCalendar.GetFirstDayOfWeek();
    EXPECT_EQ(firstDayOfWeek, 6);

    I18nCalendar satCalendar("zh-Hans-CN-u-fw-sat", CalendarType::CHINESE);
    firstDayOfWeek = satCalendar.GetFirstDayOfWeek();
    EXPECT_EQ(firstDayOfWeek, 7);
    satCalendar.SetFirstDayOfWeek(3);
    firstDayOfWeek = satCalendar.GetFirstDayOfWeek();
    EXPECT_EQ(firstDayOfWeek, 3);

    LocaleConfig::SetSystemLanguage(systemLanguage);
    LocaleConfig::SetSystemLocale(systemLocale);
}
} // namespace I18n
} // namespace Global
} // namespace OHOS