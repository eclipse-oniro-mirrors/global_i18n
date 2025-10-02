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

#ifndef OHOS_GLOBAL_I18N_TABOO_H
#define OHOS_GLOBAL_I18N_TABOO_H

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <mutex>

namespace OHOS {
namespace Global {
namespace I18n {
enum DataFileType {
    CONFIG_FILE,
    DATA_FILE
};

class Taboo {
public:
    Taboo(const std::string& path);
    ~Taboo();
    std::string ReplaceCountryName(const std::string& region, const std::string& displayLanguage,
        const std::string& name);
    std::string ReplaceLanguageName(const std::string& language, const std::string& displayLanguage,
        const std::string& name);
    std::string ReplaceTimeZoneName(const std::string& tzId, const std::string& displayLanguage,
        const std::string& name);
    std::string ReplaceCityName(const std::string& cityId, const std::string& displayLanguage,
        const std::string& name);
    std::string ReplacePhoneLocationName(const std::string& phoneNumber, const std::string& displayLanguage,
        const std::string& name);
    std::unordered_set<std::string> GetBlockedLanguages() const;
    std::unordered_set<std::string> GetBlockedRegions(const std::string& language) const;
    std::unordered_set<std::string> GetBlockedCities() const;
    std::unordered_set<std::string> GetBlockedPhoneNumbers() const;

private:
    void ParseTabooData(const std::string& path, DataFileType fileType, const std::string& Locale = "");
    void ProcessTabooConfigData(const std::string& name, const std::string& value);
    void ProcessTabooLocaleData(const std::string& locale, const std::string& name, const std::string& value);
    void HandleTabooData(DataFileType fileType, const std::string& locale, const std::string& name,
        const std::string& value);
    std::vector<std::string> QueryKeyFallBack(const std::string& key) const;
    std::tuple<std::string, std::string> LanguageFallBack(const std::string& language);
    void ReadResourceList();
    std::string GetLanguageFromFileName(const std::string& fileName);
    std::unordered_set<std::string> GetBlockedRegions() const;
    void ParseBlockedLanguagesAndRegions(const std::string& key, const std::string& value);
    void ParseBlockedCities(const std::string& key, const std::string& value);
    void ParseBlockedPhoneNumbers(const std::string& key, const std::string& value);
    void InitLocaleTabooData(const std::string& fallbackLanguage, const std::string& fileName);
    void InitFallBackSuffixes();
    // Indicates which regions support name replacement using taboo data.
    std::unordered_set<std::string> supportedRegions;
    // Indicates which languages support name replacement using taboo data.
    std::unordered_set<std::string> supportedLanguages;
    // Indicates which time zones support name replacement using taboo data.
    std::unordered_set<std::string> supportedTimeZones;
    // Indicates which cities support name replacement using taboo data.
    std::unordered_set<std::string> supportedcities;
    // Indicates which phone numbers support location name replacement using taboo data.
    std::unordered_set<std::string> supportedPhoneNumbers;
    // cache the name replacement taboo data of different locale.
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> localeTabooData;
    // Indicates which locales are supported to find taboo data.
    std::unordered_map<std::string, std::string> resources;
    std::unordered_set<std::string> initResourcesList;

    std::unordered_map<std::string, std::unordered_set<std::string>> blockedLanguages;
    std::unordered_map<std::string, std::unordered_set<std::string>> blockedRegions;
    std::unordered_map<std::string, std::unordered_map<std::string,
        std::unordered_set<std::string>>> languageBlockedRegions;
    std::unordered_map<std::string, std::unordered_set<std::string>> blockedCities;
    std::unordered_map<std::string, std::unordered_set<std::string>> blockedPhoneNumbers;

    std::vector<std::string> fallBackSuffixes;

    static const std::string TABOO_CONFIG_FILE;
    static const std::string TABOO_DATA_FILE;
    static const std::string FILE_PATH_SPLITOR;
    static const std::string LANGUAGE_REGION_SPLITOR;
    static const std::string TABOO_DATA_SPLITOR;
    static const std::string OMITTED_SYMBOL;
    static const std::string ROOT_TAG;
    static const std::string ITEM_TAG;
    static const std::string NAME_TAG;
    static const std::string VALUE_TAG;

    static const std::string SUPPORTED_LANGUAGE_TAG; // supported languages key in taboo-config.xml
    static const std::string SUPPORTED_REGION_TAG; // supported regions key in taboo-config.xml
    static const std::string SUPPORTED_TIME_ZONE_TAG; // supported time zones key in taboo-config.xml
    static const std::string SUPPORTED_CITY_TAG; // supported cities key in taboo-config.xml
    static const std::string SUPPORTED_PHONE_NUMBER_TAG; // supported phone numbers key in taboo-config.xml
    static const std::string LANGUAGE_KEY; // start part of language name replacement data key.
    static const std::string REGION_KEY; // start part of region name replacement data key.
    static const std::string TIME_ZONE_KEY; // start part of time zone name replacement data key.
    static const std::string CITY_KEY;  // start part of city name replacement data key.
    static const std::string PHONE_NUMBER_KEY; // start part of phone number location name replacement data key.
    static const std::string BLOCKED_LANG_TAG;
    static const std::string BLOCKED_CITY_TAG;
    static const std::string BLOCKED_PHONE_NUMBER_TAG;

    static const std::string MCC_MNC_FIRST_KEY;
    static const std::string MCC_MNC_SECOND_KEY;
    static const std::string CUST_REGION_KEY;
    static const std::string MCC_MNC_TAG;
    static const std::string MCC_TAG;
    static const std::string CUST_REGION_TAG;
    static const std::string DEFAULT_REGION_TAG;
    static const std::string COMMON_SUFFIX;
    static const size_t MCC_MNC_LENGTH;
    static const size_t MCC_LENGTH;

    std::string tabooDataPath;
    bool isTabooDataExist = false;
    std::mutex tabooMutex;
};
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif