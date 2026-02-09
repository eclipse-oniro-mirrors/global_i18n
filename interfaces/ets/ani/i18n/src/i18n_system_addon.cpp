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

#include "i18n_system_addon.h"

#include "error_util.h"
#include "i18n_hilog.h"
#include "i18n_service_ability_client.h"
#include "locale_config.h"
#include "locale_info.h"
#include "preferred_language.h"
#include "utils.h"
#include "variable_converter.h"

using namespace OHOS;
using namespace Global;
using namespace I18n;

static const std::string NAME_OF_TEMPERATURE = "type";
static const std::string TYPE_OF_TEMPERATURE = "TemperatureType";
static const std::string NAME_OF_WEEKDAY = "type";
static const std::string TYPE_OF_WEEKDAY = "WeekDay";
static const int OFFSET_OF_ENUM = 1;
const std::vector<ani_native_function> I18nSystemAddon::METHODS = {
    ani_native_function { "getDisplayCountry", nullptr, reinterpret_cast<void *>(GetDisplayCountry) },
    ani_native_function { "getDisplayLanguage", nullptr, reinterpret_cast<void *>(GetDisplayLanguage) },
    ani_native_function { "getSystemLanguages", nullptr, reinterpret_cast<void *>(GetSystemLanguages) },
    ani_native_function { "getSystemCountries", nullptr, reinterpret_cast<void *>(GetSystemCountries) },
    ani_native_function { "isSuggested", nullptr, reinterpret_cast<void *>(IsSuggested) },
    ani_native_function { "getSystemLanguage", nullptr, reinterpret_cast<void *>(GetSystemLanguage) },
    ani_native_function { "setSystemLanguage", nullptr, reinterpret_cast<void *>(SetSystemLanguage) },
    ani_native_function { "getSystemRegion", nullptr, reinterpret_cast<void *>(GetSystemRegion) },
    ani_native_function { "setSystemRegion", nullptr, reinterpret_cast<void *>(SetSystemRegion) },
    ani_native_function { "getSystemLocale", nullptr, reinterpret_cast<void *>(GetSystemLocale) },
    ani_native_function { "setSystemLocale", nullptr, reinterpret_cast<void *>(SetSystemLocale) },
    ani_native_function { "is24HourClock", nullptr, reinterpret_cast<void *>(Is24HourClock) },
    ani_native_function { "set24HourClock", nullptr, reinterpret_cast<void *>(Set24HourClock) },
    ani_native_function { "addPreferredLanguage", nullptr, reinterpret_cast<void *>(AddPreferredLanguage) },
    ani_native_function { "removePreferredLanguage", nullptr, reinterpret_cast<void *>(RemovePreferredLanguage) },
    ani_native_function { "getPreferredLanguageList", nullptr,
        reinterpret_cast<void *>(GetPreferredLanguageList) },
    ani_native_function { "getFirstPreferredLanguage", nullptr,
        reinterpret_cast<void *>(GetFirstPreferredLanguage) },
    ani_native_function { "setAppPreferredLanguage", nullptr, reinterpret_cast<void *>(SetAppPreferredLanguage) },
    ani_native_function { "getAppPreferredLanguage", nullptr, reinterpret_cast<void *>(GetAppPreferredLanguage) },
    ani_native_function { "setUsingLocalDigit", nullptr, reinterpret_cast<void *>(SetUsingLocalDigit) },
    ani_native_function { "getUsingLocalDigit", nullptr, reinterpret_cast<void *>(GetUsingLocalDigit) },
    ani_native_function { "getSimplifiedLanguage", nullptr, reinterpret_cast<void *>(GetSimplifiedLanguage) },
    ani_native_function { "setTemperatureType", nullptr, reinterpret_cast<void *>(SetTemperatureType) },
    ani_native_function { "getTemperatureType", nullptr, reinterpret_cast<void *>(GetTemperatureType) },
    ani_native_function { "getTemperatureName", nullptr, reinterpret_cast<void *>(GetTemperatureName) },
    ani_native_function { "setFirstDayOfWeek", nullptr, reinterpret_cast<void *>(SetFirstDayOfWeek) },
    ani_native_function { "getFirstDayOfWeek", nullptr, reinterpret_cast<void *>(GetFirstDayOfWeek) },
    ani_native_function { "getSystemCollations", nullptr, reinterpret_cast<void *>(GetSystemCollations) },
    ani_native_function { "getUsingCollation", nullptr, reinterpret_cast<void *>(GetUsingCollation) },
    ani_native_function { "setSystemCollation", nullptr, reinterpret_cast<void *>(SetSystemCollation) },
    ani_native_function { "getSystemMeasurements", nullptr, reinterpret_cast<void *>(GetSystemMeasurements) },
    ani_native_function { "getUsingMeasurement", nullptr, reinterpret_cast<void *>(GetUsingMeasurement) },
    ani_native_function { "setSystemMeasurement", nullptr, reinterpret_cast<void *>(SetSystemMeasurement) },
    ani_native_function { "getUsingNumberingSystem", nullptr, reinterpret_cast<void *>(GetUsingNumberingSystem) },
    ani_native_function { "setSystemNumberingSystem",
        nullptr, reinterpret_cast<void *>(SetSystemNumberingSystem) },
    ani_native_function { "getSystemNumberingSystems",
        nullptr, reinterpret_cast<void *>(GetSystemNumberingSystems) },
    ani_native_function { "getSystemNumberPatterns", nullptr, reinterpret_cast<void *>(GetSystemNumberPatterns) },
    ani_native_function { "setSystemNumberPattern", nullptr, reinterpret_cast<void *>(SetSystemNumberPattern) },
    ani_native_function { "getUsingNumberPattern", nullptr, reinterpret_cast<void *>(GetUsingNumberPattern) },
    ani_native_function { "getSystemNumericalDatePatterns",
        nullptr, reinterpret_cast<void *>(GetSystemNumericalDatePatterns) },
    ani_native_function { "setSystemNumericalDatePattern", nullptr,
        reinterpret_cast<void *>(SetSystemNumericalDatePattern) },
    ani_native_function { "getUsingNumericalDatePattern", nullptr,
        reinterpret_cast<void *>(GetUsingNumericalDatePattern) },
    ani_native_function { "getSystemLocaleInstance", nullptr, reinterpret_cast<void *>(GetSystemLocaleInstance) },
};

ani_string I18nSystemAddon::GetDisplayCountry(ani_env *env, [[maybe_unused]] ani_object object,
    ani_string country, ani_string locale, ani_object sentenceCase)
{
    std::string countryBuf = VariableConverter::AniStrToString(env, country);
    std::string displayLocaleBuf = VariableConverter::AniStrToString(env, locale);
    LocaleInfo localeInfo(countryBuf);
    if (!LocaleConfig::IsValidRegion(countryBuf) && localeInfo.GetRegion() == "") {
        ErrorUtil::AniThrow(env, I18N_NOT_VALID, "country", "a valid region");
        return nullptr;
    } else if (!LocaleConfig::IsValidTag(displayLocaleBuf)) {
        ErrorUtil::AniThrow(env, I18N_NOT_VALID, "locale", "a valid locale");
        return nullptr;
    }

    bool sentenceCaseBool = true;
    ani_boolean isUndefined;
    if (ANI_OK != env->Reference_IsUndefined(sentenceCase, &isUndefined)) {
        HILOG_ERROR_I18N("I18nSystemAddon::GetDisplayCountry: Check sentenceCase reference is undefined failed");
        return nullptr;
    }
    if (!isUndefined) {
        ani_boolean value;
        if (ANI_OK != env->Object_CallMethodByName_Boolean(sentenceCase, "toBoolean", ":z", &value)) {
            HILOG_ERROR_I18N("I18nSystemAddon::GetDisplayCountry: Unbox Boolean failed");
            return nullptr;
        }
        sentenceCaseBool = value;
    }

    std::string str = LocaleConfig::GetDisplayRegion(countryBuf, displayLocaleBuf, sentenceCaseBool);
    return VariableConverter::StringToAniStr(env, str);
}

ani_string I18nSystemAddon::GetDisplayLanguage(ani_env *env, [[maybe_unused]] ani_object object,
    ani_string language, ani_string locale, ani_object sentenceCase)
{
    std::string displayLocaleBuf = VariableConverter::AniStrToString(env, locale);
    if (!LocaleConfig::IsValidTag(displayLocaleBuf)) {
        ErrorUtil::AniThrow(env, I18N_NOT_VALID, "locale", "a valid locale");
        return nullptr;
    }

    bool sentenceCaseBool = true;
    ani_boolean isUndefined;
    if (ANI_OK != env->Reference_IsUndefined(sentenceCase, &isUndefined)) {
        HILOG_ERROR_I18N("I18nSystemAddon::GetDisplayLanguage: Check sentenceCase reference is undefined failed");
        return nullptr;
    }
    if (!isUndefined) {
        ani_boolean value;
        if (ANI_OK != env->Object_CallMethodByName_Boolean(sentenceCase, "toBoolean", ":z", &value)) {
            HILOG_ERROR_I18N("I18nSystemAddon::GetDisplayLanguage: Unbox Boolean failed");
            return nullptr;
        }
        sentenceCaseBool = value;
    }

    std::string str = LocaleConfig::GetDisplayLanguage(VariableConverter::AniStrToString(env, language),
        displayLocaleBuf, sentenceCaseBool);
    return VariableConverter::StringToAniStr(env, str);
}

ani_object I18nSystemAddon::GetSystemLanguages(ani_env *env, [[maybe_unused]] ani_object object)
{
    std::unordered_set<std::string> systemLanguages = LocaleConfig::GetSystemLanguages();
    std::vector<std::string> vector(systemLanguages.begin(), systemLanguages.end());
    return VariableConverter::CreateArray(env, vector);
}

ani_object I18nSystemAddon::GetSystemCountries(ani_env *env, [[maybe_unused]] ani_object object, ani_string language)
{
    std::unordered_set<std::string> systemCountries =
        LocaleConfig::GetSystemCountries(VariableConverter::AniStrToString(env, language));
    std::vector<std::string> vector(systemCountries.begin(), systemCountries.end());
    return VariableConverter::CreateArray(env, vector);
}

ani_boolean I18nSystemAddon::IsSuggested(ani_env *env, [[maybe_unused]] ani_object object,
    ani_string language, ani_string region)
{
    bool isSuggested = false;
    ani_boolean isUndefined;
    if (ANI_OK != env->Reference_IsUndefined(region, &isUndefined)) {
        HILOG_ERROR_I18N("I18nSystemAddon::IsSuggested: Check region reference is undefined failed");
        return false;
    }
    if (!isUndefined) {
        isSuggested = LocaleConfig::IsSuggested(VariableConverter::AniStrToString(env, language),
            VariableConverter::AniStrToString(env, region));
    } else {
        isSuggested = LocaleConfig::IsSuggested(VariableConverter::AniStrToString(env, language));
    }
    return isSuggested;
}

ani_string I18nSystemAddon::GetSystemLanguage(ani_env *env, [[maybe_unused]] ani_object object)
{
    std::string str = LocaleConfig::GetSystemLanguage();
    return VariableConverter::StringToAniStr(env, str);
}

void I18nSystemAddon::SetSystemLanguage(ani_env *env, [[maybe_unused]] ani_object object, ani_string language)
{
    I18nErrorCode err = I18nServiceAbilityClient::SetSystemLanguage(VariableConverter::AniStrToString(env, language));
    HILOG_INFO_I18N("I18nSystemAddon::SetSystemLanguage with code %{public}d", static_cast<int32_t>(err));
    bool success = err == I18nErrorCode::SUCCESS;
    if (!success) {
        ErrorUtil::AniThrow(env, I18N_NO_PERMISSION, "", "");
    }
}

ani_string I18nSystemAddon::GetSystemRegion(ani_env *env, [[maybe_unused]] ani_object object)
{
    std::string str = LocaleConfig::GetSystemRegion();
    return VariableConverter::StringToAniStr(env, str);
}

void I18nSystemAddon::SetSystemRegion(ani_env *env, [[maybe_unused]] ani_object object, ani_string region)
{
    I18nErrorCode err = I18nServiceAbilityClient::SetSystemRegion(VariableConverter::AniStrToString(env, region));
    HILOG_INFO_I18N("I18nSystemAddon::SetSystemRegion with code %{public}d", static_cast<int32_t>(err));
    bool success = err == I18nErrorCode::SUCCESS;
    if (!success) {
        ErrorUtil::AniThrow(env, I18N_NO_PERMISSION, "", "");
    }
}

ani_string I18nSystemAddon::GetSystemLocale(ani_env *env, [[maybe_unused]] ani_object object)
{
    std::string str = LocaleConfig::GetSystemLocale();
    return VariableConverter::StringToAniStr(env, str);
}

void I18nSystemAddon::SetSystemLocale(ani_env *env, [[maybe_unused]] ani_object object, ani_string locale)
{
    I18nErrorCode err = I18nServiceAbilityClient::SetSystemLocale(VariableConverter::AniStrToString(env, locale));
    bool success = err == I18nErrorCode::SUCCESS;
    if (!success) {
        ErrorUtil::AniThrow(env, I18N_NO_PERMISSION, "", "");
    }
}

ani_boolean I18nSystemAddon::Is24HourClock([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object object)
{
    return LocaleConfig::Is24HourClock();
}

void I18nSystemAddon::Set24HourClock([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object object,
    ani_boolean option)
{
    std::string optionStr = option ? "true" : "false";
    I18nErrorCode err = I18nServiceAbilityClient::Set24HourClock(optionStr);
    HILOG_INFO_I18N("I18nSystemAddon::Set24HourClock with code %{public}d", static_cast<int32_t>(err));
    bool success = err == I18nErrorCode::SUCCESS;
    if (!success) {
        ErrorUtil::AniThrow(env, I18N_NO_PERMISSION, "", "");
    }
}

void I18nSystemAddon::AddPreferredLanguage(ani_env *env, [[maybe_unused]] ani_object object,
    ani_string language, ani_object number)
{
    ani_int index = 1000000;
    ani_boolean isUndefined;
    if (ANI_OK != env->Reference_IsUndefined(number, &isUndefined)) {
        HILOG_ERROR_I18N("Reference IsUndefined failed");
        return;
    }
    if (!isUndefined) {
        if (ANI_OK != env->Object_CallMethodByName_Int(number, "toInt", ":i", &index)) {
            HILOG_ERROR_I18N("I18nSystemAddon::AddPreferredLanguage: Unbox number to int failed");
            return;
        }
    }
    I18nErrorCode err = I18nServiceAbilityClient::AddPreferredLanguage(
        VariableConverter::AniStrToString(env, language), index);
    HILOG_INFO_I18N("I18nSystemAddon::AddPreferredLanguage with code %{public}d", static_cast<int32_t>(err));
    if (err == I18nErrorCode::NO_PERMISSION) {
        ErrorUtil::AniThrow(env, I18N_NO_PERMISSION, "", "");
    }
    if (err != I18nErrorCode::SUCCESS) {
        ErrorUtil::AniThrow(env, I18N_NOT_VALID, "language", "a valid language");
    }
}

void I18nSystemAddon::RemovePreferredLanguage([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object object,
    ani_int index)
{
    int len = static_cast<int>(PreferredLanguage::GetPreferredLanguageList().size());
    if ((index < 0 || index > len - 1)) {
        ErrorUtil::AniThrow(env, I18N_NOT_VALID, "index", "within the valid range");
        return;
    }

    I18nErrorCode err = I18nServiceAbilityClient::RemovePreferredLanguage(index);
    bool success = err == I18nErrorCode::SUCCESS;
    if (!success) {
        ErrorUtil::AniThrow(env, I18N_NO_PERMISSION, "", "");
    }
}

ani_object I18nSystemAddon::GetPreferredLanguageList(ani_env *env, [[maybe_unused]] ani_object object)
{
    std::vector<std::string> languageList = PreferredLanguage::GetPreferredLanguageList();
    return VariableConverter::CreateArray(env, languageList);
}

ani_string I18nSystemAddon::GetFirstPreferredLanguage(ani_env *env, [[maybe_unused]] ani_object object)
{
    std::string language = PreferredLanguage::GetFirstPreferredLanguage();
    return VariableConverter::StringToAniStr(env, language);
}

void I18nSystemAddon::SetAppPreferredLanguage(ani_env *env, [[maybe_unused]] ani_object object, ani_string language)
{
    std::string localeTag = VariableConverter::AniStrToString(env, language);
    UErrorCode icuStatus = U_ZERO_ERROR;
    icu::Locale locale = icu::Locale::forLanguageTag(localeTag, icuStatus);
    if (U_FAILURE(icuStatus) || !(IsValidLocaleTag(locale) || localeTag.compare("default") == 0)) {
        HILOG_ERROR_I18N("I18nSystemAddon::SetAppPreferredLanguage: locale is not valid");
        ErrorUtil::AniThrow(env, I18N_NOT_VALID, "language", "a valid language");
        return;
    }

#ifdef SUPPORT_APP_PREFERRED_LANGUAGE
    I18nErrorCode errCode = I18nErrorCode::SUCCESS;
    PreferredLanguage::SetAppPreferredLanguage(localeTag, errCode);
    if (errCode != I18nErrorCode::SUCCESS) {
        HILOG_ERROR_I18N("I18nSystemAddon::SetAppPreferredLanguage: set app preferred language failed");
    }
#endif
}

ani_string I18nSystemAddon::GetAppPreferredLanguage(ani_env *env, [[maybe_unused]] ani_object object)
{
#ifdef SUPPORT_APP_PREFERRED_LANGUAGE
    std::string language = PreferredLanguage::GetAppPreferredLanguage();
#else
    std::string language = PreferredLanguage::GetFirstPreferredLanguage();
#endif
    return VariableConverter::StringToAniStr(env, language);
}

void I18nSystemAddon::SetUsingLocalDigit([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object object,
    ani_boolean flag)
{
    I18nErrorCode err = I18nServiceAbilityClient::SetUsingLocalDigit(flag);
    HILOG_INFO_I18N("I18nSystemAddon::SetUsingLocalDigit with code %{public}d", static_cast<int32_t>(err));
    bool res = err == I18nErrorCode::SUCCESS;
    if (!res) {
        ErrorUtil::AniThrow(env, I18N_NO_PERMISSION, "", "");
    }
}

ani_boolean I18nSystemAddon::GetUsingLocalDigit([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object object)
{
    return LocaleConfig::GetUsingLocalDigit();
}

ani_string I18nSystemAddon::GetSimplifiedLanguage([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object object,
    ani_string language)
{
    std::string simplifiedLanguage = "";
    ani_boolean isUndefined;
    if (ANI_OK != env->Reference_IsUndefined(language, &isUndefined)) {
        HILOG_ERROR_I18N("I18nSystemAddon::GetSimplifiedLanguage: Check language reference is undefined failed");
        return nullptr;
    }
    if (isUndefined) {
        simplifiedLanguage = LocaleConfig::GetSimplifiedSystemLanguage();
    } else {
        int code = 0;
        simplifiedLanguage = LocaleConfig::GetSimplifiedLanguage(
            VariableConverter::AniStrToString(env, language), code);
        if (code != 0) {
            HILOG_ERROR_I18N("I18nSystemAddon::GetSimplifiedLanguage: language is not valid");
            ErrorUtil::AniThrow(env, I18N_NOT_VALID, "language", "a valid language");
            return nullptr;
        }
    }
    return VariableConverter::StringToAniStr(env, simplifiedLanguage);
}

void I18nSystemAddon::SetTemperatureType([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object object,
    ani_enum_item type)
{
    TemperatureType typeEnum = TemperatureType::CELSIUS;
    ani_int value;
    if (ANI_OK != env->EnumItem_GetValue_Int(type, &value)) {
        HILOG_ERROR_I18N("I18nSystemAddon::SetTemperatureType: parse temperature type failed.");
        return;
    }
    typeEnum = static_cast<TemperatureType>(value);

    I18nErrorCode err = I18nServiceAbilityClient::SetTemperatureType(typeEnum);
    if (err == I18nErrorCode::SUCCESS) {
        return;
    }
    switch (err) {
        case I18nErrorCode::NOT_SYSTEM_APP:
            ErrorUtil::AniThrow(env, I18N_NOT_SYSTEM_APP, "", "");
            break;
        case I18nErrorCode::NO_PERMISSION:
            ErrorUtil::AniThrow(env, I18N_NO_PERMISSION, "", "");
            break;
        case I18nErrorCode::INVALID_TEMPERATURE_TYPE:
            ErrorUtil::AniThrow(env, I18N_NOT_VALID, NAME_OF_TEMPERATURE, TYPE_OF_TEMPERATURE);
            break;
        default:
            HILOG_ERROR_I18N("I18nSystemAddon::SetTemperatureType: set temperature type failed.");
    }
}

ani_enum_item I18nSystemAddon::GetTemperatureType([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object object)
{
    TemperatureType type = LocaleConfig::GetTemperatureType();
    int32_t index = static_cast<int32_t>(type) - OFFSET_OF_ENUM;
    static const char *enumName = "@ohos.i18n.i18n.TemperatureType";
    return VariableConverter::GetEnumItemByIndex(env, enumName, index);
}

ani_string I18nSystemAddon::GetTemperatureName([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object object,
    ani_enum_item type)
{
    TemperatureType typeEnum = TemperatureType::CELSIUS;
    ani_int value;
    if (ANI_OK != env->EnumItem_GetValue_Int(type, &value)) {
        HILOG_ERROR_I18N("I18nSystemAddon::GetTemperatureName: parse temperature type failed.");
        return nullptr;
    }
    typeEnum = static_cast<TemperatureType>(value);

    std::string temperatureName = LocaleConfig::GetTemperatureName(typeEnum);
    if (temperatureName.empty()) {
        ErrorUtil::AniThrow(env, I18N_NOT_VALID, NAME_OF_TEMPERATURE, TYPE_OF_TEMPERATURE);
        return nullptr;
    }
    return VariableConverter::StringToAniStr(env, temperatureName);
}

void I18nSystemAddon::SetFirstDayOfWeek([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object object,
    ani_enum_item type)
{
    ani_int weekDay;
    if (ANI_OK != env->EnumItem_GetValue_Int(type, &weekDay)) {
        HILOG_ERROR_I18N("I18nSystemAddon::SetFirstDayOfWeek: get week day failed.");
        return;
    }
    WeekDay typeEnum = static_cast<WeekDay>(weekDay);

    I18nErrorCode err = I18nServiceAbilityClient::SetFirstDayOfWeek(typeEnum);
    if (err == I18nErrorCode::SUCCESS) {
        return;
    }
    switch (err) {
        case I18nErrorCode::NOT_SYSTEM_APP:
            ErrorUtil::AniThrow(env, I18N_NOT_SYSTEM_APP, "", "");
            break;
        case I18nErrorCode::NO_PERMISSION:
            ErrorUtil::AniThrow(env, I18N_NO_PERMISSION, "", "");
            break;
        case I18nErrorCode::INVALID_WEEK_DAY:
            ErrorUtil::AniThrow(env, I18N_NOT_VALID, NAME_OF_WEEKDAY, TYPE_OF_WEEKDAY);
            break;
        default:
            HILOG_ERROR_I18N("I18nSystemAddon::SetFirstDayOfWeek: set first day of week failed.");
    }
}

ani_enum_item I18nSystemAddon::GetFirstDayOfWeek([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object object)
{
    WeekDay type = LocaleConfig::GetFirstDayOfWeek();
    int32_t index = static_cast<int32_t>(type) - OFFSET_OF_ENUM;
    static const char *enumName = "@ohos.i18n.i18n.WeekDay";
    return VariableConverter::GetEnumItemByIndex(env, enumName, index);
}

ani_object I18nSystemAddon::GetSystemCollations(ani_env *env, [[maybe_unused]] ani_object object)
{
    std::unordered_map<std::string, std::string> systemCollations;
    I18nErrorCode err = I18nServiceAbilityClient::GetSystemCollations(systemCollations);
    if (err == I18nErrorCode::NOT_SYSTEM_APP) {
        ErrorUtil::AniThrow(env, I18N_NOT_SYSTEM_APP, "", "");
        return nullptr;
    }
    return VariableConverter::CreateAniMap(env, systemCollations);
}

ani_string I18nSystemAddon::GetUsingCollation(ani_env *env, [[maybe_unused]] ani_object object)
{
    std::string usingCollation;
    I18nErrorCode err = I18nServiceAbilityClient::GetUsingCollation(usingCollation);
    if (err == I18nErrorCode::NOT_SYSTEM_APP) {
        ErrorUtil::AniThrow(env, I18N_NOT_SYSTEM_APP, "", "");
        return nullptr;
    }
    return VariableConverter::StringToAniStr(env, usingCollation);
}

void I18nSystemAddon::SetSystemCollation(ani_env *env, [[maybe_unused]] ani_object object, ani_string aniIdentifier)
{
    std::string identifier = VariableConverter::AniStrToString(env, aniIdentifier);
    if (identifier.empty()) {
        HILOG_ERROR_I18N("I18nSystemAddon::SetSystemCollation: param is empty.");
        return;
    }
    I18nErrorCode err = I18nServiceAbilityClient::SetSystemCollation(identifier);
    if (err == I18nErrorCode::SUCCESS) {
        return;
    }
    switch (err) {
        case I18nErrorCode::NOT_SYSTEM_APP:
            ErrorUtil::AniThrow(env, I18N_NOT_SYSTEM_APP, "", "");
            break;
        case I18nErrorCode::NO_PERMISSION:
            ErrorUtil::AniThrow(env, I18N_NO_PERMISSION, "", "");
            break;
        case I18nErrorCode::INVALID_PARAM:
            ErrorUtil::AniThrow(env, I18N_NOT_VALID_NEW, "identifier", "in a collation supported by the system");
            break;
        default:
            HILOG_ERROR_I18N("I18nSystemAddon::SetSystemCollation: Set system collation failed.");
    }
}

ani_object I18nSystemAddon::GetSystemMeasurements([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object object)
{
    std::unordered_map<std::string, std::string> systemMeasurements;
    I18nErrorCode err = I18nServiceAbilityClient::GetSystemMeasurements(systemMeasurements);
    if (err == I18nErrorCode::NOT_SYSTEM_APP) {
        ErrorUtil::AniThrow(env, I18N_NOT_SYSTEM_APP, "", "");
        return nullptr;
    }
    return VariableConverter::CreateAniMap(env, systemMeasurements);
}

ani_string I18nSystemAddon::GetUsingMeasurement([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object object)
{
    std::string usingMeasurement;
    I18nErrorCode err = I18nServiceAbilityClient::GetUsingMeasurement(usingMeasurement);
    if (err == I18nErrorCode::NOT_SYSTEM_APP) {
        ErrorUtil::AniThrow(env, I18N_NOT_SYSTEM_APP, "", "");
        return nullptr;
    }
    return VariableConverter::StringToAniStr(env, usingMeasurement);
}

void I18nSystemAddon::SetSystemMeasurement([[maybe_unused]] ani_env *env,
    [[maybe_unused]] ani_object object, ani_string aniIdentifier)
{
    std::string identifier = VariableConverter::AniStrToString(env, aniIdentifier);
    if (identifier.empty()) {
        HILOG_ERROR_I18N("I18nSystemAddon::SetSystemMeasurement: identifier is empty.");
        return;
    }
    I18nErrorCode err = I18nServiceAbilityClient::SetSystemMeasurement(identifier);
    if (err == I18nErrorCode::SUCCESS) {
        return;
    }
    switch (err) {
        case I18nErrorCode::NOT_SYSTEM_APP:
            ErrorUtil::AniThrow(env, I18N_NOT_SYSTEM_APP, "", "");
            break;
        case I18nErrorCode::NO_PERMISSION:
            ErrorUtil::AniThrow(env, I18N_NO_PERMISSION, "", "");
            break;
        case I18nErrorCode::INVALID_PARAM:
            ErrorUtil::AniThrow(env, I18N_NOT_VALID_NEW, "identifier", "in a measurement supported by the system");
            break;
        default:
            HILOG_ERROR_I18N("I18nSystemAddon::SetSystemMeasurement: Set system measurement failed.");
    }
}

ani_string I18nSystemAddon::GetUsingNumberingSystem([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object object)
{
    std::string usingNumberingSystem;
    I18nErrorCode err = I18nServiceAbilityClient::GetUsingNumberingSystem(usingNumberingSystem);
    if (err == I18nErrorCode::NOT_SYSTEM_APP) {
        ErrorUtil::AniThrow(env, I18N_NOT_SYSTEM_APP, "", "");
        return nullptr;
    }
    return VariableConverter::StringToAniStr(env, usingNumberingSystem);
}

void I18nSystemAddon::SetSystemNumberingSystem([[maybe_unused]] ani_env *env,
    [[maybe_unused]] ani_object object, ani_string aniIdentifier)
{
    std::string identifier = VariableConverter::AniStrToString(env, aniIdentifier);
    if (identifier.empty()) {
        HILOG_ERROR_I18N("I18nSystemAddon::SetSystemNumberingSystem: identifier is empty.");
        return;
    }
    I18nErrorCode err = I18nServiceAbilityClient::SetSystemNumberingSystem(identifier);
    if (err == I18nErrorCode::SUCCESS) {
        return;
    }
    switch (err) {
        case I18nErrorCode::NOT_SYSTEM_APP:
            ErrorUtil::AniThrow(env, I18N_NOT_SYSTEM_APP, "", "");
            break;
        case I18nErrorCode::NO_PERMISSION:
            ErrorUtil::AniThrow(env, I18N_NO_PERMISSION, "", "");
            break;
        case I18nErrorCode::INVALID_PARAM:
            ErrorUtil::AniThrow(env, I18N_NOT_VALID_NEW, "identifier", "in a numbering system supported "
                "by the system");
            break;
        default:
            HILOG_ERROR_I18N("I18nSystemAddon::SetSystemNumberingSystem: Set system numbering system failed.");
    }
}
    
ani_object I18nSystemAddon::GetSystemNumberingSystems([[maybe_unused]] ani_env *env,
    [[maybe_unused]] ani_object object)
{
    std::unordered_map<std::string, std::string> systemNumberingSystems;
    I18nErrorCode err = I18nServiceAbilityClient::GetSystemNumberingSystems(systemNumberingSystems);
    if (err == I18nErrorCode::NOT_SYSTEM_APP) {
        ErrorUtil::AniThrow(env, I18N_NOT_SYSTEM_APP, "", "");
        return nullptr;
    }
    return VariableConverter::CreateAniMap(env, systemNumberingSystems);
}

ani_object I18nSystemAddon::GetSystemNumberPatterns([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object object)
{
    std::unordered_map<std::string, std::string> systemNumberPatterns;
    I18nErrorCode err = I18nServiceAbilityClient::GetSystemNumberPatterns(systemNumberPatterns);
    if (err == I18nErrorCode::NOT_SYSTEM_APP) {
        ErrorUtil::AniThrow(env, I18N_NOT_SYSTEM_APP, "", "");
        return nullptr;
    }
    return VariableConverter::CreateAniMap(env, systemNumberPatterns);
}

void I18nSystemAddon::SetSystemNumberPattern([[maybe_unused]] ani_env *env,
    [[maybe_unused]] ani_object object, ani_string aniPattern)
{
    std::string pattern = VariableConverter::AniStrToString(env, aniPattern);
    if (pattern.empty()) {
        HILOG_ERROR_I18N("I18nSystemAddon::SetSystemNumberPattern: pattern is empty.");
        return;
    }
    I18nErrorCode err = I18nServiceAbilityClient::SetSystemNumberPattern(pattern);
    if (err == I18nErrorCode::SUCCESS) {
        return;
    }
    switch (err) {
        case I18nErrorCode::NOT_SYSTEM_APP:
            ErrorUtil::AniThrow(env, I18N_NOT_SYSTEM_APP, "", "");
            break;
        case I18nErrorCode::NO_PERMISSION:
            ErrorUtil::AniThrow(env, I18N_NO_PERMISSION, "", "");
            break;
        case I18nErrorCode::INVALID_PARAM:
            ErrorUtil::AniThrow(env, I18N_NOT_VALID_NEW, "pattern", "in a number pattern supported by the system");
            break;
        default:
            HILOG_ERROR_I18N("I18nSystemAddon::SetSystemNumberPattern: Set system number pattern failed.");
    }
}

ani_string I18nSystemAddon::GetUsingNumberPattern([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object object)
{
    std::string usingNumberPattern;
    I18nErrorCode err = I18nServiceAbilityClient::GetUsingNumberPattern(usingNumberPattern);
    if (err == I18nErrorCode::NOT_SYSTEM_APP) {
        ErrorUtil::AniThrow(env, I18N_NOT_SYSTEM_APP, "", "");
        return nullptr;
    }
    return VariableConverter::StringToAniStr(env, usingNumberPattern);
}

ani_object I18nSystemAddon::GetSystemNumericalDatePatterns([[maybe_unused]] ani_env *env,
    [[maybe_unused]] ani_object object)
{
    std::unordered_map<std::string, std::string> numericalDatePatterns;
    I18nErrorCode err = I18nServiceAbilityClient::GetSystemNumericalDatePatterns(numericalDatePatterns);
    if (err == I18nErrorCode::NOT_SYSTEM_APP) {
        ErrorUtil::AniThrow(env, I18N_NOT_SYSTEM_APP, "", "");
        return nullptr;
    }
    return VariableConverter::CreateAniMap(env, numericalDatePatterns);
}

void I18nSystemAddon::SetSystemNumericalDatePattern([[maybe_unused]] ani_env *env,
    [[maybe_unused]] ani_object object, ani_string aniIdentifier)
{
    std::string identifier = VariableConverter::AniStrToString(env, aniIdentifier);
    if (identifier.empty()) {
        HILOG_ERROR_I18N("I18nSystemAddon::SetSystemNumericalDatePattern: identifier is empty");
        return;
    }
    I18nErrorCode err = I18nServiceAbilityClient::SetSystemNumericalDatePattern(identifier);
    if (err == I18nErrorCode::SUCCESS) {
        return;
    }
    switch (err) {
        case I18nErrorCode::NOT_SYSTEM_APP:
            ErrorUtil::AniThrow(env, I18N_NOT_SYSTEM_APP, "", "");
            break;
        case I18nErrorCode::NO_PERMISSION:
            ErrorUtil::AniThrow(env, I18N_NO_PERMISSION, "", "");
            break;
        case I18nErrorCode::INVALID_PARAM:
            ErrorUtil::AniThrow(env, I18N_NOT_VALID_NEW, "identifier", "in a numerical date pattern supported by "
                "the system");
            break;
        default:
            HILOG_ERROR_I18N(
                "I18nSystemAddon::SetSystemNumericalDatePattern: Set system numerical date pattern failed.");
    }
}

ani_string I18nSystemAddon::GetUsingNumericalDatePattern([[maybe_unused]] ani_env *env,
    [[maybe_unused]] ani_object object)
{
    std::string identifier;
    I18nErrorCode err = I18nServiceAbilityClient::GetUsingNumericalDatePattern(identifier);
    if (err == I18nErrorCode::NOT_SYSTEM_APP) {
        ErrorUtil::AniThrow(env, I18N_NOT_SYSTEM_APP, "", "");
        return nullptr;
    }
    return VariableConverter::StringToAniStr(env, identifier);
}

ani_object I18nSystemAddon::GetSystemLocaleInstance([[maybe_unused]] ani_env *env,
    [[maybe_unused]] ani_object object)
{
    static const char *className = "std.core.Intl.Locale";
    ani_class cls;
    if (ANI_OK != env->FindClass(className, &cls)) {
        HILOG_ERROR_I18N("GetSystemLocaleInstance: Find class '%{public}s' failed", className);
        return nullptr;
    }

    ani_method ctor;
    if (ANI_OK != env->Class_FindMethod(cls, "<ctor>",
        "X{C{std.core.Intl.Locale}C{std.core.String}}C{std.core.Intl.LocaleOptions}:", &ctor)) {
        HILOG_ERROR_I18N("GetSystemLocaleInstance: Find method '<ctor>' failed");
        return nullptr;
    }
    ani_ref undefinedRef;
    if (ANI_OK != env->GetUndefined(&undefinedRef)) {
        HILOG_ERROR_I18N("GetSystemLocaleInstance: GetUndefined failed");
        return nullptr;
    }
    std::string locale = LocaleConfig::GetSystemLocaleInstanceTag();
    ani_string localeAniStr = VariableConverter::StringToAniStr(env, locale);
    if (localeAniStr == nullptr) {
        HILOG_ERROR_I18N("GetSystemLocaleInstance: Get ani string failed");
        return nullptr;
    }
    ani_object ret;
    if (ANI_OK != env->Object_New(cls, ctor, &ret, localeAniStr, undefinedRef)) {
        HILOG_ERROR_I18N("GetSystemLocaleInstance: New object '%{public}s' failed", className);
        return nullptr;
    }
    return ret;
}

ani_status I18nSystemAddon::BindContextSystem(ani_env *env)
{
    static const char *className = "@ohos.i18n.i18n.System";
    ani_class cls;
    if (ANI_OK != env->FindClass(className, &cls)) {
        HILOG_ERROR_I18N("BindContextSystem: Find class '%{public}s' failed", className);
        return ANI_ERROR;
    }

    if (ANI_OK != env->Class_BindStaticNativeMethods(cls, METHODS.data(), METHODS.size())) {
        HILOG_ERROR_I18N("BindContextSystem: Cannot bind native methods to '%{public}s'", className);
        return ANI_ERROR;
    }

    return ANI_OK;
}
