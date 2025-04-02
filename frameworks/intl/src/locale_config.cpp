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
#include <regex>
#include "accesstoken_kit.h"
#ifdef SUPPORT_GRAPHICS
#include "app_mgr_client.h"
#include "ability_manager_client.h"
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
const char *LocaleConfig::LANGUAGE_KEY = "persist.global.language";
const char *LocaleConfig::LOCALE_KEY = "persist.global.locale";
const char *LocaleConfig::HOUR_KEY = "persist.global.is24Hour";
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
const char *LocaleConfig::FORBIDDEN_LANGUAGES_NAME = "forbidden_languages";
const char *LocaleConfig::FORBIDDEN_REGIONS_NAME = "forbidden_regions";
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
const char *LocaleConfig::NUMBER_SYSTEM_KEY = "-nu-";
const std::string LocaleConfig::HOUR_EVENT_DATA = "24HourChange";
unordered_set<string> LocaleConfig::supportedLocales;
unordered_set<string> LocaleConfig::supportedLocalesV15;
unordered_set<string> LocaleConfig::supportLocales;
unordered_set<string> LocaleConfig::supportedRegions;
unordered_set<string> LocaleConfig::overrideSupportedRegions;
unordered_set<string> LocaleConfig::dialectLang;
unordered_set<string> LocaleConfig::blockedLanguages;
unordered_set<string> LocaleConfig::blockedRegions;
unordered_map<string, unordered_set<string>> LocaleConfig::blockedLanguageRegions;
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

std::map<std::string, std::string> LocaleConfig::supportedDialectLocales;
std::map<string, string> LocaleConfig::locale2DisplayName {};
std::map<string, string> LocaleConfig::region2DisplayName {};
std::string LocaleConfig::currentDialectLocale = "";
std::string LocaleConfig::currentOverrideRegion = "";
std::mutex LocaleConfig::dialectLocaleMutex;
std::mutex LocaleConfig::region2DisplayNameMutex;
std::mutex LocaleConfig::locale2DisplayNameMutex;

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
        if (status != U_ZERO_ERROR) {
            return "";
        }
        icu::LocaleDisplayNames *dspNames = icu::LocaleDisplayNames::createInstance(displayLocale,
            UDialectHandling::ULDN_DIALECT_NAMES);
        icu::Locale tempLocale = icu::Locale::forLanguageTag(language.c_str(), status);
        if (status != U_ZERO_ERROR) {
            return "";
        }
        if (dspNames != nullptr) {
            dspNames->localeDisplayName(tempLocale, unistr);
            delete dspNames;
        }
    } else {
        UErrorCode status = U_ZERO_ERROR;
        icu::Locale displayLoc = icu::Locale::forLanguageTag(displayLocaleTag, status);
        if (status != U_ZERO_ERROR) {
            return "";
        }
        icu::Locale locale = icu::Locale::forLanguageTag(language, status);
        if (status != U_ZERO_ERROR) {
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

string LocaleConfig::GetSystemLanguage()
{
    std::string systemLanguage = ReadSystemParameter(LANGUAGE_KEY, CONFIG_LEN);
    if (systemLanguage.empty()) {
        systemLanguage = ReadSystemParameter(DEFAULT_LANGUAGE_KEY, CONFIG_LEN);
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
        return "CN";
    }
    return systemRegion;
}

string LocaleConfig::GetCountry(const string& parameter)
{
    std::string systemRegion = ReadSystemParameter(parameter.c_str(), CONFIG_LEN);
    if (systemRegion.empty()) {
        HILOG_INFO_I18N("GetCountry: ReadSystemParameter %{public}s failed", parameter.c_str());
        return systemRegion;
    }
    UErrorCode status = U_ZERO_ERROR;
    icu::Locale origin = icu::Locale::forLanguageTag(systemRegion, status);
    if (U_SUCCESS(status)) {
        const char* country = origin.getCountry();
        if (country != nullptr) {
            return country;
        }
        HILOG_INFO_I18N("GetCountry: %{public}s get failed, country is nullptr", parameter.c_str());
    } else {
        HILOG_INFO_I18N("GetCountry: %{public}s get failed, U_SUCCESS is false", parameter.c_str());
    }
    return "";
}

string LocaleConfig::GetSystemLocale()
{
    std::string systemLocale = ReadSystemParameter(LOCALE_KEY, CONFIG_LEN);
    if (systemLocale.empty()) {
        systemLocale = ReadSystemParameter(DEFAULT_LOCALE_KEY, CONFIG_LEN);
    }
    return systemLocale;
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
void LocaleConfig::GetSystemLanguages(vector<string> &ret)
{
    std::copy(whiteLanguages.begin(), whiteLanguages.end(), std::back_inserter(ret));
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

void LocaleConfig::GetSystemCountries(vector<string> &ret)
{
    std::copy(supportedRegions.begin(), supportedRegions.end(), std::back_inserter(ret));
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

void LocaleConfig::ProcessForbiddenRegions(const unordered_set<string> &forbiddenRegions)
{
    for (auto it = forbiddenRegions.begin(); it != forbiddenRegions.end(); ++it) {
        size_t pos = it->rfind("-");
        std::string language = it->substr(0, pos);
        std::string region = it->substr(pos + 1);
        if (language.compare("*") == 0) {
            blockedRegions.insert(region);
        } else {
            if (blockedLanguageRegions.find(language) == blockedLanguageRegions.end()) {
                blockedLanguageRegions[language] = { region };
            } else {
                blockedLanguageRegions[language].insert(region);
            }
        }
    }
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
    unordered_set<string> forbiddenRegions;
    LoadRegionsLanguages(forbiddenRegions);
    ProcessForbiddenRegions(forbiddenRegions);
    Expunge(supportedRegions, blockedRegions);
    Expunge(whiteLanguages, blockedLanguages);
    GetListFromFile(SUPPORTED_LOCALES_PATH, SUPPORTED_LOCALES_NAME, supportedLocales);
    GetListFromFile(SUPPORTED_LOCALES_OLD_PATH, SUPPORTED_LOCALES_NAME, supportedLocalesV15);
    GetListFromFile(SUPPORT_LOCALES_PATH, SUPPORT_LOCALES_NAME, supportLocales);
    GetListFromFile(OVERRIDE_SUPPORTED_REGIONS_PATH, OVERRIDE_SUPPORTED_REGIONS_NAME, overrideSupportedRegions);
    GetListFromFile(DIALECT_LANGS_PATH, DIALECT_LANGS_NAME, dialectLang);
    ExtendWhiteLanguages();
    return true;
}

void LocaleConfig::LoadRegionsLanguages(unordered_set<std::string>& forbiddenRegions)
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
        } else if (content != nullptr && !xmlStrcmp(cur->name,
            reinterpret_cast<const xmlChar *>(FORBIDDEN_REGIONS_NAME))) {
            Split(reinterpret_cast<const char*>(content), ",", forbiddenRegions);
        } else if (content != nullptr && !xmlStrcmp(cur->name,
            reinterpret_cast<const xmlChar *>(FORBIDDEN_LANGUAGES_NAME))) {
            Split(reinterpret_cast<const char*>(content), ",", blockedLanguages);
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
    if (status != U_ZERO_ERROR) {
        return "";
    }
    origin.addLikelySubtags(status);
    if (status != U_ZERO_ERROR) {
        return "";
    }
    icu::LocaleBuilder builder = icu::LocaleBuilder().setLanguage(origin.getLanguage()).
        setScript(origin.getScript()).setRegion(origin.getCountry());
    icu::Locale temp = builder.setExtension('u', "").build(status);
    if (status != U_ZERO_ERROR) {
        return "";
    }
    string fullLanguage = temp.toLanguageTag<string>(status);
    if (status != U_ZERO_ERROR) {
        return "";
    }
    if (selfDialectMap.find(fullLanguage) != selfDialectMap.end()) {
        return selfDialectMap[fullLanguage];
    }
    builder.setRegion("");
    temp = builder.build(status);
    if (status != U_ZERO_ERROR) {
        return "";
    }
    fullLanguage = temp.toLanguageTag<string>(status);
    if (status != U_ZERO_ERROR) {
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
        if (status != U_ZERO_ERROR) {
            return PseudoLocalizationProcessor("");
        }
        icu::Locale locale = icu::Locale::forLanguageTag(language, status);
        if (status != U_ZERO_ERROR) {
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
        if (status != U_ZERO_ERROR) {
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
    if (status != U_ZERO_ERROR) {
        return PseudoLocalizationProcessor("");
    }
    icu::Locale locale = icu::Locale::forLanguageTag(displayLocale, status);
    if (status != U_ZERO_ERROR) {
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
    std::size_t found = localeTag.find("-u-");
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
    found = systemLocaleTag.find("-u-");
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
    std::string is24Hour = ReadSystemParameter(HOUR_KEY, CONFIG_LEN);
    return is24Hour.empty();
}

bool LocaleConfig::Is24HourClock()
{
    std::string is24Hour = ReadSystemParameter(HOUR_KEY, CONFIG_LEN);
    if (is24Hour.empty()) {
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
    std::string is24HourVal = ReadSystemParameter(HOUR_KEY, CONFIG_LEN);
    HILOG_INFO_I18N("GetSystemHour: read from system param:%{public}s.", is24HourVal.c_str());
    bool is24Hour = Is24HourClock();
    return is24Hour ? "true" : "false";
}

bool LocaleConfig::Is24HourLocale(const std::string& systemLocale)
{
    static std::unordered_map<std::string, bool> is24HourLocaleMap;
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
    if (localNumberSystem.compare(localeInfo.GetNumberingSystem()) != 0) {
        return false;
    }
    return true;
}

std::unordered_set<std::string> LocaleConfig::GetBlockedLanguages()
{
    return blockedLanguages;
}

std::unordered_set<std::string> LocaleConfig::GetBlockedRegions()
{
    return blockedRegions;
}

std::unordered_set<std::string> LocaleConfig::GetLanguageBlockedRegions()
{
    std::string systemLanguage = LocaleConfig::GetSystemLanguage();
    if (blockedLanguageRegions.find(systemLanguage) != blockedLanguageRegions.end()) {
        return blockedLanguageRegions[systemLanguage];
    }
    std::unordered_set<std::string> emptyResult;
    return emptyResult;
}

I18nErrorCode LocaleConfig::SetSystemLanguage(const std::string &languageTag)
{
    if (!IsValidTag(languageTag)) {
        HILOG_ERROR_I18N("LocaleConfig::SetSystemLanguage %{public}s is not valid language tag.",
            languageTag.c_str());
        return I18nErrorCode::INVALID_LANGUAGE_TAG;
    }
    // save old language, reset system language to old language if update locale failed.
    std::string oldLanguageTag = GetSystemLanguage();
    if (SetParameter(LANGUAGE_KEY, languageTag.data()) != 0) {
        HILOG_ERROR_I18N("LocaleConfig::SetSystemLanguage update system language failed.");
        return I18nErrorCode::UPDATE_SYSTEM_LANGUAGE_FAILED;
    }
    std::string newLocaleTag = UpdateLanguageOfLocale(languageTag);
    if (SetSystemLocale(newLocaleTag) == I18nErrorCode::SUCCESS) {
        return I18nErrorCode::SUCCESS;
    }
    // reset system language to old language in case that system language is inconsist with system locale's lanuage.
    HILOG_ERROR_I18N("LocaleConfig::SetSystemLanguage update system locale failed.");
    SetParameter(LANGUAGE_KEY, oldLanguageTag.data());
    return I18nErrorCode::UPDATE_SYSTEM_LANGUAGE_FAILED;
}

I18nErrorCode LocaleConfig::SetSystemRegion(const std::string &regionTag)
{
    QueryUpgradeLocale();
    if (!IsValidRegion(regionTag)) {
        HILOG_ERROR_I18N("LocaleConfig::SetSystemRegion %{public}s is not valid region tag.", regionTag.c_str());
        return I18nErrorCode::INVALID_REGION_TAG;
    }
    return SetSystemLocale(UpdateRegionOfLocale(regionTag));
}

void LocaleConfig::QueryUpgradeLocale()
{
    std::string upgradeLocale = ReadSystemParameter(UPGRADE_LOCALE_KEY, CONFIG_LEN);
    if (!upgradeLocale.empty()) {
        HILOG_ERROR_I18N("LocaleConfig::QueryUpgradeLocale: upgrade locale is %{public}s.", upgradeLocale.c_str());
    }
}

I18nErrorCode LocaleConfig::SetSystemLocale(const std::string &localeTag)
{
    if (!IsValidTag(localeTag)) {
        HILOG_ERROR_I18N("LocaleConfig::SetSystemLocale %{public}s is not a valid locale tag.", localeTag.c_str());
        return I18nErrorCode::INVALID_LOCALE_TAG;
    }
    if (SetParameter(LOCALE_KEY, localeTag.data()) != 0) {
        return I18nErrorCode::UPDATE_SYSTEM_LOCALE_FAILED;
    }
#ifdef SUPPORT_GRAPHICS
    UpdateConfiguration(AAFwk::GlobalConfigurationKey::SYSTEM_LANGUAGE, localeTag);
    return PublishCommonEvent(EventFwk::CommonEventSupport::COMMON_EVENT_LOCALE_CHANGED);
#else
    return I18nErrorCode::SUCCESS;
#endif
}

bool LocaleConfig::IsValid24HourClockValue(const std::string &tag)
{
    if (tag.compare("true") == 0 || tag.compare("false") == 0 || tag.length() == 0) {
        return true;
    }
    return false;
}

I18nErrorCode LocaleConfig::Set24HourClock(const std::string &option)
{
    if (!IsValid24HourClockValue(option)) {
        HILOG_ERROR_I18N("LocaleConfig::Set24HourClock invalid 24 Hour clock tag: %{public}s", option.c_str());
        return I18nErrorCode::INVALID_24_HOUR_CLOCK_TAG;
    }
    if (SetParameter(HOUR_KEY, option.data()) != 0) {
        HILOG_ERROR_I18N("LocaleConfig::Set24HourClock update 24 hour clock failed with option=%{public}s",
            option.c_str());
        return I18nErrorCode::UPDATE_24_HOUR_CLOCK_FAILED;
    }
#ifdef SUPPORT_GRAPHICS
    UpdateConfiguration(AAFwk::GlobalConfigurationKey::SYSTEM_HOUR, option);
    return PublishCommonEvent(EventFwk::CommonEventSupport::COMMON_EVENT_TIME_CHANGED);
#else
    return I18nErrorCode::SUCCESS;
#endif
}

I18nErrorCode LocaleConfig::SetUsingLocalDigit(bool flag)
{
    // check whether current language support local digit.
    std::string localeTag = GetSystemLocale();
    std::string languageTag = localeTag.substr(0, 2); // obtain 2 length language code.
    auto it = localDigitMap.find(languageTag);
    if (it == localDigitMap.end()) {
        HILOG_ERROR_I18N("LocaleConfig::SetUsingLocalDigit current system doesn't support set local digit");
        return I18nErrorCode::UPDATE_LOCAL_DIGIT_FAILED;
    }
    // update system locale.
    return SetSystemLocale(UpdateNumberSystemOfLocale(it->second, flag));
}

std::string LocaleConfig::UpdateNumberSystemOfLocale(const std::string &localDigitTag, bool flag)
{
    if (flag) {
        // add local digit tag to number system param of locale
        return AddLocalDigitToLocale(localDigitTag);
    }
    // remove local digit tag to number system param of locale
    return RemoveLocalDigitFromLocale(localDigitTag);
}

std::string LocaleConfig::AddLocalDigitToLocale(const std::string &localDigitTag)
{
    std::string localeTag = GetSystemLocale();
    // Case: no extend param, add '-u-' and number system tag.
    if (localeTag.find("-u-") == std::string::npos) {
        localeTag += "-u" + std::string(NUMBER_SYSTEM_KEY) + localDigitTag;
        return localeTag;
    }
    // Case: has extend param but doesn't hava number system param, add number system tag.
    if (localeTag.find(NUMBER_SYSTEM_KEY) == std::string::npos) {
        localeTag += std::string(NUMBER_SYSTEM_KEY) + localDigitTag;
        return localeTag;
    }
    // Case: has number system param, replace local digit tag to localDigitTag.
    LocaleInfo localeInfo(localeTag);
    std::string oldNumberSystem = localeInfo.GetNumberingSystem();
    localeTag.replace(localeTag.find(oldNumberSystem), oldNumberSystem.length(), localDigitTag);
    return localeTag;
}

std::string LocaleConfig::RemoveLocalDigitFromLocale(const std::string &localDigitTag)
{
    // remove number system tag from locale
    std::string localeTag = GetSystemLocale();
    std::string numberSystemTag = NUMBER_SYSTEM_KEY + localDigitTag;
    size_t pos = localeTag.find(numberSystemTag);
    if (pos != std::string::npos) {
        localeTag.replace(pos, numberSystemTag.length(), "");
    }
    // remove "-u" if localeTag ends with "-u"
    size_t uLength = 2;
    if (localeTag.find("-u") == (localeTag.length() - uLength)) {
        localeTag.resize(localeTag.length() - uLength);
    }
    return localeTag;
}

#ifdef SUPPORT_GRAPHICS
void LocaleConfig::UpdateConfiguration(const char *key, const std::string &value)
{
    AppExecFwk::Configuration configuration;
    configuration.AddItem(key, value);
    auto appMgrClient = std::make_unique<AppExecFwk::AppMgrClient>();
    appMgrClient->UpdateConfiguration(configuration);
    HILOG_INFO_I18N("LocaleConfig::UpdateLanguageConfiguration update configuration finished.");
}

I18nErrorCode LocaleConfig::PublishCommonEvent(const std::string &eventType)
{
    OHOS::AAFwk::Want localeChangeWant;
    localeChangeWant.SetAction(eventType);
    OHOS::EventFwk::CommonEventData event(localeChangeWant);
    if (EventFwk::CommonEventSupport::COMMON_EVENT_TIME_CHANGED.compare(eventType) == 0) {
        event.SetData(HOUR_EVENT_DATA);
    }
    if (!OHOS::EventFwk::CommonEventManager::PublishCommonEvent(event)) {
        HILOG_ERROR_I18N("LocaleConfig::PublishCommonEvent Failed to Publish event %{public}s",
            localeChangeWant.GetAction().c_str());
        return I18nErrorCode::PUBLISH_COMMON_EVENT_FAILED;
    }
    HILOG_INFO_I18N("LocaleConfig::PublishCommonEvent publish event finished.");
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
        return "";
    }
    std::string regionTag = systemLocale.getCountry();
    std::string extendParamTag;
    size_t pos = systemLocaleTag.find("-u-");
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
    // combine locale with origin locale's language and script with regionTag.
    UErrorCode status = U_ZERO_ERROR;
    const icu::Locale origin = icu::Locale::forLanguageTag(localeTag, status);
    if (U_FAILURE(status)) {
        HILOG_ERROR_I18N("LocaleConfig::UpdateRegionOfLocale init origin locale failed.");
        return "";
    }
    icu::LocaleBuilder builder = icu::LocaleBuilder().setLanguage(origin.getLanguage()).
        setScript(origin.getScript()).setRegion(regionTag);
    icu::Locale temp = builder.setExtension('u', "").build(status);
    string ret = temp.toLanguageTag<string>(status);
    if (U_FAILURE(status)) {
        HILOG_ERROR_I18N("LocaleConfig::UpdateRegionOfLocale obtain new locale's tag failed.");
        return "";
    }
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
            LocaleInfo* candidateInfo = new LocaleInfo(candidate);
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
} // namespace I18n
} // namespace Global
} // namespace OHOS
