/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include <filesystem>
#include <sys/stat.h>
#include <unistd.h>
#include "i18n_hilog.h"
#include "i18n_timezone.h"
#include "ohos/init_data.h"
#include "phonenumbers/phonenumberutil.h"
#include "unicode/strenum.h"
#include "unicode/timezone.h"
#include "utils.h"
#include "zone_util.h"

using namespace OHOS::Global::I18n;
using namespace icu;
using namespace std;

const char *ZoneUtil::COUNTRY_ZONE_DATA_PATH = "/system/usr/ohos_timezone/tzlookup.xml";
const char *ZoneUtil::DISTRO_COUNTRY_ZONE_DATA_PATH = "/system/etc/tzdata_distro/hos/tzlookup.xml";
const char *ZoneUtil::DEFAULT_ZONE_CONFIG_PATH = "/system/usr/ohos_timezone/country_default_zone.cfg";
const char *ZoneUtil::DISTRO_DEFAULT_ZONE_CONFIG_PATH = "/system/etc/tzdata_distro/hos/country_default_zone.cfg";
const char *ZoneUtil::DEFAULT_TIMEZONE = "GMT";
const char *ZoneUtil::TIMEZONES_TAG = "timezones";
const char *ZoneUtil::ID_TAG = "id";
const char *ZoneUtil::DEFAULT_TAG = "default";
const char *ZoneUtil::BOOSTED_TAG = "defaultBoost";
const char *ZoneUtil::ROOT_TAG = "countryzones";
const char *ZoneUtil::SECOND_TAG = "country";
const char *ZoneUtil::CODE_TAG = "code";
const char *ZoneUtil::TIMEZONE_KEY = "persist.time.timezone";
const char *ZoneUtil::TZLOOKUP_FILE_PATH = ZoneUtil::GetTZLookupDataPath();
bool ZoneUtil::defaultMapInitialized;
std::mutex ZoneUtil::defaultMapMutex;

unordered_map<string, string> ZoneUtil::defaultMap {};

bool ZoneUtil::icuInitialized = ZoneUtil::Init();

std::string ZoneUtil::GetDefaultZone(const std::string& country)
{
    string ret;
    if (!InitDefaultMap()) {
        HILOG_ERROR_I18N("ZoneUtil::GetDefaultZone: init defaultMap failed.");
        return ret;
    }
    string temp(country);
    for (size_t i = 0; i < temp.size(); i++) {
        temp[i] = (char)toupper(temp[i]);
    }
    if (defaultMap.find(temp) != defaultMap.end()) {
        return defaultMap[temp];
    }
    StringEnumeration *strEnum = TimeZone::createEnumeration(temp.c_str());
    GetString(strEnum, ret);
    if (strEnum != nullptr) {
        delete strEnum;
    }
    return ret;
}

std::string ZoneUtil::GetDefaultZone(int32_t number)
{
    using i18n::phonenumbers::PhoneNumberUtil;
    PhoneNumberUtil* phoneUtil = PhoneNumberUtil::GetInstance();
    if (phoneUtil == nullptr) {
        HILOG_ERROR_I18N("ZoneUtil::GetDefaultZone: Get phone number util failed.");
        return "";
    }
    std::string regionCode;
    phoneUtil->GetRegionCodeForCountryCode(number, &regionCode);
    return GetDefaultZone(regionCode);
}

std::string ZoneUtil::GetDefaultZone(const std::string& country, int32_t offset)
{
    UErrorCode status = U_ZERO_ERROR;
    StringEnumeration *strEnum =
        TimeZone::createTimeZoneIDEnumeration(UCAL_ZONE_TYPE_ANY, country.c_str(), &offset, status);
    if (U_FAILURE(status)) {
        return "";
    }
    string ret;
    GetString(strEnum, ret);
    if (strEnum != nullptr) {
        delete strEnum;
        strEnum = nullptr;
    }
    return ret;
}

std::string ZoneUtil::GetDefaultZone(int32_t number, int32_t offset)
{
    using i18n::phonenumbers::PhoneNumberUtil;
    PhoneNumberUtil* phoneUtil = PhoneNumberUtil::GetInstance();
    if (phoneUtil == nullptr) {
        HILOG_ERROR_I18N("ZoneUtil::GetDefaultZone: Get phone number util failed.");
        return "";
    }
    std::string regionCode;
    phoneUtil->GetRegionCodeForCountryCode(number, &regionCode);
    return GetDefaultZone(regionCode, offset);
}

void ZoneUtil::GetZoneList(const std::string& country, std::vector<std::string>& retVec)
{
    StringEnumeration *strEnum = TimeZone::createEnumeration(country.c_str());
    GetList(strEnum, retVec);
    if (strEnum != nullptr) {
        delete strEnum;
        strEnum = nullptr;
    }
}

void ZoneUtil::GetZoneList(const std::string& country, int32_t offset, std::vector<std::string>& retVec)
{
    UErrorCode status = U_ZERO_ERROR;
    StringEnumeration *strEnum =
        TimeZone::createTimeZoneIDEnumeration(UCAL_ZONE_TYPE_ANY, country.c_str(), &offset, status);
    if (U_FAILURE(status)) {
        delete strEnum;
        strEnum = nullptr;
        return;
    }
    GetList(strEnum, retVec);
    if (strEnum != nullptr) {
        delete strEnum;
        strEnum = nullptr;
    }
}

void ZoneUtil::GetString(StringEnumeration *strEnum, string& ret)
{
    UErrorCode status = U_ZERO_ERROR;
    if (!strEnum) {
        return;
    }
    int32_t count = strEnum->count(status);
    if (U_FAILURE(status) || count <= 0) {
        return;
    }
    const UnicodeString *uniStr = strEnum->snext(status);
    if (U_FAILURE(status) || (!uniStr)) {
        return;
    }
    UnicodeString canonicalUnistring;
    TimeZone::getCanonicalID(*uniStr, canonicalUnistring, status);
    if (U_FAILURE(status)) {
        return;
    }
    canonicalUnistring.toUTF8String(ret);
    return;
}

void ZoneUtil::GetList(StringEnumeration *strEnum, vector<string> &retVec)
{
    if (!strEnum) {
        return;
    }
    UErrorCode status = U_ZERO_ERROR;
    int32_t count = strEnum->count(status);
    if (count <= 0 || U_FAILURE(status)) {
        return;
    }
    while (count > 0) {
        const UnicodeString *uniStr = strEnum->snext(status);
        if ((!uniStr) || U_FAILURE(status)) {
            retVec.clear();
            break;
        }
        UnicodeString canonicalUnistring;
        TimeZone::getCanonicalID(*uniStr, canonicalUnistring, status);
        if (U_FAILURE(status)) {
            retVec.clear();
            break;
        }
        string canonicalString = "";
        canonicalUnistring.toUTF8String(canonicalString);
        if ((canonicalString != "") && (find(retVec.begin(), retVec.end(), canonicalString) == retVec.end())) {
            retVec.push_back(canonicalString);
        }
        --count;
    }
    return;
}

bool ZoneUtil::Init()
{
    SetHwIcuDirectory();
    return true;
}

CountryResult ZoneUtil::LookupTimezoneByCountryAndNITZ(std::string &region, NITZData &nitzData)
{
    std::vector<std::string> zones;
    std::string defaultTimezone;
    std::string systemTimezone = ReadSystemParameter(TIMEZONE_KEY, SYS_PARAM_LEN);
    if (systemTimezone.length() == 0) {
        systemTimezone = DEFAULT_TIMEZONE;
    }
    if (TZLOOKUP_FILE_PATH != nullptr) {
        bool isBoosted = false;
        HILOG_INFO_I18N("ZoneUtil::LookupTimezoneByCountryAndNITZ use tzlookup.xml");
        GetCountryZones(region, defaultTimezone, isBoosted, zones);
    } else {
        HILOG_INFO_I18N("ZoneUtil::LookupTimezoneByCountryAndNITZ use icu data");
        GetICUCountryZones(region, zones, defaultTimezone);
    }
    return Match(zones, nitzData, systemTimezone);
}

CountryResult ZoneUtil::LookupTimezoneByNITZ(NITZData &nitzData)
{
    std::string systemTimezone = ReadSystemParameter(TIMEZONE_KEY, SYS_PARAM_LEN);
    if (systemTimezone.length() == 0) {
        systemTimezone = DEFAULT_TIMEZONE;
    }
    std::set<std::string> icuTimezones = I18nTimeZone::GetAvailableIDs();
    std::vector<std::string> validZones;
    for (auto it = icuTimezones.begin(); it != icuTimezones.end(); ++it) {
        validZones.push_back(*it);
    }

    CountryResult result = Match(validZones, nitzData, systemTimezone);
    if (result.timezoneId.empty() && nitzData.isDST >= 0) {
        NITZData newNITZData = { -1, nitzData.totalOffset, nitzData.currentMillis };  // -1 means not consider DST
        result = Match(validZones, newNITZData, systemTimezone);
    }
    return result;
}

CountryResult ZoneUtil::LookupTimezoneByCountry(std::string &region, int64_t currentMillis)
{
    std::vector<std::string> zones;
    bool isBoosted = false;
    std::string defaultTimezone;
    CountryResult result = { true, MatchQuality::DEFAULT_BOOSTED, defaultTimezone };
    if (TZLOOKUP_FILE_PATH != nullptr) {
        HILOG_INFO_I18N("ZoneUtil::LookupTimezoneByCountry use tzlookup.xml");
        GetCountryZones(region, defaultTimezone, isBoosted, zones);
        if (defaultTimezone.empty()) {
            HILOG_ERROR_I18N("ZoneUtil::LookupTimezoneByCountry can't find default timezone for region %{public}s",
                region.c_str());
        }
    } else {
        HILOG_INFO_I18N("ZoneUtil::LookupTimezoneByCountry use icu data");
        GetICUCountryZones(region, zones, defaultTimezone);
    }
    result.timezoneId = defaultTimezone;
    if (isBoosted) {
        return result;
    }
    if (zones.size() == 0) {
        result.quality = MatchQuality::MULTIPLE_ZONES_DIFFERENT_OFFSET;
    } else if (zones.size() == 1) {
        result.quality = MatchQuality::SINGLE_ZONE;
    } else if (CheckSameDstOffset(zones, defaultTimezone, currentMillis)) {
        result.quality = MatchQuality::MULTIPLE_ZONES_SAME_OFFSET;
    } else {
        result.quality = MatchQuality::MULTIPLE_ZONES_DIFFERENT_OFFSET;
    }
    return result;
}

const char *ZoneUtil::GetTZLookupDataPath()
{
    using std::filesystem::directory_iterator;
    struct stat s;
    if (stat(DISTRO_COUNTRY_ZONE_DATA_PATH, &s) == 0) {
        return DISTRO_COUNTRY_ZONE_DATA_PATH;
    }
    if (stat(COUNTRY_ZONE_DATA_PATH, &s) == 0) {
        return COUNTRY_ZONE_DATA_PATH;
    } else {
        return nullptr;
    }
}

const char *ZoneUtil::GetCountryCodeTimezoneCfgPath()
{
    if (access(DISTRO_DEFAULT_ZONE_CONFIG_PATH, R_OK) != -1) {
        return DISTRO_DEFAULT_ZONE_CONFIG_PATH;
    }
    if (access(DEFAULT_ZONE_CONFIG_PATH, R_OK) != -1) {
        return DEFAULT_ZONE_CONFIG_PATH;
    }
    return nullptr;
}

bool ZoneUtil::IsFindCountry(xmlDocPtr &doc, xmlNodePtr &cur, xmlNodePtr &value,
    std::string &region)
{
    while (cur != nullptr && xmlStrcmp(cur->name, reinterpret_cast<const xmlChar *>(SECOND_TAG)) == 0) {
        value = cur->xmlChildrenNode;
        if (value == nullptr) {
            cur = cur->next;
            continue;
        }
        if (xmlStrcmp(value->name, reinterpret_cast<const xmlChar*>(CODE_TAG)) != 0) {
            xmlFreeDoc(doc);
            HILOG_ERROR_I18N("ZoneUtil::GetCountryZones invalid code_tag");
            return false;
        }
        xmlChar *codePtr = xmlNodeGetContent(value);
        if (codePtr == nullptr) {
            cur = cur->next;
            continue;
        }
        const char* xmlRegion = reinterpret_cast<const char*>(codePtr);
        if (region.compare(xmlRegion) == 0) {
            xmlFree(codePtr);
            return true;
        } else {
            xmlFree(codePtr);
            cur = cur->next;
            continue;
        }
    }
    return false;
}

void ZoneUtil::GetCountryZones(std::string &region, std::string &defaultTimzone, bool &isBoosted,
    std::vector<std::string> &zones)
{
    xmlKeepBlanksDefault(0);
    xmlDocPtr doc = xmlParseFile(TZLOOKUP_FILE_PATH);
    if (!doc) {
        HILOG_ERROR_I18N("ZoneUtil::GetCountryZones can not open tzlookup.xml");
        return;
    }
    xmlNodePtr cur = xmlDocGetRootElement(doc);
    if (!cur || xmlStrcmp(cur->name, reinterpret_cast<const xmlChar *>(ROOT_TAG)) != 0) {
        xmlFreeDoc(doc);
        HILOG_ERROR_I18N("ZoneUtil::GetCountryZones invalid Root_tag");
        return;
    }
    cur = cur->xmlChildrenNode;
    xmlNodePtr value;
    bool findCountry = IsFindCountry(doc, cur, value, region);
    if (findCountry) {
        value = value->next;
        GetDefaultAndBoost(value, defaultTimzone, isBoosted, zones);
    }
    xmlUnlinkNode(cur);
    xmlFreeNode(cur);
    xmlFreeDoc(doc);
    return;
}

void ZoneUtil::GetDefaultAndBoost(xmlNodePtr &value, std::string &defaultTimezone, bool &isBoosted,
    std::vector<std::string> &zones)
{
    if (value == nullptr || xmlStrcmp(value->name, reinterpret_cast<const xmlChar*>(DEFAULT_TAG)) != 0) {
        HILOG_ERROR_I18N("ZoneUtil::GetDefaultAndBoost invalid default_tag");
        return;
    }
    xmlChar *defaultPtr = xmlNodeGetContent(value);
    if (defaultPtr != nullptr) {
        defaultTimezone = reinterpret_cast<const char*>(defaultPtr);
        xmlFree(defaultPtr);
    }
    value = value->next;
    if (value == nullptr) {
        HILOG_ERROR_I18N("ZoneUtil::GetDefaultAndBoost doesn't contains id");
        return;
    }
    if (xmlStrcmp(value->name, reinterpret_cast<const xmlChar *>(BOOSTED_TAG)) == 0) {
        isBoosted = true;
        value = value->next;
    } else {
        isBoosted = false;
    }
    GetTimezones(value, zones);
}

void ZoneUtil::GetTimezones(xmlNodePtr &value, std::vector<std::string> &zones)
{
    if (xmlStrcmp(value->name, reinterpret_cast<const xmlChar *>(TIMEZONES_TAG)) != 0) {
        HILOG_ERROR_I18N("ZoneUtil::GetTimezones invalid timezones_tag");
        return;
    }
    value = value->xmlChildrenNode;
    while (value != nullptr) {
        if (xmlStrcmp(value->name, reinterpret_cast<const xmlChar *>(ID_TAG)) != 0) {
            HILOG_ERROR_I18N("ZoneUtil::GetTimezones invalid id_tag");
            return;
        }
        xmlChar *idPtr = xmlNodeGetContent(value);
        if (idPtr != nullptr) {
            zones.push_back(reinterpret_cast<const char*>(idPtr));
            xmlFree(idPtr);
        }
        value = value->next;
    }
}

void ZoneUtil::GetICUCountryZones(std::string &region, std::vector<std::string> &zones, std::string &defaultTimezone)
{
    std::set<std::string> validZoneIds = I18nTimeZone::GetAvailableIDs();
    std::set<std::string> countryZoneIds;
    StringEnumeration *strEnum = TimeZone::createEnumeration(region.c_str());
    UErrorCode status = U_ZERO_ERROR;
    const UnicodeString *timezoneIdUStr = strEnum == nullptr ? nullptr : strEnum->snext(status);
    while (timezoneIdUStr != nullptr && U_SUCCESS(status)) {
        UnicodeString canonicalUnistring;
        TimeZone::getCanonicalID(*timezoneIdUStr, canonicalUnistring, status);
        std::string timezoneId;
        canonicalUnistring.toUTF8String(timezoneId);
        if (validZoneIds.find(timezoneId) != validZoneIds.end()) {
            countryZoneIds.insert(timezoneId);
        }
        timezoneIdUStr = strEnum->snext(status);
    }
    for (auto it = countryZoneIds.begin(); it != countryZoneIds.end(); ++it) {
        zones.push_back(*it);
    }
    if (InitDefaultMap() && defaultMap.find(region) != defaultMap.end()) {
        defaultTimezone = defaultMap[region];
    } else {
        if (zones.size() > 0) {
            defaultTimezone = zones[0];
        }
    }
    delete strEnum;
}

CountryResult ZoneUtil::Match(std::vector<std::string> &zones, NITZData &nitzData, std::string &systemTimezone)
{
    bool isOnlyMatch = true;
    std::string matchedZoneId;
    bool local = false;
    bool useSystemTimezone = false;
    for (size_t i = 0; i < zones.size(); i++) {
        std::string zoneId = zones[i];
        UnicodeString unicodeZoneID(zoneId.data(), zoneId.length());
        TimeZone *timezone = TimeZone::createTimeZone(unicodeZoneID);
        if (timezone == nullptr) {
            HILOG_ERROR_I18N("ZoneUtil::Match: Create time zone failed.");
            continue;
        }
        int32_t rawOffset;
        int32_t dstOffset;
        UErrorCode status = UErrorCode::U_ZERO_ERROR;
        timezone->getOffset(nitzData.currentMillis, static_cast<UBool>(local), rawOffset, dstOffset, status);
        if (U_FAILURE(status)) {
            HILOG_ERROR_I18N("ZoneUtil::Match: Time zone get offset failed.");
            delete timezone;
            continue;
        }
        if ((nitzData.totalOffset - rawOffset == dstOffset) &&
            (nitzData.isDST < 0 || nitzData.isDST == (dstOffset != 0))) {
            if (matchedZoneId.empty()) {
                matchedZoneId = zoneId;
            } else {
                isOnlyMatch = false;
            }
            if (strcmp(zoneId.c_str(), systemTimezone.c_str()) == 0) {
                matchedZoneId = systemTimezone;
                useSystemTimezone = true;
            }
            if (!isOnlyMatch && useSystemTimezone) {
                delete timezone;
                break;
            }
        }
        delete timezone;
    }
    CountryResult result = {isOnlyMatch, MatchQuality::DEFAULT_BOOSTED, matchedZoneId};
    return result;
}

bool ZoneUtil::CheckSameDstOffset(std::vector<std::string> &zones, std::string &defaultTimezoneId,
    int64_t currentMillis)
{
    UnicodeString defaultID(defaultTimezoneId.data(), defaultTimezoneId.length());
    TimeZone *defaultTimezone = TimeZone::createTimeZone(defaultID);
    if (defaultTimezone == nullptr) {
        HILOG_ERROR_I18N("ZoneUtil::CheckSameDstOffset: Create default time zone failed.");
        return false;
    }
    int32_t rawOffset = 0;
    int32_t dstOffset = 0;
    bool local = false;
    UErrorCode status = U_ZERO_ERROR;
    defaultTimezone->getOffset(currentMillis, (UBool)local, rawOffset, dstOffset, status);
    if (U_FAILURE(status)) {
        delete defaultTimezone;
        HILOG_ERROR_I18N("ZoneUtil::CheckSameDstOffset can not get timezone defaultID offset");
        return false;
    }
    delete defaultTimezone;
    int32_t totalOffset = rawOffset + dstOffset;
    for (size_t i = 0; i < zones.size(); i++) {
        UnicodeString unicodeZoneID(zones[i].data(), zones[i].length());
        TimeZone *timezone = TimeZone::createTimeZone(unicodeZoneID);
        if (timezone == nullptr) {
            HILOG_ERROR_I18N("ZoneUtil::CheckSameDstOffset: Create time zone failed.");
            return false;
        }
        timezone->getOffset(currentMillis, (UBool)local, rawOffset, dstOffset, status);
        if (U_FAILURE(status)) {
            delete timezone;
            HILOG_ERROR_I18N("ZoneUtil::CheckSameDstOffset can not get timezone unicodeZoneID offset");
            return false;
        }
        if (totalOffset - rawOffset != dstOffset) {
            delete timezone;
            return false;
        }
        delete timezone;
    }
    return true;
}

bool ZoneUtil::InitDefaultMap()
{
    if (defaultMapInitialized) {
        return true;
    }
    std::lock_guard<std::mutex> lock(defaultMapMutex);
    if (defaultMapInitialized) {
        return true;
    }
    const char* configPath = GetCountryCodeTimezoneCfgPath();
    bool initStatus = ReadDefaultTimezoneConfigFile(configPath);
    if (initStatus) {
        defaultMapInitialized = true;
    }
    return initStatus;
}

bool ZoneUtil::ReadDefaultTimezoneConfigFile(const char* configPath)
{
    if (configPath == nullptr) {
        HILOG_ERROR_I18N("ZoneUtil::ReadDefaultTimezoneConfigFile: configPath is NULL.");
        return false;
    }
    std::string absPath = GetAbsoluteFilePath(configPath);
    if (absPath.empty()) {
        HILOG_ERROR_I18N("ZoneUtil::ReadDefaultTimezoneConfigFile: get absolute path failed.");
        return false;
    }
    FILE* configFile = fopen(absPath.c_str(), "r");
    if (configFile == nullptr) {
        HILOG_ERROR_I18N("ZoneUtil::ReadDefaultTimezoneConfigFile: configFile is nullptr.");
        return false;
    }
    char buffer[READ_BUFFER_SIZE];
    while (fgets(buffer, READ_BUFFER_SIZE - 1, configFile) != nullptr) {
        std::string line(buffer);
        size_t start = line.find_first_of(":");
        size_t end = line.find_first_of(";");
        if (start == std::string::npos || end == std::string::npos) {
            continue;
        }
        if (end < start + 1) {
            continue;
        }
        std::string countryCode = line.substr(0, start);
        std::string tzId = line.substr(start + 1, end - start - 1);
        defaultMap.insert(std::make_pair(countryCode, tzId));
    }
    int ret = fclose(configFile);
    if (ret != 0) {
        HILOG_ERROR_I18N("ZoneUtil::ReadDefaultTimezoneConfigFile close %{public}s file error.", absPath.c_str());
    }
    return true;
}