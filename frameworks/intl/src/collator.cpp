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
#include "collator.h"

#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "unicode/strenum.h"
#include "unicode/errorcode.h"
#include "unicode/locid.h"
#include "unicode/stringpiece.h"
#include "unicode/ucol.h"
#include "unicode/uloc.h"
#include "unicode/unistr.h"
#include "unicode/urename.h"

#include "i18n_hilog.h"
#include "locale_config.h"
#include "locale_helper.h"

namespace OHOS {
namespace Global {
namespace I18n {
std::set<std::string> Collator::GetAvailableLocales()
{
    std::set<std::string> allLocales;
    int32_t count = 0;
    const icu::Locale *localeList = icu::Collator::getAvailableLocales(count);
    if (localeList == nullptr) {
        HILOG_ERROR_I18N("Collator::GetAvailableLocales: getAvailableLocales nullptr.");
        return allLocales;
    }
    for (int32_t i = 0; i < count; ++i) {
        allLocales.insert(localeList[i].getName());
    }
    return allLocales;
}

std::vector<std::string> Collator::SupportedLocalesOf(const std::vector<std::string> &requestLocales,
                                                      const std::map<std::string, std::string> &configs,
                                                      I18nErrorCode &status)
{
    std::vector<std::string> undefined = {};
    auto requestedLocales = LocaleHelper::CanonicalizeLocaleList(requestLocales, status);
    if (status != I18nErrorCode::SUCCESS) {
        return undefined;
    }

    std::string localeMatcher = LocaleHelper::ParseOption(configs, "localeMatcher", "best fit", true, status);
    if (status != I18nErrorCode::SUCCESS) {
        return undefined;
    }
    std::set<std::string> availableLocales = GetAvailableLocales();
    return LocaleHelper::LookupSupportedLocales(availableLocales, requestedLocales);
}

void Collator::ParseAllOptions(std::map<std::string, std::string> &options)
{
    localeMatcher = LocaleHelper::ParseOption(options, "localeMatcher", "best fit", true, i18nStatus);
    usage = LocaleHelper::ParseOption(options, "usage", "sort", true, i18nStatus);
    sensitivity = LocaleHelper::ParseOption(options, "sensitivity", "variant", true, i18nStatus);
    ignorePunctuation = LocaleHelper::ParseOption(options, "ignorePunctuation", "false", true, i18nStatus);
    numeric = LocaleHelper::ParseOption(options, "numeric", "", true, i18nStatus);
    caseFirst = LocaleHelper::ParseOption(options, "caseFirst", "", true, i18nStatus);
    collation = LocaleHelper::ParseOptionWithoutCheck(options, "collation", "");
}

Collator::Collator(std::vector<std::string> &localeTags, std::map<std::string, std::string> &options,
    const std::string &defaultLocale)
{
    i18nStatus = I18nErrorCode::SUCCESS;
    auto requestedLocales = LocaleHelper::CanonicalizeLocaleList(localeTags, i18nStatus);
    if (i18nStatus != I18nErrorCode::SUCCESS) {
        return;
    }
    Init(requestedLocales, options, defaultLocale);
}

Collator::Collator(std::vector<std::string> &localeTags, std::map<std::string, std::string> &options)
{
    Init(localeTags, options, LocaleConfig::GetEffectiveLocale());
}

void Collator::Init(std::vector<std::string> &localeTags, std::map<std::string, std::string> &options,
    const std::string &defaultLocale)
{
    i18nStatus = I18nErrorCode::SUCCESS;
    ParseAllOptions(options);

    localeTags.push_back(defaultLocale);
    for (size_t i = 0; i < localeTags.size(); i++) {
        std::string curLocale = localeTags[i];
        UErrorCode status = U_ZERO_ERROR;
        locale = icu::Locale::forLanguageTag(icu::StringPiece(curLocale), status);
        if (U_FAILURE(status)) {
            continue;
        }
        if (LocaleInfo::allValidLocales.count(locale.getLanguage()) > 0) {
            localeInfo = std::make_unique<LocaleInfo>(curLocale, options);
            if (!localeInfo->InitSuccess()) {
                continue;
            }
            locale = localeInfo->GetLocale();
            localeStr = localeInfo->GetBaseName();
            createSuccess = InitCollator();
            if (!createSuccess) {
                continue;
            }
            break;
        }
    }
}

bool Collator::IsValidCollation(std::string &collation)
{
    const char *currentCollation = uloc_toLegacyType("collation", collation.c_str());
    if (currentCollation == nullptr) {
        return false;
    }
    UErrorCode status = U_ZERO_ERROR;
    std::unique_ptr<icu::StringEnumeration> enumeration(
        icu::Collator::getKeywordValuesForLocale("collation", icu::Locale(locale.getBaseName()), false, status));
    if (U_FAILURE(status)) {
        return false;
    }
    int length;
    const char *validCollations = nullptr;
    if (enumeration != nullptr) {
        validCollations = enumeration->next(&length, status);
    }
    while (validCollations != nullptr) {
        if (!strcmp(validCollations, currentCollation)) {
            return true;
        }
        if (enumeration != nullptr) {
            validCollations = enumeration->next(&length, status);
        }
    }
    return false;
}

void Collator::SetCollation()
{
    UErrorCode status = U_ZERO_ERROR;
    if (!collation.empty()) {
        if (IsValidCollation(collation)) {
            locale.setUnicodeKeywordValue("co", collation, status);
        } else {
            collation = "default";
            locale.setUnicodeKeywordValue("co", nullptr, status);
        }
    } else {
        collation = localeInfo->GetCollation();
        if (!collation.empty()) {
            if (IsValidCollation(collation)) {
                locale.setUnicodeKeywordValue("co", collation, status);
            } else {
                locale.setUnicodeKeywordValue("co", nullptr, status);
                collation = "default";
            }
        } else {
            locale.setUnicodeKeywordValue("co", nullptr, status);
            collation = "default";
        }
    }
    if (U_FAILURE(status)) {
        HILOG_ERROR_I18N("Collator::SetCollation: Set unicode key word value failed.");
    }
}

void Collator::SetUsage()
{
    if (usage == "search") {
        collation = "default";
        UErrorCode status = U_ZERO_ERROR;
        locale.setUnicodeKeywordValue("co", nullptr, status);
        if (U_FAILURE(status)) {
            HILOG_ERROR_I18N("Collator::SetUsage: Set unicode key word value failed.");
        }
    }
}

void Collator::SetNumeric()
{
    if (!collatorPtr) {
        return;
    }
    if (numeric.empty()) {
        numeric = localeInfo->GetNumeric();
        if (numeric != "true" && numeric != "false") {
            numeric = "false";
        }
    }
    UErrorCode status = U_ZERO_ERROR;
    if (numeric == "true") {
        collatorPtr->setAttribute(UColAttribute::UCOL_NUMERIC_COLLATION,
            UColAttributeValue::UCOL_ON, status);
    } else {
        collatorPtr->setAttribute(UColAttribute::UCOL_NUMERIC_COLLATION,
            UColAttributeValue::UCOL_OFF, status);
    }
    if (U_FAILURE(status)) {
        HILOG_ERROR_I18N("Collator::SetNumeric: Set attribute failed.");
    }
}

void Collator::SetCaseFirst()
{
    if (!collatorPtr) {
        return;
    }
    if (caseFirst.empty()) {
        caseFirst = localeInfo->GetCaseFirst();
        if (caseFirst != "upper" && caseFirst != "lower" && caseFirst != "false") {
            caseFirst = "false";
        }
    }
    UErrorCode status = U_ZERO_ERROR;
    if (caseFirst == "upper") {
        collatorPtr->setAttribute(UColAttribute::UCOL_CASE_FIRST,
            UColAttributeValue::UCOL_UPPER_FIRST, status);
    } else if (caseFirst == "lower") {
        collatorPtr->setAttribute(UColAttribute::UCOL_CASE_FIRST,
            UColAttributeValue::UCOL_LOWER_FIRST, status);
    } else {
        collatorPtr->setAttribute(UColAttribute::UCOL_CASE_FIRST,
            UColAttributeValue::UCOL_OFF, status);
    }
    if (U_FAILURE(status)) {
        HILOG_ERROR_I18N("Collator::SetCaseFirst: Set attribute failed.");
    }
}

void Collator::SetSensitivity()
{
    if (!collatorPtr) {
        return;
    }
    if (sensitivity == "base") {
        collatorPtr->setStrength(icu::Collator::PRIMARY);
    } else if (sensitivity == "accent") {
        collatorPtr->setStrength(icu::Collator::SECONDARY);
    } else if (sensitivity == "case") {
        collatorPtr->setStrength(icu::Collator::PRIMARY);
        UErrorCode status = U_ZERO_ERROR;
        collatorPtr->setAttribute(UColAttribute::UCOL_CASE_LEVEL,
            UColAttributeValue::UCOL_ON, status);
    } else {
        collatorPtr->setStrength(icu::Collator::TERTIARY);
    }
}

void Collator::SetIgnorePunctuation()
{
    if (!collatorPtr) {
        return;
    }
    if (ignorePunctuation == "true") {
        UErrorCode status = U_ZERO_ERROR;
        collatorPtr->setAttribute(UColAttribute::UCOL_ALTERNATE_HANDLING,
            UColAttributeValue::UCOL_SHIFTED, status);
        if (U_FAILURE(status)) {
            HILOG_ERROR_I18N("Collator::SetIgnorePunctuation: Set attribute failed.");
        }
    }
}

bool Collator::InitCollator()
{
    SetCollation();
    SetUsage();
    UErrorCode status = UErrorCode::U_ZERO_ERROR;
    collatorPtr = icu::Collator::createInstance(locale, status);
    if (!U_SUCCESS(status) || collatorPtr == nullptr) {
        if (collatorPtr != nullptr) {
            delete collatorPtr;
            collatorPtr = nullptr;
        }
        return false;
    }
    SetNumeric();
    SetCaseFirst();
    SetSensitivity();
    SetIgnorePunctuation();
    return true;
}

Collator::~Collator()
{
    if (collatorPtr != nullptr) {
        delete collatorPtr;
        collatorPtr = nullptr;
    }
}

CompareResult Collator::Compare(const std::string &first, const std::string &second)
{
    if (!collatorPtr) {
        return CompareResult::INVALID;
    }
    icu::Collator::EComparisonResult result = collatorPtr->compare(icu::UnicodeString(first.data(), first.length()),
        icu::UnicodeString(second.data(), second.length()));
    if (result == icu::Collator::EComparisonResult::LESS) {
        return CompareResult::SMALLER;
    } else if (result == icu::Collator::EComparisonResult::EQUAL) {
        return CompareResult::EQUAL;
    } else {
        return CompareResult::GREATER;
    }
}

void Collator::ResolvedOptions(std::map<std::string, std::string> &options)
{
    options.insert(std::pair<std::string, std::string>("localeMatcher", localeMatcher));
    options.insert(std::pair<std::string, std::string>("locale", localeStr));
    options.insert(std::pair<std::string, std::string>("usage", usage));
    options.insert(std::pair<std::string, std::string>("sensitivity", sensitivity));
    options.insert(std::pair<std::string, std::string>("ignorePunctuation", ignorePunctuation));
    options.insert(std::pair<std::string, std::string>("numeric", numeric));
    options.insert(std::pair<std::string, std::string>("caseFirst", caseFirst));
    options.insert(std::pair<std::string, std::string>("collation", collation));
}

I18nErrorCode Collator::GetError() const
{
    return i18nStatus;
}
} // namespace I18n
} // namespace Global
} // namespace OHOS
