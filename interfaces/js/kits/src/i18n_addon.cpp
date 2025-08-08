/*
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd.
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
#include <unordered_map>
#include <vector>

#include "error_util.h"
#include "i18n_hilog.h"
#include "holiday_manager_addon.h"
#include "entity_recognizer_addon.h"
#include "i18n_calendar_addon.h"
#include "i18n_normalizer_addon.h"
#include "i18n_system_addon.h"
#include "i18n_timezone_addon.h"
#include "i18n_unicode_addon.h"
#include "js_utils.h"
#include "locale_config.h"
#include "locale_info.h"
#include "locale_matcher.h"
#include "node_api.h"
#include "simple_date_time_format_addon.h"
#include "simple_number_format_addon.h"
#include "styled_number_format_addon.h"
#include "system_locale_manager_addon.h"
#include "unicode/datefmt.h"
#include "unicode/locid.h"
#include "unicode/smpdtfmt.h"
#include "unicode/translit.h"
#include "utils.h"
#include "variable_convertor.h"
#include "i18n_addon.h"
#include "date_time_sequence.h"
#include "locale_info_addon.h"

namespace OHOS {
namespace Global {
namespace I18n {
static thread_local napi_ref* g_brkConstructor = nullptr;
static thread_local napi_ref g_indexUtilConstructor = nullptr;
static thread_local napi_ref* g_transConstructor = nullptr;

std::unordered_map<std::string, int32_t> I18nAddon::EnumTemperatureType {
    { "CELSIUS", 1 },
    { "FAHRENHEIT", 2 },
    { "KELVIN", 3 }
};

std::unordered_map<std::string, int32_t> I18nAddon::EnumWeekDay {
    { "MON", 1 },
    { "TUE", 2 },
    { "WED", 3 },
    { "THU", 4 },
    { "FRI", 5 },
    { "SAT", 6 },
    { "SUN", 7 }
};

I18nAddon::I18nAddon() : env_(nullptr) {}

I18nAddon::~I18nAddon()
{
    PhoneNumberFormat::CloseDynamicHandler();
}

void I18nAddon::Destructor(napi_env env, void *nativeObject, void *hint)
{
    if (!nativeObject) {
        return;
    }
    delete reinterpret_cast<I18nAddon *>(nativeObject);
    nativeObject = nullptr;
}

napi_value I18nAddon::InitI18nUtil(napi_env env, napi_value exports)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_STATIC_FUNCTION("unitConvert", UnitConvert),
        DECLARE_NAPI_STATIC_FUNCTION("getDateOrder", GetDateOrder),
        DECLARE_NAPI_STATIC_FUNCTION("getTimePeriodName", GetTimePeriodName),
        DECLARE_NAPI_STATIC_FUNCTION("getBestMatchLocale", GetBestMatchLocale),
        DECLARE_NAPI_STATIC_FUNCTION("getThreeLetterLanguage", GetThreeLetterLanguage),
        DECLARE_NAPI_STATIC_FUNCTION("getThreeLetterRegion", GetThreeLetterRegion),
        DECLARE_NAPI_STATIC_FUNCTION("getUnicodeWrappedFilePath", GetUnicodeWrappedFilePath)
    };
    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, "I18NUtil", NAPI_AUTO_LENGTH, JSUtils::DefaultConstructor, nullptr,
        sizeof(properties) / sizeof(napi_property_descriptor), properties, &constructor);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("InitI18nUtil: Define class failed when InitI18NUtil.");
        return nullptr;
    }

    status = napi_set_named_property(env, exports, "I18NUtil", constructor);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("InitI18nUtil: Set property failed when InitI18NUtil.");
        return nullptr;
    }
    return exports;
}

napi_value I18nAddon::Init(napi_env env, napi_value exports)
{
    napi_status initStatus = napi_ok;
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("getDisplayLanguage", I18nSystemAddon::GetDisplayLanguage),
        DECLARE_NAPI_FUNCTION("getDisplayCountry", I18nSystemAddon::GetDisplayCountry),
        DECLARE_NAPI_FUNCTION("getSystemLanguage", I18nSystemAddon::GetSystemLanguage),
        DECLARE_NAPI_FUNCTION("getSystemRegion", I18nSystemAddon::GetSystemRegion),
        DECLARE_NAPI_FUNCTION("getSystemLocale", I18nSystemAddon::GetSystemLocale),
        DECLARE_NAPI_FUNCTION("getCalendar", I18nCalendarAddon::GetCalendar),
        DECLARE_NAPI_FUNCTION("isRTL", IsRTL),
        DECLARE_NAPI_FUNCTION("getLineInstance", GetLineInstance),
        DECLARE_NAPI_FUNCTION("getInstance", GetIndexUtil),
        DECLARE_NAPI_FUNCTION("addPreferredLanguage", I18nSystemAddon::AddPreferredLanguage),
        DECLARE_NAPI_FUNCTION("removePreferredLanguage", I18nSystemAddon::RemovePreferredLanguage),
        DECLARE_NAPI_FUNCTION("getPreferredLanguageList", I18nSystemAddon::GetPreferredLanguageList),
        DECLARE_NAPI_FUNCTION("getFirstPreferredLanguage", I18nSystemAddon::GetFirstPreferredLanguage),
        DECLARE_NAPI_FUNCTION("getSimpleNumberFormatBySkeleton",
            SimpleNumberFormatAddon::GetSimpleNumberFormatBySkeleton),
        DECLARE_NAPI_FUNCTION("getSimpleDateTimeFormatByPattern",
            SimpleDateTimeFormatAddon::GetSimpleDateTimeFormatByPattern),
        DECLARE_NAPI_FUNCTION("getSimpleDateTimeFormatBySkeleton",
            SimpleDateTimeFormatAddon::GetSimpleDateTimeFormatBySkeleton),
        DECLARE_NAPI_FUNCTION("is24HourClock", I18nSystemAddon::Is24HourClock),
        DECLARE_NAPI_FUNCTION("set24HourClock", I18nSystemAddon::Set24HourClock),
        DECLARE_NAPI_FUNCTION("getTimeZone", I18nTimeZoneAddon::GetI18nTimeZone),
        DECLARE_NAPI_PROPERTY("NormalizerMode", I18nNormalizerAddon::CreateI18NNormalizerModeEnum(env)),
        DECLARE_NAPI_PROPERTY("TemperatureType", CreateTemperatureTypeEnum(env)),
        DECLARE_NAPI_PROPERTY("WeekDay", CreateWeekDayEnum(env))
    };
    initStatus = napi_define_properties(env, exports, sizeof(properties) / sizeof(napi_property_descriptor),
        properties);
    if (initStatus != napi_ok) {
        HILOG_ERROR_I18N("Failed to set properties at init");
        return nullptr;
    }
    return exports;
}

void GetOptionMap(napi_env env, napi_value option, std::map<std::string, std::string> &map)
{
    if (VariableConvertor::CheckNapiIsNull(env, option)) {
        size_t len;
        napi_get_value_string_utf8(env, option, nullptr, 0, &len);
        std::vector<char> styleBuf(len + 1);
        napi_status status = napi_get_value_string_utf8(env, option, styleBuf.data(), len + 1, &len);
        if (status != napi_ok) {
            HILOG_ERROR_I18N("GetOptionMap: Failed to get string item");
            return;
        }
        map.insert(std::make_pair("unitDisplay", styleBuf.data()));
    }
}

napi_value I18nAddon::UnitConvert(napi_env env, napi_callback_info info)
{
    size_t argc = 5;
    napi_value argv[5] = { 0 };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        return nullptr;
    }
    std::string fromUnit;
    VariableConvertor::GetOptionValue(env, argv[0], "unit", fromUnit);
    std::string fromMeasSys;
    VariableConvertor::GetOptionValue(env, argv[0], "measureSystem", fromMeasSys);
    std::string toUnit;
    VariableConvertor::GetOptionValue(env, argv[1], "unit", toUnit);
    std::string toMeasSys;
    VariableConvertor::GetOptionValue(env, argv[1], "measureSystem", toMeasSys);
    double number = 0;
    napi_get_value_double(env, argv[2], &number); // 2 is the index of value
    int convertStatus = Convert(number, fromUnit, fromMeasSys, toUnit, toMeasSys);
    size_t len;
    napi_get_value_string_utf8(env, argv[3], nullptr, 0, &len); // 3 is the index of value
    std::vector<char> localeBuf(len + 1);
    // 3 is the index of value
    status = napi_get_value_string_utf8(env, argv[3], localeBuf.data(), len + 1, &len);
    if (status != napi_ok) {
        return nullptr;
    }
    std::vector<std::string> localeTags;
    localeTags.push_back(localeBuf.data());
    std::map<std::string, std::string> map = {};
    map.insert(std::make_pair("style", "unit"));
    if (!convertStatus) {
        map.insert(std::make_pair("unit", fromUnit));
    } else {
        map.insert(std::make_pair("unit", toUnit));
    }
    // 4 is the index of value
    GetOptionMap(env, argv[4], map);
    std::unique_ptr<NumberFormat> numberFmt = nullptr;
    numberFmt = std::make_unique<NumberFormat>(localeTags, map);
    std::string value = numberFmt->Format(number);
    napi_value result;
    status = napi_create_string_utf8(env, value.c_str(), NAPI_AUTO_LENGTH, &result);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("UnitConvert: Failed to create string item");
        return nullptr;
    }
    return result;
}

napi_value I18nAddon::GetDateOrder(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = { 0 };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        return nullptr;
    }
    size_t len = 0;
    napi_get_value_string_utf8(env, argv[0], nullptr, 0, &len);
    std::vector<char> languageBuf(len + 1);
    status = napi_get_value_string_utf8(env, argv[0], languageBuf.data(), len + 1, &len);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("Failed to get locale string for GetDateOrder");
        return nullptr;
    }
    std::string languageTag = languageBuf.data();
    std::string value = DateTimeSequence::GetDateOrder(languageTag);
    napi_value result;
    status = napi_create_string_utf8(env, value.c_str(), NAPI_AUTO_LENGTH, &result);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("GetDateOrder Failed to create string item");
        return nullptr;
    }
    return result;
}

napi_value I18nAddon::GetTimePeriodName(napi_env env, napi_callback_info info)
{
    napi_value result;
    int32_t hour;
    std::string localeTag;
    if (GetParamOfGetTimePeriodName(env, info, localeTag, hour) == -1) {
        HILOG_ERROR_I18N("GetTimePeriodName param error");
        napi_create_string_utf8(env, "", NAPI_AUTO_LENGTH, &result);
        return result;
    }

    UErrorCode icuStatus = U_ZERO_ERROR;
    icu::Locale locale = icu::Locale::forLanguageTag(localeTag.data(), icuStatus);
    if (U_FAILURE(icuStatus) || !IsValidLocaleTag(locale)) {
        ErrorUtil::NapiThrow(env, I18N_NOT_VALID, "locale", "a valid locale", true);
        return nullptr;
    }
    icu::SimpleDateFormat* formatter = dynamic_cast<icu::SimpleDateFormat*>
        (icu::DateFormat::createDateInstance(icu::DateFormat::EStyle::kDefault, locale));
    if (!formatter) {
        HILOG_ERROR_I18N("GetTimePeriodName Failed to create SimpleDateFormat");
        napi_create_string_utf8(env, "", NAPI_AUTO_LENGTH, &result);
        return result;
    }
    formatter->applyPattern("B");

    std::string temp;
    icu::UnicodeString name;
    icu::Calendar *calendar = icu::Calendar::createInstance(locale, icuStatus);
    calendar->set(UCalendarDateFields::UCAL_HOUR_OF_DAY, hour);
    formatter->format(calendar->getTime(icuStatus), name);
    name.toUTF8String(temp);
    napi_create_string_utf8(env, PseudoLocalizationProcessor(temp).c_str(), NAPI_AUTO_LENGTH, &result);
    delete formatter;
    delete calendar;
    return result;
}

int I18nAddon::GetParamOfGetTimePeriodName(napi_env env, napi_callback_info info, std::string &tag, int32_t &hour)
{
    size_t argc = 2;
    napi_value argv[2] = { 0 };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("GetTimePeriodName can't get parameters from getTimePerioudName.");
        return -1;
    } else if (argc < 1) {
        ErrorUtil::NapiNotFoundError(env, I18N_NOT_FOUND, "hour", true);
        return -1;
    }

    napi_valuetype valueType = napi_valuetype::napi_undefined;
    napi_typeof(env, argv[0], &valueType);
    if (valueType != napi_valuetype::napi_number) {
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, "hour", "number", true);
        return -1;
    }
    status = napi_get_value_int32(env, argv[0], &hour);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("GetTimePeriodName can't get number from js param");
        return -1;
    }

    valueType = napi_valuetype::napi_undefined;
    napi_typeof(env, argv[1], &valueType);
    if (valueType == napi_valuetype::napi_null || valueType == napi_valuetype::napi_undefined) {
        tag = LocaleConfig::GetSystemLocale();
    } else if (valueType == napi_valuetype::napi_string) {
        int code = 0;
        tag = VariableConvertor::GetString(env, argv[1], code);
        if (code) {
            HILOG_ERROR_I18N("GetTimePeriodName can't get string from js param");
            return -1;
        }
    } else {
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, "locale", "string", true);
        return -1;
    }
    return 0;
}

LocaleInfo* ProcessJsParamLocale(napi_env env, napi_value argv)
{
    int32_t code = 0;
    std::string localeTag = VariableConvertor::GetString(env, argv, code);
    if (code != 0) {
        HILOG_ERROR_I18N("ProcessJsParamLocale: Failed to obtain the parameter.");
        return nullptr;
    }
    UErrorCode icuStatus = U_ZERO_ERROR;
    icu::Locale locale = icu::Locale::forLanguageTag(localeTag.data(), icuStatus);
    if (U_FAILURE(icuStatus) || !IsValidLocaleTag(locale)) {
        ErrorUtil::NapiThrow(env, I18N_NOT_VALID, "locale", "a valid locale", true);
        return nullptr;
    }
    return new LocaleInfo(localeTag);
}

bool ProcessJsParamLocaleList(napi_env env, napi_value argv, std::vector<LocaleInfo*> &candidateLocales,
    LocaleInfo *requestLocale)
{
    std::vector<std::string> localeTagList;
    if (!VariableConvertor::GetStringArrayFromJsParam(env, argv, "localeList", localeTagList)) {
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
            HILOG_ERROR_I18N("GetBestMatchLocale param localeList Invalid: %{public}s.", it->data());
            ErrorUtil::NapiThrow(env, I18N_NOT_VALID, "locale of localeList", "a valid locale", true);
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

void ReleaseParam(LocaleInfo *locale, std::vector<LocaleInfo*> &candidateLocales)
{
    delete locale;
    for (auto it = candidateLocales.begin(); it != candidateLocales.end(); ++it) {
        delete *it;
    }
}

napi_value I18nAddon::GetBestMatchLocale(napi_env env, napi_callback_info info)
{
    size_t argc = 2;
    napi_value argv[2] = { nullptr };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok || argc < 2) { // 2 is the request param num.
        ErrorUtil::NapiNotFoundError(env, I18N_NOT_FOUND, "locale or localeList", true);
        return nullptr;
    }
    LocaleInfo *requestLocale = ProcessJsParamLocale(env, argv[0]);
    if (requestLocale == nullptr) {
        return nullptr;
    }
    std::vector<LocaleInfo*> candidateLocales;
    bool isValidParam = ProcessJsParamLocaleList(env, argv[1], candidateLocales, requestLocale);
    if (!isValidParam) {
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
    napi_value result = nullptr;
    status = napi_create_string_utf8(env, bestMatchLocaleTag.c_str(), NAPI_AUTO_LENGTH, &result);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("Create format stirng failed.");
        return nullptr;
    }
    return result;
}

napi_value I18nAddon::GetThreeLetterLanguage(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = { 0 };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("GetThreeLetterLanguage napi get param error.");
        return nullptr;
    } else if (argc < 1) {
        ErrorUtil::NapiNotFoundError(env, I18N_NOT_FOUND, "locale", true);
        return nullptr;
    }

    napi_valuetype valueType = napi_valuetype::napi_undefined;
    napi_typeof(env, argv[0], &valueType);
    if (valueType != napi_valuetype::napi_string) {
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, "locale", "string", true);
        return nullptr;
    }

    int32_t code = 0;
    std::string languageTag = VariableConvertor::GetString(env, argv[0], code);
    if (code != 0) {
        HILOG_ERROR_I18N("GetThreeLetterLanguage: Failed to obtain the parameter.");
        return nullptr;
    }

    std::string language = GetISO3Language(languageTag);

    napi_value result;
    status = napi_create_string_utf8(env, language.c_str(), NAPI_AUTO_LENGTH, &result);
    if (status != napi_ok || language.empty()) {
        HILOG_ERROR_I18N("GetThreeLetterLanguage create string fail or empty");
        ErrorUtil::NapiThrow(env, I18N_NOT_VALID, "locale", "a valid locale", true);
        return nullptr;
    }
    return result;
}

napi_value I18nAddon::GetThreeLetterRegion(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = { 0 };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("GetThreeLetterRegion: Failed to obtain the parameter.");
        return nullptr;
    } else if (argc < 1) {
        ErrorUtil::NapiNotFoundError(env, I18N_NOT_FOUND, "locale", true);
        return nullptr;
    }

    napi_valuetype valueType = napi_valuetype::napi_undefined;
    napi_typeof(env, argv[0], &valueType);
    if (valueType != napi_valuetype::napi_string) {
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, "locale", "string", true);
        return nullptr;
    }

    int32_t code = 0;
    std::string regionTag = VariableConvertor::GetString(env, argv[0], code);
    if (code != 0) {
        HILOG_ERROR_I18N("GetThreeLetterRegion: Failed to obtain the parameter.");
        return nullptr;
    }

    std::string country = GetISO3Country(regionTag);

    napi_value result;
    status = napi_create_string_utf8(env, country.c_str(), NAPI_AUTO_LENGTH, &result);
    if (status != napi_ok || country.empty()) {
        HILOG_ERROR_I18N("GetThreeLetterRegion create string fail or empty");
        ErrorUtil::NapiThrow(env, I18N_NOT_VALID, "locale", "a valid locale", true);
        return nullptr;
    }
    return result;
}

napi_value I18nAddon::InitI18nTransliterator(napi_env env, napi_value exports)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("transform", Transform),
    };
    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, "Transliterator", NAPI_AUTO_LENGTH, I18nTransliteratorConstructor,
        nullptr, sizeof(properties) / sizeof(napi_property_descriptor), properties, &constructor);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("InitI18nTransliterator: Failed to define transliterator class at Init");
        return nullptr;
    }
    exports = I18nAddon::InitTransliterator(env, exports);
    g_transConstructor = new (std::nothrow) napi_ref;
    if (!g_transConstructor) {
        HILOG_ERROR_I18N("InitI18nTransliterator: Failed to create trans ref at init");
        return nullptr;
    }
    status = napi_create_reference(env, constructor, 1, g_transConstructor);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("InitI18nTransliterator: Failed to create trans reference at init");
        return nullptr;
    }
    return exports;
}

napi_value I18nAddon::InitTransliterator(napi_env env, napi_value exports)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_STATIC_FUNCTION("getAvailableIDs", GetAvailableIDs),
        DECLARE_NAPI_STATIC_FUNCTION("getInstance", GetTransliteratorInstance)
    };
    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, "I18nTransliterator", NAPI_AUTO_LENGTH, JSUtils::DefaultConstructor,
        nullptr, sizeof(properties) / sizeof(napi_property_descriptor), properties, &constructor);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("InitTransliterator: Failed to define class Transliterator.");
        return nullptr;
    }
    status = napi_set_named_property(env, exports, "Transliterator", constructor);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("InitTransliterator: Set property failed When InitTransliterator.");
        return nullptr;
    }
    return exports;
}

napi_value I18nAddon::I18nTransliteratorConstructor(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = { 0 };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        return nullptr;
    }
    napi_valuetype valueType = napi_valuetype::napi_undefined;
    napi_typeof(env, argv[0], &valueType);
    if (valueType != napi_valuetype::napi_string) {
        HILOG_ERROR_I18N("I18nTransliteratorConstructor: Parameter type does not match");
        return nullptr;
    }
    int32_t code = 0;
    std::string idTag = VariableConvertor::GetString(env, argv[0], code);
    if (code) {
        return nullptr;
    }
    std::unique_ptr<I18nAddon> obj = nullptr;
    obj = std::make_unique<I18nAddon>();
    status =
        napi_wrap(env, thisVar, reinterpret_cast<void *>(obj.get()), I18nAddon::Destructor, nullptr, nullptr);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("I18nTransliteratorConstructor: TransliteratorConstructor: Wrap II18nAddon failed");
        return nullptr;
    }
    if (!obj->InitTransliteratorContext(env, info, idTag)) {
        obj.release();
        return nullptr;
    }
    obj.release();
    return thisVar;
}

bool I18nAddon::InitTransliteratorContext(napi_env env, napi_callback_info info, const std::string &idTag)
{
    UErrorCode status = U_ZERO_ERROR;
    icu::UnicodeString unistr = icu::UnicodeString::fromUTF8(idTag);
    icu::Transliterator *trans = icu::Transliterator::createInstance(unistr, UTransDirection::UTRANS_FORWARD, status);
    if (U_FAILURE(status) || (trans == nullptr)) {
        return false;
    }
    transliterator_ = std::unique_ptr<icu::Transliterator>(trans);
    return transliterator_ != nullptr;
}

napi_value I18nAddon::Transform(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = { nullptr };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    I18nAddon *obj = nullptr;
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->transliterator_) {
        HILOG_ERROR_I18N("Get Transliterator object failed");
        return nullptr;
    }
    if (!argv[0]) {
        return nullptr;
    }
    napi_valuetype valueType = napi_valuetype::napi_undefined;
    napi_typeof(env, argv[0], &valueType);
    if (valueType != napi_valuetype::napi_string) {
        HILOG_ERROR_I18N("Transform: Parameter type does not match");
        return nullptr;
    }
    size_t len = 0;
    status = napi_get_value_string_utf8(env, argv[0], nullptr, 0, &len);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("Transform: Get field length failed napi_get_value_string_utf8");
        return nullptr;
    }
    std::vector<char> buf(len + 1);
    status = napi_get_value_string_utf8(env, argv[0], buf.data(), len + 1, &len);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("Transform: Get string value failed");
        return nullptr;
    }
    icu::UnicodeString unistr = icu::UnicodeString::fromUTF8(buf.data());
    obj->transliterator_->transliterate(unistr);
    std::string temp;
    unistr.toUTF8String(temp);
    napi_value value;
    status = napi_create_string_utf8(env, temp.c_str(), NAPI_AUTO_LENGTH, &value);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("Transform: Get field length failed napi_create_string_utf8");
        return nullptr;
    }
    return value;
}

napi_value I18nAddon::GetAvailableIDs(napi_env env, napi_callback_info info)
{
    size_t argc = 0;
    napi_value *argv = nullptr;
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        return nullptr;
    }
    UErrorCode icuStatus = U_ZERO_ERROR;
    icu::StringEnumeration *strenum = icu::Transliterator::getAvailableIDs(icuStatus);
    if (U_FAILURE(icuStatus)) {
        HILOG_ERROR_I18N("Failed to get available ids");
        if (strenum) {
            delete strenum;
        }
        return nullptr;
    }

    napi_value result = nullptr;
    napi_create_array(env, &result);
    uint32_t i = 0;
    const char *temp = nullptr;
    if (strenum == nullptr) {
        return nullptr;
    }
    while ((temp = strenum->next(nullptr, icuStatus)) != nullptr) {
        if (U_FAILURE(icuStatus)) {
            break;
        }
        napi_value val = nullptr;
        napi_create_string_utf8(env, temp, strlen(temp), &val);
        napi_set_element(env, result, i, val);
        ++i;
    }
    delete strenum;
    return result;
}

napi_value I18nAddon::GetTransliteratorInstance(napi_env env, napi_callback_info info)
{
    size_t argc = 1; // retrieve 2 arguments
    napi_value argv[1] = { 0 };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    napi_value constructor = nullptr;
    napi_status status = napi_get_reference_value(env, *g_transConstructor, &constructor);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("Failed to create reference at GetCalendar");
        return nullptr;
    }
    napi_value result = nullptr;
    status = napi_new_instance(env, constructor, 1, argv, &result); // 2 arguments
    if (status != napi_ok) {
        HILOG_ERROR_I18N("Get Transliterator create instance failed");
        return nullptr;
    }
    return result;
}

napi_value I18nAddon::IsRTL(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = { 0 };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        return nullptr;
    }
    size_t len = 0;
    napi_get_value_string_utf8(env, argv[0], nullptr, 0, &len);
    std::vector<char> localeBuf(len + 1);
    status = napi_get_value_string_utf8(env, argv[0], localeBuf.data(), len + 1, &len);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("IsRTL: Failed to get string item");
        return nullptr;
    }
    bool isRTL = LocaleConfig::IsRTL(localeBuf.data());
    napi_value result = nullptr;
    status = napi_get_boolean(env, isRTL, &result);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("IsRTL failed");
        return nullptr;
    }
    return result;
}

napi_value I18nAddon::GetUnicodeWrappedFilePath(napi_env env, napi_callback_info info)
{
    size_t argc = 3;
    napi_value argv[3] = { 0 };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("GetUnicodeWrappedFilePath: Get param info failed");
        return nullptr;
    }
    if (argc < 1) {
        ErrorUtil::NapiNotFoundError(env, I18N_NOT_FOUND, "path", true);
        return nullptr;
    }
    VariableConvertor::VerifyType(env, "path", napi_valuetype::napi_string, argv[0]);
    int32_t code = 0;
    std::string path = VariableConvertor::GetString(env, argv[0], code);
    if (code) {
        HILOG_ERROR_I18N("GetUnicodeWrappedFilePath: Get param string argv[0] failed");
        return nullptr;
    }
    char delimiter = PATH_SEPARATOR;
    if (argc >= 2) { // 2 is parameter count
        VariableConvertor::VerifyType(env, "delimiter", napi_valuetype::napi_string, argv[1]);
        delimiter = GetDelimiter(env, argv[1]);
        if (delimiter == '\0') {
            HILOG_ERROR_I18N("GetUnicodeWrappedFilePath: Second param is empty");
            return nullptr;
        }
    }
    LocaleInfoAddon *obj = nullptr;
    if (argc == 3) { // 3 is parameter count
        napi_status status = napi_unwrap(env,
            argv[2], reinterpret_cast<void **>(&obj)); // 2 is the last parameter index
        if (status != napi_ok || !obj || !obj->GetLocaleInfo()) {
            HILOG_ERROR_I18N("GetUnicodeWrappedFilePath: Get Locale object failed");
            return nullptr;
        }
    }
    std::string errorCode;
    std::shared_ptr<LocaleInfo> localeInfoPtr = obj == nullptr ? nullptr : obj->GetLocaleInfo();
    std::string result = LocaleConfig::GetUnicodeWrappedFilePath(path, delimiter,
        localeInfoPtr, errorCode);
    if (!errorCode.empty()) {
        ErrorUtil::NapiThrow(env, I18N_NOT_VALID, errorCode, "valid", true);
        return nullptr;
    }
    return VariableConvertor::CreateString(env, result);
}

char I18nAddon::GetDelimiter(napi_env env, napi_value argVal)
{
    int32_t code = 0;
    std::string result = VariableConvertor::GetString(env, argVal, code);
    if (code) {
        HILOG_ERROR_I18N("GetDelimiter: Get string failed");
        return '\0';
    }
    if (result.length() != 1) {
        ErrorUtil::NapiThrow(env, I18N_NOT_VALID, "delimiter", "a valid delimiter", true);
        return '\0';
    }
    return result.at(0);
}

napi_value I18nAddon::InitPhoneNumberFormat(napi_env env, napi_value exports)
{
    napi_status status = napi_ok;
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("isValidNumber", IsValidPhoneNumber),
        DECLARE_NAPI_FUNCTION("format", FormatPhoneNumber),
        DECLARE_NAPI_FUNCTION("getLocationName", GetLocationName)
    };

    napi_value constructor;
    status = napi_define_class(env, "PhoneNumberFormat", NAPI_AUTO_LENGTH, PhoneNumberFormatConstructor, nullptr,
                               sizeof(properties) / sizeof(napi_property_descriptor), properties, &constructor);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("InitPhoneNumberFormat: Define class failed when InitPhoneNumberFormat");
        return nullptr;
    }

    status = napi_set_named_property(env, exports, "PhoneNumberFormat", constructor);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("Set property failed when InitPhoneNumberFormat");
        return nullptr;
    }
    return exports;
}

napi_value I18nAddon::PhoneNumberFormatConstructor(napi_env env, napi_callback_info info)
{
    size_t argc = 2;
    napi_value argv[2] = { 0 };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        return nullptr;
    }
    napi_valuetype valueType = napi_valuetype::napi_undefined;
    napi_typeof(env, argv[0], &valueType);
    if (valueType != napi_valuetype::napi_string) {
        HILOG_ERROR_I18N("PhoneNumberFormatConstructor: Parameter type does not match");
        return nullptr;
    }
    size_t len = 0;
    status = napi_get_value_string_utf8(env, argv[0], nullptr, 0, &len);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("PhoneNumberFormatConstructor: Get country tag length failed");
        return nullptr;
    }
    std::vector<char> country (len + 1);
    status = napi_get_value_string_utf8(env, argv[0], country.data(), len + 1, &len);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("Get country tag failed");
        return nullptr;
    }
    std::map<std::string, std::string> options;
    std::string typeStr;
    VariableConvertor::GetOptionValue(env, argv[1], "type", typeStr);
    options.insert(std::make_pair("type", typeStr));
    std::unique_ptr<I18nAddon> obj = nullptr;
    obj = std::make_unique<I18nAddon>();
    status = napi_wrap(env, thisVar, reinterpret_cast<void *>(obj.get()),
                       I18nAddon::Destructor, nullptr, nullptr);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("PhoneNumberFormatConstructor: Wrap I18nAddon failed");
        return nullptr;
    }
    if (!obj->InitPhoneNumberFormatContext(env, info, country.data(), options)) {
        return nullptr;
    }
    obj.release();
    return thisVar;
}

bool I18nAddon::InitPhoneNumberFormatContext(napi_env env, napi_callback_info info, const std::string &country,
                                             const std::map<std::string, std::string> &options)
{
    napi_value global = nullptr;
    napi_status status = napi_get_global(env, &global);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("InitPhoneNumberFormatContext: Get global failed");
        return false;
    }
    env_ = env;
    phonenumberfmt_ = PhoneNumberFormat::CreateInstance(country, options);

    return phonenumberfmt_ != nullptr;
}

napi_value I18nAddon::IsValidPhoneNumber(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = { 0 };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    napi_valuetype valueType = napi_valuetype::napi_undefined;
    napi_typeof(env, argv[0], &valueType);
    if (valueType != napi_valuetype::napi_string) {
        HILOG_ERROR_I18N("IsValidPhoneNumber: Parameter type does not match");
        return nullptr;
    }

    size_t len = 0;
    napi_status status = napi_get_value_string_utf8(env, argv[0], nullptr, 0, &len);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("IsValidPhoneNumber: Get phone number length failed");
        return nullptr;
    }
    std::vector<char> buf(len + 1);
    status = napi_get_value_string_utf8(env, argv[0], buf.data(), len + 1, &len);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("IsValidPhoneNumber: Get phone number failed");
        return nullptr;
    }

    I18nAddon *obj = nullptr;
    status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->phonenumberfmt_) {
        HILOG_ERROR_I18N("IsValidPhoneNumber: GetPhoneNumberFormat object failed");
        return nullptr;
    }

    bool isValid = obj->phonenumberfmt_->isValidPhoneNumber(buf.data());

    napi_value result = nullptr;
    status = napi_get_boolean(env, isValid, &result);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("IsValidPhoneNumber: Create boolean failed");
        return nullptr;
    }

    return result;
}

napi_value I18nAddon::GetLocationName(napi_env env, napi_callback_info info)
{
    size_t argc = 2;
    napi_value argv[2] = {0, 0};
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);

    int32_t code = 0;
    std::string number = VariableConvertor::GetString(env, argv[0], code);
    if (code) {
        return nullptr;
    }
    std::string language = VariableConvertor::GetString(env, argv[1], code);
    if (code) {
        return nullptr;
    }

    I18nAddon *obj = nullptr;
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->phonenumberfmt_) {
        HILOG_ERROR_I18N("GetLocationName: GetPhoneNumberFormat object failed");
        return nullptr;
    }

    std::string resStr = obj->phonenumberfmt_->getLocationName(number.data(), language.data());
    napi_value result = nullptr;
    status = napi_create_string_utf8(env, resStr.c_str(), NAPI_AUTO_LENGTH, &result);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("Create result string failed");
        return nullptr;
    }

    return result;
}

napi_value I18nAddon::FormatPhoneNumber(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = { 0 };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    napi_valuetype valueType = napi_valuetype::napi_undefined;
    napi_typeof(env, argv[0], &valueType);
    if (valueType != napi_valuetype::napi_string) {
        HILOG_ERROR_I18N("FormatPhoneNumber: Parameter type does not match");
        return nullptr;
    }

    size_t len = 0;
    napi_status status = napi_get_value_string_utf8(env, argv[0], nullptr, 0, &len);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("FormatPhoneNumber: Get phone number length failed");
        return nullptr;
    }
    std::vector<char> buf(len + 1);
    status = napi_get_value_string_utf8(env, argv[0], buf.data(), len + 1, &len);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("FormatPhoneNumber: Get phone number failed");
        return nullptr;
    }

    I18nAddon *obj = nullptr;
    status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->phonenumberfmt_) {
        HILOG_ERROR_I18N("Get PhoneNumberFormat object failed");
        return nullptr;
    }

    std::string formattedPhoneNumber = obj->phonenumberfmt_->format(buf.data());

    napi_value result = nullptr;
    status = napi_create_string_utf8(env, formattedPhoneNumber.c_str(), NAPI_AUTO_LENGTH, &result);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("Create format phone number failed");
        return nullptr;
    }
    return result;
}

napi_value I18nAddon::InitI18nIndexUtil(napi_env env, napi_value exports)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("getIndexList", GetIndexList),
        DECLARE_NAPI_FUNCTION("addLocale", AddLocale),
        DECLARE_NAPI_FUNCTION("getIndex", GetIndex)
    };

    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, "IndexUtil", NAPI_AUTO_LENGTH, I18nIndexUtilConstructor, nullptr,
        sizeof(properties) / sizeof(napi_property_descriptor), properties, &constructor);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("InitI18nIndexUtil: Define class failed when InitI18nIndexUtil.");
        return nullptr;
    }
    exports = I18nAddon::InitIndexUtil(env, exports);
    status = napi_create_reference(env, constructor, 1, &g_indexUtilConstructor);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("InitI18nIndexUtil: Failed to create reference at init.");
        return nullptr;
    }
    return exports;
}

napi_value I18nAddon::InitIndexUtil(napi_env env, napi_value exports)
{
    napi_property_descriptor properties[] = {};
    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, "I18nIndexUtil", NAPI_AUTO_LENGTH, JSUtils::DefaultConstructor,
        nullptr, sizeof(properties) / sizeof(napi_property_descriptor), properties, &constructor);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("InitIndexUtil: Failed to define class IndexUtil.");
        return nullptr;
    }
    status = napi_set_named_property(env, exports, "IndexUtil", constructor);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("InitIndexUtil: Set property failed When InitIndexUtil.");
        return nullptr;
    }
    return exports;
}

napi_value I18nAddon::I18nBreakIteratorConstructor(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = { nullptr };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        return nullptr;
    }
    napi_valuetype valueType = napi_valuetype::napi_undefined;
    napi_typeof(env, argv[0], &valueType);
    if (valueType != napi_valuetype::napi_string) {
        HILOG_ERROR_I18N("BreakIteratorConstructor: Parameter type does not match");
        return nullptr;
    }
    int32_t code = 0;
    std::string localeTag = VariableConvertor::GetString(env, argv[0], code);
    if (code) {
        return nullptr;
    }
    std::unique_ptr<I18nAddon> obj = nullptr;
    obj = std::make_unique<I18nAddon>();
    status =
        napi_wrap(env, thisVar, reinterpret_cast<void *>(obj.get()), I18nAddon::Destructor, nullptr, nullptr);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("BreakIteratorConstructor: Wrap II18nAddon failed");
        return nullptr;
    }
    obj->brkiter_ = std::make_unique<I18nBreakIterator>(localeTag);
    if (!obj->brkiter_) {
        HILOG_ERROR_I18N("Wrap BreakIterator failed");
        return nullptr;
    }
    obj.release();
    return thisVar;
}

napi_value I18nAddon::InitI18nBreakIterator(napi_env env, napi_value exports)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("current", Current),
        DECLARE_NAPI_FUNCTION("first", First),
        DECLARE_NAPI_FUNCTION("last", Last),
        DECLARE_NAPI_FUNCTION("next", Next),
        DECLARE_NAPI_FUNCTION("previous", Previous),
        DECLARE_NAPI_FUNCTION("setLineBreakText", SetText),
        DECLARE_NAPI_FUNCTION("following", Following),
        DECLARE_NAPI_FUNCTION("getLineBreakText", GetText),
        DECLARE_NAPI_FUNCTION("isBoundary", IsBoundary)
    };
    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, "BreakIterator", NAPI_AUTO_LENGTH, I18nBreakIteratorConstructor,
        nullptr, sizeof(properties) / sizeof(napi_property_descriptor), properties, &constructor);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("InitI18nBreakIterator: Failed to define class BreakIterator at Init");
        return nullptr;
    }
    exports = I18nAddon::InitBreakIterator(env, exports);
    g_brkConstructor = new (std::nothrow) napi_ref;
    if (!g_brkConstructor) {
        HILOG_ERROR_I18N("InitI18nBreakIterator: Failed to create brkiterator ref at init");
        return nullptr;
    }
    status = napi_create_reference(env, constructor, 1, g_brkConstructor);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("InitI18nBreakIterator: Failed to create reference g_brkConstructor at init");
        return nullptr;
    }
    return exports;
}

napi_value I18nAddon::InitBreakIterator(napi_env env, napi_value exports)
{
    napi_property_descriptor properties[] = {};
    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, "I18nBreakIterator", NAPI_AUTO_LENGTH, JSUtils::DefaultConstructor,
        nullptr, sizeof(properties) / sizeof(napi_property_descriptor), properties, &constructor);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("InitBreakIterator: Failed to define class BreakIterator.");
        return nullptr;
    }
    status = napi_set_named_property(env, exports, "BreakIterator", constructor);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("InitBreakIterator: Set property failed When InitBreakIterator.");
        return nullptr;
    }
    return exports;
}

napi_value I18nAddon::GetLineInstance(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = { nullptr };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    napi_value constructor = nullptr;
    napi_status status = napi_get_reference_value(env, *g_brkConstructor, &constructor);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("Failed to create reference at GetLineInstance");
        return nullptr;
    }
    if (!argv[0]) {
        return nullptr;
    }
    napi_value result = nullptr;
    status = napi_new_instance(env, constructor, 1, argv, &result); // 1 arguments
    if (status != napi_ok) {
        HILOG_ERROR_I18N("GetLineInstance create instance failed");
        return nullptr;
    }
    return result;
}

napi_value I18nAddon::Current(napi_env env, napi_callback_info info)
{
    size_t argc = 0;
    napi_value *argv = nullptr;
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    I18nAddon *obj = nullptr;
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->brkiter_) {
        HILOG_ERROR_I18N("Current: Get BreakIterator object failed");
        return nullptr;
    }
    int value = obj->brkiter_->Current();
    napi_value result = nullptr;
    status = napi_create_int32(env, value, &result);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("Current: Create int32_t value failed");
        return nullptr;
    }
    return result;
}

napi_value I18nAddon::First(napi_env env, napi_callback_info info)
{
    size_t argc = 0;
    napi_value *argv = nullptr;
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    I18nAddon *obj = nullptr;
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->brkiter_) {
        HILOG_ERROR_I18N("First: Get BreakIterator object failed");
        return nullptr;
    }
    int value = obj->brkiter_->First();
    napi_value result = nullptr;
    status = napi_create_int32(env, value, &result);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("First: Create int32_t value failed");
        return nullptr;
    }
    return result;
}

napi_value I18nAddon::Last(napi_env env, napi_callback_info info)
{
    size_t argc = 0;
    napi_value *argv = nullptr;
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    I18nAddon *obj = nullptr;
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->brkiter_) {
        HILOG_ERROR_I18N("Last: Get BreakIterator object failed");
        return nullptr;
    }
    int value = obj->brkiter_->Last();
    napi_value result = nullptr;
    status = napi_create_int32(env, value, &result);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("Last: Create int32_t value failed");
        return nullptr;
    }
    return result;
}

napi_value I18nAddon::Previous(napi_env env, napi_callback_info info)
{
    size_t argc = 0;
    napi_value *argv = nullptr;
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    I18nAddon *obj = nullptr;
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->brkiter_) {
        HILOG_ERROR_I18N("Previous: Get BreakIterator object failed");
        return nullptr;
    }
    int value = obj->brkiter_->Previous();
    napi_value result = nullptr;
    status = napi_create_int32(env, value, &result);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("Previous: Create int32_t value failed");
        return nullptr;
    }
    return result;
}

napi_value I18nAddon::Next(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = { nullptr };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    I18nAddon *obj = nullptr;
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->brkiter_) {
        HILOG_ERROR_I18N("Next: Get BreakIterator object failed");
        return nullptr;
    }
    int value = 1;
    if (VariableConvertor::CheckNapiIsNull(env, argv[0])) {
        napi_valuetype valueType = napi_valuetype::napi_undefined;
        napi_typeof(env, argv[0], &valueType);
        if (valueType != napi_valuetype::napi_number) {
            HILOG_ERROR_I18N("Next: Parameter type does not match");
            return nullptr;
        }
        status = napi_get_value_int32(env, argv[0], &value);
        if (status != napi_ok) {
            HILOG_ERROR_I18N("Next: Retrieve next value failed");
            return nullptr;
        }
    }
    value = obj->brkiter_->Next(value);
    napi_value result = nullptr;
    status = napi_create_int32(env, value, &result);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("Next: Create int32_t value failed");
        return nullptr;
    }
    return result;
}

napi_value I18nAddon::SetText(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = { nullptr };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    I18nAddon *obj = nullptr;
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->brkiter_) {
        HILOG_ERROR_I18N("SetText: Get BreakIterator object failed");
        return nullptr;
    }
    if (!argv[0]) {
        return nullptr;
    }
    napi_valuetype valueType = napi_valuetype::napi_undefined;
    napi_typeof(env, argv[0], &valueType);
    if (valueType != napi_valuetype::napi_string) {
        HILOG_ERROR_I18N("SetText: Parameter type does not match");
        return nullptr;
    }
    size_t len = 0;
    status = napi_get_value_string_utf8(env, argv[0], nullptr, 0, &len);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("SetText: Get field length failed");
        return nullptr;
    }
    std::vector<char> buf(len + 1);
    status = napi_get_value_string_utf8(env, argv[0], buf.data(), len + 1, &len);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("SetText: Get string value failed");
        return nullptr;
    }
    obj->brkiter_->SetText(buf.data());
    return nullptr;
}

napi_value I18nAddon::GetText(napi_env env, napi_callback_info info)
{
    size_t argc = 0;
    napi_value *argv = nullptr;
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    I18nAddon *obj = nullptr;
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->brkiter_) {
        HILOG_ERROR_I18N("GetText: Get BreakIterator object failed");
        return nullptr;
    }
    napi_value value = nullptr;
    std::string temp;
    obj->brkiter_->GetText(temp);
    status = napi_create_string_utf8(env, temp.c_str(), NAPI_AUTO_LENGTH, &value);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("GetText: Get field length failed");
        return nullptr;
    }
    return value;
}

napi_value I18nAddon::Following(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = { nullptr };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    I18nAddon *obj = nullptr;
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->brkiter_) {
        HILOG_ERROR_I18N("Following: Get BreakIterator object failed");
        return nullptr;
    }
    if (!argv[0]) {
        return nullptr;
    }
    napi_valuetype valueType = napi_valuetype::napi_undefined;
    napi_typeof(env, argv[0], &valueType);
    if (valueType != napi_valuetype::napi_number) {
        HILOG_ERROR_I18N("Following: Parameter type does not match");
        return nullptr;
    }
    int value;
    status = napi_get_value_int32(env, argv[0], &value);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("Following: Retrieve following value failed");
        return nullptr;
    }
    value = obj->brkiter_->Following(value);
    napi_value result = nullptr;
    status = napi_create_int32(env, value, &result);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("Following: Create int32_t value failed");
        return nullptr;
    }
    return result;
}

napi_value I18nAddon::IsBoundary(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = { nullptr };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    I18nAddon *obj = nullptr;
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->brkiter_) {
        HILOG_ERROR_I18N("IsBoundary: Get BreakIterator object failed");
        return nullptr;
    }
    if (!argv[0]) {
        return nullptr;
    }
    napi_valuetype valueType = napi_valuetype::napi_undefined;
    int value;
    napi_typeof(env, argv[0], &valueType);
    if (valueType != napi_valuetype::napi_number) {
        HILOG_ERROR_I18N("IsBoundary: Parameter type does not match");
        return nullptr;
    }
    status = napi_get_value_int32(env, argv[0], &value);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("IsBoundary: Retrieve following value failed");
        return nullptr;
    }
    bool boundary = obj->brkiter_->IsBoundary(value);
    napi_value result = nullptr;
    status = napi_get_boolean(env, boundary, &result);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("IsBoundary: Create boolean failed");
        return nullptr;
    }
    return result;
}

napi_value I18nAddon::I18nIndexUtilConstructor(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = { nullptr };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        return nullptr;
    }
    std::string localeTag = "";
    if (argc > 0) {
        napi_valuetype valueType = napi_valuetype::napi_undefined;
        napi_typeof(env, argv[0], &valueType);
        if (valueType != napi_valuetype::napi_string) {
            HILOG_ERROR_I18N("IndexUtilConstructor: Parameter type does not match");
            return nullptr;
        }
        size_t len = 0;
        status = napi_get_value_string_utf8(env, argv[0], nullptr, 0, &len);
        if (status != napi_ok) {
            HILOG_ERROR_I18N("IndexUtilConstructor: Get locale length failed");
            return nullptr;
        }
        std::vector<char> localeBuf(len + 1);
        status = napi_get_value_string_utf8(env, argv[0], localeBuf.data(), len + 1, &len);
        if (status != napi_ok) {
            HILOG_ERROR_I18N("IndexUtilConstructor: Get locale failed");
            return nullptr;
        }
        localeTag = localeBuf.data();
    }
    std::unique_ptr<I18nAddon> obj = nullptr;
    obj = std::make_unique<I18nAddon>();
    status =
        napi_wrap(env, thisVar, reinterpret_cast<void *>(obj.get()), I18nAddon::Destructor, nullptr, nullptr);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("IndexUtilConstructor: Wrap II18nAddon failed");
        return nullptr;
    }
    if (!obj->InitIndexUtilContext(env, info, localeTag)) {
        return nullptr;
    }
    obj.release();
    return thisVar;
}

bool I18nAddon::InitIndexUtilContext(napi_env env, napi_callback_info info, const std::string &localeTag)
{
    napi_value global = nullptr;
    napi_status status = napi_get_global(env, &global);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("InitIndexUtilContext: Get global failed");
        return false;
    }
    env_ = env;
    indexUtil_ = std::make_unique<IndexUtil>(localeTag);
    return indexUtil_ != nullptr;
}

napi_value I18nAddon::GetIndexUtil(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = { 0 };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    napi_value constructor = nullptr;
    napi_status status = napi_get_reference_value(env, g_indexUtilConstructor, &constructor);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("Failed to create reference at GetIndexUtil");
        return nullptr;
    }
    napi_value result = nullptr;
    if (!VariableConvertor::CheckNapiIsNull(env, argv[0])) {
        status = napi_new_instance(env, constructor, 0, argv, &result);
    } else {
        status = napi_new_instance(env, constructor, 1, argv, &result);
    }
    if (status != napi_ok) {
        HILOG_ERROR_I18N("Get calendar create instance failed");
        return nullptr;
    }
    return result;
}

napi_value I18nAddon::GetIndexList(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, &data);

    I18nAddon *obj = nullptr;
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->indexUtil_) {
        HILOG_ERROR_I18N("GetIndexList: GetPhoneNumberFormat object failed");
        return nullptr;
    }

    std::vector<std::string> indexList = obj->indexUtil_->GetIndexList();
    napi_value result = nullptr;
    status = napi_create_array_with_length(env, indexList.size(), &result);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("Failed to create array");
        return nullptr;
    }
    for (size_t i = 0; i < indexList.size(); i++) {
        napi_value element = nullptr;
        status = napi_create_string_utf8(env, indexList[i].c_str(), NAPI_AUTO_LENGTH, &element);
        if (status != napi_ok) {
            HILOG_ERROR_I18N("GetIndexList: Failed to create string item");
            return nullptr;
        }
        status = napi_set_element(env, result, i, element);
        if (status != napi_ok) {
            HILOG_ERROR_I18N("Failed to set array item");
            return nullptr;
        }
    }
    return result;
}

napi_value I18nAddon::AddLocale(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = { 0 };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    napi_valuetype valueType = napi_valuetype::napi_undefined;
    napi_typeof(env, argv[0], &valueType);
    if (valueType != napi_valuetype::napi_string) {
        HILOG_ERROR_I18N("AddLocale: Parameter type does not match");
        return nullptr;
    }
    size_t len = 0;
    napi_status status = napi_get_value_string_utf8(env, argv[0], nullptr, 0, &len);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("AddLocale: Get locale length failed");
        return nullptr;
    }
    std::vector<char> buf(len + 1);
    status = napi_get_value_string_utf8(env, argv[0], buf.data(), len + 1, &len);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("AddLocale: Get locale failed");
        return nullptr;
    }
    I18nAddon *obj = nullptr;
    status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->indexUtil_) {
        HILOG_ERROR_I18N("AddLocale: Get IndexUtil object failed");
        return nullptr;
    }
    obj->indexUtil_->AddLocale(buf.data());
    return nullptr;
}

napi_value I18nAddon::GetIndex(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = { 0 };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    napi_valuetype valueType = napi_valuetype::napi_undefined;
    napi_typeof(env, argv[0], &valueType);
    if (valueType != napi_valuetype::napi_string) {
        HILOG_ERROR_I18N("GetIndex: Parameter type does not match");
        return nullptr;
    }
    size_t len = 0;
    napi_status status = napi_get_value_string_utf8(env, argv[0], nullptr, 0, &len);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("GetIndex: Get String length failed");
        return nullptr;
    }
    std::vector<char> buf(len + 1);
    status = napi_get_value_string_utf8(env, argv[0], buf.data(), len + 1, &len);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("Get String failed");
        return nullptr;
    }
    I18nAddon *obj = nullptr;
    status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->indexUtil_) {
        HILOG_ERROR_I18N("GetIndex: Get IndexUtil object failed");
        return nullptr;
    }
    std::string index = obj->indexUtil_->GetIndex(buf.data());
    napi_value result = nullptr;
    status = napi_create_string_utf8(env, index.c_str(), NAPI_AUTO_LENGTH, &result);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("GetIndex Failed");
        return nullptr;
    }
    return result;
}

napi_value I18nAddon::ObjectConstructor(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, &data);
    if (status != napi_ok) {
        return nullptr;
    }
    std::unique_ptr<I18nAddon> obj = nullptr;
    obj = std::make_unique<I18nAddon>();
    status =
        napi_wrap(env, thisVar, reinterpret_cast<void *>(obj.get()), I18nAddon::Destructor, nullptr, nullptr);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("ObjectConstructor: Wrap I18nAddon failed");
        return nullptr;
    }
    obj.release();
    return thisVar;
}

napi_value I18nAddon::InitUtil(napi_env env, napi_value exports)
{
    napi_status status = napi_ok;
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("unitConvert", UnitConvert)
    };

    napi_value constructor = nullptr;
    status = napi_define_class(env, "Util", NAPI_AUTO_LENGTH, ObjectConstructor, nullptr,
        sizeof(properties) / sizeof(napi_property_descriptor), properties, &constructor);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("Define class failed when InitUtil");
        return nullptr;
    }

    status = napi_set_named_property(env, exports, "Util", constructor);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("Set property failed when InitUtil");
        return nullptr;
    }
    return exports;
}

napi_value I18nAddon::CreateTemperatureTypeEnum(napi_env env)
{
    napi_value temperatureType = nullptr;
    napi_status status = napi_create_object(env, &temperatureType);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("I18nAddon::CreateTemperatureTypeEnum: Create temperatureType failed.");
        return nullptr;
    }
    for (auto& nameToValue : EnumTemperatureType) {
        std::string name = nameToValue.first;
        int32_t value = static_cast<int32_t>(nameToValue.second);
        status = VariableConvertor::SetEnumValue(env, temperatureType, name, value);
        if (status != napi_ok) {
            HILOG_ERROR_I18N("I18nAddon::CreateTemperatureTypeEnum: set enum name %{public}s failed.",
                name.c_str());
            return nullptr;
        }
    }
    return temperatureType;
}

napi_value I18nAddon::CreateWeekDayEnum(napi_env env)
{
    napi_value weekDay = nullptr;
    napi_status status = napi_create_object(env, &weekDay);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("I18nAddon::CreateWeekDayEnum: Create weekDay failed.");
        return nullptr;
    }
    for (auto& nameToValue : EnumWeekDay) {
        std::string name = nameToValue.first;
        int32_t value = static_cast<int32_t>(nameToValue.second);
        status = VariableConvertor::SetEnumValue(env, weekDay, name, value);
        if (status != napi_ok) {
            HILOG_ERROR_I18N("I18nAddon::CreateWeekDayEnum: set enum name %{public}s failed.",
                name.c_str());
            return nullptr;
        }
    }
    return weekDay;
}

napi_value Init(napi_env env, napi_value exports)
{
    napi_value val = I18nAddon::Init(env, exports);
    val = I18nAddon::InitPhoneNumberFormat(env, val);
    val = I18nAddon::InitI18nBreakIterator(env, val);
    val = I18nCalendarAddon::InitI18nCalendar(env, val);
    val = I18nAddon::InitI18nIndexUtil(env, val);
    val = I18nAddon::InitI18nUtil(env, val);
    val = I18nTimeZoneAddon::InitI18nTimeZone(env, val);
    val = I18nAddon::InitI18nTransliterator(env, val);
    val = I18nUnicodeAddon::InitCharacter(env, val);
    val = I18nUnicodeAddon::InitI18nUnicode(env, val);
    val = I18nAddon::InitUtil(env, val);
    val = I18nNormalizerAddon::InitI18nNormalizer(env, val);
    val = SystemLocaleManagerAddon::InitSystemLocaleManager(env, val);
    val = I18nSystemAddon::InitI18nSystem(env, val);
    val = HolidayManagerAddon::InitHolidayManager(env, val);
    val = EntityRecognizerAddon::InitEntityRecognizer(env, val);
    val = SimpleNumberFormatAddon::InitSimpleNumberFormat(env, val);
    val = SimpleDateTimeFormatAddon::InitSimpleDateTimeFormat(env, val);
    val = StyledNumberFormatAddon::InitStyledNumberFormat(env, val);
    return val;
}

static napi_module g_i18nModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = Init,
    .nm_modname = "i18n",
    .nm_priv = nullptr,
    .reserved = { 0 }
};

extern "C" __attribute__((constructor)) void I18nRegister()
{
    napi_module_register(&g_i18nModule);
}
} // namespace I18n
} // namespace Global
} // namespace OHOS
