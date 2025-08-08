/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <cstdint>
#include <cstdlib>
#include <string>
#include <vector>
#include "locale_config.h"
#include "locale_info.h"
#include "i18n_ffi.h"
#include "i18n_hilog.h"
#include "i18n_struct.h"
#include "i18n_system_ffi.h"
#include "preferred_language.h"
#include "utils.h"
 
namespace OHOS {
namespace Global {
namespace I18n {
using namespace OHOS::HiviewDFX;
extern "C"
{
    char* FfiI18nSystemGetAppPreferredLanguage()
    {
        #ifdef SUPPORT_APP_PREFERRED_LANGUAGE
            std::string language = PreferredLanguage::GetAppPreferredLanguage();
        #else
            std::string language = PreferredLanguage::GetFirstPreferredLanguage();
        #endif
        return MallocCString(language);
    }

    char* FfiI18nSystemGetSystemLanguage()
    {
        return MallocCString(LocaleConfig::GetSystemLanguage());
    }

    char* FfiI18nSystemGetSystemRegion()
    {
        return MallocCString(LocaleConfig::GetSystemRegion());
    }

    bool FfiI18nSystemIsSuggested(char* language, char* region, int32_t parameterStatus)
    {
        bool isSuggested = false;
        if (parameterStatus == 1) {
            isSuggested = LocaleConfig::IsSuggested(std::string(language), std::string(region));
        } else {
            isSuggested = LocaleConfig::IsSuggested(std::string(language));
        }
        return isSuggested;
    }

    CArrStr FfiI18nSystemGetSystemCountries(char* language)
    {
        std::unordered_set<std::string> systemCountries = LocaleConfig::GetSystemCountries(language);
        std::vector<std::string> result;
        result.assign(systemCountries.begin(), systemCountries.end());
        return VectorStringToCArr(result);
    }

    char* FfiI18nSystemGetDisplayCountry(char* country, char* locale, bool sentenceCase, int32_t* errcode)
    {
        std::string countryStr(country);
        std::string localeStr(locale);
        LocaleInfo localeInfo(countryStr);
        if (!LocaleConfig::IsValidRegion(countryStr) && localeInfo.GetRegion().empty()) {
            HILOG_ERROR_I18N("GetDisplayCountry: Failed to get display country, because param country is invalid!");
            *errcode = I18N_NOT_VALID;
            return nullptr;
        } else if (!LocaleConfig::IsValidTag(locale)) {
            HILOG_ERROR_I18N("GetDisplayCountry: Failed to get display country, because param locale is invalid!");
            *errcode = I18N_NOT_VALID;
            return nullptr;
        }
        std::string region = LocaleConfig::GetDisplayRegion(countryStr, localeStr, sentenceCase);
        return MallocCString(region);
    }

    bool FfiI18nSystemGetUsingLocalDigit()
    {
        return LocaleConfig::GetUsingLocalDigit();
    }

    int32_t FfiI18nSystemSetAppPreferredLanguage(char* language)
    {
        UErrorCode icuStatus = U_ZERO_ERROR;
        std::string localeTag(language);
        icu::Locale locale = icu::Locale::forLanguageTag(localeTag, icuStatus);
        if (U_FAILURE(icuStatus) || !(IsValidLocaleTag(locale) || localeTag.compare("default") == 0)) {
            HILOG_ERROR_I18N("SetAppPreferredLanguage does not support this locale");
            return I18N_NOT_VALID;
        }
        #ifdef SUPPORT_APP_PREFERRED_LANGUAGE
            I18nErrorCode errCode = I18nErrorCode::SUCCESS;
            PreferredLanguage::SetAppPreferredLanguage(localeTag, errCode);
            if (errCode != I18nErrorCode::SUCCESS) {
                HILOG_ERROR_I18N("Set app language to i18n app preferences failed, error code: %d", errCode);
                return -1;
            }
        #endif
            return 0;
    }

    char* FfiI18nSystemGetFirstPreferredLanguage()
    {
        return MallocCString(PreferredLanguage::GetFirstPreferredLanguage());
    }

    CArrStr FfiI18nSystemGetPreferredLanguageList()
    {
        return VectorStringToCArr(PreferredLanguage::GetPreferredLanguageList());
    }

    bool FfiI18nSystemIs24HourClock()
    {
        return LocaleConfig::Is24HourClock();
    }

    CArrStr FfiI18nSystemGetSystemLanguages()
    {
        std::unordered_set<std::string> systemLanguages = LocaleConfig::GetSystemLanguages();
        std::vector<std::string> result;
        result.assign(systemLanguages.begin(), systemLanguages.end());
        return VectorStringToCArr(result);
    }

    char* FfiI18nSystemGetDisplayLanguage(char* language, char* locale, bool sentenceCase, int32_t* errCode)
    {
        std::string languageStr(language);
        std::string localeStr(locale);
        if (!LocaleConfig::IsValidTag(localeStr)) {
            HILOG_ERROR_I18N("GetDisplayLanguage: Failed to get display language, because param locale is invalid!");
            *errCode = I18N_NOT_VALID;
            return nullptr;
        }
        std::string value = LocaleConfig::GetDisplayLanguage(languageStr, localeStr, sentenceCase);
        if (value.length() == 0) {
            HILOG_ERROR_I18N("GetDisplayLanguage: result is empty.");
        }
        return MallocCString(value);
    }

    char* FfiI18nSystemGetSystemLocale()
    {
        return MallocCString(LocaleConfig::GetSystemLocale());
    }
}
}  // namespace I18n
}  // namespace Global
}  // namespace OHOS