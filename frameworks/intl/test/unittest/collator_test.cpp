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
#include "collator_test.h"

#include <map>
#include <vector>

#include "collator.h"

using namespace OHOS::Global::I18n;
using testing::ext::TestSize;
using namespace std;

namespace OHOS {
namespace Global {
namespace I18n {

void CollatorTest::SetUpTestCase(void)
{}

void CollatorTest::TearDownTestCase(void)
{}

void CollatorTest::SetUp(void)
{}

void CollatorTest::TearDown(void)
{}

/**
 * @tc.name: CollatorFuncTest001
 * @tc.desc: Test Intl Collator.supportedLocalesOf
 * @tc.type: FUNC
 */
HWTEST_F(CollatorTest, CollatorFuncTest001, TestSize.Level1)
{
    std::vector<std::string> localeTags = {};
    std::map<std::string, std::string> options = {};
    Collator collator(localeTags, options);
    std::map<std::string, std::string> resolvedOptions;
    collator.ResolvedOptions(resolvedOptions);
    auto it = resolvedOptions.find("locale");
    ASSERT_TRUE(it != resolvedOptions.end());
    std::string defaultLocale = it->second;

    std::vector<std::string> requestLocales = {"zxx"};
    requestLocales.push_back(defaultLocale);
    I18nErrorCode status = I18nErrorCode::SUCCESS;
    std::vector<std::string> resultLocales = Collator::SupportedLocalesOf(requestLocales, options, status);
    EXPECT_EQ(status, I18nErrorCode::SUCCESS);
    EXPECT_EQ(resultLocales.size(), 1);
    EXPECT_EQ(resultLocales[0], defaultLocale);
}

/**
 * @tc.name: CollatorFuncTest002
 * @tc.desc: Test Intl Collator.supportedLocalesOf
 * @tc.type: FUNC
 */
HWTEST_F(CollatorTest, CollatorFuncTest002, TestSize.Level1)
{
    std::vector<std::string> localeTags = {"de", "en", "es", "fr", "it"};
    std::map<std::string, std::string> options = {{"localeMatcher", "lookup"}};
    I18nErrorCode status = I18nErrorCode::SUCCESS;
    std::vector<std::string> resultLocales = Collator::SupportedLocalesOf(localeTags, options, status);
    EXPECT_EQ(status, I18nErrorCode::SUCCESS);
    for (auto locale : resultLocales) {
        std::vector<std::string> locales;
        locales.push_back(locale);
        std::map<std::string, std::string> configs = {{"sensitivity", "variant"}, {"localeMatcher", "lookup"}};
        Collator collator(locales, configs);
        CompareResult result = collator.Compare("A", "b");
        EXPECT_EQ(result, CompareResult::SMALLER);
        result = collator.Compare("b", "C");
        EXPECT_EQ(result, CompareResult::SMALLER);
    }
}

/**
 * @tc.name: CollatorFuncTest003
 * @tc.desc: Test Intl Collator.supportedLocalesOf
 * @tc.type: FUNC
 */
HWTEST_F(CollatorTest, CollatorFuncTest003, TestSize.Level1)
{
    std::vector<std::string> localeTags = {"ban", "id-u-co-pinyin", "de-ID"};
    std::map<std::string, std::string> options = {{"localeMatcher", "lookup"}};
    I18nErrorCode status = I18nErrorCode::SUCCESS;
    std::vector<std::string> resultLocales = Collator::SupportedLocalesOf(localeTags, options, status);
    EXPECT_EQ(status, I18nErrorCode::SUCCESS);
    EXPECT_EQ(resultLocales.size(), 2);
    EXPECT_EQ(resultLocales[0], "id-u-co-pinyin");
    EXPECT_EQ(resultLocales[1], "de-ID");
    std::map<std::string, std::string> optionsBogus = {{"localeMatcher", "bogus"}};
    std::vector<std::string> resultLocalesBogus = Collator::SupportedLocalesOf(localeTags, optionsBogus, status);
    EXPECT_EQ(status, I18nErrorCode::INVALID_PARAM);
    EXPECT_EQ(resultLocalesBogus.size(), 0);
}

/**
 * @tc.name: CollatorFuncTest004
 * @tc.desc: Test Intl Collator.supportedLocalesOf
 * @tc.type: FUNC
 */
HWTEST_F(CollatorTest, CollatorFuncTest004, TestSize.Level1)
{
    std::vector<std::string> localeTags = {"ban", "id-u-co-pinyin", "de-ID"};
    std::map<std::string, std::string> options = {{"localeMatcher", "bogus"}};
    Collator collator(localeTags, options);
    I18nErrorCode status = collator.GetError();
    EXPECT_EQ(status, I18nErrorCode::INVALID_PARAM);
}

/**
 * @tc.name: CollatorFuncTest005
 * @tc.desc: Test Intl Collator.supportedLocalesOf
 * @tc.type: FUNC
 */
HWTEST_F(CollatorTest, CollatorFuncTest005, TestSize.Level1)
{
    std::vector<std::string> localeTags = {"ban", "id-u-co-pinyin", "de-ID"};
    std::map<std::string, std::string> options = {{"usage", "bogus"}};
    Collator collator(localeTags, options);
    I18nErrorCode status = collator.GetError();
    EXPECT_EQ(status, I18nErrorCode::INVALID_PARAM);
}

/**
 * @tc.name: CollatorFuncTest006
 * @tc.desc: Test Intl Collator.supportedLocalesOf
 * @tc.type: FUNC
 */
HWTEST_F(CollatorTest, CollatorFuncTest006, TestSize.Level1)
{
    std::vector<std::string> localeTags = {"ban", "id-u-co-pinyin", "de-ID"};
    std::map<std::string, std::string> options = {{"sensitivity", "bogus"}};
    Collator collator(localeTags, options);
    I18nErrorCode status = collator.GetError();
    EXPECT_EQ(status, I18nErrorCode::INVALID_PARAM);
}

/**
 * @tc.name: CollatorFuncTest007
 * @tc.desc: Test Intl Collator.supportedLocalesOf
 * @tc.type: FUNC
 */
HWTEST_F(CollatorTest, CollatorFuncTest007, TestSize.Level1)
{
    std::vector<std::string> localeTags = {"ban", "id-u-co-pinyin", "de-ID"};
    std::map<std::string, std::string> options = {{"ignorePunctuation", "bogus"}};
    Collator collator(localeTags, options);
    I18nErrorCode status = collator.GetError();
    EXPECT_EQ(status, I18nErrorCode::INVALID_PARAM);
}

/**
 * @tc.name: CollatorFuncTest008
 * @tc.desc: Test Intl Collator.supportedLocalesOf
 * @tc.type: FUNC
 */
HWTEST_F(CollatorTest, CollatorFuncTest008, TestSize.Level1)
{
    std::vector<std::string> localeTags = {"ban", "id-u-co-pinyin", "de-ID"};
    std::map<std::string, std::string> options = {{"numeric", "bogus"}};
    Collator collator(localeTags, options);
    I18nErrorCode status = collator.GetError();
    EXPECT_EQ(status, I18nErrorCode::INVALID_PARAM);
}

/**
 * @tc.name: CollatorFuncTest009
 * @tc.desc: Test Intl Collator.supportedLocalesOf
 * @tc.type: FUNC
 */
HWTEST_F(CollatorTest, CollatorFuncTest009, TestSize.Level1)
{
    std::vector<std::string> localeTags = {"ban", "id-u-co-pinyin", "de-ID"};
    std::map<std::string, std::string> options = {{"caseFirst", "bogus"}};
    Collator collator(localeTags, options);
    I18nErrorCode status = collator.GetError();
    EXPECT_EQ(status, I18nErrorCode::INVALID_PARAM);
}
} // namespace I18n
} // namespace Global
} // namespace OHOS