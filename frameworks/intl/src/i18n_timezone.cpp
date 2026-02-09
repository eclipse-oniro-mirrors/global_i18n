/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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
#include "i18n_timezone.h"

#include <dlfcn.h>
#include <filesystem>
#include <fstream>
#include <sys/stat.h>
#include <unistd.h>
#include "i18n_hilog.h"
#include "libxml/globals.h"
#include "libxml/tree.h"
#include "libxml/xmlstring.h"
#include "locale_config.h"
#include "locale_matcher.h"
#include "locale_info.h"
#include "map"
#include "securec.h"
#include "set"
#include "string"
#include "type_traits"
#include "unicode/umachine.h"
#include "utility"
#include "utils.h"
#include "unicode/utypes.h"
#include "vector"
#include "unicode/locid.h"
#include "unicode/unistr.h"
#include "utils.h"
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <regex>

namespace OHOS {
namespace Global {
namespace I18n {
using namespace std::filesystem;

const char *I18nTimeZone::TIMEZONE_KEY = "persist.time.timezone";
const char *I18nTimeZone::DEFAULT_TIMEZONE = "GMT";

const char *I18nTimeZone::CITY_TIMEZONE_DATA_PATH = "/system/usr/ohos_timezone/ohos_timezones.xml";
const char *I18nTimeZone::DEVICE_CITY_TIMEZONE_DATA_PATH = "/system/usr/ohos_timezone/device_timezones.xml";
const char *I18nTimeZone::DISTRO_DEVICE_CITY_TIMEZONE_DATA_PATH = "/system/etc/tzdata_distro/device_timezones.xml";
const char *I18nTimeZone::TZ_PIXEL_PATH = "/system/usr/ohos_timezone";
const char *I18nTimeZone::DISTRO_TZ_PIXEL_PATH = "/system/etc/tzdata_distro";
const char *I18nTimeZone::DEFAULT_LOCALE = "root";
const char *I18nTimeZone::CITY_DISPLAYNAME_PATH = "/system/usr/ohos_timezone/ohos_city_dispname/";
const char *I18nTimeZone::BASE_DEVICE_CITY_DISPLAYNAME_PATH = "/system/usr/ohos_timezone/device_city_dispname/";
const char *I18nTimeZone::DISTRO_DEVICE_CITY_DISPLAYNAME_PATH = "/system/etc/tzdata_distro/device_city_dispname/";
const char *I18nTimeZone::DISTRO_ROOT_DISPLAYNAME_PATH = "/system/etc/tzdata_distro/device_city_dispname/root.xml";
const char *I18nTimeZone::TIMEZONE_ROOT_TAG = "timezones";
const char *I18nTimeZone::TIMEZONE_SECOND_ROOT_TAG = "timezone";
const char *I18nTimeZone::CITY_DISPLAYNAME_ROOT_TAG = "display_names";
const char *I18nTimeZone::CITY_DISPLAYNAME_SECOND_ROOT_TAG = "display_name";
const char *I18nTimeZone::NUM_TIMEZONE_CFG_PATH = "/system/usr/ohos_locale_config/num_timezone.cfg";
const char *I18nTimeZone::DISTRO_NUM_TIMEZONE_CFG_PATH = "/system/etc/tzdata_distro/num_timezone.cfg";

std::set<std::string> I18nTimeZone::supportedLocales {};
std::unordered_set<std::string> I18nTimeZone::availableZoneCityIDs {};
std::unordered_map<std::string, std::string> I18nTimeZone::city2TimeZoneID {};
std::unordered_map<std::string, std::string> I18nTimeZone::cityDisplayNameCache {};
std::string I18nTimeZone::cityDisplayNameCacheLocale = "";
std::map<std::string, std::string> I18nTimeZone::BEST_MATCH_LOCALE {};
std::mutex I18nTimeZone::matchLocaleMutex;
std::mutex I18nTimeZone::initZoneInfoMutex;
std::mutex I18nTimeZone::initSupportedLocalesMutex;
std::mutex I18nTimeZone::cityDisplayNameMutex;
bool I18nTimeZone::initAvailableZoneInfo = false;
bool I18nTimeZone::initSupportedLocales = false;
bool I18nTimeZone::numTimezoneCategoryInitialized = false;
std::mutex I18nTimeZone::initNumTimezoneCategoryMutex;

std::map<int, std::string> I18nTimeZone::categoryNum2TimezoneWN {};
std::map<int, std::string> I18nTimeZone::categoryNum2TimezoneEN {};
std::map<int, std::string> I18nTimeZone::categoryNum2TimezoneWS {};
std::map<int, std::string> I18nTimeZone::categoryNum2TimezoneES {};

I18nTimeZone::I18nTimeZone(std::string &id, bool isZoneID)
{
    if (isZoneID) {
        if (id.empty()) {
            std::string systemTimezone = ReadSystemParameter(TIMEZONE_KEY, SYS_PARAM_LEN);
            if (systemTimezone.empty()) {
                HILOG_ERROR_I18N("I18nTimeZone::I18nTimeZone: Read system time zone failed.");
                systemTimezone = DEFAULT_TIMEZONE;
            }
            icu::UnicodeString unicodeZoneID(systemTimezone.data(), systemTimezone.length());
            timezone = icu::TimeZone::createTimeZone(unicodeZoneID);
        } else {
            icu::UnicodeString unicodeZoneID(id.data(), id.length());
            timezone = icu::TimeZone::createTimeZone(unicodeZoneID);
        }
    } else {
        GetAvailableZoneCityIDs();
        if (city2TimeZoneID.find(id) == city2TimeZoneID.end()) {
            timezone = icu::TimeZone::createDefault();
        } else {
            std::string timezoneID = city2TimeZoneID.at(id);
            icu::UnicodeString unicodeZoneID(timezoneID.data(), timezoneID.length());
            timezone = icu::TimeZone::createTimeZone(unicodeZoneID);
        }
    }
}

I18nTimeZone::~I18nTimeZone()
{
    if (timezone != nullptr) {
        delete timezone;
        timezone = nullptr;
    }
}

icu::TimeZone* I18nTimeZone::GetTimeZone()
{
    return timezone;
}

std::unique_ptr<I18nTimeZone> I18nTimeZone::CreateInstance(std::string &id, bool isZoneID)
{
    std::unique_ptr<I18nTimeZone> i18nTimeZone = std::make_unique<I18nTimeZone>(id, isZoneID);
    if (i18nTimeZone == nullptr) {
        return nullptr;
    }
    if (i18nTimeZone->GetTimeZone() == nullptr) {
        return nullptr;
    }
    return i18nTimeZone;
}

int32_t I18nTimeZone::GetOffset(double date)
{
    if (timezone == nullptr) {
        return 0;
    }
    int32_t rawOffset = 0;
    int32_t dstOffset = 0;
    bool local = false;
    UErrorCode status = U_ZERO_ERROR;
    timezone->getOffset(date, (UBool)local, rawOffset, dstOffset, status);
    if (U_FAILURE(status)) {
        return 0;
    }
    return rawOffset + dstOffset;
}

int32_t I18nTimeZone::GetRawOffset()
{
    if (timezone == nullptr) {
        return 0;
    }
    return timezone->getRawOffset();
}

std::string I18nTimeZone::GetID()
{
    if (timezone == nullptr) {
        return "";
    }
    icu::UnicodeString zoneID;
    timezone->getID(zoneID);
    std::string result;
    zoneID.toUTF8String(result);
    return result;
}

std::string I18nTimeZone::GetDisplayName()
{
    if (timezone == nullptr) {
        return "";
    }
    std::string localeStr = LocaleConfig::GetEffectiveLocale();
    return GetDisplayName(localeStr, false);
}

std::string I18nTimeZone::GetDisplayName(bool isDST)
{
    std::string localeStr = LocaleConfig::GetEffectiveLocale();
    return GetDisplayName(localeStr, isDST);
}

std::string I18nTimeZone::GetDisplayName(std::string localeStr)
{
    return GetDisplayName(localeStr, false);
}

std::string I18nTimeZone::GetDisplayName(
    std::string localeStr, bool isDST)
{
    icu::TimeZone::EDisplayType style = icu::TimeZone::EDisplayType::LONG_GENERIC;
    UErrorCode status = U_ZERO_ERROR;
    icu::Locale locale;
    std::string result;
    if (LocaleConfig::IsValidTag(localeStr)) {
        locale = icu::Locale::forLanguageTag(localeStr, status);
    } else {
        std::string systemLocale = LocaleConfig::GetEffectiveLocale();
        locale = icu::Locale::forLanguageTag(systemLocale, status);
    }
    if (U_FAILURE(status)) {
        HILOG_ERROR_I18N("create icu Locale for %{public}s failed.", localeStr.c_str());
        return PseudoLocalizationProcessor(result);
    }
    icu::UnicodeString name;
    if (timezone == nullptr) {
        return PseudoLocalizationProcessor(result);
    }
    timezone->getDisplayName((UBool)isDST, style, locale, name);
    name.toUTF8String(result);
    return PseudoLocalizationProcessor(GetDisplayNameByTaboo(localeStr, result));
}

std::string I18nTimeZone::GetDisplayNameByTaboo(
    const std::string& localeStr, const std::string& result)
{
    TabooUtils* tabooUtils = TabooUtils::GetInstance();
    if (tabooUtils == nullptr) {
        HILOG_ERROR_I18N("I18nTimeZone::GetDisplayNameByTaboo: tabooUtils is nullptr.");
        return result;
    }
    return tabooUtils->ReplaceTimeZoneName(GetID(), localeStr, result);
}

bool I18nTimeZone::ReadTimeZoneData(const char *xmlPath)
{
    xmlKeepBlanksDefault(0);
    if (xmlPath == nullptr) {
        return false;
    }
    xmlDocPtr doc = xmlParseFile(xmlPath);
    if (!doc) {
        return false;
    }
    xmlNodePtr cur = xmlDocGetRootElement(doc);
    if (!cur || xmlStrcmp(cur->name, reinterpret_cast<const xmlChar *>(TIMEZONE_ROOT_TAG))) {
        xmlFreeDoc(doc);
        return false;
    }
    cur = cur->xmlChildrenNode;
    while (cur != nullptr && !xmlStrcmp(cur->name, reinterpret_cast<const xmlChar *>(TIMEZONE_SECOND_ROOT_TAG))) {
        xmlNodePtr value = cur->xmlChildrenNode;
        xmlChar *contents[ELEMENT_NUM] = { 0 }; // 2 represent cityid, zoneid;
        bool xmlNodeIsNull = false;
        for (size_t i = 0; i < ELEMENT_NUM && value != nullptr; i++) {
            contents[i] = xmlNodeGetContent(value);
            value = value->next;
            if (contents[i] == nullptr) {
                xmlNodeIsNull = true;
            }
        }
        if (!xmlNodeIsNull) {
            // 0 represents cityid index, 1 represents zoneid index
            availableZoneCityIDs.insert(reinterpret_cast<const char *>(contents[0]));
            const char* pairKey = reinterpret_cast<const char *>(contents[0]);
            const char* pairVal = reinterpret_cast<const char *>(contents[1]);
            city2TimeZoneID.insert(std::make_pair<std::string, std::string>(pairKey, pairVal));
        }
        for (size_t i = 0; i < ELEMENT_NUM; i++) {
            if (contents[i] != nullptr) {
                xmlFree(contents[i]);
            }
        }
        cur = cur->next;
    }
    xmlFreeDoc(doc);
    return true;
}

void I18nTimeZone::GetTimezoneIDFromZoneInfo(std::set<std::string> &availableIDs, std::string &parentPath,
    std::string &parentName)
{
    using std::filesystem::directory_iterator;

    struct stat s;
    for (const auto &dirEntry : directory_iterator{parentPath}) {
        std::string zonePath = dirEntry.path();
        if (stat(zonePath.c_str(), &s) != 0) {
            HILOG_ERROR_I18N("GetTimezoneIDFromZoneInfo: zoneinfo path %{public}s not exist.", parentPath.c_str());
            return;
        }
        std::string zoneName = zonePath.substr(parentPath.length() + 1); // 1 add length of path splitor
        std::string finalZoneName = parentName + "/" + zoneName;
        if (s.st_mode & S_IFDIR) {
            GetTimezoneIDFromZoneInfo(availableIDs, zonePath, finalZoneName);
        } else {
            availableIDs.insert(finalZoneName);
        }
    }
}

std::set<std::string> I18nTimeZone::GetAvailableIDs()
{
    return GetTimeZoneAvailableIDs();
}

std::unordered_set<std::string> I18nTimeZone::GetAvailableZoneCityIDs()
{
    if (initAvailableZoneInfo) {
        return availableZoneCityIDs;
    }
    std::lock_guard<std::mutex> lock(initZoneInfoMutex);
    if (initAvailableZoneInfo) {
        return availableZoneCityIDs;
    }
    struct stat s;
    if (stat(DISTRO_DEVICE_CITY_TIMEZONE_DATA_PATH, &s) == 0) {
        ReadTimeZoneData(DISTRO_DEVICE_CITY_TIMEZONE_DATA_PATH);
    } else if (stat(DEVICE_CITY_TIMEZONE_DATA_PATH, &s) == 0) {
        ReadTimeZoneData(DEVICE_CITY_TIMEZONE_DATA_PATH);
    } else {
        ReadTimeZoneData(CITY_TIMEZONE_DATA_PATH);
    }
    TabooUtils* tabooUtils = TabooUtils::GetInstance();
    if (tabooUtils == nullptr) {
        HILOG_ERROR_I18N("I18nTimeZone::GetAvailableZoneCityIDs: tabooUtils is nullptr.");
        return availableZoneCityIDs;
    }
    std::unordered_set<std::string> blockedCities = tabooUtils->GetBlockedCities();
    for (const auto& city : blockedCities) {
        if (availableZoneCityIDs.find(city) != availableZoneCityIDs.end()) {
            availableZoneCityIDs.erase(city);
            city2TimeZoneID.erase(city);
        }
    }
    initAvailableZoneInfo = true;
    return availableZoneCityIDs;
}

std::string I18nTimeZone::FindCityDisplayNameFromXml(std::string &cityID, std::string &locale)
{
    std::lock_guard<std::mutex> lock(cityDisplayNameMutex);
    if (cityDisplayNameCacheLocale.compare(locale) == 0) {
        return GetCityDisplayNameFromCache(cityID);
    }
    std::string displayName = FindCityDisplayNameFromXmlInner(cityID, locale);
    return displayName;
}

std::string I18nTimeZone::FindCityDisplayNameFromXmlInner(std::string &cityID, std::string &locale)
{
    xmlKeepBlanksDefault(0);
    std::string xmlPath = GetCityDisplayNameXmlPath(locale);
    xmlDocPtr doc = xmlParseFile(xmlPath.c_str());
    if (!doc) {
        HILOG_ERROR_I18N("FindCityDisplayNameFromXml: can't parse city displayname: %{public}s", xmlPath.c_str());
        return "";
    }
    xmlNodePtr cur = xmlDocGetRootElement(doc);
    if (!cur || xmlStrcmp(cur->name, reinterpret_cast<const xmlChar *>(CITY_DISPLAYNAME_ROOT_TAG))) {
        xmlFreeDoc(doc);
        HILOG_ERROR_I18N(
            "FindCityDisplayNameFromXml: city displayname file %{public}s has wrong root tag.", xmlPath.c_str());
        return "";
    }
    cur = cur->xmlChildrenNode;
    cityDisplayNameCache.clear();
    while (cur != nullptr && !xmlStrcmp(cur->name,
        reinterpret_cast<const xmlChar *>(CITY_DISPLAYNAME_SECOND_ROOT_TAG))) {
        xmlNodePtr value = cur->xmlChildrenNode;
        xmlChar *contents[ELEMENT_NUM] = { 0 }; // 2 represent cityid, displayName;
        bool xmlNodeIsNull = false;
        for (size_t i = 0; i < ELEMENT_NUM && value != nullptr; i++) {
            contents[i] = xmlNodeGetContent(value);
            if (contents[i] == nullptr) {
                xmlNodeIsNull = true;
                break;
            }
            value = value->next;
        }
        if (!xmlNodeIsNull) {
            const char* xmlKey = reinterpret_cast<const char *>(contents[0]);
            const char* xmlValue = reinterpret_cast<const char *>(contents[1]);
            AddCityDisplayNameCache(xmlKey, xmlValue);
        }
        for (size_t i = 0; i < ELEMENT_NUM; i++) {
            if (contents[i] != nullptr) {
                xmlFree(contents[i]);
            }
        }
        cur = cur->next;
    }
    xmlFreeDoc(doc);
    return ResetCityDisplayNameCache(cityID, locale);
}

std::string I18nTimeZone::ResetCityDisplayNameCache(std::string &cityID, std::string &locale)
{
    cityDisplayNameCacheLocale = locale;
    if (cityDisplayNameCache.find(cityID) != cityDisplayNameCache.end()) {
        return cityDisplayNameCache.at(cityID);
    }
    HILOG_ERROR_I18N("I18nTimeZone::ResetCityDisplayNameCache: not found cityID:%{public}s in locale:%{public}s",
        cityID.c_str(), locale.c_str());
    return "";
}

void I18nTimeZone::AddCityDisplayNameCache(const char* xmlCityId, const char* xmlValue)
{
    if (xmlCityId == nullptr) {
        HILOG_ERROR_I18N("I18nTimeZone::AddCityDisplayNameCache: xmlCityId is NULL.");
        return;
    }
    if (xmlValue == nullptr) {
        HILOG_ERROR_I18N("I18nTimeZone::AddCityDisplayNameCache: xmlValue is NULL.");
        return;
    }
    std::string cityId(xmlCityId);
    std::string cityDispName(xmlValue);
    if (!cityId.empty() && !cityDispName.empty()) {
        cityDisplayNameCache.insert(std::make_pair(cityId, cityDispName));
    }
}

std::string I18nTimeZone::GetCityDisplayNameFromCache(std::string &cityID)
{
    if (cityDisplayNameCache.find(cityID) != cityDisplayNameCache.end()) {
        return cityDisplayNameCache.at(cityID);
    }
    HILOG_ERROR_I18N("I18nTimeZone::GetCityDisplayNameFromCache: not found cityID:%{public}s in locale:%{public}s",
        cityID.c_str(), cityDisplayNameCacheLocale.c_str());
    return "";
}

std::string I18nTimeZone::GetCityDisplayNameXmlPath(const std::string &locale)
{
    const char *deviceCityDisplayNamePath = GetDeviceCityDisplayNamePath();
    if (strlen(deviceCityDisplayNamePath) != 0) {
        return deviceCityDisplayNamePath + locale + ".xml";
    } else {
        return CITY_DISPLAYNAME_PATH + locale + ".xml";
    }
}

std::map<std::string, std::string> I18nTimeZone::FindCityDisplayNameMap(std::string &locale)
{
    xmlKeepBlanksDefault(0);
    std::map<std::string, std::string> resultMap;
    std::string xmlPath = GetCityDisplayNameXmlPath(locale);
    xmlDocPtr doc = xmlParseFile(xmlPath.c_str());
    if (!doc) {
        HILOG_ERROR_I18N("FindCityDisplayNameMap: can't parse city displayname file %{public}s", xmlPath.c_str());
        return resultMap;
    }
    xmlNodePtr cur = xmlDocGetRootElement(doc);
    if (!cur || xmlStrcmp(cur->name, reinterpret_cast<const xmlChar *>(CITY_DISPLAYNAME_ROOT_TAG))) {
        xmlFreeDoc(doc);
        HILOG_ERROR_I18N(
            "FindCityDisplayNameMap: city displayname file %{public}s has wrong root tag.", xmlPath.c_str());
        return resultMap;
    }
    cur = cur->xmlChildrenNode;
    while (cur != nullptr && !xmlStrcmp(cur->name,
        reinterpret_cast<const xmlChar *>(CITY_DISPLAYNAME_SECOND_ROOT_TAG))) {
        xmlNodePtr value = cur->xmlChildrenNode;
        xmlChar *contents[ELEMENT_NUM] = { 0 }; // 2 represent cityid, displayName;
        bool xmlNodeIsNull = false;
        for (size_t i = 0; i < ELEMENT_NUM && value != nullptr; i++) {
            contents[i] = xmlNodeGetContent(value);
            value = value->next;
            if (contents[i] == nullptr) {
                xmlNodeIsNull = true;
            }
        }
        if (!xmlNodeIsNull) {
            const char* key = reinterpret_cast<const char *>(contents[0]);
            const char* displayName = reinterpret_cast<const char *>(contents[1]);
            resultMap.insert(std::make_pair<std::string, std::string>(key, displayName));
        }
        for (size_t i = 0; i < ELEMENT_NUM; i++) {
            if (contents[i] != nullptr) {
                xmlFree(contents[i]);
            }
        }
        cur = cur->next;
    }
    xmlFreeDoc(doc);
    return resultMap;
}

bool I18nTimeZone::GetSupportedLocales()
{
    if (initSupportedLocales) {
        return true;
    }
    std::lock_guard<std::mutex> lock(initSupportedLocalesMutex);
    if (initSupportedLocales) {
        return true;
    }

    using std::filesystem::directory_iterator;
    struct stat s;
    std::string displayNamePath = GetDeviceCityDisplayNamePath();
    if (displayNamePath.length() == 0) {
        displayNamePath = CITY_DISPLAYNAME_PATH;
    }
    for (const auto &dirEntry : directory_iterator{displayNamePath}) {
        std::string xmlPath = dirEntry.path();
        if (stat(xmlPath.c_str(), &s) != 0) {
            HILOG_ERROR_I18N("city displayname file %{public}s not exist.", xmlPath.c_str());
            return false;
        }
        int32_t localeStrLen = static_cast<int32_t>(xmlPath.length()) - static_cast<int32_t>(
            displayNamePath.length()) - 4;  // 4 is the length of ".xml"
        std::string localeStr = xmlPath.substr(displayNamePath.length(), localeStrLen);
        supportedLocales.insert(localeStr);
    }
    initSupportedLocales = true;
    return true;
}

std::string I18nTimeZone::GetFallBack(std::string &requestLocaleStr)
{
    auto iter = BEST_MATCH_LOCALE.find(requestLocaleStr);
    if (iter != BEST_MATCH_LOCALE.end()) {
        return iter->second;
    }
    std::vector<LocaleInfo*> matchLocaleList;
    std::unique_ptr<LocaleInfo> requestLocale = std::make_unique<LocaleInfo>(requestLocaleStr);
    for (auto it = supportedLocales.begin(); it != supportedLocales.end(); ++it) {
        std::string tempLocaleStr = it->data();
        tempLocaleStr = StrReplaceAll(tempLocaleStr, "_", "-");
        LocaleInfo* tempLocale = new LocaleInfo(tempLocaleStr);
        if (LocaleMatcher::Match(requestLocale.get(), tempLocale)) {
            matchLocaleList.push_back(tempLocale);
        } else {
            delete tempLocale;
        }
    }
    std::string bestMatchLocaleTag = DEFAULT_LOCALE;
    if (matchLocaleList.size() == 0) {
        return bestMatchLocaleTag;
    }
    LocaleInfo *bestMatch = matchLocaleList[0];
    for (size_t i = 1; i < matchLocaleList.size(); ++i) {
        if (LocaleMatcher::IsMoreSuitable(bestMatch, matchLocaleList[i], requestLocale.get()) < 0) {
            bestMatch = matchLocaleList[i];
        }
    }
    bestMatchLocaleTag = bestMatch->ToString();
    for (size_t i = 0; i < matchLocaleList.size(); ++i) {
        delete matchLocaleList[i];
    }
    bestMatchLocaleTag = StrReplaceAll(bestMatchLocaleTag, "-", "_");
    SetBestMatchLocale(requestLocaleStr, bestMatchLocaleTag);
    return bestMatchLocaleTag;
}

void I18nTimeZone::SetBestMatchLocale(const std::string& key, const std::string& value)
{
    std::lock_guard<std::mutex> matchLocaleLock(matchLocaleMutex);
    BEST_MATCH_LOCALE.insert(std::make_pair(key, value));
}

std::string I18nTimeZone::GetCityDisplayName(std::string &cityID, std::string &localeStr)
{
    GetAvailableZoneCityIDs();
    if (availableZoneCityIDs.find(cityID) == availableZoneCityIDs.end()) {
        HILOG_ERROR_I18N("I18nTimeZone::GetCityDisplayName: %{public}s is not supported cityID.", cityID.c_str());
        return PseudoLocalizationProcessor("");
    }
    std::string requestLocaleStr = GetLocaleBaseName(localeStr);
    if (requestLocaleStr.empty()) {
        HILOG_ERROR_I18N("I18nTimeZone::GetCityDisplayName: requestLocaleStr is empty.");
        return PseudoLocalizationProcessor("");
    }
    std::string displayName = FindCityDisplayNameFromXml(cityID, requestLocaleStr);
    TabooUtils* tabooUtils = TabooUtils::GetInstance();
    if (tabooUtils == nullptr) {
        HILOG_ERROR_I18N("I18nTimeZone::GetCityDisplayName: tabooUtils is nullptr.");
        return PseudoLocalizationProcessor(displayName);
    }
    requestLocaleStr = StrReplaceAll(requestLocaleStr, "_", "-");
    displayName = tabooUtils->ReplaceCityName(cityID, requestLocaleStr, displayName);
    return PseudoLocalizationProcessor(displayName);
}

std::string I18nTimeZone::GetLocaleBaseName(std::string &localeStr)
{
    bool status = GetSupportedLocales();
    if (!status) {
        HILOG_ERROR_I18N("get supported Locales failed");
        return "";
    }
    UErrorCode errorCode = U_ZERO_ERROR;
    icu::Locale locale = icu::Locale::forLanguageTag(localeStr, errorCode);
    if (U_FAILURE(errorCode)) {
        HILOG_ERROR_I18N("create icu Locale for %{public}s failed", localeStr.c_str());
        return "";
    }
    std::string requestLocaleStr = locale.getBaseName();
    requestLocaleStr = StrReplaceAll(requestLocaleStr, "_", "-");
    return GetFallBack(requestLocaleStr);
}

std::string I18nTimeZone::GetTimezoneIdByCityId(const std::string &cityId)
{
    GetAvailableZoneCityIDs();
    if (city2TimeZoneID.find(cityId) == city2TimeZoneID.end()) {
        return "";
    }
    return city2TimeZoneID.at(cityId);
}

std::vector<std::string> I18nTimeZone::GetTimezoneIdByLocation(const double x, const double y)
{
    std::vector<std::string> tzIdList;
#ifdef SUPPORT_GRAPHICS
    if (!CheckLatitudeAndLongitude(x, y)) {
        return tzIdList;
    }
    std::map<int, std::string> categoryMap = GetTimeZoneCategoryMap(x, y);
    std::vector<std::string> filePaths = FindTzData();
    size_t fileCount = filePaths.size();
    if (fileCount < 1) {
        return tzIdList;
    }
    std::string preferredPath = GetPreferredPath(x, filePaths);
    if (preferredPath.empty()) {
        return tzIdList;
    }
    uint32_t width = 0;
    uint32_t height = 0;
    std::tie(width, height) = GetTzDataWidth(filePaths);
    double calculateX = y * width / (TZ_X_PLUS * 1.0) + width / (TZ_HALF_OF_SIZE * 1.0);
    double calculateY = x * ((height * fileCount) / (TZ_X_PLUS * TZ_HALF_OF_SIZE * 1.0)) +
                        (height * fileCount) / (TZ_HALF_OF_SIZE * 1.0);
    uint16_t fixedX = static_cast<uint16_t>(calculateX);
    uint16_t fixedY = static_cast<uint16_t>(calculateY);
    if (ParamExceedScope(fixedX, fixedY, width, height * fileCount)) {
        HILOG_ERROR_I18N("invalid width:%{public}d or height: %{public}d", fixedX, fixedY);
        return tzIdList;
    }
    uint16_t actualHeight = fileCount > 1 ? (fixedY % height) : fixedY;
    std::vector<int> pixel = GetColorData(fixedX, fixedY, actualHeight, preferredPath);
    for (size_t i = 0; i < pixel.size(); i++) {
        //255 is invalid pixel value required
        if (pixel[i] != TZ_MAX_PIXEL_VALUE && categoryMap.find(pixel[i]) != categoryMap.end()) {
            std::string zdId = categoryMap[pixel[i]];
            tzIdList.push_back(zdId);
        }
    }
#endif
    return tzIdList;
}

std::map<int, std::string> I18nTimeZone::GetTimeZoneCategoryMap(const double x, const double y)
{
    if (!InitTimezoneCategoryMap()) {
        HILOG_ERROR_I18N("I18nTimeZone::GetTimeZoneCategoryMap: init timeozne category map failed.");
        return {};
    }
    if (x < 0 && Geq(y, 0)) {
        return categoryNum2TimezoneWN;
    } else if (Geq(x, 0) && Geq(y, 0)) {
        return categoryNum2TimezoneEN;
    } else if (x < 0 && y < 0) {
        return categoryNum2TimezoneWS;
    } else {
        return categoryNum2TimezoneES;
    }
}

const char *I18nTimeZone::GetNumTimezoneCfgPath()
{
    if (access(DISTRO_NUM_TIMEZONE_CFG_PATH, R_OK) != -1) {
        return DISTRO_NUM_TIMEZONE_CFG_PATH;
    }
    if (access(NUM_TIMEZONE_CFG_PATH, R_OK) != -1) {
        return NUM_TIMEZONE_CFG_PATH;
    }
    return nullptr;
}

bool I18nTimeZone::InitTimezoneCategoryMap()
{
    if (numTimezoneCategoryInitialized) {
        return true;
    }
    std::lock_guard<std::mutex> lock(initNumTimezoneCategoryMutex);
    if (numTimezoneCategoryInitialized) {
        return true;
    }
    const char* numTimeZoneCfgPath = GetNumTimezoneCfgPath();
    bool initStatus = ReadNumTimezoneConfigFile(numTimeZoneCfgPath);
    if (initStatus) {
        numTimezoneCategoryInitialized = true;
    }
    return initStatus;
}

bool I18nTimeZone::ReadNumTimezoneConfigFile(const char* configPath)
{
    if (configPath == nullptr) {
        HILOG_ERROR_I18N("I18nTimeZone::ReadNumTimezoneConfigFile: configPath is NULL.");
        return false;
    }
    std::string absPath = GetAbsoluteFilePath(configPath);
    if (absPath.empty()) {
        HILOG_ERROR_I18N("I18nTimeZone::ReadNumTimezoneConfigFile: get absolute path failed.");
        return false;
    }
    FILE* configFile = fopen(absPath.c_str(), "r");
    if (configFile == nullptr) {
        HILOG_ERROR_I18N("I18nTimeZone::ReadNumTimezoneConfigFile: configFile is nullptr.");
        return false;
    }
    char buffer[READ_BUFFER_SIZE];
    std::string curIndex;
    while (fgets(buffer, READ_BUFFER_SIZE - 1, configFile) != nullptr) {
        std::string line(buffer);
        line.resize(line.find_last_not_of("\r\n") + 1);
        line = trim(line);
        std::size_t found = line.find("::");
        if (found != std::string::npos) {
            curIndex = line.substr(0, found);
            continue;
        }
        size_t start = line.find_first_of(",");
        if (start == std::string::npos) {
            HILOG_ERROR_I18N("I18nTimeZone::ReadNumTimezoneConfigFile: %{public}s invalid format", line.c_str());
            continue;
        }
        std::string numStr = line.substr(0, start);
        std::string tzId = line.substr(start + 1);
        tzId = trim(tzId);
        AddNumTimezoneCategoryMap(curIndex, numStr, tzId);
    }
    int ret = fclose(configFile);
    if (ret != 0) {
        HILOG_ERROR_I18N("I18nTimeZone::ReadNumTimezoneConfigFile close %{public}s file error.", absPath.c_str());
    }
    return true;
}

void I18nTimeZone::AddNumTimezoneCategoryMap(std::string &curIndex, std::string &numStr, std::string &tzId)
{
    if (curIndex.empty()) {
        HILOG_ERROR_I18N("I18nTimeZone::AddNumTimezoneCategoryMap: curIndex is empty");
        return;
    }
    if (numStr.empty()) {
        HILOG_ERROR_I18N("I18nTimeZone::AddNumTimezoneCategoryMap: numStr is empty");
        return;
    }
    if (tzId.empty()) {
        HILOG_ERROR_I18N("I18nTimeZone::AddNumTimezoneCategoryMap: tzId is empty");
        return;
    }
    int32_t status = 0;
    int32_t num = ConvertString2Int(numStr, status);
    if (status != 0) {
        HILOG_ERROR_I18N("I18nTimeZone::AddNumTimezoneCategoryMap numStr:%{public}s is not digit.", numStr.c_str());
        return;
    }
    if (curIndex == "west_north") {
        categoryNum2TimezoneWN.insert(std::make_pair(num, tzId));
    } else if (curIndex == "east_north") {
        categoryNum2TimezoneEN.insert(std::make_pair(num, tzId));
    } else if (curIndex == "west_south") {
        categoryNum2TimezoneWS.insert(std::make_pair(num, tzId));
    } else if (curIndex == "east_south") {
        categoryNum2TimezoneES.insert(std::make_pair(num, tzId));
    }
}

bool I18nTimeZone::CheckLatitudeAndLongitude(const double x, const double y)
{
    if (x < -1.0 * TZ_X_PLUS) {
        return false;
    }
    if (Geq(x, TZ_X_PLUS * 1.0 - NUM_PRECISION)) {
        return false;
    }
    if (y < -1.0 * TZ_X_PLUS/TZ_HALF_OF_SIZE) {
        return false;
    }
    if (Geq(y, TZ_X_PLUS * 1.0/TZ_HALF_OF_SIZE - NUM_PRECISION)) {
        return false;
    }
    return true;
}

std::vector<int> I18nTimeZone::GetColorData(const uint16_t x, const uint16_t y,
                                            uint16_t actualHeight, std::string preferredPath)
{
    std::vector<int> result;
    FILE *fp;
    png_structp png_ptr;
    png_infop info_ptr;
    png_bytep row_pointers;
    int code = InitPngptr(png_ptr, info_ptr, &fp, preferredPath);
    if (code != 0) {
        return result;
    }
    try {
        rewind(fp);
        png_init_io(png_ptr, fp);
        png_read_info(png_ptr, info_ptr);
        unsigned int rowbytes = png_get_rowbytes(png_ptr, info_ptr);
        row_pointers = (png_bytep)png_malloc(png_ptr, rowbytes);
        if (row_pointers == nullptr) {
            png_destroy_read_struct(&png_ptr, &info_ptr, 0);
            CloseFile(fp);
            HILOG_ERROR_I18N("malloc rowbytes failed.");
            return result;
        }
        png_start_read_image(png_ptr);
        for (int i = 0; i < (actualHeight + 1); i++) {
            png_read_row(png_ptr, row_pointers, NULL);
        }
        for (size_t i = 0; i < 3; i++) { // 3 is RGB color pixel length
            std::string pixel = std::to_string(*(row_pointers + x * 3 + i)); // 3 is RGB color pixel length
            result.push_back(atoi(pixel.c_str()));
        }
        png_free(png_ptr, row_pointers);
        CloseFile(fp);
    } catch (...) {
        png_destroy_read_struct(&png_ptr, &info_ptr, 0);
        CloseFile(fp);
    }
    return result;
}

std::tuple<uint32_t, uint32_t> I18nTimeZone::GetTzDataWidth(std::vector<std::string> filePaths)
{
    if (filePaths.size() == 0) {
        HILOG_ERROR_I18N("I18nTimeZone::GetTzDataWidth: filePaths is empty.");
        return std::make_tuple(0, 0);
    }
    std::string firstPath = filePaths.at(0);
    firstPath = GetAbsoluteFilePath(firstPath.c_str());
    if (firstPath.empty()) {
        HILOG_ERROR_I18N("I18nTimeZone::GetTzDataWidth: firstPath %{public}s is invalid.", firstPath.c_str());
        return std::make_tuple(0, 0);
    }
    FILE* fp;
    png_structp png_ptr;
    png_infop info_ptr;
    fp = fopen(firstPath.c_str(), "rb");
    if (fp == NULL) {
        HILOG_ERROR_I18N("I18nTimeZone::GetTzDataWidth: fopen(%{public}s) return NULL.", firstPath.c_str());
        return std::make_tuple(0, 0);
    }
    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr) {
        CloseFile(fp);
        HILOG_ERROR_I18N("I18nTimeZone::GetTzDataWidth: create read_struct failed.");
        return std::make_tuple(0, 0);
    }
    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_read_struct(&png_ptr, &info_ptr, 0);
        CloseFile(fp);
        HILOG_ERROR_I18N("I18nTimeZone::GetTzDataWidth: create info_struct failed.");
        return std::make_tuple(0, 0);
    }
    rewind(fp);
    png_init_io(png_ptr, fp);
    png_read_info(png_ptr, info_ptr);
    unsigned int w;
    unsigned int h;
    png_get_IHDR(png_ptr, info_ptr, &w, &h, NULL, NULL, NULL, NULL, NULL);
    png_destroy_read_struct(&png_ptr, &info_ptr, 0);
    CloseFile(fp);
    return std::make_tuple(w, h);
}

int I18nTimeZone::InitPngptr(png_structp &png_ptr, png_infop &info_ptr, FILE **fp,
                             std::string preferredPath)
{
    std::string validFilePath = GetAbsoluteFilePath(preferredPath);
    if (validFilePath.empty()) {
        HILOG_ERROR_I18N("timezone data filepath invalid.");
        return 1;
    }
    *fp = fopen(validFilePath.c_str(), "rb");
    if (*fp == NULL) {
        HILOG_ERROR_I18N("timezone data resource file not exists.");
        return 1;
    }
    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr) {
        png_destroy_read_struct(&png_ptr, &info_ptr, 0);
        CloseFile(*fp);
        HILOG_ERROR_I18N("create read_struct failed.");
        return 1;
    }
    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_read_struct(&png_ptr, &info_ptr, 0);
        CloseFile(*fp);
        HILOG_ERROR_I18N("create info_struct failed.");
        return 1;
    }
    return 0;
}

std::string I18nTimeZone::GetPreferredPath(const double x,
                                           const std::vector<std::string> &filePaths)
{
    if (filePaths.size() == 1) {
        return filePaths.at(0);
    }
    int fixedX = static_cast<int>(x + TZ_X_PLUS);
    for (unsigned int i = 0; i < filePaths.size(); i++) {
        std::string path = filePaths.at(i);
        std::string left = path.substr(path.find("-") + 1, 3);
        std::string right = path.substr(path.find("-") + 4, 3);
        if (fixedX >= atoi(left.c_str()) && fixedX < atoi(right.c_str())) {
            return path;
        }
    }
    return "";
}

std::vector<std::string> I18nTimeZone::FindTzData()
{
    using std::filesystem::directory_iterator;
    std::map<std::string, std::vector<std::string>> pathMap;
    std::regex reg("tz_[0-9]{7}-(\\d{6})\\.dat");
    std::regex regVersion("_[0-9]{7}");
    const std::vector<std::filesystem::path> pixelPaths = {TZ_PIXEL_PATH, DISTRO_TZ_PIXEL_PATH};
    for (const auto& pixelPath : pixelPaths) {
        if (!FileExist(pixelPath)) {
            HILOG_INFO_I18N("pixelPaths does not exists: %{public}s.", pixelPath.c_str());
            continue;
        }

        for (const auto& entry : directory_iterator{pixelPath}) {
            const std::string path = entry.path();
            std::smatch match;
            bool found = RegexSearchNoExcept(path, match, reg);
            bool hasVerison = RegexSearchNoExcept(path, match, regVersion);
            if (found && hasVerison) {
                std::string version = match[0].str();
                SetVersionPathMap(version, path, &pathMap);
            }
        }
    }
    std::vector<std::string> filePaths;
    std::map<std::string, std::vector<std::string>>::reverse_iterator iter;
    for (iter = pathMap.rbegin(); iter != pathMap.rend(); ++iter) {
        if (ValidateDataIntegrity(iter->second)) {
            return iter->second;
        }
    }
    return filePaths;
}

bool I18nTimeZone::ValidateDataIntegrity(const std::vector<std::string> &pathList)
{
    size_t total = 0;
    for (unsigned int i = 0; i < pathList.size(); i++) {
        std::string path = pathList.at(i);
        std::string left = path.substr(path.find("-") + 1, 3);
        std::string right = path.substr(path.find("-") + 4, 3);
        int height = abs(atoi(right.c_str()) - atoi(left.c_str()));
        total += static_cast<size_t>(height);
    }
    return total == TZ_X_PLUS * TZ_HALF_OF_SIZE;
}

void I18nTimeZone::SetVersionPathMap(std::string verison, std::string path,
                                     std::map<std::string, std::vector<std::string>> *pathMap)
{
    if (pathMap == nullptr) {
        return;
    }
    if (pathMap->find(verison) != pathMap->end()) {
        std::vector<std::string> *list = &(pathMap->find(verison)->second);
        list->push_back(path);
    } else {
        std::vector<std::string> list;
        list.push_back(path);
        pathMap->insert(std::pair<std::string, std::vector<std::string>>(verison, list));
    }
}

void I18nTimeZone::CloseFile(FILE *fp)
{
    if (fp != nullptr) {
        fclose(fp);
    }
}

bool I18nTimeZone::ParamExceedScope(const int x, const int y, int fixedX, int fixedY)
{
    if (x < 0 || y < 0 || fixedX == 0 || fixedY == 0) {
        return true;
    }
    if (x > (fixedX - 1) || y > (fixedY - 1)) {
        return true;
    }
    return false;
}

const char *I18nTimeZone::GetDeviceCityDisplayNamePath()
{
    struct stat s;
    if (stat(DISTRO_ROOT_DISPLAYNAME_PATH, &s) == 0) {
        return DISTRO_DEVICE_CITY_DISPLAYNAME_PATH;
    } else if (stat(BASE_DEVICE_CITY_DISPLAYNAME_PATH, &s) == 0) {
        return BASE_DEVICE_CITY_DISPLAYNAME_PATH;
    } else {
        return "";
    }
}
}
}
}
