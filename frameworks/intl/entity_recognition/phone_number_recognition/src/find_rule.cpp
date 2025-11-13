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
#include "find_rule.h"
#include "i18n_hilog.h"

namespace OHOS {
namespace Global {
namespace I18n {
FindRule::FindRule(icu::UnicodeString& regex, std::string& insensitive)
{
    this->regex = regex;
    this->insensitive = insensitive;
    if (regex.length() == 0) {
        return;
    }
}

icu::RegexPattern* FindRule::GetPattern()
{
    UErrorCode status = U_ZERO_ERROR;
    icu::RegexPattern* pattern;
    // Sets whether regular expression matching is case sensitive
    if (insensitive == "True") {
        pattern = icu::RegexPattern::compile(this->regex, URegexpFlag::UREGEX_CASE_INSENSITIVE, status);
    } else {
        pattern = icu::RegexPattern::compile(this->regex, 0, status);
    }
    if (U_FAILURE(status)) {
        HILOG_ERROR_I18N("FindRule::GetPattern: Compile regex pattern failed.");
        return nullptr;
    }
    return pattern;
}
} // namespace I18n
} // namespace Global
} // namespace OHOS