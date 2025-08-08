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

std::map<int, std::string> I18nTimeZone::categoryNum2TimezoneWN {
    {0, "Africa/Abidjan"},
    {2, "Africa/Algiers"},
    {5, "Africa/Bissau"},
    {6, "Africa/Casablanca"},
    {7, "Africa/Ceuta"},
    {10, "Africa/El_Aaiun"},
    {13, "Africa/Monrovia"},
    {16, "America/Adak"},
    {17, "America/Anchorage"},
    {22, "America/Bahia_Banderas"},
    {23, "America/Barbados"},
    {24, "America/Belem"},
    {25, "America/Belize"},
    {27, "America/Boa_Vista"},
    {28, "America/Bogota"},
    {29, "America/Boise"},
    {30, "America/Cambridge_Bay"},
    {31, "America/Cancun"},
    {32, "America/Caracas"},
    {33, "America/Cayenne"},
    {35, "America/Chicago"},
    {36, "America/Chihuahua"},
    {37, "America/Ciudad_Juarez"},
    {38, "America/Costa_Rica"},
    {41, "America/Danmarkshavn"},
    {42, "America/Dawson"},
    {43, "America/Dawson_Creek"},
    {44, "America/Denver"},
    {45, "America/Detroit"},
    {47, "America/Edmonton"},
    {48, "America/El_Salvador"},
    {49, "America/Fort_Nelson"},
    {50, "America/Glace_Bay"},
    {51, "America/Goose_Bay"},
    {52, "America/Grand_Turk"},
    {55, "America/Guatemala"},
    {56, "America/Guayaquil"},
    {57, "America/Guyana"},
    {58, "America/Halifax"},
    {59, "America/Havana"},
    {60, "America/Hermosillo"},
    {61, "America/Indiana/Indianapolis"},
    {62, "America/Indiana/Knox"},
    {63, "America/Indiana/Marengo"},
    {64, "America/Indiana/Petersburg"},
    {65, "America/Indiana/Tell_City"},
    {66, "America/Indiana/Vevay"},
    {67, "America/Indiana/Vincennes"},
    {68, "America/Indiana/Winamac"},
    {69, "America/Inuvik"},
    {70, "America/Iqaluit"},
    {71, "America/Jamaica"},
    {72, "America/Juneau"},
    {73, "America/Kentucky/Louisville"},
    {74, "America/Kentucky/Monticello"},
    {76, "America/Los_Angeles"},
    {78, "America/Managua"},
    {79, "America/Manaus"},
    {81, "America/Martinique"},
    {82, "America/Matamoros"},
    {83, "America/Mazatlan"},
    {84, "America/Menominee"},
    {85, "America/Merida"},
    {86, "America/Metlakatla"},
    {87, "America/Mexico_City"},
    {88, "America/Miquelon"},
    {89, "America/Moncton"},
    {90, "America/Monterrey"},
    {93, "America/New_York"},
    {94, "America/Nome"},
    {95, "America/Noronha"},
    {96, "America/North_Dakota/Beulah"},
    {97, "America/North_Dakota/Center"},
    {98, "America/North_Dakota/New_Salem"},
    {99, "America/Nuuk"},
    {100, "America/Ojinaga"},
    {101, "America/Panama"},
    {102, "America/Paramaribo"},
    {103, "America/Phoenix"},
    {104, "America/Port-au-Prince"},
    {106, "America/Puerto_Rico"},
    {107, "America/Rankin_Inlet"},
    {108, "America/Regina"},
    {109, "America/Resolute"},
    {110, "America/Santarem"},
    {111, "America/Santo_Domingo"},
    {112, "America/Scoresbysund"},
    {113, "America/Sitka"},
    {115, "America/St_Johns"},
    {120, "America/Swift_Current"},
    {121, "America/Tegucigalpa"},
    {122, "America/Thule"},
    {123, "America/Tijuana"},
    {124, "America/Toronto"},
    {126, "America/Vancouver"},
    {127, "America/Whitehorse"},
    {128, "America/Winnipeg"},
    {129, "America/Yakutat"},
    {130, "Asia/Anadyr"},
    {131, "Atlantic/Azores"},
    {132, "Atlantic/Bermuda"},
    {133, "Atlantic/Canary"},
    {134, "Atlantic/Cape_Verde"},
    {135, "Atlantic/Faroe"},
    {136, "Atlantic/Madeira"},
    {138, "Etc/GMT"},
    {139, "Etc/GMT+1"},
    {140, "Etc/GMT+10"},
    {141, "Etc/GMT+11"},
    {142, "Etc/GMT+12"},
    {143, "Etc/GMT+2"},
    {144, "Etc/GMT+3"},
    {145, "Etc/GMT+4"},
    {146, "Etc/GMT+5"},
    {147, "Etc/GMT+6"},
    {148, "Etc/GMT+7"},
    {149, "Etc/GMT+8"},
    {150, "Etc/GMT+9"},
    {151, "Etc/GMT-12"},
    {152, "Europe/Dublin"},
    {153, "Europe/Gibraltar"},
    {154, "Europe/Berlin"},
    {157, "Europe/Lisbon"},
    {158, "Europe/London"},
    {159, "Europe/Madrid"},
    {161, "Europe/Paris"},
    {162, "Pacific/Galapagos"},
    {163, "Pacific/Honolulu"},
    {164, "Pacific/Kiritimati"},
    {165, "Pacific/Pago_Pago"}
};
std::map<int, std::string> I18nTimeZone::categoryNum2TimezoneEN {
    {0, "Africa/Abidjan"},
    {2, "Africa/Algiers"},
    {3, "Africa/Johannesburg"},
    {7, "Africa/Cairo"},
    {10, "Africa/Juba"},
    {12, "Africa/Khartoum"},
    {14, "Africa/Lagos"},
    {21, "Africa/Ndjamena"},
    {25, "Africa/Sao_Tome"},
    {26, "Africa/Tripoli"},
    {27, "Africa/Tunis"},
    {28, "America/Adak"},
    {31, "Asia/Almaty"},
    {32, "Asia/Amman"},
    {33, "Asia/Anadyr"},
    {34, "Asia/Aqtau"},
    {35, "Asia/Aqtobe"},
    {36, "Asia/Ashgabat"},
    {37, "Asia/Atyrau"},
    {38, "Asia/Baghdad"},
    {40, "Asia/Baku"},
    {42, "Asia/Barnaul"},
    {43, "Asia/Beirut"},
    {44, "Asia/Bishkek"},
    {46, "Asia/Chita"},
    {48, "Asia/Colombo"},
    {49, "Asia/Damascus"},
    {50, "Asia/Dhaka"},
    {51, "Asia/Dubai"},
    {52, "Asia/Dushanbe"},
    {53, "Asia/Famagusta"},
    {54, "Asia/Gaza"},
    {55, "Asia/Hebron"},
    {56, "Asia/Ho_Chi_Minh"},
    {57, "Asia/Hong_Kong"},
    {58, "Asia/Hovd"},
    {59, "Asia/Irkutsk"},
    {60, "Asia/Jakarta"},
    {62, "Asia/Jerusalem"},
    {63, "Asia/Kabul"},
    {64, "Asia/Kamchatka"},
    {65, "Asia/Karachi"},
    {66, "Asia/Kathmandu"},
    {67, "Asia/Khandyga"},
    {68, "Asia/Kolkata"},
    {69, "Asia/Krasnoyarsk"},
    {73, "Asia/Macau"},
    {74, "Asia/Magadan"},
    {75, "Asia/Makassar"},
    {76, "Asia/Manila"},
    {78, "Asia/Nicosia"},
    {79, "Asia/Novokuznetsk"},
    {80, "Asia/Novosibirsk"},
    {81, "Asia/Omsk"},
    {82, "Asia/Oral"},
    {84, "Asia/Pontianak"},
    {85, "Asia/Pyongyang"},
    {86, "Asia/Qatar"},
    {87, "Asia/Qostanay"},
    {88, "Asia/Qyzylorda"},
    {89, "Asia/Riyadh"},
    {90, "Asia/Sakhalin"},
    {91, "Asia/Samarkand"},
    {92, "Asia/Seoul"},
    {93, "Asia/Shanghai"},
    {94, "Asia/Singapore"},
    {95, "Asia/Srednekolymsk"},
    {96, "Asia/Taipei"},
    {97, "Asia/Tashkent"},
    {98, "Asia/Tbilisi"},
    {99, "Asia/Tehran"},
    {100, "Asia/Thimbu"},
    {101, "Asia/Tokyo"},
    {102, "Asia/Tomsk"},
    {103, "Asia/Ulaanbaatar"},
    {104, "Asia/Urumqi"},
    {105, "Asia/Ust-Nera"},
    {107, "Asia/Vladivostok"},
    {108, "Asia/Yakutsk"},
    {109, "Asia/Yangon"},
    {110, "Asia/Yekaterinburg"},
    {111, "Asia/Yerevan"},
    {112, "Etc/GMT"},
    {113, "Etc/GMT-1"},
    {114, "Etc/GMT-10"},
    {115, "Etc/GMT-11"},
    {116, "Etc/GMT-12"},
    {117, "Etc/GMT-2"},
    {118, "Etc/GMT-3"},
    {119, "Etc/GMT-4"},
    {120, "Etc/GMT-5"},
    {121, "Etc/GMT-6"},
    {122, "Etc/GMT-7"},
    {123, "Etc/GMT-8"},
    {124, "Etc/GMT-9"},
    {126, "Europe/Andorra"},
    {127, "Europe/Astrakhan"},
    {128, "Europe/Athens"},
    {129, "Europe/Belgrade"},
    {130, "Europe/Berlin"},
    {132, "Europe/Brussels"},
    {133, "Europe/Bucharest"},
    {134, "Europe/Budapest"},
    {135, "Pacific/Port_Moresby"},
    {136, "Europe/Chisinau"},
    {138, "Europe/Helsinki"},
    {139, "Europe/Istanbul"},
    {140, "Europe/Kaliningrad"},
    {141, "Europe/Kirov"},
    {142, "Europe/Kiev"},
    {144, "Europe/London"},
    {146, "Europe/Madrid"},
    {147, "Europe/Malta"},
    {149, "Europe/Minsk"},
    {151, "Europe/Moscow"},
    {153, "Europe/Paris"},
    {155, "Europe/Prague"},
    {156, "Europe/Riga"},
    {157, "Europe/Rome"},
    {158, "Europe/Samara"},
    {161, "Europe/Saratov"},
    {162, "Europe/Simferopol"},
    {163, "Europe/Skopje"},
    {164, "Europe/Sofia"},
    {166, "Europe/Tallinn"},
    {167, "Europe/Tirane"},
    {168, "Europe/Ulyanovsk"},
    {170, "Europe/Vienna"},
    {171, "Europe/Vilnius"},
    {172, "Europe/Volgograd"},
    {173, "Europe/Warsaw"},
    {175, "Europe/Zurich"},
    {176, "Indian/Maldives"},
    {177, "Pacific/Chuuk"},
    {178, "Pacific/Guam"},
    {179, "Pacific/Kosrae"},
    {180, "Pacific/Kwajalein"},
    {181, "Pacific/Guadalcanal"},
    {185, "Pacific/Tarawa"}
};
std::map<int, std::string> I18nTimeZone::categoryNum2TimezoneWS {
    {0, "Africa/Johannesburg"},
    {1, "America/Araguaina"},
    {2, "America/Argentina/Buenos_Aires"},
    {3, "America/Argentina/Catamarca"},
    {4, "America/Argentina/Cordoba"},
    {5, "America/Argentina/Jujuy"},
    {6, "America/Argentina/La_Rioja"},
    {7, "America/Argentina/Mendoza"},
    {8, "America/Argentina/Rio_Gallegos"},
    {9, "America/Argentina/Salta"},
    {10, "America/Argentina/San_Juan"},
    {11, "America/Argentina/San_Luis"},
    {12, "America/Argentina/Tucuman"},
    {13, "America/Argentina/Ushuaia"},
    {14, "America/Asuncion"},
    {15, "America/Bahia"},
    {16, "America/Belem"},
    {17, "America/Boa_Vista"},
    {18, "America/Bogota"},
    {19, "America/Campo_Grande"},
    {20, "America/Cuiaba"},
    {21, "America/Eirunepe"},
    {22, "America/Fortaleza"},
    {23, "America/Guayaquil"},
    {24, "America/Coyhaique"},
    {25, "America/Lima"},
    {26, "America/Maceio"},
    {27, "America/Manaus"},
    {28, "America/Montevideo"},
    {29, "America/Noronha"},
    {30, "America/Porto_Velho"},
    {31, "America/Punta_Arenas"},
    {32, "America/Recife"},
    {33, "America/Rio_Branco"},
    {34, "America/Santarem"},
    {35, "America/Santiago"},
    {36, "America/Sao_Paulo"},
    {37, "Africa/Abidjan"},
    {38, "Antarctica/Palmer"},
    {39, "Antarctica/Rothera"},
    {40, "Atlantic/South_Georgia"},
    {41, "America/Puerto_Rico"},
    {42, "Atlantic/Stanley"},
    {43, "Etc/GMT"},
    {44, "Etc/GMT+1"},
    {45, "Etc/GMT+10"},
    {46, "Etc/GMT+11"},
    {47, "Etc/GMT+12"},
    {48, "Etc/GMT+2"},
    {49, "Etc/GMT+3"},
    {50, "Etc/GMT+4"},
    {51, "Etc/GMT+5"},
    {52, "Etc/GMT+6"},
    {53, "Etc/GMT+7"},
    {54, "Etc/GMT+8"},
    {55, "Etc/GMT+9"},
    {56, "Etc/GMT-12"},
    {57, "Etc/UTC"},
    {58, "Pacific/Apia"},
    {59, "Pacific/Auckland"},
    {60, "Pacific/Chatham"},
    {61, "Pacific/Easter"},
    {62, "Pacific/Fakaofo"},
    {63, "Pacific/Fiji"},
    {64, "Pacific/Honolulu"},
    {65, "Pacific/Galapagos"},
    {66, "Pacific/Gambier"},
    {67, "Pacific/Kanton"},
    {68, "Pacific/Kiritimati"},
    {69, "Pacific/Marquesas"},
    {71, "Pacific/Pago_Pago"},
    {72, "Pacific/Pitcairn"},
    {73, "Pacific/Rarotonga"},
    {74, "Pacific/Tarawa"},
    {75, "Pacific/Tongatapu"},
};
std::map<int, std::string> I18nTimeZone::categoryNum2TimezoneES {
    {6, "Africa/Johannesburg"},
    {7, "Africa/Lagos"},
    {8, "Asia/Riyadh"},
    {9, "Asia/Tokyo"},
    {10, "Asia/Yangon"},
    {20, "Africa/Sao_Tome"},
    {21, "Africa/Windhoek"},
    {22, "Antarctica/Casey"},
    {23, "Antarctica/Davis"},
    {25, "Antarctica/Macquarie"},
    {26, "Antarctica/Mawson"},
    {29, "Antarctica/Troll"},
    {30, "Antarctica/Vostok"},
    {31, "Asia/Dili"},
    {32, "Asia/Dubai"},
    {33, "Asia/Jakarta"},
    {34, "Asia/Makassar"},
    {35, "Asia/Pontianak"},
    {36, "Australia/Adelaide"},
    {37, "Australia/Brisbane"},
    {38, "Australia/Broken_Hill"},
    {39, "Australia/Darwin"},
    {40, "Australia/Eucla"},
    {41, "Australia/Hobart"},
    {42, "Australia/Lindeman"},
    {43, "Australia/Lord_Howe"},
    {44, "Australia/Melbourne"},
    {45, "Australia/Perth"},
    {46, "Australia/Sydney"},
    {47, "Etc/GMT"},
    {48, "Etc/GMT-1"},
    {49, "Etc/GMT-10"},
    {50, "Etc/GMT-11"},
    {51, "Etc/GMT-12"},
    {52, "Etc/GMT-2"},
    {53, "Etc/GMT-3"},
    {54, "Etc/GMT-4"},
    {55, "Etc/GMT-5"},
    {56, "Etc/GMT-6"},
    {57, "Etc/GMT-7"},
    {58, "Etc/GMT-8"},
    {59, "Etc/GMT-9"},
    {61, "Indian/Chagos"},
    {67, "Indian/Maldives"},
    {68, "Indian/Mauritius"},
    {71, "Pacific/Auckland"},
    {72, "Pacific/Bougainville"},
    {73, "Pacific/Efate"},
    {74, "Pacific/Fiji"},
    {76, "Pacific/Guadalcanal"},
    {77, "Pacific/Nauru"},
    {78, "Pacific/Norfolk"},
    {79, "Pacific/Noumea"},
    {80, "Pacific/Port_Moresby"},
    {81, "Pacific/Tarawa"}
};

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
    if (i18nTimeZone->GetTimeZone() == nullptr) {
        return nullptr;
    }
    return i18nTimeZone;
}

int32_t I18nTimeZone::GetOffset(double date)
{
    int32_t rawOffset = 0;
    int32_t dstOffset = 0;
    bool local = false;
    UErrorCode status = U_ZERO_ERROR;
    if (timezone == nullptr) {
        return 0;
    }
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
    std::string localeStr = LocaleConfig::GetSystemLocale();
    return GetDisplayName(localeStr, false);
}

std::string I18nTimeZone::GetDisplayName(bool isDST)
{
    std::string localeStr = LocaleConfig::GetSystemLocale();
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
    std::string systemLocale = LocaleConfig::GetSystemLocale();
    icu::Locale locale = icu::Locale::forLanguageTag(systemLocale, status);
    std::string result;
    if (LocaleConfig::IsValidTag(localeStr)) {
        locale = icu::Locale::forLanguageTag(localeStr, status);
    }
    if (U_FAILURE(status)) {
        HILOG_ERROR_I18N("create icu Locale for %{public}s failed.", localeStr.c_str());
        return PseudoLocalizationProcessor(result);
    }
    icu::UnicodeString name;
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

std::set<std::string> I18nTimeZone::GetAvailableIDs(I18nErrorCode &errorCode)
{
    return GetTimeZoneAvailableIDs(errorCode);
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
    if (cityDisplayNameCacheLocale.compare(locale) == 0) {
        return GetCityDisplayNameFromCache(cityID);
    }
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
    } else {
        return city2TimeZoneID.at(cityId);
    }
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
    GetTzDataWidth(filePaths, &width, &height);
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

void I18nTimeZone::GetTzDataWidth(std::vector<std::string> filePaths, uint32_t *width, uint32_t *height)
{
    if (filePaths.size() == 0 || !CheckFilePath(filePaths.at(0))) {
        return;
    } else {
        FILE* fp;
        png_structp png_ptr;
        png_infop info_ptr;
        try {
            std::string path = filePaths.at(0);
            fp = fopen(path.c_str(), "rb");
            if (fp == NULL) {
                return;
            }
            png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
            info_ptr = png_create_info_struct(png_ptr);
            rewind(fp);
            png_init_io(png_ptr, fp);
            png_read_info(png_ptr, info_ptr);
            unsigned int w, h;
            png_get_IHDR(png_ptr, info_ptr, &w, &h, NULL, NULL, NULL, NULL, NULL);
            *width = w;
            *height = h;
            png_destroy_read_struct(&png_ptr, &info_ptr, 0);
            CloseFile(fp);
        } catch(...) {
            png_destroy_read_struct(&png_ptr, &info_ptr, 0);
            CloseFile(fp);
        }
    }
}

int I18nTimeZone::InitPngptr(png_structp &png_ptr, png_infop &info_ptr, FILE **fp,
                             std::string preferredPath)
{
    bool validFilePath = CheckFilePath(preferredPath);
    if (!validFilePath) {
        HILOG_ERROR_I18N("timezone data filepath invalid.");
        return 1;
    }
    *fp = fopen(preferredPath.c_str(), "rb");
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
    } else {
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
    if (fp != nullptr && fp != NULL) {
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
