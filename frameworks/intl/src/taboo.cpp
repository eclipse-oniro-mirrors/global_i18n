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

#include <cstring>
#include <filesystem>
#include <sys/stat.h>
#include "i18n_hilog.h"
#include "libxml/globals.h"
#include "libxml/tree.h"
#include "libxml/xmlstring.h"
#include "locale_compare.h"
#include "locale_config.h"
#include "utils.h"
#include "taboo.h"

namespace OHOS {
namespace Global {
namespace I18n {
const std::string Taboo::TABOO_CONFIG_FILE = "taboo-config.xml";
const std::string Taboo::TABOO_DATA_FILE = "taboo-data.xml";
const std::string Taboo::FILE_PATH_SPLITOR = "/";
const std::string Taboo::LANGUAGE_REGION_SPLITOR = "_";
const std::string Taboo::TABOO_DATA_SPLITOR = ",";
const std::string Taboo::OMITTED_SYMBOL = "*";
const std::string Taboo::ROOT_TAG = "taboo";
const std::string Taboo::ITEM_TAG = "item";
const std::string Taboo::NAME_TAG = "name";
const std::string Taboo::VALUE_TAG = "value";
const std::string Taboo::SUPPORTED_LANGUAGE_TAG = "languages";
const std::string Taboo::SUPPORTED_REGION_TAG = "regions";
const std::string Taboo::SUPPORTED_TIME_ZONE_TAG = "timezones";
const std::string Taboo::SUPPORTED_CITY_TAG = "cities";
const std::string Taboo::SUPPORTED_PHONE_NUMBER_TAG = "phonenumbers";
const std::string Taboo::LANGUAGE_KEY = "language_";
const std::string Taboo::REGION_KEY = "region_";
const std::string Taboo::TIME_ZONE_KEY = "timezone_";
const std::string Taboo::CITY_KEY = "city_";
const std::string Taboo::PHONE_NUMBER_KEY = "phonenumber_";
const std::string Taboo::BLOCKED_LANG_TAG = "taboo_blocked_lang";
const std::string Taboo::BLOCKED_CITY_TAG = "taboo_blocked_city";
const std::string Taboo::BLOCKED_PHONE_NUMBER_TAG = "taboo_blocked_phonenumber";
const std::string Taboo::MCC_MNC_FIRST_KEY = "telephony.sim.opkey0";
const std::string Taboo::MCC_MNC_SECOND_KEY = "telephony.sim.opkey1";
const std::string Taboo::CUST_REGION_KEY = "const.cust.region";
const std::string Taboo::MCC_MNC_TAG = "_mccmnc_";
const std::string Taboo::MCC_TAG = "_mcc_";
const std::string Taboo::CUST_REGION_TAG = "_vc_";
const std::string Taboo::DEFAULT_REGION_TAG = "_c_";
const std::string Taboo::COMMON_SUFFIX = "_r_all";
const size_t Taboo::MCC_MNC_LENGTH = 5;
const size_t Taboo::MCC_LENGTH = 3;

Taboo::Taboo(const std::string& path) : tabooDataPath(path)
{
    using std::filesystem::directory_iterator;

    std::string tabooConfigFilePath = tabooDataPath + TABOO_CONFIG_FILE;
    struct stat s;
    isTabooDataExist = stat(tabooConfigFilePath.c_str(), &s) == 0;
    if (!isTabooDataExist) {
        HILOG_ERROR_I18N("Taboo::Taboo: Taboo data not exist.");
        return;
    }
    // parse taboo-config.xml to obtain supported regions and languages for name replacement.
    ParseTabooData(tabooConfigFilePath, DataFileType::CONFIG_FILE);
    ReadResourceList();
    InitFallBackSuffixes();
}

Taboo::~Taboo()
{
}

std::string Taboo::ReplaceCountryName(const std::string& region, const std::string& displayLanguage,
    const std::string& name)
{
    if (!isTabooDataExist) {
        HILOG_ERROR_I18N("Taboo::ReplaceCountryName: Taboo data not exist.");
        return name;
    }
    if (supportedRegions.find(region) == supportedRegions.end()) {
        return name;
    }
    std::string fallbackLanguage;
    std::string fileName;
    std::tie(fallbackLanguage, fileName) = LanguageFallBack(displayLanguage);
    if (fallbackLanguage.empty()) {
        return name;
    }
    InitLocaleTabooData(fallbackLanguage, fileName);
    auto tabooData = localeTabooData[fallbackLanguage];
    std::string key = REGION_KEY + region;
    std::vector<std::string> fallbackRegionKeys = QueryKeyFallBack(key);
    for (auto& fallbackRegionKey : fallbackRegionKeys) {
        if (tabooData.find(fallbackRegionKey) != tabooData.end()) {
            return tabooData.at(fallbackRegionKey);
        }
    }
    return name;
}

std::string Taboo::ReplaceLanguageName(const std::string& language, const std::string& displayLanguage,
    const std::string& name)
{
    if (!isTabooDataExist) {
        HILOG_ERROR_I18N("Taboo::ReplaceCountryName: Taboo data not exist.");
        return name;
    }
    if (supportedLanguages.find(language) == supportedLanguages.end()) {
        return name;
    }
    std::string fallbackLanguage;
    std::string fileName;
    std::tie(fallbackLanguage, fileName) = LanguageFallBack(displayLanguage);
    if (fallbackLanguage.empty()) {
        return name;
    }
    InitLocaleTabooData(fallbackLanguage, fileName);
    auto tabooData = localeTabooData[fallbackLanguage];
    std::string key = LANGUAGE_KEY + language;
    std::vector<std::string> fallbackLanguageKeys = QueryKeyFallBack(key);
    for (const auto& fallbackLanguageKey : fallbackLanguageKeys) {
        if (tabooData.find(fallbackLanguageKey) != tabooData.end()) {
            return tabooData.at(fallbackLanguageKey);
        }
    }
    return name;
}

std::string Taboo::ReplaceTimeZoneName(const std::string& tzId, const std::string& displayLanguage,
    const std::string& name)
{
    if (!isTabooDataExist) {
        HILOG_ERROR_I18N("Taboo::ReplaceCountryName: Taboo data not exist.");
        return name;
    }
    if (supportedTimeZones.find(tzId) == supportedTimeZones.end()) {
        return name;
    }
    std::string fallbackLanguage;
    std::string fileName;
    std::tie(fallbackLanguage, fileName) = LanguageFallBack(displayLanguage);
    if (fallbackLanguage.empty()) {
        return name;
    }
    InitLocaleTabooData(fallbackLanguage, fileName);
    auto tabooData = localeTabooData[fallbackLanguage];
    std::string key = TIME_ZONE_KEY + tzId;
    std::vector<std::string> fallbackTimeZoneKeys = QueryKeyFallBack(key);
    for (const auto& fallbackTimeZoneKey : fallbackTimeZoneKeys) {
        if (tabooData.find(fallbackTimeZoneKey) != tabooData.end()) {
            return tabooData.at(fallbackTimeZoneKey);
        }
    }
    return name;
}

std::string Taboo::ReplaceCityName(const std::string& cityId, const std::string& displayLanguage,
    const std::string& name)
{
    if (!isTabooDataExist) {
        HILOG_ERROR_I18N("Taboo::ReplaceCityName: Taboo data not exist.");
        return name;
    }
    if (supportedcities.find(cityId) == supportedcities.end()) {
        return name;
    }
    std::string fallbackLanguage;
    std::string fileName;
    std::tie(fallbackLanguage, fileName) = LanguageFallBack(displayLanguage);
    if (fallbackLanguage.empty()) {
        return name;
    }
    InitLocaleTabooData(fallbackLanguage, fileName);
    auto tabooData = localeTabooData[fallbackLanguage];
    std::string key = CITY_KEY + cityId;
    std::vector<std::string> fallbackCityKeys = QueryKeyFallBack(key);
    for (const auto& fallbackCityKey : fallbackCityKeys) {
        if (tabooData.find(fallbackCityKey) != tabooData.end()) {
            return tabooData.at(fallbackCityKey);
        }
    }
    return name;
}

std::string Taboo::ReplacePhoneLocationName(const std::string& phoneNumber, const std::string& displayLanguage,
    const std::string& name)
{
    if (!isTabooDataExist) {
        HILOG_ERROR_I18N("Taboo::ReplaceCountryName: Taboo data not exist.");
        return name;
    }
    std::string prefix;
    for (const auto& supportedPhoneNumber : supportedPhoneNumbers) {
        if (phoneNumber.compare(0, supportedPhoneNumber.length(), supportedPhoneNumber) == 0) {
            prefix = supportedPhoneNumber;
            break;
        }
    }
    if (prefix.empty()) {
        return name;
    }
    std::string fallbackLanguage;
    std::string fileName;
    std::tie(fallbackLanguage, fileName) = LanguageFallBack(displayLanguage);
    if (fallbackLanguage.empty()) {
        return name;
    }
    InitLocaleTabooData(fallbackLanguage, fileName);
    auto tabooData = localeTabooData[fallbackLanguage];
    std::string key = PHONE_NUMBER_KEY + prefix;
    std::vector<std::string> fallbackPhoneNunberKeys = QueryKeyFallBack(key);
    for (const auto& fallbackPhoneNunberKey : fallbackPhoneNunberKeys) {
        if (tabooData.find(fallbackPhoneNunberKey) != tabooData.end()) {
            return tabooData.at(fallbackPhoneNunberKey);
        }
    }
    return name;
}

std::unordered_set<std::string> Taboo::GetBlockedLanguages() const
{
    std::vector<std::string> fallbackKeys = QueryKeyFallBack(BLOCKED_LANG_TAG);
    for (const auto& fallbackKey : fallbackKeys) {
        if (blockedLanguages.find(fallbackKey) != blockedLanguages.end()) {
            return blockedLanguages.at(fallbackKey);
        }
    }
    return {};
}

std::unordered_set<std::string> Taboo::GetBlockedRegions() const
{
    std::vector<std::string> fallbackKeys = QueryKeyFallBack(BLOCKED_LANG_TAG);
    for (const auto& fallbackKey : fallbackKeys) {
        if (blockedRegions.find(fallbackKey) != blockedRegions.end()) {
            return blockedRegions.at(fallbackKey);
        }
    }
    return {};
}

std::unordered_set<std::string> Taboo::GetBlockedRegions(const std::string& language) const
{
    std::unordered_set<std::string> result = GetBlockedRegions();
    if (language.empty()) {
        return result;
    }
    std::unordered_set<std::string> temp;
    std::vector<std::string> fallbackKeys = QueryKeyFallBack(BLOCKED_LANG_TAG);
    for (const auto& fallbackKey : fallbackKeys) {
        if (languageBlockedRegions.find(fallbackKey) != languageBlockedRegions.end()) {
            auto languageBlockedRegion = languageBlockedRegions.at(fallbackKey);
            if (languageBlockedRegion.find(language) != languageBlockedRegion.end()) {
                temp = languageBlockedRegion.at(language);
            }
            break;
        }
    }
    result.insert(temp.begin(), temp.end());
    return result;
}

std::unordered_set<std::string> Taboo::GetBlockedCities() const
{
    std::vector<std::string> fallbackKeys = QueryKeyFallBack(BLOCKED_CITY_TAG);
    for (const auto& fallbackKey : fallbackKeys) {
        if (blockedCities.find(fallbackKey) != blockedCities.end()) {
            return blockedCities.at(fallbackKey);
        }
    }
    return {};
}

std::unordered_set<std::string> Taboo::GetBlockedPhoneNumbers() const
{
    std::vector<std::string> fallbackKeys = QueryKeyFallBack(BLOCKED_PHONE_NUMBER_TAG);
    for (const auto& fallbackKey : fallbackKeys) {
        if (blockedPhoneNumbers.find(fallbackKey) != blockedPhoneNumbers.end()) {
            return blockedPhoneNumbers.at(fallbackKey);
        }
    }
    return {};
}

void Taboo::ParseTabooData(const std::string& path, DataFileType fileType, const std::string& locale)
{
    xmlKeepBlanksDefault(0);
    xmlDocPtr doc = xmlParseFile(path.c_str());
    if (doc == nullptr) {
        HILOG_ERROR_I18N("Taboo::ParseTabooData: Parse taboo data file failed: %{public}s", path.c_str());
        return;
    }
    xmlNodePtr cur = xmlDocGetRootElement(doc);
    if (cur == nullptr || xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>(ROOT_TAG.c_str())) != 0) {
        xmlFreeDoc(doc);
        HILOG_ERROR_I18N("Taboo::ParseTabooData: Get root tag from taboo data file failed: %{public}s", path.c_str());
        return;
    }
    cur = cur->xmlChildrenNode;
    const xmlChar* nameTag = reinterpret_cast<const xmlChar*>(NAME_TAG.c_str());
    const xmlChar* valueTag = reinterpret_cast<const xmlChar*>(VALUE_TAG.c_str());
    while (cur != nullptr && xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>(ITEM_TAG.c_str())) == 0) {
        xmlChar* name = xmlGetProp(cur, nameTag);
        xmlChar* value = xmlGetProp(cur, valueTag);
        if (name == nullptr || value == nullptr) {
            HILOG_ERROR_I18N("Taboo::ParseTabooData: Get name and value property failed: %{public}s", path.c_str());
            xmlFree(name);
            xmlFree(value);
            cur = cur->next;
            continue;
        }
        std::string nameStr = reinterpret_cast<const char*>(name);
        std::string valueStr = reinterpret_cast<const char*>(value);
        HandleTabooData(fileType, locale, nameStr, valueStr);
        xmlFree(name);
        xmlFree(value);
        cur = cur->next;
    }
    xmlFreeDoc(doc);
}

void Taboo::HandleTabooData(DataFileType fileType, const std::string& locale, const std::string& name,
    const std::string& value)
{
    switch (fileType) {
        case DataFileType::CONFIG_FILE:
            ProcessTabooConfigData(name, value);
            break;
        case DataFileType::DATA_FILE:
            ProcessTabooLocaleData(locale, name, value);
            break;
        default:
            break;
    }
}

void Taboo::ProcessTabooConfigData(const std::string& key, const std::string& value)
{
    if (key.compare(0, BLOCKED_LANG_TAG.length(), BLOCKED_LANG_TAG) == 0) {
        ParseBlockedLanguagesAndRegions(key, value);
    } else if (key.compare(0, BLOCKED_CITY_TAG.length(), BLOCKED_CITY_TAG) == 0) {
        ParseBlockedCities(key, value);
    } else if (key.compare(0, BLOCKED_PHONE_NUMBER_TAG.length(), BLOCKED_PHONE_NUMBER_TAG) == 0) {
        ParseBlockedPhoneNumbers(key, value);
    } else if (key.compare(SUPPORTED_LANGUAGE_TAG) == 0) {
        Split(value, TABOO_DATA_SPLITOR, supportedLanguages);
    } else if (key.compare(SUPPORTED_REGION_TAG) == 0) {
        Split(value, TABOO_DATA_SPLITOR, supportedRegions);
    } else if (key.compare(SUPPORTED_TIME_ZONE_TAG) == 0) {
        Split(value, TABOO_DATA_SPLITOR, supportedTimeZones);
    } else if (key.compare(SUPPORTED_CITY_TAG) == 0) {
        Split(value, TABOO_DATA_SPLITOR, supportedcities);
    } else if (key.compare(SUPPORTED_PHONE_NUMBER_TAG) == 0) {
        Split(value, TABOO_DATA_SPLITOR, supportedPhoneNumbers);
    }
}

void Taboo::ProcessTabooLocaleData(const std::string& locale, const std::string& name, const std::string& value)
{
    if (localeTabooData.find(locale) != localeTabooData.end()) {
        localeTabooData[locale][name] = value;
        return;
    }
    std::unordered_map<std::string, std::string> data;
    data[name] = value;
    localeTabooData[locale] = data;
}

std::vector<std::string> Taboo::QueryKeyFallBack(const std::string& key) const
{
    size_t len = fallBackSuffixes.size();
    std::vector<std::string> fallback(len);
    for (size_t i = 0; i < len; i++) {
        fallback[i] = key + fallBackSuffixes[i];
    }
    return fallback;
}

std::tuple<std::string, std::string> Taboo::LanguageFallBack(const std::string& language)
{
    std::string bestMatch;
    std::string fileName;
    int32_t bestScore = -1;

    for (auto it = resources.begin(); it != resources.end(); ++it) {
        std::string resLanguage = it->first;
        int32_t score = LocaleCompare::Compare(language, resLanguage);
        if (score > bestScore) {
            bestMatch = resLanguage;
            fileName = it->second;
            bestScore = score;
        }
    }
    if (bestScore < 0) {
        return std::make_tuple("", "");
    }
    return std::make_tuple(bestMatch, fileName);
}

void Taboo::ReadResourceList()
{
    using std::filesystem::directory_iterator;
    struct stat s;
    for (const auto &dirEntry : directory_iterator{tabooDataPath}) {
        std::string path = dirEntry.path();
        if (stat(path.c_str(), &s) != 0) {
            HILOG_ERROR_I18N("Taboo::ReadResourceList: Get path status failed.");
            continue;
        }
        if (s.st_mode & S_IFDIR) {
            std::string fileName = path.substr(tabooDataPath.length());
            std::string language = GetLanguageFromFileName(fileName);
            resources[language] = fileName;
        }
    }
}

std::string Taboo::GetLanguageFromFileName(const std::string& fileName)
{
    if (fileName.length() == 3) { // 3 is the length of file 'xml'
        return "en";
    }
    std::string language = fileName.substr(4);
    if (language[0] == 'b' && language[1] == '+') {
        language = language.substr(2); // 2 is the length of "b+"
    }
    size_t pos = language.find("+");
    if (pos != std::string::npos) {
        language = language.replace(pos, 1, "-");
    }
    pos = language.find("-r");
    if (pos != std::string::npos) {
        language = language.replace(pos, 2, "-"); // 2 is the length of "-r"
    }
    return language;
}

void Taboo::ParseBlockedLanguagesAndRegions(const std::string& key, const std::string& value)
{
    std::vector<std::string> blockedDatas;
    Split(value, TABOO_DATA_SPLITOR, blockedDatas);
    if (blockedDatas.size() == 0) {
        return;
    }
    std::unordered_set<std::string> blockedLanguage;
    std::unordered_set<std::string> blockedRegion;
    std::unordered_map<std::string, std::unordered_set<std::string>> languageBlockedRegion;
    for (const auto& blockedData : blockedDatas) {
        if (blockedData.empty()) {
            continue;
        }
        size_t pos = blockedData.find(LANGUAGE_REGION_SPLITOR);
        if (pos == std::string::npos) {
            HILOG_ERROR_I18N("Taboo::ParseBlockedLanguagesAndRegions: Invalid blocked data %{public}s.",
                blockedData.c_str());
            continue;
        }
        std::string language = blockedData.substr(0, pos);
        std::string region = blockedData.substr(pos + 1);
        if (language.empty() || region.empty()) {
            HILOG_ERROR_I18N("Taboo::ParseBlockedLanguagesAndRegions: Language or region is empty, data %{public}s.",
                blockedData.c_str());
            continue;
        }
        if (language.compare(OMITTED_SYMBOL) == 0 && region.compare(OMITTED_SYMBOL) == 0) {
            HILOG_ERROR_I18N("Taboo::ParseBlockedLanguagesAndRegions: BlockedData is inValid.");
            continue;
        } else if (region.compare(OMITTED_SYMBOL) == 0) {
            blockedLanguage.insert(language);
        } else if (language.compare(OMITTED_SYMBOL) == 0) {
            blockedRegion.insert(region);
        } else {
            languageBlockedRegion[language].insert(region);
        }
    }
    blockedLanguages[key] = blockedLanguage;
    blockedRegions[key] = blockedRegion;
    languageBlockedRegions[key] = languageBlockedRegion;
}

void Taboo::ParseBlockedCities(const std::string& key, const std::string& value)
{
    std::unordered_set<std::string> blockedCity;
    Split(value, TABOO_DATA_SPLITOR, blockedCity);
    blockedCities[key] = blockedCity;
}

void Taboo::ParseBlockedPhoneNumbers(const std::string& key, const std::string& value)
{
    std::unordered_set<std::string> blockedPhoneNumber;
    Split(value, TABOO_DATA_SPLITOR, blockedPhoneNumber);
    blockedPhoneNumbers[key] = blockedPhoneNumber;
}

void Taboo::InitLocaleTabooData(const std::string& fallbackLanguage, const std::string& fileName)
{
    if (initResourcesList.find(fallbackLanguage) != initResourcesList.end()) {
        return;
    }
    std::lock_guard<std::mutex> tabooLock(tabooMutex);
    if (initResourcesList.find(fallbackLanguage) != initResourcesList.end()) {
        return;
    }
    localeTabooData[fallbackLanguage] = {};
    std::string localeTabooDataFilePath = tabooDataPath + fileName + FILE_PATH_SPLITOR + TABOO_DATA_FILE;
    ParseTabooData(localeTabooDataFilePath, DataFileType::DATA_FILE, fallbackLanguage);
    initResourcesList.insert(fallbackLanguage);
}

void Taboo::InitFallBackSuffixes()
{
    std::string mccmncFirst = ReadSystemParameter(MCC_MNC_FIRST_KEY.c_str(), LocaleConfig::CONFIG_LEN);
    if (mccmncFirst.empty()) {
        HILOG_ERROR_I18N("Taboo::InitFallBackSuffixes: Get first mccmnc failed.");
    }
    std::string mccFirst;
    if (mccmncFirst.length() == MCC_MNC_LENGTH) {
        fallBackSuffixes.emplace_back(MCC_MNC_TAG + mccmncFirst);
        mccFirst = mccmncFirst.substr(0, MCC_LENGTH);
    }
    std::string mccmncSecond = ReadSystemParameter(MCC_MNC_SECOND_KEY.c_str(), LocaleConfig::CONFIG_LEN);
    if (mccmncSecond.empty()) {
        HILOG_ERROR_I18N("Taboo::InitFallBackSuffixes: Get second mccmnc failed.");
    }
    std::string mccSecond;
    if (mccmncSecond.length() == MCC_MNC_LENGTH) {
        fallBackSuffixes.emplace_back(MCC_MNC_TAG + mccmncSecond);
        mccSecond = mccmncSecond.substr(0, MCC_LENGTH);
    }
    if (!mccFirst.empty()) {
        fallBackSuffixes.emplace_back(MCC_TAG + mccFirst);
    }
    if (!mccSecond.empty()) {
        fallBackSuffixes.emplace_back(MCC_TAG + mccSecond);
    }
    std::string custRegion = ReadSystemParameter(CUST_REGION_KEY.c_str(), LocaleConfig::CONFIG_LEN);
    if (!custRegion.empty()) {
        fallBackSuffixes.emplace_back(CUST_REGION_TAG + custRegion);
    } else {
        HILOG_ERROR_I18N("Taboo::InitFallBackSuffixes: Get cust region failed.");
    }
    std::string defaultRegion = ReadSystemParameter(LocaleConfig::DEFAULT_REGION_KEY, LocaleConfig::CONFIG_LEN);
    if (!defaultRegion.empty()) {
        fallBackSuffixes.emplace_back(DEFAULT_REGION_TAG + defaultRegion);
    } else {
        HILOG_ERROR_I18N("Taboo::InitFallBackSuffixes: Get default region failed.");
    }
    fallBackSuffixes.emplace_back(COMMON_SUFFIX);
}
} // namespace I18n
} // namespace Global
} // namespace OHOS