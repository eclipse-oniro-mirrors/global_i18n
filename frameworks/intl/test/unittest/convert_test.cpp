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
#include "convert_test.h"

#include <cstdio>
#include <gtest/gtest.h>
#include <map>
#include <vector>
#include <unistd.h>
#include "unicode/utypes.h"
#include "unicode/ucnv.h"
#include "unicode/ustring.h"
#include "unicode/uchar.h"
#include "unicode/uloc.h"
#include "unicode/unistr.h"
#include "ohos/init_data.h"

using namespace OHOS::Global::I18n;
using testing::ext::TestSize;
using namespace std;
using namespace testing;

namespace OHOS {
namespace Global {
namespace I18n {
class ConvertTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();

    static std::map<std::string, std::vector<char>> source1;
    static std::map<std::string, std::vector<char>> source2;
};

/* "abc中國臺灣" */
std::map<std::string, std::vector<char>> ConvertTest::source1 = {
    { "UTF-8", { 0x61, 0x62, 0x63, 0xE4, 0xB8, 0xAD, 0xE5, 0x9C, 0x8B, 0xE8, 0x87, 0xBA, 0xE7, 0x81, 0xA3 } },
    { "UTF-7", { 0x61, 0x62, 0x63, 0x2B, 0x54, 0x69, 0x31, 0x58, 0x43, 0x34, 0x48, 0x36, 0x63, 0x47, 0x4D, 0x2D } },
    { "UTF-16", { 0xFF, 0xFE, 0x61, 0x00, 0x62, 0x00, 0x63, 0x00, 0x2D, 0x4E, 0x0B, 0x57, 0xFA, 0x81, 0x63, 0x70 } },
    { "UTF-16BE", { 0x00, 0x61, 0x00, 0x62, 0x00, 0x63, 0x4E, 0x2D, 0x57, 0x0B, 0x81, 0xFA, 0x70, 0x63 } },
    { "UTF-16LE", { 0x61, 0x00, 0x62, 0x00, 0x63, 0x00, 0x2D, 0x4E, 0x0B, 0x57, 0xFA, 0x81, 0x63, 0x70 } },
    { "UTF-32", { 0xFF, 0xFE, 0x00, 0x00, 0x61, 0x00, 0x00, 0x00,
                  0x62, 0x00, 0x00, 0x00, 0x63, 0x00, 0x00, 0x00,
                  0x2D, 0x4E, 0x00, 0x00, 0x0B, 0x57, 0x00, 0x00,
                  0xFA, 0x81, 0x00, 0x00, 0x63, 0x70, 0x00, 0x00 } },
    { "UTF-32BE", { 0x00, 0x00, 0x00, 0x61, 0x00, 0x00, 0x00, 0x62, 0x00,
                    0x00, 0x00, 0x63, 0x00, 0x00, 0x4E, 0x2D, 0x00, 0x00,
                    0x57, 0x0B, 0x00, 0x00, 0x81, 0xFA, 0x00, 0x00, 0x70, 0x63 } },
    { "UTF-32LE", { 0x61, 0x00, 0x00, 0x00, 0x62, 0x00, 0x00, 0x00, 0x63,
                    0x00, 0x00, 0x00, 0x2D, 0x4E, 0x00, 0x00, 0x0B, 0x57,
                    0x00, 0x00, 0xFA, 0x81, 0x00, 0x00, 0x63, 0x70, 0x00, 0x00 } },
    { "GBK", { 0x61, 0x62, 0x63, 0xD6, 0xD0, 0x87, 0xF8, 0xC5, 0x5F, 0x9E, 0xB3 } },
    { "BIG5", { 0x61, 0x62, 0x63, 0xA4, 0xA4, 0xB0, 0xEA, 0xBB, 0x4F, 0xC6, 0x57 } },
};

/* "中aرདЧüん국" */
std::map<std::string, std::vector<char>> ConvertTest::source2 = {
    { "UTF-8", { 0xE4, 0xB8, 0xAD, 0x61, 0xD8, 0xB1, 0xE0, 0xBD, 0x91, 0xD0,
                 0xA7, 0xC3, 0xBC, 0xE3, 0x82, 0x93, 0xEA, 0xB5, 0xAD } },
    { "UTF-7", { 0x2B, 0x54, 0x69, 0x30, 0x2D, 0x61, 0x2B, 0x42, 0x6A, 0x45, 0x50, 0x55,
                 0x51, 0x51, 0x6E, 0x41, 0x50, 0x77, 0x77, 0x6B, 0x36, 0x31, 0x74, 0x2D } },
    { "UTF-16", { 0xFF, 0xFE, 0x2D, 0x4E, 0x61, 0x00, 0x31, 0x06, 0x51,
                  0x0F, 0x27, 0x04, 0xFC, 0x00, 0x93, 0x30, 0x6D, 0xAD } },
    { "UTF-16BE", { 0x4E, 0x2D, 0x00, 0x61, 0x06, 0x31, 0x0F, 0x51,
                    0x04, 0x27, 0x00, 0xFC, 0x30, 0x93, 0xAD, 0x6D } },
    { "UTF-16LE", { 0x2D, 0x4E, 0x61, 0x00, 0x31, 0x06, 0x51, 0x0F,
                    0x27, 0x04, 0xFC, 0x00, 0x93, 0x30, 0x6D, 0xAD } },
    { "UTF-32", { 0xFF, 0xFE, 0x00, 0x00, 0x2D, 0x4E, 0x00, 0x00, 0x61,
                  0x00, 0x00, 0x00, 0x31, 0x06, 0x00, 0x00, 0x51, 0x0F,
                  0x00, 0x00, 0x27, 0x04, 0x00, 0x00, 0xFC, 0x00, 0x00,
                  0x00, 0x93, 0x30, 0x00, 0x00, 0x6D, 0xAD, 0x00, 0x00 } },
    { "UTF-32BE", { 0x00, 0x00, 0x4E, 0x2D, 0x00, 0x00, 0x00, 0x61,
                    0x00, 0x00, 0x06, 0x31, 0x00, 0x00, 0x0F, 0x51,
                    0x00, 0x00, 0x04, 0x27, 0x00, 0x00, 0x00, 0xFC,
                    0x00, 0x00, 0x30, 0x93, 0x00, 0x00, 0xAD, 0x6D } },
    { "UTF-32LE", { 0x2D, 0x4E, 0x00, 0x00, 0x61, 0x00, 0x00, 0x00,
                    0x31, 0x06, 0x00, 0x00, 0x51, 0x0F, 0x00, 0x00,
                    0x27, 0x04, 0x00, 0x00, 0xFC, 0x00, 0x00, 0x00,
                    0x93, 0x30, 0x00, 0x00, 0x6D, 0xAD, 0x00, 0x00 } },
};

void ConvertTest::SetUpTestCase(void)
{
    SetHwIcuDirectory();
}

void ConvertTest::TearDownTestCase(void)
{
}

void ConvertTest::SetUp(void)
{}

void ConvertTest::TearDown(void)
{}

/**
 * @tc.name: ConvertTest0001
 * @tc.desc: Test ucnv_convert
 * @tc.type: FUNC
 */
HWTEST_F(ConvertTest, ConvertTest0001, TestSize.Level1)
{
    /* from UTF-8 to others */
    for (auto iter = ConvertTest::source1.begin(); iter != ConvertTest::source1.end(); ++iter) {
        if (iter->first == "UTF-8") {
            continue;
        }
        UErrorCode err = U_ZERO_ERROR;
        char target[100];
        int32_t size = ucnv_convert(iter->first.c_str(), /* out */
                                    "UTF-8",  /* in */
                                    target,
                                    sizeof(target),
                                    &ConvertTest::source1["UTF-8"][0],
                                    ConvertTest::source1["UTF-8"].size(),
                                    &err);
        ASSERT_TRUE(U_SUCCESS(err));
        EXPECT_EQ(size, iter->second.size());
        for (int32_t i = 0; i < size; ++i) {
            EXPECT_EQ(target[i], iter->second[i]);
        }
    }

    /* from others to UTF-8 */
    for (auto iter = ConvertTest::source1.begin(); iter != ConvertTest::source1.end(); ++iter) {
        if (iter->first == "UTF-8") {
            continue;
        }
        UErrorCode err = U_ZERO_ERROR;
        char target[100];
        int32_t size = ucnv_convert("UTF-8", /* out */
                                    iter->first.c_str(),  /* in */
                                    target,
                                    sizeof(target),
                                    &(iter->second)[0],
                                    iter->second.size(),
                                    &err);
        ASSERT_TRUE(U_SUCCESS(err));
        EXPECT_EQ(size, ConvertTest::source1["UTF-8"].size());
        for (int32_t i = 0; i < size; ++i) {
            EXPECT_EQ(target[i], ConvertTest::source1["UTF-8"][i]);
        }
    }
}

/**
 * @tc.name: ConvertTest0002
 * @tc.desc: Test ucnv_toUChars and ucnv_fromUChars
 * @tc.type: FUNC
 */
HWTEST_F(ConvertTest, ConvertTest0002, TestSize.Level1)
{
    for (auto iter = ConvertTest::source1.begin(); iter != ConvertTest::source1.end(); ++iter) {
        if (iter->first == "UTF-8") {
            continue;
        }
        UErrorCode status = U_ZERO_ERROR;
        UConverter *convUTF8 = ucnv_open("UTF-8", &status);
        ASSERT_TRUE(U_SUCCESS(status));
        UConverter *convOthers = ucnv_open(iter->first.c_str(), &status);
        ASSERT_TRUE(U_SUCCESS(status));
        UChar ucharOther[100];
        char targetOther[100];
        // from UTF-8 to UChars
        int32_t ucharLen = ucnv_toUChars(convUTF8, ucharOther, 100,
            &ConvertTest::source1["UTF-8"][0], ConvertTest::source1["UTF-8"].size(), &status);
        ASSERT_TRUE(U_SUCCESS(status));

        // from UChars to other encoding
        int32_t otherLen = ucnv_fromUChars(convOthers, targetOther, 100, ucharOther, ucharLen, &status);
        ASSERT_TRUE(U_SUCCESS(status));

        EXPECT_EQ(otherLen, iter->second.size());
        for (int32_t i = 0; i < otherLen; ++i) {
            EXPECT_EQ(targetOther[i], iter->second[i]);
        }

        UChar ucharUTF8[100];
        char targetUTF8[100];
        // from other encoding to UChars
        int32_t ucharLen2 = ucnv_toUChars(convOthers, ucharUTF8, 100,
            &(iter->second)[0], iter->second.size(), &status);
        ASSERT_TRUE(U_SUCCESS(status));

        EXPECT_EQ(ucharLen, ucharLen2);
        for (int32_t i = 0; i < ucharLen; ++i) {
            EXPECT_EQ(ucharOther[i], ucharUTF8[i]);
        }

        // from UChars to UTF-8
        int32_t utf8Len = ucnv_fromUChars(convUTF8, targetUTF8, 100, ucharUTF8, ucharLen2, &status);
        ASSERT_TRUE(U_SUCCESS(status));

        EXPECT_EQ(utf8Len, ConvertTest::source1["UTF-8"].size());
        for (int32_t i = 0; i < utf8Len; ++i) {
            EXPECT_EQ(targetUTF8[i], ConvertTest::source1["UTF-8"][i]);
        }

        ucnv_close(convUTF8);
        ucnv_close(convOthers);
    }
}

/**
 * @tc.name: ConvertTest0003
 * @tc.desc: Test ucnv_convert
 * @tc.type: FUNC
 */
HWTEST_F(ConvertTest, ConvertTest0003, TestSize.Level1)
{
    /* from UTF-8 to others */
    for (auto iter = ConvertTest::source2.begin(); iter != ConvertTest::source2.end(); ++iter) {
        if (iter->first == "UTF-8") {
            continue;
        }
        UErrorCode err = U_ZERO_ERROR;
        char target[100];
        int32_t size = ucnv_convert(iter->first.c_str(), /* out */
                                    "UTF-8",  /* in */
                                    target,
                                    sizeof(target),
                                    &ConvertTest::source2["UTF-8"][0],
                                    ConvertTest::source2["UTF-8"].size(),
                                    &err);
        ASSERT_TRUE(U_SUCCESS(err));
        EXPECT_EQ(size, iter->second.size());
        for (int32_t i = 0; i < size; ++i) {
            EXPECT_EQ(target[i], iter->second[i]);
        }
    }

    /* from others to UTF-8 */
    for (auto iter = ConvertTest::source2.begin(); iter != ConvertTest::source2.end(); ++iter) {
        if (iter->first == "UTF-8") {
            continue;
        }
        UErrorCode err = U_ZERO_ERROR;
        char target[100];
        int32_t size = ucnv_convert("UTF-8", /* out */
                                    iter->first.c_str(),  /* in */
                                    target,
                                    sizeof(target),
                                    &(iter->second)[0],
                                    iter->second.size(),
                                    &err);
        ASSERT_TRUE(U_SUCCESS(err));
        EXPECT_EQ(size, ConvertTest::source2["UTF-8"].size());
        for (int32_t i = 0; i < size; ++i) {
            EXPECT_EQ(target[i], ConvertTest::source2["UTF-8"][i]);
        }
    }
}

/**
 * @tc.name: ConvertTest0004
 * @tc.desc: Test ucnv_toUChars and ucnv_fromUChars
 * @tc.type: FUNC
 */
HWTEST_F(ConvertTest, ConvertTest0004, TestSize.Level1)
{
    for (auto iter = ConvertTest::source2.begin(); iter != ConvertTest::source2.end(); ++iter) {
        if (iter->first == "UTF-8") {
            continue;
        }
        UErrorCode status = U_ZERO_ERROR;
        UConverter *convUTF8 = ucnv_open("UTF-8", &status);
        ASSERT_TRUE(U_SUCCESS(status));
        UConverter *convOthers = ucnv_open(iter->first.c_str(), &status);
        ASSERT_TRUE(U_SUCCESS(status));
        UChar ucharOther[100];
        char targetOther[100];
        // from UTF-8 to UChars
        int32_t ucharLen = ucnv_toUChars(convUTF8, ucharOther, 100,
            &ConvertTest::source2["UTF-8"][0], ConvertTest::source2["UTF-8"].size(), &status);
        ASSERT_TRUE(U_SUCCESS(status));

        // from UChars to other encoding
        int32_t otherLen = ucnv_fromUChars(convOthers, targetOther, 100, ucharOther, ucharLen, &status);
        ASSERT_TRUE(U_SUCCESS(status));

        EXPECT_EQ(otherLen, iter->second.size());
        for (int32_t i = 0; i < otherLen; ++i) {
            EXPECT_EQ(targetOther[i], iter->second[i]);
        }

        UChar ucharUTF8[100];
        char targetUTF8[100];
        // from other encoding to UChars
        int32_t ucharLen2 = ucnv_toUChars(convOthers, ucharUTF8, 100,
            &(iter->second)[0], iter->second.size(), &status);
        ASSERT_TRUE(U_SUCCESS(status));

        EXPECT_EQ(ucharLen, ucharLen2);
        for (int32_t i = 0; i < ucharLen; ++i) {
            EXPECT_EQ(ucharOther[i], ucharUTF8[i]);
        }

        // from UChars to UTF-8
        int32_t utf8Len = ucnv_fromUChars(convUTF8, targetUTF8, 100, ucharUTF8, ucharLen2, &status);
        ASSERT_TRUE(U_SUCCESS(status));

        EXPECT_EQ(utf8Len, ConvertTest::source2["UTF-8"].size());
        for (int32_t i = 0; i < utf8Len; ++i) {
            EXPECT_EQ(targetUTF8[i], ConvertTest::source2["UTF-8"][i]);
        }

        ucnv_close(convUTF8);
        ucnv_close(convOthers);
    }
}

/**
 * @tc.name: ConvertTest0005
 * @tc.desc: Test ucnv_setFromUCallBack UCNV_FROM_U_CALLBACK_STOP
 * @tc.type: FUNC
 */
HWTEST_F(ConvertTest, ConvertTest0005, TestSize.Level1)
{
    /* "中a" */
    char sourceGBK[] = { 0xD6, 0xD0, 0x61 };
    UErrorCode status = U_ZERO_ERROR;
    UConverter *convUTF8 = ucnv_open("UTF-8", &status);
    ASSERT_TRUE(U_SUCCESS(status));
    UConverter *convGBK = ucnv_open("GBK", &status);
    ASSERT_TRUE(U_SUCCESS(status));
    ucnv_setFromUCallBack(convGBK, UCNV_FROM_U_CALLBACK_STOP, nullptr, nullptr, nullptr, &status);
    UChar ucharOther[100];
    char targetOther[100];
    // from UTF-8 to UChars
    int32_t ucharLen = ucnv_toUChars(convUTF8, ucharOther, 100,
        &ConvertTest::source2["UTF-8"][0], ConvertTest::source2["UTF-8"].size(), &status);
    ASSERT_TRUE(U_SUCCESS(status));

    // from UChars to other encoding
    int32_t otherLen = ucnv_fromUChars(convGBK, targetOther, 100, ucharOther, ucharLen, &status);
    EXPECT_EQ(status, U_INVALID_CHAR_FOUND);

    EXPECT_EQ(otherLen, sizeof(sourceGBK));
    for (int32_t i = 0; i < otherLen; ++i) {
        EXPECT_EQ(targetOther[i], sourceGBK[i]);
    }

    ucnv_close(convUTF8);
    ucnv_close(convGBK);
}

/**
 * @tc.name: ConvertTest0006
 * @tc.desc: Test ucnv_setFromUCallBack UCNV_FROM_U_CALLBACK_SKIP
 * @tc.type: FUNC
 */
HWTEST_F(ConvertTest, ConvertTest0006, TestSize.Level1)
{
    /* "中aЧüん" */
    char sourceGBK[] = { 0xD6, 0xD0, 0x61, 0xA7, 0xB9, 0xA8, 0xB9, 0xA4, 0xF3 };
    UErrorCode status = U_ZERO_ERROR;
    UConverter *convUTF8 = ucnv_open("UTF-8", &status);
    ASSERT_TRUE(U_SUCCESS(status));
    UConverter *convGBK = ucnv_open("GBK", &status);
    ASSERT_TRUE(U_SUCCESS(status));
    ucnv_setFromUCallBack(convGBK, UCNV_FROM_U_CALLBACK_SKIP, nullptr, nullptr, nullptr, &status);
    UChar ucharOther[100];
    char targetOther[100];
    // from UTF-8 to UChars
    int32_t ucharLen = ucnv_toUChars(convUTF8, ucharOther, 100,
        &ConvertTest::source2["UTF-8"][0], ConvertTest::source2["UTF-8"].size(), &status);
    ASSERT_TRUE(U_SUCCESS(status));

    // from UChars to other encoding
    int32_t otherLen = ucnv_fromUChars(convGBK, targetOther, 100, ucharOther, ucharLen, &status);
    ASSERT_TRUE(U_SUCCESS(status));

    EXPECT_EQ(otherLen, sizeof(sourceGBK));
    for (int32_t i = 0; i < otherLen; ++i) {
        EXPECT_EQ(targetOther[i], sourceGBK[i]);
    }

    ucnv_close(convUTF8);
    ucnv_close(convGBK);
}

/**
 * @tc.name: ConvertTest0007
 * @tc.desc: Test ucnv_setFromUCallBack UCNV_FROM_U_CALLBACK_ESCAPE
 * @tc.type: FUNC
 */
HWTEST_F(ConvertTest, ConvertTest0007, TestSize.Level1)
{
    /* "中a\u0631\u0F51Чüん\uAD6D" */
    char sourceGBK[] = { 0xD6, 0xD0, 0x61, 0x5C, 0x75, 0x30, 0x36, 0x33, 0x31, 0x5C, 0x75, 0x30, 0x46,
                       0x35, 0x31, 0xA7, 0xB9, 0xA8, 0xB9, 0xA4, 0xF3, 0x5C, 0x75, 0x41, 0x44, 0x36, 0x44};
    UErrorCode status = U_ZERO_ERROR;
    UConverter *convUTF8 = ucnv_open("UTF-8", &status);
    ASSERT_TRUE(U_SUCCESS(status));
    UConverter *convGBK = ucnv_open("GBK", &status);
    ASSERT_TRUE(U_SUCCESS(status));
    /* UCNV_ESCAPE_ICU UCNV_ESCAPE_JAVA UCNV_ESCAPE_C UCNV_ESCAPE_XML_DEC UCNV_ESCAPE_XML_HEX */
    ucnv_setFromUCallBack(convGBK, UCNV_FROM_U_CALLBACK_ESCAPE, UCNV_ESCAPE_C, nullptr, nullptr, &status);
    UChar ucharOther[100];
    char targetOther[100];
    // from UTF-8 to UChars
    int32_t ucharLen = ucnv_toUChars(convUTF8, ucharOther, 100,
        &ConvertTest::source2["UTF-8"][0], ConvertTest::source2["UTF-8"].size(), &status);
    ASSERT_TRUE(U_SUCCESS(status));

    // from UChars to other encoding
    int32_t otherLen = ucnv_fromUChars(convGBK, targetOther, 100, ucharOther, ucharLen, &status);
    ASSERT_TRUE(U_SUCCESS(status));

    EXPECT_EQ(otherLen, sizeof(sourceGBK));
    for (int32_t i = 0; i < otherLen; ++i) {
        EXPECT_EQ(targetOther[i], sourceGBK[i]);
    }

    ucnv_close(convUTF8);
    ucnv_close(convGBK);
}

/**
 * @tc.name: ConvertTest0008
 * @tc.desc: Test ucnv_setToUCallBack UCNV_TO_U_CALLBACK_STOP
 * @tc.type: FUNC
 */
HWTEST_F(ConvertTest, ConvertTest0008, TestSize.Level1)
{
    /* "丂?疎" */
    char sourceGBK[] = { 0x81, 0x40, 0x81, 0x7F, 0xAF, 0x45 };
    /* "丂" */
    UChar unicode[] = { 0x4E02 };
    UErrorCode status = U_ZERO_ERROR;
    UConverter *convGBK = ucnv_open("GBK", &status);
    ASSERT_TRUE(U_SUCCESS(status));
    ucnv_setToUCallBack(convGBK, UCNV_TO_U_CALLBACK_STOP, nullptr, nullptr, nullptr, &status);
    UChar ucharOther[100];
    // from GBK to UNICODE
    int32_t ucharLen = ucnv_toUChars(convGBK, ucharOther, 100, sourceGBK, sizeof(sourceGBK), &status);
    EXPECT_EQ(status, U_ILLEGAL_CHAR_FOUND);

    EXPECT_EQ(ucharLen, sizeof(unicode) / sizeof(UChar));
    for (int32_t i = 0; i < ucharLen; ++i) {
        EXPECT_EQ(ucharOther[i], unicode[i]);
    }

    ucnv_close(convGBK);
}

/**
 * @tc.name: ConvertTest0009
 * @tc.desc: Test ucnv_setToUCallBack UCNV_TO_U_CALLBACK_SKIP
 * @tc.type: FUNC
 */
HWTEST_F(ConvertTest, ConvertTest0009, TestSize.Level1)
{
    /* "丂?疎" */
    char sourceGBK[] = { 0x81, 0x40, 0x81, 0x7F, 0xAF, 0x45 };
    /* "丂疎" */
    UChar unicode[] = { 0x4E02, 0x007F, 0x758E };
    UErrorCode status = U_ZERO_ERROR;
    UConverter *convGBK = ucnv_open("GBK", &status);
    ASSERT_TRUE(U_SUCCESS(status));
    ucnv_setToUCallBack(convGBK, UCNV_TO_U_CALLBACK_SKIP, nullptr, nullptr, nullptr, &status);
    UChar ucharOther[100];
    // from GBK to UNICODE
    int32_t ucharLen = ucnv_toUChars(convGBK, ucharOther, 100, sourceGBK, sizeof(sourceGBK), &status);
    ASSERT_TRUE(U_SUCCESS(status));

    EXPECT_EQ(ucharLen, sizeof(unicode) / sizeof(UChar));
    for (int32_t i = 0; i < ucharLen; ++i) {
        EXPECT_EQ(ucharOther[i], unicode[i]);
    }

    ucnv_close(convGBK);
}

/**
 * @tc.name: ConvertTest00010
 * @tc.desc: Test ucnv_setToUCallBack UCNV_TO_U_CALLBACK_ESCAPE
 * @tc.type: FUNC
 */
HWTEST_F(ConvertTest, ConvertTest00010, TestSize.Level1)
{
    /* "丂?疎" */
    char sourceGBK[] = { 0x81, 0x40, 0x81, 0x7F, 0xAF, 0x45 };
    /* "丂\x81疎" */
    UChar unicode[] = { 0x4E02, 0x005C, 0x0078, 0x038, 0x0031, 0x007F, 0x758E };
    UErrorCode status = U_ZERO_ERROR;
    UConverter *convGBK = ucnv_open("GBK", &status);
    ASSERT_TRUE(U_SUCCESS(status));
    /* UCNV_ESCAPE_ICU UCNV_ESCAPE_JAVA UCNV_ESCAPE_C UCNV_ESCAPE_XML_DEC UCNV_ESCAPE_XML_HEX */
    ucnv_setToUCallBack(convGBK, UCNV_TO_U_CALLBACK_ESCAPE, UCNV_ESCAPE_C, nullptr, nullptr, &status);
    UChar ucharOther[100];
    // from GBK to UNICODE
    int32_t ucharLen = ucnv_toUChars(convGBK, ucharOther, 100, sourceGBK, sizeof(sourceGBK), &status);
    ASSERT_TRUE(U_SUCCESS(status));

    EXPECT_EQ(ucharLen, sizeof(unicode) / sizeof(UChar));
    for (int32_t i = 0; i < ucharLen; ++i) {
        EXPECT_EQ(ucharOther[i], unicode[i]);
    }

    ucnv_close(convGBK);
}
} // namespace I18n
} // namespace Global
} // namespace OHOS