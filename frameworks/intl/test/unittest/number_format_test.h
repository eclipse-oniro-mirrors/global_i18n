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

#ifndef OHOS_GLOBAL_I18N_NUMBER_FORMAT_TEST
#define OHOS_GLOBAL_I18N_NUMBER_FORMAT_TEST

#include <gtest/gtest.h>
#include <string>

namespace OHOS {
namespace Global {
namespace I18n {
class NumberFormatTest : public testing::Test {
public:
    static std::string originalLanguage;
    static std::string originalRegion;
    static std::string originalLocale;
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

int NumberFormatFuncTest001(void);
int NumberFormatFuncTest002(void);
int NumberFormatFuncTest003(void);
int NumberFormatFuncTest004(void);
int NumberFormatFuncTest005(void);
int NumberFormatFuncTest006(void);
int NumberFormatFuncTest007(void);
int NumberFormatFuncTest008(void);
int NumberFormatFuncTest009(void);
int NumberFormatFuncTest0010(void);
int NumberFormatFuncTest0011(void);
int NumberFormatFuncTest0012(void);
int NumberFormatFuncTest0013(void);
int NumberFormatFuncTest0014(void);
int NumberFormatFuncTest0015(void);
int NumberFormatFuncTest0016(void);
int NumberFormatFuncTest0017(void);
int NumberFormatFuncTest0018(void);
int NumberFormatFuncTest0019(void);
int NumberFormatFuncTest0020(void);
int NumberFormatFuncTest0021(void);
int NumberFormatFuncTest0022(void);
int NumberFormatFuncTest0023(void);
int NumberFormatFuncTest0024(void);
int NumberFormatFuncTest0025(void);
int NumberFormatFuncTest0026(void);
int NumberFormatFuncTest0027(void);
int NumberFormatFuncTest0028(void);
int NumberFormatFuncTest0029(void);
int NumberFormatFuncTest0030(void);
int NumberFormatFuncTest0031(void);
int NumberFormatFuncTest0032(void);
int NumberFormatFuncTest0033(void);
int NumberFormatFuncTest0034(void);
int NumberFormatFuncTest0035(void);
int NumberFormatFuncTest0036(void);
int NumberFormatFuncTest0037(void);
int NumberFormatFuncTest0038(void);
int NumberFormatFuncTest0039(void);
int NumberFormatFuncTest0040(void);
int NumberFormatFuncTest0041(void);
int NumberFormatFuncTest0042(void);
int NumberFormatFuncTest0043(void);
int NumberFormatFuncTest0044(void);
int NumberFormatFuncTest0045(void);
int NumberFormatFuncTest0046(void);
int NumberFormatFuncTest0047(void);
int NumberFormatFuncTest0048(void);
int NumberFormatFuncTest0049(void);
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif