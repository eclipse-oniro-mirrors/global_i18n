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
#include <regex>
#include "accesstoken_kit.h"
#ifdef SUPPORT_GRAPHICS
#include "app_mgr_client.h"
#include "ability_manager_client.h"
#include "configuration.h"
#include <common_event_data.h>
#include <common_event_manager.h>
#include <common_event_publish_info.h>
#include <common_event_support.h>
#endif
#include <cctype>
#include "config_policy_utils.h"
#include "i18n_hilog.h"
#include "ipc_skeleton.h"
#include "libxml/parser.h"
#include "locale_info.h"
#include "locale_matcher.h"
#include "multi_users.h"
#include "unicode/localebuilder.h"
#include "unicode/locdspnm.h"
#include "unicode/locid.h"
#include "unicode/smpdtfmt.h"
#include "ohos/init_data.h"
#include "parameter.h"
#include "securec.h"
#include "string_ex.h"
#include "ucase.h"
#include "ulocimp.h"
#include "unicode/unistr.h"
#include "ureslocs.h"
#include "unicode/ustring.h"
#include "ustr_imp.h"
#include "utils.h"
#include "tokenid_kit.h"
#include "locale_config.h"

namespace OHOS {
namespace Global {
namespace I18n {
using namespace std;
const std::string LocaleConfig::LANGUAGE_KEY = "persist.global.language";
const std::string LocaleConfig::LOCALE_KEY = "persist.global.locale";
const std::string LocaleConfig::HOUR_KEY = "persist.global.is24Hour";
const std::string LocaleConfig::HOUR_EVENT_DATA = "24HourChange";
const char *LocaleConfig::UPGRADE_LOCALE_KEY = "persist.global.upgrade_locale";
const char *LocaleConfig::DEFAULT_LOCALE_KEY = "const.global.locale";
const char *LocaleConfig::DEFAULT_LANGUAGE_KEY = "const.global.language";
const char *LocaleConfig::DEFAULT_REGION_KEY = "const.global.region";
const char *LocaleConfig::SIM_COUNTRY_CODE_KEY = "telephony.sim.countryCode0";
const char *LocaleConfig::SUPPORTED_LOCALES_NAME = "supported_locales";
const char *LocaleConfig::REGIONS_LANGUAGES_PATH = "etc/xml/i18n_param_config.xml";
const char *LocaleConfig::REGIONS_LANGUAGES_NAME = "i18n_param_config";
const char *LocaleConfig::SUPPORTED_REGIONS_NAME = "supported_regions";
const char *LocaleConfig::WHITE_LANGUAGES_NAME = "white_languages";
const char *LocaleConfig::SUPPORTED_LOCALES_PATH = "/system/usr/ohos_locale_config/supported_locales.xml";
const char *LocaleConfig::SUPPORTED_LOCALES_OLD_PATH = "/system/usr/ohos_locale_config/supported_locales_old.xml";
const char *LocaleConfig::SUPPORT_LOCALES_PATH = "/etc/ohos_lang_config/supported_locales.xml";
const char *LocaleConfig::SUPPORT_LOCALES_NAME = "supported_locales";
const char *LocaleConfig::DIALECT_LANGS_PATH = "/system/usr/ohos_locale_config/dialect_languages.xml";
const char *LocaleConfig::DIALECT_LANGS_NAME = "dialect_langs";
const char *LocaleConfig::OVERRIDE_SUPPORTED_REGIONS_NAME = "supported_regions";
const char *LocaleConfig::OVERRIDE_SUPPORTED_REGIONS_PATH =
    "/system/usr/ohos_locale_config/region/supported_regions.xml";
const char *LocaleConfig::DEFAULT_LOCALE = "en-Latn";
const char *LocaleConfig::supportLocalesTag = "supported_locales";
const char *LocaleConfig::LANG_PATH = "/etc/ohos_lang_config/";
const char *LocaleConfig::REGION_PATH = "/system/usr/ohos_locale_config/region/";
const char *LocaleConfig::rootTag = "languages";
const char *LocaleConfig::secondRootTag = "lang";
const char *LocaleConfig::rootRegion = "regions";
const char *LocaleConfig::secondRootRegion = "region";
const std::string LocaleConfig::EXT_PARAM_KEY = "-u-";
const std::string LocaleConfig::NUMBER_SYSTEM_KEY = "nu";
const std::string LocaleConfig::TEMPERATURE_UNIT_KEY = "mu";
const std::string LocaleConfig::WEEK_DAY_KEY = "fw";
const std::string LocaleConfig::TIMEZONE_KEY = "persist.time.timezone";
const std::string LocaleConfig::DEFAULT_TIMEZONE = "GMT";
unordered_set<string> LocaleConfig::supportedLocales;
unordered_set<string> LocaleConfig::supportedLocalesV15;
unordered_set<string> LocaleConfig::supportLocales;
unordered_set<string> LocaleConfig::supportedRegions;
unordered_set<string> LocaleConfig::overrideSupportedRegions;
unordered_set<string> LocaleConfig::dialectLang;
unordered_set<string> LocaleConfig::whiteLanguages;
std::set<std::string> LocaleConfig::extendWhiteLanguageList;
std::set<std::string> LocaleConfig::extendWhiteLanguageListV15;
unordered_map<string, string> LocaleConfig::dialectMap {
    { "es-Latn-419", "es-Latn-419" },
    { "es-Latn-BO", "es-Latn-419" },
    { "es-Latn-BR", "es-Latn-419" },
    { "es-Latn-BZ", "es-Latn-419" },
    { "es-Latn-CL", "es-Latn-419" },
    { "es-Latn-CO", "es-Latn-419" },
    { "es-Latn-CR", "es-Latn-419" },
    { "es-Latn-CU", "es-Latn-419" },
    { "es-Latn-DO", "es-Latn-419" },
    { "es-Latn-EC", "es-Latn-419" },
    { "es-Latn-GT", "es-Latn-419" },
    { "es-Latn-HN", "es-Latn-419" },
    { "es-Latn-MX", "es-Latn-419" },
    { "es-Latn-NI", "es-Latn-419" },
    { "es-Latn-PA", "es-Latn-419" },
    { "es-Latn-PE", "es-Latn-419" },
    { "es-Latn-PR", "es-Latn-419" },
    { "es-Latn-PY", "es-Latn-419" },
    { "es-Latn-SV", "es-Latn-419" },
    { "es-Latn-US", "es-Latn-419" },
    { "es-Latn-UY", "es-Latn-419" },
    { "es-Latn-VE", "es-Latn-419" },
    { "pt-Latn-PT", "pt-Latn-PT" }
};

unordered_map<string, string> LocaleConfig::dialectMapV15 {
    { "es-Latn-419", "es-Latn-419" },
    { "es-Latn-BO", "es-Latn-419" },
    { "es-Latn-BR", "es-Latn-419" },
    { "es-Latn-BZ", "es-Latn-419" },
    { "es-Latn-CL", "es-Latn-419" },
    { "es-Latn-CO", "es-Latn-419" },
    { "es-Latn-CR", "es-Latn-419" },
    { "es-Latn-CU", "es-Latn-419" },
    { "es-Latn-DO", "es-Latn-419" },
    { "es-Latn-EC", "es-Latn-419" },
    { "es-Latn-GT", "es-Latn-419" },
    { "es-Latn-HN", "es-Latn-419" },
    { "es-Latn-MX", "es-Latn-419" },
    { "es-Latn-NI", "es-Latn-419" },
    { "es-Latn-PA", "es-Latn-419" },
    { "es-Latn-PE", "es-Latn-419" },
    { "es-Latn-PR", "es-Latn-419" },
    { "es-Latn-PY", "es-Latn-419" },
    { "es-Latn-SV", "es-Latn-419" },
    { "es-Latn-US", "es-Latn-419" },
    { "es-Latn-UY", "es-Latn-419" },
    { "es-Latn-VE", "es-Latn-419" },
    { "pt-Latn-PT", "pt-Latn-PT" },
    { "en-Latn-US", "en-Latn-US" }
};

unordered_map<string, string> LocaleConfig::localDigitMap {
    { "ar", "arab" },
    { "as", "beng" },
    { "bn", "beng" },
    { "fa", "arabext" },
    { "mr", "deva" },
    { "my", "mymr" },
    { "ne", "deva" },
    { "ur", "latn" }
};

std::unordered_map<std::string, std::vector<std::string>> LocaleConfig::dialectLanguages {
    { "en-Latn", { "en-Latn-US", "en-Latn-GB" } },
    { "pt-Latn", { "pt-Latn-PT", "pt-Latn-BR" } },
    { "zh-Hant", { "zh-Hant-HK", "zh-Hant-TW" } },
};

std::unordered_map<std::string, std::string> LocaleConfig::resourceIdMap {
    { "zh", "zh-Hans" },
    { "zh-HK", "zh-Hant-HK" },
    { "zh-TW", "zh-Hant" },
    { "az", "az-Latn" },
    { "bs", "bs-Latn" },
    { "jv", "jv-Latn" },
    { "uz", "uz-Latn" },
    { "mn", "mn-Cyrl" },
};

std::unordered_map<TemperatureType, std::string> LocaleConfig::temperatureTypeToName {
    { TemperatureType::CELSIUS, "celsius" },
    { TemperatureType::FAHRENHEIT, "fahrenhe" },
    { TemperatureType::KELVIN, "kelvin" }
};

std::unordered_map<std::string, TemperatureType> LocaleConfig::nameToTemperatureType {
    { "celsius", TemperatureType::CELSIUS },
    { "fahrenhe", TemperatureType::FAHRENHEIT },
    { "kelvin", TemperatureType::KELVIN }
};

std::unordered_set<std::string> LocaleConfig::fahrenheitUsingRegions {
    "BS",
    "BZ",
    "KY",
    "PW",
    "US"
};

std::unordered_map<WeekDay, std::string> LocaleConfig::weekDayToName {
    { WeekDay::MON, "mon" },
    { WeekDay::TUE, "tue" },
    { WeekDay::WED, "wed" },
    { WeekDay::THU, "thu" },
    { WeekDay::FRI, "fri" },
    { WeekDay::SAT, "sat" },
    { WeekDay::SUN, "sun" }
};

std::unordered_map<std::string, WeekDay> LocaleConfig::nameToWeekDay {
    { "mon", WeekDay::MON },
    { "tue", WeekDay::TUE },
    { "wed", WeekDay::WED },
    { "thu", WeekDay::THU },
    { "fri", WeekDay::FRI },
    { "sat", WeekDay::SAT },
    { "sun", WeekDay::SUN }
};

std::unordered_map<icu::Calendar::EDaysOfWeek, WeekDay> LocaleConfig::eDaysOfWeekToWeekDay {
    { icu::Calendar::EDaysOfWeek::MONDAY, WeekDay::MON },
    { icu::Calendar::EDaysOfWeek::TUESDAY, WeekDay::TUE },
    { icu::Calendar::EDaysOfWeek::WEDNESDAY, WeekDay::WED },
    { icu::Calendar::EDaysOfWeek::THURSDAY, WeekDay::THU },
    { icu::Calendar::EDaysOfWeek::FRIDAY, WeekDay::FRI },
    { icu::Calendar::EDaysOfWeek::SATURDAY, WeekDay::SAT },
    { icu::Calendar::EDaysOfWeek::SUNDAY, WeekDay::SUN }
};

std::map<std::string, std::string> LocaleConfig::supportedDialectLocales;
std::map<string, string> LocaleConfig::locale2DisplayName {};
std::map<string, string> LocaleConfig::region2DisplayName {};
std::unordered_map<std::string, bool> LocaleConfig::is24HourLocaleMap {};
std::string LocaleConfig::currentDialectLocale = "";
std::string LocaleConfig::currentOverrideRegion = "";
std::mutex LocaleConfig::dialectLocaleMutex;
std::mutex LocaleConfig::region2DisplayNameMutex;
std::mutex LocaleConfig::locale2DisplayNameMutex;
std::shared_mutex LocaleConfig::is24HourLocaleMapMutex;

set<std::string> LocaleConfig::validCaTag {
    "buddhist",
    "chinese",
    "coptic",
    "dangi",
    "ethioaa",
    "ethiopic",
    "gregory",
    "hebrew",
    "indian",
    "islamic",
    "islamic-umalqura",
    "islamic-tbla",
    "islamic-civil",
    "islamic-rgsa",
    "iso8601",
    "japanese",
    "persian",
    "roc",
    "islamicc",
};
set<std::string> LocaleConfig::validCoTag {
    "big5han",
    "compat",
    "dict",
    "direct",
    "ducet",
    "eor",
    "gb2312",
    "phonebk",
    "phonetic",
    "pinyin",
    "reformed",
    "searchjl",
    "stroke",
    "trad",
    "unihan",
    "zhuyin",
};
set<std::string> LocaleConfig::validKnTag {
    "true",
    "false",
};
set<std::string> LocaleConfig::validKfTag {
    "upper",
    "lower",
    "false",
};
set<std::string> LocaleConfig::validNuTag {
    "adlm", "ahom", "arab", "arabext", "bali", "beng",
    "bhks", "brah", "cakm", "cham", "deva", "diak",
    "fullwide", "gong", "gonm", "gujr", "guru", "hanidec",
    "hmng", "hmnp", "java", "kali", "khmr", "knda",
    "lana", "lanatham", "laoo", "latn", "lepc", "limb",
    "mathbold", "mathdbl", "mathmono", "mathsanb", "mathsans", "mlym",
    "modi", "mong", "mroo", "mtei", "mymr", "mymrshan",
    "mymrtlng", "newa", "nkoo", "olck", "orya", "osma",
    "rohg", "saur", "segment", "shrd", "sind", "sinh",
    "sora", "sund", "takr", "talu", "tamldec", "telu",
    "thai", "tibt", "tirh", "vaii", "wara", "wcho",
};
set<std::string> LocaleConfig::validHcTag {
    "h12",
    "h23",
    "h11",
    "h24",
};

static unordered_map<string, string> g_languageMap = {
    { "zh-Hans", "zh-Hans" },
    { "zh-Hant-HK", "zh-Hant-HK" },
    { "zh-Hant", "zh-Hant" },
    { "my-Qaag", "my-Qaag" },
    { "es-Latn-419", "es-419" },
    { "es-Latn-US", "es-419" },
    { "az-Latn", "az-Latn" },
    { "bs-Latn", "bs-Latn" },
    { "en-Latn-US", "en" },
    { "en-Qaag", "en-Qaag" },
    { "uz-Latn", "uz-Latn" },
    { "sr-Latn", "sr-Latn" },
    { "jv-Latn", "jv-Latn" },
    { "pt-Latn-BR", "pt-BR" },
    { "pa-Guru", "pa-Guru" },
    { "mai-Deva", "mai-Deva" }
};

string Adjust(const string &origin)
{
    auto iter = g_languageMap.find(origin);
    if (iter != g_languageMap.end()) {
        return iter->second;
    }
    for (iter = g_languageMap.begin(); iter != g_languageMap.end(); ++iter) {
        string key = iter->first;
        if (!origin.compare(0, key.length(), key)) {
            return iter->second;
        }
    }
    return origin;
}

string GetDisplayLanguageInner(const string &language, const string &displayLocaleTag, bool sentenceCase)
{
    icu::UnicodeString unistr;
    // 0 is the start position of language, 2 is the length of zh and fa
    if (!language.compare(0, 2, "zh") || !language.compare(0, 2, "fa") || !language.compare(0, 2, "ro")) {
        UErrorCode status = U_ZERO_ERROR;
        icu::Locale displayLocale = icu::Locale::forLanguageTag(displayLocaleTag.c_str(), status);
        if (U_FAILURE(status)) {
            return "";
        }
        icu::Locale tempLocale = icu::Locale::forLanguageTag(language.c_str(), status);
        if (U_FAILURE(status)) {
            return "";
        }
        icu::LocaleDisplayNames *dspNames = icu::LocaleDisplayNames::createInstance(displayLocale,
            UDialectHandling::ULDN_DIALECT_NAMES);
        if (dspNames != nullptr) {
            dspNames->localeDisplayName(tempLocale, unistr);
            delete dspNames;
        }
    } else {
        UErrorCode status = U_ZERO_ERROR;
        icu::Locale displayLoc = icu::Locale::forLanguageTag(displayLocaleTag, status);
        if (U_FAILURE(status)) {
            return "";
        }
        icu::Locale locale = icu::Locale::forLanguageTag(language, status);
        if (U_FAILURE(status)) {
            return "";
        }
        locale.getDisplayName(displayLoc, unistr);
    }
    if (sentenceCase) {
        UChar32 ch = ucase_toupper(unistr.char32At(0));
        unistr.replace(0, 1, ch);
    }
    string out;
    unistr.toUTF8String(out);
    return out;
}

bool LocaleConfig::listsInitialized = LocaleConfig::InitializeLists();

std::string LocaleConfig::GetEffectiveLanguage()
{
    std::string systemLocale = GetSystemLocale();
    std::string systemLanguage = GetSystemLanguage();
    return ComputeEffectiveLanguage(systemLocale, systemLanguage);
}

string LocaleConfig::GetSystemLanguage()
{
    std::string systemLanguage = ReadSystemParameter(LANGUAGE_KEY.c_str(), CONFIG_LEN);
    if (!systemLanguage.empty()) {
        return systemLanguage;
    }
    systemLanguage = ReadSystemParameter(DEFAULT_LANGUAGE_KEY, CONFIG_LEN);
    if (systemLanguage.empty()) {
        HILOG_ERROR_I18N("LocaleConfig::GetSystemLanguage: Read default system language failed.");
        return "zh-Hans";
    }
    return systemLanguage;
}

string LocaleConfig::GetSystemRegion()
{
    std::string systemRegion = GetCountry(LOCALE_KEY);
    if (systemRegion.empty()) {
        systemRegion = GetCountry(DEFAULT_LOCALE_KEY);
    }
    if (systemRegion.empty()) {
        HILOG_ERROR_I18N("LocaleConfig::GetSystemRegion: system region is empty.");
        return "CN";
    }
    return systemRegion;
}

string LocaleConfig::GetCountry(const string& parameter)
{
    std::string systemRegion = ReadSystemParameter(parameter.c_str(), CONFIG_LEN);
    if (systemRegion.empty()) {
        HILOG_ERROR_I18N("LocaleConfig::GetCountry: ReadSystemParameter %{public}s failed.", parameter.c_str());
        return systemRegion;
    }
    UErrorCode status = U_ZERO_ERROR;
    icu::Locale origin = icu::Locale::forLanguageTag(systemRegion, status);
    if (U_FAILURE(status)) {
        HILOG_ERROR_I18N("LocaleConfig::GetCountry: Create icu::Locale failed.");
        return "";
    }
    const char* country = origin.getCountry();
    if (country == nullptr) {
        HILOG_ERROR_I18N("LocaleConfig::GetCountry: Get country from icu::Locale failed.");
        return "";
    }
    return country;
}

string LocaleConfig::GetSystemLocale()
{
    std::string systemLocale = ReadSystemParameter(LOCALE_KEY.c_str(), CONFIG_LEN);
    if (!systemLocale.empty()) {
        return systemLocale;
    }
    HILOG_ERROR_I18N("LocaleConfig::GetSystemLocale: Read system locale failed.");
    systemLocale = ReadSystemParameter(DEFAULT_LOCALE_KEY, CONFIG_LEN);
    if (systemLocale.empty()) {
        HILOG_ERROR_I18N("LocaleConfig::GetSystemLocale: Read default system locale failed.");
        return "zh-Hans-CN";
    }
    return systemLocale;
}

std::string LocaleConfig::GetSystemTimezone()
{
    std::string systemTimezone = ReadSystemParameter(TIMEZONE_KEY.c_str(), CONFIG_LEN);
    if (systemTimezone.empty()) {
        HILOG_ERROR_I18N("LocaleConfig::GetSystemTimezone: Read system time zone failed.");
        return DEFAULT_TIMEZONE;
    }
    return systemTimezone;
}

bool LocaleConfig::IsValidLanguage(const string &language)
{
    string::size_type size = language.size();
    if ((size != LANGUAGE_LEN) && (size != LANGUAGE_LEN + 1)) {
        return false;
    }
    for (size_t i = 0; i < size; ++i) {
        if ((language[i] > 'z') || (language[i] < 'a')) {
            return false;
        }
    }
    return true;
}

bool LocaleConfig::IsValidRegion(const string &region)
{
    string::size_type size = region.size();
    if (size != LocaleInfo::REGION_LEN) {
        return false;
    }
    for (size_t i = 0; i < LocaleInfo::REGION_LEN; ++i) {
        if ((region[i] > 'Z') || (region[i] < 'A')) {
            return false;
        }
    }
    return true;
}

bool LocaleConfig::IsValidTag(const string &tag)
{
    if (!tag.size()) {
        return false;
    }
    vector<string> splits;
    Split(tag, "-", splits);
    if (!IsValidLanguage(splits[0])) {
        return false;
    }
    return true;
}

void LocaleConfig::Split(const string &src, const string &sep, vector<string> &dest)
{
    string::size_type begin = 0;
    string::size_type end = src.find(sep);
    while (end != string::npos) {
        dest.push_back(src.substr(begin, end - begin));
        begin = end + sep.size();
        end = src.find(sep, begin);
    }
    if (begin != src.size()) {
        dest.push_back(src.substr(begin));
    }
}

void LocaleConfig::Split(const string &src, const string &sep, std::unordered_set<string> &dest)
{
    string::size_type begin = 0;
    string::size_type end = src.find(sep);
    while (end != string::npos) {
        dest.insert(src.substr(begin, end - begin));
        begin = end + sep.size();
        end = src.find(sep, begin);
    }
    if (begin != src.size()) {
        dest.insert(src.substr(begin));
    }
}

// language in white languages should have script.
std::unordered_set<std::string> LocaleConfig::GetSystemLanguages()
{
    std::unordered_set<std::string> result(whiteLanguages);
    std::unordered_set<string> blockedLanguages = GetBlockedLanguages();
    Expunge(result, blockedLanguages);
    return result;
}

const unordered_set<string>& LocaleConfig::GetSupportedLocales()
{
    return supportedLocales;
}

const unordered_set<string>& LocaleConfig::GetSupportedLocalesV15()
{
    return supportedLocalesV15;
}

const unordered_set<string>& LocaleConfig::GetSupportedRegions()
{
    return supportedRegions;
}

std::unordered_set<std::string> LocaleConfig::GetSystemCountries(const std::string& language)
{
    std::unordered_set<std::string> result(supportedRegions);
    std::unordered_set<std::string> blockedRegion = LocaleConfig::GetBlockedRegions(language);
    Expunge(result, blockedRegion);
    return result;
}

bool LocaleConfig::IsSuggested(const string &language)
{
    unordered_set<string> relatedLocales;
    vector<string> simCountries;
    GetCountriesFromSim(simCountries);
    GetRelatedLocales(relatedLocales, simCountries);
    for (auto iter = relatedLocales.begin(); iter != relatedLocales.end();) {
        if (extendWhiteLanguageList.find(*iter) == extendWhiteLanguageList.end()) {
            iter = relatedLocales.erase(iter);
        } else {
            ++iter;
        }
    }
    string mainLanguage = GetMainLanguage(language, dialectMap);
    return relatedLocales.find(mainLanguage) != relatedLocales.end();
}

bool LocaleConfig::IsSuggested(const std::string &language, const std::string &region)
{
    unordered_set<string> relatedLocales;
    vector<string> countries { region };
    GetRelatedLocales(relatedLocales, countries);
    for (auto iter = relatedLocales.begin(); iter != relatedLocales.end();) {
        if (extendWhiteLanguageList.find(*iter) == extendWhiteLanguageList.end()) {
            iter = relatedLocales.erase(iter);
        } else {
            ++iter;
        }
    }
    string mainLanguage = GetMainLanguage(language, dialectMap);
    return relatedLocales.find(mainLanguage) != relatedLocales.end();
}

bool LocaleConfig::IsSuggestedV15(const string &language)
{
    unordered_set<string> relatedLocales;
    vector<string> simCountries;
    GetCountriesFromSim(simCountries);
    GetRelatedLocalesV15(relatedLocales, simCountries);
    auto iter = relatedLocales.begin();
    while (iter != relatedLocales.end()) {
        if (extendWhiteLanguageListV15.find(*iter) == extendWhiteLanguageListV15.end()) {
            iter = relatedLocales.erase(iter);
        } else {
            ++iter;
        }
    }
    string mainLanguage = GetMainLanguage(language, dialectMapV15);
    return relatedLocales.find(mainLanguage) != relatedLocales.end();
}

bool LocaleConfig::IsSuggestedV15(const std::string &language, const std::string &region)
{
    unordered_set<string> relatedLocales;
    vector<string> countries { region };
    GetRelatedLocalesV15(relatedLocales, countries);
    auto iter = relatedLocales.begin();
    while (iter != relatedLocales.end()) {
        if (extendWhiteLanguageListV15.find(*iter) == extendWhiteLanguageListV15.end()) {
            iter = relatedLocales.erase(iter);
        } else {
            ++iter;
        }
    }
    string mainLanguage = GetMainLanguage(language, dialectMapV15);
    return relatedLocales.find(mainLanguage) != relatedLocales.end();
}

void LocaleConfig::ExtendWhiteLanguages()
{
    UErrorCode status = U_ZERO_ERROR;
    for (auto iter = whiteLanguages.begin(); iter != whiteLanguages.end(); ++iter) {
        extendWhiteLanguageList.insert(*iter);
        extendWhiteLanguageListV15.insert(*iter);
        icu::Locale locale = icu::Locale::forLanguageTag((*iter).c_str(), status);
        locale.addLikelySubtags(status);
        if (U_FAILURE(status)) {
            HILOG_INFO_I18N("create Locale object for %{public}s failed.", (*iter).c_str());
            continue;
        }
        const char* baseName = locale.getBaseName();
        if (baseName != nullptr) {
            std::string baseNameStr(baseName);
            std::replace(baseNameStr.begin(), baseNameStr.end(), '_', '-');
            extendWhiteLanguageList.insert(baseNameStr);
            extendWhiteLanguageListV15.insert(baseNameStr);
        }
        const char* language = locale.getLanguage();
        if (language != nullptr) {
            std::string languageStr(language);
            ExtendLanguageWithScript(languageStr);
        }
    }
}

void LocaleConfig::ExtendLanguageWithScript(const std::string &languageStr)
{
    if (languageStr.empty()) {
        HILOG_INFO_I18N("ExtendLanguageWithScript languageStr is empty.");
        return;
    }
    string mainLanguage = GetMainLanguage(languageStr, dialectMap);
    if (!mainLanguage.empty()) {
        extendWhiteLanguageList.insert(mainLanguage);
    }
}

void LocaleConfig::GetRelatedLocales(unordered_set<string> &relatedLocales, vector<string> countries)
{
    // remove unsupported countries
    const unordered_set<string> &regions = GetSupportedRegions();
    auto iter = countries.begin();
    while (iter != countries.end()) {
        if (regions.find(*iter) == regions.end()) {
            iter = countries.erase(iter);
        } else {
            ++iter;
        }
    }
    const unordered_set<string> &locales = GetSupportedLocales();
    for (const string &locale : locales) {
        bool find = false;
        for (string &country : countries) {
            if (locale.find(country) != string::npos) {
                find = true;
                break;
            }
        }
        if (!find) {
            continue;
        }
        string mainLanguage = GetMainLanguage(locale, dialectMap);
        if (!mainLanguage.empty()) {
            relatedLocales.insert(mainLanguage);
        }
    }
}

void LocaleConfig::GetRelatedLocalesV15(unordered_set<string> &relatedLocales, vector<string> countries)
{
    // remove unsupported countries
    const unordered_set<string> &regions = GetSupportedRegions();
    auto iter = countries.begin();
    while (iter != countries.end()) {
        if (regions.find(*iter) == regions.end()) {
            iter = countries.erase(iter);
        } else {
            ++iter;
        }
    }
    const unordered_set<string> &locales = GetSupportedLocalesV15();
    for (const string &locale : locales) {
        bool find = false;
        for (const string &country : countries) {
            if (locale.find(country) != string::npos) {
                find = true;
                break;
            }
        }
        if (!find) {
            continue;
        }
        string mainLanguage = GetMainLanguage(locale, dialectMapV15);
        if (!mainLanguage.empty()) {
            relatedLocales.insert(mainLanguage);
        }
    }
}

void LocaleConfig::GetCountriesFromSim(vector<string> &simCountries)
{
    simCountries.push_back(GetSystemRegion());
    char value[CONFIG_LEN];
    int code = GetParameter(SIM_COUNTRY_CODE_KEY, "", value, CONFIG_LEN);
    if (code > 0) {
        simCountries.push_back(value);
    }
}

void LocaleConfig::GetListFromFile(const char *path, const char *resourceName, unordered_set<string> &ret)
{
    xmlKeepBlanksDefault(0);
    if (!path) {
        return;
    }
    xmlDocPtr doc = xmlParseFile(path);
    if (!doc) {
        return;
    }
    xmlNodePtr cur = xmlDocGetRootElement(doc);
    if (!cur || xmlStrcmp(cur->name, reinterpret_cast<const xmlChar *>(resourceName))) {
        xmlFreeDoc(doc);
        return;
    }
    cur = cur->xmlChildrenNode;
    xmlChar *content = nullptr;
    while (cur != nullptr) {
        content = xmlNodeGetContent(cur);
        if (content != nullptr) {
            ret.insert(reinterpret_cast<const char*>(content));
            xmlFree(content);
            cur = cur->next;
        } else {
            break;
        }
    }
    xmlFreeDoc(doc);
}

void LocaleConfig::Expunge(unordered_set<string> &src, const unordered_set<string> &another)
{
    for (auto iter = src.begin(); iter != src.end();) {
        if (another.find(*iter) != another.end()) {
            iter = src.erase(iter);
        } else {
            ++iter;
        }
    }
}

bool LocaleConfig::InitializeLists()
{
    SetHwIcuDirectory();
    LoadRegionsLanguages();
    GetListFromFile(SUPPORTED_LOCALES_PATH, SUPPORTED_LOCALES_NAME, supportedLocales);
    GetListFromFile(SUPPORTED_LOCALES_OLD_PATH, SUPPORTED_LOCALES_NAME, supportedLocalesV15);
    GetListFromFile(SUPPORT_LOCALES_PATH, SUPPORT_LOCALES_NAME, supportLocales);
    GetListFromFile(OVERRIDE_SUPPORTED_REGIONS_PATH, OVERRIDE_SUPPORTED_REGIONS_NAME, overrideSupportedRegions);
    GetListFromFile(DIALECT_LANGS_PATH, DIALECT_LANGS_NAME, dialectLang);
    ExtendWhiteLanguages();
    return true;
}

__attribute__((destructor)) void LocaleConfig::CleanupXmlResources()
{
    if (listsInitialized) {
        xmlCleanupParser();
    }
}

void LocaleConfig::LoadRegionsLanguages()
{
    char buf[MAX_PATH_LEN] = {0};
    char* path = GetOneCfgFile(REGIONS_LANGUAGES_PATH, buf, MAX_PATH_LEN);
    xmlKeepBlanksDefault(0);
    if (!path) {
        return;
    }
    xmlDocPtr doc = xmlParseFile(path);
    if (!doc) {
        return;
    }
    xmlNodePtr cur = xmlDocGetRootElement(doc);
    if (!cur || xmlStrcmp(cur->name, reinterpret_cast<const xmlChar *>(REGIONS_LANGUAGES_NAME))) {
        xmlFreeDoc(doc);
        return;
    }
    cur = cur->xmlChildrenNode;
    xmlChar *content = nullptr;
    while (cur != nullptr) {
        content = xmlNodeGetContent(cur);
        if (content != nullptr && !xmlStrcmp(cur->name, reinterpret_cast<const xmlChar *>(SUPPORTED_REGIONS_NAME))) {
            Split(reinterpret_cast<const char*>(content), ",", supportedRegions);
        } else if (content != nullptr && !xmlStrcmp(cur->name,
            reinterpret_cast<const xmlChar *>(WHITE_LANGUAGES_NAME))) {
            Split(reinterpret_cast<const char*>(content), ",", whiteLanguages);
        }
        if (content != nullptr) {
            xmlFree(content);
        }
        cur = cur->next;
    }
    xmlFreeDoc(doc);
}

string LocaleConfig::GetMainLanguage(const string &language,
    std::unordered_map<std::string, std::string> selfDialectMap)
{
    UErrorCode status = U_ZERO_ERROR;
    icu::Locale origin = icu::Locale::forLanguageTag(language, status);
    if (U_FAILURE(status)) {
        return "";
    }
    origin.addLikelySubtags(status);
    if (U_FAILURE(status)) {
        return "";
    }
    icu::LocaleBuilder builder = icu::LocaleBuilder().setLanguage(origin.getLanguage()).
        setScript(origin.getScript()).setRegion(origin.getCountry());
    icu::Locale temp = builder.setExtension('u', "").build(status);
    if (U_FAILURE(status)) {
        return "";
    }
    string fullLanguage = temp.toLanguageTag<string>(status);
    if (U_FAILURE(status)) {
        return "";
    }
    if (selfDialectMap.find(fullLanguage) != selfDialectMap.end()) {
        return selfDialectMap[fullLanguage];
    }
    builder.setRegion("");
    temp = builder.build(status);
    if (U_FAILURE(status)) {
        return "";
    }
    fullLanguage = temp.toLanguageTag<string>(status);
    if (U_FAILURE(status)) {
        return "";
    }
    return fullLanguage;
}

string LocaleConfig::GetDisplayLanguage(const string &language, const string &displayLocale, bool sentenceCase)
{
    std::string result;
    string adjust = Adjust(language);
    if (adjust == language) {
        UErrorCode status = U_ZERO_ERROR;
        icu::Locale displayLoc = icu::Locale::forLanguageTag(displayLocale, status);
        if (U_FAILURE(status)) {
            return PseudoLocalizationProcessor("");
        }
        icu::Locale locale = icu::Locale::forLanguageTag(language, status);
        if (U_FAILURE(status)) {
            return PseudoLocalizationProcessor("");
        }
        icu::UnicodeString unistr;
        std::string lang(locale.getLanguage());
        if (dialectLang.find(lang) != dialectLang.end()) {
            result = GetDisplayLanguageWithDialect(language, displayLocale);
        }
    }
    if (result.empty()) {
        result = GetDisplayLanguageInner(adjust, displayLocale, sentenceCase);
    }
    TabooUtils* tabooUtils = TabooUtils::GetInstance();
    if (tabooUtils != nullptr) {
        result = tabooUtils->ReplaceLanguageName(adjust, displayLocale, result);
    } else {
        HILOG_ERROR_I18N("LocaleConfig::GetDisplayLanguage: tabooUtils is nullptr.");
    }
    if (sentenceCase && !result.empty()) {
        char ch = static_cast<char>(toupper(result[0]));
        return PseudoLocalizationProcessor(result.replace(0, 1, 1, ch));
    }
    return PseudoLocalizationProcessor(result);
}

std::string LocaleConfig::ComputeLocale(const std::string &displayLocale)
{
    if (supportedDialectLocales.size() == 0) {
        xmlKeepBlanksDefault(0);
        xmlDocPtr doc = xmlParseFile(SUPPORT_LOCALES_PATH);
        if (!doc) {
            return "";
        }
        xmlNodePtr cur = xmlDocGetRootElement(doc);
        if (!cur || xmlStrcmp(cur->name, reinterpret_cast<const xmlChar *>(supportLocalesTag))) {
            xmlFreeDoc(doc);
            HILOG_INFO_I18N("can not parse language supported locale file");
            return "";
        }
        cur = cur->xmlChildrenNode;
        while (cur != nullptr) {
            xmlChar *content = xmlNodeGetContent(cur);
            if (content == nullptr) {
                HILOG_INFO_I18N("get xml node content failed");
                break;
            }
            std::map<std::string, std::string> localeInfoConfigs = {};
            LocaleInfo localeinfo(reinterpret_cast<const char*>(content), localeInfoConfigs);
            std::string maximizeLocale = localeinfo.Maximize();
            const char* key = maximizeLocale.c_str();
            const char* value = reinterpret_cast<const char*>(content);
            SetSupportedDialectLocales(key, value);
            xmlFree(content);
            cur = cur->next;
        }
        xmlFreeDoc(doc);
    }
    std::map<std::string, std::string> configs = {};
    LocaleInfo localeinfo(displayLocale, configs);
    std::string maximizeLocale = localeinfo.Maximize();
    if (supportedDialectLocales.find(maximizeLocale) != supportedDialectLocales.end()) {
        return supportedDialectLocales.at(maximizeLocale);
    }
    return "";
}

void LocaleConfig::SetSupportedDialectLocales(const char* key, const char* value)
{
    std::lock_guard<std::mutex> dialectLocaleLock(dialectLocaleMutex);
    supportedDialectLocales.insert(
        std::make_pair<std::string, std::string>(key, value));
}

void LocaleConfig::ReadLangData(const char *langDataPath)
{
    xmlKeepBlanksDefault(0);
    if (langDataPath == nullptr) {
        return;
    }
    xmlDocPtr doc = xmlParseFile(langDataPath);
    if (!doc) {
        HILOG_INFO_I18N("can not open language data file");
        return;
    }
    xmlNodePtr cur = xmlDocGetRootElement(doc);
    if (!cur || xmlStrcmp(cur->name, reinterpret_cast<const xmlChar *>(rootTag))) {
        xmlFreeDoc(doc);
        HILOG_INFO_I18N("parse language data file failed");
        return;
    }
    cur = cur->xmlChildrenNode;
    while (cur != nullptr && !xmlStrcmp(cur->name, reinterpret_cast<const xmlChar *>(secondRootTag))) {
        xmlChar *langContents[ELEMENT_NUM] = { 0 }; // 2 represent langid, displayname;
        xmlNodePtr langValue = cur->xmlChildrenNode;
        bool xmlNodeNull = false;
        for (size_t i = 0; i < ELEMENT_NUM && langValue != nullptr; i++) {
            langContents[i] = xmlNodeGetContent(langValue);
            langValue = langValue->next;
            if (langContents[i] == nullptr) {
                xmlNodeNull = true;
            }
        }
        if (!xmlNodeNull) {
            // 0 represents langid index, 1 represents displayname index
            const char* key = reinterpret_cast<const char *>(langContents[0]);
            const char* value = reinterpret_cast<const char *>(langContents[1]);
            SetLocale2DisplayName(key, value);
        }
        for (size_t i = 0; i < ELEMENT_NUM; i++) {
            if (langContents[i] != nullptr) {
                xmlFree(langContents[i]);
            }
        }
        cur = cur->next;
    }
    xmlFreeDoc(doc);
}

void LocaleConfig::SetRegion2DisplayName(const char* key, const char* value)
{
    std::lock_guard<std::mutex> regionDisplayLock(region2DisplayNameMutex);
    region2DisplayName.insert(
        std::make_pair<std::string, std::string>(key, value));
}

void LocaleConfig::SetLocale2DisplayName(const char* key, const char* value)
{
    std::lock_guard<std::mutex> localeDisplayLock(locale2DisplayNameMutex);
    locale2DisplayName.insert(
        std::make_pair<std::string, std::string>(key, value));
}

void LocaleConfig::ReadRegionData(const char *regionDataPath)
{
    xmlKeepBlanksDefault(0);
    if (regionDataPath == nullptr) {
        return;
    }
    xmlDocPtr doc = xmlParseFile(regionDataPath);
    if (!doc) {
        HILOG_INFO_I18N("can not open region data file");
        return;
    }
    xmlNodePtr cur = xmlDocGetRootElement(doc);
    if (cur) {
        HILOG_INFO_I18N("cur pointer is true");
    }
    if (!cur || xmlStrcmp(cur->name, reinterpret_cast<const xmlChar *>(rootRegion))) {
        xmlFreeDoc(doc);
        HILOG_INFO_I18N("parse region data file failed");
        return;
    }
    cur = cur->xmlChildrenNode;
    while (cur != nullptr && !xmlStrcmp(cur->name, reinterpret_cast<const xmlChar *>(secondRootRegion))) {
        xmlChar *regionContents[ELEMENT_NUM] = { 0 };
        xmlNodePtr regionValue = cur->xmlChildrenNode;
        bool xmlNodeNull = false;
        for (size_t i = 0; i < ELEMENT_NUM && regionValue != nullptr; i++) {
            regionContents[i] = xmlNodeGetContent(regionValue);
            regionValue = regionValue->next;
            if (regionContents[i] == nullptr) {
                xmlNodeNull = true;
            }
        }
        if (!xmlNodeNull) {
            // 0 represents langid index, 1 represents displayname index
            const char* regionKey = reinterpret_cast<const char *>(regionContents[0]);
            const char* regionVal = reinterpret_cast<const char *>(regionContents[1]);
            SetRegion2DisplayName(regionKey, regionVal);
        }
        for (size_t i = 0; i < ELEMENT_NUM; i++) {
            if (regionContents[i] != nullptr) {
                xmlFree(regionContents[i]);
            }
        }
        cur = cur->next;
    }
    xmlFreeDoc(doc);
}

string LocaleConfig::GetDisplayLanguageWithDialect(const std::string &localeStr, const std::string &displayLocale)
{
    std::string finalLocale = ComputeLocale(displayLocale);
    if (finalLocale.empty()) {
        return "";
    }
    if (finalLocale.compare(currentDialectLocale) != 0) {
        std::string xmlPath = LANG_PATH + finalLocale + ".xml";
        locale2DisplayName.clear();
        ReadLangData(xmlPath.c_str());
        currentDialectLocale = finalLocale;
    }
    if (locale2DisplayName.find(localeStr) != locale2DisplayName.end()) {
        return locale2DisplayName.at(localeStr);
    }
    std::map<std::string, std::string> configs = {};
    LocaleInfo locale(localeStr, configs);
    std::string language = locale.GetLanguage();
    std::string scripts = locale.GetScript();
    std::string region = locale.GetRegion();
    if (scripts.length() != 0) {
        std::string languageAndScripts = language + "-" + scripts;
        if (locale2DisplayName.find(languageAndScripts) != locale2DisplayName.end()) {
            return locale2DisplayName.at(languageAndScripts);
        }
    }
    if (region.length() != 0) {
        std::string languageAndRegion = language + "-" + region;
        if (locale2DisplayName.find(languageAndRegion) != locale2DisplayName.end()) {
            return locale2DisplayName.at(languageAndRegion);
        }
    }
    if (locale2DisplayName.find(language) != locale2DisplayName.end()) {
        return locale2DisplayName.at(language);
    }
    return "";
}

string LocaleConfig::GetDisplayOverrideRegion(const std::string &region, const std::string &displayLocale)
{
    UErrorCode status = U_ZERO_ERROR;
    icu::Locale originLocale;
    icu::UnicodeString displayRegion;
    if (displayLocale.compare(currentOverrideRegion) != 0) {
        std::string xmlPath = REGION_PATH + displayLocale + ".xml";
        region2DisplayName.clear();
        ReadRegionData(xmlPath.c_str());
        currentOverrideRegion = displayLocale;
    }
    if (region2DisplayName.find(region) != region2DisplayName.end()) {
        return region2DisplayName.at(region);
    } else {
        icu::Locale locale = icu::Locale::forLanguageTag(displayLocale, status);
        if (U_FAILURE(status)) {
            return "";
        }
        if (IsValidRegion(region)) {
            icu::LocaleBuilder builder = icu::LocaleBuilder().setRegion(region);
            originLocale = builder.build(status);
        } else {
            originLocale = icu::Locale::forLanguageTag(region, status);
        }
        originLocale.getDisplayCountry(locale, displayRegion);
        std::string result;
        displayRegion.toUTF8String(result);
        return result;
    }
}

string LocaleConfig::GetDisplayRegion(const string &region, const string &displayLocale, bool sentenceCase)
{
    UErrorCode status = U_ZERO_ERROR;
    icu::Locale originLocale;
    if (IsValidRegion(region)) {
        icu::LocaleBuilder builder = icu::LocaleBuilder().setRegion(region);
        originLocale = builder.build(status);
    } else {
        originLocale = icu::Locale::forLanguageTag(region, status);
    }
    std::string country(originLocale.getCountry());
    if (country.length() == 0) {
        return PseudoLocalizationProcessor("");
    }
    if (U_FAILURE(status)) {
        return PseudoLocalizationProcessor("");
    }
    icu::Locale locale = icu::Locale::forLanguageTag(displayLocale, status);
    if (U_FAILURE(status)) {
        return PseudoLocalizationProcessor("");
    }
    icu::UnicodeString unistr;
    icu::UnicodeString displayRegion;
    std::string result;
    if (overrideSupportedRegions.find(displayLocale) != overrideSupportedRegions.end()) {
        result = GetDisplayOverrideRegion(region, displayLocale);
    } else {
        originLocale.getDisplayCountry(locale, displayRegion);
        displayRegion.toUTF8String(result);
    }
    TabooUtils* tabooUtils = TabooUtils::GetInstance();
    if (tabooUtils != nullptr) {
        result = tabooUtils->ReplaceCountryName(region, displayLocale, result);
    } else {
        HILOG_ERROR_I18N("LocaleConfig::GetDisplayRegion: tabooUtils is nullptr.");
    }
    if (sentenceCase) {
        char ch = static_cast<char>(toupper(result[0]));
        return PseudoLocalizationProcessor(result.replace(0, 1, 1, ch));
    }
    return PseudoLocalizationProcessor(result);
}

bool LocaleConfig::IsRTL(const string &locale)
{
    icu::Locale curLocale(locale.c_str());
    return curLocale.isRightToLeft();
}

void parseExtension(const std::string &extension, std::map<std::string, std::string> &map)
{
    std::string pattern = "-..-";
    std::regex express(pattern);

    std::regex_token_iterator<std::string::const_iterator> begin1(extension.cbegin(), extension.cend(), express);
    std::regex_token_iterator<std::string::const_iterator> begin2(extension.cbegin(), extension.cend(), express, -1);
    begin2++;
    for (; begin1 != std::sregex_token_iterator() && begin2 != std::sregex_token_iterator(); begin1++, begin2++) {
        map.insert(std::pair<std::string, std::string>(begin1->str(), begin2->str()));
    }
}

void setExtension(std::string &extension, const std::string &tag, const std::set<string> &validValue,
    const std::map<std::string, std::string> &extensionMap,
    const std::map<std::string, std::string> &defaultExtensionMap)
{
    std::string value;
    auto it = extensionMap.find(tag);
    if (it != extensionMap.end()) {
        value = it->second;
        if (validValue.find(value) == validValue.end()) {
            return;
        } else {
            extension += tag;
            extension += value;
        }
    } else {
        it = defaultExtensionMap.find(tag);
        if (it != defaultExtensionMap.end()) {
            value = it->second;
            if (validValue.find(value) == validValue.end()) {
                return;
            } else {
                extension += tag;
                extension += value;
            }
        }
    }
}

void setOtherExtension(std::string &extension, std::map<std::string, std::string> &extensionMap,
    std::map<std::string, std::string> &defaultExtensionMap)
{
    std::set<std::string> tags;
    tags.insert("-ca-");
    tags.insert("-co-");
    tags.insert("-kn-");
    tags.insert("-kf-");
    tags.insert("-nu-");
    tags.insert("-hc-");

    for (auto it = tags.begin(); it != tags.end(); it++) {
        extensionMap.erase(*it);
        defaultExtensionMap.erase(*it);
    }

    for (auto it = defaultExtensionMap.begin(); it != defaultExtensionMap.end(); it++) {
        extensionMap.insert(std::pair<std::string, std::string>(it->first, it->second));
    }

    for (auto it = extensionMap.begin(); it != extensionMap.end(); it++) {
        extension += it->first;
        extension += it->second;
    }
}

std::string LocaleConfig::GetValidLocale(const std::string &localeTag)
{
    std::string baseLocale = "";
    std::string extension = "";
    std::size_t found = localeTag.find(EXT_PARAM_KEY);
    baseLocale = localeTag.substr(0, found);
    if (found != std::string::npos) {
        extension = localeTag.substr(found);
    }
    std::map<std::string, std::string> extensionMap;
    if (extension != "") {
        parseExtension(extension, extensionMap);
    }

    std::string systemLocaleTag = GetSystemLocale();
    std::string defaultExtension = "";
    found = systemLocaleTag.find(EXT_PARAM_KEY);
    if (found != std::string::npos) {
        defaultExtension = systemLocaleTag.substr(found);
    }
    std::map<std::string, std::string> defaultExtensionMap;
    if (defaultExtension != "") {
        parseExtension(defaultExtension, defaultExtensionMap);
    }

    std::string ext = "";
    setExtension(ext, "-ca-", validCaTag, extensionMap, defaultExtensionMap);
    setExtension(ext, "-co-", validCoTag, extensionMap, defaultExtensionMap);
    setExtension(ext, "-kn-", validKnTag, extensionMap, defaultExtensionMap);
    setExtension(ext, "-kf-", validKfTag, extensionMap, defaultExtensionMap);
    setExtension(ext, "-nu-", validNuTag, extensionMap, defaultExtensionMap);
    setExtension(ext, "-hc-", validHcTag, extensionMap, defaultExtensionMap);

    std::string otherExt = "";
    setOtherExtension(otherExt, extensionMap, defaultExtensionMap);
    if (ext != "" || otherExt != "") {
        return baseLocale + "-u" + ext + otherExt;
    } else {
        return baseLocale;
    }
}

bool LocaleConfig::IsEmpty24HourClock()
{
    std::string is24Hour = ReadSystemParameter(HOUR_KEY.c_str(), CONFIG_LEN);
    if (is24Hour.empty()) {
        HILOG_ERROR_I18N("LocaleConfig::IsEmpty24HourClock: Read system is24Hour failed.");
        return false;
    }
    if (is24Hour.compare("default") == 0) {
        return true;
    }
    return false;
}

bool LocaleConfig::Is24HourClock()
{
    std::string is24Hour = ReadSystemParameter(HOUR_KEY.c_str(), CONFIG_LEN);
    if (is24Hour.empty()) {
        HILOG_ERROR_I18N("LocaleConfig::Is24HourClock: Read system is24Hour failed.");
        return false;
    }
    if (is24Hour.compare("default") == 0) {
        std::string systemLocale = GetSystemLocale();
        return Is24HourLocale(systemLocale);
    }
    if (is24Hour.compare("true") == 0) {
        return true;
    }
    return false;
}

std::string LocaleConfig::GetSystemHour()
{
    std::string is24HourVal = ReadSystemParameter(HOUR_KEY.c_str(), CONFIG_LEN);
    HILOG_INFO_I18N("GetSystemHour: read from system param:%{public}s.", is24HourVal.c_str());
    bool is24Hour = Is24HourClock();
    return is24Hour ? "true" : "false";
}

bool LocaleConfig::Is24HourLocale(const std::string& systemLocale)
{
    {
        std::shared_lock<std::shared_mutex> lock(is24HourLocaleMapMutex);
        if (is24HourLocaleMap.find(systemLocale) != is24HourLocaleMap.end()) {
            return is24HourLocaleMap[systemLocale];
        }
    }
    std::unique_lock<std::shared_mutex> lock(is24HourLocaleMapMutex);
    if (is24HourLocaleMap.find(systemLocale) != is24HourLocaleMap.end()) {
        return is24HourLocaleMap[systemLocale];
    }
    UErrorCode status = U_ZERO_ERROR;
    icu::Locale locale = icu::Locale::forLanguageTag(icu::StringPiece(systemLocale), status);
    if (U_FAILURE(status)) {
        HILOG_INFO_I18N("Is24HourLocale: %{public}s create locale failed", systemLocale.c_str());
        return false;
    }

    icu::UnicodeString formatPattern;
    icu::DateFormat* dateFormat = icu::DateFormat::createTimeInstance(icu::DateFormat::EStyle::kLong, locale);
    if (dateFormat == nullptr) {
        HILOG_INFO_I18N("Is24HourLocale: createTimeInstance failed");
        return false;
    }
    icu::SimpleDateFormat* simDateFormat = static_cast<icu::SimpleDateFormat*>(dateFormat);
    if (simDateFormat == nullptr) {
        HILOG_INFO_I18N("Is24HourLocale: failed to convert dateFormat");
        return false;
    }
    simDateFormat->toPattern(formatPattern);
    delete dateFormat;
    std::string pattern;
    formatPattern.toUTF8String(pattern);
    bool result = HasDesignator(pattern, 'H');
    is24HourLocaleMap[systemLocale] = result;
    return result;
}

bool LocaleConfig::HasDesignator(const std::string& pattern, const char designator)
{
    if (pattern.empty()) {
        HILOG_INFO_I18N("HasDesignator: pattern is empty");
        return false;
    }
    bool insideQuote = false;
    for (const auto& c : pattern) {
        if (c == '\'') {
            insideQuote = !insideQuote;
        } else if (!insideQuote) {
            if (c == designator) {
                return true;
            }
        }
    }
    return false;
}

bool LocaleConfig::GetUsingLocalDigit()
{
    std::string locale = GetSystemLocale();
    LocaleInfo localeInfo(locale);
    std::string language = localeInfo.GetLanguage();
    if (localDigitMap.find(language) == localDigitMap.end()) {
        return false;
    }
    std::string localNumberSystem = localDigitMap.at(language);
    return localNumberSystem.compare(localeInfo.GetNumberingSystem()) == 0;
}

std::unordered_set<std::string> LocaleConfig::GetBlockedLanguages()
{
    TabooUtils* tabooUtils = TabooUtils::GetInstance();
    if (tabooUtils == nullptr) {
        HILOG_ERROR_I18N("LocaleConfig::GetBlockedLanguages: tabooUtils is nullptr.");
        return {};
    }
    return tabooUtils->GetBlockedLanguages();
}

std::unordered_set<std::string> LocaleConfig::GetBlockedRegions(const std::string& language)
{
    TabooUtils* tabooUtils = TabooUtils::GetInstance();
    if (tabooUtils == nullptr) {
        HILOG_ERROR_I18N("LocaleConfig::GetBlockedRegions: tabooUtils is nullptr.");
        return {};
    }
    return tabooUtils->GetBlockedRegions(language);
}

I18nErrorCode LocaleConfig::SetSystemLanguage(const std::string &languageTag, int32_t userId)
{
    if (!IsValidTag(languageTag)) {
        HILOG_ERROR_I18N("LocaleConfig::SetSystemLanguage %{public}s is not valid language tag.",
            languageTag.c_str());
        return I18nErrorCode::INVALID_LANGUAGE_TAG;
    }
    std::string languageCode = LocaleEncode(languageTag);
    HILOG_INFO_I18N("LocaleConfig::SetSystemLanguage: set language %{public}s.", languageCode.c_str());
    // save old language, reset system language to old language if update locale failed.
    std::string oldLanguageTag = GetSystemLanguage();
    if (SetParameter(LANGUAGE_KEY.c_str(), languageTag.data()) != 0) {
        HILOG_ERROR_I18N("LocaleConfig::SetSystemLanguage update system language failed.");
        return I18nErrorCode::UPDATE_SYSTEM_LANGUAGE_FAILED;
    }
    std::string newLocaleTag = UpdateLanguageOfLocale(languageTag);
    if (SetSystemLocale(newLocaleTag, userId) == I18nErrorCode::SUCCESS) {
#ifdef SUPPORT_MULTI_USER
        std::string localId = (userId != -1) ? std::to_string(userId) : "";
        MultiUsers::SaveLanguage(localId, languageTag);
#endif
        return I18nErrorCode::SUCCESS;
    }
    // reset system language to old language in case that system language is inconsist with system locale's lanuage.
    HILOG_ERROR_I18N("LocaleConfig::SetSystemLanguage update system locale failed.");
    SetParameter(LANGUAGE_KEY.c_str(), oldLanguageTag.data());
    return I18nErrorCode::UPDATE_SYSTEM_LANGUAGE_FAILED;
}

I18nErrorCode LocaleConfig::SetSystemRegion(const std::string &regionTag, int32_t userId)
{
    QueryUpgradeLocale();
    if (!IsValidRegion(regionTag)) {
        HILOG_ERROR_I18N("LocaleConfig::SetSystemRegion %{public}s is not valid region tag.", regionTag.c_str());
        return I18nErrorCode::INVALID_REGION_TAG;
    }
    std::string regionCode = LocaleEncode(regionTag);
    HILOG_INFO_I18N("LocaleConfig::SetSystemRegion: set region %{public}s.", regionCode.c_str());
    return SetSystemLocale(UpdateRegionOfLocale(regionTag), userId);
}

void LocaleConfig::QueryUpgradeLocale()
{
    std::string upgradeLocale = ReadSystemParameter(UPGRADE_LOCALE_KEY, CONFIG_LEN);
    if (!upgradeLocale.empty()) {
        HILOG_INFO_I18N("LocaleConfig::QueryUpgradeLocale: upgrade locale is %{public}s.", upgradeLocale.c_str());
    }
}

std::string LocaleConfig::ComputeEffectiveLanguage(const std::string& locale, const std::string& language)
{
    if (locale.empty()) {
        HILOG_ERROR_I18N("LocaleConfig::ComputeEffectiveLanguage: locale is empty.");
        return locale;
    }
    size_t prefixLength = 2;
    if (locale.compare(0, prefixLength, "es") != 0 && locale.compare(0, prefixLength, "pt") != 0) {
        return locale;
    }
    std::shared_ptr<LocaleInfo> esESLocale = std::make_shared<LocaleInfo>("es-ES");
    std::shared_ptr<LocaleInfo> esUSLocale = std::make_shared<LocaleInfo>("es-US");
    std::shared_ptr<LocaleInfo> ptPTLocale = std::make_shared<LocaleInfo>("pt-PT");
    std::shared_ptr<LocaleInfo> ptBRLocale = std::make_shared<LocaleInfo>("pt-BR");
    std::vector<LocaleInfo*> candidateLocales {
        esESLocale.get(),
        esUSLocale.get(),
        ptPTLocale.get(),
        ptBRLocale.get()
    };
    std::shared_ptr<LocaleInfo> requestLocale = std::make_shared<LocaleInfo>(locale);
    std::string bestMatchedLocale = LocaleMatcher::GetBestMatchedLocale(requestLocale.get(), candidateLocales);
    if (bestMatchedLocale.empty()) {
        HILOG_ERROR_I18N("LocaleConfig::ComputeEffectiveLanguage: Get best matched locale failed.");
        return locale;
    }
    if (language.empty()) {
        HILOG_ERROR_I18N("LocaleConfig::ComputeEffectiveLanguage: language is empty.");
        return locale;
    }
    std::string systemLanguage = language;
    if (language.compare("es") == 0) {
        systemLanguage = "es-ES";
    }
    if (bestMatchedLocale.compare(systemLanguage) == 0) {
        return locale;
    }
    return systemLanguage;
}

I18nErrorCode LocaleConfig::SetSystemLocale(const std::string &localeTag, int32_t userId)
{
    if (!IsValidTag(localeTag)) {
        HILOG_ERROR_I18N("LocaleConfig::SetSystemLocale %{public}s is not a valid locale tag.", localeTag.c_str());
        return I18nErrorCode::INVALID_LOCALE_TAG;
    }
    std::string localeCode = LocaleEncode(localeTag);
    HILOG_INFO_I18N("LocaleConfig::SetSystemLocale: set locale %{public}s.", localeCode.c_str());
    if (SetParameter(LOCALE_KEY.c_str(), localeTag.data()) != 0) {
        return I18nErrorCode::UPDATE_SYSTEM_LOCALE_FAILED;
    }
    std::string systemLanguage = GetSystemLanguage();
    std::string effectiveLanguage = ComputeEffectiveLanguage(localeTag, systemLanguage);
#ifdef SUPPORT_MULTI_USER
    std::string localId = (userId != -1) ? std::to_string(userId) : "";
    MultiUsers::SaveLocale(localId, localeTag);
#endif
#ifdef SUPPORT_GRAPHICS
    UpdateConfiguration(AAFwk::GlobalConfigurationKey::SYSTEM_LANGUAGE, effectiveLanguage, userId);
    return PublishCommonEvent(EventFwk::CommonEventSupport::COMMON_EVENT_LOCALE_CHANGED, userId);
#else
    return I18nErrorCode::SUCCESS;
#endif
}

bool LocaleConfig::IsValid24HourClockValue(const std::string &tag)
{
    return (tag.compare("true") == 0 || tag.compare("false") == 0 || tag.compare("default") == 0);
}

I18nErrorCode LocaleConfig::Set24HourClock(const std::string &option, int32_t userId)
{
    if (!IsValid24HourClockValue(option)) {
        HILOG_ERROR_I18N("LocaleConfig::Set24HourClock invalid 24 Hour clock tag: %{public}s", option.c_str());
        return I18nErrorCode::INVALID_24_HOUR_CLOCK_TAG;
    }
    HILOG_INFO_I18N("LocaleConfig::Set24HourClock: update 24 hour clock %{public}s", option.c_str());
    if (SetParameter(HOUR_KEY.c_str(), option.data()) != 0) {
        HILOG_ERROR_I18N("LocaleConfig::Set24HourClock update 24 hour clock failed with option=%{public}s",
            option.c_str());
        return I18nErrorCode::UPDATE_24_HOUR_CLOCK_FAILED;
    }
#ifdef SUPPORT_MULTI_USER
    std::string localId;
    if (userId != -1) {
        localId = std::to_string(userId);
    }
    MultiUsers::SaveIs24Hour(localId, option);
#endif
#ifdef SUPPORT_GRAPHICS
    UpdateConfiguration(AAFwk::GlobalConfigurationKey::SYSTEM_HOUR, option, userId);
    return PublishCommonEvent(EventFwk::CommonEventSupport::COMMON_EVENT_TIME_CHANGED, userId);
#else
    return I18nErrorCode::SUCCESS;
#endif
}

I18nErrorCode LocaleConfig::SetUsingLocalDigit(bool flag, int32_t userId)
{
    // check whether current language support local digit.
    std::string localeTag = GetSystemLocale();
    std::string languageTag = localeTag.substr(0, 2); // obtain 2 length language code.
    auto it = localDigitMap.find(languageTag);
    if (it == localDigitMap.end()) {
        HILOG_ERROR_I18N("LocaleConfig::SetUsingLocalDigit current system doesn't support set local digit");
        return I18nErrorCode::UPDATE_LOCAL_DIGIT_FAILED;
    }
    std::string localDigit = flag ? it->second : "";
    // update system locale.
    return SetSystemLocale(ModifyExtParam(localeTag, NUMBER_SYSTEM_KEY, localDigit), userId);
}

I18nErrorCode LocaleConfig::SetTemperatureType(const TemperatureType& type, int32_t userId)
{
    if (temperatureTypeToName.find(type) == temperatureTypeToName.end()) {
        HILOG_ERROR_I18N("LocaleConfig::SetTemperatureType: temperature type failed.");
        return I18nErrorCode::INVALID_TEMPERATURE_TYPE;
    }
    std::string option = temperatureTypeToName[type];
    std::string localeTag = GetSystemLocale();
    if (localeTag.empty()) {
        HILOG_ERROR_I18N("LocaleConfig::SetTemperatureType: get system locale failed.");
        return I18nErrorCode::UPDATE_TEMPERATURE_TYPE_FAILED;
    }
    // update system locale.
    return SetSystemLocale(ModifyExtParam(localeTag, TEMPERATURE_UNIT_KEY, option), userId);
}

TemperatureType LocaleConfig::GetTemperatureType()
{
    std::string localeTag = GetSystemLocale();
    if (localeTag.empty()) {
        return TemperatureType::CELSIUS;
    }
    std::unordered_map<std::string, std::string> extParamMap = ParseExtParam(localeTag);
    std::string type;
    if (extParamMap.find(TEMPERATURE_UNIT_KEY) != extParamMap.end()) {
        type = extParamMap[TEMPERATURE_UNIT_KEY];
    }
    if (!type.empty() && nameToTemperatureType.find(type) != nameToTemperatureType.end()) {
        return nameToTemperatureType[type];
    }
    std::string regionTag = GetSystemRegion();
    if (fahrenheitUsingRegions.find(regionTag) != fahrenheitUsingRegions.end()) {
        return TemperatureType::FAHRENHEIT;
    }
    return TemperatureType::CELSIUS;
}

std::string LocaleConfig::GetTemperatureName(const TemperatureType& type)
{
    if (temperatureTypeToName.find(type) == temperatureTypeToName.end()) {
        HILOG_ERROR_I18N("LocaleConfig::GetTemperatureName: temperature type failed.");
        return "";
    }
    if (type == TemperatureType::FAHRENHEIT) {
        return "fahrenheit";
    }
    return temperatureTypeToName[type];
}

I18nErrorCode LocaleConfig::SetFirstDayOfWeek(const WeekDay& type, int32_t userId)
{
    if (weekDayToName.find(type) == weekDayToName.end()) {
        HILOG_ERROR_I18N("LocaleConfig::SetFirstDayOfWeek: week day failed.");
        return I18nErrorCode::INVALID_WEEK_DAY;
    }
    std::string option = weekDayToName[type];
    std::string localeTag = GetSystemLocale();
    if (localeTag.empty()) {
        HILOG_ERROR_I18N("LocaleConfig::SetFirstDayOfWeek: get system locale failed.");
        return I18nErrorCode::UPDATE_WEEK_DAY_FAILED;
    }
    // update system locale.
    return SetSystemLocale(ModifyExtParam(localeTag, WEEK_DAY_KEY, option), userId);
}

WeekDay LocaleConfig::GetFirstDayOfWeek()
{
    std::string localeTag = GetSystemLocale();
    if (localeTag.empty()) {
        HILOG_ERROR_I18N("LocaleConfig::GetFirstDayOfWeek: get system locale failed.");
        return WeekDay::MON;
    }
    std::unordered_map<std::string, std::string> extParamMap = ParseExtParam(localeTag);
    std::string type;
    if (extParamMap.find(WEEK_DAY_KEY) != extParamMap.end()) {
        type = extParamMap[WEEK_DAY_KEY];
    }
    if (!type.empty() && nameToWeekDay.find(type) != nameToWeekDay.end()) {
        return nameToWeekDay[type];
    }
    UErrorCode status = U_ZERO_ERROR;
    icu::Locale tempLocale = icu::Locale::forLanguageTag(localeTag, status);
    if (U_FAILURE(status)) {
        HILOG_ERROR_I18N("LocaleConfig::GetFirstDayOfWeek: create icu::Locale failed.");
        return WeekDay::MON;
    }
    icu::Calendar* calendar = icu::Calendar::createInstance(tempLocale, status);
    if (U_FAILURE(status)) {
        HILOG_ERROR_I18N("LocaleConfig::GetFirstDayOfWeek: create icu::Calendar failed.");
        return WeekDay::MON;
    }
    icu::Calendar::EDaysOfWeek firstDay = calendar->getFirstDayOfWeek();
    delete calendar;
    return eDaysOfWeekToWeekDay[firstDay];
}

#ifdef SUPPORT_GRAPHICS
void LocaleConfig::UpdateConfiguration(const char *key, const std::string &value, int32_t userId)
{
    AppExecFwk::Configuration configuration;
    configuration.AddItem(key, value);
    auto appMgrClient = std::make_unique<AppExecFwk::AppMgrClient>();

    AppExecFwk::AppMgrResultCode status = appMgrClient->UpdateConfiguration(configuration, userId);
    if (status != AppExecFwk::AppMgrResultCode::RESULT_OK) {
        HILOG_ERROR_I18N("LocaleConfig::UpdateConfiguration: Update configuration userId %{public}d failed.", userId);
        return;
    }
    HILOG_INFO_I18N("LocaleConfig::UpdateLanguageConfiguration: update userId %{public}d.", userId);
}

I18nErrorCode LocaleConfig::PublishCommonEvent(const std::string &eventType, int32_t userId)
{
    OHOS::AAFwk::Want localeChangeWant;
    localeChangeWant.SetAction(eventType);
    OHOS::EventFwk::CommonEventData event(localeChangeWant);
    if (EventFwk::CommonEventSupport::COMMON_EVENT_TIME_CHANGED.compare(eventType) == 0) {
        event.SetData(HOUR_EVENT_DATA);
    }
    if (userId == -1) {
        if (!OHOS::EventFwk::CommonEventManager::PublishCommonEvent(event)) {
            HILOG_ERROR_I18N("LocaleConfig::PublishCommonEvent Failed to Publish event %{public}s",
                localeChangeWant.GetAction().c_str());
            return I18nErrorCode::PUBLISH_COMMON_EVENT_FAILED;
        }
        HILOG_INFO_I18N("LocaleConfig::PublishCommonEvent publish event finished.");
        return I18nErrorCode::SUCCESS;
    }

    if (!OHOS::EventFwk::CommonEventManager::PublishCommonEventAsUser(event, userId)) {
        HILOG_ERROR_I18N("LocaleConfig::PublishCommonEvent Failed to Publish event %{public}s, userId %{public}d.",
            localeChangeWant.GetAction().c_str(), userId);
        return I18nErrorCode::PUBLISH_COMMON_EVENT_FAILED;
    }
    HILOG_INFO_I18N("LocaleConfig::PublishCommonEvent publish event finished,  userId %{public}d.", userId);
    return I18nErrorCode::SUCCESS;
}
#endif

std::string LocaleConfig::UpdateLanguageOfLocale(const std::string &languageTag)
{
    // Compute language and script part from languageTag.
    UErrorCode status = U_ZERO_ERROR;
    icu::Locale languageLocale = icu::Locale::forLanguageTag(languageTag.c_str(), status);
    if (U_FAILURE(status)) {
        HILOG_ERROR_I18N("LocaleConfig::UpdateLanguageOfLocale init icu Locale for language %{public}s failed.",
            languageTag.c_str());
        return "";
    }
    std::string langTag = languageLocale.getLanguage();
    std::string scriptTag = languageLocale.getScript();
    // Compute region and extend param part from current system locale.
    std::string systemLocaleTag = GetSystemLocale();
    icu::Locale systemLocale = icu::Locale::forLanguageTag(systemLocaleTag.c_str(), status);
    if (U_FAILURE(status)) {
        HILOG_ERROR_I18N("LocaleConfig::UpdateSystemLocale init icu Locale for locale %{public}s failed.",
            systemLocaleTag.c_str());
        return systemLocaleTag;
    }
    std::string regionTag = systemLocale.getCountry();
    std::string extendParamTag;
    size_t pos = systemLocaleTag.find(EXT_PARAM_KEY);
    if (pos != std::string::npos) {
        extendParamTag = systemLocaleTag.substr(pos);
    }
    // Combine above elements.
    return CreateLocale(langTag, scriptTag, regionTag, extendParamTag);
}

std::string LocaleConfig::CreateLocale(const std::string &languageTag, const std::string &scriptTag,
    const std::string &regionTag, const std::string &extendParamTag)
{
    // combine language, script, region and extend param with '-'
    std::string localeTag = languageTag;
    std::string splitor = "-";
    if (scriptTag.length() > 0) {
        localeTag += splitor + scriptTag;
    }
    if (regionTag.length() > 0) {
        localeTag += splitor + regionTag;
    }
    if (extendParamTag.length() > 0) {
        localeTag += extendParamTag;
    }
    return localeTag;
}

std::string LocaleConfig::UpdateRegionOfLocale(const std::string &regionTag)
{
    std::string localeTag = GetSystemLocale();
    // if current system locale is null, contruct a locale from region tag.
    if (localeTag.length() == 0) {
        return CreateLocaleFromRegion(regionTag);
    }
    std::string extParam;
    size_t pos = localeTag.find(EXT_PARAM_KEY);
    if (pos != std::string::npos) {
        extParam = localeTag.substr(pos);
    }
    // combine locale with origin locale's language and script with regionTag.
    UErrorCode status = U_ZERO_ERROR;
    const icu::Locale origin = icu::Locale::forLanguageTag(localeTag, status);
    if (U_FAILURE(status)) {
        HILOG_ERROR_I18N("LocaleConfig::UpdateRegionOfLocale init origin locale failed.");
        return localeTag;
    }
    icu::LocaleBuilder builder = icu::LocaleBuilder().setLanguage(origin.getLanguage()).
        setScript(origin.getScript()).setRegion(regionTag);
    icu::Locale temp = builder.build(status);
    if (U_FAILURE(status)) {
        HILOG_ERROR_I18N("LocaleConfig::UpdateRegionOfLocale: build locale failed.");
        return localeTag;
    }
    string ret = temp.toLanguageTag<string>(status);
    if (U_FAILURE(status)) {
        HILOG_ERROR_I18N("LocaleConfig::UpdateRegionOfLocale obtain new locale's tag failed.");
        return localeTag;
    }
    ret += extParam;
    ret = ModifyExtParam(ret, NUMBER_SYSTEM_KEY, "");
    return ret;
}

std::string LocaleConfig::CreateLocaleFromRegion(const std::string &regionTag)
{
    // fill locale with icu
    icu::Locale locale("", regionTag.c_str());
    UErrorCode status = U_ZERO_ERROR;
    locale.addLikelySubtags(status);
    if (U_FAILURE(status)) {
        HILOG_ERROR_I18N("LocaleConfig::CreateLocaleFromRegion init new locale failed.");
        return "";
    }
    std::string localeTag = locale.toLanguageTag<string>(status);
    if (U_FAILURE(status)) {
        HILOG_ERROR_I18N("LocaleConfig::CreateLocaleFromRegion obtain new locale's tag failed.");
        return "";
    }
    return localeTag;
}

std::string LocaleConfig::ModifyExtParam(const std::string& locale, const std::string& key, const std::string& value)
{
    if (locale.empty()) {
        HILOG_ERROR_I18N("LocaleConfig::ModifyExtParam: locale is empty.");
        return locale;
    }
    if (key.empty()) {
        HILOG_ERROR_I18N("LocaleConfig::ParseExtParam: key is empty.");
        return locale;
    }
    std::unordered_map<std::string, std::string> paramMap;
    std::string localeBase = locale;
    size_t pos = locale.find(EXT_PARAM_KEY);
    if (pos != std::string::npos) {
        localeBase = locale.substr(0, pos);
        std::string param = locale.substr(pos);
        paramMap = ParseExtParam(param);
    }
    paramMap[key] = value;
    std::string result = localeBase + GenerateExtParam(paramMap);
    return result;
}

std::unordered_map<std::string, std::string> LocaleConfig::ParseExtParam(const std::string& param)
{
    std::unordered_map<std::string, std::string> result;
    size_t pos = param.find(EXT_PARAM_KEY);
    if (pos == std::string::npos) {
        HILOG_INFO_I18N("LocaleConfig::ParseExtParam: param is invalid.");
        return result;
    }
    std::string extParam = param.substr(pos + EXT_PARAM_KEY.length());
    std::vector<std::string> paramArray;
    Split(extParam, "-", paramArray);
    size_t pairLen = 2;
    if (paramArray.size() % pairLen != 0) {
        HILOG_ERROR_I18N("LocaleConfig::ParseExtParam: parse ext param failed.");
        return result;
    }
    for (size_t pos = 0; pos + 1 < paramArray.size(); pos += pairLen) {
        std::string key = paramArray[pos];
        std::string value = paramArray[pos + 1];
        if (key.empty() || value.empty()) {
            HILOG_ERROR_I18N("LocaleConfig::ParseExtParam: key or value is empty.");
            continue;
        }
        auto status = result.insert({key, value});
        if (!status.second) {
            HILOG_ERROR_I18N("LocaleConfig::ParseExtParam: insert key (%{public}s) and value (%{public}s) failed.",
                key.c_str(), value.c_str());
        }
    }
    return result;
}

std::string LocaleConfig::GenerateExtParam(const std::unordered_map<std::string, std::string>& paramMap)
{
    std::string result;
    if (paramMap.size() == 0) {
        return result;
    }
    for (auto& param : paramMap) {
        std::string key = param.first;
        std::string value = param.second;
        if (value.empty()) {
            HILOG_INFO_I18N("LocaleConfig::GenerateExtParam: erase key (%{public}s) ", key.c_str());
            continue;
        }
        result += "-" + key + "-" + value;
    }
    if (!result.empty()) {
        result = "-u" + result;
    }
    return result;
}

std::string LocaleConfig::GetSimplifiedLanguage(const std::string& languageTag, int32_t &code)
{
    std::string simplifiedLanguage = "";
    if (IsValidTag(languageTag)) {
        LocaleInfo localeInfo(languageTag);
        simplifiedLanguage = localeInfo.Minimize();
    } else {
        code = 1;
    }
    return simplifiedLanguage;
}

std::string LocaleConfig::GetSimplifiedSystemLanguage()
{
    std::string locale = GetSystemLanguage();
    LocaleInfo localeInfo(locale);
    LocaleInfo localeInfoMax(localeInfo.Maximize());
    std::string language = localeInfoMax.GetLanguage();
    language = language + '-' + localeInfoMax.GetScript();
    if (dialectLanguages.find(language) != dialectLanguages.end()) {
        std::string systemRegion = localeInfo.GetRegion();
        if (!systemRegion.empty() && language.compare("en-Latn") != 0) {
            locale = language + '-' + systemRegion;
        } else {
            locale = GetSystemLocale();
        }
        LocaleInfo* requestLocale = new(std::nothrow) LocaleInfo(locale);
        if (requestLocale == nullptr) {
            HILOG_ERROR_I18N("GetSimplifiedLanguage: %{public}s failed to construct LocaleInfo.", locale.c_str());
            return "";
        }
        std::vector<LocaleInfo*> candidateLocales;
        for (auto& candidate : dialectLanguages[language]) {
            LocaleInfo* candidateInfo = new(std::nothrow) LocaleInfo(candidate);
            if (candidateInfo == nullptr) {
                HILOG_ERROR_I18N("GetSimplifiedLanguage: %{public}s failed to construct LocaleInfo.",
                    candidate.c_str());
                continue;
            }
            candidateLocales.push_back(candidateInfo);
        }
        locale = LocaleMatcher::GetBestMatchedLocale(requestLocale, candidateLocales);

        for (LocaleInfo* candidateInfo : candidateLocales) {
            delete candidateInfo;
            candidateInfo = nullptr;
        }
        delete requestLocale;
        requestLocale = nullptr;
    }
    LocaleInfo simplifiedLocale(locale);
    std::string ret = simplifiedLocale.Minimize();
    auto iter = resourceIdMap.find(ret);
    if (iter != resourceIdMap.end()) {
        ret = iter->second;
    }
    return ret;
}

std::string LocaleConfig::GetUnicodeWrappedFilePath(const std::string &path, const char delimiter,
    std::shared_ptr<LocaleInfo> localeInfo, std::string &invalidField)
{
    if (delimiter == '\0') {
        invalidField = "delimiter";
        return path;
    }
    if (path.empty() || path.length() > PATH_MAX) {
        invalidField = "path";
        return path;
    }
    std::string locale;
    if (localeInfo != nullptr) {
        locale = localeInfo->GetBaseName();
    }
    if (!IsValidTag(locale)) {
        locale = GetSystemLocale();
    }
    if (!IsRTL(locale)) {
        return path;
    }
    std::string result;
    std::vector<string> dest;
    std::string sep(1, delimiter); // 1 is char count
    Split(path, sep, dest);
    if (dest.size() == 1) { // 1 is array's size
        return path;
    }
    for (size_t i = 0 ; i < dest.size(); i++) {
        if (dest[i].empty()) {
            continue;
        }
        if (i == 0 && dest[i].compare(sep) != 0) {
            dest[i] = "\u200f\u200e" + dest[i] + "\u200e";
            continue;
        }
        dest[i] = "\u200e" + dest[i] + "\u200e";
    }
    std::string newSep = "\u200f" + sep;
    Merge(dest, newSep, result);
    return result;
}
} // namespace I18n
} // namespace Global
} // namespace OHOS
