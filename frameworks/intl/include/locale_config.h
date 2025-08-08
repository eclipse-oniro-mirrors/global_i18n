/*
 * Copyright (c) 2021-2025 Huawei Device Co., Ltd.
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
#ifndef OHOS_GLOBAL_I18N_LOCALE_CONFIG_H
#define OHOS_GLOBAL_I18N_LOCALE_CONFIG_H

#include <map>
#include <mutex>
#include <vector>
#include <set>
#include <shared_mutex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include "i18n_types.h"
#include "taboo_utils.h"
#include "unicode/calendar.h"
#include "locale_info.h"
#include "unicode/locid.h"
#include "unicode/utypes.h"

namespace OHOS {

#ifdef SUPPORT_GRAPHICS
template <typename T>
class sptr;
class IRemoteObject;
#endif

namespace Global {
namespace I18n {
class LocaleConfig {
public:
    LocaleConfig() = default;
    virtual ~LocaleConfig() = default;

    /**
     * @brief Set the System Language to language Tag.
     *
     * @param languageTag language tag to set.
     * @return I18nErrorCode Return SUCCESS indicates that the setting was successful.
     */
    static I18nErrorCode SetSystemLanguage(const std::string &languageTag, int32_t userId = -1);

    /**
     * @brief Set the System Region to region tag.
     *
     * @param regionTag region tag to set.
     * @return I18nErrorCode Return SUCCESS indicates that the setting was successful.
     */
    static I18nErrorCode SetSystemRegion(const std::string &regionTag, int32_t userId = -1);

    /**
     * @brief Set the System Locale to locale tag.
     *
     * @param localeTag locale tag to set.
     * @return I18nErrorCode Return SUCCESS indicates that the setting was successful.
     */
    static I18nErrorCode SetSystemLocale(const std::string &localeTag, int32_t userId = -1);

    /**
     * @brief Set system hour to 12 or 24-hour clock.
     *
     * @param option When flag is true, the system adopts 24-hour clock;
     * When flag is false, the system adopts 12-hour clock; When flag is empty string, the system adopts
     * system locale default behavior.
     * @return I18nErrorCode Return SUCCESS indicates that the setting was successful.
     */
    static I18nErrorCode Set24HourClock(const std::string &option, int32_t userId = -1);

    /**
     * @brief Set system digit to local digit or Arbic digit.
     *
     * @param flag When flag is True, the system adopts local digit;
     * When flag is False, the system doesn't adopt local digit;
     * @return I18nErrorCode Return SUCCESS indicates that the setting was successful.
     */
    static I18nErrorCode SetUsingLocalDigit(bool flag, int32_t userId = -1);

    /**
     * @brief Set user's preferrence for temperature type.
     *
     * @param type temperature type to set.
     * @return I18nErrorCode Return SUCCESS indicates that the setting was successful.
     */
    static I18nErrorCode SetTemperatureType(const TemperatureType& type, int32_t userId = -1);

    /**
     * @brief Set user's preferrence for first day of week.
     *
     * @param type one of week days.
     * @return I18nErrorCode Return SUCCESS indicates that the setting was successful.
     */
    static I18nErrorCode SetFirstDayOfWeek(const WeekDay& type, int32_t userId = -1);

    static TemperatureType GetTemperatureType();
    static std::string GetTemperatureName(const TemperatureType& type);
    static WeekDay GetFirstDayOfWeek();
    static std::string GetEffectiveLanguage();
    static std::string GetSystemLanguage();
    static std::string GetSystemRegion();
    static std::string GetSystemLocale();
    static std::string GetSystemTimezone();
    static std::unordered_set<std::string> GetSystemLanguages();
    static std::unordered_set<std::string> GetSystemCountries(const std::string& language = "");
    static bool IsSuggested(const std::string &language);
    static bool IsSuggested(const std::string &language, const std::string &region);
    static bool IsSuggestedV15(const std::string &language);
    static bool IsSuggestedV15(const std::string &language, const std::string &region);
    static std::string GetDisplayLanguage(const std::string &language, const std::string &displayLocale,
        bool sentenceCase);
    static std::string GetDisplayRegion(const std::string &region, const std::string &displayLocale,
        bool sentenceCase);
    static bool IsRTL(const std::string &locale);
    static std::string GetValidLocale(const std::string &localeTag);
#ifdef SUPPORT_GRAPHICS
    static bool Is24HourClock(sptr<IRemoteObject> &proxy);
#endif
    static bool IsEmpty24HourClock();
    static bool Is24HourClock();
    static std::string GetSystemHour();
    static bool GetUsingLocalDigit();
    static std::unordered_set<std::string> GetBlockedLanguages();
    static std::unordered_set<std::string> GetBlockedRegions(const std::string& language = "");
    static bool IsValidLanguage(const std::string &language);
    static bool IsValidRegion(const std::string &region);
    static bool IsValidTag(const std::string &tag);
    static bool IsValid24HourClockValue(const std::string &tag);
    static std::string GetSimplifiedLanguage(const std::string& language, int32_t &code);
    static std::string GetSimplifiedSystemLanguage();
    static std::string GetUnicodeWrappedFilePath(const std::string &path, const char delimiter,
        std::shared_ptr<LocaleInfo> localeInfo, std::string &invalidField);
    static const std::string HOUR_EVENT_DATA;
    static const std::string LANGUAGE_KEY;
    static const std::string LOCALE_KEY;
    static const std::string HOUR_KEY;
    static const char *DEFAULT_REGION_KEY;
    static constexpr int CONFIG_LEN = 128;

private:
    static void Split(const std::string &src, const std::string &sep, std::vector<std::string> &dest);
    static void Split(const std::string &src, const std::string &sep, std::unordered_set<std::string> &dest);
    static I18nErrorCode UpdateSystemLocale(const std::string &language);
    static std::string ComputeEffectiveLanguage(const std::string& locale, const std::string& language);
    static constexpr uint32_t LANGUAGE_LEN = 2;
    static constexpr uint32_t LOCALE_ITEM_COUNT = 3;
    static constexpr uint32_t SCRIPT_OFFSET = 2;
    static const char *UPGRADE_LOCALE_KEY;
    static const char *DEFAULT_LOCALE_KEY;
    static const char *DEFAULT_LANGUAGE_KEY;
    static const char *SIM_COUNTRY_CODE_KEY;
    static const char *SUPPORTED_LOCALES_PATH;
    static const char *SUPPORTED_LOCALES_OLD_PATH;
    static const char *SUPPORTED_LOCALES_NAME;
    static const char *SUPPORTED_REGIONS_NAME;
    static const char *WHITE_LANGUAGES_NAME;
    static const char *REGIONS_LANGUAGES_PATH;
    static const char *REGIONS_LANGUAGES_NAME;
    static const char *SUPPORT_LOCALES_PATH;
    static const char *SUPPORT_LOCALES_NAME;
    static const char *DEFAULT_LOCALE;
    static const char *supportLocalesTag;
    static const char *LANG_PATH;
    static const char *rootTag;
    static const char *secondRootTag;
    static const char *rootRegion;
    static const char *secondRootRegion;
    static const uint32_t ELEMENT_NUM = 2;

    static const char *SUPPORTED_LANGUAGE_EN_LATN_PATH;
    static const char *SUPPORTED_LANGUAGE_EN_LATN_NAME;

    static const char *OVERRIDE_SUPPORTED_REGIONS_NAME;
    static const char *OVERRIDE_SUPPORTED_REGIONS_PATH;
    static const char *DIALECT_LANGS_PATH;
    static const char *DIALECT_LANGS_NAME;
    static const char *REGION_PATH;
    static const std::string TIMEZONE_KEY;
    static const std::string DEFAULT_TIMEZONE;
    static std::mutex dialectLocaleMutex;
    static std::mutex region2DisplayNameMutex;
    static std::mutex locale2DisplayNameMutex;
    static std::shared_mutex is24HourLocaleMapMutex;
    static const std::string EXT_PARAM_KEY;
    static const std::string NUMBER_SYSTEM_KEY;
    static const std::string TEMPERATURE_UNIT_KEY;
    static const std::string WEEK_DAY_KEY;

    static const std::unordered_set<std::string>& GetSupportedLocales();
    static const std::unordered_set<std::string>& GetSupportedLocalesV15();
    static const std::unordered_set<std::string>& GetSupportedRegions();
    static void GetCountriesFromSim(std::vector<std::string> &simCountries);
    static void GetRelatedLocales(std::unordered_set<std::string> &relatedLocales,
        const std::vector<std::string> countries);
    static void GetRelatedLocalesV15(std::unordered_set<std::string> &relatedLocales,
        const std::vector<std::string> countries);
    static void GetListFromFile(const char *path, const char *resourceName, std::unordered_set<std::string> &ret);
    static void LoadRegionsLanguages();
    static void Expunge(std::unordered_set<std::string> &src, const std::unordered_set<std::string> &another);
    static std::string GetMainLanguage(const std::string &language,
        std::unordered_map<std::string, std::string> selfDialectMap);
    static std::string GetCountry(const std::string& parameter);
    static std::string GetDisplayLanguageWithDialect(const std::string &language, const std::string &displayLocale);
    static std::string GetDisplayOverrideRegion(const std::string &region, const std::string &displayLocale);
    static std::string ComputeLocale(const std::string &displayLocale);
    static void ReadLangData(const char *langDataPath);
    static void ReadRegionData(const char *regionDataPath);
    static void SetSupportedDialectLocales(const char* key, const char* value);
    static void SetRegion2DisplayName(const char* key, const char* value);
    static void SetLocale2DisplayName(const char* key, const char* value);
    static bool Is24HourLocale(const std::string& systemLocale);
    static bool HasDesignator(const std::string& inFormat, const char designator);
    static void QueryUpgradeLocale();

#ifdef SUPPORT_GRAPHICS
    /**
     * @brief Provided for updating i18n configuration include language and hour24.
     *
     * @param key Indicats configuration key.
     * @param value Indicates configuration value.
     * @param userId Indicats update user id.
     * @param isSwitchUser Indicates whether user switch.
     */
    static void UpdateConfiguration(const char *key, const std::string &value, int32_t userId);

    /**
     * @brief Provided for public i18n event include locale-change event and time-change event.
     *
     * @param eventType Indicates event type.
     * @return I18nErrorCode Return SUCCESS indicates that the setting was successful.
     */
    static I18nErrorCode PublishCommonEvent(const std::string &eventType, int32_t userId);
#endif

    /**
     * @brief Change language part of system locale to languageTag.
     *
     * @param languageTag Indicats language tag use to update locale.
     * @return std::string Return locale tag.
     */
    static std::string UpdateLanguageOfLocale(const std::string &languageTag);

    /**
     * @brief Provided for combining language, script, region, extend param to locale.
     *
     * @param languageTag Indicate language part in locale.
     * @param scriptTag Indicate script part in locale.
     * @param regionTag Indicate region part in locale.
     * @param extendParamTag Indicate extend param part in locale.
     * @return std::string Return locale tag.
     */
    static std::string CreateLocale(const std::string &languageTag, const std::string &scriptTag,
        const std::string &regionTag, const std::string &extendParamTag);

    /**
     * @brief Create a locale tag from region tag. Use icu to fill in missing parts.
     *
     * @param regionTag Indicats region tag used to create locale.
     * @return std::string Return locale tag.
     */
    static std::string CreateLocaleFromRegion(const std::string &regionTag);

    /**
     * @brief Change the region part of system locale to regionTag.
     *
     * @param regionTag Indicates region tag used to update locale.
     * @return std::string Return locale Tag.
     */
    static std::string UpdateRegionOfLocale(const std::string &regionTag);
    static std::string ModifyExtParam(const std::string& locale, const std::string& key, const std::string& value);;
    static std::unordered_map<std::string, std::string> ParseExtParam(const std::string& param);
    static std::string GenerateExtParam(const std::unordered_map<std::string, std::string>& paramMap);
    static void ExtendWhiteLanguages();
    static void ExtendLanguageWithScript(const std::string &languageStr);
    static __attribute__((destructor)) void CleanupXmlResources();
    static std::unordered_set<std::string> supportedLocales;
    static std::unordered_set<std::string> supportedLocalesV15;
    static std::unordered_set<std::string> supportedRegions;
    static std::unordered_set<std::string> supportLocales;
    static std::unordered_set<std::string> dialectLang;
    static std::unordered_set<std::string> overrideSupportedRegions;
    static std::unordered_set<std::string> whiteLanguages;
    static std::set<std::string> extendWhiteLanguageList;
    static std::set<std::string> extendWhiteLanguageListV15;
    static std::map<std::string, std::string> supportedDialectLocales;
    static std::unordered_map<std::string, std::string> dialectMap;
    static std::unordered_map<std::string, std::string> dialectMapV15;
    static std::unordered_map<std::string, std::string> localDigitMap;
    static std::unordered_map<TemperatureType, std::string> temperatureTypeToName;
    static std::unordered_map<std::string, TemperatureType> nameToTemperatureType;
    static std::unordered_set<std::string> fahrenheitUsingRegions;
    static std::unordered_map<WeekDay, std::string> weekDayToName;
    static std::unordered_map<std::string, WeekDay> nameToWeekDay;
    static std::unordered_map<icu::Calendar::EDaysOfWeek, WeekDay> eDaysOfWeekToWeekDay;
    static std::map<std::string, std::string> locale2DisplayName;
    static std::map<std::string, std::string> region2DisplayName;
    static std::unordered_map<std::string, bool> is24HourLocaleMap;
    static std::unordered_map<std::string, std::vector<std::string>> dialectLanguages;
    static std::unordered_map<std::string, std::string> resourceIdMap;
    static std::string currentDialectLocale;
    static std::string currentOverrideRegion;
    static std::set<std::string> validCaTag;
    static std::set<std::string> validCoTag;
    static std::set<std::string> validKnTag;
    static std::set<std::string> validKfTag;
    static std::set<std::string> validNuTag;
    static std::set<std::string> validHcTag;
    static bool listsInitialized;
    static bool InitializeLists();
};
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif
