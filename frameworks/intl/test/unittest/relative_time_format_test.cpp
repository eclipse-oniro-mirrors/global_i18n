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
#include <sys/stat.h>
#include <cmath>
#include "i18n_types.h"
#include "relative_time_format.h"
#include "format_utils.h"
#include "locale_helper.h"
#include "relative_time_format_test.h"

using namespace OHOS::Global::I18n;
using testing::ext::TestSize;
using namespace std;
using namespace testing;

namespace OHOS {
namespace Global {
namespace I18n {

void RelativeTimeFormatTest::SetUpTestCase(void)
{
}

void RelativeTimeFormatTest::TearDownTestCase(void)
{
}

void RelativeTimeFormatTest::SetUp(void)
{}

void RelativeTimeFormatTest::TearDown(void)
{}

/**
 * @tc.name: RelativeTimeFormatTest0001
 * @tc.desc: Test RelativeTimeFormat
 * @tc.type: FUNC
 */
HWTEST_F(RelativeTimeFormatTest, RelativeTimeFormatTest0001, TestSize.Level1)
{
    ErrorMessage errorMsg;
    const std::vector<std::string> localeTags = {"en"};
    std::vector<std::string> result = RelativeTimeFormat::CanonicalizeLocales(localeTags, errorMsg);
    EXPECT_EQ(localeTags.size(), 1);
    std::vector<std::string> locales { "en" };
    map<string, string> options = {
        { "style", "narrow" }
    };
    std::unique_ptr<RelativeTimeFormat> relativeTimeFmt =
        std::make_unique<RelativeTimeFormat>(locales, options, true);
    EXPECT_TRUE(relativeTimeFmt != nullptr);
}

/**
 * @tc.name: RelativeTimeFormatTest0002
 * @tc.desc: Test GetNumberFieldType
 * @tc.type: FUNC
 */
HWTEST_F(RelativeTimeFormatTest, RelativeTimeFormatTest0002, TestSize.Level1)
{
    const std::string napiType = "number";
    const int32_t fieldId = UNUM_FRACTION_FIELD;
    std::string type = FormatUtils::GetNumberFieldType(napiType, fieldId, 12.45);
    EXPECT_EQ(type, "fraction");
    const int32_t fieldId2 = UNUM_INTEGER_FIELD;
    std::string type2 = FormatUtils::GetNumberFieldType("bigint", fieldId2, 100000);
    EXPECT_EQ(type2, "integer");
    std::string type3 = FormatUtils::GetNumberFieldType("number", fieldId2, INFINITY);
    EXPECT_EQ(type3, "infinity");
    std::string type4 = FormatUtils::GetNumberFieldType("number", fieldId2, NAN);
    EXPECT_EQ(type4, "nan");
}

/**
 * @tc.name: RelativeTimeFormatTest0003
 * @tc.desc: Test CheckParamLocales
 * @tc.type: FUNC
 */
HWTEST_F(RelativeTimeFormatTest, RelativeTimeFormatTest0003, TestSize.Level1)
{
    const std::vector<std::string> localeArray = {"@1"};
    std::string res = LocaleHelper::CheckParamLocales(localeArray);
    EXPECT_EQ(res, "invalid locale");
    const std::vector<std::string> localeArr = {};
    std::string res2 = LocaleHelper::CheckParamLocales(localeArr);
    EXPECT_EQ(res2, "");
    const std::vector<std::string> localeArr3 = {"default"};
    std::string res3 = LocaleHelper::CheckParamLocales(localeArr3);
    EXPECT_EQ(res3, "");
}
} // namespace I18n
} // namespace Global
} // namespace OHOS