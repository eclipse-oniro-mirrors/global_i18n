/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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
#include "accesstoken_kit.h"
#include "i18n_service_ability_client.h"
#include "locale_config.h"
#include "nativetoken_kit.h"
#include "preferred_language.h"
#include "token_setproc.h"
#include "i18n_service_test.h"

using testing::ext::TestSize;
using namespace std;
namespace OHOS {
namespace Global {
namespace I18n {
static const uint64_t SELF_TOKEN_ID = GetSelfTokenID();
static constexpr int32_t I18N_UID = 3013;
static constexpr int32_t ROOT_UID = 0;

class I18nServiceTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void I18nServiceTest::SetUpTestCase(void)
{}

void I18nServiceTest::TearDownTestCase(void)
{}

void I18nServiceTest::SetUp(void)
{}

void I18nServiceTest::TearDown(void)
{}

void RemoveNativeTokenTypeAndPermissions()
{
    SetSelfTokenID(SELF_TOKEN_ID);
    seteuid(ROOT_UID);
    OHOS::Security::AccessToken::AccessTokenKit::ReloadNativeTokenInfo();
    seteuid(I18N_UID);
}

void AddNativeTokenTypeAndPermissions()
{
    const char* permissions[] = {
        "ohos.permission.UPDATE_CONFIGURATION"
    };
    NativeTokenInfoParams infoInstance = {
        .dcapsNum = 0,
        .permsNum = sizeof(permissions) / sizeof(permissions[0]),
        .aclsNum = 0,
        .dcaps = nullptr,
        .perms = permissions,
        .acls = nullptr,
        .aplStr = "system_basic",
    };
    infoInstance.processName = "I18nServiceTest";
    SetSelfTokenID(GetAccessTokenId(&infoInstance));
    seteuid(ROOT_UID);
    int32_t ret = OHOS::Security::AccessToken::AccessTokenKit::ReloadNativeTokenInfo();
    printf("ReloadNativeTokenInfo result is %d. \n", ret);
    seteuid(I18N_UID);
}

void InitTestEnvironment()
{
    LocaleConfig::SetSystemLanguage("zh-Hans");
    LocaleConfig::SetSystemRegion("CN");
    LocaleConfig::SetSystemLocale("zh-Hans-CN");
    LocaleConfig::Set24HourClock("true");
    std::vector<std::string> preferredLanguageList = PreferredLanguage::GetPreferredLanguageList();
    if (preferredLanguageList.size() > 1) {
        for (size_t i = preferredLanguageList.size(); i > 1; --i) {
            PreferredLanguage::RemovePreferredLanguage(i);
        }
    }
    RemoveNativeTokenTypeAndPermissions();
}

/**
 * @tc.name: I18nServiceFuncTest001
 * @tc.desc: Test I18nServiceAbilityClient SetSystemLanguage: no permission case
 * @tc.type: FUNC
 */
HWTEST_F(I18nServiceTest, I18nServiceFuncTest001, TestSize.Level1)
{
    InitTestEnvironment();
    I18nErrorCode err = I18nServiceAbilityClient::SetSystemLanguage("en");
    EXPECT_EQ(err, I18nErrorCode::NO_PERMISSION);
}

/**
 * @tc.name: I18nServiceFuncTest002
 * @tc.desc: Test I18nServiceAbilityClient SetSystemLanguage
 * @tc.type: FUNC
 */
HWTEST_F(I18nServiceTest, I18nServiceFuncTest002, TestSize.Level1)
{
    InitTestEnvironment();
    AddNativeTokenTypeAndPermissions();
    I18nErrorCode err = I18nServiceAbilityClient::SetSystemLanguage("en");
    EXPECT_EQ(err, I18nErrorCode::SUCCESS);
}

/**
 * @tc.name: I18nServiceFuncTest003
 * @tc.desc: Test I18nServiceAbilityClient SetSystemLanguage: invalid language tag
 * @tc.type: FUNC
 */
HWTEST_F(I18nServiceTest, I18nServiceFuncTest003, TestSize.Level1)
{
    InitTestEnvironment();
    AddNativeTokenTypeAndPermissions();
    I18nErrorCode err = I18nServiceAbilityClient::SetSystemLanguage("fake_language_tag");
    EXPECT_EQ(err, I18nErrorCode::INVALID_LANGUAGE_TAG);
}

/**
 * @tc.name: I18nServiceFuncTest004
 * @tc.desc: Test I18nServiceAbilityClient SetSystemRegion: no permission case.
 * @tc.type: FUNC
 */
HWTEST_F(I18nServiceTest, I18nServiceFuncTest004, TestSize.Level1)
{
    InitTestEnvironment();
    I18nErrorCode err = I18nServiceAbilityClient::SetSystemRegion("US");
    EXPECT_EQ(err, I18nErrorCode::NO_PERMISSION);
}

/**
 * @tc.name: I18nServiceFuncTest005
 * @tc.desc: Test I18nServiceAbilityClient SetSystemRegion
 * @tc.type: FUNC
 */
HWTEST_F(I18nServiceTest, I18nServiceFuncTest005, TestSize.Level1)
{
    InitTestEnvironment();
    AddNativeTokenTypeAndPermissions();
    I18nErrorCode err = I18nServiceAbilityClient::SetSystemRegion("US");
    EXPECT_EQ(err, I18nErrorCode::SUCCESS);
}

/**
 * @tc.name: I18nServiceFuncTest006
 * @tc.desc: Test I18nServiceAbilityClient SetSystemRegion: invalid region tag.
 * @tc.type: FUNC
 */
HWTEST_F(I18nServiceTest, I18nServiceFuncTest006, TestSize.Level1)
{
    InitTestEnvironment();
    AddNativeTokenTypeAndPermissions();
    I18nErrorCode err = I18nServiceAbilityClient::SetSystemRegion("fake_region_tag");
    EXPECT_EQ(err, I18nErrorCode::INVALID_REGION_TAG);
}

/**
 * @tc.name: I18nServiceFuncTest007
 * @tc.desc: Test I18nServiceAbilityClient SetSystemLocale: no permission case.
 * @tc.type: FUNC
 */
HWTEST_F(I18nServiceTest, I18nServiceFuncTest007, TestSize.Level1)
{
    InitTestEnvironment();
    I18nErrorCode err = I18nServiceAbilityClient::SetSystemLocale("en-US");
    EXPECT_EQ(err, I18nErrorCode::NO_PERMISSION);
}

/**
 * @tc.name: I18nServiceFuncTest008
 * @tc.desc: Test I18nServiceAbilityClient SetSystemLocale
 * @tc.type: FUNC
 */
HWTEST_F(I18nServiceTest, I18nServiceFuncTest008, TestSize.Level1)
{
    InitTestEnvironment();
    AddNativeTokenTypeAndPermissions();
    I18nErrorCode err = I18nServiceAbilityClient::SetSystemLocale("en-US");
    EXPECT_EQ(err, I18nErrorCode::SUCCESS);
    err = I18nServiceAbilityClient::SetSystemLocale("en-XA");
    LocaleConfig::GetDisplayLanguage("zh", "en-US", true);
}

/**
 * @tc.name: I18nServiceFuncTest009
 * @tc.desc: Test I18nServiceAbilityClient SetSystemLocale: invalid locale tag.
 * @tc.type: FUNC
 */
HWTEST_F(I18nServiceTest, I18nServiceFuncTest009, TestSize.Level1)
{
    InitTestEnvironment();
    AddNativeTokenTypeAndPermissions();
    I18nErrorCode err = I18nServiceAbilityClient::SetSystemRegion("fake_locale_tag");
    EXPECT_EQ(err, I18nErrorCode::INVALID_REGION_TAG);
}

/**
 * @tc.name: I18nServiceFuncTest010
 * @tc.desc: Test I18nServiceAbilityClient Set24HourClock: no permission.
 * @tc.type: FUNC
 */
HWTEST_F(I18nServiceTest, I18nServiceFuncTest010, TestSize.Level1)
{
    InitTestEnvironment();
    I18nErrorCode err = I18nServiceAbilityClient::Set24HourClock("false");
    EXPECT_EQ(err, I18nErrorCode::NO_PERMISSION);
}

/**
 * @tc.name: I18nServiceFuncTest011
 * @tc.desc: Test I18nServiceAbilityClient Set24HourClock: set 24 hour clock to false.
 * @tc.type: FUNC
 */
HWTEST_F(I18nServiceTest, I18nServiceFuncTest011, TestSize.Level1)
{
    InitTestEnvironment();
    AddNativeTokenTypeAndPermissions();
    I18nErrorCode err = I18nServiceAbilityClient::Set24HourClock("false");
    EXPECT_EQ(err, I18nErrorCode::SUCCESS);
}

/**
 * @tc.name: I18nServiceFuncTest012
 * @tc.desc: Test I18nServiceAbilityClient Set24HourClock: set 24 hour clock to true.
 * @tc.type: FUNC
 */
HWTEST_F(I18nServiceTest, I18nServiceFuncTest012, TestSize.Level1)
{
    InitTestEnvironment();
    AddNativeTokenTypeAndPermissions();
    I18nErrorCode err = I18nServiceAbilityClient::Set24HourClock("true");
    EXPECT_EQ(err, I18nErrorCode::SUCCESS);
}

/**
 * @tc.name: I18nServiceFuncTest013
 * @tc.desc: Test I18nServiceAbilityClient SetUsingLocalDigit: test no permission case.
 * @tc.type: FUNC
 */
HWTEST_F(I18nServiceTest, I18nServiceFuncTest013, TestSize.Level1)
{
    InitTestEnvironment();
    I18nErrorCode err = I18nServiceAbilityClient::SetUsingLocalDigit(true);
    EXPECT_EQ(err, I18nErrorCode::NO_PERMISSION);
}

/**
 * @tc.name: I18nServiceFuncTest014
 * @tc.desc: Test I18nServiceAbilityClient SetUsingLocalDigit: test no local digit language.
 * @tc.type: FUNC
 */
HWTEST_F(I18nServiceTest, I18nServiceFuncTest014, TestSize.Level1)
{
    InitTestEnvironment();
    AddNativeTokenTypeAndPermissions();
    I18nErrorCode err = I18nServiceAbilityClient::SetUsingLocalDigit(true);
    EXPECT_EQ(err, I18nErrorCode::UPDATE_LOCAL_DIGIT_FAILED);
}

/**
 * @tc.name: I18nServiceFuncTest015
 * @tc.desc: Test I18nServiceAbilityClient SetUsingLocalDigit test set local digit to true
 * @tc.type: FUNC
 */
HWTEST_F(I18nServiceTest, I18nServiceFuncTest015, TestSize.Level1)
{
    InitTestEnvironment();
    AddNativeTokenTypeAndPermissions();
    I18nErrorCode err = I18nServiceAbilityClient::SetSystemLanguage("ar");
    EXPECT_EQ(err, I18nErrorCode::SUCCESS);
    err = I18nServiceAbilityClient::SetUsingLocalDigit(true);
    EXPECT_EQ(err, I18nErrorCode::SUCCESS);
}

/**
 * @tc.name: I18nServiceFuncTest016
 * @tc.desc: Test I18nServiceAbilityClient SetUsingLocalDigit test set local digit to false
 * @tc.type: FUNC
 */
HWTEST_F(I18nServiceTest, I18nServiceFuncTest016, TestSize.Level1)
{
    InitTestEnvironment();
    AddNativeTokenTypeAndPermissions();
    I18nErrorCode err = I18nServiceAbilityClient::SetSystemLanguage("ar");
    EXPECT_EQ(err, I18nErrorCode::SUCCESS);
    err = I18nServiceAbilityClient::SetUsingLocalDigit(false);
    EXPECT_EQ(err, I18nErrorCode::SUCCESS);
}

/**
 * @tc.name: I18nServiceFuncTest017
 * @tc.desc: Test I18nServiceAbilityClient AddPreferredLanguage: test no permission case
 * @tc.type: FUNC
 */
HWTEST_F(I18nServiceTest, I18nServiceFuncTest017, TestSize.Level1)
{
    InitTestEnvironment();
    I18nErrorCode err = I18nServiceAbilityClient::AddPreferredLanguage("ar", 0);
    EXPECT_EQ(err, I18nErrorCode::NO_PERMISSION);
}

/**
 * @tc.name: I18nServiceFuncTest018
 * @tc.desc: Test I18nServiceAbilityClient AddPreferredLanguage: test add language to preferred language list header.
 * @tc.type: FUNC
 */
HWTEST_F(I18nServiceTest, I18nServiceFuncTest018, TestSize.Level1)
{
    InitTestEnvironment();
    AddNativeTokenTypeAndPermissions();
    I18nErrorCode err = I18nServiceAbilityClient::AddPreferredLanguage("ar", 0);
    EXPECT_EQ(err, I18nErrorCode::SUCCESS);
}

/**
 * @tc.name: I18nServiceFuncTest019
 * @tc.desc: Test I18nServiceAbilityClient AddPreferredLanguage: test add language to preferred language list tailor.
 * @tc.type: FUNC
 */
HWTEST_F(I18nServiceTest, I18nServiceFuncTest019, TestSize.Level1)
{
    InitTestEnvironment();
    AddNativeTokenTypeAndPermissions();
    I18nErrorCode err = I18nServiceAbilityClient::AddPreferredLanguage("fr", 1);
    EXPECT_EQ(err, I18nErrorCode::SUCCESS);
}

/**
 * @tc.name: I18nServiceFuncTest020
 * @tc.desc: Test I18nServiceAbilityClient AddPreferredLanguage: test add language to negative index.
 * @tc.type: FUNC
 */
HWTEST_F(I18nServiceTest, I18nServiceFuncTest020, TestSize.Level1)
{
    InitTestEnvironment();
    AddNativeTokenTypeAndPermissions();
    I18nErrorCode err = I18nServiceAbilityClient::AddPreferredLanguage("fr", -10);
    EXPECT_EQ(err, I18nErrorCode::SUCCESS);
}

/**
 * @tc.name: I18nServiceFuncTest021
 * @tc.desc: Test I18nServiceAbilityClient AddPreferredLanguage: test add language to large index.
 * @tc.type: FUNC
 */
HWTEST_F(I18nServiceTest, I18nServiceFuncTest021, TestSize.Level1)
{
    InitTestEnvironment();
    AddNativeTokenTypeAndPermissions();
    I18nErrorCode err = I18nServiceAbilityClient::AddPreferredLanguage("fr", 10);
    EXPECT_EQ(err, I18nErrorCode::SUCCESS);
}

/**
 * @tc.name: I18nServiceFuncTest022
 * @tc.desc: Test I18nServiceAbilityClient AddPreferredLanguage: test add invalid language tag.
 * @tc.type: FUNC
 */
HWTEST_F(I18nServiceTest, I18nServiceFuncTest022, TestSize.Level1)
{
    InitTestEnvironment();
    AddNativeTokenTypeAndPermissions();
    I18nErrorCode err = I18nServiceAbilityClient::AddPreferredLanguage("fr", 1);
    EXPECT_EQ(err, I18nErrorCode::SUCCESS);
}

/**
 * @tc.name: I18nServiceFuncTest023
 * @tc.desc: Test I18nServiceAbilityClient RemovePreferredLanguage: test no permission case.
 * @tc.type: FUNC
 */
HWTEST_F(I18nServiceTest, I18nServiceFuncTest023, TestSize.Level1)
{
    InitTestEnvironment();
    I18nErrorCode err = I18nServiceAbilityClient::AddPreferredLanguage("fr", -1);
    EXPECT_EQ(err, I18nErrorCode::NO_PERMISSION);
}

/**
 * @tc.name: I18nServiceFuncTest024
 * @tc.desc: Test I18nServiceAbilityClient RemovePreferredLanguage: remove when preferred language list size is 1.
 * @tc.type: FUNC
 */
HWTEST_F(I18nServiceTest, I18nServiceFuncTest024, TestSize.Level1)
{
    InitTestEnvironment();
    AddNativeTokenTypeAndPermissions();
    I18nErrorCode err = I18nServiceAbilityClient::RemovePreferredLanguage(0);
    EXPECT_EQ(err, I18nErrorCode::REMOVE_PREFERRED_LANGUAGE_FAILED);
}

/**
 * @tc.name: I18nServiceFuncTest025
 * @tc.desc: Test I18nServiceAbilityClient RemovePreferredLanguage: remove large index.
 * @tc.type: FUNC
 */
HWTEST_F(I18nServiceTest, I18nServiceFuncTest025, TestSize.Level1)
{
    InitTestEnvironment();
    AddNativeTokenTypeAndPermissions();
    I18nErrorCode err = I18nServiceAbilityClient::AddPreferredLanguage("fr", 0);
    EXPECT_EQ(err, I18nErrorCode::SUCCESS);
    err = I18nServiceAbilityClient::RemovePreferredLanguage(10);
    EXPECT_EQ(err, I18nErrorCode::SUCCESS);
}

/**
 * @tc.name: I18nServiceFuncTest026
 * @tc.desc: Test I18nServiceAbilityClient RemovePreferredLanguage: remove first preferred language.
 * @tc.type: FUNC
 */
HWTEST_F(I18nServiceTest, I18nServiceFuncTest026, TestSize.Level1)
{
    InitTestEnvironment();
    AddNativeTokenTypeAndPermissions();
    I18nErrorCode err = I18nServiceAbilityClient::AddPreferredLanguage("fr", 0);
    EXPECT_EQ(err, I18nErrorCode::SUCCESS);
    err = I18nServiceAbilityClient::RemovePreferredLanguage(0);
    EXPECT_EQ(err, I18nErrorCode::SUCCESS);
}

/**
 * @tc.name: I18nServiceFuncTest027
 * @tc.desc: Test I18nServiceAbilityClient RemovePreferredLanguage: remove last preferred language.
 * @tc.type: FUNC
 */
HWTEST_F(I18nServiceTest, I18nServiceFuncTest027, TestSize.Level1)
{
    InitTestEnvironment();
    AddNativeTokenTypeAndPermissions();
    I18nErrorCode err = I18nServiceAbilityClient::AddPreferredLanguage("fr", 0);
    EXPECT_EQ(err, I18nErrorCode::SUCCESS);
    err = I18nServiceAbilityClient::RemovePreferredLanguage(1);
    EXPECT_EQ(err, I18nErrorCode::SUCCESS);
}

/**
 * @tc.name: I18nServiceFuncTest028
 * @tc.desc: Test I18nServiceAbilityClient RemovePreferredLanguage: remove negative index.
 * @tc.type: FUNC
 */
HWTEST_F(I18nServiceTest, I18nServiceFuncTest028, TestSize.Level1)
{
    InitTestEnvironment();
    AddNativeTokenTypeAndPermissions();
    I18nErrorCode err = I18nServiceAbilityClient::AddPreferredLanguage("fr", 0);
    EXPECT_EQ(err, I18nErrorCode::SUCCESS);
    err = I18nServiceAbilityClient::RemovePreferredLanguage(-1);
    EXPECT_EQ(err, I18nErrorCode::SUCCESS);
}

/**
 * @tc.name: I18nServiceFuncTest029
 * @tc.desc: Test I18nServiceAbilityClient SetTemperatureType: set temperature type kelvin.
 * @tc.type: FUNC
 */
HWTEST_F(I18nServiceTest, I18nServiceFuncTest029, TestSize.Level1)
{
    InitTestEnvironment();
    AddNativeTokenTypeAndPermissions();
    I18nErrorCode err = I18nServiceAbilityClient::SetTemperatureType(TemperatureType::KELVIN);
    EXPECT_EQ(err, I18nErrorCode::SUCCESS);
}

/**
 * @tc.name: I18nServiceFuncTest030
 * @tc.desc: Test I18nServiceAbilityClient SetFirstDayOfWeek: set first day of week SUN.
 * @tc.type: FUNC
 */
HWTEST_F(I18nServiceTest, I18nServiceFuncTest030, TestSize.Level1)
{
    InitTestEnvironment();
    AddNativeTokenTypeAndPermissions();
    I18nErrorCode err = I18nServiceAbilityClient::SetFirstDayOfWeek(WeekDay::SUN);
    EXPECT_EQ(err, I18nErrorCode::SUCCESS);
}

/**
 * @tc.name: I18nServiceFuncTest031
 * @tc.desc: Test I18nServiceAbilityClient set and get Collation
 * @tc.type: FUNC
 */
HWTEST_F(I18nServiceTest, I18nServiceFuncTest031, TestSize.Level1)
{
    InitTestEnvironment();
    AddNativeTokenTypeAndPermissions();
    I18nErrorCode err = I18nServiceAbilityClient::SetSystemLanguage("en-Latn-US");
    EXPECT_EQ(err, I18nErrorCode::SUCCESS);
    err = I18nServiceAbilityClient::SetSystemLocale("en-Latn-US");
    EXPECT_EQ(err, I18nErrorCode::SUCCESS);

    std::unordered_map<std::string, std::string> systemCollations;
    err = I18nServiceAbilityClient::GetSystemCollations(systemCollations);
    EXPECT_EQ(err, I18nErrorCode::SUCCESS);
    EXPECT_TRUE(systemCollations.find("lower") != systemCollations.end());
    EXPECT_TRUE(systemCollations.find("upper") != systemCollations.end());
    err = I18nServiceAbilityClient::SetSystemCollation("upper");
    EXPECT_EQ(err, I18nErrorCode::SUCCESS);
    std::string usingCollation;
    err = I18nServiceAbilityClient::GetUsingCollation(usingCollation);
    EXPECT_EQ(err, I18nErrorCode::SUCCESS);
    EXPECT_EQ(usingCollation, "upper");
}

/**
 * @tc.name: I18nServiceFuncTest032
 * @tc.desc: Test I18nServiceAbilityClient set and get numbering system
 * @tc.type: FUNC
 */
HWTEST_F(I18nServiceTest, I18nServiceFuncTest032, TestSize.Level1)
{
    InitTestEnvironment();
    AddNativeTokenTypeAndPermissions();
    I18nErrorCode err = I18nServiceAbilityClient::SetSystemLanguage("ar");
    EXPECT_EQ(err, I18nErrorCode::SUCCESS);
    err = I18nServiceAbilityClient::SetSystemLocale("ar-CN");
    EXPECT_EQ(err, I18nErrorCode::SUCCESS);

    std::unordered_map<std::string, std::string> systemNumberingSystems;
    err = I18nServiceAbilityClient::GetSystemNumberingSystems(systemNumberingSystems);
    EXPECT_EQ(err, I18nErrorCode::SUCCESS);
    EXPECT_TRUE(systemNumberingSystems.find("arab") != systemNumberingSystems.end());
    EXPECT_TRUE(systemNumberingSystems.find("latn") != systemNumberingSystems.end());
    err = I18nServiceAbilityClient::SetSystemNumberingSystem("latn");
    EXPECT_EQ(err, I18nErrorCode::SUCCESS);
    std::string usingNumberingSystem;
    err = I18nServiceAbilityClient::GetUsingNumberingSystem(usingNumberingSystem);
    EXPECT_EQ(err, I18nErrorCode::SUCCESS);
    EXPECT_EQ(usingNumberingSystem, "latn");
}

/**
 * @tc.name: I18nServiceFuncTest033
 * @tc.desc: Test I18nServiceAbilityClient set and get number pattern
 * @tc.type: FUNC
 */
HWTEST_F(I18nServiceTest, I18nServiceFuncTest033, TestSize.Level1)
{
    InitTestEnvironment();
    AddNativeTokenTypeAndPermissions();
    I18nErrorCode err = I18nServiceAbilityClient::SetSystemLanguage("zh-Hans");
    EXPECT_EQ(err, I18nErrorCode::SUCCESS);
    err = I18nServiceAbilityClient::SetSystemLocale("zh-Hans-CN");
    EXPECT_EQ(err, I18nErrorCode::SUCCESS);

    std::unordered_map<std::string, std::string> systemNumberPatterns;
    err = I18nServiceAbilityClient::GetSystemNumberPatterns(systemNumberPatterns);
    EXPECT_EQ(err, I18nErrorCode::SUCCESS);
    EXPECT_TRUE(systemNumberPatterns.find("002c002e") != systemNumberPatterns.end());
    EXPECT_TRUE(systemNumberPatterns.find("0000002e") != systemNumberPatterns.end());
    EXPECT_TRUE(systemNumberPatterns.find("002e002c") != systemNumberPatterns.end());
    EXPECT_TRUE(systemNumberPatterns.find("0000002c") != systemNumberPatterns.end());
    err = I18nServiceAbilityClient::SetSystemNumberPattern("002e002c");
    EXPECT_EQ(err, I18nErrorCode::SUCCESS);
    std::string usingNumberPattern;
    err = I18nServiceAbilityClient::GetUsingNumberPattern(usingNumberPattern);
    EXPECT_EQ(err, I18nErrorCode::SUCCESS);
    EXPECT_EQ(usingNumberPattern, "002e002c");
}

/**
 * @tc.name: I18nServiceFuncTest034
 * @tc.desc: Test I18nServiceAbilityClient set and get measurement
 * @tc.type: FUNC
 */
HWTEST_F(I18nServiceTest, I18nServiceFuncTest034, TestSize.Level1)
{
    InitTestEnvironment();
    AddNativeTokenTypeAndPermissions();
    I18nErrorCode err = I18nServiceAbilityClient::SetSystemLanguage("zh-Hans");
    EXPECT_EQ(err, I18nErrorCode::SUCCESS);
    err = I18nServiceAbilityClient::SetSystemLocale("zh-Hans-CN");
    EXPECT_EQ(err, I18nErrorCode::SUCCESS);

    std::unordered_map<std::string, std::string> systemMeasurements;
    err = I18nServiceAbilityClient::GetSystemMeasurements(systemMeasurements);
    EXPECT_EQ(err, I18nErrorCode::SUCCESS);
    EXPECT_TRUE(systemMeasurements.find("uksystem") != systemMeasurements.end());
    EXPECT_TRUE(systemMeasurements.find("metric") != systemMeasurements.end());
    EXPECT_TRUE(systemMeasurements.find("ussystem") != systemMeasurements.end());
    err = I18nServiceAbilityClient::SetSystemMeasurement("ussystem");
    EXPECT_EQ(err, I18nErrorCode::SUCCESS);
    std::string usingMeasurement;
    err = I18nServiceAbilityClient::GetUsingMeasurement(usingMeasurement);
    EXPECT_EQ(err, I18nErrorCode::SUCCESS);
    EXPECT_EQ(usingMeasurement, "ussystem");
}

/**
 * @tc.name: I18nServiceFuncTest035
 * @tc.desc: Test I18nServiceAbilityClient set and get numerical date patterns
 * @tc.type: FUNC
 */
HWTEST_F(I18nServiceTest, I18nServiceFuncTest035, TestSize.Level1)
{
    InitTestEnvironment();
    AddNativeTokenTypeAndPermissions();
    I18nErrorCode err = I18nServiceAbilityClient::SetSystemLanguage("zh-Hans");
    EXPECT_EQ(err, I18nErrorCode::SUCCESS);
    err = I18nServiceAbilityClient::SetSystemLocale("zh-Hans-CN");
    EXPECT_EQ(err, I18nErrorCode::SUCCESS);
    std::unordered_map<std::string, std::string> numericalDatePatterns;
    err = I18nServiceAbilityClient::GetSystemNumericalDatePatterns(numericalDatePatterns);
    EXPECT_EQ(err, I18nErrorCode::SUCCESS);
    EXPECT_TRUE(numericalDatePatterns.find("d/MM/y") != numericalDatePatterns.end());
    EXPECT_TRUE(numericalDatePatterns.find("d/M/yy") != numericalDatePatterns.end());
    EXPECT_TRUE(numericalDatePatterns.find("y-MM-d") != numericalDatePatterns.end());
    EXPECT_TRUE(numericalDatePatterns.find("d/M/y") != numericalDatePatterns.end());
    EXPECT_TRUE(numericalDatePatterns.find("d/MM/yy") != numericalDatePatterns.end());
    EXPECT_TRUE(numericalDatePatterns.find("d.MM.yy") != numericalDatePatterns.end());
    EXPECT_TRUE(numericalDatePatterns.find("yy/MM/d") != numericalDatePatterns.end());
    EXPECT_TRUE(numericalDatePatterns.find("d.MM.y") != numericalDatePatterns.end());
    EXPECT_TRUE(numericalDatePatterns.find("d-MM-y") != numericalDatePatterns.end());
    EXPECT_TRUE(numericalDatePatterns.find("y/M/d") != numericalDatePatterns.end());
    EXPECT_TRUE(numericalDatePatterns.find("d-M-y") != numericalDatePatterns.end());

    err = I18nServiceAbilityClient::SetSystemNumericalDatePattern("d-M-y");
    EXPECT_EQ(err, I18nErrorCode::SUCCESS);

    std::string identifier;
    err = I18nServiceAbilityClient::GetUsingNumericalDatePattern(identifier);
    EXPECT_EQ(identifier, "d-M-y");
}
} // namespace I18n
} // namespace Global
} // namespace OHOS