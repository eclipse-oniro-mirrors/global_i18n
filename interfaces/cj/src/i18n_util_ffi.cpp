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
#include <vector>
#include <utility>
#include <map>
#include <memory>
#include <string>

#include "i18n_ffi.h"
#include "i18n_hilog.h"
#include "i18n_util_ffi.h"
#include "locale_config.h"
#include "locale_info.h"
#include "character.h"
#include "utils.h"
#include "unicode/locid.h"
#include "unicode/datefmt.h"
#include "unicode/smpdtfmt.h"
#include "i18n_struct.h"
#include "locale_matcher.h"
#include "measure_data.h"
#include "number_format.h"
#include "date_time_sequence.h"

namespace OHOS {
namespace Global {
namespace I18n {
std::vector<std::string> convert2CppStringArray(const CArrStr& carrStr)
{
    std::vector<std::string> cppStringArray;
    for (int i = 0; i < carrStr.length; ++i) {
        cppStringArray.push_back(carrStr.data[i]);
    }
    return cppStringArray;
}

extern "C" {
void ReleaseParam(LocaleInfo *locale, std::vector<LocaleInfo*> &candidateLocales)
{
    delete locale;
    for (auto it = candidateLocales.begin(); it != candidateLocales.end(); ++it) {
        delete *it;
    }
}

LocaleInfo* GetLocaleInfo(std::string localeStr, int32_t* errcode)
{
    UErrorCode status = U_ZERO_ERROR;
    icu::Locale locale = icu::Locale::forLanguageTag(localeStr, status);
    if (U_FAILURE(status) || !IsValidLocaleTag(locale)) {
        *errcode = I18N_NOT_VALID;
        return nullptr;
    }
    return new LocaleInfo(localeStr);
}

bool ProcessLocaleList(std::vector<std::string> localeTagList,
    std::vector<LocaleInfo*> &candidateLocales, LocaleInfo *requestLocale, int32_t* errcode)
    {
    if (localeTagList.size() == 0) {
        *errcode = I18N_NOT_VALID;
        return true;
    }
    for (auto it = localeTagList.begin(); it != localeTagList.end(); ++it) {
        UErrorCode icuStatus = U_ZERO_ERROR;
        icu::Locale locale = icu::Locale::forLanguageTag(it->data(), icuStatus);
        if (U_FAILURE(icuStatus) || !IsValidLocaleTag(locale)) {
            *errcode = I18N_NOT_VALID;
            return false;
        }
        LocaleInfo *temp = new LocaleInfo(*it);
        if (LocaleMatcher::Match(requestLocale, temp)) {
            candidateLocales.push_back(temp);
        } else {
            delete temp;
        }
    }
    return true;
}

char* FfiI18nUtilUnitConvert(UnitInfo fromUnit, UnitInfo toUnit, double value, const char* locale, const char* style)
{
    double number = value;
    std::string fromInfoUnit(fromUnit.unit);
    std::string fromInfoMeasureSystem(fromUnit.measureSystem);
    std::string toInfoUnit(toUnit.unit);
    std::string toInfoMeasureSystem(toUnit.measureSystem);

    int32_t convertStatus = Convert(number, fromInfoUnit, fromInfoMeasureSystem, toInfoUnit, toInfoMeasureSystem);

    std::vector<std::string> localeTags;
    std::map<std::string, std::string> map = {};

    map.insert(std::make_pair("style", "unit"));
    if (!convertStatus) {
        map.insert(std::make_pair("unit", fromInfoUnit));
    } else {
        map.insert(std::make_pair("unit", toInfoUnit));
    }
    map.insert(std::make_pair("unitDisplay", std::string(style)));
    std::string localeTag(locale);
    localeTags.push_back(localeTag);
    std::unique_ptr<NumberFormat> numberFmt = nullptr;
    numberFmt = std::make_unique<NumberFormat>(localeTags, map);
    std::string result = numberFmt->Format(number);
    char* res = MallocCString(result);
    return res;
}

char* FfiI18nUtilGetBestMatchLocale(const char* locale, CArrStr localeList, int32_t* errCode)
{
    std::vector<std::string> localeArr = convert2CppStringArray(localeList);
    std::string localeStr(locale);
    LocaleInfo *requestLocale = GetLocaleInfo(localeStr, errCode);
    if (requestLocale == nullptr) {
        return nullptr;
    }
    std::vector<LocaleInfo*> candidateLocales;
    bool isVaildParam = ProcessLocaleList(localeArr, candidateLocales, requestLocale, errCode);
    if (!isVaildParam) {
        *errCode = I18N_NOT_VALID;
        ReleaseParam(requestLocale, candidateLocales);
        return nullptr;
    }
    std::string bestMatchLocaleTag = "";
    if (candidateLocales.size() > 0) {
        LocaleInfo *bestMatch = candidateLocales[0];
        for (size_t i = 1; i < candidateLocales.size(); ++i) {
            if (LocaleMatcher::IsMoreSuitable(bestMatch, candidateLocales[i], requestLocale) < 0) {
                bestMatch = candidateLocales[i];
            }
        }
        bestMatchLocaleTag = bestMatch->ToString();
    }
    ReleaseParam(requestLocale, candidateLocales);
    char* res = MallocCString(bestMatchLocaleTag);
    return res;
}

char* FfiI18nUtilGetDateOrder(const char* locale)
{
    std::string loacleStr(locale);
    char* res = MallocCString(DateTimeSequence::GetDateOrder(loacleStr));
    return res;
}

char* FfiI18nUtilGetTimePeriodName(int32_t hour, const char* locale, int32_t* errcode)
{
    std::string localeTag;
    if (locale == nullptr) {
        localeTag = LocaleConfig::GetSystemLocale();
    } else {
        localeTag = std::string(locale);
    }
    UErrorCode status = U_ZERO_ERROR;
    icu::Locale localeIcu = icu::Locale::forLanguageTag(localeTag, status);
    if (U_FAILURE(status) || !IsValidLocaleTag(localeIcu)) {
        *errcode = I18N_NOT_VALID;
        return nullptr;
    }
    icu::DateFormat* dateFormatter = icu::DateFormat::createDateInstance(icu::DateFormat::EStyle::kDefault, locale);
    if (dateFormatter == nullptr) {
        *errcode = I18N_NOT_VALID;
        return nullptr;
    }
    icu::SimpleDateFormat* formatter = static_cast<icu::SimpleDateFormat*>(dateFormatter);
    formatter->applyPattern("B");
    std::string temp;
    icu::UnicodeString name;
    icu::Calendar *calendar = icu::Calendar::createInstance(locale, status);
    if (calendar == nullptr) {
        *errcode = I18N_NOT_VALID;
        delete formatter;
        return nullptr;
    }
    calendar->set(UCalendarDateFields::UCAL_HOUR_OF_DAY, hour);
    formatter->format(calendar->getTime(status), name);
    name.toUTF8String(temp);
    char* res = MallocCString(PseudoLocalizationProcessor(temp));
    delete formatter;
    delete calendar;
    return res;
}

char* FfiI18nUtilGetThreeLetterLanguage(const char* locale, int32_t* errcode)
{
    std::string language = GetISO3Language(std::string(locale));
    if (language.empty()) {
        *errcode = I18N_NOT_VALID;
        HILOG_ERROR_I18N("GetThreeLetterLanguage create string fail or empty");
        return nullptr;
    }
    char* res = MallocCString(language);
    return res;
}

char* FfiI18nUtilGetThreeLetterRegion(const char* locale, int32_t* errcode)
{
    std::string country = GetISO3Country(std::string(locale));
    if (country.empty()) {
        HILOG_ERROR_I18N("GetThreeLetterRegion create string fail or empty");
        *errcode = I18N_NOT_VALID;
        return nullptr;
    }
    char* res = MallocCString(country);
    return res;
}
}
}  // namespace I18n
}  // namespace Global
}  // namespace OHOS
