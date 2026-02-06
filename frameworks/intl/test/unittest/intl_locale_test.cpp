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
#include "intl_locale.h"

using namespace OHOS::Global::I18n;
using testing::ext::TestSize;
using namespace std;

namespace OHOS {
namespace Global {
namespace I18n {
class IntlLocaleTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void IntlLocaleTest::SetUpTestCase(void)
{
}

void IntlLocaleTest::TearDownTestCase(void)
{
}

void IntlLocaleTest::SetUp(void)
{
}

void IntlLocaleTest::TearDown(void)
{}

/**
 * @tc.name: IntlLocaleFuncTest001
 * @tc.desc: Test Intl Locale
 * @tc.type: FUNC
 */
HWTEST_F(IntlLocaleTest, IntlLocaleFuncTest001, TestSize.Level1)
{
    std::string localeTag = "zh-Hans-CN";
    std::unordered_map<std::string, std::string> configs = {};
    std::string errMessage;
    std::unique_ptr<IntlLocale> intlLocale = std::make_unique<IntlLocale>(localeTag, configs, errMessage);
    ASSERT_TRUE(intlLocale != nullptr);
    EXPECT_EQ(errMessage, "");
    std::string language = intlLocale->GetLanguage();
    std::string baseName = intlLocale->GetBaseName();
    std::string region = intlLocale->GetRegion();
    std::string script = intlLocale->GetScript();
    std::string calendar = intlLocale->GetCalendar();
    std::string collation = intlLocale->GetCollation();
    std::string hourCycle = intlLocale->GetHourCycle();
    std::string numberingSystem = intlLocale->GetNumberingSystem();
    std::string numeric = intlLocale->GetNumeric();
    std::string caseFirst = intlLocale->GetCaseFirst();
    std::string maximize = intlLocale->Maximize();
    std::string minimize = intlLocale->Minimize();
    std::string localeString = intlLocale->ToString(errMessage);
    EXPECT_EQ(language, "zh");
    EXPECT_EQ(baseName, "zh-Hans-CN");
    EXPECT_EQ(region, "CN");
    EXPECT_EQ(script, "Hans");
    EXPECT_EQ(calendar, "");
    EXPECT_EQ(collation, "");
    EXPECT_EQ(hourCycle, "");
    EXPECT_EQ(numberingSystem, "");
    EXPECT_EQ(numeric, "");
    EXPECT_EQ(caseFirst, "");
    EXPECT_EQ(maximize, "zh-Hans-CN");
    EXPECT_EQ(minimize, "zh");
    EXPECT_EQ(localeString, "zh-Hans-CN");
    EXPECT_EQ(errMessage, "");
}

/**
 * @tc.name: IntlLocaleFuncTest002
 * @tc.desc: Test Intl Locale
 * @tc.type: FUNC
 */
HWTEST_F(IntlLocaleTest, IntlLocaleFuncTest002, TestSize.Level1)
{
    std::string localeTag = "zh-Hans-CN";
    std::unordered_map<std::string, std::string> configs = {
        { "language", "en" },
        { "region", "US" },
        { "script", "Latn" },
        { "calendar", "buddhist" },
        { "collation", "compat" },
        { "hourCycle", "h11" },
        { "numberingSystem", "arab" },
        { "numeric", "true" },
        { "caseFirst", "upper" }
    };
    std::string errMessage;
    std::unique_ptr<IntlLocale> intlLocale = std::make_unique<IntlLocale>(localeTag, configs, errMessage);
    ASSERT_TRUE(intlLocale != nullptr);
    EXPECT_EQ(errMessage, "");
    std::string language = intlLocale->GetLanguage();
    std::string baseName = intlLocale->GetBaseName();
    std::string region = intlLocale->GetRegion();
    std::string script = intlLocale->GetScript();
    std::string calendar = intlLocale->GetCalendar();
    std::string collation = intlLocale->GetCollation();
    std::string hourCycle = intlLocale->GetHourCycle();
    std::string numberingSystem = intlLocale->GetNumberingSystem();
    std::string numeric = intlLocale->GetNumeric();
    std::string caseFirst = intlLocale->GetCaseFirst();
    std::string maximize = intlLocale->Maximize();
    std::string minimize = intlLocale->Minimize();
    std::string localeString = intlLocale->ToString(errMessage);
    EXPECT_EQ(language, "en");
    EXPECT_EQ(baseName, "en-Latn-US");
    EXPECT_EQ(region, "US");
    EXPECT_EQ(script, "Latn");
    EXPECT_EQ(calendar, "buddhist");
    EXPECT_EQ(collation, "compat");
    EXPECT_EQ(hourCycle, "h11");
    EXPECT_EQ(numberingSystem, "arab");
    EXPECT_EQ(numeric, "true");
    EXPECT_EQ(caseFirst, "upper");
    EXPECT_EQ(maximize, "en-Latn-US-u-ca-buddhist-co-compat-hc-h11-kf-upper-kn-nu-arab");
    EXPECT_EQ(minimize, "en-u-ca-buddhist-co-compat-hc-h11-kf-upper-kn-nu-arab");
    EXPECT_EQ(localeString, "en-Latn-US-u-ca-buddhist-co-compat-hc-h11-kf-upper-kn-nu-arab");
    EXPECT_EQ(errMessage, "");
}

/**
 * @tc.name: IntlLocaleFuncTest003
 * @tc.desc: Test Intl Locale
 * @tc.type: FUNC
 */
HWTEST_F(IntlLocaleTest, IntlLocaleFuncTest003, TestSize.Level1)
{
    std::string localeTag = "zh-Hans-CN";
    std::unordered_map<std::string, std::string> configs = {
        { "language", "zhzh" }
    };
    std::string errMessage;
    std::unique_ptr<IntlLocale> intlLocale = std::make_unique<IntlLocale>(localeTag, configs, errMessage);
    EXPECT_EQ(errMessage, "apply option to tag failed");
}

/**
 * @tc.name: IntlLocaleFuncTest004
 * @tc.desc: Test Intl Locale
 * @tc.type: FUNC
 */
HWTEST_F(IntlLocaleTest, IntlLocaleFuncTest004, TestSize.Level1)
{
    std::string localeTag = "zh-Hans-CN";
    std::unordered_map<std::string, std::string> configs = {
        { "script", "" }
    };
    std::string errMessage;
    std::unique_ptr<IntlLocale> intlLocale = std::make_unique<IntlLocale>(localeTag, configs, errMessage);
    EXPECT_EQ(errMessage, "apply option to tag failed");
}

/**
 * @tc.name: IntlLocaleFuncTest005
 * @tc.desc: Test Intl Locale
 * @tc.type: FUNC
 */
HWTEST_F(IntlLocaleTest, IntlLocaleFuncTest005, TestSize.Level1)
{
    std::string localeTag = "";
    std::unordered_map<std::string, std::string> configs = {
        { "language", "en" },
        { "region", "US" },
        { "script", "Latn" }
    };
    std::string errMessage;
    std::unique_ptr<IntlLocale> intlLocale = std::make_unique<IntlLocale>(localeTag, configs, errMessage);
    EXPECT_EQ(errMessage, "apply option to tag failed");
}

/**
 * @tc.name: IntlLocaleFuncTest006
 * @tc.desc: Test Intl Locale
 * @tc.type: FUNC
 */
HWTEST_F(IntlLocaleTest, IntlLocaleFuncTest006, TestSize.Level1)
{
    std::string localeTag = "zh-Hans-CN";
    std::unordered_map<std::string, std::string> configs = {
        { "hourCycle", "h10" }
    };
    std::string errMessage;
    std::unique_ptr<IntlLocale> intlLocale = std::make_unique<IntlLocale>(localeTag, configs, errMessage);
    EXPECT_EQ(errMessage, "Value out of range for locale options property");
}

/**
 * @tc.name: IntlLocaleFuncTest007
 * @tc.desc: Test Intl Locale
 * @tc.type: FUNC
 */
HWTEST_F(IntlLocaleTest, IntlLocaleFuncTest007, TestSize.Level1)
{
    std::string localeTag = "zh-Hans-CN";
    std::unordered_map<std::string, std::string> configs = {
        { "caseFirst", "fake" }
    };
    std::string errMessage;
    std::unique_ptr<IntlLocale> intlLocale = std::make_unique<IntlLocale>(localeTag, configs, errMessage);
    EXPECT_EQ(errMessage, "Value out of range for locale options property");
}
} // namespace I18n
} // namespace Global
} // namespace OHOS