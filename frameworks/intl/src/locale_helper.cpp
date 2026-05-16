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
#include "locale_helper.h"
#include <sstream>
#include "unicode/localebuilder.h"
#include "i18n_hilog.h"
#include "locale_info.h"
#include "utils.h"

namespace OHOS {
namespace Global {
namespace I18n {
namespace {
constexpr uint8_t INTL_INDEX_TWO = 2;
constexpr uint8_t INTL_INDEX_THREE = 3;
constexpr uint8_t INTL_INDEX_FOUR = 4;
constexpr uint8_t INTL_INDEX_FIVE = 5;
constexpr uint8_t INTL_INDEX_EIGHT = 8;
const int32_t MIN_CALENDAR_LENGTH = 3;
const int32_t MAX_CALENDAR_LENGTH = 8;
const int32_t CURRENCY_LENGTH = 3;

inline constexpr int AsciiAlphaToLower(uint32_t c)
{
    constexpr uint32_t FLAG = 0x20;
    return static_cast<int>(c | FLAG);
}

template<typename T, typename U>
bool InRange(T value, U start, U end)
{
    if (start > end || sizeof(T) < sizeof(U)) {
        return false;
    }
    return (value >= static_cast<T>(start)) && (value <= static_cast<T>(end));
}

bool IsDigit(const std::string &str, size_t min, size_t max)
{
    if (!InRange(str.length(), min, max)) {
        return false;
    }
    for (char i : str) {
        if (!InRange(i, '0', '9')) {
            return false;
        }
    }
    return true;
}

bool IsAsciiAlpha(char ch)
{
    return InRange(ch, 'A', 'Z') || InRange(ch, 'a', 'z');
}

bool IsAlphanum(const std::string &str, size_t min, size_t max)
{
    if (!InRange(str.length(), min, max)) {
        return false;
    }
    for (char i : str) {
        if (!IsAsciiAlpha(i) && !InRange(i, '0', '9')) {
            return false;
        }
    }
    return true;
}

bool IsThirdDigitAlphanum(const std::string &value)
{
    return value.length() == INTL_INDEX_FOUR && InRange(value[0], '0', '9') &&
        IsAlphanum(value.substr(1), INTL_INDEX_THREE, INTL_INDEX_THREE);
}

bool IsVariantSubtag(const std::string &value)
{
    return IsThirdDigitAlphanum(value) || IsAlphanum(value, INTL_INDEX_FIVE, INTL_INDEX_EIGHT);
}

bool IsLanguageSubtag(const std::string &value)
{
    return LocaleHelper::IsAlpha(value, INTL_INDEX_TWO, INTL_INDEX_THREE) ||
        LocaleHelper::IsAlpha(value, INTL_INDEX_FIVE, INTL_INDEX_EIGHT);
}

bool IsScriptSubtag(const std::string &value)
{
    return LocaleHelper::IsAlpha(value, INTL_INDEX_FOUR, INTL_INDEX_FOUR);
}

bool IsRegionSubtag(const std::string &value)
{
    return LocaleHelper::IsAlpha(value, INTL_INDEX_TWO, INTL_INDEX_TWO) ||
        IsDigit(value, INTL_INDEX_THREE, INTL_INDEX_THREE);
}

bool IsExtensionSingleton(const std::string &value)
{
    return IsAlphanum(value, 1, 1);
}
}

std::string LocaleHelper::defaultLocale = "";
std::mutex LocaleHelper::defaultLocaleMutex;

const std::unordered_map<std::string, std::unordered_set<std::string>> LocaleHelper::KEY_TO_OPTION_NAME = {
    { "localeMatcher", { "best fit", "lookup" } },
    { "style", { "narrow", "short", "long" }},
    { "type", { "language", "region", "script", "currency", "calendar", "dateTimeField" } },
    { "fallback", { "code", "none" } },
    { "languageDisplay", { "dialect", "standard" } },
    { "usage", { "sort", "search" } },
    { "numeric", {  "false", "true" } },
    { "caseFirst", { "false", "upper", "lower" } },
    { "sensitivity", { "base", "accent", "case", "variant" } },
    { "ignorePunctuation", { "true", "false" } },
    { "hourCycle", { "h11", "h12", "h23", "h24" } },
};

bool LocaleHelper::IsValidOptionName(const std::string& key, const std::string& option)
{
    auto optionIter = KEY_TO_OPTION_NAME.find(key);
    if (optionIter == KEY_TO_OPTION_NAME.end()) {
        return false;
    }
    return optionIter->second.find(option) != optionIter->second.end();
}

std::string LocaleHelper::ParseOptionWithoutCheck(const std::map<std::string, std::string> &options,
                                                  const std::string &key,
                                                  const std::string &defaultOption)
{
    auto optionsIter = options.find(key);
    if (optionsIter == options.end()) {
        return defaultOption;
    }
    return optionsIter->second;
}

std::string LocaleHelper::ParseOption(const std::map<std::string, std::string>& options,
                                      const std::string& key,
                                      const std::string& defaultOption,
                                      bool isOptional,
                                      I18nErrorCode& status)
{
    auto optionsMapIter = KEY_TO_OPTION_NAME.find(key);
    if (optionsMapIter == KEY_TO_OPTION_NAME.end()) {
        return defaultOption;
    }
    auto optionsIter = options.find(key);
    if (optionsIter == options.end()) {
        if (!isOptional) {
            status = I18nErrorCode::MISSING_PARAM;
            HILOG_ERROR_I18N("LocaleHelper::ParseOption: missing param %{public}s.", key.c_str());
        }
        return defaultOption;
    }
    std::string option = optionsIter->second;
    const std::unordered_set<std::string>& optionsName = optionsMapIter->second;
    auto nameIter = optionsName.find(option);
    if (nameIter == optionsName.end()) {
        status = I18nErrorCode::INVALID_PARAM;
        return option;
    }
    return option;
}

std::string LocaleHelper::BestAvailableLocale(const std::set<std::string> &availableLocales,
                                              const std::string &locale)
{
    std::string localeCandidate = locale;
    std::string undefined = std::string();
    while (true) {
        for (const std::string& itemStr : availableLocales) {
            if (itemStr == localeCandidate) {
                return localeCandidate;
            }
        }
        size_t pos = localeCandidate.rfind('-');
        if (pos == std::string::npos) {
            return undefined;
        }
        if (pos >= INTL_INDEX_TWO && localeCandidate[pos - INTL_INDEX_TWO] == '-') {
            pos -= INTL_INDEX_TWO;
        }
        localeCandidate.resize(pos);
    }
}

std::vector<std::string> LocaleHelper::LookupSupportedLocales(const std::set<std::string> &availableLocales,
                                                              const std::vector<std::string> &requestLocales)
{
    std::vector<std::string> resultLocales;
    std::unordered_set<std::string> visitedLocales;
    for (const auto& candidateLocale : requestLocales) {
        std::string availableLocale = LocaleHelper::BestAvailableLocale(availableLocales, candidateLocale);
        if (!availableLocale.empty() && visitedLocales.find(candidateLocale) == visitedLocales.end()) {
            resultLocales.push_back(candidateLocale);
            visitedLocales.insert(candidateLocale);
        }
    }
    return resultLocales;
}

std::vector<std::string> LocaleHelper::CanonicalizeLocaleList(const std::vector<std::string> &locales,
                                                              I18nErrorCode &status)
{
    std::vector<std::string> resultLocales;
    for (const std::string& locale : locales) {
        if (locale.empty() || !LocaleHelper::IsStructurallyValidLanguageTag(locale)) {
            status = I18nErrorCode::INVALID_LOCALE_TAG;
            return {};
        }
        std::string localeStr = locale;
        std::transform(localeStr.begin(), localeStr.end(), localeStr.begin(), AsciiAlphaToLower);
        UErrorCode icuStatus = U_ZERO_ERROR;
        icu::Locale formalLocale = icu::Locale::forLanguageTag(localeStr.c_str(), icuStatus);
        if (U_FAILURE(icuStatus) || formalLocale.isBogus()) {
            status = I18nErrorCode::INVALID_LOCALE_TAG;
            return {};
        }
        formalLocale = icu::LocaleBuilder().setLocale(formalLocale).build(icuStatus);
        // Canonicalize the locale ID of this object according to CLDR.
        formalLocale.canonicalize(icuStatus);
        if ((U_FAILURE(icuStatus)) || (formalLocale.isBogus())) {
            status = I18nErrorCode::INVALID_LOCALE_TAG;
            return {};
        }
        std::string languageTag = formalLocale.toLanguageTag<std::string>(icuStatus);
        if (U_FAILURE(icuStatus)) {
            status = I18nErrorCode::INVALID_LOCALE_TAG;
            return {};
        }
        resultLocales.push_back(languageTag);
    }
    return resultLocales;
}

bool LocaleHelper::IsUnicodeScriptSubtag(const std::string& value)
{
    UErrorCode status = U_ZERO_ERROR;
    icu::LocaleBuilder builder;
    builder.setScript(value).build(status);
    return U_SUCCESS(status);
}

bool LocaleHelper::IsUnicodeRegionSubtag(const std::string& value)
{
    UErrorCode status = U_ZERO_ERROR;
    icu::LocaleBuilder builder;
    builder.setRegion(value).build(status);
    return U_SUCCESS(status);
}

bool LocaleHelper::IsWellFormedCurrencyCode(const std::string &currency)
{
    if (currency.length() != CURRENCY_LENGTH) {
        return false;
    }
    for (char c : currency) {
        if (!IsAsciiAlpha(c)) {
            return false;
        }
    }
    return true;
}

bool LocaleHelper::IsWellFormedCalendarCode(const std::string &calendar)
{
    std::string value = calendar;
    while (true) {
        std::size_t found_dash = value.find('-');
        if (found_dash == std::string::npos) {
            return IsAlphanum(value, MIN_CALENDAR_LENGTH, MAX_CALENDAR_LENGTH);
        }
        if (!IsAlphanum(value.substr(0, found_dash), MIN_CALENDAR_LENGTH, MAX_CALENDAR_LENGTH)) {
            return false;
        }
        value = value.substr(found_dash + 1);
    }
}

bool LocaleHelper::IsStructurallyValidLanguageTag(const std::string& tag)
{
    std::string tagCollection = tag;
    std::vector<std::string> containers;
    std::string substring;
    std::set<std::string> uniqueSubtags;
    size_t address = 1;
    for (auto it = tagCollection.begin(); it != tagCollection.end(); it++) {
        if (*it != '-' && it != tagCollection.end() - 1) {
            substring += *it;
            continue;
        }
        if (it == tagCollection.end() - 1) {
            substring += *it;
        }
        containers.push_back(substring);
        if (IsVariantSubtag(substring)) {
            std::transform(substring.begin(), substring.end(), substring.begin(), AsciiAlphaToLower);
            if (!uniqueSubtags.insert(substring).second) {
                return false;
            }
        }
        substring.clear();
    }
    bool result = DealwithLanguageTag(containers, address);
    return result;
}

bool LocaleHelper::DealwithLanguageTag(const std::vector<std::string> &containers, size_t &address)
{
    // The abstract operation returns true if locale can be generated from the ABNF grammar in section 2.1 of the RFC,
    // starting with Language-Tag, and does not contain duplicate variant or singleton subtags
    // If language tag is empty, return false.
    if (containers.empty()) {
        return false;
    }

    // a. if the first tag is not language, return false.
    if (!IsLanguageSubtag(containers[0])) {
        return false;
    }

    // if the tag include language only, like "zh" or "de", return true;
    if (containers.size() == 1) {
        return true;
    }

    // Else, then
    // if is unique singleton subtag, script and region tag.
    if (IsExtensionSingleton(containers[1])) {
        return true;
    }

    address = 1;  // Start parsing from the second element of the container.
    if (IsScriptSubtag(containers[address])) {
        address++;
        if (containers.size() == address) {
            return true;
        }
    }

    if (IsRegionSubtag(containers[address])) {
        address++;
    }

    for (size_t i = address; i < containers.size(); i++) {
        if (IsExtensionSingleton(containers[i])) {
            return true;
        }
        if (!IsVariantSubtag(containers[i])) {
            return false;
        }
    }
    return true;
}

bool LocaleHelper::IsAlpha(const std::string &str, size_t min, size_t max)
{
    if (!InRange(str.length(), min, max)) {
        return false;
    }
    for (char c : str) {
        if (!IsAsciiAlpha(c)) {
            return false;
        }
    }
    return true;
}

bool LocaleHelper::IsNormativeCalendar(const std::string &value)
{
    return IsWellAlphaNumList(value);
}

bool LocaleHelper::IsNormativeNumberingSystem(const std::string &value)
{
    return IsWellAlphaNumList(value);
}

bool LocaleHelper::IsWellAlphaNumList(const std::string &value)
{
    if (value.length() < INTL_INDEX_THREE) {
        return false;
    }
    char lastChar = value[value.length() - 1];
    if (lastChar == '-') {
        return false;
    }
    std::vector<std::string> items;
    Split(value, "-", items);
    for (auto &item : items) {
        if (!IsAlphanum(item, INTL_INDEX_THREE, INTL_INDEX_EIGHT)) {
            return false;
        }
    }
    return true;
}

double LocaleHelper::TruncateDouble(double number)
{
    if (std::isnan(number)) {
        return 0;
    }
    if (!std::isfinite(number)) {
        return number;
    }
    // -0 to +0
    if (Eq(number, 0)) {
        return 0;
    }
    double ret = Geq(number, 0) ? std::floor(number) : std::ceil(number);
    return ret;
}

std::set<std::string> LocaleHelper::GetAvailableLocales()
{
    std::set<std::string> result;
    int32_t count = 0;
    const icu::Locale* locales = icu::Locale::getAvailableLocales(count);
    if (count < 1 || locales == nullptr) {
        HILOG_ERROR_I18N("LocaleHelper::GetAvailableLocales: Get available locales failed.");
        return result;
    }
    for (int i = 0; i < count; i++) {
        const char* name = locales[i].getName();
        if (name != nullptr) {
            result.insert(name);
        }
    }
    return result;
}

std::vector<std::string> LocaleHelper::SupportedLocalesOf(const std::vector<std::string> &requestLocales,
    const std::map<std::string, std::string> &configs, I18nErrorCode &status)
{
    std::vector<std::string> undefined = {};
    auto requestedLocales = CanonicalizeLocaleList(requestLocales, status);
    if (status != I18nErrorCode::SUCCESS) {
        return undefined;
    }
    std::string localeMatcher = ParseOption(configs, "localeMatcher", "best fit", true, status);
    if (status != I18nErrorCode::SUCCESS) {
        return undefined;
    }
    std::set<std::string> availableLocales = GetAvailableLocales();
    return LookupSupportedLocales(availableLocales, requestedLocales);
}

std::string LocaleHelper::CheckParamLocales(const std::vector<std::string> &localeArray)
{
    I18nErrorCode i18nStatus = I18nErrorCode::SUCCESS;
    std::vector<std::string> requestedLocales = CanonicalizeLocaleList(localeArray, i18nStatus);
    if (i18nStatus != I18nErrorCode::SUCCESS) {
        HILOG_ERROR_I18N("LocaleHelper::CheckParamLocales: CanonicalizeLocaleList failed");
        return "invalid locale";
    }
    return "";
}

std::string LocaleHelper::DefaultLocale()
{
    if (!defaultLocale.empty()) {
        return defaultLocale;
    }
    std::lock_guard<std::mutex> defaultLocaleLock(defaultLocaleMutex);
    if (!defaultLocale.empty()) {
        return defaultLocale;
    }
    icu::Locale icuDefaultLocale;
    if (strcmp(icuDefaultLocale.getName(), "en_US_POSIX") == 0 || strcmp(icuDefaultLocale.getName(), "c") == 0) {
        defaultLocale = "en-US";
    } else if (icuDefaultLocale.isBogus()) {
        defaultLocale = "und";
    } else {
        UErrorCode icuStatus = U_ZERO_ERROR;
        defaultLocale = icuDefaultLocale.toLanguageTag<std::string>(icuStatus);
        if (U_FAILURE(icuStatus)) {
            HILOG_ERROR_I18N("LocaleHelper::DefaultLocale: toLanguageTag failed.");
        }
    }
    return defaultLocale;
}

std::string LocaleHelper::LookupMatcher(const std::set<std::string>& availableLocales,
                                        const std::vector<std::string>& requestedLocales)
{
    for (const std::string& locale : requestedLocales) {
        std::string availableLocale = BestAvailableLocale(availableLocales, locale);
        if (!availableLocale.empty()) {
            return availableLocale;
        }
    }
    return "";
}
} // namespace I18n
} // namespace Global
} // namespace OHOS
