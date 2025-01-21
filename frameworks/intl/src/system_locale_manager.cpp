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

#include "i18n_hilog.h"
#include "locale_config.h"
#include "system_locale_manager.h"
#include "unicode/calendar.h"
#include "unicode/timezone.h"
#include "utils.h"
#include "i18n_timezone.h"

namespace OHOS {
namespace Global {
namespace I18n {
const char* SystemLocaleManager::SIM_COUNTRY_CODE_KEY = "telephony.sim.countryCode0";

SystemLocaleManager::SystemLocaleManager()
{
    tabooUtils = std::make_unique<TabooUtils>();
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
    std::unordered_set<std::string> blockedLanguages = LocaleConfig::GetBlockedLanguages();
    for (auto it = languages.begin(); it != languages.end(); ++it) {
        if (blockedLanguages.find(*it) != blockedLanguages.end()) {
            continue;
        }
        std::string languageDisplayName = LocaleConfig::GetDisplayLanguage(*it, options.localeTag, true);
        languageDisplayName = tabooUtils->ReplaceLanguageName(*it, options.localeTag, languageDisplayName);
        std::string languageNativeName;
        if (options.isUseLocalName) {
            languageNativeName = LocaleConfig::GetDisplayLanguage(*it, *it, true);
            languageNativeName = tabooUtils->ReplaceLanguageName(*it, *it, languageNativeName);
        }
        bool isSuggestedWithSystemRegion = LocaleConfig::IsSuggested(*it, LocaleConfig::GetSystemRegion());
        std::string simRegion = ReadSystemParameter(SIM_COUNTRY_CODE_KEY, CONFIG_LEN);
        bool isSuggestedWithSimRegion = false;
        if (simRegion.length() > 0) {
            isSuggestedWithSimRegion = LocaleConfig::IsSuggested(*it, simRegion);
        }
        SuggestionType suggestionType = SuggestionType::SUGGESTION_TYPE_NONE;
        if (isSuggestedWithSimRegion) {
            suggestionType = SuggestionType::SUGGESTION_TYPE_SIM;
        } else if (isSuggestedWithSystemRegion) {
            suggestionType = SuggestionType::SUGGESTION_TYPE_RELATED;
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
    std::unordered_set<std::string> blockedRegions = LocaleConfig::GetBlockedRegions();
    std::unordered_set<std::string> blockedLanguageRegions = LocaleConfig::GetLanguageBlockedRegions();
    std::string pseudoProcessedRegion = PseudoLocalizationProcessor("");
    for (auto it = countries.begin(); it != countries.end(); ++it) {
        if (blockedRegions.find(*it) != blockedRegions.end() || blockedLanguageRegions.find(*it) !=
            blockedLanguageRegions.end()) {
            continue;
        }
        std::string regionDisplayName = LocaleConfig::GetDisplayRegion(*it, options.localeTag, true);
        regionDisplayName = tabooUtils->ReplaceCountryName(*it, options.localeTag, regionDisplayName);
        bool isSuggestedRegion = LocaleConfig::IsSuggested(LocaleConfig::GetSystemLanguage(), *it);
        SuggestionType suggestionType = SuggestionType::SUGGESTION_TYPE_NONE;
        if (isSuggestedRegion) {
            suggestionType = SuggestionType::SUGGESTION_TYPE_RELATED;
        }
        LocaleItem item { *it, suggestionType, regionDisplayName, pseudoProcessedRegion };
        localeItemList.push_back(item);
    }
    SortLocaleItemList(localeItemList, options);
    return localeItemList;
}

void SystemLocaleManager::SortLocaleItemList(std::vector<LocaleItem> &localeItemList, const SortOptions &options)
{
    std::vector<std::string> collatorLocaleTags { options.localeTag };
    std::map<std::string, std::string> collatorOptions {};
    Collator *collator = new Collator(collatorLocaleTags, collatorOptions);
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

std::vector<TimeZoneCityItem> SystemLocaleManager::GetTimezoneCityInfoArray(I18nErrorCode& status)
{
    std::vector<TimeZoneCityItem> result;
    status = I18nErrorCode::SUCCESS;
    if (!CheckSystemPermission()) {
        status = I18nErrorCode::NOT_SYSTEM_APP;
        return result;
    }
    std::set<std::string> zoneCityIds = I18nTimeZone::GetAvailableZoneCityIDs();
    std::string locale = LocaleConfig::GetSystemLocale();
    std::string localeBaseName = I18nTimeZone::GetLocaleBaseName(locale);
    std::map<std::string, std::string> displayNameMap = I18nTimeZone::FindCityDisplayNameMap(localeBaseName);
    std::map<std::string, icu::TimeZone*> tzMap;
    bool ifEnforce = GetPseudoLocalizationEnforce();
    for (auto it = zoneCityIds.begin(); it != zoneCityIds.end(); ++it) {
        std::string cityId = *it;
        std::string cityDisplayName = "";
        if (displayNameMap.find(cityId) != displayNameMap.end()) {
            cityDisplayName = displayNameMap.find(cityId)->second;
        }
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
            icuTimeZone->getOffset(date, (UBool)local, rawOffset, dstOffset, status);
            tzMap.insert({timezoneId, icuTimeZone});
        }
        struct TimeZoneCityItem tzCityItem = {
            timezoneId, cityId, PseudoLocalizationProcessor(cityDisplayName, ifEnforce), dstOffset + rawOffset,
            PseudoLocalizationProcessor("", ifEnforce), rawOffset
        };
        result.push_back(tzCityItem);
    }
    SortTimezoneCityItemList(locale, result);
    return result;
}

void SystemLocaleManager::SortTimezoneCityItemList(const std::string &locale,
                                                   std::vector<TimeZoneCityItem> &timezoneCityItemList)
{
    std::vector<std::string> collatorLocaleTags { locale };
    std::map<std::string, std::string> collatorOptions {};
    Collator *collator = new Collator(collatorLocaleTags, collatorOptions);
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
} // I18n
} // Global
} // OHOS