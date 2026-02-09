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

#include "intl_plural_rules_test.h"

#include "intl_plural_rules.h"

using testing::ext::TestSize;

namespace OHOS {
namespace Global {
namespace I18n {
void IntlPluralRulesTest::SetUp(void) {}

void IntlPluralRulesTest::TearDown(void) {}

/**
 * @tc.name: IntlPluralRulesFuncTest001
 * @tc.desc: Test Intl Plural constructor
 * @tc.type: FUNC
 */
HWTEST_F(IntlPluralRulesTest, IntlPluralRulesFuncTest001, TestSize.Level1)
{
    std::vector<std::string> localeTags = { "en-GB", "ar-EG" };
    std::unordered_map<std::string, std::string> options = {
        { "localeMatcher", "lookup" },
        { "type", "cardinal"},
        { "minimumIntegerDigits", "1"},
        { "minimumSignificantDigits", "1"},
        { "maximumSignificantDigits", "21"}
    };
    ErrorMessage errorMessage;
    IntlPluralRules intlPulral(localeTags, options, errorMessage);
    EXPECT_EQ(errorMessage.type, ErrorType::NO_ERROR);
    EXPECT_EQ(intlPulral.Select(0, errorMessage), "other");
    EXPECT_EQ(intlPulral.Select(1, errorMessage), "one");
    EXPECT_EQ(intlPulral.Select(2, errorMessage), "other");
    IntlPluralRules::ResolvedValue resolvedValue = intlPulral.ResolvedOptions();
    EXPECT_EQ(resolvedValue.roundingType, RoundingType::SIGNIFICANTDIGITS);
    EXPECT_EQ(resolvedValue.locale, "en");
    EXPECT_EQ(resolvedValue.type, "cardinal");
    EXPECT_EQ(resolvedValue.minimumIntegerDigits, 1);
    EXPECT_EQ(resolvedValue.minimumDigits, 1);
    EXPECT_EQ(resolvedValue.maximumDigits, 21);
    EXPECT_EQ(resolvedValue.pluralCategories[0], "one");
    EXPECT_EQ(resolvedValue.pluralCategories[1], "other");
}

/**
 * @tc.name: IntlPluralRulesFuncTest002
 * @tc.desc: Test Intl Plural constructor
 * @tc.type: FUNC
 */
HWTEST_F(IntlPluralRulesTest, IntlPluralRulesFuncTest002, TestSize.Level1)
{
    std::vector<std::string> localeTags = { "en-GB", "ar-EG" };
    std::unordered_map<std::string, std::string> options = {
        { "localeMatcher", "lookup" },
        { "type", "ordinal"},
        { "minimumIntegerDigits", "2"},
        { "minimumFractionDigits", "10"},
        { "maximumFractionDigits", "20"}
    };
    ErrorMessage errorMessage;
    IntlPluralRules intlPulral(localeTags, options, errorMessage);
    EXPECT_EQ(errorMessage.type, ErrorType::NO_ERROR);
    EXPECT_EQ(intlPulral.Select(0, errorMessage), "other");
    EXPECT_EQ(intlPulral.Select(1, errorMessage), "one");
    EXPECT_EQ(intlPulral.Select(2, errorMessage), "two");
    EXPECT_EQ(intlPulral.Select(3, errorMessage), "few");
    IntlPluralRules::ResolvedValue resolvedValue = intlPulral.ResolvedOptions();
    EXPECT_EQ(resolvedValue.roundingType, RoundingType::FRACTIONDIGITS);
    EXPECT_EQ(resolvedValue.locale, "en");
    EXPECT_EQ(resolvedValue.type, "ordinal");
    EXPECT_EQ(resolvedValue.minimumIntegerDigits, 2);
    EXPECT_EQ(resolvedValue.minimumDigits, 10);
    EXPECT_EQ(resolvedValue.maximumDigits, 20);
    EXPECT_EQ(resolvedValue.pluralCategories[0], "few");
    EXPECT_EQ(resolvedValue.pluralCategories[1], "one");
    EXPECT_EQ(resolvedValue.pluralCategories[2], "two");
    EXPECT_EQ(resolvedValue.pluralCategories[3], "other");
}

/**
 * @tc.name: IntlPluralRulesFuncTest003
 * @tc.desc: Test Intl Plural default constructor
 * @tc.type: FUNC
 */
HWTEST_F(IntlPluralRulesTest, IntlPluralRulesFuncTest003, TestSize.Level1)
{
    std::vector<std::string> localeTags = {};
    std::unordered_map<std::string, std::string> options = {};
    ErrorMessage errorMessage;
    IntlPluralRules intlPulral(localeTags, options, errorMessage);
    EXPECT_EQ(errorMessage.type, ErrorType::NO_ERROR);
    EXPECT_EQ(intlPulral.Select(0, errorMessage), "other");
    EXPECT_EQ(intlPulral.Select(1, errorMessage), "one");
    EXPECT_EQ(intlPulral.Select(2, errorMessage), "other");
    IntlPluralRules::ResolvedValue resolvedValue = intlPulral.ResolvedOptions();
    EXPECT_EQ(resolvedValue.roundingType, RoundingType::FRACTIONDIGITS);
    EXPECT_EQ(resolvedValue.locale, "en-US");
    EXPECT_EQ(resolvedValue.type, "cardinal");
    EXPECT_EQ(resolvedValue.minimumIntegerDigits, 1);
    EXPECT_EQ(resolvedValue.minimumDigits, 0);
    EXPECT_EQ(resolvedValue.maximumDigits, 3);
    EXPECT_EQ(resolvedValue.pluralCategories[0], "one");
    EXPECT_EQ(resolvedValue.pluralCategories[1], "other");
}

/**
 * @tc.name: IntlPluralRulesFuncTest004
 * @tc.desc: Test Intl Plural constructor bogus param
 * @tc.type: FUNC
 */
HWTEST_F(IntlPluralRulesTest, IntlPluralRulesFuncTest004, TestSize.Level1)
{
    std::vector<std::string> invalidLocaleTags = { "en-GB", "en_GB" };
    std::unordered_map<std::string, std::string> options = {};
    ErrorMessage invalidLocaleError;
    IntlPluralRules invalidPulral(invalidLocaleTags, options, invalidLocaleError);
    EXPECT_EQ(invalidLocaleError.type, ErrorType::RANGE_ERROR);
    EXPECT_EQ(invalidLocaleError.message, "invalid locale");

    std::vector<std::string> localeTags = {};
    std::unordered_map<std::string, std::string> bogusOptions = { {"localeMatcher", "bogus"} };
    ErrorMessage localeMatcherError;
    invalidPulral = IntlPluralRules(localeTags, bogusOptions, localeMatcherError);
    EXPECT_EQ(localeMatcherError.type, ErrorType::RANGE_ERROR);
    EXPECT_EQ(localeMatcherError.message, "getStringOption failed");

    ErrorMessage typeError;
    bogusOptions = { {"type", "bogus"} };
    invalidPulral = IntlPluralRules(localeTags, bogusOptions, typeError);
    EXPECT_EQ(typeError.type, ErrorType::RANGE_ERROR);
    EXPECT_EQ(typeError.message, "getStringOption failed");

    ErrorMessage minMinFraction;
    bogusOptions = { {"minimumFractionDigits", "-1"} };
    invalidPulral = IntlPluralRules(localeTags, bogusOptions, minMinFraction);
    EXPECT_EQ(minMinFraction.type, ErrorType::RANGE_ERROR);
    EXPECT_EQ(minMinFraction.message, "");

    ErrorMessage maxMinFraction;
    bogusOptions = { {"minimumFractionDigits", "21"} };
    invalidPulral = IntlPluralRules(localeTags, bogusOptions, maxMinFraction);
    EXPECT_EQ(maxMinFraction.type, ErrorType::RANGE_ERROR);
    EXPECT_EQ(maxMinFraction.message, "");

    ErrorMessage minMaxFraction;
    bogusOptions = { {"maximumFractionDigits", "-1"} };
    invalidPulral = IntlPluralRules(localeTags, bogusOptions, minMaxFraction);
    EXPECT_EQ(minMaxFraction.type, ErrorType::RANGE_ERROR);
    EXPECT_EQ(minMaxFraction.message, "");

    ErrorMessage maxMaxFraction;
    bogusOptions = { {"maximumFractionDigits", "21"} };
    invalidPulral = IntlPluralRules(localeTags, bogusOptions, maxMaxFraction);
    EXPECT_EQ(maxMaxFraction.type, ErrorType::RANGE_ERROR);
    EXPECT_EQ(maxMaxFraction.message, "");
}

/**
 * @tc.name: IntlPluralRulesFuncTest005
 * @tc.desc: Test Intl Plural constructor bogus param
 * @tc.type: FUNC
 */
HWTEST_F(IntlPluralRulesTest, IntlPluralRulesFuncTest005, TestSize.Level1)
{
    std::vector<std::string> localeTags = {};
    ErrorMessage minInteger;
    std::unordered_map<std::string, std::string> bogusOptions = { {"minimumIntegerDigits", "0"} };
    IntlPluralRules invalidPulral(localeTags, bogusOptions, minInteger);
    EXPECT_EQ(minInteger.type, ErrorType::RANGE_ERROR);
    EXPECT_EQ(minInteger.message, "");

    ErrorMessage maxInteger;
    bogusOptions = { {"minimumIntegerDigits", "22"} };
    invalidPulral = IntlPluralRules(localeTags, bogusOptions, maxInteger);
    EXPECT_EQ(maxInteger.type, ErrorType::RANGE_ERROR);
    EXPECT_EQ(maxInteger.message, "");

    ErrorMessage minMinSignificant;
    bogusOptions = { {"minimumSignificantDigits", "0"} };
    invalidPulral = IntlPluralRules(localeTags, bogusOptions, minMinSignificant);
    EXPECT_EQ(minMinSignificant.type, ErrorType::RANGE_ERROR);
    EXPECT_EQ(minMinSignificant.message, "");

    ErrorMessage maxMinSignificant;
    bogusOptions = { {"minimumSignificantDigits", "22"} };
    invalidPulral = IntlPluralRules(localeTags, bogusOptions, maxMinSignificant);
    EXPECT_EQ(maxMinSignificant.type, ErrorType::RANGE_ERROR);
    EXPECT_EQ(maxMinSignificant.message, "");

    ErrorMessage minMaxSignificant;
    bogusOptions = { {"maximumSignificantDigits", "0"} };
    invalidPulral = IntlPluralRules(localeTags, bogusOptions, minMaxSignificant);
    EXPECT_EQ(minMaxSignificant.type, ErrorType::RANGE_ERROR);
    EXPECT_EQ(minMaxSignificant.message, "");

    ErrorMessage maxMaxSignificant;
    bogusOptions = { {"maximumSignificantDigits", "22"} };
    invalidPulral = IntlPluralRules(localeTags, bogusOptions, maxMaxSignificant);
    EXPECT_EQ(maxMaxSignificant.type, ErrorType::RANGE_ERROR);
    EXPECT_EQ(maxMaxSignificant.message, "");
}

/**
 * @tc.name: IntlPluralRulesFuncTest006
 * @tc.desc: Test Intl Plural SupportedLocalesOf
 * @tc.type: FUNC
 */
 HWTEST_F(IntlPluralRulesTest, IntlPluralRulesFuncTest006, TestSize.Level1)
{
    std::vector<std::string> requestLocales = { "en-US", "ban", "ar-OM", "de-DE" };
    std::unordered_map<std::string, std::string> options = { {"localeMatcher", "lookup"} };
    ErrorMessage errorMessage;
    std::vector<std::string> supportedLocales =
        IntlPluralRules::SupportedLocalesOf(requestLocales, options, errorMessage);
    EXPECT_EQ(errorMessage.type, ErrorType::NO_ERROR);
    EXPECT_EQ(supportedLocales.size(), 3);
    EXPECT_EQ(supportedLocales[0], "en-US");
    EXPECT_EQ(supportedLocales[1], "ar-OM");
    EXPECT_EQ(supportedLocales[2], "de-DE");
}

/**
 * @tc.name: IntlPluralRulesFuncTest007
 * @tc.desc: Test Intl Plural SupportedLocalesOf bogus locale
 * @tc.type: FUNC
 */
 HWTEST_F(IntlPluralRulesTest, IntlPluralRulesFuncTest007, TestSize.Level1)
{
    std::vector<std::string> requestLocales = { "zh-CN", "test", "ar-OM", "de-DE" };
    std::unordered_map<std::string, std::string> options = { {"localeMatcher", "best fit"} };
    ErrorMessage errorMessage;
    std::vector<std::string> supportedLocales =
        IntlPluralRules::SupportedLocalesOf(requestLocales, options, errorMessage);
    EXPECT_EQ(errorMessage.type, ErrorType::RANGE_ERROR);
    EXPECT_EQ(errorMessage.message, "invalid locale");
}

/**
 * @tc.name: IntlPluralRulesFuncTest008
 * @tc.desc: Test Intl Plural SupportedLocalesOf bogus options
 * @tc.type: FUNC
 */
 HWTEST_F(IntlPluralRulesTest, IntlPluralRulesFuncTest008, TestSize.Level1)
{
    std::vector<std::string> requestLocales = { "zh-CN", "en-US", "ar-OM", "de-DE" };
    std::unordered_map<std::string, std::string> options = { {"localeMatcher", "bogus"} };
    ErrorMessage errorMessage;
    std::vector<std::string> supportedLocales =
        IntlPluralRules::SupportedLocalesOf(requestLocales, options, errorMessage);
    EXPECT_EQ(errorMessage.type, ErrorType::RANGE_ERROR);
    EXPECT_EQ(errorMessage.message, "getStringOption failed");
}
} // namespace I18n
} // namespace Global
} // namespace OHOS
