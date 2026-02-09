/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include <i18n_addon.h>

#include "advanced_measure_format_addon.h"
#include "date_time_sequence.h"
#include "entity_recognizer_addon.h"
#include "error_util.h"
#include "holiday_manager_addon.h"
#include "i18n_calendar_addon.h"
#include "i18n_hilog.h"
#include "i18n_normalizer_addon.h"
#include "i18n_system_addon.h"
#include "i18n_timezone_addon.h"
#include "i18n_unicode_addon.h"
#include "locale_config.h"
#include "locale_matcher.h"
#include "simple_date_time_format_addon.h"
#include "simple_number_format_addon.h"
#include "styled_date_time_format_addon.h"
#include "styled_number_format_addon.h"
#include "system_locale_manager_addon.h"
#include "variable_converter.h"
#include "zone_offset_transition_addon.h"
#include "zone_rules_addon.h"


using namespace OHOS;
using namespace Global;
using namespace I18n;

static const char PATH_SEPARATOR = '/';
static const int INDEX_ONE_OF_VECTOR = 0;

I18nPhoneNumberFormatAddon* I18nPhoneNumberFormatAddon::UnwrapAddon(ani_env *env, ani_object object)
{
    ani_long ptr;
    if (ANI_OK != env->Object_GetFieldByName_Long(object, "nativePhoneNumberFormat", &ptr)) {
        HILOG_ERROR_I18N("Get Long 'nativePhoneNumberFormat' failed");
        return nullptr;
    }
    return reinterpret_cast<I18nPhoneNumberFormatAddon*>(ptr);
}

ani_object I18nPhoneNumberFormatAddon::Create(ani_env *env, [[maybe_unused]] ani_object object,
    ani_string country, ani_object options)
{
    std::map<std::string, std::string> optionsStr;
    std::string typeStr;
    ani_boolean isUndefined;
    if (ANI_OK != env->Reference_IsUndefined(options, &isUndefined)) {
        HILOG_ERROR_I18N("I18nPhoneNumberFormatAddon::Create: Check options reference is undefined failed");
        return nullptr;
    }
    if (!isUndefined) {
        VariableConverter::GetStringMember(env, options, "type", typeStr);
    }
    optionsStr.insert(std::make_pair("type", typeStr));

    std::unique_ptr<I18nPhoneNumberFormatAddon> obj = std::make_unique<I18nPhoneNumberFormatAddon>();
    obj->phonenumberfmt_ = PhoneNumberFormat::CreateInstance(
        VariableConverter::AniStrToString(env, country), optionsStr);

    static const char *className = "@ohos.i18n.i18n.PhoneNumberFormat";
    ani_object phoneNumberFormatObject = VariableConverter::CreateAniObject(env, className, obj.get());
    obj.release();
    return phoneNumberFormatObject;
}

ani_boolean I18nPhoneNumberFormatAddon::IsValidNumber(ani_env *env, ani_object object, ani_string phoneNumber)
{
    auto obj = UnwrapAddon(env, object);
    if (obj == nullptr || obj->phonenumberfmt_ == nullptr) {
        HILOG_ERROR_I18N("I18nPhoneNumberFormatAddon::IsValidNumber: GetPhoneNumberFormat object failed");
        return false;
    }
    return obj->phonenumberfmt_->isValidPhoneNumber(VariableConverter::AniStrToString(env, phoneNumber));
}

ani_string I18nPhoneNumberFormatAddon::Format(ani_env *env, ani_object object, ani_string phoneNumber)
{
    auto obj = UnwrapAddon(env, object);
    if (obj == nullptr || obj->phonenumberfmt_ == nullptr) {
        HILOG_ERROR_I18N("I18nPhoneNumberFormatAddon::Format: Get PhoneNumberFormat object failed");
        return nullptr;
    }

    std::string formattedPhoneNumber =
        obj->phonenumberfmt_->format(VariableConverter::AniStrToString(env, phoneNumber));
    return VariableConverter::StringToAniStr(env, formattedPhoneNumber);
}

ani_string I18nPhoneNumberFormatAddon::GetLocationName(ani_env *env, ani_object object,
    ani_string phoneNumber, ani_string locale)
{
    auto obj = UnwrapAddon(env, object);
    if (obj == nullptr || obj->phonenumberfmt_ == nullptr) {
        HILOG_ERROR_I18N("I18nPhoneNumberFormatAddon::GetLocationName: GetPhoneNumberFormat object failed");
        return nullptr;
    }

    std::string resStr = obj->phonenumberfmt_->getLocationName(VariableConverter::AniStrToString(env, phoneNumber),
        VariableConverter::AniStrToString(env, locale));
    return VariableConverter::StringToAniStr(env, resStr);
}

I18nBreakIteratorAddon* I18nBreakIteratorAddon::UnwrapAddon(ani_env *env, ani_object object)
{
    ani_long ptr;
    if (ANI_OK != env->Object_GetFieldByName_Long(object, "nativeBreakIterator", &ptr)) {
        HILOG_ERROR_I18N("Get Long 'nativeBreakIterator' failed");
        return nullptr;
    }
    return reinterpret_cast<I18nBreakIteratorAddon*>(ptr);
}

ani_object I18nBreakIteratorAddon::GetLineInstance(ani_env *env, ani_string locale)
{
    std::unique_ptr<I18nBreakIteratorAddon> obj = std::make_unique<I18nBreakIteratorAddon>();
    obj->brkiter_ = std::make_unique<I18nBreakIterator>(VariableConverter::AniStrToString(env, locale));

    static const char* className = "@ohos.i18n.i18n.BreakIterator";
    ani_object breakIteratorObject = VariableConverter::CreateAniObject(env, className, obj.get());
    obj.release();
    return breakIteratorObject;
}

ani_int I18nBreakIteratorAddon::Current(ani_env *env, ani_object object)
{
    I18nBreakIteratorAddon* obj = UnwrapAddon(env, object);
    if (obj == nullptr || obj->brkiter_ == nullptr) {
        HILOG_ERROR_I18N("I18nBreakIteratorAddon::Current: Get BreakIterator object failed");
        return -1;
    }
    return obj->brkiter_->Current();
}

ani_int I18nBreakIteratorAddon::First(ani_env *env, ani_object object)
{
    I18nBreakIteratorAddon* obj = UnwrapAddon(env, object);
    if (obj == nullptr || obj->brkiter_ == nullptr) {
        HILOG_ERROR_I18N("I18nBreakIteratorAddon::First: Get BreakIterator object failed");
        return -1;
    }
    return obj->brkiter_->First();
}

ani_int I18nBreakIteratorAddon::Last(ani_env *env, ani_object object)
{
    I18nBreakIteratorAddon* obj = UnwrapAddon(env, object);
    if (obj == nullptr || obj->brkiter_ == nullptr) {
        HILOG_ERROR_I18N("I18nBreakIteratorAddon::Last: Get BreakIterator object failed");
        return -1;
    }
    return obj->brkiter_->Last();
}

ani_int I18nBreakIteratorAddon::Next(ani_env *env, ani_object object, ani_object index)
{
    I18nBreakIteratorAddon* obj = UnwrapAddon(env, object);
    if (obj == nullptr || obj->brkiter_ == nullptr) {
        HILOG_ERROR_I18N("I18nBreakIteratorAddon::Next: Get BreakIterator object failed");
        return -1;
    }

    ani_int value = 1;
    ani_boolean isUndefined;
    if (ANI_OK != env->Reference_IsUndefined(index, &isUndefined)) {
        HILOG_ERROR_I18N("I18nBreakIteratorAddon::Next: Check index reference is undefined failed");
        return -1;
    }
    if (!isUndefined) {
        if (ANI_OK != env->Object_CallMethodByName_Int(index, "toInt", ":i", &value)) {
            HILOG_ERROR_I18N("I18nBreakIteratorAddon::Next: Unbox Int failed");
            return -1;
        }
    }
    return obj->brkiter_->Next(value);
}

ani_int I18nBreakIteratorAddon::Previous(ani_env *env, ani_object object)
{
    I18nBreakIteratorAddon* obj = UnwrapAddon(env, object);
    if (obj == nullptr || obj->brkiter_ == nullptr) {
        HILOG_ERROR_I18N("I18nBreakIteratorAddon::Previous: Get BreakIterator object failed");
        return -1;
    }
    return obj->brkiter_->Previous();
}

void I18nBreakIteratorAddon::SetLineBreakText(ani_env *env, ani_object object, ani_string text)
{
    I18nBreakIteratorAddon* obj = UnwrapAddon(env, object);
    if (obj == nullptr || obj->brkiter_ == nullptr) {
        HILOG_ERROR_I18N("I18nBreakIteratorAddon::SetLineBreakText: Get BreakIterator object failed");
        return;
    }
    obj->brkiter_->SetText(VariableConverter::AniStrToString(env, text).c_str());
}

ani_int I18nBreakIteratorAddon::Following(ani_env *env, ani_object object, ani_int offset)
{
    I18nBreakIteratorAddon* obj = UnwrapAddon(env, object);
    if (obj == nullptr || obj->brkiter_ == nullptr) {
        HILOG_ERROR_I18N("I18nBreakIteratorAddon::Following: Get BreakIterator object failed");
        return -1;
    }
    return obj->brkiter_->Following(offset);
}

ani_string I18nBreakIteratorAddon::GetLineBreakText(ani_env *env, ani_object object)
{
    I18nBreakIteratorAddon* obj = UnwrapAddon(env, object);
    if (obj == nullptr || obj->brkiter_ == nullptr) {
        HILOG_ERROR_I18N("I18nBreakIteratorAddon::GetLineBreakText: Get BreakIterator object failed");
        return nullptr;
    }

    std::string temp;
    obj->brkiter_->GetText(temp);
    return VariableConverter::StringToAniStr(env, temp);
}

ani_boolean I18nBreakIteratorAddon::IsBoundary(ani_env *env, ani_object object, ani_int offset)
{
    I18nBreakIteratorAddon* obj = UnwrapAddon(env, object);
    if (obj == nullptr || obj->brkiter_ == nullptr) {
        HILOG_ERROR_I18N("I18nBreakIteratorAddon::IsBoundary: Get BreakIterator object failed");
        return false;
    }
    return obj->brkiter_->IsBoundary(offset);
}

I18nIndexUtilAddon* I18nIndexUtilAddon::UnwrapAddon(ani_env *env, ani_object object)
{
    ani_long ptr;
    if (ANI_OK != env->Object_GetFieldByName_Long(object, "nativeIndexUtil", &ptr)) {
        HILOG_ERROR_I18N("Get Long 'nativeIndexUtil' failed");
        return nullptr;
    }
    return reinterpret_cast<I18nIndexUtilAddon*>(ptr);
}

ani_object I18nIndexUtilAddon::GetInstance(ani_env *env, ani_string string)
{
    std::string localeTag = "";
    ani_boolean isUndefined;
    if (ANI_OK != env->Reference_IsUndefined(string, &isUndefined)) {
        HILOG_ERROR_I18N("I18nIndexUtilAddon::GetInstance: Check string reference is undefined failed");
        return nullptr;
    }
    if (!isUndefined) {
        localeTag = VariableConverter::AniStrToString(env, string);
    }

    std::unique_ptr<I18nIndexUtilAddon> obj = std::make_unique<I18nIndexUtilAddon>();
    obj->indexUtil_ = std::make_unique<IndexUtil>(localeTag);

    static const char* className = "@ohos.i18n.i18n.IndexUtil";
    ani_object indexUtilObject = VariableConverter::CreateAniObject(env, className, obj.get());
    obj.release();
    return indexUtilObject;
}

ani_object I18nIndexUtilAddon::GetIndexList(ani_env *env, ani_object object)
{
    I18nIndexUtilAddon* obj = UnwrapAddon(env, object);
    if (obj == nullptr || obj->indexUtil_ == nullptr) {
        HILOG_ERROR_I18N("I18nIndexUtilAddon::GetIndexList: GetPhoneNumberFormat object failed");
        return nullptr;
    }
    std::vector<std::string> indexList = obj->indexUtil_->GetIndexList();
    return VariableConverter::CreateArray(env, indexList);
}

void I18nIndexUtilAddon::AddLocale(ani_env *env, ani_object object, ani_string locale)
{
    I18nIndexUtilAddon* obj = UnwrapAddon(env, object);
    if (obj == nullptr || obj->indexUtil_ == nullptr) {
        HILOG_ERROR_I18N("I18nIndexUtilAddon::AddLocale: Get IndexUtil object failed");
        return;
    }
    obj->indexUtil_->AddLocale(VariableConverter::AniStrToString(env, locale));
}

ani_object I18nIndexUtilAddon::GetIndex(ani_env *env, ani_object object, ani_string text)
{
    I18nIndexUtilAddon* obj = UnwrapAddon(env, object);
    if (obj == nullptr || obj->indexUtil_ == nullptr) {
        HILOG_ERROR_I18N("I18nIndexUtilAddon::GetIndex: Get IndexUtil object failed");
        return nullptr;
    }

    std::string index = obj->indexUtil_->GetIndex(VariableConverter::AniStrToString(env, text));
    return VariableConverter::StringToAniStr(env, index);
}

I18nTransliteratorAddon* I18nTransliteratorAddon::UnwrapAddon(ani_env *env, ani_object object)
{
    ani_long ptr;
    if (ANI_OK != env->Object_GetFieldByName_Long(object, "nativeTransliterator", &ptr)) {
        HILOG_ERROR_I18N("Get Long 'nativeTransliterator' failed");
        return nullptr;
    }
    return reinterpret_cast<I18nTransliteratorAddon*>(ptr);
}

ani_object I18nTransliteratorAddon::GetAvailableIDs(ani_env *env, [[maybe_unused]] ani_object object)
{
    UErrorCode icuStatus = U_ZERO_ERROR;
    icu::StringEnumeration *strenum = icu::Transliterator::getAvailableIDs(icuStatus);
    if (U_FAILURE(icuStatus)) {
        HILOG_ERROR_I18N("I18nTransliteratorAddon::GetAvailableIDs: Failed to get available ids");
        if (strenum) {
            delete strenum;
        }
        return nullptr;
    }

    std::vector<std::string> availableIDs;
    uint32_t i = 0;
    const char *temp = nullptr;
    if (strenum == nullptr) {
        return nullptr;
    }
    while ((temp = strenum->next(nullptr, icuStatus)) != nullptr) {
        if (U_FAILURE(icuStatus)) {
            break;
        }
        availableIDs.push_back(temp);
        ++i;
    }
    delete strenum;
    return VariableConverter::CreateArray(env, availableIDs);
}

ani_object I18nTransliteratorAddon::GetInstance(ani_env *env, [[maybe_unused]] ani_object object, ani_string id)
{
    std::unique_ptr<I18nTransliteratorAddon> obj = std::make_unique<I18nTransliteratorAddon>();
    UErrorCode status = U_ZERO_ERROR;
    icu::UnicodeString unistr = icu::UnicodeString::fromUTF8(VariableConverter::AniStrToString(env, id));
    icu::Transliterator *trans = icu::Transliterator::createInstance(unistr, UTransDirection::UTRANS_FORWARD, status);
    if (U_FAILURE(status) || (trans == nullptr)) {
        return nullptr;
    }

    obj->transliterator_ = std::unique_ptr<icu::Transliterator>(trans);
    static const char* className = "@ohos.i18n.i18n.Transliterator";
    ani_object transliteratorObject = VariableConverter::CreateAniObject(env, className, obj.get());
    obj.release();
    return transliteratorObject;
}

ani_string I18nTransliteratorAddon::Transform(ani_env *env, ani_object object, ani_string text)
{
    I18nTransliteratorAddon* obj = UnwrapAddon(env, object);
    if (obj == nullptr || obj->transliterator_ == nullptr) {
        HILOG_ERROR_I18N("I18nTransliteratorAddon::Transform: Get Transliterator object failed");
        return nullptr;
    }

    icu::UnicodeString unistr = icu::UnicodeString::fromUTF8(VariableConverter::AniStrToString(env, text));
    obj->transliterator_->transliterate(unistr);

    std::string temp;
    unistr.toUTF8String(temp);
    return VariableConverter::StringToAniStr(env, temp);
}

static bool GetUnitInfo(ani_env *env, ani_object obj, std::string &unit, std::string &system)
{
    std::string value;
    if (!VariableConverter::GetStringMember(env, obj, "unit", value)) {
        return false;
    }
    unit = value;

    if (!VariableConverter::GetStringMember(env, obj, "measureSystem", value)) {
        return false;
    }
    system = value;
    return true;
}

ani_string I18nAddon::UnitConvert(ani_env *env, [[maybe_unused]] ani_object object, ani_object fromUnit,
    ani_object toUnit, ani_double value, ani_string locale, ani_string style)
{
    std::string fromUnitStr;
    std::string fromMeasSys;
    std::string toUnitStr;
    std::string toMeasSys;
    if (!GetUnitInfo(env, fromUnit, fromUnitStr, fromMeasSys) || !GetUnitInfo(env, toUnit, toUnitStr, toMeasSys)) {
        HILOG_ERROR_I18N("I18nAddon::UnitConvert: Get UnitInfo failed");
        return nullptr;
    }

    int convertStatus = Convert(value, fromUnitStr, fromMeasSys, toUnitStr, toMeasSys);
    std::map<std::string, std::string> map = {};
    map.insert(std::make_pair("style", "unit"));
    if (!convertStatus) {
        map.insert(std::make_pair("unit", fromUnitStr));
    } else {
        map.insert(std::make_pair("unit", toUnitStr));
    }

    ani_boolean isUndefined;
    if (ANI_OK != env->Reference_IsUndefined(style, &isUndefined)) {
        HILOG_ERROR_I18N("I18nAddon::UnitConvert: Check style reference is undefined failed");
        return nullptr;
    }
    if (!isUndefined) {
        map.insert(std::make_pair("unitDisplay", VariableConverter::AniStrToString(env, style)));
    }

    std::vector<std::string> localeTags;
    localeTags.push_back(VariableConverter::AniStrToString(env, locale));
    std::unique_ptr<NumberFormat> numberFmt = std::make_unique<NumberFormat>(localeTags, map);
    std::string str = numberFmt->Format(value);
    return VariableConverter::StringToAniStr(env, str);
}

ani_string I18nAddon::GetDateOrder(ani_env *env, [[maybe_unused]] ani_object object, ani_string locale)
{
    std::string value = DateTimeSequence::GetDateOrder(VariableConverter::AniStrToString(env, locale));
    return VariableConverter::StringToAniStr(env, value);
}

ani_string I18nAddon::GetTimePeriodName(ani_env *env, [[maybe_unused]] ani_object object,
    ani_int hour, ani_string locale)
{
    std::string localeTag;
    ani_boolean isUndefined;
    if (ANI_OK != env->Reference_IsUndefined(locale, &isUndefined)) {
        HILOG_ERROR_I18N("I18nAddon::GetTimePeriodName: Check locale reference is undefined failed");
        return nullptr;
    }
    if (!isUndefined) {
        localeTag = VariableConverter::AniStrToString(env, locale);
    } else {
        localeTag = LocaleConfig::GetSystemLocale();
    }

    UErrorCode icuStatus = U_ZERO_ERROR;
    icu::Locale icuLocale = icu::Locale::forLanguageTag(localeTag, icuStatus);
    if (U_FAILURE(icuStatus) || !IsValidLocaleTag(icuLocale)) {
        ErrorUtil::AniThrow(env, I18N_NOT_VALID, "locale", "a valid locale");
        return nullptr;
    }
    icu::SimpleDateFormat* formatter = dynamic_cast<icu::SimpleDateFormat*>
        (icu::DateFormat::createDateInstance(icu::DateFormat::EStyle::kDefault, icuLocale));
    if (!formatter) {
        HILOG_ERROR_I18N("I18nAddon::GetTimePeriodName: Failed to create SimpleDateFormat");
        return nullptr;
    }
    formatter->applyPattern("B");

    icu::UnicodeString name;
    icu::Calendar *calendar = icu::Calendar::createInstance(icuLocale, icuStatus);
    if (calendar == nullptr) {
        delete formatter;
        return nullptr;
    }
    calendar->set(UCalendarDateFields::UCAL_HOUR_OF_DAY, hour);
    formatter->format(calendar->getTime(icuStatus), name);
    std::string result;
    name.toUTF8String(result);
    delete formatter;
    delete calendar;
    return VariableConverter::StringToAniStr(env, result);
}

static LocaleInfo* ProcessJsParamLocale(ani_env *env, ani_string locale)
{
    std::string localeTag = VariableConverter::AniStrToString(env, locale);
    UErrorCode icuStatus = U_ZERO_ERROR;
    icu::Locale icuLocale = icu::Locale::forLanguageTag(localeTag, icuStatus);
    if (U_FAILURE(icuStatus) || !IsValidLocaleTag(icuLocale)) {
        ErrorUtil::AniThrow(env, I18N_NOT_VALID, "locale", "a valid locale");
        return nullptr;
    }
    return new LocaleInfo(localeTag);
}

static bool ProcessJsParamLocaleList(ani_env *env, ani_object localeList, std::vector<LocaleInfo*> &candidateLocales,
    LocaleInfo *requestLocale)
{
    std::vector<std::string> localeTagList;
    if (!VariableConverter::ParseStringArray(env, localeList, localeTagList)) {
        HILOG_ERROR_I18N("ProcessJsParamLocaleList: Failed to obtain the parameter.");
        return false;
    }
    if (localeTagList.size() == 0) {
        return true;
    }

    for (auto it = localeTagList.begin(); it != localeTagList.end(); ++it) {
        UErrorCode icuStatus = U_ZERO_ERROR;
        icu::Locale locale = icu::Locale::forLanguageTag(it->data(), icuStatus);
        if (U_FAILURE(icuStatus) || !IsValidLocaleTag(locale)) {
            HILOG_ERROR_I18N("ProcessJsParamLocaleList: locale invalid: %{public}s.", it->data());
            ErrorUtil::AniThrow(env, I18N_NOT_VALID, "locale of localeList", "a valid locale");
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

static void ReleaseParam(LocaleInfo *locale, std::vector<LocaleInfo*> &candidateLocales)
{
    delete locale;
    for (auto it = candidateLocales.begin(); it != candidateLocales.end(); ++it) {
        delete *it;
    }
}

ani_string I18nAddon::GetBestMatchLocale(ani_env *env, [[maybe_unused]] ani_object object,
    ani_string locale, ani_object localeList)
{
    LocaleInfo *requestLocale = ProcessJsParamLocale(env, locale);
    if (requestLocale == nullptr) {
        return nullptr;
    }

    std::vector<LocaleInfo*> candidateLocales;
    bool isValidParam = ProcessJsParamLocaleList(env, localeList, candidateLocales, requestLocale);
    if (!isValidParam) {
        ReleaseParam(requestLocale, candidateLocales);
        return nullptr;
    }

    std::string bestMatchLocaleTag = "";
    if (candidateLocales.size() > 0) {
        LocaleInfo *bestMatch = candidateLocales[INDEX_ONE_OF_VECTOR];
        for (size_t i = 1; i < candidateLocales.size(); ++i) {
            if (LocaleMatcher::IsMoreSuitable(bestMatch, candidateLocales[i], requestLocale) < 0) {
                bestMatch = candidateLocales[i];
            }
        }
        bestMatchLocaleTag = bestMatch->ToString();
    }
    ReleaseParam(requestLocale, candidateLocales);
    return VariableConverter::StringToAniStr(env, bestMatchLocaleTag);
}

ani_string I18nAddon::GetThreeLetterLanguage(ani_env *env, [[maybe_unused]] ani_object object, ani_string locale)
{
    std::string language = GetISO3Language(VariableConverter::AniStrToString(env, locale));
    return VariableConverter::StringToAniStr(env, language);
}

ani_string I18nAddon::GetThreeLetterRegion(ani_env *env, [[maybe_unused]] ani_object object, ani_string locale)
{
    std::string country = GetISO3Country(VariableConverter::AniStrToString(env, locale));
    return VariableConverter::StringToAniStr(env, country);
}

ani_string I18nAddon::GetUnicodeWrappedFilePath(ani_env *env, [[maybe_unused]] ani_object object,
    ani_string path, ani_string delimiter, ani_object locale)
{
    char delimiterChar = PATH_SEPARATOR;
    ani_boolean isUndefined;
    if (ANI_OK != env->Reference_IsUndefined(delimiter, &isUndefined)) {
        HILOG_ERROR_I18N("I18nAddon::GetUnicodeWrappedFilePath: get delimiter Reference_IsUndefined failed");
        return nullptr;
    }
    if (!isUndefined) {
        std::string result = VariableConverter::AniStrToString(env, delimiter);
        if (result.length() != 1) {
            ErrorUtil::AniThrow(env, I18N_NOT_VALID, "delimiter", "a valid delimiter");
            delimiterChar = '\0';
        } else {
            delimiterChar = result.at(0);
        }
    }

    if (ANI_OK != env->Reference_IsUndefined(locale, &isUndefined)) {
        HILOG_ERROR_I18N("I18nAddon::GetUnicodeWrappedFilePath: get locale Reference_IsUndefined failed");
        return nullptr;
    }
    std::string localeTag = "";
    if (!isUndefined) {
        localeTag = VariableConverter::GetLocaleTagFromBuiltinLocale(env, locale);
    }

    std::string errorCode;
    std::shared_ptr<LocaleInfo> localeInfoPtr = localeTag.empty() ? nullptr : std::make_shared<LocaleInfo>(localeTag);
    std::string result = LocaleConfig::GetUnicodeWrappedFilePath(VariableConverter::AniStrToString(env, path),
        delimiterChar, localeInfoPtr, errorCode);
    if (!errorCode.empty()) {
        ErrorUtil::AniThrow(env, I18N_NOT_VALID, errorCode, "valid");
        return nullptr;
    }
    return VariableConverter::StringToAniStr(env, result);
}

ani_boolean I18nAddon::IsRTL(ani_env *env, ani_string locale)
{
    return LocaleConfig::IsRTL(VariableConverter::AniStrToString(env, locale));
}

ani_status I18nPhoneNumberFormatAddon::BindContextPhoneNumberFormat(ani_env *env)
{
    static const char *className = "@ohos.i18n.i18n.PhoneNumberFormat";
    ani_class cls;
    if (ANI_OK != env->FindClass(className, &cls)) {
        HILOG_ERROR_I18N("BindContextPhoneNumberFormat: Find class '%{public}s' failed", className);
        return ANI_ERROR;
    }

    std::array staticMethods = {
        ani_native_function { "create", nullptr, reinterpret_cast<void *>(I18nPhoneNumberFormatAddon::Create) },
    };
    if (ANI_OK != env->Class_BindStaticNativeMethods(cls, staticMethods.data(), staticMethods.size())) {
        HILOG_ERROR_I18N("Cannot bind static native methods to '%{public}s'", className);
        return ANI_ERROR;
    }

    std::array methods = {
        ani_native_function { "isValidNumber", nullptr,
            reinterpret_cast<void *>(I18nPhoneNumberFormatAddon::IsValidNumber) },
        ani_native_function { "format", nullptr, reinterpret_cast<void *>(I18nPhoneNumberFormatAddon::Format) },
        ani_native_function { "getLocationName", nullptr,
            reinterpret_cast<void *>(I18nPhoneNumberFormatAddon::GetLocationName) },
    };
    if (ANI_OK != env->Class_BindNativeMethods(cls, methods.data(), methods.size())) {
        HILOG_ERROR_I18N("BindContextPhoneNumberFormat: Cannot bind native methods to '%{public}s'", className);
        return ANI_ERROR;
    }
    return ANI_OK;
}

ani_status I18nBreakIteratorAddon::BindContextBreakIterator(ani_env *env)
{
    static const char *className = "@ohos.i18n.i18n.BreakIterator";
    ani_class cls;
    if (ANI_OK != env->FindClass(className, &cls)) {
        HILOG_ERROR_I18N("BindContextBreakIterator: Find class '%{public}s' failed", className);
        return ANI_ERROR;
    }

    std::array methods = {
        ani_native_function { "current", nullptr, reinterpret_cast<void *>(I18nBreakIteratorAddon::Current) },
        ani_native_function { "first", nullptr, reinterpret_cast<void *>(I18nBreakIteratorAddon::First) },
        ani_native_function { "last", nullptr, reinterpret_cast<void *>(I18nBreakIteratorAddon::Last) },
        ani_native_function { "next", nullptr, reinterpret_cast<void *>(I18nBreakIteratorAddon::Next) },
        ani_native_function { "previous", nullptr, reinterpret_cast<void *>(I18nBreakIteratorAddon::Previous) },
        ani_native_function { "setLineBreakText", nullptr,
            reinterpret_cast<void *>(I18nBreakIteratorAddon::SetLineBreakText) },
        ani_native_function { "following", nullptr, reinterpret_cast<void *>(I18nBreakIteratorAddon::Following) },
        ani_native_function { "getLineBreakText", nullptr,
            reinterpret_cast<void *>(I18nBreakIteratorAddon::GetLineBreakText) },
        ani_native_function { "isBoundary", nullptr, reinterpret_cast<void *>(I18nBreakIteratorAddon::IsBoundary) },
    };

    if (ANI_OK != env->Class_BindNativeMethods(cls, methods.data(), methods.size())) {
        HILOG_ERROR_I18N("BindContextBreakIterator: Cannot bind native methods to '%{public}s'", className);
        return ANI_ERROR;
    };

    return ANI_OK;
}

ani_status I18nIndexUtilAddon::BindContextIndexUtil(ani_env *env)
{
    static const char *className = "@ohos.i18n.i18n.IndexUtil";
    ani_class cls;
    if (ANI_OK != env->FindClass(className, &cls)) {
        HILOG_ERROR_I18N("BindContextIndexUtil: Find class '%{public}s' failed", className);
        return ANI_ERROR;
    }

    std::array methods = {
        ani_native_function { "getIndexList", nullptr, reinterpret_cast<void *>(I18nIndexUtilAddon::GetIndexList) },
        ani_native_function { "addLocale", nullptr, reinterpret_cast<void *>(I18nIndexUtilAddon::AddLocale) },
        ani_native_function { "getIndex", nullptr, reinterpret_cast<void *>(I18nIndexUtilAddon::GetIndex) },
    };

    if (ANI_OK != env->Class_BindNativeMethods(cls, methods.data(), methods.size())) {
        HILOG_ERROR_I18N("BindContextIndexUtil: Cannot bind native methods to '%{public}s'", className);
        return ANI_ERROR;
    };

    return ANI_OK;
}

ani_status I18nTransliteratorAddon::BindContextTransliterator(ani_env *env)
{
    static const char *className = "@ohos.i18n.i18n.Transliterator";
    ani_class cls;
    if (ANI_OK != env->FindClass(className, &cls)) {
        HILOG_ERROR_I18N("BindContextTransliterator: Find class '%{public}s' failed", className);
        return ANI_ERROR;
    }

    std::array staticMethods = {
        ani_native_function { "getAvailableIDs", nullptr,
            reinterpret_cast<void *>(I18nTransliteratorAddon::GetAvailableIDs) },
        ani_native_function { "getInstance", nullptr, reinterpret_cast<void *>(I18nTransliteratorAddon::GetInstance) },
    };
    if (ANI_OK != env->Class_BindStaticNativeMethods(cls, staticMethods.data(), staticMethods.size())) {
        HILOG_ERROR_I18N("Cannot bind static native methods to '%{public}s'", className);
        return ANI_ERROR;
    }

    std::array methods = {
        ani_native_function { "transform", nullptr, reinterpret_cast<void *>(I18nTransliteratorAddon::Transform) },
    };
    if (ANI_OK != env->Class_BindNativeMethods(cls, methods.data(), methods.size())) {
        HILOG_ERROR_I18N("BindContextTransliterator: Cannot bind native methods to '%{public}s'", className);
        return ANI_ERROR;
    }
    return ANI_OK;
}

ani_status I18nAddon::BindContextI18NUtil(ani_env *env)
{
    static const char *className = "@ohos.i18n.i18n.I18NUtil";
    ani_class cls;
    if (ANI_OK != env->FindClass(className, &cls)) {
        HILOG_ERROR_I18N("BindContextI18NUtil: Find class '%{public}s' failed", className);
        return ANI_ERROR;
    }

    std::array methods = {
        ani_native_function { "unitConvert", nullptr, reinterpret_cast<void *>(UnitConvert) },
        ani_native_function { "getDateOrder", nullptr, reinterpret_cast<void *>(GetDateOrder) },
        ani_native_function { "getTimePeriodName", nullptr, reinterpret_cast<void *>(GetTimePeriodName) },
        ani_native_function { "getBestMatchLocale", nullptr, reinterpret_cast<void *>(GetBestMatchLocale) },
        ani_native_function { "getThreeLetterLanguage", nullptr, reinterpret_cast<void *>(GetThreeLetterLanguage) },
        ani_native_function { "getThreeLetterRegion", nullptr, reinterpret_cast<void *>(GetThreeLetterRegion) },
        ani_native_function { "getUnicodeWrappedFilePath", nullptr,
            reinterpret_cast<void *>(GetUnicodeWrappedFilePath) },
    };

    if (ANI_OK != env->Class_BindStaticNativeMethods(cls, methods.data(), methods.size())) {
        HILOG_ERROR_I18N("BindContextI18NUtil: Cannot bind native methods to '%{public}s'", className);
        return ANI_ERROR;
    };

    return ANI_OK;
}

ani_status I18nAddon::BindContextNamespace(ani_env *env)
{
    static const char* nameSpaceName = "@ohos.i18n.i18n";
    ani_namespace ns;
    if (ANI_OK != env->FindNamespace(nameSpaceName, &ns)) {
        HILOG_ERROR_I18N("BindContextNamespace: Find namespace '%{public}s' failed", nameSpaceName);
        return ANI_ERROR;
    }
    std::array methods = {
        ani_native_function { "getCalendar", nullptr, reinterpret_cast<void *>(I18nCalendarAddon::GetCalendar) },
        ani_native_function { "isRTL", nullptr, reinterpret_cast<void *>(I18nAddon::IsRTL) },
        ani_native_function { "getLineInstance", nullptr,
            reinterpret_cast<void *>(I18nBreakIteratorAddon::GetLineInstance) },
        ani_native_function { "getInstance", nullptr, reinterpret_cast<void *>(I18nIndexUtilAddon::GetInstance) },
        ani_native_function { "getTimeZone", nullptr, reinterpret_cast<void *>(I18nTimeZoneAddon::GetTimeZone) },
        ani_native_function { "getSimpleDateTimeFormatByPattern", nullptr,
            reinterpret_cast<void *>(I18nSimpleDateTimeFormatAddon::GetSimpleDateTimeFormatByPattern) },
        ani_native_function { "getSimpleDateTimeFormatBySkeleton", nullptr,
            reinterpret_cast<void *>(I18nSimpleDateTimeFormatAddon::GetSimpleDateTimeFormatBySkeleton) },
        ani_native_function { "getSimpleNumberFormatBySkeleton", nullptr,
            reinterpret_cast<void *>(I18nSimpleNumberFormatAddon::GetSimpleNumberFormatBySkeleton) },
    };
    if (ANI_OK != env->Namespace_BindNativeFunctions(ns, methods.data(), methods.size())) {
        HILOG_ERROR_I18N("BindContextNamespace: Cannot bind native methods to '%{public}s'", nameSpaceName);
        return ANI_ERROR;
    };

    return ANI_OK;
}

ani_status BindContext(ani_env *env)
{
    auto status = I18nTransliteratorAddon::BindContextTransliterator(env);
    if (status != ANI_OK) {
        return status;
    }

    status = I18nNormalizerAddon::BindContextNormalizer(env);
    if (status != ANI_OK) {
        return status;
    }

    status = I18nSysLocaleMgrAddon::BindContextSysLocaleMgr(env);
    if (status != ANI_OK) {
        return status;
    }

    status = I18nHolidayManagerAddon::BindContextHolidayManager(env);
    if (status != ANI_OK) {
        return status;
    }

    status = I18nEntityRecognizerAddon::BindContextEntityRecognizer(env);
    if (status != ANI_OK) {
        return status;
    }

    status = I18nSimpleDateTimeFormatAddon::BindContextSimpleDateTimeFormat(env);
    if (status != ANI_OK) {
        return status;
    }

    status = I18nSimpleNumberFormatAddon::BindContextSimpleNumberFormat(env);
    if (status != ANI_OK) {
        return status;
    }

    status = I18nStyledNumberFormatAddon::BindContextStyledNumberFormat(env);
    if (status != ANI_OK) {
        return status;
    }

    status = I18nStyledDateTimeFormatAddon::BindContextStyledDateTimeFormat(env);
    if (status != ANI_OK) {
        return status;
    }

    status = AdvancedMeasureFormatAddon::BindContextAdvancedMeasureFormat(env);
    if (status != ANI_OK) {
        return status;
    }

    status = I18nAddon::BindContextNamespace(env);
    if (status != ANI_OK) {
        return status;
    }
    return status;
}

ani_status BindZoneContext(ani_env *env)
{
    auto status = ZoneRulesAddon::BindContextZoneRules(env);
    if (status != ANI_OK) {
        return status;
    }
    return ZoneOffsetTransitionAddon::BindContextZoneOffsetTransition(env);
}

ANI_EXPORT ani_status ANI_Constructor(ani_vm *vm, uint32_t *result)
{
    ani_env *env;
    if (ANI_OK != vm->GetEnv(ANI_VERSION_1, &env)) {
        HILOG_ERROR_I18N("Unsupported ANI_VERSION_1");
        return ANI_ERROR;
    }

    auto status = I18nSystemAddon::BindContextSystem(env);
    if (status != ANI_OK) {
        return status;
    }

    status = I18nAddon::BindContextI18NUtil(env);
    if (status != ANI_OK) {
        return status;
    }

    status = I18nPhoneNumberFormatAddon::BindContextPhoneNumberFormat(env);
    if (status != ANI_OK) {
        return status;
    }

    status = I18nCalendarAddon::BindContextCalendar(env);
    if (status != ANI_OK) {
        return status;
    }

    status = I18nBreakIteratorAddon::BindContextBreakIterator(env);
    if (status != ANI_OK) {
        return status;
    }

    status = I18nIndexUtilAddon::BindContextIndexUtil(env);
    if (status != ANI_OK) {
        return status;
    }

    status = I18nUnicodeAddon::BindContextUnicode(env);
    if (status != ANI_OK) {
        return status;
    }

    status = I18nTimeZoneAddon::BindContextTimeZone(env);
    if (status != ANI_OK) {
        return status;
    }
    status = BindZoneContext(env);
    if (status != ANI_OK) {
        return status;
    }
    status = BindContext(env);
    *result = ANI_VERSION_1;
    return ANI_OK;
}
