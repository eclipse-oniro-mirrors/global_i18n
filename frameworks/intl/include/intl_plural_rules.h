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

#ifndef OHOS_GLOBAL_INTL_PLURAL_RULES_H
#define OHOS_GLOBAL_INTL_PLURAL_RULES_H

#include <mutex>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <string>
#include <vector>
#include "unicode/locid.h"
#include "unicode/numberformatter.h"
#include "unicode/plurrule.h"
#include "i18n_types.h"

namespace OHOS {
namespace Global {
namespace I18n {
enum class RoundingType : uint8_t { FRACTIONDIGITS = 0x01, SIGNIFICANTDIGITS, COMPACTROUNDING, EXCEPTION };

class IntlPluralRules {
public:
    struct ResolvedValue {
        RoundingType roundingType = RoundingType::EXCEPTION;
        std::string locale = "";
        std::string type = "";
        int32_t minimumIntegerDigits = 0;
        int32_t minimumDigits = 0;
        int32_t maximumDigits = 0;
        std::vector<std::string> pluralCategories {};
    };

    static std::vector<std::string> SupportedLocalesOf(const std::vector<std::string>& requestLocales,
        const std::unordered_map<std::string, std::string>& options, ErrorMessage& errorMessage);

    IntlPluralRules(const std::vector<std::string>& localeTags,
        const std::unordered_map<std::string, std::string>& options, ErrorMessage& errorMessage);
    std::string Select(double number, ErrorMessage& errorMessage) const;
    ResolvedValue ResolvedOptions() const;

private:
    static std::set<std::string> GetAvailableLocales();
    static bool GetNextLocale(icu::StringEnumeration* locales, std::string& validLocale, int32_t* len);
    static int32_t GetDefaultNumberOption(const std::unordered_map<std::string, std::string>& options,
        const std::string& key, int32_t minimum, int32_t maximum, int32_t fallback, ErrorMessage& errorMessage);
    static bool IsValidOptionName(const std::unordered_set<std::string>& optionValues, const std::string& value);
    static std::string ParseLocaleMatcher(const std::unordered_map<std::string, std::string>& options,
        ErrorMessage& errorMessage);
    static std::string ParseType(const std::unordered_map<std::string, std::string>& options,
        ErrorMessage& errorMessage);

    bool ParseOptions(const std::unordered_map<std::string, std::string>& options, ErrorMessage& errorMessage);
    bool ResolveLocale(const std::set<std::string>& availableLocales,
        const std::vector<std::string>& requestedLocales, ErrorMessage& errorMessage);
    bool InitIntlPluralRules(const std::unordered_map<std::string, std::string>& options, ErrorMessage& errorMessage);
    bool SetNumberFormatDigitOptions(const std::unordered_map<std::string, std::string>& options,
        int32_t mnfdDefault, int32_t mxfdDefault, ErrorMessage& errorMessage);
    bool SetSignificantDigitsOptions(const std::unordered_map<std::string, std::string>& options,
        ErrorMessage& errorMessage);
    bool SetFractionDigitsOptions(const std::unordered_map<std::string, std::string>& options, int32_t mnfdDefault,
        int32_t mxfdDefault, ErrorMessage& errorMessage);
    bool GetPluralCategories(ResolvedValue& options) const;

    static const int32_t STRING_SEPARATOR_LENGTH;
    static const int32_t MAX_DIGITS_VALUE;
    static const int32_t MAX_FRACTION_DIGITS_VALUE;
    static const int32_t MNFD_DEFAULT;
    static const int32_t MXFD_DEFAULT;
    static bool initAvailableLocales;
    static std::mutex getLocalesMutex;
    static std::set<std::string> availableLocales;
    static const std::string LOCALE_MATCHER_TAG;
    static const std::string TYPE_TAG;
    static const std::string MIN_INTEGER_DIGITS;
    static const std::string MIN_SIGNALE_DIGITS;
    static const std::string MAX_SIGNALE_DIGITS;
    static const std::string MIN_FRACTION_DIGITS;
    static const std::string MAX_FRACTION_DIGITS;
    static const std::unordered_set<std::string> LOCALE_MATCHER_VALUE;
    static const std::unordered_set<std::string> TYPE_VALUE;
    static const std::unordered_map<std::string, UPluralType> TYPE_TO_ICU_TYPE;

    bool initSuccess = false;
    std::string localeMatcher;
    std::string type;
    int32_t minimumIntegerDigits = -1;
    int32_t minimumFractionDigits = -1;
    int32_t maximumFractionDigits = -1;
    int32_t minimumSignificantDigits = -1;
    int32_t maximumSignificantDigits = -1;
    std::string roundingPriority;
    std::string roundingIncrement;
    std::string roundingMode;
    RoundingType roundingType = RoundingType::EXCEPTION;
    std::string localeStr;
    icu::Locale icuLocale;
    icu::number::LocalizedNumberFormatter icuNumberFormatter;
    std::unique_ptr<icu::PluralRules> icuPluralRules = nullptr;
};
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif
