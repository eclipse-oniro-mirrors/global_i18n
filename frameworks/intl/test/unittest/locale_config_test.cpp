/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "locale_config_test.h"
#include <gtest/gtest.h>
#include "locale_config.h"
#include "unicode/locid.h"
#include "parameter.h"

using namespace OHOS::Global::I18n;
using testing::ext::TestSize;
using namespace std;

namespace OHOS {
namespace Global {
namespace I18n {
class LocaleConfigTest : public testing::Test {
public:
    static string originalLanguage;
    static string originalRegion;
    static string originalLocale;
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();

    static std::vector<std::pair<std::string, std::string>> languageAndRegion;
    static std::vector<std::string> simplifiedLanguage;
};

string LocaleConfigTest::originalLanguage;
string LocaleConfigTest::originalRegion;
string LocaleConfigTest::originalLocale;

std::vector<std::pair<std::string, std::string>> LocaleConfigTest::languageAndRegion {
    { "zh-Hans", "CN" }, { "zh-Hant", "HK" }, { "zh-Hant", "TW" }, { "en-Latn-US", "US" }, { "en-Latn-US", "GB" },
    { "bo", "CN" }, { "ug", "CN" }, { "am", "CN" }, { "ar", "CN" }, { "as", "CN" }, { "az-Latn", "IR" },
    { "be", "CN" }, { "bg", "CN" }, { "bn", "CN" }, { "bs-Latn", "CN" }, { "ca", "CN" }, { "cs", "CN" },
    { "da", "CN" }, { "de", "CN" }, { "el", "CN" }, { "es-ES", "US" }, { "es-US", "ES" }, { "et", "CN" },
    { "eu", "CN" }, { "fa", "IR" }, { "fi", "CN" }, { "tl", "CN" }, { "fr", "CN" },  { "jv-Latn", "CN" },
    { "mai", "CN" }, { "mn-Cyrl", "CN" }, { "uz-Latn", "CN" }
};

std::vector<std::string> LocaleConfigTest::simplifiedLanguage {
    "zh-Hans", "zh-Hant-HK", "zh-Hant", "en", "en-GB", "bo", "ug", "am", "ar", "as", "az-Latn", "be", "bg", "bn",
    "bs-Latn", "ca", "cs", "da", "de", "el", "es", "es-US", "et", "eu", "fa", "fi", "tl", "fr", "jv-Latn", "mai",
    "mn-Cyrl", "uz-Latn"
};

void LocaleConfigTest::SetUpTestCase(void)
{
    originalLanguage = LocaleConfig::GetSystemLanguage();
    originalRegion = LocaleConfig::GetSystemRegion();
    originalLocale = LocaleConfig::GetSystemLocale();
    LocaleConfig::SetSystemLanguage("zh-Hans");
    LocaleConfig::SetSystemRegion("CN");
    LocaleConfig::SetSystemLocale("zh-Hans-CN");
}

void LocaleConfigTest::TearDownTestCase(void)
{
    LocaleConfig::SetSystemLanguage(originalLanguage);
    LocaleConfig::SetSystemRegion(originalRegion);
    LocaleConfig::SetSystemLocale(originalLocale);
}

void LocaleConfigTest::SetUp(void)
{}

void LocaleConfigTest::TearDown(void)
{}

/**
 * @tc.name: LocaleConfigFuncTest001
 * @tc.desc: Test LocaleConfig GetSystemLanguage default
 * @tc.type: FUNC
 */
HWTEST_F(LocaleConfigTest, LocaleConfigFuncTest001, TestSize.Level1)
{
    int ret = SetParameter("hm.sys.language", "");
    if (!ret) {
        EXPECT_EQ(LocaleConfig::GetSystemLanguage(), "zh-Hans");
    }
}

/**
 * @tc.name: LocaleConfigFuncTest002
 * @tc.desc: Test LocaleConfig GetSystemRegion default.
 * @tc.type: FUNC
 */
HWTEST_F(LocaleConfigTest, LocaleConfigFuncTest002, TestSize.Level1)
{
    int ret = SetParameter("hm.sys.locale", "");
    if (!ret) {
        EXPECT_EQ(LocaleConfig::GetSystemRegion(), "CN");
    }
}

/**
 * @tc.name: LocaleConfigFuncTest003
 * @tc.desc: Test LocaleConfig GetSystemLocale default
 * @tc.type: FUNC
 */
HWTEST_F(LocaleConfigTest, LocaleConfigFuncTest003, TestSize.Level1)
{
    int ret = SetParameter("hm.sys.locale", "");
    if (!ret) {
        EXPECT_EQ(LocaleConfig::GetSystemLocale(), "zh-Hans-CN");
    }
}

/**
 * @tc.name: LocaleConfigFuncTest004
 * @tc.desc: Test LocaleConfig SetSystemLanguage
 * @tc.type: FUNC
 */
HWTEST_F(LocaleConfigTest, LocaleConfigFuncTest004, TestSize.Level1)
{
    string language = "pt-PT";
    I18nErrorCode status = LocaleConfig::SetSystemLanguage(language);
    EXPECT_EQ(status, I18nErrorCode::SUCCESS);
    LocaleConfig::SetSystemLanguage("zh-Hans");
    EXPECT_EQ(status, I18nErrorCode::SUCCESS);
    LocaleConfig::SetSystemLanguage("%$=");
}

/**
 * @tc.name: LocaleConfigFuncTest005
 * @tc.desc: Test LocaleConfig SetSystemLocale
 * @tc.type: FUNC
 */
HWTEST_F(LocaleConfigTest, LocaleConfigFuncTest005, TestSize.Level1)
{
    string locale = "zh-Hant-TW";
    I18nErrorCode status = LocaleConfig::SetSystemLocale(locale);
    EXPECT_EQ(status, I18nErrorCode::SUCCESS);
    status = LocaleConfig::SetSystemLocale("zh-Hans-CN");
    EXPECT_EQ(status, I18nErrorCode::SUCCESS);
}

/**
 * @tc.name: LocaleConfigFuncTest006
 * @tc.desc: Test LocaleConfig SetSystemRegion
 * @tc.type: FUNC
 */
HWTEST_F(LocaleConfigTest, LocaleConfigFuncTest006, TestSize.Level1)
{
    string locale = "zh-Hant-TW";
    I18nErrorCode status = LocaleConfig::SetSystemLocale(locale);
    EXPECT_EQ(status, I18nErrorCode::SUCCESS);
    status = LocaleConfig::SetSystemRegion("HK");
    EXPECT_EQ(status, I18nErrorCode::SUCCESS);
    status = LocaleConfig::SetSystemLocale("zh-Hans-CN");
    EXPECT_EQ(status, I18nErrorCode::SUCCESS);
    status = LocaleConfig::SetSystemRegion("@*%");
    EXPECT_EQ(status, I18nErrorCode::INVALID_REGION_TAG);
    status = LocaleConfig::SetSystemLocale("zh-Hans-CN", 2);
    EXPECT_NE(status, I18nErrorCode::SUCCESS);
    status = LocaleConfig::SetSystemLocale("zh-Hans-CN", 2);
    EXPECT_NE(status, I18nErrorCode::SUCCESS);
    status = LocaleConfig::SetSystemLocale("ar-EG");
    EXPECT_EQ(status, I18nErrorCode::SUCCESS);
    EXPECT_FALSE(LocaleConfig::GetUsingLocalDigit());
    status = LocaleConfig::SetSystemLocale("$$@");
    EXPECT_NE(status, I18nErrorCode::SUCCESS);
    status = LocaleConfig::SetSystemLocale("zh-Hans-CN");
    EXPECT_EQ(status, I18nErrorCode::SUCCESS);
}

/**
 * @tc.name: LocaleConfigFuncTest007
 * @tc.desc: Test LocaleConfig GetSystemCountries
 * @tc.type: FUNC
 */
HWTEST_F(LocaleConfigTest, LocaleConfigFuncTest007, TestSize.Level1)
{
    std::unordered_set<std::string> countries = LocaleConfig::GetSystemCountries("zh");
    EXPECT_TRUE(countries.size() > 200);
}

/**
 * @tc.name: LocaleConfigFuncTest008
 * @tc.desc: Test LocaleConfig GetDisplayLanguage
 * @tc.type: FUNC
 */
HWTEST_F(LocaleConfigTest, LocaleConfigFuncTest008, TestSize.Level1)
{
    std::string languageTag = "pt";
    std::string localeTag = "en-US";
    std::string displayName = LocaleConfig::GetDisplayLanguage(languageTag, localeTag, true);
    EXPECT_EQ(displayName, "Portuguese");

    displayName = LocaleConfig::GetDisplayLanguage(languageTag, localeTag, false);
    EXPECT_EQ(displayName, "Portuguese");

    std::string fakeLocaleTag = "FakeLocaleTag";
    displayName = LocaleConfig::GetDisplayLanguage(languageTag, fakeLocaleTag, true);
    EXPECT_EQ(displayName, "");

    std::string fakeLanguageTag = "FakeLanguageTag";
    displayName = LocaleConfig::GetDisplayLanguage(fakeLanguageTag, localeTag, true);
    EXPECT_EQ(displayName, "");

    languageTag = "zh-Hans-CN";
    localeTag = "en-US";
    displayName = LocaleConfig::GetDisplayLanguage(languageTag, localeTag, false);
    EXPECT_EQ(displayName, "Simplified Chinese");

    displayName = LocaleConfig::GetDisplayLanguage(languageTag, localeTag, true);
    EXPECT_EQ(displayName, "Simplified Chinese");

    displayName = LocaleConfig::GetDisplayLanguage(languageTag, fakeLocaleTag, false);
    EXPECT_EQ(displayName, "");

    languageTag = "pt-Latn-BR";
    displayName = LocaleConfig::GetDisplayLanguage(languageTag, localeTag, false);
    EXPECT_EQ(displayName, "Portuguese (Brazil)");

    displayName = LocaleConfig::GetDisplayLanguage(languageTag, fakeLocaleTag, false);
    EXPECT_EQ(displayName, "");

    languageTag = "zh";
    displayName = LocaleConfig::GetDisplayLanguage(languageTag, localeTag, true);
    EXPECT_EQ(displayName, "Chinese");

    localeTag = "en-Hans";
    displayName = LocaleConfig::GetDisplayLanguage(languageTag, localeTag, true);
    EXPECT_EQ(displayName, "Chinese");

    languageTag = "zh-Hans";
    localeTag = "en-US";
    displayName = LocaleConfig::GetDisplayLanguage(languageTag, localeTag, true);
    EXPECT_EQ(displayName, "Simplified Chinese");

    languageTag = "ro-MD";
    displayName = LocaleConfig::GetDisplayLanguage(languageTag, localeTag, true);
    EXPECT_EQ(displayName, "Moldavian");
}

/**
 * @tc.name: LocaleConfigFuncTest009
 * @tc.desc: Test LocaleConfig GetDisplayRegion
 * @tc.type: FUNC
 */
HWTEST_F(LocaleConfigTest, LocaleConfigFuncTest009, TestSize.Level1)
{
    std::string regionTag = "JP";
    std::string localeTag = "zh-Hans-CN";
    std::string displayName = LocaleConfig::GetDisplayRegion(regionTag, localeTag, false);
    EXPECT_EQ(displayName, "日本");

    std::string fakeRegionTag = "XX";
    displayName = LocaleConfig::GetDisplayRegion(fakeRegionTag, localeTag, false);
    EXPECT_EQ(displayName, "XX");

    std::string fakeLocaleTag = "FakeLocaleTag";
    displayName = LocaleConfig::GetDisplayRegion(regionTag, fakeLocaleTag, false);
    EXPECT_EQ(displayName, "");

    localeTag = "en-Latn-US";
    displayName = LocaleConfig::GetDisplayRegion(regionTag, localeTag, false);
    EXPECT_EQ(displayName, "Japan");

    regionTag = "zh-Hans-CN";
    displayName = LocaleConfig::GetDisplayRegion(regionTag, localeTag, false);
    EXPECT_EQ(displayName, "China");

    regionTag = "HK";
    displayName = LocaleConfig::GetDisplayRegion(regionTag, localeTag, false);
    EXPECT_EQ(displayName, "Hong Kong (China)");
}

/**
 * @tc.name: LocaleConfigFuncTest010
 * @tc.desc: Test LocaleConfig GetDisplayLanguage
 * @tc.type: FUNC
 */
HWTEST_F(LocaleConfigTest, LocaleConfigFuncTest010, TestSize.Level1)
{
    std::string languageTag = "zh-Hant-HK";
    std::string localeTag = "zh-Hant";
    std::string displayName = LocaleConfig::GetDisplayLanguage(languageTag, localeTag, true);
    EXPECT_EQ(displayName, "繁體中文（中國香港）");

    languageTag = "zh-Hant-HK";
    localeTag = "zh-Hans";
    displayName = LocaleConfig::GetDisplayLanguage(languageTag, localeTag, true);
    EXPECT_EQ(displayName, "繁体中文（中国香港）");

    languageTag = "zh-Hant-HK";
    localeTag = "en-US";
    displayName = LocaleConfig::GetDisplayLanguage(languageTag, localeTag, true);
    EXPECT_EQ(displayName, "Traditional Chinese(Hong Kong, China)");
}

/**
 * @tc.name: LocaleConfigFuncTest011
 * @tc.desc: Test LocaleConfig GetDisplayRegion
 * @tc.type: FUNC
 */
HWTEST_F(LocaleConfigTest, LocaleConfigFuncTest011, TestSize.Level1)
{
    EXPECT_EQ(LocaleConfig::GetDisplayRegion("zh", "en-US", true), "");
    EXPECT_EQ(LocaleConfig::GetDisplayRegion("zh-Hans-CN", "en-US", true), "China");
}

/**
 * @tc.name: LocaleConfigFuncTest012
 * @tc.desc: Test LocaleConfig GetDisplayRegion
 * @tc.type: FUNC
 */
HWTEST_F(LocaleConfigTest, LocaleConfigFuncTest012, TestSize.Level1)
{
    EXPECT_EQ(LocaleConfig::GetDisplayRegion("zh-Hans", "en-US", true), "");
}

/**
 * @tc.name: LocaleConfigFuncTest013
 * @tc.desc: Test LocaleConfig LocalDigit
 * @tc.type: FUNC
 */
HWTEST_F(LocaleConfigTest, LocaleConfigFuncTest013, TestSize.Level1)
{
    bool current = LocaleConfig::GetUsingLocalDigit();
    I18nErrorCode status = LocaleConfig::SetUsingLocalDigit(true);
    EXPECT_EQ(status, I18nErrorCode::UPDATE_LOCAL_DIGIT_FAILED);
    current = LocaleConfig::GetUsingLocalDigit();
    EXPECT_FALSE(current);
}

/**
 * @tc.name: LocaleConfigFuncTest014
 * @tc.desc: Test LocaleConfig 24 hour clock
 * @tc.type: FUNC
 */
HWTEST_F(LocaleConfigTest, LocaleConfigFuncTest014, TestSize.Level1)
{
    bool current = LocaleConfig::Is24HourClock();
    I18nErrorCode status = LocaleConfig::Set24HourClock("true");
    EXPECT_EQ(status, I18nErrorCode::SUCCESS);
    current = LocaleConfig::Is24HourClock();
    EXPECT_TRUE(current);
    status = LocaleConfig::Set24HourClock("abc");
    EXPECT_EQ(status, I18nErrorCode::INVALID_24_HOUR_CLOCK_TAG);
    LocaleConfig::Set24HourClock("");
    LocaleConfig::Set24HourClock("false");
}

/**
 * @tc.name: LocaleConfigFuncTest015
 * @tc.desc: Test LocaleConfig valid locales
 * @tc.type: FUNC
 */
HWTEST_F(LocaleConfigTest, LocaleConfigFuncTest015, TestSize.Level1)
{
    string localeTag = "zh";
    string validLocaleTag = LocaleConfig::GetValidLocale(localeTag);
    EXPECT_EQ(validLocaleTag, "zh");

    localeTag = "zh-u-hc-h12-nu-latn-ca-chinese-co-pinyin-kf-upper-kn-true";
    validLocaleTag = LocaleConfig::GetValidLocale(localeTag);
    EXPECT_EQ(validLocaleTag, "zh-u-ca-chinese-co-pinyin-kn-true-kf-upper-nu-latn-hc-h12");
}

/**
 * @tc.name: LocaleConfigFuncTest016
 * @tc.desc: Test LocaleConfig isRTL
 * @tc.type: FUNC
 */
HWTEST_F(LocaleConfigTest, LocaleConfigFuncTest016, TestSize.Level1)
{
    string localeTag = "zh";
    bool isRTL = LocaleConfig::IsRTL(localeTag);
    EXPECT_TRUE(!isRTL);
    localeTag = "ar";
    isRTL = LocaleConfig::IsRTL(localeTag);
    EXPECT_TRUE(isRTL);
}

/**
 * @tc.name: LocaleConfigFuncTest017
 * @tc.desc: Test LocaleConfig isSuggested
 * @tc.type: FUNC
 */
HWTEST_F(LocaleConfigTest, LocaleConfigFuncTest017, TestSize.Level1)
{
    string language = "zh";
    bool isSuggested = LocaleConfig::IsSuggested(language);
    EXPECT_TRUE(isSuggested);
    string region = "CN";
    isSuggested = LocaleConfig::IsSuggested(language, region);
    EXPECT_TRUE(isSuggested);
    std::string fakeLanguage = "FakeLanguage";
    isSuggested = LocaleConfig::IsSuggested(fakeLanguage);
    EXPECT_FALSE(isSuggested);
}

/**
 * @tc.name: LocaleConfigFuncTest018
 * @tc.desc: Test LocaleCOnfig systemRegion
 * @tc.type: FUNC
 */
HWTEST_F(LocaleConfigTest, LocaleConfigFuncTest018, TestSize.Level1)
{
    string currentRegion = LocaleConfig::GetSystemRegion();
    EXPECT_TRUE(currentRegion.length() > 0);
    string region = "CN";
    I18nErrorCode status = LocaleConfig::SetSystemRegion(region);
    EXPECT_EQ(status, I18nErrorCode::SUCCESS);
    currentRegion = LocaleConfig::GetSystemRegion();
    EXPECT_EQ(currentRegion, region);
}

/**
 * @tc.name: LocaleConfigFuncTest019
 * @tc.desc: Test LocaleCOnfig systemLocale
 * @tc.type: FUNC
 */
HWTEST_F(LocaleConfigTest, LocaleConfigFuncTest019, TestSize.Level1)
{
    string currentLocale = LocaleConfig::GetSystemLocale();
    EXPECT_TRUE(currentLocale.length() > 0);
    string locale = "zh-Hans-CN";
    I18nErrorCode status = LocaleConfig::SetSystemLocale(locale);
    EXPECT_EQ(status, I18nErrorCode::SUCCESS);
    currentLocale = LocaleConfig::GetSystemLocale();
    EXPECT_EQ(currentLocale, locale);
}

/**
 * @tc.name: LocaleConfigFuncTest020
 * @tc.desc: Test LocaleConfig systemLanguages
 * @tc.type: FUNC
 */
HWTEST_F(LocaleConfigTest, LocaleConfigFuncTest020, TestSize.Level1)
{
    std::unordered_set<std::string> languages = LocaleConfig::GetSystemLanguages();
    EXPECT_TRUE(languages.size() > 2);
    LocaleConfig::SetSystemLocale("ur-Latn-PK");
    LocaleConfig::SetUsingLocalDigit(true);
    LocaleConfig::SetUsingLocalDigit(false);
}

/**
 * @tc.name: LocaleConfigFuncTest021
 * @tc.desc: Test LocaleConfig IsValidLanguage
 * @tc.type: FUNC
 */
HWTEST_F(LocaleConfigTest, LocaleConfigFuncTest021, TestSize.Level1)
{
    const string language = "zh";
    const string fakeLanguage = "010";
    const string tag = "zh-Hans";
    const string fakeTag = "13-Hans";
    bool flag = LocaleConfig::IsValidLanguage(language);
    EXPECT_TRUE(flag);
    flag = LocaleConfig::IsValidLanguage(fakeLanguage);
    EXPECT_TRUE(!flag);
    flag = LocaleConfig::IsValidTag(tag);
    EXPECT_TRUE(flag);
    flag = LocaleConfig::IsValidTag(fakeTag);
    EXPECT_TRUE(!flag);
    EXPECT_FALSE(LocaleConfig::IsValidTag(""));
}

/**
 * @tc.name: LocaleConfigFuncTest022
 * @tc.desc: Test LocaleConfig GetSimplifiedSystemLanguage
 * @tc.type: FUNC
 */
HWTEST_F(LocaleConfigTest, LocaleConfigFuncTest022, TestSize.Level1)
{
    for (auto index = 0; index < LocaleConfigTest::languageAndRegion.size(); ++index) {
        LocaleConfig::SetSystemLanguage(languageAndRegion[index].first);
        LocaleConfig::SetSystemRegion(languageAndRegion[index].second);
        std::string ret = LocaleConfig::GetSimplifiedSystemLanguage();
        EXPECT_EQ(ret, LocaleConfigTest::simplifiedLanguage[index]);
    }
}
} // namespace I18n
} // namespace Global
} // namespace OHOS