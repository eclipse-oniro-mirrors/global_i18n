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
#ifndef GLOBAL_LOCALE_HELPER_H
#define GLOBAL_LOCALE_HELPER_H

#include <string>
#include <map>
#include <mutex>
#include <set>
#include <unordered_set>
#include <vector>
#include "unicode/locid.h"
#include "i18n_types.h"

namespace OHOS {
namespace Global {
namespace I18n {
class LocaleHelper {
public:
    static bool IsValidOptionName(const std::string& key, const std::string& option);
    static std::string ParseOptionWithoutCheck(const std::map<std::string, std::string> &options,
                                               const std::string &key,
                                               const std::string &defaultOption);
    static std::string ParseOption(const std::map<std::string, std::string> &options,
                                   const std::string &key,
                                   const std::string &defaultOption,
                                   bool isOptional,
                                   I18nErrorCode &status);
    static std::string BestAvailableLocale(const std::set<std::string> &availableLocales,
                                           const std::string &locale);
    static std::vector<std::string> LookupSupportedLocales(const std::set<std::string> &availableLocales,
                                                           const std::vector<std::string> &requestLocales);
    static std::vector<std::string> CanonicalizeLocaleList(const std::vector<std::string> &locales,
                                                           I18nErrorCode &status);
    static bool IsUnicodeScriptSubtag(const std::string &value);
    static bool IsUnicodeRegionSubtag(const std::string &value);
    static bool IsWellFormedCurrencyCode(const std::string &currency);
    static bool IsWellFormedCalendarCode(const std::string &calendar);
    static bool IsStructurallyValidLanguageTag(const std::string& tag);
    static bool DealwithLanguageTag(const std::vector<std::string> &containers, size_t &address);
    static bool IsAlpha(const std::string &str, size_t min, size_t max);
    static bool IsNormativeCalendar(const std::string &value);
    static bool IsNormativeNumberingSystem(const std::string &value);
    static double TruncateDouble(double number);
    static std::set<std::string> GetAvailableLocales();
    static std::vector<std::string> SupportedLocalesOf(const std::vector<std::string> &requestLocales,
        const std::map<std::string, std::string> &configs, I18nErrorCode &status);
    static std::string CheckParamLocales(const std::vector<std::string> &localeArray);
    static std::string DefaultLocale();
    static std::string LookupMatcher(const std::set<std::string>& availableLocales,
                                     const std::vector<std::string>& requestedLocales);

private:
    static bool IsWellAlphaNumList(const std::string &value);

    static const std::unordered_map<std::string, std::unordered_set<std::string>> KEY_TO_OPTION_NAME;
    static std::string defaultLocale;
    static std::mutex defaultLocaleMutex;
};
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif