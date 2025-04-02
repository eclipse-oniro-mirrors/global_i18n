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
 * See the License for the specific locale governing permissions and
 * limitations under the License.
 */
#include "date_time_rule.h"
#include "utils.h"

namespace OHOS {
namespace Global {
namespace I18n {
std::string DateTimeRule::xmlCommonPath = "/system/usr/ohos_locale_config/datetime/common.xml";

DateTimeRule::DateTimeRule(std::string& locale)
{
    this->loadMap = {{"sub_rules", &this->subRules},
                     {"universe_rules", &this->universeRules},
                     {"filter_rules", &this->filterRules},
                     {"past_rules", &this->pastRules},
                     {"locale_rules", &this->localesRules},
                     {"delimiter", &this->delimiter},
                     {"param", &this->param},
                     {"default_locale", &this->localeMap},
                     {"isRelDates", &this->relDates}};
    Init(locale);
}

DateTimeRule::~DateTimeRule()
{
    std::unordered_map<std::string, icu::RegexPattern*>::iterator iter;
    for (iter = patternsMap.begin(); iter != patternsMap.end(); ++iter) {
        icu::RegexPattern* pattern = iter->second;
        if (pattern != nullptr) {
            delete pattern;
        }
        pattern = nullptr;
    }
}

void DateTimeRule::Init(std::string& locale)
{
    InitRules(xmlCommonPath);
    std::string xmlPath = "/system/usr/ohos_locale_config/datetime/" + locale + ".xml";
    if (!CheckTzDataFilePath(xmlPath)) {
        this->locale = this->localeMap["locale"];
    } else {
        this->locale = locale;
    }
    xmlPath = "/system/usr/ohos_locale_config/datetime/" + this->locale + ".xml";
    InitRules(xmlPath);
    std::string xmlPathBackup = "/system/usr/ohos_locale_config/datetime/" + this->localeMap["backup"] + ".xml";
    InitRuleBackup(xmlPathBackup);
    RuleLevel();
}

std::string DateTimeRule::GetLocale()
{
    return this->locale;
}

void DateTimeRule::RuleLevel()
{
    std::string ruleName = "mark_ShortDateLevel";
    std::string shortDateMark = GetWithoutB(ruleName);
    if (shortDateMark == "ymd") {
        // 1 indicates the level of the "20016" rule in the "ymd" style.
        levels["20016"] = 1;
        // 3 indicates the level of the "20014" rule in the "ymd" style.
        levels["20014"] = 3;
        // 2 indicates the level of the "20015" rule in the "ymd" style.
        levels["20015"] = 2;
    } else if (shortDateMark == "mdy") {
        // 2 indicates the level of the "20016" rule in the "mdy" style.
        levels["20016"] = 2;
        // 3 indicates the level of the "20014" rule in the "mdy" style.
        levels["20014"] = 3;
        // 1 indicates the level of the "20015" rule in the "mdy" style.
        levels["20015"] = 1;
    }
}

void DateTimeRule::InitRules(std::string& xmlPath)
{
    if (!CheckTzDataFilePath(xmlPath)) {
        return;
    }
    xmlKeepBlanksDefault(0);
    xmlDocPtr doc = xmlParseFile(xmlPath.c_str());
    if (doc == nullptr) return;
    xmlNodePtr root = xmlDocGetRootElement(doc);
    xmlNodePtr cur = root->xmlChildrenNode;
    while (cur != nullptr) {
        std::string category = reinterpret_cast<const char*>(cur->name);
        if (category == "sub_rules_map") {
            xmlNodePtr value = cur->xmlChildrenNode;
            while (value != nullptr) {
                std::string key = reinterpret_cast<const char*>(value->name);
                // remove the first 3 chars of key.
                key = key.substr(3);
                xmlNodePtr subValue = value->xmlChildrenNode;
                std::unordered_map<std::string, std::string> tempMap;
                LoadStrToStr(&tempMap, subValue);
                subRulesMap[key] = tempMap;
                value = value->next;
            }
        } else if (this->loadMap.find(category) != this->loadMap.end()) {
            xmlNodePtr valueNext = cur->xmlChildrenNode;
            LoadStrToStr(this->loadMap[category], valueNext);
        } else if (category == "pattern") {
            xmlNodePtr valueNext = cur->xmlChildrenNode;
            LoadStrToPattern(this->patternsMap, valueNext);
        }
        cur = cur->next;
    }
    xmlFreeDoc(doc);
}

void DateTimeRule::InitRuleBackup(std::string& xmlPathBackup)
{
    if (!CheckTzDataFilePath(xmlPathBackup)) {
        return;
    }
    xmlKeepBlanksDefault(0);
    xmlDocPtr doc = xmlParseFile(xmlPathBackup.c_str());
    if (doc == nullptr) return;
    xmlNodePtr cur = xmlDocGetRootElement(doc)->xmlChildrenNode;
    while (cur != nullptr) {
        std::string category = reinterpret_cast<const char*>(cur->name);
        xmlNodePtr valueNext = cur->xmlChildrenNode;
        if (category == "param") {
            LoadStrToStr(&paramBackup, valueNext);
        } else if (category == "locale_rules") {
            LoadStrToStr(&localesRulesBackup, valueNext);
        }
        cur = cur->next;
    }
    xmlFreeDoc(doc);
}

// load the rules of the cur node to the map
void DateTimeRule::LoadStrToPattern(std::unordered_map<std::string, icu::RegexPattern*>& map, xmlNodePtr cur)
{
    while (cur != nullptr) {
        std::string key = reinterpret_cast<const char*>(cur->name);
        // remove the first 3 chars of key.
        key = key.substr(3);
        xmlNodePtr value = cur->xmlChildrenNode;
        bool flag = false;
        // load rule content
        if (value != nullptr && !xmlStrcmp(value->name, reinterpret_cast<const xmlChar*>("flag"))) {
            xmlChar* typePtr = xmlNodeGetContent(value);
            if (typePtr != nullptr) {
                std::string type = reinterpret_cast<const char*>(typePtr);
                flag = (type == "True") ? true : flag;
                xmlFree(typePtr);
            }
            value = value->next;
        }
        icu::UnicodeString content;
        while (value != nullptr && !xmlStrcmp(value->name, reinterpret_cast<const xmlChar*>("content"))) {
            xmlChar* contentPtr = xmlNodeGetContent(value);
            if (contentPtr != nullptr) {
                icu::UnicodeString tempContent = reinterpret_cast<char*>(contentPtr);
                content += tempContent;
                xmlFree(contentPtr);
            }
            value = value->next;
        }
        UErrorCode status = U_ZERO_ERROR;
        icu::RegexPattern* pattern;
        if (flag) {
            pattern = icu::RegexPattern::compile(content, URegexpFlag::UREGEX_CASE_INSENSITIVE, status);
        } else {
            pattern = icu::RegexPattern::compile(content, 0, status);
        }
        map[key] = pattern;
        cur = cur->next;
    }
}

// load the rules of the cur node to the map
void DateTimeRule::LoadStrToStr(std::unordered_map<std::string, std::string>* map, xmlNodePtr cur)
{
    while (cur != nullptr) {
        std::string key = reinterpret_cast<const char*>(cur->name);
        // remove the first 3 chars of key.
        key = key.substr(3);
        xmlNodePtr value = cur->xmlChildrenNode;
        // load rule level
        if (value != nullptr && !xmlStrcmp(value->name, reinterpret_cast<const xmlChar*>("level"))) {
            xmlChar* levelPtr = xmlNodeGetContent(value);
            if (levelPtr != nullptr) {
                std::string levelStr = reinterpret_cast<const char*>(levelPtr);
                int32_t status = 0;
                int level = ConvertString2Int(levelStr, status);
                levels[key] = level;
                xmlFree(levelPtr);
            } else {
                break;
            }
            value = value->next;
        }
        // load rule content
        std::string content;
        while (value != nullptr && !xmlStrcmp(value->name, reinterpret_cast<const xmlChar*>("content"))) {
            xmlChar* contentPtr = xmlNodeGetContent(value);
            if (contentPtr != nullptr) {
                std::string tempContent = reinterpret_cast<const char*>(contentPtr);
                content += tempContent;
                xmlFree(contentPtr);
            }
            value = value->next;
        }
        (*map)[key] = content;
        cur = cur->next;
    }
}

// process rule paramMap[ruleName]. for example: Sat|Mon|Tue  ->  \\bSat\\b|\\bMon\\b|\\bTue\\b
std::string DateTimeRule::Get(std::unordered_map<std::string, std::string>& paramMap, std::string& ruleName)
{
    std::string result = "";
    if (paramMap.empty() || paramMap.find(ruleName) == paramMap.end()) {
        return result;
    }
    result = paramMap[ruleName];
    std::vector<std::string> temps;
    std::string splitStr = "|";
    Split(result, splitStr, temps);
    std::string sb;
    std::string mark = "";
    if (delimiter.find(locale) != delimiter.end()) {
        mark = delimiter[locale];
    } else {
        mark = "\\b";
    }
    for (auto& temp : temps) {
        if (!CompareBeginEnd(temp, "\\b", true)) {
            sb += mark;
        }
        sb += temp;
        if (!CompareBeginEnd(temp, "\\b", false) && !CompareBeginEnd(temp, ".", false)) {
            sb += mark;
        }
        sb += "|";
    }
    result = sb;
    if (CompareBeginEnd(result, "|", false)) {
        result.pop_back();
    }
    return result;
}

// check whether src starts or ends with target.
bool DateTimeRule::CompareBeginEnd(const std::string src, const std::string target, bool flag)
{
    size_t lengthSrc = src.length();
    size_t lengthTarget = target.length();
    if (lengthSrc < lengthTarget) {
        return false;
    }
    std::string subStr;
    if (flag) {
        subStr = src.substr(0, lengthTarget);
    } else {
        subStr = src.substr(lengthSrc - lengthTarget, lengthTarget);
    }
    return subStr == target;
}


std::string DateTimeRule::GetWithoutB(const std::string& ruleName)
{
    std::string result = "";
    if (param.empty() || param.find(ruleName) == param.end()) {
        return result;
    }
    result = param[ruleName];
    return result;
}

// check whether hyphen is a valid date and time separator.
bool DateTimeRule::IsRelDates(icu::UnicodeString& hyphen, std::string& locale)
{
    bool isRel = false;
    if (hyphen.trim().isEmpty()) {
        isRel = true;
    } else if (hyphen.trim() == ',' && relDates.find(locale) != relDates.end() &&
        relDates[locale].find(",") != std::string::npos) {
        isRel = true;
    }
    return isRel;
}

std::string DateTimeRule::trim(const std::string& src)
{
    std::string target = src;
    if (target.empty()) {
        return target;
    }
    target.erase(0, target.find_first_not_of(" "));
    target.erase(target.find_last_not_of(" ") + 1);
    return target;
}

int DateTimeRule::CompareLevel(std::string& key1, std::string& key2)
{
    int result = 0;
    int level1 = GetLevel(key1);
    int level2 = GetLevel(key2);
    if (level1 > level2) {
        result = 1;
    } else if (level1 < level2) {
        result = -1;
    }
    return result;
}

int DateTimeRule::GetLevel(std::string& name)
{
    int baselevel;
    int32_t status = 0;
    int key = ConvertString2Int(name, status);
    // 9999 and 20000 are the rule numbers.
    if (key > 9999 && key < 20000) {
        // 10 is basic level.
        baselevel = 10;
    // 19999 and 40000 are the rule numbers.
    } else if (key > 19999 && key < 40000) {
        // 20 is basic level.
        baselevel = 20;
    } else {
        // 30 is basic level.
        baselevel = 30;
    }
    int addLeve = 1;
    if (levels.find(name) != levels.end()) {
        addLeve = levels[name];
    }
    int level = baselevel + addLeve;
    return level;
}

std::unordered_map<std::string, std::string> DateTimeRule::GetUniverseRules()
{
    return universeRules;
}

std::unordered_map<std::string, std::string> DateTimeRule::GetLocalesRules()
{
    return localesRules;
}

std::unordered_map<std::string, std::string> DateTimeRule::GetLocalesRulesBackup()
{
    return localesRulesBackup;
}

std::unordered_map<std::string, std::unordered_map<std::string, std::string>> DateTimeRule::GetSubRulesMap()
{
    return subRulesMap;
}

std::unordered_map<std::string, std::string> DateTimeRule::GetSubRules()
{
    return subRules;
}

std::unordered_map<std::string, std::string> DateTimeRule::GetFilterRules()
{
    return filterRules;
}

std::unordered_map<std::string, std::string> DateTimeRule::GetPastRules()
{
    return pastRules;
}

std::unordered_map<std::string, std::string> DateTimeRule::GetParam()
{
    return param;
}

std::unordered_map<std::string, std::string> DateTimeRule::GetParamBackup()
{
    return paramBackup;
}

std::unordered_map<std::string, icu::RegexPattern*> DateTimeRule::GetPatternsMap()
{
    return patternsMap;
}
} // namespace I18n
} // namespace Global
} // namespace OHOS