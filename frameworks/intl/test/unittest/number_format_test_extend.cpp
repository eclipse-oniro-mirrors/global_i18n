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

#include "number_format_test_extend.h"
#include <gtest/gtest.h>
#include "locale_config.h"
#include "number_format.h"
#include "simple_number_format.h"
#include "unicode/locid.h"
#include "parameter.h"

using namespace OHOS::Global::I18n;
using testing::ext::TestSize;
using namespace std;

namespace OHOS {
namespace Global {
namespace I18n {
/**
 * @tc.name: NumberFormatFuncTest0050
 * @tc.desc: Test Intl NumberFormat.format
 * @tc.type: FUNC
 */
HWTEST_F(NumberFormatTest, NumberFormatFuncTest0050, TestSize.Level1)
{
    string localeAR = "ar";
    vector<string> localesAR{localeAR};
    string localeHE = "HE";
    vector<string> localesHE{localeHE};
    string localeMY = "my";
    vector<string> localesMY{localeMY};
    string style = "unit";
    string unit = "beat-per-minute";
    map<string, string> options = { { "style", style},
                                    { "unit", unit },
                                    { "numberingSystem", "latn" } };
    std::unique_ptr<NumberFormat> numFmtAR = std::make_unique<NumberFormat>(localesAR, options);
    ASSERT_TRUE(numFmtAR != nullptr);
    EXPECT_EQ(numFmtAR->Format(0), "0 نبضات/د");
    EXPECT_EQ(numFmtAR->Format(1), "نبضة واحدة/د");
    EXPECT_EQ(numFmtAR->Format(2), "نبضتان/د");
    EXPECT_EQ(numFmtAR->Format(3), "3 نبضات/د");
    EXPECT_EQ(numFmtAR->Format(11), "11 نبضة/د");
    EXPECT_EQ(numFmtAR->Format(100), "100 نبضة/د");
    std::unique_ptr<NumberFormat> numFmtHE = std::make_unique<NumberFormat>(localesHE, options);
    ASSERT_TRUE(numFmtHE != nullptr);
    EXPECT_EQ(numFmtHE->Format(0), "0 פעימות לדקה");
    EXPECT_EQ(numFmtHE->Format(1), "פעימה 1 לדקה");
    EXPECT_EQ(numFmtHE->Format(2), "2 פעימות לדקה");
    EXPECT_EQ(numFmtHE->Format(3), "3 פעימות לדקה");
    std::unique_ptr<NumberFormat> numFmtMY = std::make_unique<NumberFormat>(localesMY, options);
    ASSERT_TRUE(numFmtMY != nullptr);
    EXPECT_EQ(numFmtMY->Format(123), "123 bpm");
}

/**
 * @tc.name: NumberFormatFuncTest0051
 * @tc.desc: Test Intl NumberFormat.format
 * @tc.type: FUNC
 */
HWTEST_F(NumberFormatTest, NumberFormatFuncTest0051, TestSize.Level1)
{
    string localeAR = "ar";
    vector<string> localesAR{localeAR};
    string localeHE = "HE";
    vector<string> localesHE{localeHE};
    string localeMY = "my";
    vector<string> localesMY{localeMY};
    string style = "unit";
    string unit = "body-weight-per-second";
    map<string, string> options = { { "style", style},
                                    { "unit", unit },
                                    { "numberingSystem", "latn" } };
    std::unique_ptr<NumberFormat> numFmtAR = std::make_unique<NumberFormat>(localesAR, options);
    ASSERT_TRUE(numFmtAR != nullptr);
    EXPECT_EQ(numFmtAR->Format(0), "وزن 0 أجسام/ث");
    EXPECT_EQ(numFmtAR->Format(1), "وزن جسم واحد/ث");
    EXPECT_EQ(numFmtAR->Format(2), "وزن جسمين/ث");
    EXPECT_EQ(numFmtAR->Format(3), "وزن 3 أجسام/ث");
    EXPECT_EQ(numFmtAR->Format(11), "وزن 11 جسمًا/ث");
    EXPECT_EQ(numFmtAR->Format(100), "وزن 100 جسم/ث");
    std::unique_ptr<NumberFormat> numFmtHE = std::make_unique<NumberFormat>(localesHE, options);
    ASSERT_TRUE(numFmtHE != nullptr);
    EXPECT_EQ(numFmtHE->Format(0), "0 BW/s");
    EXPECT_EQ(numFmtHE->Format(1), "1 BW/s");
    EXPECT_EQ(numFmtHE->Format(2), "2 BW/s");
    EXPECT_EQ(numFmtHE->Format(3), "3 BW/s");
    std::unique_ptr<NumberFormat> numFmtMY = std::make_unique<NumberFormat>(localesMY, options);
    ASSERT_TRUE(numFmtMY != nullptr);
    EXPECT_EQ(numFmtMY->Format(123), "123 BW/s");
}

/**
 * @tc.name: NumberFormatFuncTest0052
 * @tc.desc: Test Intl NumberFormat.format
 * @tc.type: FUNC
 */
HWTEST_F(NumberFormatTest, NumberFormatFuncTest0052, TestSize.Level1)
{
    string localeAR = "ar";
    vector<string> localesAR{localeAR};
    string localeHE = "HE";
    vector<string> localesHE{localeHE};
    string localeMY = "my";
    vector<string> localesMY{localeMY};
    string style = "unit";
    string unit = "breath-per-minute";
    map<string, string> options = { { "style", style},
                                    { "unit", unit },
                                    { "numberingSystem", "latn" } };
    std::unique_ptr<NumberFormat> numFmtAR = std::make_unique<NumberFormat>(localesAR, options);
    ASSERT_TRUE(numFmtAR != nullptr);
    EXPECT_EQ(numFmtAR->Format(0), "0 أنفاس/د");
    EXPECT_EQ(numFmtAR->Format(1), "نفس واحد/د");
    EXPECT_EQ(numFmtAR->Format(2), "نفسان/د");
    EXPECT_EQ(numFmtAR->Format(3), "3 أنفاس/د");
    EXPECT_EQ(numFmtAR->Format(11), "11 نفسًا/د");
    EXPECT_EQ(numFmtAR->Format(100), "100 نفس/د");
    std::unique_ptr<NumberFormat> numFmtHE = std::make_unique<NumberFormat>(localesHE, options);
    ASSERT_TRUE(numFmtHE != nullptr);
    EXPECT_EQ(numFmtHE->Format(0), "0 נשימות לדקה");
    EXPECT_EQ(numFmtHE->Format(1), "נשימה 1 לדקה");
    EXPECT_EQ(numFmtHE->Format(2), "2 נשימות לדקה");
    EXPECT_EQ(numFmtHE->Format(3), "3 נשימות לדקה");
    std::unique_ptr<NumberFormat> numFmtMY = std::make_unique<NumberFormat>(localesMY, options);
    ASSERT_TRUE(numFmtMY != nullptr);
    EXPECT_EQ(numFmtMY->Format(123), "123 brpm");
}

/**
 * @tc.name: NumberFormatFuncTest0053
 * @tc.desc: Test Intl NumberFormat.format
 * @tc.type: FUNC
 */
HWTEST_F(NumberFormatTest, NumberFormatFuncTest0053, TestSize.Level1)
{
    string localeAR = "ar";
    vector<string> localesAR{localeAR};
    string localeHE = "HE";
    vector<string> localesHE{localeHE};
    string localeMY = "my";
    vector<string> localesMY{localeMY};
    string style = "unit";
    string unit = "foot-per-hour";
    map<string, string> options = { { "style", style},
                                    { "unit", unit },
                                    { "numberingSystem", "latn" } };
    std::unique_ptr<NumberFormat> numFmtAR = std::make_unique<NumberFormat>(localesAR, options);
    ASSERT_TRUE(numFmtAR != nullptr);
    EXPECT_EQ(numFmtAR->Format(0), "0 أقدام/س");
    EXPECT_EQ(numFmtAR->Format(1), "قدم واحدة/س");
    EXPECT_EQ(numFmtAR->Format(2), "قدمان/س");
    EXPECT_EQ(numFmtAR->Format(3), "3 أقدام/س");
    EXPECT_EQ(numFmtAR->Format(11), "11 قدمًا/س");
    EXPECT_EQ(numFmtAR->Format(100), "100 قدم/س");
    std::unique_ptr<NumberFormat> numFmtHE = std::make_unique<NumberFormat>(localesHE, options);
    ASSERT_TRUE(numFmtHE != nullptr);
    EXPECT_EQ(numFmtHE->Format(0), "0 רגל לשעה");
    EXPECT_EQ(numFmtHE->Format(1), "1 רגל לשעה");
    EXPECT_EQ(numFmtHE->Format(2), "2 רגל לשעה");
    EXPECT_EQ(numFmtHE->Format(3), "3 רגל לשעה");
    std::unique_ptr<NumberFormat> numFmtMY = std::make_unique<NumberFormat>(localesMY, options);
    ASSERT_TRUE(numFmtMY != nullptr);
    EXPECT_EQ(numFmtMY->Format(123), "123 ft/h");
}

/**
 * @tc.name: NumberFormatFuncTest0054
 * @tc.desc: Test Intl NumberFormat.format
 * @tc.type: FUNC
 */
HWTEST_F(NumberFormatTest, NumberFormatFuncTest0054, TestSize.Level1)
{
    string localeAR = "ar";
    vector<string> localesAR{localeAR};
    string localeHE = "HE";
    vector<string> localesHE{localeHE};
    string localeMY = "my";
    vector<string> localesMY{localeMY};
    string style = "unit";
    string unit = "jump-rope-per-minute";
    map<string, string> options = { { "style", style},
                                    { "unit", unit },
                                    { "numberingSystem", "latn" } };
    std::unique_ptr<NumberFormat> numFmtAR = std::make_unique<NumberFormat>(localesAR, options);
    ASSERT_TRUE(numFmtAR != nullptr);
    EXPECT_EQ(numFmtAR->Format(0), "0 قفزات/د");
    EXPECT_EQ(numFmtAR->Format(1), "قفزة/د");
    EXPECT_EQ(numFmtAR->Format(2), "قفزتان/د");
    EXPECT_EQ(numFmtAR->Format(3), "3 قفزات/د");
    EXPECT_EQ(numFmtAR->Format(11), "11 قفزة/د");
    EXPECT_EQ(numFmtAR->Format(100), "100 قفزة/د");
    std::unique_ptr<NumberFormat> numFmtHE = std::make_unique<NumberFormat>(localesHE, options);
    ASSERT_TRUE(numFmtHE != nullptr);
    EXPECT_EQ(numFmtHE->Format(0), "0 קפיצות לדקה");
    EXPECT_EQ(numFmtHE->Format(1), "קפיצה 1 לדקה");
    EXPECT_EQ(numFmtHE->Format(2), "2 קפיצות לדקה");
    EXPECT_EQ(numFmtHE->Format(3), "3 קפיצות לדקה");
    std::unique_ptr<NumberFormat> numFmtMY = std::make_unique<NumberFormat>(localesMY, options);
    ASSERT_TRUE(numFmtMY != nullptr);
    EXPECT_EQ(numFmtMY->Format(123), "တစ်မိနစ်လျှင် 123 ကြိမ်");
}

/**
 * @tc.name: NumberFormatFuncTest0055
 * @tc.desc: Test Intl NumberFormat.format
 * @tc.type: FUNC
 */
HWTEST_F(NumberFormatTest, NumberFormatFuncTest0055, TestSize.Level1)
{
    string localeAR = "ar";
    vector<string> localesAR{localeAR};
    string localeHE = "HE";
    vector<string> localesHE{localeHE};
    string localeMY = "my";
    vector<string> localesMY{localeMY};
    string style = "unit";
    string unit = "meter-per-hour";
    map<string, string> options = { { "style", style},
                                    { "unit", unit },
                                    { "numberingSystem", "latn" } };
    std::unique_ptr<NumberFormat> numFmtAR = std::make_unique<NumberFormat>(localesAR, options);
    ASSERT_TRUE(numFmtAR != nullptr);
    EXPECT_EQ(numFmtAR->Format(0), "0 م/س");
    EXPECT_EQ(numFmtAR->Format(1), "1 م/س");
    EXPECT_EQ(numFmtAR->Format(2), "2 م/س");
    EXPECT_EQ(numFmtAR->Format(3), "3 م/س");
    EXPECT_EQ(numFmtAR->Format(11), "11 م/س");
    EXPECT_EQ(numFmtAR->Format(100), "100 م/س");
    std::unique_ptr<NumberFormat> numFmtHE = std::make_unique<NumberFormat>(localesHE, options);
    ASSERT_TRUE(numFmtHE != nullptr);
    EXPECT_EQ(numFmtHE->Format(0), "0 מטר לשעה");
    EXPECT_EQ(numFmtHE->Format(1), "1 מטר לשעה");
    EXPECT_EQ(numFmtHE->Format(2), "2 מטר לשעה");
    EXPECT_EQ(numFmtHE->Format(3), "3 מטר לשעה");
    std::unique_ptr<NumberFormat> numFmtMY = std::make_unique<NumberFormat>(localesMY, options);
    ASSERT_TRUE(numFmtMY != nullptr);
    EXPECT_EQ(numFmtMY->Format(123), "123 m/h");
}

/**
 * @tc.name: NumberFormatFuncTest0056
 * @tc.desc: Test Intl NumberFormat.format
 * @tc.type: FUNC
 */
HWTEST_F(NumberFormatTest, NumberFormatFuncTest0056, TestSize.Level1)
{
    string localeAR = "ar";
    vector<string> localesAR{localeAR};
    string localeHE = "HE";
    vector<string> localesHE{localeHE};
    string localeMY = "my";
    vector<string> localesMY{localeMY};
    string style = "unit";
    string unit = "milliliter-per-minute-per-kilogram";
    map<string, string> options = { { "style", style},
                                    { "unit", unit },
                                    { "numberingSystem", "latn" } };
    std::unique_ptr<NumberFormat> numFmtAR = std::make_unique<NumberFormat>(localesAR, options);
    ASSERT_TRUE(numFmtAR != nullptr);
    EXPECT_EQ(numFmtAR->Format(0), "0 ملم/كغم/د");
    EXPECT_EQ(numFmtAR->Format(1), "1 ملم/كغم/د");
    EXPECT_EQ(numFmtAR->Format(2), "2 ملم/كغم/د");
    EXPECT_EQ(numFmtAR->Format(3), "3 ملم/كغم/د");
    EXPECT_EQ(numFmtAR->Format(11), "11 ملم/كغم/د");
    EXPECT_EQ(numFmtAR->Format(100), "100 ملم/كغم/د");
    std::unique_ptr<NumberFormat> numFmtHE = std::make_unique<NumberFormat>(localesHE, options);
    ASSERT_TRUE(numFmtHE != nullptr);
    EXPECT_EQ(numFmtHE->Format(0), "0 ml/kg/min");
    EXPECT_EQ(numFmtHE->Format(1), "1 ml/kg/min");
    EXPECT_EQ(numFmtHE->Format(2), "2 ml/kg/min");
    EXPECT_EQ(numFmtHE->Format(3), "3 ml/kg/min");
    std::unique_ptr<NumberFormat> numFmtMY = std::make_unique<NumberFormat>(localesMY, options);
    ASSERT_TRUE(numFmtMY != nullptr);
    EXPECT_EQ(numFmtMY->Format(123), "123 ml/kg/min");
}

/**
 * @tc.name: NumberFormatFuncTest0057
 * @tc.desc: Test Intl NumberFormat.format
 * @tc.type: FUNC
 */
HWTEST_F(NumberFormatTest, NumberFormatFuncTest0057, TestSize.Level1)
{
    string localeAR = "ar";
    vector<string> localesAR{localeAR};
    string localeHE = "HE";
    vector<string> localesHE{localeHE};
    string localeMY = "my";
    vector<string> localesMY{localeMY};
    string style = "unit";
    string unit = "rotation-per-minute";
    map<string, string> options = { { "style", style},
                                    { "unit", unit },
                                    { "numberingSystem", "latn" } };
    std::unique_ptr<NumberFormat> numFmtAR = std::make_unique<NumberFormat>(localesAR, options);
    ASSERT_TRUE(numFmtAR != nullptr);
    EXPECT_EQ(numFmtAR->Format(0), "0 دورات/د");
    EXPECT_EQ(numFmtAR->Format(1), "دورة واحدة/د");
    EXPECT_EQ(numFmtAR->Format(2), "دورتان/د");
    EXPECT_EQ(numFmtAR->Format(3), "3 دورات/د");
    EXPECT_EQ(numFmtAR->Format(11), "11 دورة/د");
    EXPECT_EQ(numFmtAR->Format(100), "100 دورة/د");
    std::unique_ptr<NumberFormat> numFmtHE = std::make_unique<NumberFormat>(localesHE, options);
    ASSERT_TRUE(numFmtHE != nullptr);
    EXPECT_EQ(numFmtHE->Format(0), "0 סל\u0022ד");
    EXPECT_EQ(numFmtHE->Format(1), "1 סל\u0022ד");
    EXPECT_EQ(numFmtHE->Format(2), "2 סל\u0022ד");
    EXPECT_EQ(numFmtHE->Format(3), "3 סל\u0022ד");
    std::unique_ptr<NumberFormat> numFmtMY = std::make_unique<NumberFormat>(localesMY, options);
    ASSERT_TRUE(numFmtMY != nullptr);
    EXPECT_EQ(numFmtMY->Format(123), "123 rpm");
}

/**
 * @tc.name: NumberFormatFuncTest0058
 * @tc.desc: Test Intl NumberFormat.format
 * @tc.type: FUNC
 */
HWTEST_F(NumberFormatTest, NumberFormatFuncTest0058, TestSize.Level1)
{
    string localeAR = "ar";
    vector<string> localesAR{localeAR};
    string localeHE = "HE";
    vector<string> localesHE{localeHE};
    string localeMY = "my";
    vector<string> localesMY{localeMY};
    string style = "unit";
    string unit = "step-per-minute";
    map<string, string> options = { { "style", style},
                                    { "unit", unit },
                                    { "numberingSystem", "latn" } };
    std::unique_ptr<NumberFormat> numFmtAR = std::make_unique<NumberFormat>(localesAR, options);
    ASSERT_TRUE(numFmtAR != nullptr);
    EXPECT_EQ(numFmtAR->Format(0), "0 خطوات/د");
    EXPECT_EQ(numFmtAR->Format(1), "خطوة واحدة/د");
    EXPECT_EQ(numFmtAR->Format(2), "خطوتان/د");
    EXPECT_EQ(numFmtAR->Format(3), "3 خطوات/د");
    EXPECT_EQ(numFmtAR->Format(11), "11 خطوة/د");
    EXPECT_EQ(numFmtAR->Format(100), "100 خطوة/د");
    std::unique_ptr<NumberFormat> numFmtHE = std::make_unique<NumberFormat>(localesHE, options);
    ASSERT_TRUE(numFmtHE != nullptr);
    EXPECT_EQ(numFmtHE->Format(0), "0 צעדים לדקה");
    EXPECT_EQ(numFmtHE->Format(1), "צעד 1 לדקה");
    EXPECT_EQ(numFmtHE->Format(2), "2 צעדים לדקה");
    EXPECT_EQ(numFmtHE->Format(3), "3 צעדים לדקה");
    std::unique_ptr<NumberFormat> numFmtMY = std::make_unique<NumberFormat>(localesMY, options);
    ASSERT_TRUE(numFmtMY != nullptr);
    EXPECT_EQ(numFmtMY->Format(123), "တစ်မိနစ်လျှင် 123 လှမ်း");
}

/**
 * @tc.name: NumberFormatFuncTest0059
 * @tc.desc: Test Intl NumberFormat.format
 * @tc.type: FUNC
 */
HWTEST_F(NumberFormatTest, NumberFormatFuncTest0059, TestSize.Level1)
{
    string localeAR = "ar";
    vector<string> localesAR{localeAR};
    string localeHE = "HE";
    vector<string> localesHE{localeHE};
    string localeMY = "my";
    vector<string> localesMY{localeMY};
    string style = "unit";
    string unit = "stroke-per-minute";
    map<string, string> options = { { "style", style},
                                    { "unit", unit },
                                    { "numberingSystem", "latn" } };
    std::unique_ptr<NumberFormat> numFmtAR = std::make_unique<NumberFormat>(localesAR, options);
    ASSERT_TRUE(numFmtAR != nullptr);
    EXPECT_EQ(numFmtAR->Format(0), "0 ضربات/د");
    EXPECT_EQ(numFmtAR->Format(1), "ضربة واحدة/د");
    EXPECT_EQ(numFmtAR->Format(2), "ضربتان/د");
    EXPECT_EQ(numFmtAR->Format(3), "3 ضربات/د");
    EXPECT_EQ(numFmtAR->Format(11), "11 ضربة/د");
    EXPECT_EQ(numFmtAR->Format(100), "100 ضربة/د");
    std::unique_ptr<NumberFormat> numFmtHE = std::make_unique<NumberFormat>(localesHE, options);
    ASSERT_TRUE(numFmtHE != nullptr);
    EXPECT_EQ(numFmtHE->Format(0), "0 תנועות לדקה");
    EXPECT_EQ(numFmtHE->Format(1), "תנועה 1 לדקה");
    EXPECT_EQ(numFmtHE->Format(2), "2 תנועות לדקה");
    EXPECT_EQ(numFmtHE->Format(3), "3 תנועות לדקה");
    std::unique_ptr<NumberFormat> numFmtMY = std::make_unique<NumberFormat>(localesMY, options);
    ASSERT_TRUE(numFmtMY != nullptr);
    EXPECT_EQ(numFmtMY->Format(123), "တစ်မိနစ်လျှင် 123 ချက်");
}

/**
 * @tc.name: NumberFormatFuncTest0060
 * @tc.desc: Test Intl NumberFormat.format
 * @tc.type: FUNC
 */
HWTEST_F(NumberFormatTest, NumberFormatFuncTest0060, TestSize.Level1)
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
    ASSERT_TRUE(format != nullptr);
    EXPECT_EQ(res, format->Format(12));
    delete format;

    vector<string> locales = { "en-CN" };
    string style = "decimal";
    map<string, string> options = { { "style", style },
                                    { "notation", "compact" } };
    NumberFormat *numFmt = new (std::nothrow) NumberFormat(locales, options);
    ASSERT_TRUE(numFmt != nullptr);
    string out = numFmt->Format(123456.789);
    EXPECT_EQ(out, "123K");
    EXPECT_EQ(numFmt->GetStyle(), style);
    delete numFmt;

    locales.clear();
    locales.push_back("en-GB");
    options = { { "style", "unit" },
                { "unit", "day" },
                { "unitUsage", "elapsed-time-second"} };
    NumberFormat *numFormat = new (std::nothrow) NumberFormat(locales, options);
    ASSERT_TRUE(numFormat != nullptr);
    std::string nowRes = numFormat->Format(0.00001);
    std::string secondRes = numFormat->Format(0.00004);
    std::string dayRes = numFormat->Format(1.5);
    std::string monthRes = numFormat->Format(-62.5);

    EXPECT_EQ(nowRes, "now");
    EXPECT_EQ(dayRes, "yesterday");
    delete numFormat;
    if (NumberFormatTest::deviceType == "wearable" || NumberFormatTest::deviceType == "liteWearable" ||
        NumberFormatTest::deviceType == "watch") {
        EXPECT_EQ(secondRes, "3 sec ago");
        EXPECT_EQ(monthRes, "2 mo ago");
    } else if (NumberFormatTest::deviceType == "tablet" || NumberFormatTest::deviceType == "2in1" ||
        NumberFormatTest::deviceType == "tv" || NumberFormatTest::deviceType == "pc") {
        EXPECT_EQ(secondRes, "3 seconds ago");
        EXPECT_EQ(monthRes, "2 months ago");
    } else {
        EXPECT_EQ(secondRes, "3 seconds ago");
        EXPECT_EQ(monthRes, "2 months ago");
    }
}

/**
 * @tc.name: NumberFormatFuncTest0061
 * @tc.desc: Test Intl NumberFormat.format
 * @tc.type: FUNC
 */
HWTEST_F(NumberFormatTest, NumberFormatFuncTest0061, TestSize.Level1)
{
    vector<string> locales;
    locales.push_back("en-GB");
    map<string, string> unitOptions = {
        { "style", "unit" },
        { "unit", "hectare" }
    };
    NumberFormat *unitFormatter = new NumberFormat(locales, unitOptions);
    ASSERT_TRUE(unitFormatter != nullptr);
    string unitRes = unitFormatter->Format(123);
    map<string, string> currencyOptions = {
        { "style", "currency" },
        { "currency", "USD" }
    };
    NumberFormat *currencyFormatter = new NumberFormat(locales, currencyOptions);
    ASSERT_TRUE(currencyFormatter != nullptr);
    string currencyRes = currencyFormatter->Format(123);
    if (NumberFormatTest::deviceType == "wearable" || NumberFormatTest::deviceType == "liteWearable" ||
        NumberFormatTest::deviceType == "watch") {
        EXPECT_EQ(currencyRes, "$123.00");
        EXPECT_EQ(unitRes, "123ha");
    } else if (NumberFormatTest::deviceType == "tablet" || NumberFormatTest::deviceType == "2in1" ||
        NumberFormatTest::deviceType == "tv" || NumberFormatTest::deviceType == "pc") {
        EXPECT_EQ(currencyRes, "US$123.00");
        EXPECT_EQ(unitRes, "123 hectares");
    } else {
        EXPECT_EQ(currencyRes, "US$123.00");
        EXPECT_EQ(unitRes, "123 ha");
    }
    delete unitFormatter;
    delete currencyFormatter;
}

/**
 * @tc.name: NumberFormatFuncTest0062
 * @tc.desc: Test Intl NumberFormat.format
 * @tc.type: FUNC
 */
HWTEST_F(NumberFormatTest, NumberFormatFuncTest0062, TestSize.Level1)
{
    std::vector<std::string> locales = { "en-GB" };
    map<string, string> options = { { "style", "unit" },
                                    { "unit", "megabyte" },
                                    { "unitUsage", "size-file-byte"} };
    std::unique_ptr<NumberFormat> numFmt = std::make_unique<NumberFormat>(locales, options);
    ASSERT_TRUE(numFmt != nullptr);
    std::string byteRes = numFmt->Format(0.00000812);
    std::string kbRes = numFmt->Format(0.125);
    std::string mbRes = numFmt->Format(3.5);
    std::string gbRes = numFmt->Format(23122);

    options["unitUsage"] = "size-shortfile-byte";
    std::unique_ptr<NumberFormat> numFormat = std::make_unique<NumberFormat>(locales, options);
    std::string shortByteRes = numFormat->Format(0.00000812);
    std::string shortKbRes = numFormat->Format(0.125);
    std::string shortMbRes = numFormat->Format(3.5);
    std::string shortGbRes = numFormat->Format(23122);

    if (NumberFormatTest::deviceType == "wearable" || NumberFormatTest::deviceType == "liteWearable" ||
        NumberFormatTest::deviceType == "watch") {
        EXPECT_EQ(byteRes, "8B");
        EXPECT_EQ(kbRes, "125KB");
        EXPECT_EQ(mbRes, "3.50MB");
        EXPECT_EQ(gbRes, "23.12GB");
        EXPECT_EQ(shortByteRes, "8B");
        EXPECT_EQ(shortKbRes, "125KB");
        EXPECT_EQ(shortMbRes, "3.5MB");
        EXPECT_EQ(shortGbRes, "23GB");
    } else if (NumberFormatTest::deviceType == "tablet" || NumberFormatTest::deviceType == "2in1" ||
        NumberFormatTest::deviceType == "tv" || NumberFormatTest::deviceType == "pc") {
        EXPECT_EQ(byteRes, "8 bytes");
        EXPECT_EQ(kbRes, "125 kilobytes");
        EXPECT_EQ(mbRes, "3.50 megabytes");
        EXPECT_EQ(gbRes, "23.12 gigabytes");
        EXPECT_EQ(shortByteRes, "8 bytes");
        EXPECT_EQ(shortKbRes, "125 kilobytes");
        EXPECT_EQ(shortMbRes, "3.5 megabytes");
        EXPECT_EQ(shortGbRes, "23 gigabytes");
    } else {
        EXPECT_EQ(byteRes, "8 byte");
        EXPECT_EQ(kbRes, "125 KB");
        EXPECT_EQ(mbRes, "3.50 MB");
        EXPECT_EQ(gbRes, "23.12 GB");
        EXPECT_EQ(shortByteRes, "8 byte");
        EXPECT_EQ(shortKbRes, "125 KB");
        EXPECT_EQ(shortMbRes, "3.5 MB");
        EXPECT_EQ(shortGbRes, "23 GB");
    }
}

/**
 * @tc.name: NumberFormatFuncTest0063
 * @tc.desc: Test Intl NumberFormat.FormatRangeToParts
 * @tc.type: FUNC
 */
HWTEST_F(NumberFormatTest, NumberFormatFuncTest0063, TestSize.Level1)
{
    std::string locale = "de-DE";
    std::vector<std::string> locales;
    locales.push_back(locale);
    map<string, string> options = { { "style", "currency" },
                                    { "currency", "EUR" } };
    std::unique_ptr<NumberFormat> numFmt = std::make_unique<NumberFormat>(locales, options);
    ASSERT_TRUE(numFmt != nullptr);
    std::vector<std::vector<std::string>> result;
    numFmt->FormatRangeToParts(3500, 9500, result);
    EXPECT_EQ(result.size(), 13);
    ASSERT_TRUE(result.size() > 0 && result[0].size() > 1);
    EXPECT_EQ(result[0][0], "integer");
    EXPECT_EQ(result[0][1], "3");
    std::vector<std::vector<std::string>> result2;
    locale = "en-US";
    locales.clear();
    locales.push_back(locale);
    options = { { "style", "currency" },
                { "currency", "USD" } };
    double amount = 654321.987;
    std::unique_ptr<NumberFormat> numFmt2 = std::make_unique<NumberFormat>(locales, options);
    ASSERT_TRUE(numFmt2 != nullptr);
    numFmt2->FormatToParts(amount, result2);
    EXPECT_EQ(result2.size(), 6);
    std::string formatResult = numFmt2->Format(amount);
    EXPECT_EQ(formatResult, "$654,321.99");

    std::vector<std::vector<std::string>> result3;
    locales.clear();
    locales.push_back("ar");
    options = {};
    std::unique_ptr<NumberFormat> numFmt3 = std::make_unique<NumberFormat>(locales, options);
    ASSERT_TRUE(numFmt3 != nullptr);
    numFmt3->FormatToParts(result3);
    EXPECT_EQ(result3.size(), 1);
}

/**
 * @tc.name: NumberFormatFuncTest0064
 * @tc.desc: Test Intl NumberFormat.FormatRangeToParts
 * @tc.type: FUNC
 */
HWTEST_F(NumberFormatTest, NumberFormatFuncTest0064, TestSize.Level1)
{
    std::string locale = "de-DE";
    std::vector<std::string> locales;
    locales.push_back(locale);
    map<string, string> options = { { "style", "currency" },
                                    { "currency", "EUR" } };
    std::vector<std::vector<std::string>> result;
    locales.clear();
    locales.push_back("en-US");
    options = {};
    std::unique_ptr<NumberFormat> numFmt2 = std::make_unique<NumberFormat>(locales, options);
    ASSERT_TRUE(numFmt2 != nullptr);
    result.clear();
    //"87,654,321–98,765,432";
    numFmt2->FormatRangeToParts(87654321, 98765432, result);
    EXPECT_EQ(result.size(), 11);
    ASSERT_TRUE(result.size() > 0 && result[0].size() > 1);
    EXPECT_EQ(result[0][0], "integer");
    EXPECT_EQ(result[0][1], "87");
    result.clear();
    numFmt2->FormatRangeToParts(-100, -100, result);
    EXPECT_EQ(result.size(), 3);
}

/**
 * @tc.name: NumberFormatFuncTest0065
 * @tc.desc: Test Intl NumberFormat.FormatRangeToParts
 * @tc.type: FUNC
 */
HWTEST_F(NumberFormatTest, NumberFormatFuncTest0065, TestSize.Level1)
{
    std::string locale = "es-ES";
    std::vector<std::string> locales;
    locales.push_back(locale);
    map<string, string> options = { { "style", "currency" },
                                    { "currency", "EUR" },
                                    { "maximumFractionDigits", "0" } };
    std::vector<std::vector<std::string>> result;
    std::unique_ptr<NumberFormat> numFmt2 = std::make_unique<NumberFormat>(locales, options);
    ASSERT_TRUE(numFmt2 != nullptr);
    result.clear();
    // "3-5 €"
    numFmt2->FormatRangeToParts(3, 5, result);
    EXPECT_EQ(result.size(), 5);
    ASSERT_TRUE(result.size() > 4 && result[4].size() > 1);
    EXPECT_EQ(result[4][0], "currency");
    EXPECT_EQ(result[4][1], "€");
    result.clear();

    // "~3 €"
    numFmt2->FormatRangeToParts(2.9, 3.1, result);
    EXPECT_EQ(result.size(), 4);

    result.clear();
    numFmt2->FormatRangeToParts(-123, 65, result);
    EXPECT_EQ(result.size(), 8);
    std::vector<std::string> localeTags = {"ban", "id-u-co-pinyin", "de-ID"};
    std::map<std::string, std::string> map = {
        {"localeMatcher", "lookup"}
    };
    I18nErrorCode i18nStatus = I18nErrorCode::SUCCESS;
    std::vector<std::string> resultLocales = NumberFormat::SupportedLocalesOf(localeTags, map, i18nStatus);
    EXPECT_EQ(resultLocales.size(), 2);
}

/**
 * @tc.name: NumberFormatFuncTest0066
 * @tc.desc: Test Intl NumberFormat.FormatRange
 * @tc.type: FUNC
 */
HWTEST_F(NumberFormatTest, NumberFormatFuncTest0066, TestSize.Level1)
{
    std::string locale = "en";
    std::vector<std::string> locales;
    locales.push_back(locale);
    map<string, string> options = {};
    std::unique_ptr<NumberFormat> numFmt2 = std::make_unique<NumberFormat>(locales, options);
    ASSERT_TRUE(numFmt2 != nullptr);
    std::string result = numFmt2->FormatRange(543.245, 100000000000000011);
    EXPECT_EQ(result, "543.245–100,000,000,000,000,020");
    std::string result2 = numFmt2->FormatRange(543.245, 543.245);
    EXPECT_EQ(result2, "~543.245");
}

/**
 * @tc.name: NumberFormatFuncTest0067
 * @tc.desc: Test Intl NumberFormat.FormatRange
 * @tc.type: FUNC
 */
HWTEST_F(NumberFormatTest, NumberFormatFuncTest0067, TestSize.Level1)
{
    std::string systemLanguage = LocaleConfig::GetSystemLanguage();
    std::string systemLocale = LocaleConfig::GetSystemLocale();
    LocaleConfig::SetSystemLanguage("zh-Hans");
    LocaleConfig::SetSystemLocale("zh-Hans-CN-u-ms-ussystem-mu-kelvin");

    std::vector<std::string> locales { "en-Latn-US-u-mu-celsius" };
    map<string, string> options = {
        { "style", "unit" },
        { "unit", "kelvin" },
        { "unitUsage", "temperature" }
    };
    std::unique_ptr<NumberFormat> numberFormat = std::make_unique<NumberFormat>(locales, options);
    ASSERT_TRUE(numberFormat != nullptr);
    std::string result = numberFormat->Format(100);
    std::string resultOfRange = numberFormat->FormatRange(100, 200);

    if (NumberFormatTest::deviceType == "wearable" || NumberFormatTest::deviceType == "liteWearable" ||
        NumberFormatTest::deviceType == "watch") {
        EXPECT_EQ(result, "-173.15°C");
        EXPECT_EQ(resultOfRange, "-173.15 – -73.15°C");
    } else if (NumberFormatTest::deviceType == "tablet" || NumberFormatTest::deviceType == "2in1" ||
        NumberFormatTest::deviceType == "tv" || NumberFormatTest::deviceType == "pc") {
        EXPECT_EQ(result, "-173.15 degrees Celsius");
        EXPECT_EQ(resultOfRange, "-173.15 – -73.15 degrees Celsius");
    } else {
        EXPECT_EQ(result, "-173.15°C");
        EXPECT_EQ(resultOfRange, "-173.15 – -73.15°C");
    }

    LocaleConfig::SetSystemLanguage(systemLanguage);
    LocaleConfig::SetSystemLocale(systemLocale);
}

/**
 * @tc.name: NumberFormatFuncTest0068
 * @tc.desc: Test Intl NumberFormat
 * @tc.type: FUNC
 */
HWTEST_F(NumberFormatTest, NumberFormatFuncTest0068, TestSize.Level1)
{
    std::vector<std::string> locales { "ar" };
    map<string, string> options = {
        { "numeric", "auto" },
        { "localeMatcher", "best fit" }
    };
    std::unique_ptr<NumberFormat> numberFormat = std::make_unique<NumberFormat>(locales, options, true);
    ASSERT_TRUE(numberFormat != nullptr);
    map<std::string, std::string> resultMap;
    numberFormat->GetResolvedOptions(resultMap);
    EXPECT_EQ(resultMap.at("numberingSystem"), "arab");
    int32_t code = 0;
    map<std::string, std::string> optionsMap = {
        { "currency", "###" },
    };
    std::string message = NumberFormat::CheckNumberFormatOptions(optionsMap, code);
    EXPECT_EQ(message, "not a wellformed code");
    EXPECT_EQ(code, -1);
    options = { { "style", "unit" } };
    code = 0;
    std::string msg = NumberFormat::CheckNumberFormatOptions(options, code);
    EXPECT_EQ(msg, "style is unit but unit is undefined");
    options = { { "style", "currency" } };
    std::string checkCurrencyMsg = NumberFormat::CheckNumberFormatOptions(options, code);
    EXPECT_EQ(checkCurrencyMsg, "style is currency but currency is undefined");
}

/**
 * @tc.name: NumberFormatFuncTest0069
 * @tc.desc: Test Intl NumberFormat
 * @tc.type: FUNC
 */
HWTEST_F(NumberFormatTest, NumberFormatFuncTest0069, TestSize.Level1)
{
    std::pair<std::string, std::string> numberPattern = { "", "," };
    std::string sample = NumberFormat::GetNumberPatternSample("ar-CN-u-nu-latn", numberPattern);
    EXPECT_EQ(sample, "12345,67");
}
} // namespace I18n
} // namespace Global
} // namespace OHOS