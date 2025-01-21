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
#include "date_time_sequence.h"
#include "i18n_hilog.h"
#include "unicode/datefmt.h"
#include "unicode/locid.h"
#include "unicode/smpdtfmt.h"
#include "unicode/dtptngen.h"
#include <regex>

namespace OHOS {
namespace Global {
namespace I18n {

const std::unordered_map<std::string, std::string> DateTimeSequence::DATE_ORDER_MAP = {
    { "ug", "L-d-y" },
    { "ar", "y-L-d" },
    { "fa", "y-L-d" },
    { "ur", "y-L-d" },
    { "iw", "y-L-d" },
    { "he", "y-L-d" },
};

std::string DateTimeSequence::GetDateOrder(const std::string &locale)
{
    UErrorCode icuStatus = U_ZERO_ERROR;
    icu::Locale localeObj = icu::Locale::forLanguageTag(locale.data(), icuStatus);
    if (U_FAILURE(icuStatus)) {
        HILOG_ERROR_I18N("Failed to create locale for GetDateOrder");
        return "";
    }
    const char* language = localeObj.getBaseName();
    if (language == nullptr) {
        HILOG_ERROR_I18N("%{public}s getBaseName is null.", locale.c_str());
        return "";
    }
    std::string languageTag = language;
    if (DATE_ORDER_MAP.find(languageTag) != DATE_ORDER_MAP.end()) {
        std::string dateOrder = DATE_ORDER_MAP.find(languageTag)->second;
        return dateOrder;
    }
    icu::SimpleDateFormat* formatter = dynamic_cast<icu::SimpleDateFormat*>
        (icu::DateFormat::createDateInstance(icu::DateFormat::EStyle::kDefault, localeObj));
    if (U_FAILURE(icuStatus) || formatter == nullptr) {
        HILOG_ERROR_I18N("Failed to create SimpleDateFormat");
        return "";
    }
    std::string tempValue;
    icu::UnicodeString unistr;
    formatter->toPattern(unistr);
    unistr.toUTF8String<std::string>(tempValue);
    std::string value = ModifyOrder(tempValue);
    delete formatter;
    return value;
}

std::string DateTimeSequence::ModifyOrder(std::string &pattern)
{
    int order[3] = { 0 }; // total 3 elements 'y', 'M'/'L', 'd'
    int lengths[4] = { 0 }; // first elements is the currently found elememnts, thus 4 elements totally.
    bool flag = true;
    for (size_t i = 0; i < pattern.length(); ++i) {
        char ch = pattern[i];
        if (flag && std::isalpha(ch)) {
            ProcessNormal(ch, order, 3, lengths, 4); // 3, 4 are lengths of these arrays
        } else if (ch == '\'') {
            flag = !flag;
        }
    }
    std::unordered_map<char, int> pattern2index = {
        { 'y', 1 },
        { 'L', 2 },
        { 'd', 3 },
    };
    std::string ret;
    for (int i = 0; i < 3; ++i) { // 3 is the size of orders
        auto it = pattern2index.find(order[i]);
        if (it == pattern2index.end()) {
            continue;
        }
        int index = it->second;
        if ((lengths[index] > 0) && (lengths[index] <= 6)) { // 6 is the max length of a filed
            ret.append(lengths[index], order[i]);
        }
        if (i < 2) { // 2 is the size of the order minus one
            ret.append(1, '-');
        }
    }
    return ret;
}

void DateTimeSequence::ProcessNormal(char ch, int *order, size_t orderSize, int *lengths, size_t lengsSize)
{
    char adjust;
    int index = -1;
    if (ch == 'd') {
        adjust = 'd';
        index = 3; // 3 is the index of 'd'
    } else if ((ch == 'L') || (ch == 'M')) {
        adjust = 'L';
        index = 2; // 2 is the index of 'M'
    } else if (ch == 'y') {
        adjust = 'y';
        index = 1;
    } else {
        return;
    }
    if ((index < 0) || (index >= static_cast<int>(lengsSize))) {
        return;
    }
    if (lengths[index] == 0) {
        if (lengths[0] >= 3) { // 3 is the index of order
            return;
        }
        order[lengths[0]] = static_cast<int>(adjust);
        ++lengths[0];
        lengths[index] = 1;
    } else {
        ++lengths[index];
    }
}

} // namespace I18n
} // namespace Global
} // namespace OHOS