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
#ifndef OHOS_GLOBAL_I18N_NEGATIVE_RULE_H
#define OHOS_GLOBAL_I18N_NEGATIVE_RULE_H

#include <unicode/regex.h>

namespace OHOS {
namespace Global {
namespace I18n {
class NegativeRule {
public:
    NegativeRule(icu::UnicodeString& regex, std::string& insensitive);
    icu::RegexPattern* GetPattern();
    icu::UnicodeString Handle(icu::UnicodeString& src);

private:
    void ReplaceSpecifiedPos(icu::UnicodeString& chs, int start, int end);
    UErrorCode status;
    icu::UnicodeString regex;
    std::string insensitive;
};
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif