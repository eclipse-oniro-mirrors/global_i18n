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
#include <map>
#include <vector>
#include <ctime>
#include "customized_date_time_format_test.h"
#include <unistd.h>
#include "unicode/utypes.h"
#include "locale_config.h"
#include "ohos/init_data.h"
#include "i18n_types.h"
#include "simple_date_time_format.h"

using namespace OHOS::Global::I18n;
using testing::ext::TestSize;
using namespace std;
using namespace testing;

namespace OHOS {
namespace Global {
namespace I18n {

void CustomizedDateTimeFormatTest::SetUpTestCase(void)
{
    SetHwIcuDirectory();
}

void CustomizedDateTimeFormatTest::TearDownTestCase(void)
{
}

void CustomizedDateTimeFormatTest::SetUp(void)
{}

void CustomizedDateTimeFormatTest::TearDown(void)
{}

/**
 * @tc.name: CustomizedDateTimeFormatTest0001
 * @tc.desc: Test Customized Data
 * @tc.type: FUNC
 */
HWTEST_F(CustomizedDateTimeFormatTest, CustomizedDateTimeFormatTest0001, TestSize.Level1)
{
    I18nErrorCode errCode = I18nErrorCode::SUCCESS;
    struct tm tmObj = {.tm_mday = 23, .tm_mon = 3, .tm_year = 124};
    time_t timep = mktime(&tmObj);
    auto sDateFormat0 = std::make_unique<SimpleDateTimeFormat>("MEd", "am", false, errCode);
    EXPECT_EQ(sDateFormat0->Format(timep*1000), "ማክሰኞ፣ 23/04");
    auto sDateFormat1 = std::make_unique<SimpleDateTimeFormat>("yM", "am", false, errCode);
    EXPECT_EQ(sDateFormat1->Format(timep*1000), "04/2024");
    auto sDateFormat2 = std::make_unique<SimpleDateTimeFormat>("yMEd", "am", false, errCode);
    EXPECT_EQ(sDateFormat2->Format(timep*1000), "ማክሰኞ፣ 23/04/2024");
    auto sDateFormat3 = std::make_unique<SimpleDateTimeFormat>("yMMMEd", "am", false, errCode);
    EXPECT_EQ(sDateFormat3->Format(timep*1000), "ማክሰኞ፣ ኤፕሪ 23፣ 2024");
    auto sDateFormat4 = std::make_unique<SimpleDateTimeFormat>("yMd", "am", false, errCode);
    EXPECT_EQ(sDateFormat4->Format(timep*1000), "23/04/2024");
    auto sDateFormat5 = std::make_unique<SimpleDateTimeFormat>("MEd", "ar", false, errCode);
    EXPECT_EQ(sDateFormat5->Format(timep*1000), "الثلاثاء، ٢٣/‏٤");
    auto sDateFormat6 = std::make_unique<SimpleDateTimeFormat>("yMMMEd", "ar", false, errCode);
    EXPECT_EQ(sDateFormat6->Format(timep*1000), "الثلاثاء، ٢٣ أبريل، ٢٠٢٤");
    auto sDateFormat7 = std::make_unique<SimpleDateTimeFormat>("yMMMd", "ar", false, errCode);
    EXPECT_EQ(sDateFormat7->Format(timep*1000), "٢٣ أبريل، ٢٠٢٤");
    auto sDateFormat8 = std::make_unique<SimpleDateTimeFormat>("GyMMM", "bg", false, errCode);
    EXPECT_EQ(sDateFormat8->Format(timep*1000), "апр 2024 г. сл.Хр.");
    auto sDateFormat9 = std::make_unique<SimpleDateTimeFormat>("GyMMMEd", "bg", false, errCode);
    EXPECT_EQ(sDateFormat9->Format(timep*1000), "вт, 23 апр 2024 г. сл.Хр.");
    auto sDateFormat10 = std::make_unique<SimpleDateTimeFormat>("GyMMMd", "bg", false, errCode);
    EXPECT_EQ(sDateFormat10->Format(timep*1000), "23 апр 2024 г. сл.Хр.");
    auto sDateFormat11 = std::make_unique<SimpleDateTimeFormat>("MMM", "bg", false, errCode);
    EXPECT_EQ(sDateFormat11->Format(timep*1000), "апр");
    auto sDateFormat12 = std::make_unique<SimpleDateTimeFormat>("MMMEd", "bg", false, errCode);
    EXPECT_EQ(sDateFormat12->Format(timep*1000), "вт, 23 апр");
    auto sDateFormat13 = std::make_unique<SimpleDateTimeFormat>("MMMd", "bg", false, errCode);
    EXPECT_EQ(sDateFormat13->Format(timep*1000), "23 апр");
    auto sDateFormat14 = std::make_unique<SimpleDateTimeFormat>("yMMM", "bg", false, errCode);
    EXPECT_EQ(sDateFormat14->Format(timep*1000), "апр 2024 г.");
    auto sDateFormat15 = std::make_unique<SimpleDateTimeFormat>("yMMMEd", "bg", false, errCode);
    EXPECT_EQ(sDateFormat15->Format(timep*1000), "вт, 23 апр 2024 г.");
    auto sDateFormat16 = std::make_unique<SimpleDateTimeFormat>("yMMMd", "bg", false, errCode);
    EXPECT_EQ(sDateFormat16->Format(timep*1000), "23 апр 2024 г.");
    auto sDateFormat17 = std::make_unique<SimpleDateTimeFormat>("MEd", "bn", false, errCode);
    EXPECT_EQ(sDateFormat17->Format(timep*1000), "মঙ্গল, ২৩/৪");
    auto sDateFormat18 = std::make_unique<SimpleDateTimeFormat>("MMMEd", "bn", false, errCode);
    EXPECT_EQ(sDateFormat18->Format(timep*1000), "মঙ্গল, ২৩ এপ্রি");
    auto sDateFormat19 = std::make_unique<SimpleDateTimeFormat>("MMMMEd", "bn", false, errCode);
    EXPECT_EQ(sDateFormat19->Format(timep*1000), "মঙ্গল, ২৩ এপ্রিল");
    auto sDateFormat20 = std::make_unique<SimpleDateTimeFormat>("MMdd", "bn", false, errCode);
    EXPECT_EQ(sDateFormat20->Format(timep*1000), "২৩/০৪");
    auto sDateFormat21 = std::make_unique<SimpleDateTimeFormat>("yMM", "bn", false, errCode);
    EXPECT_EQ(sDateFormat21->Format(timep*1000), "০৪/২০২৪");
}

/**
 * @tc.name: CustomizedDateTimeFormatTest0002
 * @tc.desc: Test Customized Data
 * @tc.type: FUNC
 */
HWTEST_F(CustomizedDateTimeFormatTest, CustomizedDateTimeFormatTest0002, TestSize.Level1)
{
    I18nErrorCode errCode = I18nErrorCode::SUCCESS;
    struct tm tmObj = {.tm_mday = 23, .tm_mon = 3, .tm_year = 124};
    time_t timep = mktime(&tmObj);
    auto sDateFormat23 = std::make_unique<SimpleDateTimeFormat>("MMMMd", "bo", false, errCode);
    EXPECT_EQ(sDateFormat23->Format(timep*1000), "ཟླ་བ་བཞི་པ་ འི་ཚེས་ 23");
    auto sDateFormat24 = std::make_unique<SimpleDateTimeFormat>("MMMd", "bo", false, errCode);
    EXPECT_EQ(sDateFormat24->Format(timep*1000), "ཟླ་༤ ཚེས་ 23");
    auto sDateFormat25 = std::make_unique<SimpleDateTimeFormat>("yMMM", "bo", false, errCode);
    EXPECT_EQ(sDateFormat25->Format(timep*1000), "2024 ལོའི ་ཟླ་༤ ཚེས");
    auto sDateFormat26 = std::make_unique<SimpleDateTimeFormat>("yMMMMd", "bo", false, errCode);
    EXPECT_EQ(sDateFormat26->Format(timep*1000), "སྤྱི་ལོ་ 2024 ཟླ་བ་བཞི་པ་ འི་ཚེས ་23");
    auto sDateFormat27 = std::make_unique<SimpleDateTimeFormat>("yMMMd", "bo", false, errCode);
    EXPECT_EQ(sDateFormat27->Format(timep*1000), "2024 ལོའི་ ཟླ་༤ ཚེས་ 23");
    auto sDateFormat28 = std::make_unique<SimpleDateTimeFormat>("yM", "bs", false, errCode);
    EXPECT_EQ(sDateFormat28->Format(timep*1000), "4.2024");
    auto sDateFormat29 = std::make_unique<SimpleDateTimeFormat>("yMM", "bs", false, errCode);
    EXPECT_EQ(sDateFormat29->Format(timep*1000), "4.2024");
    auto sDateFormat30 = std::make_unique<SimpleDateTimeFormat>("EHm", "ca", false, errCode);
    EXPECT_EQ(sDateFormat30->Format(timep*1000), "dt. 0.00");
    auto sDateFormat31 = std::make_unique<SimpleDateTimeFormat>("EHms", "ca", false, errCode);
    EXPECT_EQ(sDateFormat31->Format(timep*1000), "dt. 0.00.00");
    auto sDateFormat32 = std::make_unique<SimpleDateTimeFormat>("Ehm", "ca", false, errCode);
    EXPECT_EQ(sDateFormat32->Format(timep*1000), "dt. 12.00 a. m.");
    auto sDateFormat33 = std::make_unique<SimpleDateTimeFormat>("Ehms", "ca", false, errCode);
    EXPECT_EQ(sDateFormat33->Format(timep*1000), "dt. 12.00.00 a. m.");
    auto sDateFormat34 = std::make_unique<SimpleDateTimeFormat>("GyMMMEd", "ca", false, errCode);
    EXPECT_EQ(sDateFormat34->Format(timep*1000), "dt., 23 abr. 2024 dC");
    auto sDateFormat35 = std::make_unique<SimpleDateTimeFormat>("GyMMMM", "ca", false, errCode);
    EXPECT_EQ(sDateFormat35->Format(timep*1000), "abril de 2024 dC");
    auto sDateFormat36 = std::make_unique<SimpleDateTimeFormat>("GyMMMMEd", "ca", false, errCode);
    EXPECT_EQ(sDateFormat36->Format(timep*1000), "dt., 23 d’abril de 2024 dC");
    auto sDateFormat37 = std::make_unique<SimpleDateTimeFormat>("GyMMMMd", "ca", false, errCode);
    EXPECT_EQ(sDateFormat37->Format(timep*1000), "23 d’abril de 2024 dC");
    auto sDateFormat38 = std::make_unique<SimpleDateTimeFormat>("GyMMMd", "ca", false, errCode);
    EXPECT_EQ(sDateFormat38->Format(timep*1000), "23 abr. 2024 dC");
    auto sDateFormat39 = std::make_unique<SimpleDateTimeFormat>("Hm", "ca", false, errCode);
    EXPECT_EQ(sDateFormat39->Format(timep*1000), "0.00");
    auto sDateFormat40 = std::make_unique<SimpleDateTimeFormat>("Hms", "ca", false, errCode);
    EXPECT_EQ(sDateFormat40->Format(timep*1000), "0.00.00");
    auto sDateFormat41 = std::make_unique<SimpleDateTimeFormat>("Hmsv", "ca", false, errCode);
    EXPECT_EQ(sDateFormat41->Format(timep*1000), "0.00.00 Hora de: Xina");
    auto sDateFormat42 = std::make_unique<SimpleDateTimeFormat>("Hmv", "ca", false, errCode);
    EXPECT_EQ(sDateFormat42->Format(timep*1000), "0.00 Hora de: Xina");
    auto sDateFormat43 = std::make_unique<SimpleDateTimeFormat>("hm", "ca", false, errCode);
    EXPECT_EQ(sDateFormat43->Format(timep*1000), "12.00 a. m.");
    auto sDateFormat44 = std::make_unique<SimpleDateTimeFormat>("hms", "ca", false, errCode);
    EXPECT_EQ(sDateFormat44->Format(timep*1000), "12.00.00 a. m.");
}

/**
 * @tc.name: CustomizedDateTimeFormatTest0003
 * @tc.desc: Test Customized Data
 * @tc.type: FUNC
 */
HWTEST_F(CustomizedDateTimeFormatTest, CustomizedDateTimeFormatTest0003, TestSize.Level1)
{
    I18nErrorCode errCode = I18nErrorCode::SUCCESS;
    struct tm tmObj = {.tm_mday = 23, .tm_mon = 3, .tm_year = 124};
    time_t timep = mktime(&tmObj);
    auto sDateFormat201 = std::make_unique<SimpleDateTimeFormat>("yMMMEd", "mr", false, errCode);
    EXPECT_EQ(sDateFormat201->Format(timep*1000), "मंगळ, २३ एप्रि, २०२४");
    auto sDateFormat202 = std::make_unique<SimpleDateTimeFormat>("MEd", "ms", false, errCode);
    EXPECT_EQ(sDateFormat202->Format(timep*1000), "Sel, 23/04");
    auto sDateFormat203 = std::make_unique<SimpleDateTimeFormat>("Md", "ms", false, errCode);
    EXPECT_EQ(sDateFormat203->Format(timep*1000), "23/04");
    auto sDateFormat204 = std::make_unique<SimpleDateTimeFormat>("yM", "ms", false, errCode);
    EXPECT_EQ(sDateFormat204->Format(timep*1000), "04/2024");
    auto sDateFormat205 = std::make_unique<SimpleDateTimeFormat>("yMEd", "ms", false, errCode);
    EXPECT_EQ(sDateFormat205->Format(timep*1000), "Sel, 23/04/2024");
    auto sDateFormat206 = std::make_unique<SimpleDateTimeFormat>("yMd", "ms", false, errCode);
    EXPECT_EQ(sDateFormat206->Format(timep*1000), "23/04/2024");
    auto sDateFormat207 = std::make_unique<SimpleDateTimeFormat>("Ed", "my", false, errCode);
    EXPECT_EQ(sDateFormat207->Format(timep*1000), "အင်္ဂါ၊ ၂၃");
    auto sDateFormat208 = std::make_unique<SimpleDateTimeFormat>("Ehm", "my", false, errCode);
    EXPECT_EQ(sDateFormat208->Format(timep*1000), "အင်္ဂါ ၁၂:၀၀ နံနက်");
    auto sDateFormat209 = std::make_unique<SimpleDateTimeFormat>("Ehms", "my", false, errCode);
    EXPECT_EQ(sDateFormat209->Format(timep*1000), "အင်္ဂါ ၁၂:၀၀:၀၀ နံနက်");
    auto sDateFormat210 = std::make_unique<SimpleDateTimeFormat>("MEd", "my", false, errCode);
    EXPECT_EQ(sDateFormat210->Format(timep*1000), "အင်္ဂါနေ့၊ ၂၃/ဧပြီ");
    auto sDateFormat211 = std::make_unique<SimpleDateTimeFormat>("MMMEd", "my", false, errCode);
    EXPECT_EQ(sDateFormat211->Format(timep*1000), "အင်္ဂါနေ့၊ ဧပြီလ ၂၃ ရက်");
    auto sDateFormat212 = std::make_unique<SimpleDateTimeFormat>("MMMMEd", "my", false, errCode);
    EXPECT_EQ(sDateFormat212->Format(timep*1000), "အင်္ဂါနေ့၊ ဧပြီလ ၂၃ ရက်");
    auto sDateFormat213 = std::make_unique<SimpleDateTimeFormat>("MMMd", "my", false, errCode);
    EXPECT_EQ(sDateFormat213->Format(timep*1000), "ဧပြီလ ၂၃ ရက်");
    auto sDateFormat214 = std::make_unique<SimpleDateTimeFormat>("Md", "my", false, errCode);
    EXPECT_EQ(sDateFormat214->Format(timep*1000), "၂၃/ဧပြီ");
    auto sDateFormat215 = std::make_unique<SimpleDateTimeFormat>("h", "my", false, errCode);
    EXPECT_EQ(sDateFormat215->Format(timep*1000), "၁၂ နံနက်");
    auto sDateFormat216 = std::make_unique<SimpleDateTimeFormat>("hm", "my", false, errCode);
    EXPECT_EQ(sDateFormat216->Format(timep*1000), "၁၂:၀၀ နံနက်");
    auto sDateFormat217 = std::make_unique<SimpleDateTimeFormat>("hms", "my", false, errCode);
    EXPECT_EQ(sDateFormat217->Format(timep*1000), "၁၂:၀၀:၀၀ နံနက်");
    auto sDateFormat218 = std::make_unique<SimpleDateTimeFormat>("hmsv", "my", false, errCode);
    EXPECT_EQ(sDateFormat218->Format(timep*1000), "၁၂:၀၀:၀၀ နံနက် တရုတ် အချိန်");
    auto sDateFormat219 = std::make_unique<SimpleDateTimeFormat>("hmv", "my", false, errCode);
    EXPECT_EQ(sDateFormat219->Format(timep*1000), "၁၂:၀၀ နံနက် တရုတ် အချိန်");
    auto sDateFormat220 = std::make_unique<SimpleDateTimeFormat>("yM", "my", false, errCode);
    EXPECT_EQ(sDateFormat220->Format(timep*1000), "ဧပြီ/၂၀၂၄");
    auto sDateFormat221 = std::make_unique<SimpleDateTimeFormat>("yMEd", "my", false, errCode);
    EXPECT_EQ(sDateFormat221->Format(timep*1000), "အင်္ဂါနေ့၊ ၂၃/ဧပြီ/၂၀၂၄");
    auto sDateFormat222 = std::make_unique<SimpleDateTimeFormat>("yMMM", "my", false, errCode);
    EXPECT_EQ(sDateFormat222->Format(timep*1000), "၂၀၂၄ ခုနှစ် ဧပြီလ");
}

/**
 * @tc.name: CustomizedDateTimeFormatTest0004
 * @tc.desc: Test Customized Data
 * @tc.type: FUNC
 */
HWTEST_F(CustomizedDateTimeFormatTest, CustomizedDateTimeFormatTest0004, TestSize.Level1)
{
    I18nErrorCode errCode = I18nErrorCode::SUCCESS;
    struct tm tmObj = {.tm_mday = 23, .tm_mon = 3, .tm_year = 124};
    time_t timep = mktime(&tmObj);
    auto sDateFormat45 = std::make_unique<SimpleDateTimeFormat>("hmsv", "ca", false, errCode);
    EXPECT_EQ(sDateFormat45->Format(timep*1000), "12.00.00 a. m. Hora de: Xina");
    auto sDateFormat46 = std::make_unique<SimpleDateTimeFormat>("hmsvvvv", "ca", false, errCode);
    EXPECT_EQ(sDateFormat46->Format(timep*1000), "12:00:00 a. m. (Hora estàndard de la Xina)");
    auto sDateFormat47 = std::make_unique<SimpleDateTimeFormat>("hmv", "ca", false, errCode);
    EXPECT_EQ(sDateFormat47->Format(timep*1000), "12.00 a. m. Hora de: Xina");
    auto sDateFormat48 = std::make_unique<SimpleDateTimeFormat>("yMMM", "ca", false, errCode);
    EXPECT_EQ(sDateFormat48->Format(timep*1000), "abr. 2024");
    auto sDateFormat49 = std::make_unique<SimpleDateTimeFormat>("yMMMd", "ca", false, errCode);
    EXPECT_EQ(sDateFormat49->Format(timep*1000), "23 abr. 2024");
    auto sDateFormat50 = std::make_unique<SimpleDateTimeFormat>("GyMMMEd", "cs", false, errCode);
    EXPECT_EQ(sDateFormat50->Format(timep*1000), "út 23. dub. 2024 n. l.");
    auto sDateFormat51 = std::make_unique<SimpleDateTimeFormat>("GyMMMd", "cs", false, errCode);
    EXPECT_EQ(sDateFormat51->Format(timep*1000), "23. dub 2024 n. l.");
    auto sDateFormat52 = std::make_unique<SimpleDateTimeFormat>("MEd", "cs", false, errCode);
    EXPECT_EQ(sDateFormat52->Format(timep*1000), "út 23.04.");
    auto sDateFormat53 = std::make_unique<SimpleDateTimeFormat>("MMMd", "cs", false, errCode);
    EXPECT_EQ(sDateFormat53->Format(timep*1000), "23. dub");
    auto sDateFormat54 = std::make_unique<SimpleDateTimeFormat>("Md", "cs", false, errCode);
    EXPECT_EQ(sDateFormat54->Format(timep*1000), "23.04.");
    auto sDateFormat55 = std::make_unique<SimpleDateTimeFormat>("yM", "cs", false, errCode);
    EXPECT_EQ(sDateFormat55->Format(timep*1000), "04.2024");
    auto sDateFormat56 = std::make_unique<SimpleDateTimeFormat>("yMEd", "cs", false, errCode);
    EXPECT_EQ(sDateFormat56->Format(timep*1000), "út 23.04.2024");
    auto sDateFormat57 = std::make_unique<SimpleDateTimeFormat>("yMMMEd", "cs", false, errCode);
    EXPECT_EQ(sDateFormat57->Format(timep*1000), "út 23. dub 2024");
    auto sDateFormat58 = std::make_unique<SimpleDateTimeFormat>("yMMMd", "cs", false, errCode);
    EXPECT_EQ(sDateFormat58->Format(timep*1000), "23. dub 2024");
    auto sDateFormat59 = std::make_unique<SimpleDateTimeFormat>("yMd", "cs", false, errCode);
    EXPECT_EQ(sDateFormat59->Format(timep*1000), "23.04.2024");
    auto sDateFormat60 = std::make_unique<SimpleDateTimeFormat>("Ed", "da", false, errCode);
    EXPECT_EQ(sDateFormat60->Format(timep*1000), "tirs. d. 23.");
    auto sDateFormat61 = std::make_unique<SimpleDateTimeFormat>("GyMMMEd", "da", false, errCode);
    EXPECT_EQ(sDateFormat61->Format(timep*1000), "tirs. d. 23. apr. 2024 e.Kr.");
    auto sDateFormat62 = std::make_unique<SimpleDateTimeFormat>("M", "da", false, errCode);
    EXPECT_EQ(sDateFormat62->Format(timep*1000), "04");
    auto sDateFormat63 = std::make_unique<SimpleDateTimeFormat>("MEd", "da", false, errCode);
    EXPECT_EQ(sDateFormat63->Format(timep*1000), "tirs. 23/04");
    auto sDateFormat64 = std::make_unique<SimpleDateTimeFormat>("MMMEd", "da", false, errCode);
    EXPECT_EQ(sDateFormat64->Format(timep*1000), "tirs. d. 23. apr.");
    auto sDateFormat65 = std::make_unique<SimpleDateTimeFormat>("MMMMEd", "da", false, errCode);
    EXPECT_EQ(sDateFormat65->Format(timep*1000), "tirs. d. 23. april");
}

/**
 * @tc.name: CustomizedDateTimeFormatTest0005
 * @tc.desc: Test Customized Data
 * @tc.type: FUNC
 */
HWTEST_F(CustomizedDateTimeFormatTest, CustomizedDateTimeFormatTest0005, TestSize.Level1)
{
    I18nErrorCode errCode = I18nErrorCode::SUCCESS;
    struct tm tmObj = {.tm_mday = 23, .tm_mon = 3, .tm_year = 124};
    time_t timep = mktime(&tmObj);
    auto sDateFormat68 = std::make_unique<SimpleDateTimeFormat>("yMEd", "da", false, errCode);
    EXPECT_EQ(sDateFormat68->Format(timep*1000), "tirs. 23/04/2024");
    auto sDateFormat69 = std::make_unique<SimpleDateTimeFormat>("yMMMEd", "da", false, errCode);
    EXPECT_EQ(sDateFormat69->Format(timep*1000), "tirs. d. 23. apr. 2024");
    auto sDateFormat70 = std::make_unique<SimpleDateTimeFormat>("yMd", "da", false, errCode);
    EXPECT_EQ(sDateFormat70->Format(timep*1000), "23/04/2024");
    auto sDateFormat71 = std::make_unique<SimpleDateTimeFormat>("MMM", "el", false, errCode);
    EXPECT_EQ(sDateFormat71->Format(timep*1000), "Απρ");
    auto sDateFormat72 = std::make_unique<SimpleDateTimeFormat>("yMMM", "el", false, errCode);
    EXPECT_EQ(sDateFormat72->Format(timep*1000), "Απρ 2024");
    auto sDateFormat73 = std::make_unique<SimpleDateTimeFormat>("MEd", "en-IE", false, errCode);
    EXPECT_EQ(sDateFormat73->Format(timep*1000), "Tue 23/04");
    auto sDateFormat74 = std::make_unique<SimpleDateTimeFormat>("Md", "en-IE", false, errCode);
    EXPECT_EQ(sDateFormat74->Format(timep*1000), "23/04");
    auto sDateFormat75 = std::make_unique<SimpleDateTimeFormat>("yMEd", "en-IE", false, errCode);
    EXPECT_EQ(sDateFormat75->Format(timep*1000), "Tue 23/04/2024");
    auto sDateFormat76 = std::make_unique<SimpleDateTimeFormat>("yMd", "en-IE", false, errCode);
    EXPECT_EQ(sDateFormat76->Format(timep*1000), "23/04/2024");
    auto sDateFormat77 = std::make_unique<SimpleDateTimeFormat>("Md", "en-NZ", false, errCode);
    EXPECT_EQ(sDateFormat77->Format(timep*1000), "23/04");
    auto sDateFormat78 = std::make_unique<SimpleDateTimeFormat>("yMMMEd", "en-ZW", false, errCode);
    EXPECT_EQ(sDateFormat78->Format(timep*1000), "Tue 23 Apr 2024");
    auto sDateFormat79 = std::make_unique<SimpleDateTimeFormat>("yMMMd", "en-ZW", false, errCode);
    EXPECT_EQ(sDateFormat79->Format(timep*1000), "23 Apr 2024");
    auto sDateFormat80 = std::make_unique<SimpleDateTimeFormat>("MMMdd", "es-419", false, errCode);
    EXPECT_EQ(sDateFormat80->Format(timep*1000), "23 abr");
    auto sDateFormat81 = std::make_unique<SimpleDateTimeFormat>("yMEd", "es-419", false, errCode);
    EXPECT_EQ(sDateFormat81->Format(timep*1000), "mar, 23/4/2024");
    auto sDateFormat82 = std::make_unique<SimpleDateTimeFormat>("yMMMEd", "es-419", false, errCode);
    EXPECT_EQ(sDateFormat82->Format(timep*1000), "mar, 23 abr 2024");
    auto sDateFormat83 = std::make_unique<SimpleDateTimeFormat>("MEd", "es-AR", false, errCode);
    EXPECT_EQ(sDateFormat83->Format(timep*1000), "mar 23/4");
    auto sDateFormat84 = std::make_unique<SimpleDateTimeFormat>("yM", "es-AR", false, errCode);
    EXPECT_EQ(sDateFormat84->Format(timep*1000), "4/2024");
    auto sDateFormat85 = std::make_unique<SimpleDateTimeFormat>("yMMMd", "es-AR", false, errCode);
    EXPECT_EQ(sDateFormat85->Format(timep*1000), "23 abr 2024");
    auto sDateFormat86 = std::make_unique<SimpleDateTimeFormat>("MMMEd", "es-CO", false, errCode);
    EXPECT_EQ(sDateFormat86->Format(timep*1000), "mar, 23 abr");
    auto sDateFormat87 = std::make_unique<SimpleDateTimeFormat>("MMMd", "es-CO", false, errCode);
    EXPECT_EQ(sDateFormat87->Format(timep*1000), "23 abr");
    auto sDateFormat88 = std::make_unique<SimpleDateTimeFormat>("MMMdd", "es-CO", false, errCode);
    EXPECT_EQ(sDateFormat88->Format(timep*1000), "23 abr");
    auto sDateFormat89 = std::make_unique<SimpleDateTimeFormat>("yMMM", "es-CO", false, errCode);
    EXPECT_EQ(sDateFormat89->Format(timep*1000), "abr 2024");
}

/**
 * @tc.name: CustomizedDateTimeFormatTest0006
 * @tc.desc: Test Customized Data
 * @tc.type: FUNC
 */
HWTEST_F(CustomizedDateTimeFormatTest, CustomizedDateTimeFormatTest0006, TestSize.Level1)
{
    I18nErrorCode errCode = I18nErrorCode::SUCCESS;
    struct tm tmObj = {.tm_mday = 23, .tm_mon = 3, .tm_year = 124};
    time_t timep = mktime(&tmObj);
    auto sDateFormat90 = std::make_unique<SimpleDateTimeFormat>("yMMMd", "es-CO", false, errCode);
    EXPECT_EQ(sDateFormat90->Format(timep*1000), "23 abr 2024");
    auto sDateFormat91 = std::make_unique<SimpleDateTimeFormat>("EHm", "es-MX", false, errCode);
    EXPECT_EQ(sDateFormat91->Format(timep*1000), "mar 0:00");
    auto sDateFormat92 = std::make_unique<SimpleDateTimeFormat>("EHms", "es-MX", false, errCode);
    EXPECT_EQ(sDateFormat92->Format(timep*1000), "mar 0:00:00");
    auto sDateFormat93 = std::make_unique<SimpleDateTimeFormat>("yMEd", "es-PA", false, errCode);
    EXPECT_EQ(sDateFormat93->Format(timep*1000), "mar, 23/04/2024");
    auto sDateFormat94 = std::make_unique<SimpleDateTimeFormat>("MMMEd", "eu", false, errCode);
    EXPECT_EQ(sDateFormat94->Format(timep*1000), "api. 23a, ar.");
    auto sDateFormat95 = std::make_unique<SimpleDateTimeFormat>("MMMMd", "eu", false, errCode);
    EXPECT_EQ(sDateFormat95->Format(timep*1000), "apirilaren 23a");
    auto sDateFormat96 = std::make_unique<SimpleDateTimeFormat>("MMMd", "eu", false, errCode);
    EXPECT_EQ(sDateFormat96->Format(timep*1000), "api. 23a");
    auto sDateFormat97 = std::make_unique<SimpleDateTimeFormat>("yMMMEd", "eu", false, errCode);
    EXPECT_EQ(sDateFormat97->Format(timep*1000), "2024 api. 23a, ar.");
    auto sDateFormat98 = std::make_unique<SimpleDateTimeFormat>("yMMMMEd", "eu", false, errCode);
    EXPECT_EQ(sDateFormat98->Format(timep*1000), "2024(e)ko apirilaren 23a, ar.");
    auto sDateFormat99 = std::make_unique<SimpleDateTimeFormat>("yMMMMd", "eu", false, errCode);
    EXPECT_EQ(sDateFormat99->Format(timep*1000), "2024(e)ko apirilaren 23a");
    auto sDateFormat100 = std::make_unique<SimpleDateTimeFormat>("yMMMd", "eu", false, errCode);
    EXPECT_EQ(sDateFormat100->Format(timep*1000), "2024 api. 23a");
    auto sDateFormat101 = std::make_unique<SimpleDateTimeFormat>("EHm", "fr-CA", false, errCode);
    EXPECT_EQ(sDateFormat101->Format(timep*1000), "mar. 00:00");
    auto sDateFormat102 = std::make_unique<SimpleDateTimeFormat>("EHms", "fr-CA", false, errCode);
    EXPECT_EQ(sDateFormat102->Format(timep*1000), "mar. 00:00:00");
    auto sDateFormat103 = std::make_unique<SimpleDateTimeFormat>("Ehm", "fr-CA", false, errCode);
    EXPECT_EQ(sDateFormat103->Format(timep*1000), "mar. 12:00 a.m.");
    auto sDateFormat104 = std::make_unique<SimpleDateTimeFormat>("Ehms", "fr-CA", false, errCode);
    EXPECT_EQ(sDateFormat104->Format(timep*1000), "mar. 12:00:00 a.m.");
    auto sDateFormat105 = std::make_unique<SimpleDateTimeFormat>("Hm", "fr-CA", false, errCode);
    EXPECT_EQ(sDateFormat105->Format(timep*1000), "00:00");
    auto sDateFormat106 = std::make_unique<SimpleDateTimeFormat>("Hms", "fr-CA", false, errCode);
    EXPECT_EQ(sDateFormat106->Format(timep*1000), "00:00:00");
    auto sDateFormat107 = std::make_unique<SimpleDateTimeFormat>("Hmsv", "fr-CA", false, errCode);
    EXPECT_EQ(sDateFormat107->Format(timep*1000), "00:00:00 heure : Chine");
    auto sDateFormat108 = std::make_unique<SimpleDateTimeFormat>("Hmv", "fr-CA", false, errCode);
    EXPECT_EQ(sDateFormat108->Format(timep*1000), "00:00 heure : Chine");
    auto sDateFormat109 = std::make_unique<SimpleDateTimeFormat>("h", "fr-CA", false, errCode);
    EXPECT_EQ(sDateFormat109->Format(timep*1000), "12 a.m.");
    auto sDateFormat110 = std::make_unique<SimpleDateTimeFormat>("hm", "fr-CA", false, errCode);
    EXPECT_EQ(sDateFormat110->Format(timep*1000), "12:00 a.m.");
    auto sDateFormat111 = std::make_unique<SimpleDateTimeFormat>("hms", "fr-CA", false, errCode);
    EXPECT_EQ(sDateFormat111->Format(timep*1000), "12:00:00 a.m.");
}

/**
 * @tc.name: CustomizedDateTimeFormatTest0007
 * @tc.desc: Test Customized Data
 * @tc.type: FUNC
 */
HWTEST_F(CustomizedDateTimeFormatTest, CustomizedDateTimeFormatTest0007, TestSize.Level1)
{
    I18nErrorCode errCode = I18nErrorCode::SUCCESS;
    struct tm tmObj = {.tm_mday = 23, .tm_mon = 3, .tm_year = 124};
    time_t timep = mktime(&tmObj);
    auto sDateFormat112 = std::make_unique<SimpleDateTimeFormat>("hmsv", "fr-CA", false, errCode);
    EXPECT_EQ(sDateFormat112->Format(timep*1000), "12:00:00 a.m. heure : Chine");
    auto sDateFormat113 = std::make_unique<SimpleDateTimeFormat>("hmv", "fr-CA", false, errCode);
    EXPECT_EQ(sDateFormat113->Format(timep*1000), "12:00 a.m. heure : Chine");
    auto sDateFormat114 = std::make_unique<SimpleDateTimeFormat>("ms", "fr-CA", false, errCode);
    EXPECT_EQ(sDateFormat114->Format(timep*1000), "00:00");
    auto sDateFormat115 = std::make_unique<SimpleDateTimeFormat>("GyMMMEd", "gl", false, errCode);
    EXPECT_EQ(sDateFormat115->Format(timep*1000), "mar, 23/04/2024 d.C.");
    auto sDateFormat116 = std::make_unique<SimpleDateTimeFormat>("MMMEd", "gl", false, errCode);
    EXPECT_EQ(sDateFormat116->Format(timep*1000), "mar, 23 abr");
    auto sDateFormat117 = std::make_unique<SimpleDateTimeFormat>("MMMMEd", "gl", false, errCode);
    EXPECT_EQ(sDateFormat117->Format(timep*1000), "mar, 23 de abril");
    auto sDateFormat118 = std::make_unique<SimpleDateTimeFormat>("MMMd", "gl", false, errCode);
    EXPECT_EQ(sDateFormat118->Format(timep*1000), "23 abr");
    auto sDateFormat119 = std::make_unique<SimpleDateTimeFormat>("yMEd", "gl", false, errCode);
    EXPECT_EQ(sDateFormat119->Format(timep*1000), "mar, 23/4/2024");
    auto sDateFormat120 = std::make_unique<SimpleDateTimeFormat>("yMMM", "gl", false, errCode);
    EXPECT_EQ(sDateFormat120->Format(timep*1000), "abr 2024");
    auto sDateFormat121 = std::make_unique<SimpleDateTimeFormat>("yMMMEd", "gl", false, errCode);
    EXPECT_EQ(sDateFormat121->Format(timep*1000), "mar, 23 abr 2024");
    auto sDateFormat122 = std::make_unique<SimpleDateTimeFormat>("yMMMM", "gl", false, errCode);
    EXPECT_EQ(sDateFormat122->Format(timep*1000), "abril de 2024");
    auto sDateFormat123 = std::make_unique<SimpleDateTimeFormat>("yMMMd", "gl", false, errCode);
    EXPECT_EQ(sDateFormat123->Format(timep*1000), "23 abr 2024");
    auto sDateFormat124 = std::make_unique<SimpleDateTimeFormat>("Md", "hu", false, errCode);
    EXPECT_EQ(sDateFormat124->Format(timep*1000), "04. 23.");
    auto sDateFormat125 = std::make_unique<SimpleDateTimeFormat>("yM", "hu", false, errCode);
    EXPECT_EQ(sDateFormat125->Format(timep*1000), "2024. 04.");
    auto sDateFormat126 = std::make_unique<SimpleDateTimeFormat>("MMMEd", "hy", false, errCode);
    EXPECT_EQ(sDateFormat126->Format(timep*1000), "23 ապր, երք");
    auto sDateFormat127 = std::make_unique<SimpleDateTimeFormat>("MMMd", "hy", false, errCode);
    EXPECT_EQ(sDateFormat127->Format(timep*1000), "23 ապր");
    auto sDateFormat128 = std::make_unique<SimpleDateTimeFormat>("yMEd", "hy", false, errCode);
    EXPECT_EQ(sDateFormat128->Format(timep*1000), "23.04.2024, երք");
    auto sDateFormat129 = std::make_unique<SimpleDateTimeFormat>("yMMM", "hy", false, errCode);
    EXPECT_EQ(sDateFormat129->Format(timep*1000), "ապր, 2024 թ.");
    auto sDateFormat130 = std::make_unique<SimpleDateTimeFormat>("yMMMEd", "hy", false, errCode);
    EXPECT_EQ(sDateFormat130->Format(timep*1000), "23 ապր, 2024 թ. երք");
    auto sDateFormat131 = std::make_unique<SimpleDateTimeFormat>("yMMMM", "hy", false, errCode);
    EXPECT_EQ(sDateFormat131->Format(timep*1000), "ապրիլի, 2024 թ.");
    auto sDateFormat132 = std::make_unique<SimpleDateTimeFormat>("yMMMd", "hy", false, errCode);
    EXPECT_EQ(sDateFormat132->Format(timep*1000), "23 ապր, 2024 թ.");
    auto sDateFormat133 = std::make_unique<SimpleDateTimeFormat>("Hmsv", "is", false, errCode);
    EXPECT_EQ(sDateFormat133->Format(timep*1000), "00:00:00 Kína");
}

/**
 * @tc.name: CustomizedDateTimeFormatTest0008
 * @tc.desc: Test Customized Data
 * @tc.type: FUNC
 */
HWTEST_F(CustomizedDateTimeFormatTest, CustomizedDateTimeFormatTest0008, TestSize.Level1)
{
    I18nErrorCode errCode = I18nErrorCode::SUCCESS;
    struct tm tmObj = {.tm_mday = 23, .tm_mon = 3, .tm_year = 124};
    time_t timep = mktime(&tmObj);
    auto sDateFormat134 = std::make_unique<SimpleDateTimeFormat>("Hmv", "is", false, errCode);
    EXPECT_EQ(sDateFormat134->Format(timep*1000), "00:00 Kína");
    auto sDateFormat135 = std::make_unique<SimpleDateTimeFormat>("MEEEEd", "ja", false, errCode);
    EXPECT_EQ(sDateFormat135->Format(timep*1000), "04/23火曜日");
    auto sDateFormat136 = std::make_unique<SimpleDateTimeFormat>("MEd", "ja", false, errCode);
    EXPECT_EQ(sDateFormat136->Format(timep*1000), "04/23(火)");
    auto sDateFormat137 = std::make_unique<SimpleDateTimeFormat>("yMEEEEd", "ja", false, errCode);
    EXPECT_EQ(sDateFormat137->Format(timep*1000), "2024/04/23火曜日");
    auto sDateFormat138 = std::make_unique<SimpleDateTimeFormat>("yMEd", "ja", false, errCode);
    EXPECT_EQ(sDateFormat138->Format(timep*1000), "2024/04/23(火)");
    auto sDateFormat139 = std::make_unique<SimpleDateTimeFormat>("Ed", "jv", false, errCode);
    EXPECT_EQ(sDateFormat139->Format(timep*1000), "23 Sel");
    auto sDateFormat140 = std::make_unique<SimpleDateTimeFormat>("GyMMMEd", "jv", false, errCode);
    EXPECT_EQ(sDateFormat140->Format(timep*1000), "Sel, 23 Apr, 2024 M");
    auto sDateFormat141 = std::make_unique<SimpleDateTimeFormat>("GyMMMd", "jv", false, errCode);
    EXPECT_EQ(sDateFormat141->Format(timep*1000), "23 Apr, 2024 M");
    auto sDateFormat142 = std::make_unique<SimpleDateTimeFormat>("MEd", "jv", false, errCode);
    EXPECT_EQ(sDateFormat142->Format(timep*1000), "Sel, 23/4");
    auto sDateFormat143 = std::make_unique<SimpleDateTimeFormat>("Md", "jv", false, errCode);
    EXPECT_EQ(sDateFormat143->Format(timep*1000), "23/4");
    auto sDateFormat144 = std::make_unique<SimpleDateTimeFormat>("yM", "jv", false, errCode);
    EXPECT_EQ(sDateFormat144->Format(timep*1000), "4/2024");
    auto sDateFormat145 = std::make_unique<SimpleDateTimeFormat>("yMEd", "jv", false, errCode);
    EXPECT_EQ(sDateFormat145->Format(timep*1000), "Sel, 23/4/2024");
    auto sDateFormat146 = std::make_unique<SimpleDateTimeFormat>("yMMMEd", "jv", false, errCode);
    EXPECT_EQ(sDateFormat146->Format(timep*1000), "Sel, 23 Apr, 2024");
    auto sDateFormat147 = std::make_unique<SimpleDateTimeFormat>("yMMMd", "jv", false, errCode);
    EXPECT_EQ(sDateFormat147->Format(timep*1000), "23 Apr, 2024");
    auto sDateFormat148 = std::make_unique<SimpleDateTimeFormat>("yMd", "jv", false, errCode);
    EXPECT_EQ(sDateFormat148->Format(timep*1000), "23/4/2024");
    auto sDateFormat149 = std::make_unique<SimpleDateTimeFormat>("yMMM", "ka", false, errCode);
    EXPECT_EQ(sDateFormat149->Format(timep*1000), "აპრ, 2024");
    auto sDateFormat150 = std::make_unique<SimpleDateTimeFormat>("yMMMEd", "ka", false, errCode);
    EXPECT_EQ(sDateFormat150->Format(timep*1000), "სამ, 23 აპრ, 2024");
    auto sDateFormat151 = std::make_unique<SimpleDateTimeFormat>("yMMMd", "ka", false, errCode);
    EXPECT_EQ(sDateFormat151->Format(timep*1000), "23 აპრ, 2024");
    auto sDateFormat152 = std::make_unique<SimpleDateTimeFormat>("Gy", "km", false, errCode);
    EXPECT_EQ(sDateFormat152->Format(timep*1000), "2024 នៃ គ.ស.");
    auto sDateFormat153 = std::make_unique<SimpleDateTimeFormat>("GyMMM", "km", false, errCode);
    EXPECT_EQ(sDateFormat153->Format(timep*1000), "ខែមេសា 2024 នៃ គ.ស.");
    auto sDateFormat154 = std::make_unique<SimpleDateTimeFormat>("GyMMMEd", "km", false, errCode);
    EXPECT_EQ(sDateFormat154->Format(timep*1000), "អង្គារ 23 ខែមេសា 2024 នៃ គ.ស.");
    auto sDateFormat155 = std::make_unique<SimpleDateTimeFormat>("GyMMMd", "km", false, errCode);
    EXPECT_EQ(sDateFormat155->Format(timep*1000), "23 ខែមេសា 2024 នៃ គ.ស.");
}

/**
 * @tc.name: CustomizedDateTimeFormatTest0009
 * @tc.desc: Test Customized Data
 * @tc.type: FUNC
 */
HWTEST_F(CustomizedDateTimeFormatTest, CustomizedDateTimeFormatTest0009, TestSize.Level1)
{
    I18nErrorCode errCode = I18nErrorCode::SUCCESS;
    struct tm tmObj = {.tm_mday = 23, .tm_mon = 3, .tm_year = 124};
    time_t timep = mktime(&tmObj);
    auto sDateFormat156 = std::make_unique<SimpleDateTimeFormat>("yMMMM", "km", false, errCode);
    EXPECT_EQ(sDateFormat156->Format(timep*1000), "ខែមេសា ឆ្នាំ2024");
    auto sDateFormat157 = std::make_unique<SimpleDateTimeFormat>("MMMEd", "kn", false, errCode);
    EXPECT_EQ(sDateFormat157->Format(timep*1000), "23 ಏಪ್ರಿ, ಮಂಗಳ");
    auto sDateFormat158 = std::make_unique<SimpleDateTimeFormat>("MMdd", "kn", false, errCode);
    EXPECT_EQ(sDateFormat158->Format(timep*1000), "23/04");
    auto sDateFormat159 = std::make_unique<SimpleDateTimeFormat>("yMEd", "kn", false, errCode);
    EXPECT_EQ(sDateFormat159->Format(timep*1000), "23/4/2024, ಮಂಗಳ");
    auto sDateFormat160 = std::make_unique<SimpleDateTimeFormat>("yMM", "kn", false, errCode);
    EXPECT_EQ(sDateFormat160->Format(timep*1000), "4/2024");
    auto sDateFormat161 = std::make_unique<SimpleDateTimeFormat>("yMMMEd", "kn", false, errCode);
    EXPECT_EQ(sDateFormat161->Format(timep*1000), "23 ಏಪ್ರಿ 2024, ಮಂಗಳ");
    auto sDateFormat162 = std::make_unique<SimpleDateTimeFormat>("yMMMd", "kn", false, errCode);
    EXPECT_EQ(sDateFormat162->Format(timep*1000), "23 ಏಪ್ರಿ 2024");
    auto sDateFormat163 = std::make_unique<SimpleDateTimeFormat>("Hms", "ko", false, errCode);
    EXPECT_EQ(sDateFormat163->Format(timep*1000), "00:00:00");
    auto sDateFormat164 = std::make_unique<SimpleDateTimeFormat>("MEd", "lo", false, errCode);
    EXPECT_EQ(sDateFormat164->Format(timep*1000), "ຄານ 23/4");
    auto sDateFormat165 = std::make_unique<SimpleDateTimeFormat>("yMEd", "lo", false, errCode);
    EXPECT_EQ(sDateFormat165->Format(timep*1000), "ຄານ 23/4/2024");
    auto sDateFormat166 = std::make_unique<SimpleDateTimeFormat>("yMMMEd", "lo", false, errCode);
    EXPECT_EQ(sDateFormat166->Format(timep*1000), "ຄານ 23 ມ.ສ. 2024");
    auto sDateFormat167 = std::make_unique<SimpleDateTimeFormat>("MMM", "lt", false, errCode);
    EXPECT_EQ(sDateFormat167->Format(timep*1000), "bal.");
    auto sDateFormat168 = std::make_unique<SimpleDateTimeFormat>("MMMEd", "lt", false, errCode);
    EXPECT_EQ(sDateFormat168->Format(timep*1000), "bal. 23 d., an");
    auto sDateFormat169 = std::make_unique<SimpleDateTimeFormat>("MMMd", "lt", false, errCode);
    EXPECT_EQ(sDateFormat169->Format(timep*1000), "bal. 23 d.");
    auto sDateFormat170 = std::make_unique<SimpleDateTimeFormat>("Md", "lt", false, errCode);
    EXPECT_EQ(sDateFormat170->Format(timep*1000), "04-23");
    auto sDateFormat171 = std::make_unique<SimpleDateTimeFormat>("yMMM", "lt", false, errCode);
    EXPECT_EQ(sDateFormat171->Format(timep*1000), "2024 m. bal.");
    auto sDateFormat172 = std::make_unique<SimpleDateTimeFormat>("yMMMEd", "lt", false, errCode);
    EXPECT_EQ(sDateFormat172->Format(timep*1000), "2024 m. bal. 23 d., an");
    auto sDateFormat173 = std::make_unique<SimpleDateTimeFormat>("yMMMd", "lt", false, errCode);
    EXPECT_EQ(sDateFormat173->Format(timep*1000), "2024 m. bal. 23 d.");
    auto sDateFormat174 = std::make_unique<SimpleDateTimeFormat>("y", "lv", false, errCode);
    EXPECT_EQ(sDateFormat174->Format(timep*1000), "2024. gada");
    auto sDateFormat175 = std::make_unique<SimpleDateTimeFormat>("yMEd", "lv", false, errCode);
    EXPECT_EQ(sDateFormat175->Format(timep*1000), "otrd., 23.04.2024.");
    auto sDateFormat176 = std::make_unique<SimpleDateTimeFormat>("yMMM", "lv", false, errCode);
    EXPECT_EQ(sDateFormat176->Format(timep*1000), "2024. gada apr.");
    auto sDateFormat177 = std::make_unique<SimpleDateTimeFormat>("yMMMEd", "lv", false, errCode);
    EXPECT_EQ(sDateFormat177->Format(timep*1000), "otrd., 2024. gada 23. apr.");
}

/**
 * @tc.name: CustomizedDateTimeFormatTest0010
 * @tc.desc: Test Customized Data
 * @tc.type: FUNC
 */
HWTEST_F(CustomizedDateTimeFormatTest, CustomizedDateTimeFormatTest0010, TestSize.Level1)
{
    I18nErrorCode errCode = I18nErrorCode::SUCCESS;
    struct tm tmObj = {.tm_mday = 23, .tm_mon = 3, .tm_year = 124};
    time_t timep = mktime(&tmObj);
    auto sDateFormat178 = std::make_unique<SimpleDateTimeFormat>("yMMMM", "lv", false, errCode);
    EXPECT_EQ(sDateFormat178->Format(timep*1000), "2024. gada aprīlis");
    auto sDateFormat179 = std::make_unique<SimpleDateTimeFormat>("yMMMd", "lv", false, errCode);
    EXPECT_EQ(sDateFormat179->Format(timep*1000), "2024. gada 23. apr.");
    auto sDateFormat180 = std::make_unique<SimpleDateTimeFormat>("yMd", "lv", false, errCode);
    EXPECT_EQ(sDateFormat180->Format(timep*1000), "23.04.2024.");
    auto sDateFormat181 = std::make_unique<SimpleDateTimeFormat>("Hmsv", "mn", false, errCode);
    EXPECT_EQ(sDateFormat181->Format(timep*1000), "00:00:00 Хятад-н цаг");
    auto sDateFormat182 = std::make_unique<SimpleDateTimeFormat>("Hmv", "mn", false, errCode);
    EXPECT_EQ(sDateFormat182->Format(timep*1000), "00:00 Хятад-н цаг");
    auto sDateFormat183 = std::make_unique<SimpleDateTimeFormat>("M", "mn", false, errCode);
    EXPECT_EQ(sDateFormat183->Format(timep*1000), "4");
    auto sDateFormat184 = std::make_unique<SimpleDateTimeFormat>("MEd", "mn", false, errCode);
    EXPECT_EQ(sDateFormat184->Format(timep*1000), "4-23, Мя");
    auto sDateFormat185 = std::make_unique<SimpleDateTimeFormat>("MMMEd", "mn", false, errCode);
    EXPECT_EQ(sDateFormat185->Format(timep*1000), "4-р сар 23, Мя");
    auto sDateFormat186 = std::make_unique<SimpleDateTimeFormat>("MMMMd", "mn", false, errCode);
    EXPECT_EQ(sDateFormat186->Format(timep*1000), "Дөрөвдүгээр сарын 23 өдөр");
    auto sDateFormat187 = std::make_unique<SimpleDateTimeFormat>("MMMd", "mn", false, errCode);
    EXPECT_EQ(sDateFormat187->Format(timep*1000), "4-р сар 23");
    auto sDateFormat188 = std::make_unique<SimpleDateTimeFormat>("Md", "mn", false, errCode);
    EXPECT_EQ(sDateFormat188->Format(timep*1000), "4/23");
    auto sDateFormat189 = std::make_unique<SimpleDateTimeFormat>("h", "mn", false, errCode);
    EXPECT_EQ(sDateFormat189->Format(timep*1000), "12 ү.ө.");
    auto sDateFormat190 = std::make_unique<SimpleDateTimeFormat>("hmsv", "mn", false, errCode);
    EXPECT_EQ(sDateFormat190->Format(timep*1000), "12:00:00 ү.ө. Хятад-н цаг");
    auto sDateFormat191 = std::make_unique<SimpleDateTimeFormat>("hmv", "mn", false, errCode);
    EXPECT_EQ(sDateFormat191->Format(timep*1000), "12:00 ү.ө. Хятад-н цаг");
    auto sDateFormat192 = std::make_unique<SimpleDateTimeFormat>("yM", "mn", false, errCode);
    EXPECT_EQ(sDateFormat192->Format(timep*1000), "2024-4");
    auto sDateFormat193 = std::make_unique<SimpleDateTimeFormat>("yMEd", "mn", false, errCode);
    EXPECT_EQ(sDateFormat193->Format(timep*1000), "2024-4-23, Мя");
    auto sDateFormat194 = std::make_unique<SimpleDateTimeFormat>("yMMM", "mn", false, errCode);
    EXPECT_EQ(sDateFormat194->Format(timep*1000), "2024 оны 4-р сар");
    auto sDateFormat195 = std::make_unique<SimpleDateTimeFormat>("yMMMEd", "mn", false, errCode);
    EXPECT_EQ(sDateFormat195->Format(timep*1000), "2024 оны 4-р сар 23, Мя");
    auto sDateFormat196 = std::make_unique<SimpleDateTimeFormat>("yMMMM", "mn", false, errCode);
    EXPECT_EQ(sDateFormat196->Format(timep*1000), "2024 оны Дөрөвдүгээр сар");
    auto sDateFormat197 = std::make_unique<SimpleDateTimeFormat>("yMMMd", "mn", false, errCode);
    EXPECT_EQ(sDateFormat197->Format(timep*1000), "2024 оны 4-р сар 23");
    auto sDateFormat198 = std::make_unique<SimpleDateTimeFormat>("yMd", "mn", false, errCode);
    EXPECT_EQ(sDateFormat198->Format(timep*1000), "2024-4-23");
}

/**
 * @tc.name: CustomizedDateTimeFormatTest0011
 * @tc.desc: Test Customized Data
 * @tc.type: FUNC
 */
HWTEST_F(CustomizedDateTimeFormatTest, CustomizedDateTimeFormatTest0011, TestSize.Level1)
{
    I18nErrorCode errCode = I18nErrorCode::SUCCESS;
    struct tm tmObj = {.tm_mday = 23, .tm_mon = 3, .tm_year = 124};
    time_t timep = mktime(&tmObj);
    auto sDateFormat223 = std::make_unique<SimpleDateTimeFormat>("yMMMEd", "my", false, errCode);
    EXPECT_EQ(sDateFormat223->Format(timep*1000), "အင်္ဂါနေ့၊ ၂၀၂၄ ခုနှစ် ဧပြီလ ၂၃ ရက်");
    auto sDateFormat224 = std::make_unique<SimpleDateTimeFormat>("yMMMd", "my", false, errCode);
    EXPECT_EQ(sDateFormat224->Format(timep*1000), "၂၀၂၄ ခုနှစ် ဧပြီလ ၂၃ ရက်");
    auto sDateFormat225 = std::make_unique<SimpleDateTimeFormat>("yMd", "my", false, errCode);
    EXPECT_EQ(sDateFormat225->Format(timep*1000), "၂၃/ဧပြီ/၂၀၂၄");
    auto sDateFormat226 = std::make_unique<SimpleDateTimeFormat>("MEd", "nl", false, errCode);
    EXPECT_EQ(sDateFormat226->Format(timep*1000), "di 23-04");
    auto sDateFormat227 = std::make_unique<SimpleDateTimeFormat>("Md", "nl", false, errCode);
    EXPECT_EQ(sDateFormat227->Format(timep*1000), "23-04");
    auto sDateFormat228 = std::make_unique<SimpleDateTimeFormat>("yM", "nl", false, errCode);
    EXPECT_EQ(sDateFormat228->Format(timep*1000), "04-2024");
    auto sDateFormat229 = std::make_unique<SimpleDateTimeFormat>("yMEd", "nl", false, errCode);
    EXPECT_EQ(sDateFormat229->Format(timep*1000), "di 23-04-2024");
    auto sDateFormat230 = std::make_unique<SimpleDateTimeFormat>("yMd", "nl", false, errCode);
    EXPECT_EQ(sDateFormat230->Format(timep*1000), "23-04-2024");
    auto sDateFormat231 = std::make_unique<SimpleDateTimeFormat>("MEd", "nl-BE", false, errCode);
    EXPECT_EQ(sDateFormat231->Format(timep*1000), "di 23/04");
    auto sDateFormat232 = std::make_unique<SimpleDateTimeFormat>("Md", "nl-BE", false, errCode);
    EXPECT_EQ(sDateFormat232->Format(timep*1000), "23/04");
    auto sDateFormat233 = std::make_unique<SimpleDateTimeFormat>("yM", "nl-BE", false, errCode);
    EXPECT_EQ(sDateFormat233->Format(timep*1000), "04/2024");
    auto sDateFormat234 = std::make_unique<SimpleDateTimeFormat>("yMEd", "nl-BE", false, errCode);
    EXPECT_EQ(sDateFormat234->Format(timep*1000), "di 23-04-24");
    auto sDateFormat235 = std::make_unique<SimpleDateTimeFormat>("yMd", "nl-BE", false, errCode);
    EXPECT_EQ(sDateFormat235->Format(timep*1000), "23/04/24");
    auto sDateFormat236 = std::make_unique<SimpleDateTimeFormat>("MEd", "no", false, errCode);
    EXPECT_EQ(sDateFormat236->Format(timep*1000), "tir. 23.04.");
    auto sDateFormat237 = std::make_unique<SimpleDateTimeFormat>("MMdd", "no", false, errCode);
    EXPECT_EQ(sDateFormat237->Format(timep*1000), "23.04.");
    auto sDateFormat238 = std::make_unique<SimpleDateTimeFormat>("Md", "no", false, errCode);
    EXPECT_EQ(sDateFormat238->Format(timep*1000), "23.04.");
    auto sDateFormat239 = std::make_unique<SimpleDateTimeFormat>("yM", "no", false, errCode);
    EXPECT_EQ(sDateFormat239->Format(timep*1000), "04.2024");
    auto sDateFormat240 = std::make_unique<SimpleDateTimeFormat>("yMEd", "no", false, errCode);
    EXPECT_EQ(sDateFormat240->Format(timep*1000), "tir. 23.04.2024");
    auto sDateFormat241 = std::make_unique<SimpleDateTimeFormat>("yMd", "no", false, errCode);
    EXPECT_EQ(sDateFormat241->Format(timep*1000), "23.04.2024");
    auto sDateFormat242 = std::make_unique<SimpleDateTimeFormat>("MEd", "or", false, errCode);
    EXPECT_EQ(sDateFormat242->Format(timep*1000), "ମଙ୍ଗଳ, 23-4");
    auto sDateFormat243 = std::make_unique<SimpleDateTimeFormat>("MMMEd", "or", false, errCode);
    EXPECT_EQ(sDateFormat243->Format(timep*1000), "ମଙ୍ଗଳ, 23 ଅପ୍ରେଲ");
    auto sDateFormat244 = std::make_unique<SimpleDateTimeFormat>("Md", "or", false, errCode);
    EXPECT_EQ(sDateFormat244->Format(timep*1000), "23-4");
}

/**
 * @tc.name: CustomizedDateTimeFormatTest0012
 * @tc.desc: Test Customized Data
 * @tc.type: FUNC
 */
HWTEST_F(CustomizedDateTimeFormatTest, CustomizedDateTimeFormatTest0012, TestSize.Level1)
{
    I18nErrorCode errCode = I18nErrorCode::SUCCESS;
    struct tm tmObj = {.tm_mday = 23, .tm_mon = 3, .tm_year = 124};
    time_t timep = mktime(&tmObj);
    auto sDateFormat245 = std::make_unique<SimpleDateTimeFormat>("yM", "or", false, errCode);
    EXPECT_EQ(sDateFormat245->Format(timep*1000), "4-2024");
    auto sDateFormat246 = std::make_unique<SimpleDateTimeFormat>("yMEd", "or", false, errCode);
    EXPECT_EQ(sDateFormat246->Format(timep*1000), "ମଙ୍ଗଳ, 23-4-2024");
    auto sDateFormat247 = std::make_unique<SimpleDateTimeFormat>("yMMMEd", "or", false, errCode);
    EXPECT_EQ(sDateFormat247->Format(timep*1000), "ମଙ୍ଗଳ, 23 ଅପ୍ରେଲ 2024");
    auto sDateFormat248 = std::make_unique<SimpleDateTimeFormat>("yMMMd", "or", false, errCode);
    EXPECT_EQ(sDateFormat248->Format(timep*1000), "23 ଅପ୍ରେଲ 2024");
    auto sDateFormat249 = std::make_unique<SimpleDateTimeFormat>("yMd", "or", false, errCode);
    EXPECT_EQ(sDateFormat249->Format(timep*1000), "23-4-2024");
    auto sDateFormat250 = std::make_unique<SimpleDateTimeFormat>("MEd", "pa", false, errCode);
    EXPECT_EQ(sDateFormat250->Format(timep*1000), "ਮੰਗਲ, 23/4");
    auto sDateFormat251 = std::make_unique<SimpleDateTimeFormat>("MMMd", "pt-PT", false, errCode);
    EXPECT_EQ(sDateFormat251->Format(timep*1000), "23 de abr.");
    auto sDateFormat252 = std::make_unique<SimpleDateTimeFormat>("yMMM", "pt-PT", false, errCode);
    EXPECT_EQ(sDateFormat252->Format(timep*1000), "abr. de 2024");
    auto sDateFormat253 = std::make_unique<SimpleDateTimeFormat>("yMMMEEEEd", "pt-PT", false, errCode);
    EXPECT_EQ(sDateFormat253->Format(timep*1000), "terça-feira, 23 de abr. de 2024");
    auto sDateFormat254 = std::make_unique<SimpleDateTimeFormat>("yMMMEd", "pt-PT", false, errCode);
    EXPECT_EQ(sDateFormat254->Format(timep*1000), "ter., 23 de abr. de 2024");
    auto sDateFormat255 = std::make_unique<SimpleDateTimeFormat>("yMMMd", "pt-PT", false, errCode);
    EXPECT_EQ(sDateFormat255->Format(timep*1000), "23 de abr. de 2024");
    auto sDateFormat256 = std::make_unique<SimpleDateTimeFormat>("MEd", "si", false, errCode);
    EXPECT_EQ(sDateFormat256->Format(timep*1000), "04-23, අඟ");
    auto sDateFormat257 = std::make_unique<SimpleDateTimeFormat>("MMMEd", "si", false, errCode);
    EXPECT_EQ(sDateFormat257->Format(timep*1000), "අප්‍රේල් 23, අඟ");
    auto sDateFormat258 = std::make_unique<SimpleDateTimeFormat>("Md", "si", false, errCode);
    EXPECT_EQ(sDateFormat258->Format(timep*1000), "04-23");
    auto sDateFormat259 = std::make_unique<SimpleDateTimeFormat>("yM", "si", false, errCode);
    EXPECT_EQ(sDateFormat259->Format(timep*1000), "2024-04");
    auto sDateFormat260 = std::make_unique<SimpleDateTimeFormat>("yMEd", "si", false, errCode);
    EXPECT_EQ(sDateFormat260->Format(timep*1000), "2024-04-23, අඟ");
    auto sDateFormat261 = std::make_unique<SimpleDateTimeFormat>("yMd", "si", false, errCode);
    EXPECT_EQ(sDateFormat261->Format(timep*1000), "2024-04-23");
    auto sDateFormat262 = std::make_unique<SimpleDateTimeFormat>("GyMMM", "sk", false, errCode);
    EXPECT_EQ(sDateFormat262->Format(timep*1000), "apr 2024 po Kr.");
    auto sDateFormat263 = std::make_unique<SimpleDateTimeFormat>("GyMMMEd", "sk", false, errCode);
    EXPECT_EQ(sDateFormat263->Format(timep*1000), "ut, 23. apr 2024 po Kr.");
    auto sDateFormat264 = std::make_unique<SimpleDateTimeFormat>("GyMMMMd", "sk", false, errCode);
    EXPECT_EQ(sDateFormat264->Format(timep*1000), "23. apríla 2024 po Kr.");
    auto sDateFormat265 = std::make_unique<SimpleDateTimeFormat>("GyMMMd", "sk", false, errCode);
    EXPECT_EQ(sDateFormat265->Format(timep*1000), "23. apr 2024 po Kr.");
    auto sDateFormat266 = std::make_unique<SimpleDateTimeFormat>("MMMEd", "sk", false, errCode);
    EXPECT_EQ(sDateFormat266->Format(timep*1000), "ut, 23. apr");
}

/**
 * @tc.name: CustomizedDateTimeFormatTest0013
 * @tc.desc: Test Customized Data
 * @tc.type: FUNC
 */
HWTEST_F(CustomizedDateTimeFormatTest, CustomizedDateTimeFormatTest0013, TestSize.Level1)
{
    I18nErrorCode errCode = I18nErrorCode::SUCCESS;
    struct tm tmObj = {.tm_mday = 23, .tm_mon = 3, .tm_year = 124};
    time_t timep = mktime(&tmObj);
    auto sDateFormat267 = std::make_unique<SimpleDateTimeFormat>("MMMMEd", "sk", false, errCode);
    EXPECT_EQ(sDateFormat267->Format(timep*1000), "ut, 23. apríla");
    auto sDateFormat268 = std::make_unique<SimpleDateTimeFormat>("MMMd", "sk", false, errCode);
    EXPECT_EQ(sDateFormat268->Format(timep*1000), "23. apr");
    auto sDateFormat269 = std::make_unique<SimpleDateTimeFormat>("yM", "sk", false, errCode);
    EXPECT_EQ(sDateFormat269->Format(timep*1000), "4. 2024");
    auto sDateFormat270 = std::make_unique<SimpleDateTimeFormat>("yMMM", "sk", false, errCode);
    EXPECT_EQ(sDateFormat270->Format(timep*1000), "apr 2024");
    auto sDateFormat271 = std::make_unique<SimpleDateTimeFormat>("yMMMEd", "sk", false, errCode);
    EXPECT_EQ(sDateFormat271->Format(timep*1000), "ut, 23. apr 2024");
    auto sDateFormat272 = std::make_unique<SimpleDateTimeFormat>("yMMMd", "sk", false, errCode);
    EXPECT_EQ(sDateFormat272->Format(timep*1000), "23. apr 2024");
    auto sDateFormat273 = std::make_unique<SimpleDateTimeFormat>("yM", "sl", false, errCode);
    EXPECT_EQ(sDateFormat273->Format(timep*1000), "4. 2024");
    auto sDateFormat274 = std::make_unique<SimpleDateTimeFormat>("EHm", "sq", false, errCode);
    EXPECT_EQ(sDateFormat274->Format(timep*1000), "Mar, 00:00");
    auto sDateFormat275 = std::make_unique<SimpleDateTimeFormat>("EHms", "sq", false, errCode);
    EXPECT_EQ(sDateFormat275->Format(timep*1000), "Mar, 00:00:00");
    auto sDateFormat276 = std::make_unique<SimpleDateTimeFormat>("Ed", "sq", false, errCode);
    EXPECT_EQ(sDateFormat276->Format(timep*1000), "Mar, 23");
    auto sDateFormat277 = std::make_unique<SimpleDateTimeFormat>("Ehm", "sq", false, errCode);
    EXPECT_EQ(sDateFormat277->Format(timep*1000), "Mar, 12:00 e paradites");
    auto sDateFormat278 = std::make_unique<SimpleDateTimeFormat>("Ehms", "sq", false, errCode);
    EXPECT_EQ(sDateFormat278->Format(timep*1000), "Mar, 12:00:00 e paradites");
    auto sDateFormat279 = std::make_unique<SimpleDateTimeFormat>("GyMMM", "sq", false, errCode);
    EXPECT_EQ(sDateFormat279->Format(timep*1000), "Prill 2024 mb.K.");
    auto sDateFormat280 = std::make_unique<SimpleDateTimeFormat>("GyMMMEd", "sq", false, errCode);
    EXPECT_EQ(sDateFormat280->Format(timep*1000), "Mar, 23 Prill 2024 mb.K.");
    auto sDateFormat281 = std::make_unique<SimpleDateTimeFormat>("GyMMMd", "sq", false, errCode);
    EXPECT_EQ(sDateFormat281->Format(timep*1000), "23 Prill 2024 mb.K.");
    auto sDateFormat282 = std::make_unique<SimpleDateTimeFormat>("MEd", "sq", false, errCode);
    EXPECT_EQ(sDateFormat282->Format(timep*1000), "Mar, 23.4");
    auto sDateFormat283 = std::make_unique<SimpleDateTimeFormat>("MMMEd", "sq", false, errCode);
    EXPECT_EQ(sDateFormat283->Format(timep*1000), "Mar, 23 Prill");
    auto sDateFormat284 = std::make_unique<SimpleDateTimeFormat>("MMMMEd", "sq", false, errCode);
    EXPECT_EQ(sDateFormat284->Format(timep*1000), "Mar, 23 Prill");
    auto sDateFormat285 = std::make_unique<SimpleDateTimeFormat>("MMMd", "sq", false, errCode);
    EXPECT_EQ(sDateFormat285->Format(timep*1000), "23 Prill");
    auto sDateFormat286 = std::make_unique<SimpleDateTimeFormat>("yMEd", "sq", false, errCode);
    EXPECT_EQ(sDateFormat286->Format(timep*1000), "Mar, 23.4.2024");
    auto sDateFormat287 = std::make_unique<SimpleDateTimeFormat>("yMMM", "sq", false, errCode);
    EXPECT_EQ(sDateFormat287->Format(timep*1000), "Prill 2024");
    auto sDateFormat288 = std::make_unique<SimpleDateTimeFormat>("yMMMEd", "sq", false, errCode);
    EXPECT_EQ(sDateFormat288->Format(timep*1000), "Mar, 23 Prill 2024");
}

/**
 * @tc.name: CustomizedDateTimeFormatTest0014
 * @tc.desc: Test Customized Data
 * @tc.type: FUNC
 */
HWTEST_F(CustomizedDateTimeFormatTest, CustomizedDateTimeFormatTest0014, TestSize.Level1)
{
    I18nErrorCode errCode = I18nErrorCode::SUCCESS;
    struct tm tmObj = {.tm_mday = 23, .tm_mon = 3, .tm_year = 124};
    time_t timep = mktime(&tmObj);
    auto sDateFormat289 = std::make_unique<SimpleDateTimeFormat>("yMMMd", "sq", false, errCode);
    EXPECT_EQ(sDateFormat289->Format(timep*1000), "23 Prill 2024");
    auto sDateFormat290 = std::make_unique<SimpleDateTimeFormat>("GyMMMEd", "sr-Latn", false, errCode);
    EXPECT_EQ(sDateFormat290->Format(timep*1000), "uto, 23. apr 2024. n. e.");
    auto sDateFormat291 = std::make_unique<SimpleDateTimeFormat>("GyMMMd", "sr-Latn", false, errCode);
    EXPECT_EQ(sDateFormat291->Format(timep*1000), "23. apr 2024. n. e.");
    auto sDateFormat292 = std::make_unique<SimpleDateTimeFormat>("MEd", "sv", false, errCode);
    EXPECT_EQ(sDateFormat292->Format(timep*1000), "tis 04-23");
    auto sDateFormat293 = std::make_unique<SimpleDateTimeFormat>("MMd", "sv", false, errCode);
    EXPECT_EQ(sDateFormat293->Format(timep*1000), "04-23");
    auto sDateFormat294 = std::make_unique<SimpleDateTimeFormat>("MMdd", "sv", false, errCode);
    EXPECT_EQ(sDateFormat294->Format(timep*1000), "04-23");
    auto sDateFormat295 = std::make_unique<SimpleDateTimeFormat>("Md", "sv", false, errCode);
    EXPECT_EQ(sDateFormat295->Format(timep*1000), "04-23");
    auto sDateFormat296 = std::make_unique<SimpleDateTimeFormat>("yMEd", "sv", false, errCode);
    EXPECT_EQ(sDateFormat296->Format(timep*1000), "tis 2024-04-23");
    auto sDateFormat297 = std::make_unique<SimpleDateTimeFormat>("MEd", "ta", false, errCode);
    EXPECT_EQ(sDateFormat297->Format(timep*1000), "செவ்., 23/4");
    auto sDateFormat298 = std::make_unique<SimpleDateTimeFormat>("yMM", "ta", false, errCode);
    EXPECT_EQ(sDateFormat298->Format(timep*1000), "4/2024");
    auto sDateFormat299 = std::make_unique<SimpleDateTimeFormat>("MEd", "te", false, errCode);
    EXPECT_EQ(sDateFormat299->Format(timep*1000), "మంగళ, 23-04");
    auto sDateFormat300 = std::make_unique<SimpleDateTimeFormat>("MMMEd", "te", false, errCode);
    EXPECT_EQ(sDateFormat300->Format(timep*1000), "మంగళ, 23 ఏప్రి");
    auto sDateFormat301 = std::make_unique<SimpleDateTimeFormat>("Md", "te", false, errCode);
    EXPECT_EQ(sDateFormat301->Format(timep*1000), "23-04");
    auto sDateFormat302 = std::make_unique<SimpleDateTimeFormat>("yM", "te", false, errCode);
    EXPECT_EQ(sDateFormat302->Format(timep*1000), "04-2024");
    auto sDateFormat303 = std::make_unique<SimpleDateTimeFormat>("yMEd", "te", false, errCode);
    EXPECT_EQ(sDateFormat303->Format(timep*1000), "మంగళ, 23-04-2024");
    auto sDateFormat304 = std::make_unique<SimpleDateTimeFormat>("yMMMEd", "te", false, errCode);
    EXPECT_EQ(sDateFormat304->Format(timep*1000), "మంగళ, 23 ఏప్రి, 2024");
    auto sDateFormat305 = std::make_unique<SimpleDateTimeFormat>("yMMMd", "te", false, errCode);
    EXPECT_EQ(sDateFormat305->Format(timep*1000), "23 ఏప్రి, 2024");
    auto sDateFormat306 = std::make_unique<SimpleDateTimeFormat>("yMd", "te", false, errCode);
    EXPECT_EQ(sDateFormat306->Format(timep*1000), "23-04-2024");
    auto sDateFormat307 = std::make_unique<SimpleDateTimeFormat>("Hm", "th", false, errCode);
    EXPECT_EQ(sDateFormat307->Format(timep*1000), "00:00");
    auto sDateFormat308 = std::make_unique<SimpleDateTimeFormat>("hmv", "th", false, errCode);
    EXPECT_EQ(sDateFormat308->Format(timep*1000), "12:00 ก่อนเที่ยง เวลาจีน");
    auto sDateFormat309 = std::make_unique<SimpleDateTimeFormat>("GyMd", "tr", false, errCode);
    EXPECT_EQ(sDateFormat309->Format(timep*1000), "23.04.2024 MS");
    auto sDateFormat310 = std::make_unique<SimpleDateTimeFormat>("MEd", "tr", false, errCode);
    EXPECT_EQ(sDateFormat310->Format(timep*1000), "23.04 Sal");
}

/**
 * @tc.name: CustomizedDateTimeFormatTest0015
 * @tc.desc: Test Customized Data
 * @tc.type: FUNC
 */
HWTEST_F(CustomizedDateTimeFormatTest, CustomizedDateTimeFormatTest0015, TestSize.Level1)
{
    I18nErrorCode errCode = I18nErrorCode::SUCCESS;
    struct tm tmObj = {.tm_mday = 23, .tm_mon = 3, .tm_year = 124};
    time_t timep = mktime(&tmObj);
    auto sDateFormat311 = std::make_unique<SimpleDateTimeFormat>("Md", "tr", false, errCode);
    EXPECT_EQ(sDateFormat311->Format(timep*1000), "23.04");
    auto sDateFormat312 = std::make_unique<SimpleDateTimeFormat>("yM", "tr", false, errCode);
    EXPECT_EQ(sDateFormat312->Format(timep*1000), "04.2024");
    auto sDateFormat313 = std::make_unique<SimpleDateTimeFormat>("yMEd", "tr", false, errCode);
    EXPECT_EQ(sDateFormat313->Format(timep*1000), "23.04.2024 Sal");
    auto sDateFormat314 = std::make_unique<SimpleDateTimeFormat>("Gy", "ug", false, errCode);
    EXPECT_EQ(sDateFormat314->Format(timep*1000), "مىلادىيە 2024");
    auto sDateFormat315 = std::make_unique<SimpleDateTimeFormat>("GyMMM", "ug", false, errCode);
    EXPECT_EQ(sDateFormat315->Format(timep*1000), "مىلادىيە 2024-يىلى 4-ئاي");
    auto sDateFormat316 = std::make_unique<SimpleDateTimeFormat>("GyMMMEd", "ug", false, errCode);
    EXPECT_EQ(sDateFormat316->Format(timep*1000), "مىلادىيە 2024-يىلى 4-ئاينىڭ 23-كۈنى سەي");
    auto sDateFormat317 = std::make_unique<SimpleDateTimeFormat>("GyMMMd", "ug", false, errCode);
    EXPECT_EQ(sDateFormat317->Format(timep*1000), "مىلادىيە 2024-يىلى 4-ئاينىڭ 23-كۈنى");
    auto sDateFormat318 = std::make_unique<SimpleDateTimeFormat>("MEd", "ug", false, errCode);
    EXPECT_EQ(sDateFormat318->Format(timep*1000), "23/4 سەي");
    auto sDateFormat319 = std::make_unique<SimpleDateTimeFormat>("MMMEd", "ug", false, errCode);
    EXPECT_EQ(sDateFormat319->Format(timep*1000), "4-ئاينىڭ 23-كۈنى سەي");
    auto sDateFormat322 = std::make_unique<SimpleDateTimeFormat>("MMMMd", "ug", false, errCode);
    EXPECT_EQ(sDateFormat322->Format(timep*1000), "4-ئاينىڭ 23-كۈنى");
    auto sDateFormat323 = std::make_unique<SimpleDateTimeFormat>("MMMd", "ug", false, errCode);
    EXPECT_EQ(sDateFormat323->Format(timep*1000), "4-ئاينىڭ 23-كۈنى");
    auto sDateFormat324 = std::make_unique<SimpleDateTimeFormat>("Md", "ug", false, errCode);
    EXPECT_EQ(sDateFormat324->Format(timep*1000), "23/4");
    auto sDateFormat325 = std::make_unique<SimpleDateTimeFormat>("yM", "ug", false, errCode);
    EXPECT_EQ(sDateFormat325->Format(timep*1000), "4/2024");
    auto sDateFormat326 = std::make_unique<SimpleDateTimeFormat>("yMEd", "ug", false, errCode);
    EXPECT_EQ(sDateFormat326->Format(timep*1000), "23/4/2024 سەي");
    auto sDateFormat327 = std::make_unique<SimpleDateTimeFormat>("yMMMEd", "ug", false, errCode);
    EXPECT_EQ(sDateFormat327->Format(timep*1000), "2024-يىلى 4-ئاينىڭ 23-كۈنى سەي");
    auto sDateFormat328 = std::make_unique<SimpleDateTimeFormat>("yMMMd", "ug", false, errCode);
    EXPECT_EQ(sDateFormat328->Format(timep*1000), "2024-يىلى 4-ئاينىڭ 23-كۈنى");
    auto sDateFormat329 = std::make_unique<SimpleDateTimeFormat>("yMd", "ug", false, errCode);
    EXPECT_EQ(sDateFormat329->Format(timep*1000), "23/4/2024");
    auto sDateFormat332 = std::make_unique<SimpleDateTimeFormat>("Gy", "vi", false, errCode);
    EXPECT_EQ(sDateFormat332->Format(timep*1000), "Năm 2024 SCN");
    auto sDateFormat333 = std::make_unique<SimpleDateTimeFormat>("GyMMM", "vi", false, errCode);
    EXPECT_EQ(sDateFormat333->Format(timep*1000), "tháng 4 năm 2024 SCN");
    auto sDateFormat334 = std::make_unique<SimpleDateTimeFormat>("GyMMMEd", "vi", false, errCode);
    EXPECT_EQ(sDateFormat334->Format(timep*1000), "Th 3, ngày 23 tháng 4 năm 2024 SCN");
    auto sDateFormat335 = std::make_unique<SimpleDateTimeFormat>("GyMMMd", "vi", false, errCode);
    EXPECT_EQ(sDateFormat335->Format(timep*1000), "Ngày 23 tháng 4 năm 2024 SCN");
    auto sDateFormat336 = std::make_unique<SimpleDateTimeFormat>("Hm", "vi", false, errCode);
    EXPECT_EQ(sDateFormat336->Format(timep*1000), "00:00");
}

/**
 * @tc.name: CustomizedDateTimeFormatTest0016
 * @tc.desc: Test Customized Data
 * @tc.type: FUNC
 */
HWTEST_F(CustomizedDateTimeFormatTest, CustomizedDateTimeFormatTest0016, TestSize.Level1)
{
    I18nErrorCode errCode = I18nErrorCode::SUCCESS;
    struct tm tmObj = {.tm_mday = 23, .tm_mon = 3, .tm_year = 124};
    time_t timep = mktime(&tmObj);
    auto sDateFormat337 = std::make_unique<SimpleDateTimeFormat>("MEd", "vi", false, errCode);
    EXPECT_EQ(sDateFormat337->Format(timep*1000), "Th 3, 23/04");
    auto sDateFormat338 = std::make_unique<SimpleDateTimeFormat>("MMMEd", "vi", false, errCode);
    EXPECT_EQ(sDateFormat338->Format(timep*1000), "Th 3, ngày 23 tháng 4");
    auto sDateFormat339 = std::make_unique<SimpleDateTimeFormat>("MMMMEd", "vi", false, errCode);
    EXPECT_EQ(sDateFormat339->Format(timep*1000), "Th 3, ngày 23 tháng 4");
    auto sDateFormat340 = std::make_unique<SimpleDateTimeFormat>("MMMMd", "vi", false, errCode);
    EXPECT_EQ(sDateFormat340->Format(timep*1000), "Ngày 23 tháng 4");
    auto sDateFormat341 = std::make_unique<SimpleDateTimeFormat>("MMMd", "vi", false, errCode);
    EXPECT_EQ(sDateFormat341->Format(timep*1000), "Ngày 23 tháng 4");
    auto sDateFormat342 = std::make_unique<SimpleDateTimeFormat>("MMdd", "vi", false, errCode);
    EXPECT_EQ(sDateFormat342->Format(timep*1000), "23/04");
    auto sDateFormat343 = std::make_unique<SimpleDateTimeFormat>("Md", "vi", false, errCode);
    EXPECT_EQ(sDateFormat343->Format(timep*1000), "23/04");
    auto sDateFormat344 = std::make_unique<SimpleDateTimeFormat>("yM", "vi", false, errCode);
    EXPECT_EQ(sDateFormat344->Format(timep*1000), "04/2024");
    auto sDateFormat345 = std::make_unique<SimpleDateTimeFormat>("yMEd", "vi", false, errCode);
    EXPECT_EQ(sDateFormat345->Format(timep*1000), "Th 3, 23/04/2024");
    auto sDateFormat346 = std::make_unique<SimpleDateTimeFormat>("yMMM", "vi", false, errCode);
    EXPECT_EQ(sDateFormat346->Format(timep*1000), "tháng 4 năm 2024");
    auto sDateFormat347 = std::make_unique<SimpleDateTimeFormat>("yMMMEd", "vi", false, errCode);
    EXPECT_EQ(sDateFormat347->Format(timep*1000), "Th 3, ngày 23 tháng 4 năm 2024");
    auto sDateFormat348 = std::make_unique<SimpleDateTimeFormat>("yMMMd", "vi", false, errCode);
    EXPECT_EQ(sDateFormat348->Format(timep*1000), "Ngày 23 tháng 4 năm 2024");
    auto sDateFormat349 = std::make_unique<SimpleDateTimeFormat>("yMd", "vi", false, errCode);
    EXPECT_EQ(sDateFormat349->Format(timep*1000), "23/04/2024");
    auto sDateFormat350 = std::make_unique<SimpleDateTimeFormat>("MEd", "zh-Hans-SG", false, errCode);
    EXPECT_EQ(sDateFormat350->Format(timep*1000), "23/04 周二");
    auto sDateFormat351 = std::make_unique<SimpleDateTimeFormat>("MMdd", "zh-Hans-SG", false, errCode);
    EXPECT_EQ(sDateFormat351->Format(timep*1000), "23/04");
    auto sDateFormat352 = std::make_unique<SimpleDateTimeFormat>("Md", "zh-Hans-SG", false, errCode);
    EXPECT_EQ(sDateFormat352->Format(timep*1000), "23/04");
    auto sDateFormat353 = std::make_unique<SimpleDateTimeFormat>("yMEd", "zh-Hans-SG", false, errCode);
    EXPECT_EQ(sDateFormat353->Format(timep*1000), "23/04/2024，周二");
    auto sDateFormat354 = std::make_unique<SimpleDateTimeFormat>("yMd", "zh-Hans-SG", false, errCode);
    EXPECT_EQ(sDateFormat354->Format(timep*1000), "23/04/2024");
    auto sDateFormat355 = std::make_unique<SimpleDateTimeFormat>("Bh", "zh-Hant", false, errCode);
    EXPECT_EQ(sDateFormat355->Format(timep*1000), "凌晨 12 時");
    auto sDateFormat356 = std::make_unique<SimpleDateTimeFormat>("Bhm", "zh-Hant", false, errCode);
    EXPECT_EQ(sDateFormat356->Format(timep*1000), "凌晨 12:00");
    auto sDateFormat357 = std::make_unique<SimpleDateTimeFormat>("Bhms", "zh-Hant", false, errCode);
    EXPECT_EQ(sDateFormat357->Format(timep*1000), "凌晨 12:00:00");
    auto sDateFormat358 = std::make_unique<SimpleDateTimeFormat>("EBhm", "zh-Hant", false, errCode);
    EXPECT_EQ(sDateFormat358->Format(timep*1000), "週二凌晨 12:00");
}

/**
 * @tc.name: CustomizedDateTimeFormatTest0017
 * @tc.desc: Test Customized Data
 * @tc.type: FUNC
 */
HWTEST_F(CustomizedDateTimeFormatTest, CustomizedDateTimeFormatTest0017, TestSize.Level1)
{
    I18nErrorCode errCode = I18nErrorCode::SUCCESS;
    struct tm tmObj = {.tm_mday = 23, .tm_mon = 3, .tm_year = 124};
    time_t timep = mktime(&tmObj);
    auto sDateFormat359 = std::make_unique<SimpleDateTimeFormat>("EBhms", "zh-Hant", false, errCode);
    EXPECT_EQ(sDateFormat359->Format(timep*1000), "週二凌晨 12:00:00");
    auto sDateFormat360 = std::make_unique<SimpleDateTimeFormat>("Ed", "zh-Hant", false, errCode);
    EXPECT_EQ(sDateFormat360->Format(timep*1000), "23 日 週二");
    auto sDateFormat361 = std::make_unique<SimpleDateTimeFormat>("Ehm", "zh-Hant", false, errCode);
    EXPECT_EQ(sDateFormat361->Format(timep*1000), "週二上午 12:00");
    auto sDateFormat362 = std::make_unique<SimpleDateTimeFormat>("Ehms", "zh-Hant", false, errCode);
    EXPECT_EQ(sDateFormat362->Format(timep*1000), "週二上午 12:00:00");
    auto sDateFormat363 = std::make_unique<SimpleDateTimeFormat>("Gy", "zh-Hant", false, errCode);
    EXPECT_EQ(sDateFormat363->Format(timep*1000), "西元 2024 年");
    auto sDateFormat364 = std::make_unique<SimpleDateTimeFormat>("GyMMM", "zh-Hant", false, errCode);
    EXPECT_EQ(sDateFormat364->Format(timep*1000), "西元 2024 年 4 月");
    auto sDateFormat365 = std::make_unique<SimpleDateTimeFormat>("GyMMMEd", "zh-Hant", false, errCode);
    EXPECT_EQ(sDateFormat365->Format(timep*1000), "西元 2024 年 4 月 23 日 週二");
    auto sDateFormat366 = std::make_unique<SimpleDateTimeFormat>("GyMMMd", "zh-Hant", false, errCode);
    EXPECT_EQ(sDateFormat366->Format(timep*1000), "西元 2024 年 4 月 23 日 週二");
    auto sDateFormat367 = std::make_unique<SimpleDateTimeFormat>("H", "zh-Hant", false, errCode);
    EXPECT_EQ(sDateFormat367->Format(timep*1000), "0 時");
    auto sDateFormat368 = std::make_unique<SimpleDateTimeFormat>("M", "zh-Hant", false, errCode);
    EXPECT_EQ(sDateFormat368->Format(timep*1000), "4 月");
    auto sDateFormat369 = std::make_unique<SimpleDateTimeFormat>("MMMEd", "zh-Hant", false, errCode);
    EXPECT_EQ(sDateFormat369->Format(timep*1000), "4 月 23 日 週二");
    auto sDateFormat371 = std::make_unique<SimpleDateTimeFormat>("MMMMd", "zh-Hant", false, errCode);
    EXPECT_EQ(sDateFormat371->Format(timep*1000), "4 月 23 日");
    auto sDateFormat372 = std::make_unique<SimpleDateTimeFormat>("MMMd", "zh-Hant", false, errCode);
    EXPECT_EQ(sDateFormat372->Format(timep*1000), "4 月 23 日");
    auto sDateFormat373 = std::make_unique<SimpleDateTimeFormat>("d", "zh-Hant", false, errCode);
    EXPECT_EQ(sDateFormat373->Format(timep*1000), "23 日");
    auto sDateFormat374 = std::make_unique<SimpleDateTimeFormat>("h", "zh-Hant", false, errCode);
    EXPECT_EQ(sDateFormat374->Format(timep*1000), "上午 12 時");
    auto sDateFormat375 = std::make_unique<SimpleDateTimeFormat>("hm", "zh-Hant", false, errCode);
    EXPECT_EQ(sDateFormat375->Format(timep*1000), "上午 12:00");
    auto sDateFormat376 = std::make_unique<SimpleDateTimeFormat>("hms", "zh-Hant", false, errCode);
    EXPECT_EQ(sDateFormat376->Format(timep*1000), "上午 12:00:00");
    auto sDateFormat377 = std::make_unique<SimpleDateTimeFormat>("hmsv", "zh-Hant", false, errCode);
    EXPECT_EQ(sDateFormat377->Format(timep*1000), "上午 12:00:00 [中國時間]");
    auto sDateFormat378 = std::make_unique<SimpleDateTimeFormat>("hmv", "zh-Hant", false, errCode);
    EXPECT_EQ(sDateFormat378->Format(timep*1000), "上午 12:00 [中國時間]");
    auto sDateFormat379 = std::make_unique<SimpleDateTimeFormat>("y", "zh-Hant", false, errCode);
    EXPECT_EQ(sDateFormat379->Format(timep*1000), "2024 年");
    auto sDateFormat380 = std::make_unique<SimpleDateTimeFormat>("yMEEEEd", "zh-Hant", false, errCode);
    EXPECT_EQ(sDateFormat380->Format(timep*1000), "2024 年 4 月 23 日 星期二");
    auto sDateFormat381 = std::make_unique<SimpleDateTimeFormat>("yMMM", "zh-Hant", false, errCode);
    EXPECT_EQ(sDateFormat381->Format(timep*1000), "2024 年 4 月");
}

/**
 * @tc.name: CustomizedDateTimeFormatTest0018
 * @tc.desc: Test Customized Data
 * @tc.type: FUNC
 */
HWTEST_F(CustomizedDateTimeFormatTest, CustomizedDateTimeFormatTest0018, TestSize.Level1)
{
    I18nErrorCode errCode = I18nErrorCode::SUCCESS;
    struct tm tmObj = {.tm_mday = 23, .tm_mon = 3, .tm_year = 124};
    time_t timep = mktime(&tmObj);
    auto sDateFormat382 = std::make_unique<SimpleDateTimeFormat>("yMMMEd", "zh-Hant", false, errCode);
    EXPECT_EQ(sDateFormat382->Format(timep*1000), "2024 年 4 月 23 日 週二");
    auto sDateFormat383 = std::make_unique<SimpleDateTimeFormat>("yMMMM", "zh-Hant", false, errCode);
    EXPECT_EQ(sDateFormat383->Format(timep*1000), "2024 年 4 月");
    auto sDateFormat384 = std::make_unique<SimpleDateTimeFormat>("yMMMd", "zh-Hant", false, errCode);
    EXPECT_EQ(sDateFormat384->Format(timep*1000), "2024 年 4 月 23 日");
    auto sDateFormat388 = std::make_unique<SimpleDateTimeFormat>("Ehm", "zh-Hant-HK", false, errCode);
    EXPECT_EQ(sDateFormat388->Format(timep*1000), "週二 上午 12:00");
    auto sDateFormat389 = std::make_unique<SimpleDateTimeFormat>("Ehms", "zh-Hant-HK", false, errCode);
    EXPECT_EQ(sDateFormat389->Format(timep*1000), "週二 上午 12:00:00");
    auto sDateFormat390 = std::make_unique<SimpleDateTimeFormat>("GyMMMEd", "zh-Hant-HK", false, errCode);
    EXPECT_EQ(sDateFormat390->Format(timep*1000), "公元 2024 年 4 月 23 日 週二");
    auto sDateFormat391 = std::make_unique<SimpleDateTimeFormat>("MMMEd", "zh-Hant-HK", false, errCode);
    EXPECT_EQ(sDateFormat391->Format(timep*1000), "4 月 23 日 週二");
    auto sDateFormat393 = std::make_unique<SimpleDateTimeFormat>("h", "zh-Hant-HK", false, errCode);
    EXPECT_EQ(sDateFormat393->Format(timep*1000), "上午 12 時");
    auto sDateFormat394 = std::make_unique<SimpleDateTimeFormat>("hm", "zh-Hant-HK", false, errCode);
    EXPECT_EQ(sDateFormat394->Format(timep*1000), "上午 12:00");
    auto sDateFormat395 = std::make_unique<SimpleDateTimeFormat>("hms", "zh-Hant-HK", false, errCode);
    EXPECT_EQ(sDateFormat395->Format(timep*1000), "上午 12:00:00");
    auto sDateFormat396 = std::make_unique<SimpleDateTimeFormat>("hmsv", "zh-Hant-HK", false, errCode);
    EXPECT_EQ(sDateFormat396->Format(timep*1000), "上午 12:00:00 [中國時間]");
    auto sDateFormat397 = std::make_unique<SimpleDateTimeFormat>("hmv", "zh-Hant-HK", false, errCode);
    EXPECT_EQ(sDateFormat397->Format(timep*1000), "上午 12:00 [中國時間]");
    auto sDateFormat398 = std::make_unique<SimpleDateTimeFormat>("yMMMEd", "zh-Hant-HK", false, errCode);
    EXPECT_EQ(sDateFormat398->Format(timep*1000), "2024 年 4 月 23 日 週二");
    auto sDateFormat67 = std::make_unique<SimpleDateTimeFormat>("yM", "da", false, errCode);
    EXPECT_EQ(sDateFormat67->Format(timep*1000), "04/2024");
    auto sDateFormat22 = std::make_unique<SimpleDateTimeFormat>("MMMEd", "bo", false, errCode);
    EXPECT_EQ(sDateFormat22->Format(timep*1000), "ཟླ་༤ ཚེས་ 23 མིག་དམར་");
    auto sDateFormat200 = std::make_unique<SimpleDateTimeFormat>("yMM", "mr", false, errCode);
    EXPECT_EQ(sDateFormat200->Format(timep*1000), "०४/२०२४");
    auto sDateFormat66 = std::make_unique<SimpleDateTimeFormat>("Md", "da", false, errCode);
    EXPECT_EQ(sDateFormat66->Format(timep*1000), "23/04");
    auto sDateFormat199 = std::make_unique<SimpleDateTimeFormat>("MMdd", "mr", false, errCode);
    EXPECT_EQ(sDateFormat199->Format(timep*1000), "२३/०४");
}
} // namespace I18n
} // namespace Global
} // namespace OHOS