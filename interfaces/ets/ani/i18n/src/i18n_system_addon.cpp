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
        HILOG_ERROR_I18N("Reference IsUndefined failed");
        return nullptr;
    }
    if (!isUndefined) {
        ani_boolean value;
        if (ANI_OK != env->Object_CallMethodByName_Boolean(sentenceCase, "unboxed", ":Z", &value)) {
            HILOG_ERROR_I18N("Unboxed Boolean failed");
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
        HILOG_ERROR_I18N("Reference IsUndefined failed");
        return nullptr;
    }
    if (!isUndefined) {
        ani_boolean value;
        if (ANI_OK != env->Object_CallMethodByName_Boolean(sentenceCase, "unboxed", ":Z", &value)) {
            HILOG_ERROR_I18N("Unboxed Boolean failed");
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
        HILOG_ERROR_I18N("Reference IsUndefined failed");
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
    HILOG_INFO_I18N("I18nSystemAddon::SetSystemLanguageImpl with code %{public}d", static_cast<int32_t>(err));
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
    HILOG_INFO_I18N("I18nSystemAddon::SetSystemRegionImpl with code %{public}d", static_cast<int32_t>(err));
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
    int index = 1000000;
    ani_boolean isUndefined;
    if (ANI_OK != env->Reference_IsUndefined(number, &isUndefined)) {
        HILOG_ERROR_I18N("Reference IsUndefined failed");
        return;
    }
    if (!isUndefined) {
        ani_double value;
        if (ANI_OK != env->Object_CallMethodByName_Double(number, "unboxed", ":D", &value)) {
            HILOG_ERROR_I18N("Unboxed Double failed");
            return;
        }
        index = static_cast<int>(value);
    }
    I18nErrorCode err = I18nServiceAbilityClient::AddPreferredLanguage(
        VariableConverter::AniStrToString(env, language), index);
    HILOG_INFO_I18N("I18nSystemAddon::AddPreferredLanguageImpl with code %{public}d", static_cast<int32_t>(err));
    if (err == I18nErrorCode::NO_PERMISSION) {
        ErrorUtil::AniThrow(env, I18N_NO_PERMISSION, "", "");
    }
    if (err != I18nErrorCode::SUCCESS) {
        ErrorUtil::AniThrow(env, I18N_NOT_VALID, "language", "a valid language");
    }
}

void I18nSystemAddon::RemovePreferredLanguage([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object object,
    ani_double index)
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
        HILOG_ERROR_I18N("SetAppPreferredLanguage does not support this locale");
        ErrorUtil::AniThrow(env, I18N_NOT_VALID, "language", "a valid language");
        return;
    }

#ifdef SUPPORT_APP_PREFERRED_LANGUAGE
    I18nErrorCode errCode = I18nErrorCode::SUCCESS;
    PreferredLanguage::SetAppPreferredLanguage(localeTag, errCode);
    if (errCode != I18nErrorCode::SUCCESS) {
        HILOG_ERROR_I18N("SetAppPreferredLanguage: set app language to i18n app preferences failed.");
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
    HILOG_INFO_I18N("I18nSystemAddon::SetUsingLocalDigitAddonImpl with code %{public}d",
        static_cast<int32_t>(err));
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
        HILOG_ERROR_I18N("Reference IsUndefined failed");
        return nullptr;
    }
    if (isUndefined) {
        simplifiedLanguage = LocaleConfig::GetSimplifiedSystemLanguage();
    } else {
        int code = 0;
        simplifiedLanguage = LocaleConfig::GetSimplifiedLanguage(
            VariableConverter::AniStrToString(env, language), code);
        if (code != 0) {
            HILOG_ERROR_I18N("GetSimplifiedLanguage: language is not valid");
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
    static const char *enumName = "L@ohos/i18n/i18n/TemperatureType;";
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
    static const char *enumName = "L@ohos/i18n/i18n/WeekDay;";
    return VariableConverter::GetEnumItemByIndex(env, enumName, index);
}

ani_status I18nSystemAddon::BindContextSystem(ani_env *env)
{
    static const char *className = "L@ohos/i18n/i18n/System;";
    ani_class cls;
    if (ANI_OK != env->FindClass(className, &cls)) {
        HILOG_ERROR_I18N("Find class '%{public}s' failed", className);
        return ANI_ERROR;
    }

    std::array methods = {
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
        ani_native_function { "getPreferredLanguageList", nullptr, reinterpret_cast<void *>(GetPreferredLanguageList) },
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
    };

    if (ANI_OK != env->Class_BindNativeMethods(cls, methods.data(), methods.size())) {
        HILOG_ERROR_I18N("Cannot bind native methods to '%{public}s'", className);
        return ANI_ERROR;
    };

    return ANI_OK;
}
