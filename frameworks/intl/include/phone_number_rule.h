/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#ifndef OHOS_GLOBAL_PHONE_NUMBER_RULE_H
#define OHOS_GLOBAL_PHONE_NUMBER_RULE_H

#include <string>
#include <unicode/regex.h>
#include <vector>
#include "regex_rule.h"

namespace OHOS {
namespace Global {
namespace I18n {
class PhoneNumberRule {
public:
    PhoneNumberRule(std::string& country);
    ~PhoneNumberRule();
    void Init();
    
    std::vector<RegexRule*> GetBorderRules();
    std::vector<RegexRule*> GetCodesRules();
    std::vector<RegexRule*> GetPositiveRules();
    std::vector<RegexRule*> GetNegativeRules();
    std::vector<RegexRule*> GetFindRules();
    // Indicates whether the rules of the country are optimized
    bool isFixed;

private:
    void InitRule(std::string& xmlPath);
    void FreePointer(std::vector<RegexRule*> &ruleList);
    void SetRules(std::string& category, icu::UnicodeString& content, std::string& valid,
        std::string& handle, std::string& insensitive, std::string& type);

    std::vector<RegexRule*> negativeRules;
    std::vector<RegexRule*> positiveRules;
    std::vector<RegexRule*> borderRules;
    std::vector<RegexRule*> codesRules;
    std::vector<RegexRule*> findRules;
    std::string country;
    std::string xmlPath;
    bool commonExit;
    static std::string xmlCommonPath;
};
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif