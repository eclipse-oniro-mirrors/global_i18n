/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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

#include "api_version.h"
#include "error_util.h"
#include "i18n_hilog.h"
#include "i18n_service_ability_client.h"
#include "i18n_service_ability_load_manager.h"
#include "i18n_system_addon.h"
#include "js_utils.h"
#include "locale_config.h"
#include "locale_info.h"
#include "preferred_language.h"
#include "utils.h"
#include "variable_convertor.h"

namespace OHOS {
namespace Global {
namespace I18n {
const std::string I18nSystemAddon::NAME_OF_TEMPERATURE = "type";
const std::string I18nSystemAddon::TYPE_OF_TEMPERATURE = "TemperatureType";
const std::string I18nSystemAddon::NAME_OF_WEEKDAY = "type";
const std::string I18nSystemAddon::TYPE_OF_WEEKDAY = "WeekDay";

I18nSystemAddon::I18nSystemAddon() {}

I18nSystemAddon::~I18nSystemAddon() {}

void I18nSystemAddon::Destructor(napi_env env, void *nativeObject, void *finalize_hint)
{
    if (!nativeObject) {
        return;
    }
    delete reinterpret_cast<I18nSystemAddon *>(nativeObject);
    nativeObject = nullptr;
}

napi_value I18nSystemAddon::InitI18nSystem(napi_env env, napi_value exports)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_STATIC_FUNCTION("getDisplayCountry", GetDisplayCountryWithError),
        DECLARE_NAPI_STATIC_FUNCTION("getDisplayLanguage", GetDisplayLanguageWithError),
        DECLARE_NAPI_STATIC_FUNCTION("getSystemLanguages", GetSystemLanguages),
        DECLARE_NAPI_STATIC_FUNCTION("getSystemCountries", GetSystemCountriesWithError),
        DECLARE_NAPI_STATIC_FUNCTION("isSuggested", IsSuggestedWithError),
        DECLARE_NAPI_STATIC_FUNCTION("getSystemLanguage", GetSystemLanguage),
        DECLARE_NAPI_STATIC_FUNCTION("setSystemLanguage", SetSystemLanguageWithError),
        DECLARE_NAPI_STATIC_FUNCTION("getSystemRegion", GetSystemRegion),
        DECLARE_NAPI_STATIC_FUNCTION("setSystemRegion", SetSystemRegionWithError),
        DECLARE_NAPI_STATIC_FUNCTION("getSystemLocale", GetSystemLocale),
        DECLARE_NAPI_STATIC_FUNCTION("setSystemLocale", SetSystemLocaleWithError),
        DECLARE_NAPI_STATIC_FUNCTION("is24HourClock", Is24HourClock),
        DECLARE_NAPI_STATIC_FUNCTION("set24HourClock", Set24HourClockWithError),
        DECLARE_NAPI_STATIC_FUNCTION("addPreferredLanguage", AddPreferredLanguageWithError),
        DECLARE_NAPI_STATIC_FUNCTION("removePreferredLanguage", RemovePreferredLanguageWithError),
        DECLARE_NAPI_STATIC_FUNCTION("getPreferredLanguageList", GetPreferredLanguageList),
        DECLARE_NAPI_STATIC_FUNCTION("setAppPreferredLanguage", SetAppPreferredLanguage),
        DECLARE_NAPI_STATIC_FUNCTION("getFirstPreferredLanguage", GetFirstPreferredLanguage),
        DECLARE_NAPI_STATIC_FUNCTION("getAppPreferredLanguage", GetAppPreferredLanguage),
        DECLARE_NAPI_STATIC_FUNCTION("setUsingLocalDigit", SetUsingLocalDigitAddonWithError),
        DECLARE_NAPI_STATIC_FUNCTION("getUsingLocalDigit", GetUsingLocalDigitAddon),
        DECLARE_NAPI_STATIC_FUNCTION("setTemperatureType", SetTemperatureType),
        DECLARE_NAPI_STATIC_FUNCTION("getTemperatureType", GetTemperatureType),
        DECLARE_NAPI_STATIC_FUNCTION("getTemperatureName", GetTemperatureName),
        DECLARE_NAPI_STATIC_FUNCTION("setFirstDayOfWeek", SetFirstDayOfWeek),
        DECLARE_NAPI_STATIC_FUNCTION("getFirstDayOfWeek", GetFirstDayOfWeek),
        DECLARE_NAPI_STATIC_FUNCTION("getSimplifiedLanguage", GetSimplifiedLanguage),
    };
    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, "System", NAPI_AUTO_LENGTH, JSUtils::DefaultConstructor, nullptr,
        sizeof(properties) / sizeof(napi_property_descriptor), properties, &constructor);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("Define class failed when InitSystem.");
        return nullptr;
    }

    status = napi_set_named_property(env, exports, "System", constructor);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("Set property failed when InitSystem.");
        return nullptr;
    }
    return exports;
}

napi_value I18nSystemAddon::GetDisplayCountry(napi_env env, napi_callback_info info)
{
    return I18nSystemAddon::GetDisplayCountryImpl(env, info, false);
}

napi_value I18nSystemAddon::GetDisplayCountryWithError(napi_env env, napi_callback_info info)
{
    return I18nSystemAddon::GetDisplayCountryImpl(env, info, true);
}

napi_value I18nSystemAddon::GetDisplayLanguage(napi_env env, napi_callback_info info)
{
    return I18nSystemAddon::GetDisplayLanguageImpl(env, info, false);
}

napi_value I18nSystemAddon::GetDisplayLanguageWithError(napi_env env, napi_callback_info info)
{
    return I18nSystemAddon::GetDisplayLanguageImpl(env, info, true);
}

napi_value I18nSystemAddon::GetSystemLanguages(napi_env env, napi_callback_info info)
{
    std::unordered_set<std::string> systemLanguages = LocaleConfig::GetSystemLanguages();
    napi_value result = nullptr;
    napi_status status = napi_create_array_with_length(env, systemLanguages.size(), &result);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("GetSystemLanguages: Failed to create array");
        return nullptr;
    }
    size_t pos = 0;
    for (const auto& language : systemLanguages) {
        napi_value value = nullptr;
        status = napi_create_string_utf8(env, language.c_str(), NAPI_AUTO_LENGTH, &value);
        if (status != napi_ok) {
            HILOG_ERROR_I18N("GetSystemLanguages: Failed to create string item");
            return nullptr;
        }
        status = napi_set_element(env, result, pos, value);
        if (status != napi_ok) {
            HILOG_ERROR_I18N("GetSystemLanguages: Failed to set array item");
            return nullptr;
        }
        pos++;
    }
    return result;
}

napi_value I18nSystemAddon::GetSystemCountries(napi_env env, napi_callback_info info)
{
    return I18nSystemAddon::GetSystemCountriesImpl(env, info, false);
}

napi_value I18nSystemAddon::GetSystemCountriesWithError(napi_env env, napi_callback_info info)
{
    return I18nSystemAddon::GetSystemCountriesImpl(env, info, true);
}

napi_value I18nSystemAddon::IsSuggested(napi_env env, napi_callback_info info)
{
    return I18nSystemAddon::IsSuggestedImpl(env, info, false);
}

napi_value I18nSystemAddon::IsSuggestedWithError(napi_env env, napi_callback_info info)
{
    return I18nSystemAddon::IsSuggestedImpl(env, info, true);
}

napi_value I18nSystemAddon::GetSystemLanguage(napi_env env, napi_callback_info info)
{
    std::string value = LocaleConfig::GetSystemLanguage();
    napi_value result = nullptr;
    napi_status status = napi_create_string_utf8(env, value.c_str(), NAPI_AUTO_LENGTH, &result);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("GetSystemLanguage: Failed to create string item");
        return nullptr;
    }
    return result;
}

napi_value I18nSystemAddon::SetSystemLanguage(napi_env env, napi_callback_info info)
{
    return I18nSystemAddon::SetSystemLanguageImpl(env, info, false);
}

napi_value I18nSystemAddon::SetSystemLanguageWithError(napi_env env, napi_callback_info info)
{
    return I18nSystemAddon::SetSystemLanguageImpl(env, info, true);
}

napi_value I18nSystemAddon::GetSystemRegion(napi_env env, napi_callback_info info)
{
    std::string value = LocaleConfig::GetSystemRegion();
    napi_value result = nullptr;
    napi_status status = napi_create_string_utf8(env, value.c_str(), NAPI_AUTO_LENGTH, &result);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("GetSystemRegion: Failed to create string item");
        return nullptr;
    }
    return result;
}

napi_value I18nSystemAddon::SetSystemRegion(napi_env env, napi_callback_info info)
{
    return I18nSystemAddon::SetSystemRegionImpl(env, info, false);
}

napi_value I18nSystemAddon::SetSystemRegionWithError(napi_env env, napi_callback_info info)
{
    return I18nSystemAddon::SetSystemRegionImpl(env, info, true);
}

napi_value I18nSystemAddon::GetSystemLocale(napi_env env, napi_callback_info info)
{
    std::string value = LocaleConfig::GetSystemLocale();
    napi_value result = nullptr;
    napi_status status = napi_create_string_utf8(env, value.c_str(), NAPI_AUTO_LENGTH, &result);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("GetSystemLocale: Failed to create string item");
        return nullptr;
    }
    return result;
}

napi_value I18nSystemAddon::SetSystemLocale(napi_env env, napi_callback_info info)
{
    return I18nSystemAddon::SetSystemLocaleImpl(env, info, false);
}

napi_value I18nSystemAddon::SetSystemLocaleWithError(napi_env env, napi_callback_info info)
{
    return I18nSystemAddon::SetSystemLocaleImpl(env, info, true);
}

napi_value I18nSystemAddon::Is24HourClock(napi_env env, napi_callback_info info)
{
    bool is24HourClock = LocaleConfig::Is24HourClock();
    napi_value result = nullptr;
    napi_status status = napi_get_boolean(env, is24HourClock, &result);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("Failed to create boolean item");
        return nullptr;
    }
    return result;
}

napi_value I18nSystemAddon::Set24HourClock(napi_env env, napi_callback_info info)
{
    return I18nSystemAddon::Set24HourClockImpl(env, info, false);
}

napi_value I18nSystemAddon::Set24HourClockWithError(napi_env env, napi_callback_info info)
{
    return I18nSystemAddon::Set24HourClockImpl(env, info, true);
}

napi_value I18nSystemAddon::AddPreferredLanguage(napi_env env, napi_callback_info info)
{
    return I18nSystemAddon::AddPreferredLanguageImpl(env, info, false);
}

napi_value I18nSystemAddon::AddPreferredLanguageWithError(napi_env env, napi_callback_info info)
{
    return I18nSystemAddon::AddPreferredLanguageImpl(env, info, true);
}

napi_value I18nSystemAddon::RemovePreferredLanguage(napi_env env, napi_callback_info info)
{
    return I18nSystemAddon::RemovePreferredLanguageImpl(env, info, false);
}

napi_value I18nSystemAddon::RemovePreferredLanguageWithError(napi_env env, napi_callback_info info)
{
    return I18nSystemAddon::RemovePreferredLanguageImpl(env, info, true);
}

napi_value I18nSystemAddon::GetPreferredLanguageList(napi_env env, napi_callback_info info)
{
    std::vector<std::string> languageList = PreferredLanguage::GetPreferredLanguageList();
    napi_value result = nullptr;
    napi_status status = napi_ok;
    status = napi_create_array_with_length(env, languageList.size(), &result);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("getPreferrdLanguageList: create array failed");
        return nullptr;
    }
    for (size_t i = 0; i < languageList.size(); i++) {
        napi_value value = nullptr;
        status = napi_create_string_utf8(env, languageList[i].c_str(), NAPI_AUTO_LENGTH, &value);
        if (status != napi_ok) {
            HILOG_ERROR_I18N("getPreferrdLanguageList: create string failed");
            return nullptr;
        }
        status = napi_set_element(env, result, i, value);
        if (status != napi_ok) {
            HILOG_ERROR_I18N("GetPreferredLanguageList: set array item failed");
            return nullptr;
        }
    }
    return result;
}

napi_value I18nSystemAddon::GetFirstPreferredLanguage(napi_env env, napi_callback_info info)
{
    std::string language = PreferredLanguage::GetFirstPreferredLanguage();
    napi_value result = nullptr;
    napi_status status = napi_ok;
    status = napi_create_string_utf8(env, language.c_str(), NAPI_AUTO_LENGTH, &result);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("getFirstPreferrdLanguage: create string result failed");
        return nullptr;
    }
    return result;
}

napi_value I18nSystemAddon::SetAppPreferredLanguage(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = { 0 };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    napi_valuetype valueType = napi_valuetype::napi_undefined;
    napi_typeof(env, argv[0], &valueType);
    if (valueType != napi_valuetype::napi_string) {
        HILOG_ERROR_I18N("SetAppPreferredLanguage Parameter type is not string");
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, "language", "string", true);
        return nullptr;
    }
    int code = 0;
    std::string localeTag = VariableConvertor::GetString(env, argv[0], code);
    if (code) {
        HILOG_ERROR_I18N("SetAppPreferredLanguage can't get string from js param");
        return nullptr;
    }
    UErrorCode icuStatus = U_ZERO_ERROR;
    icu::Locale locale = icu::Locale::forLanguageTag(localeTag.data(), icuStatus);
    if (U_FAILURE(icuStatus) || !(IsValidLocaleTag(locale) || localeTag.compare("default") == 0)) {
        HILOG_ERROR_I18N("SetAppPreferredLanguage does not support this locale");
        ErrorUtil::NapiThrow(env, I18N_NOT_VALID, "language", "a valid language", true);
        return nullptr;
    }
#ifdef SUPPORT_APP_PREFERRED_LANGUAGE
    I18nErrorCode errCode = I18nErrorCode::SUCCESS;
    PreferredLanguage::SetAppPreferredLanguage(localeTag, errCode);
    if (errCode != I18nErrorCode::SUCCESS) {
        HILOG_ERROR_I18N("SetAppPreferredLanguage: set app language to i18n app preferences failed.");
    }
#endif
    return nullptr;
}

napi_value I18nSystemAddon::GetAppPreferredLanguage(napi_env env, napi_callback_info info)
{
#ifdef SUPPORT_APP_PREFERRED_LANGUAGE
    std::string language = PreferredLanguage::GetAppPreferredLanguage();
#else
    std::string language = PreferredLanguage::GetFirstPreferredLanguage();
#endif
    napi_value result = nullptr;
    napi_status status = napi_ok;
    status = napi_create_string_utf8(env, language.c_str(), NAPI_AUTO_LENGTH, &result);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("getAppPreferrdLanguage: create string result failed");
        return nullptr;
    }
    return result;
}

napi_value I18nSystemAddon::SetUsingLocalDigitAddon(napi_env env, napi_callback_info info)
{
    return I18nSystemAddon::SetUsingLocalDigitAddonImpl(env, info, false);
}

napi_value I18nSystemAddon::SetUsingLocalDigitAddonWithError(napi_env env, napi_callback_info info)
{
    return I18nSystemAddon::SetUsingLocalDigitAddonImpl(env, info, true);
}

napi_value I18nSystemAddon::GetUsingLocalDigitAddon(napi_env env, napi_callback_info info)
{
    bool res = LocaleConfig::GetUsingLocalDigit();
    napi_value value = nullptr;
    napi_status status = napi_get_boolean(env, res, &value);
    if (status != napi_ok) {
        return nullptr;
    }
    return value;
}

napi_value I18nSystemAddon::GetDisplayCountryImpl(napi_env env, napi_callback_info info, bool throwError)
{
    // Need to get three parameters to get the display country.
    size_t argc = 3;
    napi_value argv[3] = { nullptr };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        return VariableConvertor::CreateString(env, "");
    } else if (argc < FUNC_ARGS_COUNT) {
        ErrorUtil::NapiNotFoundError(env, I18N_NOT_FOUND, "country or locale", throwError);
        return VariableConvertor::CreateString(env, "");
    }
    size_t len = 0;
    VariableConvertor::VerifyType(env, "country", napi_valuetype::napi_string, argv[0]);
    napi_get_value_string_utf8(env, argv[0], nullptr, 0, &len);
    std::vector<char> localeBuf(len + 1);
    if (!GetCountryNapiValueWithError(env, argv[0], len, localeBuf.data(), "0")) {
        return VariableConvertor::CreateString(env, "");
    }
    VariableConvertor::VerifyType(env, "locale", napi_valuetype::napi_string, argv[1]);
    napi_get_value_string_utf8(env, argv[1], nullptr, 0, &len);
    std::vector<char> displayLocaleBuf(len + 1);
    if (!GetCountryNapiValueWithError(env, argv[1], len, displayLocaleBuf.data(), "1")) {
        return VariableConvertor::CreateString(env, "");
    }
    LocaleInfo localeInfo(localeBuf.data());
    if (!LocaleConfig::IsValidRegion(localeBuf.data()) && localeInfo.GetRegion().empty()) {
        ErrorUtil::NapiThrow(env, I18N_NOT_VALID, "country", "a valid region", throwError);
        return VariableConvertor::CreateString(env, "");
    } else if (!LocaleConfig::IsValidTag(displayLocaleBuf.data())) {
        ErrorUtil::NapiThrow(env, I18N_NOT_VALID, "locale", "a valid locale", throwError);
        return VariableConvertor::CreateString(env, "");
    }
    bool sentenceCase = true;
    if (argc > FUNC_ARGS_COUNT) {
        napi_get_value_bool(env, argv[FUNC_ARGS_COUNT], &sentenceCase);
    }
    std::string value = LocaleConfig::GetDisplayRegion(localeBuf.data(), displayLocaleBuf.data(), sentenceCase);
    if (value.length() == 0) {
        HILOG_ERROR_I18N("GetDisplayCountryImpl: result is empty.");
        return VariableConvertor::CreateString(env, "");
    }
    napi_value result = nullptr;
    status = napi_create_string_utf8(env, value.c_str(), NAPI_AUTO_LENGTH, &result);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("GetDisplayCountryImpl: Failed to create string item");
        return VariableConvertor::CreateString(env, "");
    }
    return result;
}

bool I18nSystemAddon::GetCountryNapiValueWithError(napi_env env, napi_value napiValue, size_t len, char* valueBuf,
    std::string index)
{
    napi_status status = napi_get_value_string_utf8(env, napiValue, valueBuf, len + 1, &len);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("GetDisplayCountryImpl: Failed to get string item argv[%{public}s]", index.c_str());
        return false;
    }
    return true;
}

napi_value I18nSystemAddon::GetDisplayLanguageImpl(napi_env env, napi_callback_info info, bool throwError)
{
    // Need to get three parameters to get the display Language.
    size_t argc = 3;
    napi_value argv[3] = { nullptr };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        return nullptr;
    } else if (argc < FUNC_ARGS_COUNT) {
        ErrorUtil::NapiNotFoundError(env, I18N_NOT_FOUND, "language or locale", throwError);
        return nullptr;
    }
    VariableConvertor::VerifyType(env, "language", napi_valuetype::napi_string, argv[0]);
    size_t len = 0;
    napi_get_value_string_utf8(env, argv[0], nullptr, 0, &len);
    std::vector<char> localeBuf(len + 1);
    if (!GetNapiStringValueWithError(env, argv[0], len, localeBuf.data(), "0")) {
        return nullptr;
    }
    VariableConvertor::VerifyType(env, "locale", napi_valuetype::napi_string, argv[1]);
    napi_get_value_string_utf8(env, argv[1], nullptr, 0, &len);
    std::vector<char> displayLocaleBuf(len + 1);
    if (!GetNapiStringValueWithError(env, argv[1], len, displayLocaleBuf.data(), "1")) {
        return nullptr;
    }
    if (!LocaleConfig::IsValidTag(displayLocaleBuf.data())) {
        ErrorUtil::NapiThrow(env, I18N_NOT_VALID, "locale", "a valid locale", throwError);
        return nullptr;
    }
    bool sentenceCase = true;
    if (argc > FUNC_ARGS_COUNT) {
        napi_get_value_bool(env, argv[FUNC_ARGS_COUNT], &sentenceCase);
    }
    std::string value = LocaleConfig::GetDisplayLanguage(localeBuf.data(), displayLocaleBuf.data(), sentenceCase);
    if (value.length() == 0) {
        HILOG_ERROR_I18N("GetDisplayLanguageImpl: result is empty.");
        return nullptr;
    }
    napi_value result = nullptr;
    status = napi_create_string_utf8(env, value.c_str(), NAPI_AUTO_LENGTH, &result);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("GetDisplayLanguageImpl: Failed to create string item");
        return nullptr;
    }
    return result;
}

bool I18nSystemAddon::GetNapiStringValueWithError(napi_env env, napi_value napiValue, size_t len, char* valueBuf,
    std::string index)
{
    napi_status status = napi_get_value_string_utf8(env, napiValue, valueBuf, len + 1, &len);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("GetDisplayLanguageImpl: Failed to get string item argv[%{public}s]", index.c_str());
        return false;
    }
    return true;
}

napi_value I18nSystemAddon::GetSystemCountriesImpl(napi_env env, napi_callback_info info, bool throwError)
{
    size_t argc = 1;
    napi_value argv[1] = { nullptr };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        return nullptr;
    } else if (argc < 1) {
        ErrorUtil::NapiNotFoundError(env, I18N_NOT_FOUND, "language", throwError);
        return nullptr;
    }
    VariableConvertor::VerifyType(env, "language", napi_valuetype::napi_string, argv[0]);
    int32_t code = 0;
    std::string language = VariableConvertor::GetString(env, argv[0], code);
    if (code != 0) {
        HILOG_ERROR_I18N("GetSystemCountriesImpl: Get argv[0] failed.");
        return nullptr;
    }
    std::unordered_set<std::string> systemCountries = LocaleConfig::GetSystemCountries(language);
    napi_value result = nullptr;
    status = napi_create_array_with_length(env, systemCountries.size(), &result);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("GetSystemCountriesImpl: Failed to create array");
        return nullptr;
    }
    size_t pos = 0;
    for (const auto& country : systemCountries) {
        napi_value value = nullptr;
        status = napi_create_string_utf8(env, country.c_str(), NAPI_AUTO_LENGTH, &value);
        if (status != napi_ok) {
            HILOG_ERROR_I18N("GetSystemCountries: Failed to create string item");
            return nullptr;
        }
        status = napi_set_element(env, result, pos, value);
        if (status != napi_ok) {
            HILOG_ERROR_I18N("GetSystemCountries: Failed to set array item");
            return nullptr;
        }
        pos++;
    }
    return result;
}

bool I18nSystemAddon::GetApiCheckResult()
{
    ApiCompareResult result = ApiVersion::CheckApiTargetVersion("isSuggested");
    return result == ApiCompareResult::GREATER_THAN;
}

napi_value I18nSystemAddon::IsSuggestedImpl(napi_env env, napi_callback_info info, bool throwError)
{
    // Need to get two parameters to check is suggested or not.
    size_t argc = 2;
    napi_value argv[2] = { nullptr };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        return nullptr;
    } else if (argc < 1) {
        ErrorUtil::NapiNotFoundError(env, I18N_NOT_FOUND, "language", throwError);
        return nullptr;
    }
    VariableConvertor::VerifyType(env, "language", napi_valuetype::napi_string, argv[0]);
    int32_t code = 0;
    std::string language = VariableConvertor::GetString(env, argv[0], code);
    if (code != 0) {
        HILOG_ERROR_I18N("IsSuggestedImpl: Failed to get string argv[0].");
        return nullptr;
    }
    bool isSuggested = false;
    if (VariableConvertor::CheckNapiIsNull(env, argv[1])) {
        std::string region = VariableConvertor::GetString(env, argv[1], code);
        if (code != 0) {
            HILOG_ERROR_I18N("IsSuggestedImpl: Failed to get string argv[1].");
            return nullptr;
        }
        isSuggested = LocaleConfig::IsSuggestedV15(language, region);
    } else {
        isSuggested = LocaleConfig::IsSuggestedV15(language);
    }
    napi_value result = nullptr;
    status = napi_get_boolean(env, isSuggested, &result);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("Create case first boolean value failed");
        return nullptr;
    }
    return result;
}

bool I18nSystemAddon::IsSuggestedByApiVersion(const std::string &language, bool newApi)
{
    if (newApi) {
        return LocaleConfig::IsSuggested(language);
    }
    return LocaleConfig::IsSuggestedV15(language);
}

bool I18nSystemAddon::IsSuggestedByApiVersion(const std::string &language,
    const std::string &region, bool newApi)
{
    if (newApi) {
        return LocaleConfig::IsSuggested(language, region);
    }
    return LocaleConfig::IsSuggestedV15(language, region);
}

napi_value I18nSystemAddon::SetSystemLanguageImpl(napi_env env, napi_callback_info info, bool throwError)
{
    size_t argc = 1;
    napi_value argv[1] = { 0 };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        return nullptr;
    }
    if (argc < 1) {
        HILOG_ERROR_I18N("SetSystemLanguageImpl: Missing parameter");
        ErrorUtil::NapiNotFoundError(env, I18N_NOT_FOUND, "language", throwError);
        return nullptr;
    }
    size_t len = 0;
    status = napi_get_value_string_utf8(env, argv[0], nullptr, 0, &len);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("SetSystemLanguageImpl: Failed to get string len argv[0].");
        return nullptr;
    }
    std::vector<char> languageBuf(len + 1);
    status = napi_get_value_string_utf8(env, argv[0], languageBuf.data(), len + 1, &len);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("SetSystemLanguageImpl: Failed to get string item");
        return nullptr;
    }
    I18nErrorCode err = I18nServiceAbilityClient::SetSystemLanguage(languageBuf.data());
    HILOG_INFO_I18N("I18nSystemAddon::SetSystemLanguageImpl with code %{public}d", static_cast<int32_t>(err));
    bool success = err == I18nErrorCode::SUCCESS;
    if (throwError) {
        if (!success) {
            ErrorUtil::NapiThrow(env, I18N_NO_PERMISSION, "", "", throwError);
        }
        return nullptr;
    }
    napi_value result = nullptr;
    status = napi_get_boolean(env, success, &result);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("SetSystemLanguageImpl: Create set system language boolean value failed");
        return nullptr;
    }
    return result;
}

napi_value I18nSystemAddon::SetSystemRegionImpl(napi_env env, napi_callback_info info, bool throwError)
{
    size_t argc = 1;
    napi_value argv[1] = { nullptr };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        return nullptr;
    }
    if (argc < 1) {
        HILOG_ERROR_I18N("SetSystemRegionImpl: Missing parameter");
        ErrorUtil::NapiNotFoundError(env, I18N_NOT_FOUND, "region", throwError);
        return nullptr;
    }
    size_t len = 0;
    napi_get_value_string_utf8(env, argv[0], nullptr, 0, &len);
    std::vector<char> regionBuf(len + 1);
    status = napi_get_value_string_utf8(env, argv[0], regionBuf.data(), len + 1, &len);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("SetSystemRegionImpl: Failed to get string item");
        return nullptr;
    }
    I18nErrorCode err = I18nServiceAbilityClient::SetSystemRegion(regionBuf.data());
    HILOG_INFO_I18N("I18nSystemAddon::SetSystemRegionImpl with code %{public}d", static_cast<int32_t>(err));
    bool success = err == I18nErrorCode::SUCCESS;
    if (throwError) {
        if (!success) {
            ErrorUtil::NapiThrow(env, I18N_NO_PERMISSION, "", "", throwError);
        }
        return nullptr;
    }
    napi_value result = nullptr;
    status = napi_get_boolean(env, success, &result);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("SetSystemRegionImpl: Create set system language boolean value failed");
        return nullptr;
    }
    return result;
}

napi_value I18nSystemAddon::SetSystemLocaleImpl(napi_env env, napi_callback_info info, bool throwError)
{
    size_t argc = 1;
    napi_value argv[1] = { nullptr };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        return nullptr;
    }
    if (argc < 1) {
        HILOG_ERROR_I18N("SetSystemLocaleImpl: Missing parameter");
        ErrorUtil::NapiNotFoundError(env, I18N_NOT_FOUND, "locale", throwError);
        return nullptr;
    }
    size_t len = 0;
    napi_get_value_string_utf8(env, argv[0], nullptr, 0, &len);
    std::vector<char> localeBuf(len + 1);
    status = napi_get_value_string_utf8(env, argv[0], localeBuf.data(), len + 1, &len);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("SetSystemLocaleImpl: Failed to get string item");
        return nullptr;
    }
    I18nErrorCode err = I18nServiceAbilityClient::SetSystemLocale(localeBuf.data());
    HILOG_INFO_I18N("I18nSystemAddon::SetSystemLocaleImpl with code %{public}d", static_cast<int32_t>(err));
    bool success = err == I18nErrorCode::SUCCESS;
    if (throwError) {
        if (!success) {
            ErrorUtil::NapiThrow(env, I18N_NO_PERMISSION, "", "", throwError);
        }
        return nullptr;
    }
    napi_value result = nullptr;
    status = napi_get_boolean(env, success, &result);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("SetSystemLocaleImpl: Create set system language boolean value failed");
        return nullptr;
    }
    return result;
}

napi_value I18nSystemAddon::Set24HourClockImpl(napi_env env, napi_callback_info info, bool throwError)
{
    size_t argc = 1;
    napi_value argv[1] = { nullptr };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        return nullptr;
    }
    if (argc < 1) {
        HILOG_ERROR_I18N("Set24HourClockImpl: Missing parameter");
        ErrorUtil::NapiNotFoundError(env, I18N_NOT_FOUND, "option", throwError);
        return nullptr;
    }

    bool option = false;
    status = napi_get_value_bool(env, argv[0], &option);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("Set24HourClockImpl: Failed to get boolean item");
        return nullptr;
    }
    std::string optionStr = option ? "true" : "false";
    I18nErrorCode err = I18nServiceAbilityClient::Set24HourClock(optionStr);
    HILOG_INFO_I18N("I18nSystemAddon::Set24HourClock with code %{public}d", static_cast<int32_t>(err));
    bool success = err == I18nErrorCode::SUCCESS;
    if (throwError) {
        if (!success) {
            ErrorUtil::NapiThrow(env, I18N_NO_PERMISSION, "", "", throwError);
        }
        return nullptr;
    }
    napi_value result = nullptr;
    status = napi_get_boolean(env, success, &result);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("Create set 24HourClock boolean value failed");
        return nullptr;
    }
    return result;
}

napi_value I18nSystemAddon::AddPreferredLanguageImpl(napi_env env, napi_callback_info info, bool throwError)
{
    size_t argc = 2;
    napi_value argv[2] = { 0 };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        return nullptr;
    }

    std::string language;
    if (!ParseStringParam(env, argc > 0 ? argv[0] : nullptr, throwError, language)) {
        return nullptr;
    }

    int index = 1000000;
    if (VariableConvertor::CheckNapiIsNull(env, argc > 1 ? argv[1] : nullptr)) {
        status = napi_get_value_int32(env, argv[1], &index);
        if (status != napi_ok) {
            HILOG_ERROR_I18N("addPreferrdLanguage: get index failed");
            return nullptr;
        }
    }
    I18nErrorCode err = I18nServiceAbilityClient::AddPreferredLanguage(language.data(), index);
    HILOG_INFO_I18N("I18nSystemAddon::AddPreferredLanguageImpl with code %{public}d", static_cast<int32_t>(err));
    if (throwError) {
        if (err == I18nErrorCode::NO_PERMISSION || err == I18nErrorCode::NOT_SYSTEM_APP) {
            ErrorUtil::NapiThrow(env, I18N_NO_PERMISSION, "", "", throwError);
        }
        if (err != I18nErrorCode::SUCCESS) {
            ErrorUtil::NapiThrow(env, I18N_NOT_VALID, "language", "a valid language", throwError);
        }
        return nullptr;
    }
    bool addResult = true;
    if (err != I18nErrorCode::SUCCESS) {
        addResult = false;
    }
    napi_value result = nullptr;
    status = napi_get_boolean(env, addResult, &result);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("addPreferrdLanguage: create boolean result failed");
        return nullptr;
    }
    return result;
}

napi_value I18nSystemAddon::RemovePreferredLanguageImpl(napi_env env, napi_callback_info info, bool throwError)
{
    size_t argc = 1;
    napi_value argv[1] = { nullptr };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    int len = 0;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        return nullptr;
    }
    if (argc < 1) {
        HILOG_ERROR_I18N("RemovePreferredLanguageImpl: Missing parameter");
        ErrorUtil::NapiNotFoundError(env, I18N_NOT_FOUND, "index", throwError);
        return nullptr;
    }

    napi_valuetype valueType = napi_valuetype::napi_undefined;
    napi_typeof(env, argv[0], &valueType);
    if (valueType != napi_valuetype::napi_number) {
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, "index", "number", throwError);
        return nullptr;
    }
    int index = 1000000;
    status = napi_get_value_int32(env, argv[0], &index);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("removePreferrdLanguage: get index failed");
        return nullptr;
    }
    len = static_cast<int>(PreferredLanguage::GetPreferredLanguageList().size());
    if ((index < 0 || index > len - 1) && throwError) {
        ErrorUtil::NapiThrow(env, I18N_NOT_VALID, "index", "within the valid range", throwError);
        return nullptr;
    }
    I18nErrorCode err = I18nServiceAbilityClient::RemovePreferredLanguage(index);
    bool success = err == I18nErrorCode::SUCCESS;

    if (throwError) {
        if (!success) {
            ErrorUtil::NapiThrow(env, I18N_NO_PERMISSION, "", "", throwError);
        }
        return nullptr;
    }
    napi_value result = nullptr;
    status = napi_get_boolean(env, success, &result);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("removePreferrdLanguage: create boolean result failed");
        return nullptr;
    }
    return result;
}

napi_value I18nSystemAddon::SetUsingLocalDigitAddonImpl(napi_env env, napi_callback_info info, bool throwError)
{
    size_t argc = 1;
    napi_value argv[1] = { nullptr };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);

    if (argc < 1) {
        HILOG_ERROR_I18N("Invalid parameter nullptr");
        ErrorUtil::NapiNotFoundError(env, I18N_NOT_FOUND, "flag", throwError);
        return nullptr;
    }
    napi_valuetype valueType = napi_valuetype::napi_undefined;
    napi_typeof(env, argv[0], &valueType);
    if (valueType != napi_valuetype::napi_boolean) {
        HILOG_ERROR_I18N("Invalid parameter type");
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, "flag", "boolean", throwError);
        return nullptr;
    }
    bool flag = false;
    napi_status status = napi_get_value_bool(env, argv[0], &flag);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("Get parameter flag failed");
        return nullptr;
    }
    I18nErrorCode err = I18nServiceAbilityClient::SetUsingLocalDigit(flag);
    HILOG_INFO_I18N("I18nSystemAddon::SetUsingLocalDigitAddonImpl with code %{public}d",
        static_cast<int32_t>(err));
    bool res = err == I18nErrorCode::SUCCESS;
    if (throwError) {
        if (!res) {
            ErrorUtil::NapiThrow(env, I18N_NO_PERMISSION, "", "", throwError);
        }
        return nullptr;
    }
    napi_value value = nullptr;
    status = napi_get_boolean(env, res, &value);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("Invalid result");
        return nullptr;
    }
    return value;
}

bool I18nSystemAddon::ParseStringParam(napi_env env, napi_value argv, bool throwError, std::string &strParam)
{
    if (argv == nullptr) {
        HILOG_ERROR_I18N("ParseStringParam: Missing parameter");
        ErrorUtil::NapiNotFoundError(env, I18N_NOT_FOUND, "language", throwError);
        return false;
    }
    napi_valuetype valueType = napi_valuetype::napi_undefined;
    napi_typeof(env, argv, &valueType);
    if (valueType != napi_valuetype::napi_string) {
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, "language", "string", throwError);
        return false;
    }
    size_t len = 0;
    napi_status status = napi_get_value_string_utf8(env, argv, nullptr, 0, &len);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("get string parameter length failed");
        return false;
    }
    std::vector<char> res(len + 1);
    status = napi_get_value_string_utf8(env, argv, res.data(), len + 1, &len);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("get string parameter failed");
        return false;
    }
    strParam = res.data();
    return true;
}

napi_value I18nSystemAddon::SetTemperatureType(napi_env env, napi_callback_info info)
{
    TemperatureType type = TemperatureType::CELSIUS;
    if (!ParseTemperatureType(env, info, type)) {
        HILOG_ERROR_I18N("I18nSystemAddon::SetTemperatureType: parse temperature type failed.");
        return nullptr;
    }
    I18nErrorCode err = I18nServiceAbilityClient::SetTemperatureType(type);
    if (err == I18nErrorCode::SUCCESS) {
        return nullptr;
    }
    switch (err) {
        case I18nErrorCode::NOT_SYSTEM_APP:
            ErrorUtil::NapiThrow(env, I18N_NOT_SYSTEM_APP, "", "", true);
            break;
        case I18nErrorCode::NO_PERMISSION:
            ErrorUtil::NapiThrow(env, I18N_NO_PERMISSION, "", "", true);
            break;
        case I18nErrorCode::INVALID_TEMPERATURE_TYPE:
            ErrorUtil::NapiThrow(env, I18N_NOT_VALID, NAME_OF_TEMPERATURE, TYPE_OF_TEMPERATURE, true);
            break;
        default:
            HILOG_ERROR_I18N("I18nSystemAddon::SetTemperatureType: set temperature type failed.");
    }
    return nullptr;
}

napi_value I18nSystemAddon::GetTemperatureType(napi_env env, napi_callback_info info)
{
    TemperatureType type = LocaleConfig::GetTemperatureType();
    int32_t value = static_cast<int32_t>(type);
    return VariableConvertor::CreateNumber(env, value);
}

napi_value I18nSystemAddon::GetTemperatureName(napi_env env, napi_callback_info info)
{
    TemperatureType type = TemperatureType::CELSIUS;
    if (!ParseTemperatureType(env, info, type)) {
        HILOG_ERROR_I18N("I18nSystemAddon::GetTemperatureName: parse temperature type failed.");
        return VariableConvertor::CreateString(env, "");
    }
    std::string temperatureName = LocaleConfig::GetTemperatureName(type);
    if (temperatureName.empty()) {
        ErrorUtil::NapiThrow(env, I18N_NOT_VALID, NAME_OF_TEMPERATURE, TYPE_OF_TEMPERATURE, true);
        return VariableConvertor::CreateString(env, "");
    }
    return VariableConvertor::CreateString(env, temperatureName);
}

bool I18nSystemAddon::ParseTemperatureType(napi_env env, napi_callback_info info, TemperatureType& type)
{
    size_t argc = 1;
    napi_value argv[1] = { nullptr };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("I18nSystemAddon::ParseTemperatureType: get cb info failed.");
        return false;
    }
    if (argc < 1) {
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, NAME_OF_TEMPERATURE, "", true);
        return false;
    }
    napi_valuetype valueType = napi_valuetype::napi_undefined;
    status = napi_typeof(env, argv[0], &valueType);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("I18nSystemAddon::ParseTemperatureType: get value type failed.");
        return false;
    }
    if (valueType != napi_valuetype::napi_number) {
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, NAME_OF_TEMPERATURE, TYPE_OF_TEMPERATURE, true);
        return false;
    }
    int32_t temperatureType = 0;
    status = napi_get_value_int32(env, argv[0], &temperatureType);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("I18nSystemAddon::ParseTemperatureType: get temperature type failed.");
        return false;
    }
    type = static_cast<TemperatureType>(temperatureType);
    return true;
}

napi_value I18nSystemAddon::SetFirstDayOfWeek(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = { nullptr };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("I18nSystemAddon::SetFirstDayOfWeek: get cb info failed.");
        return nullptr;
    }
    if (argc < 1) {
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, NAME_OF_WEEKDAY, "", true);
        return nullptr;
    }
    napi_valuetype valueType = napi_valuetype::napi_undefined;
    status = napi_typeof(env, argv[0], &valueType);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("I18nSystemAddon::SetFirstDayOfWeek: get value type failed.");
        return nullptr;
    }
    if (valueType != napi_valuetype::napi_number) {
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, NAME_OF_WEEKDAY, TYPE_OF_WEEKDAY, true);
        return nullptr;
    }
    int32_t weekDay = 0;
    status = napi_get_value_int32(env, argv[0], &weekDay);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("I18nSystemAddon::SetFirstDayOfWeek: get week day failed.");
        return nullptr;
    }
    WeekDay type = static_cast<WeekDay>(weekDay);
    I18nErrorCode err = I18nServiceAbilityClient::SetFirstDayOfWeek(type);
    if (err == I18nErrorCode::SUCCESS) {
        return nullptr;
    }
    switch (err) {
        case I18nErrorCode::NOT_SYSTEM_APP:
            ErrorUtil::NapiThrow(env, I18N_NOT_SYSTEM_APP, "", "", true);
            break;
        case I18nErrorCode::NO_PERMISSION:
            ErrorUtil::NapiThrow(env, I18N_NO_PERMISSION, "", "", true);
            break;
        case I18nErrorCode::INVALID_WEEK_DAY:
            ErrorUtil::NapiThrow(env, I18N_NOT_VALID, NAME_OF_WEEKDAY, TYPE_OF_WEEKDAY, true);
            break;
        default:
            HILOG_ERROR_I18N("I18nSystemAddon::SetFirstDayOfWeek: set first day of week failed.");
    }
    return nullptr;
}

napi_value I18nSystemAddon::GetFirstDayOfWeek(napi_env env, napi_callback_info info)
{
    WeekDay type = LocaleConfig::GetFirstDayOfWeek();
    int32_t value = static_cast<int32_t>(type);
    return VariableConvertor::CreateNumber(env, value);
}

napi_value I18nSystemAddon::GetSimplifiedLanguage(napi_env env, napi_callback_info info)
{
    // Need to get one parameter
    size_t argc = 1;
    napi_value argv[1] = { nullptr };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("GetSimplifiedLanguage get parameter fail");
        return nullptr;
    }

    std::string simplifiedLanguage = "";
    if (VariableConvertor::CheckNapiIsNull(env, argv[0])) {
        napi_valuetype valueType = napi_valuetype::napi_undefined;
        napi_typeof(env, argv[0], &valueType);
        if (valueType != napi_valuetype::napi_string) {
            HILOG_ERROR_I18N("GetSimplifiedLanguage Parameter type is not string");
            ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, "language", "string", true);
            return nullptr;
        }
        int code = 0;
        std::string language = VariableConvertor::GetString(env, argv[0], code);
        if (code == 1) {
            HILOG_ERROR_I18N("GetSimplifiedLanguage can't get string from js param");
            return nullptr;
        }
        code = 0;
        simplifiedLanguage = LocaleConfig::GetSimplifiedLanguage(language, code);
        if (code != 0) {
            HILOG_ERROR_I18N("GetSimplifiedLanguage: language is not valid");
            ErrorUtil::NapiThrow(env, I18N_NOT_VALID, "language", "a valid language", true);
            return nullptr;
        }
    } else {
        simplifiedLanguage = LocaleConfig::GetSimplifiedSystemLanguage();
    }

    napi_value result = VariableConvertor::CreateString(env, simplifiedLanguage);
    if (!result) {
        HILOG_ERROR_I18N("GetSimplifiedLanguage: Failed to create string item");
    }
    return result;
}
} // namespace I18n
} // namespace Global
} // namespace OHOS