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
#ifdef SUPPORT_APP_PREFERRED_LANGUAGE
#include <regex>
#include "application_context.h"
#include "bundle_info.h"
#include "bundle_mgr_interface.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"
#include "preferences_helper.h"
#endif
#include "i18n_hilog.h"
#include "locale_config.h"
#include "locale_info.h"
#include "locale_matcher.h"
#include "parameter.h"
#include "preferred_language.h"
#include "utils.h"
#include "vector"

namespace OHOS {
namespace Global {
namespace I18n {
const char *PreferredLanguage::RESOURCE_PATH_HEAD = "/data/accounts/account_0/applications/";
const char *PreferredLanguage::RESOURCE_PATH_TAILOR = "/assets/entry/resources.index";
const char *PreferredLanguage::RESOURCE_PATH_SPLITOR = "/";
const char *PreferredLanguage::PREFERRED_LANGUAGES = "persist.global.preferredLanguages";
const char *PreferredLanguage::APP_LANGUAGE_KEY = "app_language";
const char *PreferredLanguage::I18N_PREFERENCES_FILE_NAME = "/i18n";
const char *PreferredLanguage::DEFAULT_PREFERRED_LANGUAGE = "en-Latn-US";
std::unordered_set<std::string> PreferredLanguage::supportLanguageListExt = { "it", "ko", "th", "zz" };

std::vector<std::string> PreferredLanguage::GetPreferredLanguageList()
{
    char preferredLanguageValue[CONFIG_LEN];
    GetParameter(PREFERRED_LANGUAGES, "", preferredLanguageValue, CONFIG_LEN);
    std::string systemLanguage = GetMatchedLanguage(LocaleConfig::GetSystemLanguage());
    std::vector<std::string> list;
    Split(preferredLanguageValue, ";", list);
    list = FilterLanguages(list);
    if (!list.size()) {
        if (systemLanguage != "") {
            list.push_back(systemLanguage);
        }
        return list;
    }
    if (list[0] == systemLanguage || systemLanguage.empty()) {
        return list;
    }
    int systemLanguageIdx = -1;
    for (size_t i = 0; i < list.size(); i++) {
        if (list[i] == systemLanguage) {
            systemLanguageIdx = (int)i;
        }
    }
    if (systemLanguageIdx == -1) {
        list.insert(list.begin(), systemLanguage);
    } else {
        for (size_t i = (size_t)systemLanguageIdx; i > 0; i--) {
            list[i] = list[i - 1];
        }
        list[0] = systemLanguage;
    }
    return list;
}

std::vector<std::string> PreferredLanguage::FilterLanguages(std::vector<std::string>& preferredLanguagesList)
{
    std::vector<std::string> matchedLanguagesList;
    std::unordered_set<std::string> matchedSet;
    for (auto& preferredLanguage : preferredLanguagesList) {
        std::string matchedLanguage = GetMatchedLanguage(preferredLanguage);
        if (matchedLanguage.empty()) {
            HILOG_ERROR_I18N("FilterLanguages: the matching result of %{public}s is empty.",
                preferredLanguage.c_str());
            matchedLanguage = DEFAULT_PREFERRED_LANGUAGE;
        }
        if (matchedSet.find(matchedLanguage) == matchedSet.end()) {
            matchedLanguagesList.push_back(matchedLanguage);
            matchedSet.insert(matchedLanguage);
            HILOG_ERROR_I18N("FilterLanguages: the matching result is %{public}s.", matchedLanguage.c_str());
        }
    }
    return matchedLanguagesList;
}

std::string PreferredLanguage::GetMatchedLanguage(const std::string& language)
{
    UErrorCode status = U_ZERO_ERROR;
    icu::Locale locale = icu::Locale::forLanguageTag(language.c_str(), status);
    if (U_FAILURE(status) || !IsValidLocaleTag(locale)) {
        HILOG_ERROR_I18N("GetMatchedLanguage: %{public}s is an invalid locale.", language.c_str());
        return "";
    }
    LocaleInfo* requestLocale = new LocaleInfo(language);
    if (requestLocale == nullptr) {
        HILOG_ERROR_I18N("GetMatchedLanguage: %{public}s failed to construct LocaleInfo.", language.c_str());
        return "";
    }
    std::vector<LocaleInfo*> candidateLocales;
    std::unordered_set<std::string> supportLanguageList = LocaleConfig::GetSystemLanguages();
    supportLanguageList.insert(supportLanguageListExt.begin(), supportLanguageListExt.end());
    for (auto& supportLanguage : supportLanguageList) {
        LocaleInfo* supportLocaleInfo = new LocaleInfo(supportLanguage);
        if (supportLocaleInfo == nullptr) {
            HILOG_ERROR_I18N("GetMatchedLanguage: %{public}s failed to construct LocaleInfo.",
                supportLanguage.c_str());
            continue;
        }
        if (LocaleMatcher::Match(requestLocale, supportLocaleInfo)) {
            candidateLocales.push_back(supportLocaleInfo);
        } else {
            delete supportLocaleInfo;
        }
    }
    std::string matchedLanguage = LocaleMatcher::GetBestMatchedLocale(requestLocale, candidateLocales);
    for (LocaleInfo* supportLocaleInfo : candidateLocales) {
        delete supportLocaleInfo;
    }
    delete requestLocale;
    return matchedLanguage;
}

std::string PreferredLanguage::GetFirstPreferredLanguage()
{
    std::vector<std::string> preferredLanguageList = GetPreferredLanguageList();
    if (preferredLanguageList.empty()) {
        return "";
    }
    return preferredLanguageList[0];
}

#ifdef SUPPORT_APP_PREFERRED_LANGUAGE
std::shared_ptr<NativePreferences::Preferences> PreferredLanguage::GetI18nAppPreferences()
{
    std::shared_ptr<AbilityRuntime::ApplicationContext> appContext = AbilityRuntime::ApplicationContext::GetInstance();
    if (appContext == nullptr) {
        HILOG_ERROR_I18N("PreferredLanguage::GetAppPreferredLanguage: Get app context failed.");
        return nullptr;
    }
    int defaultMode = appContext->GetArea();
    appContext->SwitchArea(0);
    std::string preferencesDirPath = appContext->GetPreferencesDir();
    appContext->SwitchArea(defaultMode);
    std::string i18nPreferencesFilePath = preferencesDirPath + I18N_PREFERENCES_FILE_NAME;
    int status = 0;
    NativePreferences::Options options(i18nPreferencesFilePath);
    std::shared_ptr<NativePreferences::Preferences> preferences =
        NativePreferences::PreferencesHelper::GetPreferences(options, status);
    if (status != 0) {
        HILOG_ERROR_I18N("PreferredLanguage::GetAppPreferredLanguage get i18n app preferences failed.");
        return nullptr;
    }
    return preferences;
}

bool PreferredLanguage::IsSetAppPreferredLanguage()
{
    std::shared_ptr<NativePreferences::Preferences> preferences = GetI18nAppPreferences();
    if (preferences == nullptr) {
        HILOG_ERROR_I18N(
            "PreferredLanguage::IsSetAppPreferredLanguage get i18n preferences failed, return system language.");
        return false;
    }
    std::string res = preferences->GetString(PreferredLanguage::APP_LANGUAGE_KEY, "");
    if (res.length() == 0 || res.compare("default") == 0) {
        return false;
    }
    return true;
}

std::string PreferredLanguage::GetAppPreferredLanguage()
{
    std::shared_ptr<NativePreferences::Preferences> preferences = GetI18nAppPreferences();
    if (preferences == nullptr) {
        HILOG_ERROR_I18N(
            "PreferredLanguage::GetAppPreferredLanguage get i18n preferences failed, return system language.");
        return LocaleConfig::GetEffectiveLanguage();
    }
    std::string res = preferences->GetString(PreferredLanguage::APP_LANGUAGE_KEY, "");
    if (res.length() == 0 || res.compare("default") == 0) {
        return LocaleConfig::GetEffectiveLanguage();
    }
    return res;
}

void PreferredLanguage::SetAppPreferredLanguage(const std::string &language, I18nErrorCode &errCode)
{
    std::shared_ptr<AbilityRuntime::ApplicationContext> appContext = AbilityRuntime::ApplicationContext::GetInstance();
    if (appContext != nullptr && language.compare("default") != 0) {
        appContext->SetLanguage(language);
    }
    std::shared_ptr<NativePreferences::Preferences> preferences = GetI18nAppPreferences();
    if (preferences == nullptr) {
        errCode = I18nErrorCode::FAILED;
        HILOG_ERROR_I18N("PreferredLanguage::SetAppPreferredLanguage get i18n preferences failed.");
        return;
    }
    int32_t status = preferences->PutString(PreferredLanguage::APP_LANGUAGE_KEY, language);
    if (status != 0) {
        errCode = I18nErrorCode::FAILED;
        HILOG_ERROR_I18N(
            "PreferredLanguage::SetAppPreferredLanguage set app language to i18n preferences failed.");
        return;
    }
    preferences->Flush();
}
#endif

std::string PreferredLanguage::GetPreferredLocale()
{
    std::string systemLocale = LocaleConfig::GetSystemLocale();
    LocaleInfo systemLocaleInfo(systemLocale);
    std::string systemRegion = systemLocaleInfo.GetRegion();
    std::string preferredLanguageLocale = GetFirstPreferredLanguage();
    LocaleInfo preferredLanguageLocaleInfo(preferredLanguageLocale);
    std::string preferredLanguage = preferredLanguageLocaleInfo.GetLanguage();
    std::string preferredLocale = preferredLanguage + "-" + systemRegion;
    return preferredLocale;
}

bool PreferredLanguage::IsValidLanguage(const std::string &language)
{
    std::string::size_type size = language.size();
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

bool PreferredLanguage::IsValidTag(const std::string &tag)
{
    if (!tag.size()) {
        return false;
    }
    std::vector<std::string> splits;
    Split(tag, "-", splits);
    if (!IsValidLanguage(splits[0])) {
        return false;
    }
    return true;
}

void PreferredLanguage::Split(const std::string &src, const std::string &sep, std::vector<std::string> &dest)
{
    std::string::size_type begin = 0;
    std::string::size_type end = src.find(sep);
    while (end != std::string::npos) {
        dest.push_back(src.substr(begin, end - begin));
        begin = end + sep.size();
        end = src.find(sep, begin);
    }
    if (begin != src.size()) {
        dest.push_back(src.substr(begin));
    }
}

I18nErrorCode PreferredLanguage::AddPreferredLanguage(const std::string &language, int32_t index)
{
    if (!IsValidTag(language)) {
        HILOG_ERROR_I18N("PreferredLanguage::AddPreferredLanguage %{public}s is not valid language tag.",
            language.c_str());
        return I18nErrorCode::INVALID_LANGUAGE_TAG;
    }
    std::vector<std::string> preferredLanguages;
    I18nErrorCode status = I18nErrorCode::SUCCESS;
    if (FindLanguage(language) == -1) {
        // Case: language not in current preferred language list.
        AddNonExistPreferredLanguage(language, index, preferredLanguages, status);
    } else {
        // Case: language in current preferred language list.
        AddExistPreferredLanguage(language, index, preferredLanguages, status);
    }
    if (status != I18nErrorCode::SUCCESS) {
        HILOG_ERROR_I18N("PreferredLanguage::AddPreferredLanguage failed.");
        return status;
    }
    return SetPreferredLanguages(JoinPreferredLanguages(preferredLanguages));
}

I18nErrorCode PreferredLanguage::RemovePreferredLanguage(int32_t index)
{
    std::vector<std::string> preferredLanguages = GetPreferredLanguageList();
    if (preferredLanguages.size() == 1) {
        HILOG_ERROR_I18N("PreferredLanguage::RemovePreferredLanguage can't remove the only language.");
        return I18nErrorCode::REMOVE_PREFERRED_LANGUAGE_FAILED;
    }
    // valid index is [0, preferredLanguages.size() - 1] for Remove
    int32_t validIndex = NormalizeIndex(index, preferredLanguages.size() - 1);
    preferredLanguages.erase(preferredLanguages.begin() + validIndex);
    // The first language in preferred language list is system language, therefor when first language changed
    // in preferred language list, we need to reset system language.
    if (validIndex == 0) {
        if (LocaleConfig::SetSystemLanguage(preferredLanguages[0]) != I18nErrorCode::SUCCESS) {
            HILOG_ERROR_I18N("PreferredLanguage::RemovePreferredLanguage update system language failed.");
            return I18nErrorCode::REMOVE_PREFERRED_LANGUAGE_FAILED;
        }
    }
    return SetPreferredLanguages(JoinPreferredLanguages(preferredLanguages));
}

void PreferredLanguage::AddNonExistPreferredLanguage(const std::string& language, int32_t index,
    std::vector<std::string> &preferredLanguages, I18nErrorCode &errCode)
{
    // valid index is [0, GetPreferredLanguageList().size()] for add non-exist language.
    int32_t validIndex = NormalizeIndex(index, GetPreferredLanguageList().size());
    preferredLanguages = GetPreferredLanguageList();
    preferredLanguages.insert(preferredLanguages.begin() + validIndex, language);
    // The first language in preferred language list is system language, therefor when first language changed
    // in preferred language list, we need to reset system language.
    if (validIndex == 0) {
        if (LocaleConfig::SetSystemLanguage(preferredLanguages[0]) != I18nErrorCode::SUCCESS) {
            HILOG_ERROR_I18N("PreferredLanguage::AddNonExistPreferredLanguage update system language failed.");
            errCode = I18nErrorCode::ADD_PREFERRED_LANGUAGE_NON_EXIST_FAILED;
            return;
        }
    }
    errCode = I18nErrorCode::SUCCESS;
}

void PreferredLanguage::AddExistPreferredLanguage(const std::string& language, int32_t index,
    std::vector<std::string> &preferredLanguages, I18nErrorCode &errCode)
{
    // throw error when current index is same with target index.
    // valid index is [0, GetPreferredLanguageList().size() - 1] for add exist language.
    int32_t validIndex = NormalizeIndex(index, GetPreferredLanguageList().size() - 1);
    int32_t languageIdx = FindLanguage(language);
    if (languageIdx == validIndex) {
        errCode = I18nErrorCode::ADD_PREFERRED_LANGUAGE_EXIST_FAILED;
        return;
    }
    // Move language from languageIdx to validIdx.
    preferredLanguages = GetPreferredLanguageList();
    preferredLanguages.erase(preferredLanguages.begin() + languageIdx);
    preferredLanguages.insert(preferredLanguages.begin() + validIndex, language);
    // The first language in preferred language list is system language, therefor when first language changed
    // in preferred language list, we need to reset system language.
    if (languageIdx == 0 || validIndex == 0) {
        if (LocaleConfig::SetSystemLanguage(preferredLanguages[0]) != I18nErrorCode::SUCCESS) {
            HILOG_ERROR_I18N("PreferredLanguage::AddExistPreferredLanguage update system language failed.");
            errCode = I18nErrorCode::ADD_PREFERRED_LANGUAGE_EXIST_FAILED;
            return;
        }
    }
    errCode = I18nErrorCode::SUCCESS;
}

int32_t PreferredLanguage::NormalizeIndex(int32_t index, int32_t max)
{
    if (index <= 0) {
        return 0;
    }
    if (index >= max) {
        return max;
    }
    return index;
}

// Query the index of language in system preferred language list.
int32_t PreferredLanguage::FindLanguage(const std::string &language)
{
    std::vector<std::string> preferredLanguageList = GetPreferredLanguageList();
    for (size_t i = 0; i < preferredLanguageList.size(); ++i) {
        if (preferredLanguageList[i] == language) {
            return static_cast<int32_t>(i);
        }
    }
    return -1;
}

// Join preferred language tags to string with ';'
std::string PreferredLanguage::JoinPreferredLanguages(const std::vector<std::string> preferredLanguages)
{
    std::string result = "";
    for (size_t i = 0; i < preferredLanguages.size(); ++i) {
        result += preferredLanguages[i];
        result += ";";
    }
    // delete the last ';'
    result.pop_back();
    return result;
}

// Set PREFERRED_LANGUAGES system parameter with preferredLanguages.
I18nErrorCode PreferredLanguage::SetPreferredLanguages(const std::string &preferredLanguages)
{
    // System parameter value's length can't beyong CONFIG_LEN
    if (preferredLanguages.length() > CONFIG_LEN) {
        HILOG_ERROR_I18N("PreferredLanguage::SetPreferredLanguage preferred language list is too long.");
        return I18nErrorCode::UPDATE_SYSTEM_PREFERRED_LANGUAGE_FAILED;
    }
    if (SetParameter(PREFERRED_LANGUAGES, preferredLanguages.data()) != 0) {
        HILOG_ERROR_I18N("PreferredLanguage::AddPreferredLanguage udpate preferred language param failed.");
        return I18nErrorCode::UPDATE_SYSTEM_PREFERRED_LANGUAGE_FAILED;
    }
    return I18nErrorCode::SUCCESS;
}
} // namespace I18n
} // namespace Global
} // namespace OHOS