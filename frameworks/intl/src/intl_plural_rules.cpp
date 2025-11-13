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

#include "intl_plural_rules.h"

#include "i18n_hilog.h"
#include "locale_helper.h"
#include "utils.h"

namespace OHOS {
namespace Global {
namespace I18n {
bool IntlPluralRules::initAvailableLocales = false;
std::mutex IntlPluralRules::getLocalesMutex;
std::set<std::string> IntlPluralRules::availableLocales = {};

const int32_t IntlPluralRules::STRING_SEPARATOR_LENGTH = 4;
const int32_t IntlPluralRules::MAX_DIGITS_VALUE = 21;
const int32_t IntlPluralRules::MAX_FRACTION_DIGITS_VALUE = 20;
const int32_t IntlPluralRules::MNFD_DEFAULT = 0;
const int32_t IntlPluralRules::MXFD_DEFAULT = 3;

const std::string IntlPluralRules::LOCALE_MATCHER_TAG = "localeMatcher";
const std::string IntlPluralRules::TYPE_TAG = "type";
const std::string IntlPluralRules::MIN_INTEGER_DIGITS = "minimumIntegerDigits";
const std::string IntlPluralRules::MIN_SIGNALE_DIGITS = "minimumSignificantDigits";
const std::string IntlPluralRules::MAX_SIGNALE_DIGITS = "maximumSignificantDigits";
const std::string IntlPluralRules::MIN_FRACTION_DIGITS = "minimumFractionDigits";
const std::string IntlPluralRules::MAX_FRACTION_DIGITS = "maximumFractionDigits";

const std::unordered_set<std::string> IntlPluralRules::LOCALE_MATCHER_VALUE = {
    "best fit", "lookup"
};

const std::unordered_set<std::string> IntlPluralRules::TYPE_VALUE = {
    "cardinal", "ordinal"
};

const std::unordered_map<std::string, UPluralType> IntlPluralRules::TYPE_TO_ICU_TYPE = {
    { "ordinal", UPLURAL_TYPE_ORDINAL },
    { "cardinal", UPLURAL_TYPE_CARDINAL },
};

std::vector<std::string> IntlPluralRules::SupportedLocalesOf(const std::vector<std::string>& requestLocales,
    const std::unordered_map<std::string, std::string>& options, ErrorMessage& errorMessage)
{
    std::vector<std::string> undefined = {};
    I18nErrorCode status = I18nErrorCode::SUCCESS;
    auto requestedLocales = LocaleHelper::CanonicalizeLocaleList(requestLocales, status);
    if (status != I18nErrorCode::SUCCESS) {
        errorMessage.type = ErrorType::RANGE_ERROR;
        errorMessage.message = "invalid locale";
        HILOG_ERROR_I18N("IntlPluralRules::SupportedLocalesOf: Call CanonicalizeLocaleList failed.");
        return undefined;
    }

    auto iter = options.find(LOCALE_MATCHER_TAG);
    if (iter != options.end()) {
        if (!IsValidOptionName(LOCALE_MATCHER_VALUE, iter->second)) {
            errorMessage.type = ErrorType::RANGE_ERROR;
            errorMessage.message = "getStringOption failed";
            return undefined;
        }
    }
    return LocaleHelper::LookupSupportedLocales(GetAvailableLocales(), requestedLocales);
}

IntlPluralRules::IntlPluralRules(const std::vector<std::string>& localeTags,
    const std::unordered_map<std::string, std::string>& options, ErrorMessage& errorMessage)
{
    I18nErrorCode status = I18nErrorCode::SUCCESS;
    std::vector<std::string> requestedLocales = LocaleHelper::CanonicalizeLocaleList(localeTags, status);
    if (status != I18nErrorCode::SUCCESS) {
        errorMessage.type = ErrorType::RANGE_ERROR;
        errorMessage.message = "invalid locale";
        HILOG_ERROR_I18N("IntlPluralRules::IntlPluralRules: CanonicalizeLocaleList failed.");
        return;
    }
    if (!ParseOptions(options, errorMessage)) {
        HILOG_ERROR_I18N("IntlPluralRules::IntlPluralRules: Parse configs failed.");
        return;
    }
    if (!ResolveLocale(GetAvailableLocales(), requestedLocales, errorMessage)) {
        HILOG_ERROR_I18N("IntlPluralRules::IntlPluralRules: resolve Locale failed.");
        return;
    }
    if (!InitIntlPluralRules(options, errorMessage)) {
        HILOG_ERROR_I18N("IntlPluralRules::IntlPluralRules: init IntlPluralRules failed.");
        return;
    }
    initSuccess = true;
}

bool IntlPluralRules::InitIntlPluralRules(const std::unordered_map<std::string, std::string>& options,
    ErrorMessage& errorMessage)
{
    icuNumberFormatter = icu::number::NumberFormatter::withLocale(icuLocale).roundingMode(UNUM_ROUND_HALFUP);
    icuNumberFormatter =
        icuNumberFormatter.integerWidth(icu::number::IntegerWidth::zeroFillTo(minimumIntegerDigits));
    if (roundingType != RoundingType::COMPACTROUNDING) {
        icu::number::Precision precision =
            icu::number::Precision::minMaxFraction(minimumFractionDigits, maximumFractionDigits);
        if (minimumSignificantDigits != 0) {
            precision =
                icu::number::Precision::minMaxSignificantDigits(minimumSignificantDigits, maximumSignificantDigits);
        }
        icuNumberFormatter = icuNumberFormatter.precision(precision);
    }
    auto typeIter = TYPE_TO_ICU_TYPE.find(type);
    if (typeIter == TYPE_TO_ICU_TYPE.end()) {
        HILOG_ERROR_I18N("InitIntlPluralRules: this branch is unreachable.");
        return false;
    }
    UPluralType icuType = typeIter->second;
    UErrorCode icuStatus = U_ZERO_ERROR;
    icuPluralRules.reset(icu::PluralRules::forLocale(icuLocale, icuType, icuStatus));
    if (U_FAILURE(icuStatus) || icuPluralRules == nullptr) {
        errorMessage.type = ErrorType::RANGE_ERROR;
        errorMessage.message = "cannot create icuPluralRules";
        return false;
    }
    return true;
}

bool IntlPluralRules::ParseOptions(const std::unordered_map<std::string, std::string>& options,
    ErrorMessage& errorMessage)
{
    auto localeMatcherIter = options.find(LOCALE_MATCHER_TAG);
    if (localeMatcherIter != options.end()) {
        if (!IsValidOptionName(LOCALE_MATCHER_VALUE, localeMatcherIter->second)) {
            errorMessage.type = ErrorType::RANGE_ERROR;
            errorMessage.message = "getStringOption failed";
            HILOG_ERROR_I18N("IntlPluralRules::ParseOptions: localeMatcher is invalid.");
            return false;
        }
        localeMatcher = localeMatcherIter->second;
    } else {
        localeMatcher = "best fit";
    }

    auto typeIter = options.find(TYPE_TAG);
    if (typeIter != options.end()) {
        if (!IsValidOptionName(TYPE_VALUE, typeIter->second)) {
            errorMessage.type = ErrorType::RANGE_ERROR;
            errorMessage.message = "getStringOption failed";
            HILOG_ERROR_I18N("IntlPluralRules::ParseOptions: type is invalid.");
            return false;
        }
        type = typeIter->second;
    } else {
        type = "cardinal";
    }

    if (!SetNumberFormatDigitOptions(options, MNFD_DEFAULT, MXFD_DEFAULT, errorMessage)) {
        return false;
    }
    return true;
}

bool IntlPluralRules::GetNextLocale(icu::StringEnumeration* locales, std::string& validLocale, int32_t* len)
{
    UErrorCode icuStatus = U_ZERO_ERROR;
    const char* locale = locales->next(len, icuStatus);
    if (!U_SUCCESS(icuStatus) || locale == nullptr) {
        validLocale = "";
        return false;
    }
    validLocale = std::string(locale);
    return true;
}

std::set<std::string> IntlPluralRules::GetAvailableLocales()
{
    if (initAvailableLocales) {
        return availableLocales;
    }
    std::lock_guard<std::mutex> getLocalesLock(getLocalesMutex);
    if (initAvailableLocales) {
        return availableLocales;
    }
    UErrorCode icuStatus = U_ZERO_ERROR;
    std::unique_ptr<icu::StringEnumeration> locales(icu::PluralRules::getAvailableLocales(icuStatus));
    if (U_FAILURE(icuStatus)) {
        HILOG_ERROR_I18N("IntlPluralRules::GetAvailableLocales: icu getAvailableLocales failed.");
        return availableLocales;
    }
    std::string validLocale;
    int32_t len = 0;
    while (GetNextLocale(locales.get(), validLocale, &len)) {
        if (len >= STRING_SEPARATOR_LENGTH) {
            std::replace(validLocale.begin(), validLocale.end(), '_', '-');
        }
        availableLocales.insert(validLocale);
    }
    initAvailableLocales = true;
    return availableLocales;
}

bool IntlPluralRules::ResolveLocale(const std::set<std::string>& availableLocales,
    const std::vector<std::string>& requestedLocales, ErrorMessage& errorMessage)
{
    bool initLocale = false;
    std::string availableLocale = LocaleHelper::LookupMatcher(availableLocales, requestedLocales);
    if (!availableLocale.empty()) {
        UErrorCode icuStatus = U_ZERO_ERROR;
        icuLocale = icu::Locale::forLanguageTag(icu::StringPiece(availableLocale), icuStatus);
        if (U_SUCCESS(icuStatus)) {
            initLocale = true;
        }
    }
    if (!initLocale) {
        UErrorCode icuStatus = U_ZERO_ERROR;
        std::string defaultLocale = LocaleHelper::DefaultLocale();
        icuLocale = icu::Locale::forLanguageTag(icu::StringPiece(defaultLocale), icuStatus);
        if (U_FAILURE(icuStatus)) {
            HILOG_ERROR_I18N("IntlPluralRules::ResolveLocale: Create defaultLocale failed: %{public}s.",
                defaultLocale.c_str());
            errorMessage.type = ErrorType::RANGE_ERROR;
            errorMessage.message = "invalid locale";
            return false;
        }
    }
    UErrorCode icuStatus = U_ZERO_ERROR;
    localeStr = icuLocale.toLanguageTag<std::string>(icuStatus);
    if (U_FAILURE(icuStatus)) {
        HILOG_ERROR_I18N("IntlPluralRules::ResolveLocale: Get languageTag from icuLocale failed.");
        errorMessage.type = ErrorType::RANGE_ERROR;
        errorMessage.message = "invalid locale";
        return false;
    }
    return true;
}

bool IntlPluralRules::IsValidOptionName(const std::unordered_set<std::string>& optionValues, const std::string& value)
{
    auto iter = optionValues.find(value);
    if (iter == optionValues.end()) {
        return false;
    }
    return true;
}

int32_t IntlPluralRules::GetDefaultNumberOption(const std::unordered_map<std::string, std::string>& options,
    const std::string& key, int32_t minimum, int32_t maximum, int32_t fallback, ErrorMessage& errorMessage)
{
    auto iter = options.find(key);
    if (iter == options.end()) {
        return fallback;
    }
    int32_t numberStatus = 0;
    double numberResult = ConvertString2Double(iter->second, numberStatus);
    if (numberStatus != 0 || numberResult < minimum || numberResult > maximum) {
        errorMessage.type = ErrorType::RANGE_ERROR;
        errorMessage.message = "";
        HILOG_ERROR_I18N("GetDefaultNumberOption: out of range: %{public}lf", numberResult);
        return fallback;
    }
    return std::floor(numberResult);
}

bool IntlPluralRules::SetSignificantDigitsOptions(const std::unordered_map<std::string, std::string>& options,
    ErrorMessage& errorMessage)
{
    roundingType = RoundingType::SIGNIFICANTDIGITS;
    minimumSignificantDigits = GetDefaultNumberOption(options, MIN_SIGNALE_DIGITS, 1, MAX_DIGITS_VALUE, 1,
        errorMessage);
    if (errorMessage.type != ErrorType::NO_ERROR) {
        return false;
    }
    maximumSignificantDigits = GetDefaultNumberOption(options, MAX_SIGNALE_DIGITS, minimumSignificantDigits,
        MAX_DIGITS_VALUE, MAX_DIGITS_VALUE, errorMessage);
    if (errorMessage.type != ErrorType::NO_ERROR) {
        return false;
    }
    return true;
}

bool IntlPluralRules::SetFractionDigitsOptions(const std::unordered_map<std::string, std::string>& options,
    int32_t mnfdDefault, int32_t mxfdDefault, ErrorMessage& errorMessage)
{
    roundingType = RoundingType::FRACTIONDIGITS;
    auto mxfdIter = options.find(MAX_FRACTION_DIGITS);
    if (mxfdIter != options.end()) {
        int32_t mxfd = 0;
        mxfd = GetDefaultNumberOption(options, MAX_FRACTION_DIGITS, 0, MAX_FRACTION_DIGITS_VALUE, mxfdDefault,
            errorMessage);
        if (errorMessage.type != ErrorType::NO_ERROR) {
            return false;
        }
        mnfdDefault = std::min(mnfdDefault, mxfd);
    }
    minimumFractionDigits = GetDefaultNumberOption(options, MIN_FRACTION_DIGITS,
        0, MAX_FRACTION_DIGITS_VALUE, mnfdDefault, errorMessage);
    if (errorMessage.type != ErrorType::NO_ERROR) {
        return false;
    }
    int32_t mxfdActualDefault = std::max(minimumFractionDigits, mxfdDefault);
    maximumFractionDigits = GetDefaultNumberOption(options, MAX_FRACTION_DIGITS,
        minimumFractionDigits, MAX_FRACTION_DIGITS_VALUE, mxfdActualDefault, errorMessage);
    if (errorMessage.type != ErrorType::NO_ERROR) {
        return false;
    }
    return true;
}

bool IntlPluralRules::SetNumberFormatDigitOptions(const std::unordered_map<std::string, std::string>& options,
    int32_t mnfdDefault, int32_t mxfdDefault, ErrorMessage& errorMessage)
{
    minimumFractionDigits = 0;
    maximumFractionDigits = 0;
    minimumSignificantDigits = 0;
    maximumSignificantDigits = 0;
    minimumIntegerDigits = GetDefaultNumberOption(options, MIN_INTEGER_DIGITS, 1, MAX_DIGITS_VALUE, 1, errorMessage);
    if (errorMessage.type != ErrorType::NO_ERROR) {
        return false;
    }
    auto mnsdIter = options.find(MIN_SIGNALE_DIGITS);
    auto mxsdIter = options.find(MAX_SIGNALE_DIGITS);
    if (mnsdIter != options.end() || mxsdIter != options.end()) {
        if (!SetSignificantDigitsOptions(options, errorMessage)) {
            return false;
        }
        return true;
    }
    auto mnfdIter = options.find(MIN_FRACTION_DIGITS);
    auto mxfdIter = options.find(MAX_FRACTION_DIGITS);
    if (mnfdIter != options.end() || mxfdIter != options.end()) {
        if (!SetFractionDigitsOptions(options, mnfdDefault, mxfdDefault, errorMessage)) {
            return false;
        }
    } else {
        roundingType = RoundingType::FRACTIONDIGITS;
        minimumFractionDigits = mnfdDefault;
        maximumFractionDigits = mxfdDefault;
    }
    return true;
}

std::string IntlPluralRules::Select(double number, ErrorMessage& errorMessage) const
{
    if (!initSuccess || !icuPluralRules) {
        HILOG_ERROR_I18N("IntlPluralRules::Select: Init failed.");
        return "";
    }
    UErrorCode icuStatus = U_ZERO_ERROR;
    icu::number::FormattedNumber formatted = icuNumberFormatter.formatDouble(number, icuStatus);
    if (U_FAILURE(icuStatus)) {
        errorMessage.type = ErrorType::RANGE_ERROR;
        errorMessage.message = "invalid resolve number";
        HILOG_ERROR_I18N("IntlPluralRules::Select: out of range: %{public}lf", number);
        return "";
    }
    icu::UnicodeString uString = icuPluralRules->select(formatted, icuStatus);
    if (U_FAILURE(icuStatus)) {
        errorMessage.type = ErrorType::RANGE_ERROR;
        errorMessage.message = "invalid resolve number";
        HILOG_ERROR_I18N("IntlPluralRules::Select: out of range: %{public}lf", number);
        return "";
    }
    std::string result;
    uString.toUTF8String(result);
    return result;
}

bool IntlPluralRules::GetPluralCategories(ResolvedValue& options) const
{
    std::vector<std::string> result;
    UErrorCode icuStatus = U_ZERO_ERROR;
    std::unique_ptr<icu::StringEnumeration> categories(icuPluralRules->getKeywords(icuStatus));
    if (U_FAILURE(icuStatus)) {
        HILOG_ERROR_I18N("IntlPluralRules::GetPluralCategories: get icu getKeywords failed.");
        return false;
    }
    int32_t count = categories->count(icuStatus);
    if (U_FAILURE(icuStatus)) {
        HILOG_ERROR_I18N("IntlPluralRules::GetPluralCategories: get icu categories count failed.");
        return false;
    }
    for (int32_t i = 0; i < count; i++) {
        const icu::UnicodeString *category = categories->snext(icuStatus);
        if (U_FAILURE(icuStatus)) {
            HILOG_ERROR_I18N("IntlPluralRules::GetPluralCategories: get icu category failed.");
            return false;
        }
        std::string value;
        category->toUTF8String(value);
        result.push_back(value);
    }
    options.pluralCategories = result;
    return true;
}

IntlPluralRules::ResolvedValue IntlPluralRules::ResolvedOptions() const
{
    ResolvedValue options;
    if (!initSuccess || !icuPluralRules) {
        HILOG_ERROR_I18N("IntlPluralRules::ResolvedOptions: Init failed.");
        return options;
    }
    if (!GetPluralCategories(options)) {
        return options;
    }
    options.locale = localeStr;
    options.type = type;
    options.minimumIntegerDigits = minimumIntegerDigits;
    options.roundingType = roundingType;
    if (roundingType == RoundingType::SIGNIFICANTDIGITS) {
        options.minimumDigits = minimumSignificantDigits;
        options.maximumDigits = maximumSignificantDigits;
    } else {
        options.minimumDigits = minimumFractionDigits;
        options.maximumDigits = maximumFractionDigits;
    }
    return options;
}
} // namespace I18n
} // namespace Global
} // namespace OHOS
