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
#ifndef GLOBAL_I18N_COLLATOR_H
#define GLOBAL_I18N_COLLATOR_H

#include <map>
#include <memory>
#include <set>
#include <string>
#include <unordered_set>
#include <vector>

#include "unicode/coll.h"
#include "unicode/locid.h"
#include "unicode/utypes.h"

#include "i18n_types.h"
#include "locale_info.h"

namespace OHOS {
namespace Global {
namespace I18n {
typedef enum CompareResult {
    INVALID = -2,
    SMALLER,
    EQUAL,
    GREATER,
} CompareResult;

class Collator {
public:
    static std::vector<std::string> SupportedLocalesOf(const std::vector<std::string> &requestLocales,
                                                       const std::map<std::string, std::string> &configs,
                                                       I18nErrorCode &status);

    Collator(std::vector<std::string> &localeTags, std::map<std::string, std::string> &options);
    Collator(std::vector<std::string> &localeTags, std::map<std::string, std::string> &options,
        const std::string &defaultLocale);
    ~Collator();
    CompareResult Compare(const std::string &first, const std::string &second);
    void ResolvedOptions(std::map<std::string, std::string> &options);
    I18nErrorCode GetError() const;

private:
    std::string localeStr;
    std::string localeMatcher;
    std::string usage;
    std::string sensitivity;
    std::string ignorePunctuation;
    std::string numeric;
    std::string caseFirst;
    std::string collation;

    std::unique_ptr<LocaleInfo> localeInfo = nullptr;
    icu::Locale locale;
    icu::Collator *collatorPtr = nullptr;
    bool createSuccess = false;
    I18nErrorCode i18nStatus = I18nErrorCode::SUCCESS;

    static std::set<std::string> GetAvailableLocales();
    void ParseAllOptions(std::map<std::string, std::string> &options);
    bool IsValidCollation(std::string &collation);
    void SetCollation();
    void SetUsage();
    void SetNumeric();
    void SetCaseFirst();
    void SetSensitivity();
    void SetIgnorePunctuation();
    bool InitCollator();
    void Init(std::vector<std::string> &localeTags, std::map<std::string, std::string> &options,
        const std::string &defaultLocale);
};
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif
