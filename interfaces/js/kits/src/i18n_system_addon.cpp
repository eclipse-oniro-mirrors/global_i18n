/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "error_util.h"
#include "hilog/log.h"
#include "i18n_service_ability_client.h"
#include "i18n_service_ability_load_manager.h"
#include "i18n_system_addon.h"
#include "js_utils.h"
#include "locale_config.h"
#include "preferred_language.h"
#include "utils.h"
#include "variable_convertor.h"

namespace OHOS {
namespace Global {
namespace I18n {
static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, 0xD001E00, "I18nSystemAddonJS" };
using namespace OHOS::HiviewDFX;

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
        DECLARE_NAPI_STATIC_FUNCTION("getUsingLocalDigit", GetUsingLocalDigitAddon)
    };
    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, "System", NAPI_AUTO_LENGTH, JSUtils::DefaultConstructor, nullptr,
        sizeof(properties) / sizeof(napi_property_descriptor), properties, &constructor);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Define class failed when InitSystem.");
        return nullptr;
    }

    status = napi_set_named_property(env, exports, "System", constructor);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Set property failed when InitSystem.");
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
    std::vector<std::string> systemLanguages;
    LocaleConfig::GetSystemLanguages(systemLanguages);
    napi_value result = nullptr;
    napi_status status = napi_create_array_with_length(env, systemLanguages.size(), &result);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Failed to create array");
        return nullptr;
    }
    for (size_t i = 0; i < systemLanguages.size(); i++) {
        napi_value value = nullptr;
        status = napi_create_string_utf8(env, systemLanguages[i].c_str(), NAPI_AUTO_LENGTH, &value);
        if (status != napi_ok) {
            HiLog::Error(LABEL, "GetSystemLanguages: Failed to create string item");
            return nullptr;
        }
        status = napi_set_element(env, result, i, value);
        if (status != napi_ok) {
            HiLog::Error(LABEL, "GetSystemLanguages: Failed to set array item");
            return nullptr;
        }
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
        HiLog::Error(LABEL, "Failed to create string item");
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
        HiLog::Error(LABEL, "Failed to create string item");
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
        HiLog::Error(LABEL, "Failed to create string item");
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
        HiLog::Error(LABEL, "Failed to create boolean item");
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
        HiLog::Error(LABEL, "getPreferrdLanguageList: create array failed");
        return nullptr;
    }
    for (size_t i = 0; i < languageList.size(); i++) {
        napi_value value = nullptr;
        status = napi_create_string_utf8(env, languageList[i].c_str(), NAPI_AUTO_LENGTH, &value);
        if (status != napi_ok) {
            HiLog::Error(LABEL, "getPreferrdLanguageList: create string failed");
            return nullptr;
        }
        status = napi_set_element(env, result, i, value);
        if (status != napi_ok) {
            HiLog::Error(LABEL, "GetPreferredLanguageList: set array item failed");
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
        HiLog::Error(LABEL, "getFirstPreferrdLanguage: create string result failed");
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
        HiLog::Error(LABEL, "SetAppPreferredLanguage Parameter type is not string");
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, true);
        return nullptr;
    }
    int code = 0;
    std::string localeTag = VariableConvertor::GetString(env, argv[0], code);
    if (code) {
        HiLog::Error(LABEL, "SetAppPreferredLanguage can't get string from js param");
        return nullptr;
    }
    UErrorCode icuStatus = U_ZERO_ERROR;
    icu::Locale locale = icu::Locale::forLanguageTag(localeTag.data(), icuStatus);
    if (U_FAILURE(icuStatus) || !IsValidLocaleTag(locale)) {
        HiLog::Error(LABEL, "GetTimePeriodName does not support this locale");
        ErrorUtil::NapiThrow(env, I18N_NOT_VALID, true);
        return nullptr;
    }
#ifdef SUPPORT_APP_PREFERRED_LANGUAGE
    I18nErrorCode errCode = I18nErrorCode::SUCCESS;
    PreferredLanguage::SetAppPreferredLanguage(localeTag, errCode);
    if (errCode != I18nErrorCode::SUCCESS) {
        HiLog::Error(LABEL, "SetAppPreferredLanguage: set app language to i18n app preferences failed.");
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
        HiLog::Error(LABEL, "getAppPreferrdLanguage: create string result failed");
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
        return nullptr;
    }
    if (argc < FUNC_ARGS_COUNT) {
        HiLog::Error(LABEL, "Missing parameter");
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, throwError);
        return nullptr;
    }
    size_t len = 0;
    napi_get_value_string_utf8(env, argv[0], nullptr, 0, &len);
    std::vector<char> localeBuf(len + 1);
    status = napi_get_value_string_utf8(env, argv[0], localeBuf.data(), len + 1, &len);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Failed to get string item");
        ErrorUtil::NapiThrow(env, I18N_NOT_VALID, throwError);
        return nullptr;
    }
    napi_get_value_string_utf8(env, argv[1], nullptr, 0, &len);
    std::vector<char> displayLocaleBuf(len + 1);
    status = napi_get_value_string_utf8(env, argv[1], displayLocaleBuf.data(), len + 1, &len);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Failed to get string item");
        ErrorUtil::NapiThrow(env, I18N_NOT_VALID, throwError);
        return nullptr;
    }
    bool sentenceCase = true;
    size_t sentenceCaseIndex = 2;
    if (argc > sentenceCaseIndex) {
        napi_get_value_bool(env, argv[sentenceCaseIndex], &sentenceCase);
    }
    std::string value = LocaleConfig::GetDisplayRegion(localeBuf.data(), displayLocaleBuf.data(), sentenceCase);
    if (value.length() == 0) {
        ErrorUtil::NapiThrow(env, I18N_NOT_VALID, throwError);
    }
    napi_value result = nullptr;
    status = napi_create_string_utf8(env, value.c_str(), NAPI_AUTO_LENGTH, &result);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Failed to create string item");
        return nullptr;
    }
    return result;
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
    }
    if (argc < FUNC_ARGS_COUNT) {
        HiLog::Error(LABEL, "Missing parameter");
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, throwError);
        return nullptr;
    }
    size_t len = 0;
    napi_get_value_string_utf8(env, argv[0], nullptr, 0, &len);
    std::vector<char> localeBuf(len + 1);
    status = napi_get_value_string_utf8(env, argv[0], localeBuf.data(), len + 1, &len);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Failed to get string item");
        ErrorUtil::NapiThrow(env, I18N_NOT_VALID, throwError);
        return nullptr;
    }
    napi_get_value_string_utf8(env, argv[1], nullptr, 0, &len);
    std::vector<char> displayLocaleBuf(len + 1);
    status = napi_get_value_string_utf8(env, argv[1], displayLocaleBuf.data(), len + 1, &len);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Failed to get string item");
        ErrorUtil::NapiThrow(env, I18N_NOT_VALID, throwError);
        return nullptr;
    }
    bool sentenceCase = true;
    size_t sentenceCaseIndex = 2;
    if (argc > sentenceCaseIndex) {
        napi_get_value_bool(env, argv[sentenceCaseIndex], &sentenceCase);
    }

    std::string value = LocaleConfig::GetDisplayLanguage(localeBuf.data(), displayLocaleBuf.data(), sentenceCase);
    if (value.length() == 0) {
        ErrorUtil::NapiThrow(env, I18N_NOT_VALID, throwError);
        return nullptr;
    }
    napi_value result = nullptr;
    status = napi_create_string_utf8(env, value.c_str(), NAPI_AUTO_LENGTH, &result);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Failed to create string item");
        return nullptr;
    }
    return result;
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
    }
    if (argc < 1) {
        HiLog::Error(LABEL, "Missing parameter");
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, throwError);
        return nullptr;
    }
    size_t len = 0;
    napi_get_value_string_utf8(env, argv[0], nullptr, 0, &len);
    std::vector<char> localeBuf(len + 1);
    status = napi_get_value_string_utf8(env, argv[0], localeBuf.data(), len + 1, &len);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Failed to get string item");
        ErrorUtil::NapiThrow(env, I18N_NOT_VALID, throwError);
        return nullptr;
    }
    std::vector<std::string> systemCountries;
    LocaleConfig::GetSystemCountries(systemCountries);
    napi_value result = nullptr;
    status = napi_create_array_with_length(env, systemCountries.size(), &result);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Failed to create array");
        return nullptr;
    }
    for (size_t i = 0; i < systemCountries.size(); i++) {
        napi_value value = nullptr;
        status = napi_create_string_utf8(env, systemCountries[i].c_str(), NAPI_AUTO_LENGTH, &value);
        if (status != napi_ok) {
            HiLog::Error(LABEL, "GetSystemCountries: Failed to create string item");
            return nullptr;
        }
        status = napi_set_element(env, result, i, value);
        if (status != napi_ok) {
            HiLog::Error(LABEL, "GetSystemCountries: Failed to set array item");
            return nullptr;
        }
    }
    return result;
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
    }
    if (argc < 1) {
        HiLog::Error(LABEL, "Missing parameter");
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, throwError);
        return nullptr;
    }
    size_t len = 0;
    napi_get_value_string_utf8(env, argv[0], nullptr, 0, &len);
    std::vector<char> languageBuf(len + 1);
    status = napi_get_value_string_utf8(env, argv[0], languageBuf.data(), len + 1, &len);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Failed to get string item");
        ErrorUtil::NapiThrow(env, I18N_NOT_VALID, throwError);
        return nullptr;
    }
    bool isSuggested = false;
    if (VariableConvertor::CheckNapiValueType(env, argv[1])) {
        napi_get_value_string_utf8(env, argv[1], nullptr, 0, &len);
        std::vector<char> regionBuf(len + 1);
        status = napi_get_value_string_utf8(env, argv[1], regionBuf.data(), len + 1, &len);
        if (status != napi_ok) {
            HiLog::Error(LABEL, "Failed to get string item");
            ErrorUtil::NapiThrow(env, I18N_NOT_VALID, throwError);
            return nullptr;
        }
        isSuggested = LocaleConfig::IsSuggested(languageBuf.data(), regionBuf.data());
    } else {
        isSuggested = LocaleConfig::IsSuggested(languageBuf.data());
    }
    napi_value result = nullptr;
    status = napi_get_boolean(env, isSuggested, &result);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Create case first boolean value failed");
        return nullptr;
    }
    return result;
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
        HiLog::Error(LABEL, "Missing parameter");
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, throwError);
        return nullptr;
    }
    size_t len = 0;
    napi_get_value_string_utf8(env, argv[0], nullptr, 0, &len);
    std::vector<char> languageBuf(len + 1);
    status = napi_get_value_string_utf8(env, argv[0], languageBuf.data(), len + 1, &len);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Failed to get string item");
        ErrorUtil::NapiThrow(env, I18N_NOT_VALID, throwError);
        return nullptr;
    }
    I18nErrorCode err = I18nServiceAbilityClient::SetSystemLanguage(languageBuf.data());
    HiLog::Info(LABEL, "I18nSystemAddon::SetSystemLanguageImpl with code %{public}d", static_cast<int32_t>(err));
    bool success = err == I18nErrorCode::SUCCESS;
    if (throwError) {
        if (!success) {
            ErrorUtil::NapiThrow(env, I18N_NO_PERMISSION, throwError);
        }
        return nullptr;
    }
    napi_value result = nullptr;
    status = napi_get_boolean(env, success, &result);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Create set system language boolean value failed");
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
        HiLog::Error(LABEL, "Missing parameter");
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, throwError);
        return nullptr;
    }
    size_t len = 0;
    napi_get_value_string_utf8(env, argv[0], nullptr, 0, &len);
    std::vector<char> regionBuf(len + 1);
    status = napi_get_value_string_utf8(env, argv[0], regionBuf.data(), len + 1, &len);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Failed to get string item");
        ErrorUtil::NapiThrow(env, I18N_NOT_VALID, throwError);
        return nullptr;
    }
    I18nErrorCode err = I18nServiceAbilityClient::SetSystemRegion(regionBuf.data());
    HiLog::Info(LABEL, "I18nSystemAddon::SetSystemRegionImpl with code %{public}d", static_cast<int32_t>(err));
    bool success = err == I18nErrorCode::SUCCESS;
    if (throwError) {
        if (!success) {
            ErrorUtil::NapiThrow(env, I18N_NO_PERMISSION, throwError);
        }
        return nullptr;
    }
    napi_value result = nullptr;
    status = napi_get_boolean(env, success, &result);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Create set system language boolean value failed");
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
        HiLog::Error(LABEL, "Missing parameter");
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, throwError);
        return nullptr;
    }
    size_t len = 0;
    napi_get_value_string_utf8(env, argv[0], nullptr, 0, &len);
    std::vector<char> localeBuf(len + 1);
    status = napi_get_value_string_utf8(env, argv[0], localeBuf.data(), len + 1, &len);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Failed to get string item");
        ErrorUtil::NapiThrow(env, I18N_NOT_VALID, throwError);
        return nullptr;
    }
    I18nErrorCode err = I18nServiceAbilityClient::SetSystemLocale(localeBuf.data());
    HiLog::Info(LABEL, "I18nSystemAddon::SetSystemLocaleImpl with code %{public}d", static_cast<int32_t>(err));
    bool success = err == I18nErrorCode::SUCCESS;
    if (throwError) {
        if (!success) {
            ErrorUtil::NapiThrow(env, I18N_NO_PERMISSION, throwError);
        }
        return nullptr;
    }
    napi_value result = nullptr;
    status = napi_get_boolean(env, success, &result);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Create set system language boolean value failed");
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
        HiLog::Error(LABEL, "Missing parameter");
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, throwError);
        return nullptr;
    }

    bool option = false;
    status = napi_get_value_bool(env, argv[0], &option);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Failed to get boolean item");
        ErrorUtil::NapiThrow(env, I18N_NOT_VALID, throwError);
        return nullptr;
    }
    std::string optionStr = option ? "true" : "false";
    I18nErrorCode err = I18nServiceAbilityClient::Set24HourClock(optionStr);
    HiLog::Info(LABEL, "I18nSystemAddon::Set24HourClock with code %{public}d", static_cast<int32_t>(err));
    bool success = err == I18nErrorCode::SUCCESS;
    if (throwError) {
        if (!success) {
            ErrorUtil::NapiThrow(env, I18N_NO_PERMISSION, throwError);
        }
        return nullptr;
    }
    napi_value result = nullptr;
    status = napi_get_boolean(env, success, &result);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Create set 24HourClock boolean value failed");
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
    if (VariableConvertor::CheckNapiValueType(env, argc > 1 ? argv[1] : nullptr)) {
        status = napi_get_value_int32(env, argv[1], &index);
    }
    if (status != napi_ok) {
        HiLog::Error(LABEL, "addPreferrdLanguage: get index failed");
        ErrorUtil::NapiThrow(env, I18N_NOT_VALID, throwError);
        return nullptr;
    }
    I18nErrorCode err = I18nServiceAbilityClient::AddPreferredLanguage(language.data(), index);
    HiLog::Info(LABEL, "I18nSystemAddon::AddPreferredLanguageImpl with code %{public}d", static_cast<int32_t>(err));
    if (throwError) {
        if (err == I18nErrorCode::NO_PERMISSION) {
            ErrorUtil::NapiThrow(env, I18N_NO_PERMISSION, throwError);
        }
        if (err != I18nErrorCode::SUCCESS) {
            ErrorUtil::NapiThrow(env, I18N_NOT_VALID, throwError);
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
        HiLog::Error(LABEL, "addPreferrdLanguage: create boolean result failed");
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
        HiLog::Error(LABEL, "Missing parameter");
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, throwError);
        return nullptr;
    }

    napi_valuetype valueType = napi_valuetype::napi_undefined;
    napi_typeof(env, argv[0], &valueType);
    if (valueType != napi_valuetype::napi_number) {
        ErrorUtil::NapiThrow(env, I18N_NOT_VALID, throwError);
        return nullptr;
    }
    int index = 1000000;
    status = napi_get_value_int32(env, argv[0], &index);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "removePreferrdLanguage: get index failed");
        ErrorUtil::NapiThrow(env, I18N_NOT_VALID, throwError);
        return nullptr;
    }
    len = static_cast<int>(PreferredLanguage::GetPreferredLanguageList().size());
    if ((index < 0 || index > len - 1) && throwError) {
        ErrorUtil::NapiThrow(env, I18N_NOT_VALID, throwError);
        return nullptr;
    }
    I18nErrorCode err = I18nServiceAbilityClient::RemovePreferredLanguage(index);
    bool success = err == I18nErrorCode::SUCCESS;

    if (throwError) {
        if (!success) {
            ErrorUtil::NapiThrow(env, I18N_NO_PERMISSION, throwError);
        }
        return nullptr;
    }
    napi_value result = nullptr;
    status = napi_get_boolean(env, success, &result);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "removePreferrdLanguage: create boolean result failed");
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
        HiLog::Error(LABEL, "Invalid parameter nullptr");
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, throwError);
        return nullptr;
    }
    napi_valuetype valueType = napi_valuetype::napi_undefined;
    napi_typeof(env, argv[0], &valueType);
    if (valueType != napi_valuetype::napi_boolean) {
        HiLog::Error(LABEL, "Invalid parameter type");
        ErrorUtil::NapiThrow(env, I18N_NOT_VALID, throwError);
        return nullptr;
    }
    bool flag = false;
    napi_status status = napi_get_value_bool(env, argv[0], &flag);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Get parameter flag failed");
        return nullptr;
    }
    I18nErrorCode err = I18nServiceAbilityClient::SetUsingLocalDigit(flag);
    HiLog::Info(LABEL, "I18nSystemAddon::SetUsingLocalDigitAddonImpl with code %{public}d",
        static_cast<int32_t>(err));
    bool res = err == I18nErrorCode::SUCCESS;
    if (throwError) {
        if (!res) {
            ErrorUtil::NapiThrow(env, I18N_NO_PERMISSION, throwError);
        }
        return nullptr;
    }
    napi_value value = nullptr;
    status = napi_get_boolean(env, res, &value);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Invalid result");
        return nullptr;
    }
    return value;
}

bool I18nSystemAddon::ParseStringParam(napi_env env, napi_value argv, bool throwError, std::string &strParam)
{
    if (argv == nullptr) {
        HiLog::Error(LABEL, "Missing parameter");
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, throwError);
        return false;
    }
    napi_valuetype valueType = napi_valuetype::napi_undefined;
    napi_typeof(env, argv, &valueType);
    if (valueType != napi_valuetype::napi_string) {
        ErrorUtil::NapiThrow(env, I18N_NOT_VALID, throwError);
        return false;
    }
    size_t len = 0;
    napi_status status = napi_get_value_string_utf8(env, argv, nullptr, 0, &len);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "get string parameter length failed");
        ErrorUtil::NapiThrow(env, I18N_NOT_VALID, throwError);
        return false;
    }
    std::vector<char> res(len + 1);
    status = napi_get_value_string_utf8(env, argv, res.data(), len + 1, &len);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "get string parameter failed");
        return false;
    }
    strParam = res.data();
    return true;
}
} // namespace I18n
} // namespace Global
} // namespace OHOS