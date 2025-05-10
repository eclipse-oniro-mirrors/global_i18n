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
#include "i18n_hilog.h"
#include "locale_config.h"
#include "utils.h"
#include "variable_convertor.h"
#include "system_locale_manager_addon.h"

namespace OHOS {
namespace Global {
namespace I18n {
SystemLocaleManagerAddon::SystemLocaleManagerAddon() : env_(nullptr) {}

SystemLocaleManagerAddon::~SystemLocaleManagerAddon()
{
}

void SystemLocaleManagerAddon::Destructor(napi_env env, void *nativeObject, void *hint)
{
    if (!nativeObject) {
        return;
    }
    delete reinterpret_cast<SystemLocaleManagerAddon *>(nativeObject);
    nativeObject = nullptr;
}

napi_value SystemLocaleManagerAddon::InitSystemLocaleManager(napi_env env, napi_value exports)
{
    napi_status status = napi_ok;
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("getLanguageInfoArray", GetLanguageInfoArray),
        DECLARE_NAPI_FUNCTION("getRegionInfoArray", GetCountryInfoArray),
        DECLARE_NAPI_STATIC_FUNCTION("getTimeZoneCityItemArray", GetTimeZoneCityInfoArray)
    };

    napi_value constructor = nullptr;
    status = napi_define_class(env, "SystemLocaleManager", NAPI_AUTO_LENGTH, SystemLocaleManagerConstructor, nullptr,
        sizeof(properties) / sizeof(napi_property_descriptor), properties, &constructor);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("Define class failed when InitSystemLocaleManager");
        return nullptr;
    }

    status = napi_set_named_property(env, exports, "SystemLocaleManager", constructor);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("Set property failed when InitSystemLocaleManager");
        return nullptr;
    }
    return exports;
}

napi_value SystemLocaleManagerAddon::SystemLocaleManagerConstructor(napi_env env, napi_callback_info info)
{
    if (!CheckSystemPermission()) {
        HILOG_ERROR_I18N("Permission verification failed. A non-system application calls a system API.");
        return nullptr;
    }
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, &data);
    if (status != napi_ok) {
        return nullptr;
    }
    std::unique_ptr<SystemLocaleManagerAddon> obj = std::make_unique<SystemLocaleManagerAddon>();
    status = napi_wrap(env, thisVar, reinterpret_cast<void *>(obj.get()), SystemLocaleManagerAddon::Destructor,
        nullptr, nullptr);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("Wrap SystemLocaleManagerAddon failed");
        return nullptr;
    }
    if (!obj->InitSystemLocaleManagerContext(env, info)) {
        HILOG_ERROR_I18N("Init SystemLocaleManager failed");
        return nullptr;
    }
    obj.release();
    return thisVar;
}

bool SystemLocaleManagerAddon::InitSystemLocaleManagerContext(napi_env env, napi_callback_info info)
{
    napi_value global = nullptr;
    napi_status status = napi_get_global(env, &global);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("Get global failed");
        return false;
    }
    env_ = env;
    systemLocaleManager_ = std::make_unique<SystemLocaleManager>();

    return systemLocaleManager_ != nullptr;
}

napi_value SystemLocaleManagerAddon::GetLanguageInfoArray(napi_env env, napi_callback_info info)
{
    size_t argc = 2;
    napi_value argv[2] = { 0 };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("can not obtain getLanguageInfoArray function param.");
        return nullptr;
    }
    if (argc < 1) {
        ErrorUtil::NapiNotFoundError(env, I18N_NOT_FOUND, "languages", true);
        return nullptr;
    }
    std::vector<std::string> languageList;
    bool isSuccess = VariableConvertor::GetStringArrayFromJsParam(env, argv[0], "languages", languageList);
    if (!isSuccess) {
        return nullptr;
    }
    SortOptions options;
    GetSortOptionsFromJsParam(env, argv[1], options, false);
    SystemLocaleManagerAddon *obj = nullptr;
    status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->systemLocaleManager_) {
        HILOG_ERROR_I18N("GetLanguageInfoArray: Get SystemLocaleManager object failed");
        return nullptr;
    }
    I18nErrorCode err;
    std::vector<LocaleItem> localeItemList =
        obj->systemLocaleManager_->GetLanguageInfoArray(languageList, options, err);
    if (err == I18nErrorCode::NOT_SYSTEM_APP) {
        ErrorUtil::NapiThrow(env, I18N_NOT_SYSTEM_APP, "", "", true);
        return nullptr;
    }
    napi_value result = CreateLocaleItemArray(env, localeItemList);
    return result;
}

napi_value SystemLocaleManagerAddon::GetCountryInfoArray(napi_env env, napi_callback_info info)
{
    size_t argc = 2;
    napi_value argv[2] = { 0 };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("can not obtain getCountryInfoArray function param.");
        return nullptr;
    }
    if (argc < 1) {
        ErrorUtil::NapiNotFoundError(env, I18N_NOT_FOUND, "regions", true);
        return nullptr;
    }
    std::vector<std::string> countryList;
    bool isSuccess = VariableConvertor::GetStringArrayFromJsParam(env, argv[0], "regions", countryList);
    if (!isSuccess) {
        return nullptr;
    }
    SortOptions options;
    GetSortOptionsFromJsParam(env, argv[1], options, true);
    SystemLocaleManagerAddon *obj = nullptr;
    status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->systemLocaleManager_) {
        HILOG_ERROR_I18N("GetCountryInfoArray: Get SystemLocaleManager object failed");
        return nullptr;
    }
    I18nErrorCode err;
    std::vector<LocaleItem> localeItemList = obj->systemLocaleManager_->GetCountryInfoArray(countryList, options, err);
    if (err == I18nErrorCode::NOT_SYSTEM_APP) {
        ErrorUtil::NapiThrow(env, I18N_NOT_SYSTEM_APP, "", "", true);
        return nullptr;
    }
    napi_value result = CreateLocaleItemArray(env, localeItemList);
    return result;
}

napi_value SystemLocaleManagerAddon::GetTimeZoneCityInfoArray(napi_env env, napi_callback_info info)
{
    I18nErrorCode err;
    std::vector<TimeZoneCityItem> timezoneCityItemList = SystemLocaleManager::GetTimezoneCityInfoArray(err);
    if (err == I18nErrorCode::NOT_SYSTEM_APP) {
        ErrorUtil::NapiThrow(env, I18N_NOT_SYSTEM_APP, "", "", true);
        return nullptr;
    }
    napi_value result = nullptr;
    napi_status status = napi_create_array_with_length(env, timezoneCityItemList.size(), &result);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("create TimeZoneCityItem array failed.");
        return nullptr;
    }
    for (size_t i = 0; i < timezoneCityItemList.size(); ++i) {
        napi_value item = CreateTimeZoneCityItem(env, timezoneCityItemList[i]);
        status = napi_set_element(env, result, i, item);
        if (status != napi_ok) {
            HILOG_ERROR_I18N("Failed to set TimeZoneCityItem element.");
            return nullptr;
        }
    }
    return result;
}

napi_value SystemLocaleManagerAddon::CreateTimeZoneCityItem(napi_env env, const TimeZoneCityItem &timezoneCityItem)
{
    napi_value result;
    napi_status status = napi_create_object(env, &result);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("CreateTimeZoneCityItem: Create Locale Item object failed.");
        return nullptr;
    }
    status = napi_set_named_property(env, result, "zoneId",
        VariableConvertor::CreateString(env, timezoneCityItem.zoneId));
    if (status != napi_ok) {
        HILOG_ERROR_I18N("Failed to set element zoneId.");
        return nullptr;
    }
    status = napi_set_named_property(env, result, "cityId",
        VariableConvertor::CreateString(env, timezoneCityItem.cityId));
    if (status != napi_ok) {
        HILOG_ERROR_I18N("Failed to set element cityId.");
        return nullptr;
    }
    status = napi_set_named_property(env, result, "cityDisplayName",
        VariableConvertor::CreateString(env, timezoneCityItem.cityDisplayName));
    if (status != napi_ok) {
        HILOG_ERROR_I18N("Failed to set element cityDisplayName.");
        return nullptr;
    }
    status = napi_set_named_property(env, result, "offset",
        VariableConvertor::CreateNumber(env, timezoneCityItem.offset));
    if (status != napi_ok) {
        HILOG_ERROR_I18N("Failed to set element offset.");
        return nullptr;
    }
    status = napi_set_named_property(env, result, "zoneDisplayName",
        VariableConvertor::CreateString(env, timezoneCityItem.zoneDisplayName));
    if (status != napi_ok) {
        HILOG_ERROR_I18N("Failed to set element zoneDisplayName.");
        return nullptr;
    }
    if (timezoneCityItem.rawOffset != 0) {
        status = napi_set_named_property(env, result, "rawOffset",
            VariableConvertor::CreateNumber(env, timezoneCityItem.rawOffset));
        if (status != napi_ok) {
            HILOG_ERROR_I18N("Failed to set element rawOffset.");
            return nullptr;
        }
    }
    return result;
}

void SystemLocaleManagerAddon::GetSortOptionsFromJsParam(napi_env env, napi_value &jsOptions, SortOptions &options,
    bool isRegion)
{
    std::string localeTag;
    bool isUseLocalName = true;
    bool isSuggestedFirst = true;
    if (jsOptions == nullptr) {
        localeTag = LocaleConfig::GetSystemLocale();
        if (isRegion) {
            isUseLocalName = false;
        }
    } else {
        VariableConvertor::GetOptionValue(env, jsOptions, "locale", localeTag);
        if (localeTag.length() == 0) {
            localeTag = LocaleConfig::GetSystemLocale();
        }
        bool isSuccess = VariableConvertor::GetBoolOptionValue(env, jsOptions, "isUseLocalName", isUseLocalName);
        if (!isSuccess && isRegion) {
            isUseLocalName = false;
        }
        VariableConvertor::GetBoolOptionValue(env, jsOptions, "isSuggestedFirst", isSuggestedFirst);
    }
    options.localeTag = localeTag;
    options.isUseLocalName = isUseLocalName;
    options.isSuggestedFirst = isSuggestedFirst;
}

napi_value SystemLocaleManagerAddon::CreateLocaleItemArray(napi_env env, const std::vector<LocaleItem> &localeItemList)
{
    napi_value result = nullptr;
    napi_status status = napi_create_array_with_length(env, localeItemList.size(), &result);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("create LocaleItem array failed.");
        return nullptr;
    }
    for (size_t i = 0; i < localeItemList.size(); ++i) {
        napi_value item = CreateLocaleItem(env, localeItemList[i]);
        status = napi_set_element(env, result, i, item);
        if (status != napi_ok) {
            HILOG_ERROR_I18N("Failed to set LocaleItem element.");
            return nullptr;
        }
    }
    return result;
}

napi_value SystemLocaleManagerAddon::CreateLocaleItem(napi_env env, const LocaleItem &localeItem)
{
    napi_value result;
    napi_status status = napi_create_object(env, &result);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("CreateLocaleItem: Create Locale Item object failed.");
        return nullptr;
    }
    status = napi_set_named_property(env, result, "id", VariableConvertor::CreateString(env, localeItem.id));
    if (status != napi_ok) {
        HILOG_ERROR_I18N("Failed to set element id.");
        return nullptr;
    }
    status = napi_set_named_property(env, result, "displayName",
        VariableConvertor::CreateString(env, localeItem.displayName));
    if (status != napi_ok) {
        HILOG_ERROR_I18N("Failed to set element displayName.");
        return nullptr;
    }
    if (localeItem.localName.length() != 0) {
        status = napi_set_named_property(env, result, "localName",
            VariableConvertor::CreateString(env, localeItem.localName));
        if (status != napi_ok) {
            HILOG_ERROR_I18N("Failed to set element localName.");
            return nullptr;
        }
    }
    status = napi_set_named_property(env, result, "suggestionType", CreateSuggestionType(env,
        localeItem.suggestionType));
    if (status != napi_ok) {
        HILOG_ERROR_I18N("Failed to set element suggestionType.");
        return nullptr;
    }
    return result;
}

napi_value SystemLocaleManagerAddon::CreateSuggestionType(napi_env env, SuggestionType suggestionType)
{
    napi_value result;
    napi_status status = napi_create_int32(env, static_cast<int32_t>(suggestionType), &result);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("create SuggestionType failed.");
        return nullptr;
    }
    return result;
}
} // namespace I18n
} // namespace Global
} // namespace OHOS