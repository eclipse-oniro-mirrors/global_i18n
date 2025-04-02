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

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <map>
#include <vector>
#include "number_format.h"
#include "intl_test.h"
#include "generate_ics_file.h"
#include "signature_verifier.h"
#include <unistd.h>
#include "unicode/utypes.h"

using namespace OHOS::Global::I18n;
using testing::ext::TestSize;
using namespace std;
using namespace testing;

namespace OHOS {
namespace Global {
namespace I18n {
/**
 * @tc.name: IntlFuncTest0099
 * @tc.desc: Test Intl NumberFormat.format
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest, IntlFuncTest0099, TestSize.Level1)
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
 * @tc.name: IntlFuncTest00100
 * @tc.desc: Test Intl NumberFormat.format
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest, IntlFuncTest00100, TestSize.Level1)
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
 * @tc.name: IntlFuncTest00101
 * @tc.desc: Test Intl NumberFormat.format
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest, IntlFuncTest00101, TestSize.Level1)
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
 * @tc.name: IntlFuncTest00102
 * @tc.desc: Test Intl NumberFormat.format
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest, IntlFuncTest00102, TestSize.Level1)
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
 * @tc.name: IntlFuncTest00108
 * @tc.desc: Test Intl NumberFormat.format
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest, IntlFuncTest00108, TestSize.Level1)
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
 * @tc.name: IntlFuncTest00109
 * @tc.desc: Test Intl NumberFormat.format
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest, IntlFuncTest00109, TestSize.Level1)
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
} // namespace I18n
} // namespace Global
} // namespace OHOS