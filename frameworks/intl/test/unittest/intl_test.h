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

#ifndef OHOS_GLOBAL_I18N_INTL_TEST_H
#define OHOS_GLOBAL_I18N_INTL_TEST_H

namespace OHOS {
namespace Global {
namespace I18n {
class IntlTest : public testing::Test {
public:
    static std::string originalLanguage;
    static std::string originalRegion;
    static std::string originalLocale;
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

int IntlFuncTest001(void);
int IntlFuncTest002(void);
int IntlFuncTest003(void);
int IntlFuncTest004(void);
int IntlFuncTest005(void);
int IntlFuncTest006(void);
int IntlFuncTest007(void);
int IntlFuncTest008(void);
int IntlFuncTest009(void);
int IntlFuncTest0010(void);
int IntlFuncTest0011(void);
int IntlFuncTest0012(void);
int IntlFuncTest0013(void);
int IntlFuncTest0014(void);
int IntlFuncTest0015(void);
int IntlFuncTest0016(void);
int IntlFuncTest0017(void);
int IntlFuncTest0018(void);
int IntlFuncTest0019(void);
int IntlFuncTest0020(void);
int IntlFuncTest0021(void);
int IntlFuncTest0022(void);
int IntlFuncTest0023(void);
int IntlFuncTest0024(void);
int IntlFuncTest0025(void);
int IntlFuncTest0026(void);
int IntlFuncTest0027(void);
int IntlFuncTest0028(void);
int IntlFuncTest0029(void);
int IntlFuncTest0030(void);
int IntlFuncTest0031(void);
int IntlFuncTest0032(void);
int IntlFuncTest0033(void);
int IntlFuncTest0034(void);
int IntlFuncTest0035(void);
int IntlFuncTest0036(void);
int IntlFuncTest0037(void);
int IntlFuncTest0038(void);
int IntlFuncTest0039(void);
int IntlFuncTest0040(void);
int IntlFuncTest0041(void);
int IntlFuncTest0042(void);
int IntlFuncTest0043(void);
int IntlFuncTest0044(void);
int IntlFuncTest0045(void);
int IntlFuncTest0046(void);
int IntlFuncTest0047(void);
int IntlFuncTest0048(void);
int IntlFuncTest0049(void);
int IntlFuncTest0050(void);
int IntlFuncTest0051(void);
int IntlFuncTest0052(void);
int IntlFuncTest0053(void);
int IntlFuncTest0054(void);
int IntlFuncTest0055(void);
int IntlFuncTest0056(void);
int IntlFuncTest0057(void);
int IntlFuncTest0058(void);
int IntlFuncTest0059(void);
int IntlFuncTest0060(void);
int IntlFuncTest0061(void);
int IntlFuncTest0062(void);
int IntlFuncTest0063(void);
int IntlFuncTest0064(void);
int IntlFuncTest0065(void);
int IntlFuncTest0066(void);
int IntlFuncTest0067(void);
int IntlFuncTest0068(void);
int IntlFuncTest0069(void);
int IntlFuncTest0070(void);
int IntlFuncTest0071(void);
int IntlFuncTest0072(void);
int IntlFuncTest0073(void);
int IntlFuncTest0074(void);
int IntlFuncTest0075(void);
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif