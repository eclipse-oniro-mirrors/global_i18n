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
#include "i18n_hilog.h"
#include "border_rule.h"
#include "phone_number_matched.h"

namespace OHOS {
namespace Global {
namespace I18n {
const std::string BorderRule::CONTAIN_STR = "CONTAIN";
const std::string BorderRule::CONTAIN_OR_INTERSECT_STR = "CONTAIN_OR_INTERSECT";
const std::string BorderRule::TRUE_STR = "True";

BorderRule::BorderRule(icu::UnicodeString& regex, std::string& insensitive, std::string& type)
{
    this->regex = regex;
    if (type.compare(CONTAIN_STR) == 0) {
        // 9 indicates a certain execution logic of the border rule.
        this->type = 9;
    } else if (type.compare(CONTAIN_OR_INTERSECT_STR) == 0) {
        // 8 indicates a certain execution logic of the border rule.
        this->type = 8;
    } else {
        this->type = 0;
    }
    this->insensitive = insensitive;
}

int BorderRule::GetType()
{
    return type;
}

icu::RegexPattern* BorderRule::GetPattern()
{
    // Sets whether regular expression matching is case sensitive
    UErrorCode status = U_ZERO_ERROR;
    icu::RegexPattern* pattern;
    if (insensitive.compare(TRUE_STR) == 0) {
        pattern = icu::RegexPattern::compile(this->regex, URegexpFlag::UREGEX_CASE_INSENSITIVE, status);
    } else {
        pattern = icu::RegexPattern::compile(this->regex, 0, status);
    }
    if (U_FAILURE(status)) {
        HILOG_ERROR_I18N("BorderRule::GetPattern: Compile regex pattern failed.");
        return nullptr;
    }
    return pattern;
}

bool BorderRule::Handle(PhoneNumberMatch* match, icu::UnicodeString& message)
{
    int begin = match->start();
    int end = match->end();
    // chenk the 10 characters before and after the phone number
    int beginSubTen = begin - 10 < 0 ? 0 : begin - 10;
    int endAddTen = end + 10 > message.length() ? message.length() : end + 10;
    icu::UnicodeString borderStr = message.tempSubString(beginSubTen, endAddTen - beginSubTen);
        
    icu::RegexPattern* pattern = this->GetPattern();
    if (pattern == nullptr) {
        return false;
    }
    UErrorCode status = U_ZERO_ERROR;
    icu::RegexMatcher* mat = pattern->matcher(borderStr, status);
    if (U_FAILURE(status) || mat == nullptr) {
        HILOG_ERROR_I18N("BorderRule::Handle: Pattern matcher failed.");
        delete pattern;
        return false;
    }
    int type = this->GetType();
    while (mat->find()) {
        int borderBegin = mat->start(status) + beginSubTen;
        int borderEnd = mat->end(status) + beginSubTen;
        bool isDel = false;
        if (type == PhoneNumberMatched::CONTAIN && borderBegin <= begin && end <= borderEnd) {
            isDel = true;
        } else if (type == PhoneNumberMatched::CONTAIN_OR_INTERSECT && ((borderBegin <= begin &&
            end <= borderEnd) || (borderBegin < begin && begin < borderEnd && borderEnd < end) ||
            (begin < borderBegin && borderBegin < end && end < borderEnd))) {
            isDel = true;
        }
        if (isDel) {
            delete mat;
            delete pattern;
            return false;
        }
    }
    delete mat;
    delete pattern;

    return true;
}
} // namespace I18n
} // namespace Global
} // namespace OHOS