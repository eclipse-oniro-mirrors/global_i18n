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
#include <filesystem>
#include "cpp/src/phonenumbers/phonenumber.h"
#include "cpp/src/phonenumbers/shortnumberinfo.h"
#include "cpp/src/phonenumbers/phonenumberutil.h"
#include "libxml/globals.h"
#include "libxml/tree.h"
#include "libxml/xmlstring.h"
#include "matched_number_info.h"
#include "phone_number_rule.h"
#include "utils.h"

namespace OHOS {
namespace Global {
namespace I18n {
using i18n::phonenumbers::PhoneNumber;
using i18n::phonenumbers::PhoneNumberUtil;
using i18n::phonenumbers::ShortNumberInfo;

std::string PhoneNumberRule::xmlCommonPath = "/system/usr/ohos_locale_config/phonenumber/common.xml";

PhoneNumberRule::PhoneNumberRule(std::string& country)
{
    xmlPath = "/system/usr/ohos_locale_config/phonenumber/" + country + ".xml";
    isFixed = true;
    if (!std::filesystem::exists(xmlPath)) {
        isFixed = false;
    }
    this->commonExit = false;
    this->country = country;
}

PhoneNumberRule::~PhoneNumberRule()
{
    FreePointer(negativeRules);
    FreePointer(positiveRules);
    FreePointer(borderRules);
    FreePointer(codesRules);
    FreePointer(findRules);
}

void PhoneNumberRule::FreePointer(std::vector<RegexRule*> &ruleList)
{
    std::vector<RegexRule*>::iterator iter;
    for (iter = ruleList.begin(); iter != ruleList.end(); ++iter) {
        if (*iter != nullptr) {
            delete *iter;
        }
        *iter = nullptr;
    }
    ruleList.clear();
}

void PhoneNumberRule::Init()
{
    if (isFixed) {
        InitRule(xmlPath);
    }
    InitRule(xmlCommonPath);
}

// Load the rules of the corresponding country
void PhoneNumberRule::InitRule(std::string& xmlPath)
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
        xmlNodePtr rule = cur->xmlChildrenNode;
        while (rule != nullptr && !xmlStrcmp(rule->name, reinterpret_cast<const xmlChar*>("rule"))) {
            xmlNodePtr value = rule->xmlChildrenNode;
            std::string insensitive = reinterpret_cast<char*>(xmlNodeGetContent(value));
            value = value->next;
            if (category == "common_exit") {
                commonExit = (insensitive == "True");
                break;
            }
            std::string type = reinterpret_cast<char*>(xmlNodeGetContent(value));
            value = value->next;
            std::string valid = reinterpret_cast<char*>(xmlNodeGetContent(value));
            value = value->next;
            std::string handle = reinterpret_cast<char*>(xmlNodeGetContent(value));
            icu::UnicodeString content = "";
            while (value->next != nullptr && !xmlStrcmp(value->next->name,
                reinterpret_cast<const xmlChar*>("content"))) {
                value = value->next;
                xmlChar* contentPtr = xmlNodeGetContent(value);
                icu::UnicodeString tempContent = reinterpret_cast<char*>(contentPtr);
                content += tempContent;
                xmlFree(contentPtr);
            }
            SetRules(category, content, valid, handle, insensitive, type);
            rule = rule->next;
        }
        cur = cur->next;
    }
    xmlFreeDoc(doc);
}

void PhoneNumberRule::SetRules(std::string& category, icu::UnicodeString& content, std::string& valid,
    std::string& handle, std::string& insensitive, std::string& type)
{
    if (category == "negative" || (category == "common" && commonExit)) {
        negativeRules.push_back(new RegexRule(content, valid, handle, insensitive, type));
    } else if (category == "positive") {
        positiveRules.push_back(new RegexRule(content, valid, handle, insensitive, type));
    } else if (category == "border") {
        borderRules.push_back(new RegexRule(content, valid, handle, insensitive, type));
    } else if (category == "codes") {
        codesRules.push_back(new RegexRule(content, valid, handle, insensitive, type));
    } else if (category == "find_number") {
        findRules.push_back(new RegexRule(content, valid, handle, insensitive, type));
    }
}

std::vector<RegexRule*> PhoneNumberRule::GetBorderRules()
{
    return borderRules;
}

std::vector<RegexRule*> PhoneNumberRule::GetCodesRules()
{
    return codesRules;
}

std::vector<RegexRule*> PhoneNumberRule::GetPositiveRules()
{
    return positiveRules;
}

std::vector<RegexRule*> PhoneNumberRule::GetNegativeRules()
{
    return negativeRules;
}

std::vector<RegexRule*> PhoneNumberRule::GetFindRules()
{
    return findRules;
}
} // namespace I18n
} // namespace Global
} // namespace OHOS