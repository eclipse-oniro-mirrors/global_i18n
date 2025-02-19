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
#include "taboo.h"

namespace OHOS {
namespace Global {
namespace I18n {
const char* Taboo::ROOT_TAG = "taboo";
const char* Taboo::ITEM_TAG = "item";
const char* Taboo::NAME_TAG = "name";
const char* Taboo::VALUE_TAG = "value";
std::set<std::string> Taboo::supportedRegions;
std::set<std::string> Taboo::supportedLanguages;
std::map<std::string, std::string> Taboo::RESOURCES = {};
std::mutex Taboo::TABOO_MUTEX;

Taboo::Taboo()
{
}

Taboo::Taboo(const std::string& path) : tabooDataPath(path)
{
    using std::filesystem::directory_iterator;

    std::string tabooConfigFilePath = tabooDataPath + tabooConfigFileName;
    struct stat s;
    isTabooDataExist = stat(tabooConfigFilePath.c_str(), &s) == 0;
    if (isTabooDataExist) {
        if (RESOURCES.size() == 0) {
            std::lock_guard<std::mutex> tabooLock(TABOO_MUTEX);
            if (RESOURCES.size() == 0) {
                // parse taboo-config.xml to obtain supported regions and languages for name replacement.
                ParseTabooData(tabooConfigFilePath, DataFileType::CONFIG_FILE);
                ReadResourceList();
            }
        }
    }
}

Taboo::~Taboo()
{
}

std::string Taboo::ReplaceCountryName(const std::string& region, const std::string& displayLanguage,
    const std::string& name)
{
    if (!isTabooDataExist) {
        HILOG_INFO_I18N("Taboo::ReplaceCountryName Taboo data not exist.");
        return name;
    }
    if (supportedRegions.find(region) == supportedRegions.end()) {
        HILOG_INFO_I18N("Taboo::ReplaceCountryName taboo data don't support region %{public}s", region.c_str());
        return name;
    }
    std::string key = regionKey + region;
    std::vector<std::string> fallbackRegionKeys = QueryKeyFallBack(key);
    std::string fallbackLanguage;
    std::string fileName;
    std::tie(fallbackLanguage, fileName) = LanguageFallBack(displayLanguage);
    if (fallbackLanguage.length() == 0) {
        HILOG_INFO_I18N("Taboo::ReplaceCountryName language %{public}s fallback failed", displayLanguage.c_str());
        return name;
    }
    if (localeTabooData.find(fallbackLanguage) == localeTabooData.end()) {
        localeTabooData[fallbackLanguage] = {};
        std::string localeTabooDataFilePath = tabooDataPath + fileName + filePathSplitor + tabooLocaleDataFileName;
        ParseTabooData(localeTabooDataFilePath, DataFileType::DATA_FILE, fallbackLanguage);
    }
    auto tabooData = localeTabooData[fallbackLanguage];
    for (auto it = fallbackRegionKeys.begin(); it != fallbackRegionKeys.end(); ++it) {
        if (tabooData.find(*it) != tabooData.end()) {
            return tabooData[*it];
        }
    }
    HILOG_INFO_I18N("Taboo::ReplaceCountryName not find taboo data correspond to region %{public}s",
        region.c_str());
    return name;
}

std::string Taboo::ReplaceLanguageName(const std::string& language, const std::string& displayLanguage,
    const std::string& name)
{
    if (!isTabooDataExist) {
        HILOG_INFO_I18N("Taboo::ReplaceLanguageName Taboo data not exist.");
        return name;
    }
    if (supportedLanguages.find(language) == supportedLanguages.end()) {
        HILOG_ERROR_I18N("Taboo::ReplaceLanguageName taboo data don't support language %{public}s",
            language.c_str());
        return name;
    }
    std::string key = languageKey + language;
    std::vector<std::string> fallbackLanguageKeys = QueryKeyFallBack(key);
    std::string fallbackLanguage;
    std::string fileName;
    std::tie(fallbackLanguage, fileName) = LanguageFallBack(displayLanguage);
    if (fallbackLanguage.size() == 0) {
        HILOG_ERROR_I18N("Taboo::ReplaceLanguageName language %{public}s fallback failed", displayLanguage.c_str());
        return name;
    }
    if (localeTabooData.find(fallbackLanguage) == localeTabooData.end()) {
        localeTabooData[fallbackLanguage] = {};
        std::string localeTabooDataFilePath = tabooDataPath + fileName + filePathSplitor + tabooLocaleDataFileName;
        ParseTabooData(localeTabooDataFilePath, DataFileType::DATA_FILE, fallbackLanguage);
    }
    auto tabooData = localeTabooData[fallbackLanguage];
    for (auto it = fallbackLanguageKeys.begin(); it != fallbackLanguageKeys.end(); ++it) {
        if (tabooData.find(*it) != tabooData.end()) {
            return tabooData[*it];
        }
    }
    HILOG_ERROR_I18N("Taboo::ReplaceLanguageName not find taboo data correspond to language %{public}s",
        language.c_str());
    return name;
}

void Taboo::ParseTabooData(const std::string& path, DataFileType fileType, const std::string& locale)
{
    xmlKeepBlanksDefault(0);
    xmlDocPtr doc = xmlParseFile(path.c_str());
    if (doc == nullptr) {
        HILOG_ERROR_I18N("Taboo parse taboo data file failed: %{public}s", path.c_str());
        return;
    }
    xmlNodePtr cur = xmlDocGetRootElement(doc);
    if (cur == nullptr || xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>(ROOT_TAG)) != 0) {
        xmlFreeDoc(doc);
        HILOG_ERROR_I18N("Taboo get root tag from taboo data file failed: %{public}s", path.c_str());
        return;
    }
    cur = cur->xmlChildrenNode;
    const xmlChar* nameTag = reinterpret_cast<const xmlChar*>(NAME_TAG);
    const xmlChar* valueTag = reinterpret_cast<const xmlChar*>(VALUE_TAG);
    while (cur != nullptr && xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>(ITEM_TAG)) == 0) {
        xmlChar* name = xmlGetProp(cur, nameTag);
        xmlChar* value = xmlGetProp(cur, valueTag);
        if (name == nullptr || value == nullptr) {
            HILOG_ERROR_I18N("Taboo get name and value property failed: %{public}s", path.c_str());
            cur = cur->next;
            continue;
        }
        std::string nameStr = reinterpret_cast<const char*>(name);
        std::string valueStr = reinterpret_cast<const char*>(value);
        switch (fileType) {
            case DataFileType::CONFIG_FILE:
                ProcessTabooConfigData(nameStr, valueStr);
                break;
            case DataFileType::DATA_FILE:
                ProcessTabooLocaleData(locale, nameStr, valueStr);
                break;
        }
        xmlFree(name);
        xmlFree(value);
        cur = cur->next;
    }
    xmlFreeDoc(doc);
}

void Taboo::ProcessTabooConfigData(const std::string& name, const std::string& value)
{
    if (name.compare(supportedRegionsTag) == 0) {
        SplitValue(value, supportedRegions);
    } else if (name.compare(supportedLanguagesTag) == 0) {
        SplitValue(value, supportedLanguages);
    }
}

void Taboo::ProcessTabooLocaleData(const std::string& locale, const std::string& name, const std::string& value)
{
    if (localeTabooData.find(locale) != localeTabooData.end()) {
        localeTabooData[locale][name] = value;
    } else {
        std::map<std::string, std::string> data;
        data[name] = value;
        localeTabooData[locale] = data;
    }
}

void Taboo::SplitValue(const std::string& value, std::set<std::string>& collation)
{
    size_t startPos = 0;
    while (startPos < value.length()) {
        size_t endPos = value.find(tabooDataSplitor, startPos);
        if (endPos == std::string::npos) {
            collation.insert(value.substr(startPos));
            endPos = value.length();
        } else {
            collation.insert(value.substr(startPos, endPos - startPos));
        }
        startPos = endPos + 1;
    }
}

std::vector<std::string> Taboo::QueryKeyFallBack(const std::string& key)
{
    std::vector<std::string> fallback;
    fallback.push_back(key + "_r_all");
    return fallback;
}

std::tuple<std::string, std::string> Taboo::LanguageFallBack(const std::string& language)
{
    std::string bestMatch;
    std::string fileName;
    int32_t bestScore = -1;

    for (auto it = RESOURCES.begin(); it != RESOURCES.end(); ++it) {
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
            HILOG_ERROR_I18N("get path status failed");
            continue;
        }
        if (s.st_mode & S_IFDIR) {
            std::string fileName = path.substr(tabooDataPath.length());
            std::string language = GetLanguageFromFileName(fileName);
            RESOURCES[language] = fileName;
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
} // namespace I18n
} // namespace Global
} // namespace OHOS