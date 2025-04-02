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
#include "phonenumbers/phonenumber.h"
#include "phonenumbers/shortnumberinfo.h"
#include "phonenumbers/phonenumberutil.h"
#include "i18n_hilog.h"
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
    try {
        isFixed = std::filesystem::exists(xmlPath);
    } catch (const std::filesystem::filesystem_error &except) {
        HILOG_ERROR_I18N("FileExist failed because filesystem_error, error message: %{public}s.",
            except.code().message().c_str());
        isFixed = false;
    } catch (const std::__h::__fs::filesystem::filesystem_error &except) {
        HILOG_ERROR_I18N("FileExist failed because filesystem_error, error message: %{public}s.",
            except.code().message().c_str());
        isFixed = false;
    } catch (const std::bad_alloc &except) {
        HILOG_ERROR_I18N("FileExist failed because bad_alloc, error message: %{public}s.", except.what());
        isFixed = false;
    }
    this->commonExit = false;
    this->country = country;
}

PhoneNumberRule::~PhoneNumberRule()
{
    for (auto it = negativeRules.begin(); it != negativeRules.end(); ++it) {
        if (*it != nullptr) {
            delete *it;
        }
    }
    for (auto it = positiveRules.begin(); it != positiveRules.end(); ++it) {
        if (*it != nullptr) {
            delete *it;
        }
    }
    for (auto it = borderRules.begin(); it != borderRules.end(); ++it) {
        if (*it != nullptr) {
            delete *it;
        }
    }
    for (auto it = codesRules.begin(); it != codesRules.end(); ++it) {
        if (*it != nullptr) {
            delete *it;
        }
    }
    for (auto it = findRules.begin(); it != findRules.end(); ++it) {
        if (*it != nullptr) {
            delete *it;
        }
    }
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
        ParseXmlNode(cur, category);
        cur = cur->next;
    }
    xmlFreeDoc(doc);
}

void PhoneNumberRule::ParseXmlNode(xmlNodePtr cur, std::string& category)
{
    xmlNodePtr rule = cur->xmlChildrenNode;
    while (rule != nullptr && !xmlStrcmp(rule->name, reinterpret_cast<const xmlChar*>("rule"))) {
        xmlNodePtr value = rule->xmlChildrenNode;
        std::string insensitive = XmlNodePtrToString(value);
        value = value->next;
        if (category == "common_exit") {
            commonExit = (insensitive == "True");
            break;
        }
        std::string type = XmlNodePtrToString(value);
        value = value->next;
        std::string valid = XmlNodePtrToString(value);
        value = value->next;
        std::string handle = XmlNodePtrToString(value);
        if (IsXmlNodeValueEmpty(insensitive, type, valid, handle)) {
            break;
        }
        icu::UnicodeString content = "";
        while (value->next != nullptr && !xmlStrcmp(value->next->name,
            reinterpret_cast<const xmlChar*>("content"))) {
            value = value->next;
            xmlChar* contentPtr = xmlNodeGetContent(value);
            if (contentPtr == nullptr) continue;
            icu::UnicodeString tempContent = reinterpret_cast<char*>(contentPtr);
            content += tempContent;
            xmlFree(contentPtr);
        }
        SetRules(category, content, valid, handle, insensitive, type);
        rule = rule->next;
    }
}

bool PhoneNumberRule::IsXmlNodeValueEmpty(const std::string& insensitive, const std::string& type,
    const std::string& valid, const std::string& handle)
{
    if (insensitive.empty() || type.empty() ||
        valid.empty() || handle.empty()) {
        return true;
    }
    return false;
}

std::string PhoneNumberRule::XmlNodePtrToString(xmlNodePtr valuePtr)
{
    if (valuePtr == nullptr) {
        return "";
    }
    xmlChar* charPtr = xmlNodeGetContent(valuePtr);
    if (charPtr == nullptr) {
        return "";
    }
    std::string result = reinterpret_cast<char*>(charPtr);
    xmlFree(charPtr);
    return result;
}

void PhoneNumberRule::SetRules(std::string& category, icu::UnicodeString& content, std::string& valid,
    std::string& handle, std::string& insensitive, std::string& type)
{
    if (category == "negative" || (category == "common" && commonExit)) {
        negativeRules.push_back(new NegativeRule(content, insensitive));
    } else if (category == "positive") {
        positiveRules.push_back(new PositiveRule(content, handle, insensitive));
    } else if (category == "border") {
        borderRules.push_back(new BorderRule(content, insensitive, type));
    } else if (category == "codes") {
        codesRules.push_back(new CodeRule(valid));
    } else if (category == "find_number") {
        findRules.push_back(new FindRule(content, insensitive));
    }
}

std::vector<BorderRule*> PhoneNumberRule::GetBorderRules()
{
    return borderRules;
}

std::vector<CodeRule*> PhoneNumberRule::GetCodesRules()
{
    return codesRules;
}

std::vector<PositiveRule*> PhoneNumberRule::GetPositiveRules()
{
    return positiveRules;
}

std::vector<NegativeRule*> PhoneNumberRule::GetNegativeRules()
{
    return negativeRules;
}

std::vector<FindRule*> PhoneNumberRule::GetFindRules()
{
    return findRules;
}
} // namespace I18n
} // namespace Global
} // namespace OHOS