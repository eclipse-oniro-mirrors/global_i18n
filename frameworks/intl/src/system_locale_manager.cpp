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

#include <cctype>
#include "i18n_hilog.h"
#include "locale_config.h"
#include "system_locale_manager.h"
#include "unicode/calendar.h"
#include "unicode/timezone.h"
#include "utils.h"
#include "i18n_timezone.h"
#include "unicode/localebuilder.h"
#include "unicode/locid.h"

namespace OHOS {
namespace Global {
namespace I18n {
const std::unordered_set<std::string> SystemLocaleManager::supportLocales {
    "ar-Arab-EG",
    "az-Cyrl-AZ",
    "bo-Tibt-CN",
    "jv-Latn-ID",
    "sr-Latn-RS",
    "be-Cyrl-BY",
    "bg-Cyrl-BG",
    "bn-Beng-BD",
    "bs-Cyrl-BA",
    "ca-Latn-ES",
    "cs-Latn-CZ",
    "da-Latn-DK",
    "de-Latn-DE",
    "de-Latn-LI",
    "de-Latn-CH",
    "de-Latn-AT",
    "el-Grek-GR",
    "en-Latn-AU",
    "en-Latn-CA",
    "en-Latn-GB",
    "en-Latn-IN",
    "en-Latn-NZ",
    "en-Latn-SG",
    "en-Latn-US",
    "en-Qaag-US",
    "es-Latn-ES",
    "es-Latn-US",
    "et-Latn-EE",
    "eu-Latn-ES",
    "fa-Arab-IR",
    "fa-Arab-AF",
    "fi-Latn-FI",
    "fr-Latn-FR",
    "fr-Latn-CA",
    "fr-Latn-BE",
    "fr-Latn-CH",
    "gl-Latn-ES",
    "gu-Gujr-IN",
    "hi-Deva-IN",
    "hr-Latn-HR",
    "hu-Latn-HU",
    "in-Latn-ID",
    "id-Latn-ID",
    "it-Latn-IT",
    "it-Latn-CH",
    "fil-Latn-PH",
    "iw-Hebr-IL",
    "he-Hebr-IL",
    "ja-Jpan-JP",
    "ka-Geor-GE",
    "kk-Cyrl-KZ",
    "km-Khmr-KH",
    "kn-Knda-IN",
    "ko-Kore-KR",
    "lo-Laoo-LA",
    "lt-Latn-LT",
    "lv-Latn-LV",
    "mai-Deva-IN",
    "mk-Cyrl-MK",
    "mi-Latn-NZ",
    "ml-Mlym-IN",
    "mn-Cyrl-MN",
    "mr-Deva-IN",
    "ms-Latn-MY",
    "my-Mymr-MM",
    "my-Qaag-MM",
    "nb-Latn-NO",
    "ne-Deva-NP",
    "nl-Latn-NL",
    "nl-Latn-BE",
    "or-Orya-IN",
    "pa-Arab-PK",
    "pa-Guru-IN",
    "pl-Latn-PL",
    "pt-Latn-BR",
    "pt-Latn-PT",
    "ro-Latn-RO",
    "ru-Cyrl-RU",
    "si-Sinh-LK",
    "sk-Latn-SK",
    "sl-Latn-SI",
    "sv-Latn-SE",
    "sw-Latn-KE",
    "sw-Latn-TZ",
    "ta-Taml-IN",
    "te-Telu-IN",
    "th-Thai-TH",
    "tr-Latn-TR",
    "ug-Arab-CN",
    "uk-Cyrl-UA",
    "ur-Arab-PK",
    "uz-Cyrl-UZ",
    "uz-Latn-UZ",
    "vi-Latn-VN",
    "zh-Hans-CN",
    "zh-Hant-HK",
    "zh-Hant-TW",
};

std::unordered_map<std::string, std::string> SystemLocaleManager::custLanguageMap {
    { "es-US", "es-419" },
    { "pt-PT", "pt" }
};

SystemLocaleManager::SystemLocaleManager()
{
}

SystemLocaleManager::~SystemLocaleManager()
{
}

/**
 * Language arrays are sorted according to the following steps:
 * 1. Remove blocked languages.
 * 2. Compute language locale displayName; If options.isUseLocalName is true, compute language local displayName.
 *    replace display name with taboo data.
 * 3. Judge whether language is suggested with system region and sim card region.
 * 4. Sort the languages use locale displayName, local displyName and suggestion infomation.
 */
std::vector<LocaleItem> SystemLocaleManager::GetLanguageInfoArray(const std::vector<std::string> &languages,
    const SortOptions &options, I18nErrorCode &status)
{
    std::vector<LocaleItem> localeItemList;
    status = I18nErrorCode::SUCCESS;
    if (!CheckSystemPermission()) {
        status = I18nErrorCode::NOT_SYSTEM_APP;
        return localeItemList;
    }
    
    std::unordered_set<std::string> simRegions = GetCountryCodeFromSimCard();
    for (auto it = languages.begin(); it != languages.end(); ++it) {
        std::string language = *it;
        if (custLanguageMap.find(language) != custLanguageMap.end()) {
            language = custLanguageMap[language];
        }
        std::string languageDisplayName = LocaleConfig::GetDisplayLanguage(language, options.localeTag, true);
        std::string languageNativeName;
        if (options.isUseLocalName) {
            languageNativeName = LocaleConfig::GetDisplayLanguage(language, language, true);
        }
        bool isSuggestedWithSystemRegion = LocaleConfig::IsSuggested(*it, LocaleConfig::GetSystemRegion());
        SuggestionType suggestionType = SuggestionType::SUGGESTION_TYPE_NONE;
        if (isSuggestedWithSystemRegion) {
            suggestionType = SuggestionType::SUGGESTION_TYPE_RELATED;
        } else if (IsLanguageSimRegionSuggest(simRegions, *it)) {
            suggestionType = SuggestionType::SUGGESTION_TYPE_SIM;
        }
        LocaleItem item { *it, suggestionType, languageDisplayName, languageNativeName };
        localeItemList.push_back(item);
    }
    SortLocaleItemList(localeItemList, options);
    return localeItemList;
}

/**
 * Region arrays are sorted according to the following steps:
 * 1. Remove blocked regions and blocked regions under system Language.
 * 2. Compute region locale displayName; replace display name with taboo data.
 * 3. Judge whether region is suggested with system language.
 * 4. Sort the regions use locale displayName and suggestion infomation.
 */
std::vector<LocaleItem> SystemLocaleManager::GetCountryInfoArray(const std::vector<std::string> &countries,
    const SortOptions &options, I18nErrorCode &status)
{
    std::vector<LocaleItem> localeItemList;
    status = I18nErrorCode::SUCCESS;
    if (!CheckSystemPermission()) {
        status = I18nErrorCode::NOT_SYSTEM_APP;
        return localeItemList;
    }
    std::string systemLanguage = LocaleConfig::GetSystemLanguage();
    std::unordered_set<std::string> simRegions = GetCountryCodeFromSimCard();
    for (auto it = countries.begin(); it != countries.end(); ++it) {
        bool isSuggestedRegion = IsSuggestRegion(systemLanguage, *it);
        SuggestionType suggestionType = SuggestionType::SUGGESTION_TYPE_NONE;
        if (isSuggestedRegion) {
            suggestionType = SuggestionType::SUGGESTION_TYPE_RELATED;
        } else if (simRegions.find(*it) != simRegions.end()) {
            suggestionType = SuggestionType::SUGGESTION_TYPE_SIM;
        }
        LocaleItem item = GetLocaleItem(*it, suggestionType, options.localeTag);
        localeItemList.push_back(item);
    }
    SortLocaleItemList(localeItemList, options);
    return localeItemList;
}

LocaleItem SystemLocaleManager::GetLocaleItem(const std::string &region,
    SuggestionType &suggestionType, const std::string &localeTag)
{
    std::string pseudoProcessedRegion = PseudoLocalizationProcessor("");
    std::string regionName = LocaleConfig::GetDisplayRegion(region, localeTag, true);
    LocaleItem item { region, suggestionType, regionName, pseudoProcessedRegion };
    return item;
}

void SystemLocaleManager::SortLocaleItemList(std::vector<LocaleItem> &localeItemList, const SortOptions &options)
{
    std::vector<std::string> collatorLocaleTags { options.localeTag };
    std::map<std::string, std::string> collatorOptions {};
    Collator *collator = new (std::nothrow) Collator(collatorLocaleTags, collatorOptions);
    if (collator == nullptr) {
        return;
    }
    auto compareFunc = [collator, options](LocaleItem item1, LocaleItem item2) {
        if (options.isSuggestedFirst) {
            if (item1.suggestionType < item2.suggestionType) {
                return false;
            } else if (item1.suggestionType > item2.suggestionType) {
                return true;
            }
        }
        CompareResult result = CompareResult::INVALID;
        if (item1.localName.length() != 0) {
            result = collator->Compare(item1.localName, item2.localName);
            if (result == CompareResult::SMALLER) {
                return true;
            }
            if (result == CompareResult::INVALID) {
                HILOG_ERROR_I18N("SystemLocaleManager: invalid compare result for local name.");
            }
            return false;
        }
        result = collator->Compare(item1.displayName, item2.displayName);
        if (result == CompareResult::SMALLER) {
            return true;
        }
        if (result == CompareResult::INVALID) {
            HILOG_ERROR_I18N("SystemLocaleManager: invalid compare result for display name.");
        }
        return false;
    };
    std::sort(localeItemList.begin(), localeItemList.end(), compareFunc);
    delete collator;
}

bool SystemLocaleManager::IsLanguageSimRegionSuggest(const std::unordered_set<std::string> &simRegions,
    const std::string &language)
{
    for (auto iter = simRegions.begin(); iter != simRegions.end(); ++iter) {
        if (LocaleConfig::IsSuggested(language, *iter)) {
            return true;
        }
    }
    return false;
}

bool SystemLocaleManager::IsSuggestRegion(const std::string &language, const std::string &region)
{
    UErrorCode status = U_ZERO_ERROR;
    icu::Locale origin = icu::Locale::forLanguageTag(language, status);
    if (U_FAILURE(status)) {
        HILOG_ERROR_I18N("SystemLocaleManager: create locale for %{public}s failed.", language.c_str());
        return false;
    }
    origin.addLikelySubtags(status);
    if (U_FAILURE(status)) {
        HILOG_ERROR_I18N("SystemLocaleManager: addLikelySubtags for %{public}s failed.", language.c_str());
        return false;
    }
    icu::LocaleBuilder builder = icu::LocaleBuilder().setLanguage(origin.getLanguage()).
        setScript(origin.getScript()).setRegion(region);
    icu::Locale temp = builder.setExtension('u', "").build(status);
    if (U_FAILURE(status)) {
        HILOG_ERROR_I18N("SystemLocaleManager: LocaleBuilder for %{public}s, %{public}s failed.",
            language.c_str(), region.c_str());
        return false;
    }
    std::string fullLanguage = temp.toLanguageTag<std::string>(status);
    if (U_FAILURE(status)) {
        HILOG_ERROR_I18N("SystemLocaleManager: toLanguageTag for %{public}s failed.", language.c_str());
        return false;
    }
    return supportLocales.find(fullLanguage) != supportLocales.end();
}

std::vector<TimeZoneCityItem> SystemLocaleManager::GetTimezoneCityInfoArray(I18nErrorCode& status)
{
    std::vector<TimeZoneCityItem> result;
    status = I18nErrorCode::SUCCESS;
    if (!CheckSystemPermission()) {
        status = I18nErrorCode::NOT_SYSTEM_APP;
        return result;
    }
    result = GetTimezoneCityInfoArray();
    SortTimezoneCityItemList(LocaleConfig::GetSystemLocale(), result);
    return result;
}

std::vector<TimeZoneCityItem> SystemLocaleManager::GetTimezoneCityInfoArray()
{
    std::vector<TimeZoneCityItem> result;
    std::unordered_set<std::string> zoneCityIds = I18nTimeZone::GetAvailableZoneCityIDs();
    std::string locale = LocaleConfig::GetSystemLocale();
    std::string localeBaseName = I18nTimeZone::GetLocaleBaseName(locale);
    std::map<std::string, std::string> displayNameMap = I18nTimeZone::FindCityDisplayNameMap(localeBaseName);
    std::map<std::string, icu::TimeZone*> tzMap;
    for (const auto& cityId : zoneCityIds) {
        if (displayNameMap.find(cityId) == displayNameMap.end()) {
            continue;
        }
        std::string cityDisplayName = displayNameMap.find(cityId)->second;
        cityDisplayName = GetCityDisplayNameWithTaboo(cityId, localeBaseName, cityDisplayName);
        int32_t rawOffset = 0;
        int32_t dstOffset = 0;
        bool local = false;
        UErrorCode status = U_ZERO_ERROR;
        UDate date = icu::Calendar::getNow();
        std::string timezoneId = I18nTimeZone::GetTimezoneIdByCityId(cityId);
        if (timezoneId.length() == 0) {
            continue;
        }
        if (tzMap.find(timezoneId) != tzMap.end()) {
            icu::TimeZone *icuTimeZone = tzMap.find(timezoneId)->second;
            icuTimeZone->getOffset(date, (UBool)local, rawOffset, dstOffset, status);
        } else {
            icu::UnicodeString unicodeString = icu::UnicodeString::fromUTF8(timezoneId);
            icu::TimeZone *icuTimeZone = icu::TimeZone::createTimeZone(unicodeString);
            if (icuTimeZone == nullptr) {
                continue;
            }
            icuTimeZone->getOffset(date, (UBool)local, rawOffset, dstOffset, status);
            tzMap.insert({timezoneId, icuTimeZone});
        }
        struct TimeZoneCityItem tzCityItem = {
            timezoneId, cityId, PseudoLocalizationProcessor(cityDisplayName), dstOffset + rawOffset,
            PseudoLocalizationProcessor(""), rawOffset
        };
        result.push_back(tzCityItem);
    }
    for (auto it = tzMap.begin(); it != tzMap.end(); ++it) {
        delete it->second;
        it->second = nullptr;
    }
    return result;
}

void SystemLocaleManager::SortTimezoneCityItemList(const std::string &locale,
                                                   std::vector<TimeZoneCityItem> &timezoneCityItemList)
{
    std::vector<std::string> collatorLocaleTags { locale };
    std::map<std::string, std::string> collatorOptions {};
    Collator *collator = new (std::nothrow) Collator(collatorLocaleTags, collatorOptions);
    if (collator == nullptr) {
        return;
    }
    auto sortFunc = [collator](TimeZoneCityItem item1, TimeZoneCityItem item2) {
        CompareResult result = CompareResult::INVALID;
        result = collator->Compare(item1.cityDisplayName, item2.cityDisplayName);
        if (result == CompareResult::SMALLER) {
            return true;
        }
        if (result == CompareResult::INVALID) {
            HILOG_ERROR_I18N("SystemLocaleManager: invalid compare result for city display name.");
        }
        return false;
    };
    std::sort(timezoneCityItemList.begin(), timezoneCityItemList.end(), sortFunc);
    delete collator;
}

std::unordered_set<std::string> SystemLocaleManager::GetCountryCodeFromSimCard()
{
    std::unordered_set<std::string> resultSet;
    std::vector<size_t> soltIds = {0, 1};
    for (size_t i = 0; i < soltIds.size(); i++) {
        GetCountryCodeFromSimCardInner(resultSet, soltIds[i]);
    }
    return resultSet;
}

void SystemLocaleManager::GetCountryCodeFromSimCardInner(std::unordered_set<std::string> &resultSet,
    size_t soltId)
{
}

std::string SystemLocaleManager::GetCityDisplayNameWithTaboo(const std::string &cityId,
    const std::string &localeBaseName, const std::string &cityDisplayName)
{
    TabooUtils* tabooUtils = TabooUtils::GetInstance();
    std::string requestLocale = StrReplaceAll(localeBaseName, "_", "-");
    if (tabooUtils != nullptr) {
        return tabooUtils->ReplaceCityName(cityId, requestLocale, cityDisplayName);
    }
    return cityDisplayName;
}
} // I18n
} // Global
} // OHOS