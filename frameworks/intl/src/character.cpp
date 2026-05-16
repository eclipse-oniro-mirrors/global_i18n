/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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
#include "character.h"

#include <sys/types.h>
#include <set>
#include <string>

#include "cctype"
#include "i18n_hilog.h"
#include "map"
#include "string"
#include "unicode/uchar.h"
#include "unicode/ucsdet.h"
#include "unicode/umachine.h"
#include "unicode/unistr.h"
#include "unicode/urename.h"
#include "unicode/utypes.h"

namespace OHOS {
namespace Global {
namespace I18n {
static std::set<UCharDirection> RTLDirectionSet = {
    U_RIGHT_TO_LEFT,
    U_RIGHT_TO_LEFT_ARABIC,
    U_RIGHT_TO_LEFT_EMBEDDING,
    U_RIGHT_TO_LEFT_OVERRIDE,
    U_RIGHT_TO_LEFT_ISOLATE
};

bool IsDigit(const std::string &character)
{
    icu::UnicodeString unicodeString(character.c_str());
    UChar32 char32 = unicodeString.char32At(0);
    return u_isdigit(char32);
}

bool IsSpaceChar(const std::string &character)
{
    icu::UnicodeString unicodeString(character.c_str());
    UChar32 char32 = unicodeString.char32At(0);
    return u_isJavaSpaceChar(char32);
}

bool IsWhiteSpace(const std::string &character)
{
    icu::UnicodeString unicodeString(character.c_str());
    UChar32 char32 = unicodeString.char32At(0);
    return u_isWhitespace(char32);
}

bool IsRTLCharacter(const std::string &character)
{
    icu::UnicodeString unicodeString(character.c_str());
    UChar32 char32 = unicodeString.char32At(0);
    UCharDirection direction = u_charDirection(char32);
    if (RTLDirectionSet.find(direction) != RTLDirectionSet.end()) {
        return true;
    }
    return false;
}

bool IsIdeoGraphic(const std::string &character)
{
    icu::UnicodeString unicodeString(character.c_str());
    UChar32 char32 = unicodeString.char32At(0);
    return u_hasBinaryProperty(char32, UCHAR_IDEOGRAPHIC);
}

bool IsLetter(const std::string &character)
{
    icu::UnicodeString unicodeString(character.c_str());
    UChar32 char32 = unicodeString.char32At(0);
    return isalpha(char32);
}

bool IsLowerCase(const std::string &character)
{
    icu::UnicodeString unicodeString(character.c_str());
    UChar32 char32 = unicodeString.char32At(0);
    return u_islower(char32);
}

bool IsUpperCase(const std::string &character)
{
    icu::UnicodeString unicodeString(character.c_str());
    UChar32 char32 = unicodeString.char32At(0);
    return u_isupper(char32);
}

std::pair<std::string, int32_t> DetectEncoding(const char* dataStream, size_t length)
{
    std::pair<std::string, int32_t> encodingInfo { "", 0 };
    if (dataStream == nullptr || length == 0) {
        return encodingInfo;
    }
    UErrorCode status = U_ZERO_ERROR;
    UCharsetDetector* csdet = ucsdet_open(&status);
    if (U_FAILURE(status) || csdet == nullptr) {
        HILOG_ERROR_I18N("DetectEncoding: Open csdet failed, code %{public}s.", u_errorName(status));
        return encodingInfo;
    }
    ucsdet_setText(csdet, dataStream, static_cast<int32_t>(length), &status);
    if (U_FAILURE(status)) {
        ucsdet_close(csdet);
        HILOG_ERROR_I18N("DetectEncoding: Set text failed, code %{public}s.", u_errorName(status));
        return encodingInfo;
    }
    const UCharsetMatch* match = ucsdet_detect(csdet, &status);
    if (U_FAILURE(status) || match == nullptr) {
        ucsdet_close(csdet);
        HILOG_ERROR_I18N("DetectEncoding: Detect failed, code %{public}s.", u_errorName(status));
        return encodingInfo;
    }
    const char* name = ucsdet_getName(match, &status);
    if (U_FAILURE(status) || name == nullptr) {
        ucsdet_close(csdet);
        HILOG_ERROR_I18N("DetectEncoding: Get name failed, code %{public}s.", u_errorName(status));
        return encodingInfo;
    }
    encodingInfo.first = name;
    int32_t confidence = ucsdet_getConfidence(match, &status);
    if (U_FAILURE(status)) {
        ucsdet_close(csdet);
        HILOG_ERROR_I18N("DetectEncoding: Get confidence failed, code %{public}s.", u_errorName(status));
        return encodingInfo;
    }
    encodingInfo.second = confidence;
    ucsdet_close(csdet);
    return encodingInfo;
}

std::map<UCharCategory, std::string> categoryMap = {
    { U_UNASSIGNED, "U_UNASSIGNED" },
    { U_GENERAL_OTHER_TYPES, "U_GENERAL_OTHER_TYPES" },
    { U_UPPERCASE_LETTER, "U_UPPERCASE_LETTER" },
    { U_LOWERCASE_LETTER, "U_LOWERCASE_LETTER" },
    { U_TITLECASE_LETTER, "U_TITLECASE_LETTER" },
    { U_MODIFIER_LETTER, "U_MODIFIER_LETTER" },
    { U_OTHER_LETTER, "U_OTHER_LETTER" },
    { U_NON_SPACING_MARK, "U_NON_SPACING_MARK" },
    { U_ENCLOSING_MARK, "U_ENCLOSING_MARK" },
    { U_COMBINING_SPACING_MARK, "U_COMBINING_SPACING_MARK" },
    { U_DECIMAL_DIGIT_NUMBER, "U_DECIMAL_DIGIT_NUMBER" },
    { U_LETTER_NUMBER, "U_LETTER_NUMBER" },
    { U_OTHER_NUMBER, "U_OTHER_NUMBER" },
    { U_SPACE_SEPARATOR, "U_SPACE_SEPARATOR" },
    { U_LINE_SEPARATOR, "U_LINE_SEPARATOR" },
    { U_PARAGRAPH_SEPARATOR, "U_PARAGRAPH_SEPARATOR" },
    { U_CONTROL_CHAR, "U_CONTROL_CHAR" },
    { U_FORMAT_CHAR, "U_FORMAT_CHAR" },
    { U_PRIVATE_USE_CHAR, "U_PRIVATE_USE_CHAR" },
    { U_SURROGATE, "U_SURROGATE" },
    { U_DASH_PUNCTUATION, "U_DASH_PUNCTUATION" },
    { U_START_PUNCTUATION, "U_START_PUNCTUATION" },
    { U_END_PUNCTUATION, "U_END_PUNCTUATION" },
    { U_CONNECTOR_PUNCTUATION, "U_CONNECTOR_PUNCTUATION" },
    { U_OTHER_PUNCTUATION, "U_OTHER_PUNCTUATION" },
    { U_MATH_SYMBOL, "U_MATH_SYMBOL" },
    { U_CURRENCY_SYMBOL, "U_CURRENCY_SYMBOL" },
    { U_MODIFIER_SYMBOL, "U_MODIFIER_SYMBOL" },
    { U_OTHER_SYMBOL, "U_OTHER_SYMBOL" },
    { U_INITIAL_PUNCTUATION, "U_INITIAL_PUNCTUATION" },
    { U_FINAL_PUNCTUATION, "U_FINAL_PUNCTUATION" },
    { U_CHAR_CATEGORY_COUNT, "U_CHAR_CATEGORY_COUNT" },
};

std::string GetType(const std::string &character)
{
    icu::UnicodeString unicodeString(character.c_str());
    UChar32 char32 = unicodeString.char32At(0);
    int8_t category = u_charType(char32);
    return categoryMap[UCharCategory(category)];
}
} // namespace I18n
} // namespace Global
} // namespace OHOS