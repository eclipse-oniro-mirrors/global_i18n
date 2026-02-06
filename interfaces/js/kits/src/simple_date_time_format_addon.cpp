/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#include "js_lifecycle_managers.h"
#include "js_utils.h"
#include "locale_info_addon.h"
#include "variable_convertor.h"
#include "simple_date_time_format_addon.h"

namespace OHOS {
namespace Global {
namespace I18n {
static thread_local ThreadReference* g_SimpleDateTimeFormatConstructor = nullptr;
static const size_t SIZE_WITHOUR_LOCALE = 2;
static const size_t SIZE_WITH_LOCALE = 3;
static const std::string GET_TIME_NAME = "getTime";

SimpleDateTimeFormatAddon::SimpleDateTimeFormatAddon() {}

SimpleDateTimeFormatAddon::~SimpleDateTimeFormatAddon() {}

void SimpleDateTimeFormatAddon::Destructor(napi_env env, void *nativeObject, void *hint)
{
    if (!nativeObject) {
        return;
    }
    delete reinterpret_cast<SimpleDateTimeFormatAddon *>(nativeObject);
    nativeObject = nullptr;
}

napi_value SimpleDateTimeFormatAddon::InitSimpleDateTimeFormat(napi_env env, napi_value exports)
{
    HandleScope scope(env, "InitSimpleDateTimeFormat");
    if (!scope.IsOpen()) {
        return nullptr;
    }

    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("format", Format),
    };

    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, "SimpleDateTimeFormat", NAPI_AUTO_LENGTH,
        SimpleDateTimeFormatConstructor, nullptr, sizeof(properties) / sizeof(napi_property_descriptor),
        properties, &constructor);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("InitSimpleDateTimeFormat: Failed to define class SimpleDateTimeFormat at Init");
        return nullptr;
    }

    g_SimpleDateTimeFormatConstructor = ThreadReference::CreateInstance(env, constructor, 1);
    if (!g_SimpleDateTimeFormatConstructor) {
        HILOG_ERROR_I18N("InitSimpleDateTimeFormat: Failed to create SimpleDateTimeFormat ref at init");
        return nullptr;
    }

    status = napi_set_named_property(env, exports, "SimpleDateTimeFormat", constructor);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("InitSimpleDateTimeFormat: Set property failed.");
        return nullptr;
    }
    return exports;
}

napi_value SimpleDateTimeFormatAddon::GetSimpleDateTimeFormatByPattern(napi_env env, napi_callback_info info)
{
    size_t argc = 0;
    std::vector<napi_value> parameters = GenerateParameter(env, info, true, argc);
    if (argc == 0) {
        HILOG_ERROR_I18N("GetSimpleDateTimeFormatByPattern: Generate parameter failed.");
        return nullptr;
    }
    napi_value constructor = nullptr;
    if (g_SimpleDateTimeFormatConstructor == nullptr) {
        HILOG_ERROR_I18N("Failed to create g_SimpleDateTimeFormatConstructor");
        return nullptr;
    }
    if (!g_SimpleDateTimeFormatConstructor->GetReferenceValue(env, &constructor)) {
        HILOG_ERROR_I18N("GetSimpleDateTimeFormatByPattern: Failed to create reference.");
        return nullptr;
    }
    napi_value argv[SIZE_WITH_LOCALE] = { nullptr };
    for (size_t i = 0; i < argc; i++) {
        argv[i] = parameters[i];
    }
    napi_value result = nullptr;
    napi_status status = napi_new_instance(env, constructor, argc, argv, &result);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("GetSimpleDateTimeFormatByPattern: Create instance failed");
        return nullptr;
    }
    return result;
}

napi_value SimpleDateTimeFormatAddon::GetSimpleDateTimeFormatBySkeleton(napi_env env, napi_callback_info info)
{
    size_t argc = 0;
    std::vector<napi_value> parameters = GenerateParameter(env, info, false, argc);
    if (argc <= 1) {
        HILOG_ERROR_I18N("GetSimpleDateTimeFormatBySkeleton: Generate parameter failed.");
        return nullptr;
    }
    napi_value constructor = nullptr;
    if (g_SimpleDateTimeFormatConstructor == nullptr) {
        HILOG_ERROR_I18N("Failed to create g_SimpleDateTimeFormatConstructor.");
        return nullptr;
    }
    if (!g_SimpleDateTimeFormatConstructor->GetReferenceValue(env, &constructor)) {
        HILOG_ERROR_I18N("GetSimpleDateTimeFormatBySkeleton: Failed to create reference.");
        return nullptr;
    }
    napi_value argv[SIZE_WITH_LOCALE] = { nullptr };
    for (size_t i = 0; i < argc; i++) {
        argv[i] = parameters[i];
    }
    napi_value result = nullptr;
    napi_status status = napi_new_instance(env, constructor, argc, argv, &result);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("GetSimpleDateTimeFormatBySkeleton: Create instance failed");
        return nullptr;
    }
    return result;
}

napi_value SimpleDateTimeFormatAddon::Format(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = { 0 };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("SimpleDateTimeFormatAddon::Format: get cb info failed.");
        return VariableConvertor::CreateString(env, "");
    }
    if (argc < 1) {
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, "date", "", true);
        return VariableConvertor::CreateString(env, "");
    }
    int64_t milliseconds = 0;
    if (!GetDateTime(env, argv[0], milliseconds)) {
        HILOG_ERROR_I18N("SimpleDateTimeFormatAddon::Format: get date time failed.");
        return VariableConvertor::CreateString(env, "");
    }

    SimpleDateTimeFormatAddon* obj = nullptr;
    status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->simpleDateTimeFormat_) {
        HILOG_ERROR_I18N("SimpleDateTimeFormatConstructor: unwrap SimpleDateTimeFormatAddon failed.");
        return VariableConvertor::CreateString(env, "");
    }
    std::string formatResult = obj->simpleDateTimeFormat_->Format(milliseconds);
    if (formatResult.empty()) {
        HILOG_ERROR_I18N("SimpleDateTimeFormatConstructor: format result is empty.");
    }
    return VariableConvertor::CreateString(env, formatResult);
}

std::vector<napi_value> SimpleDateTimeFormatAddon::GenerateParameter(napi_env env, napi_callback_info info,
    bool isBestPattern, size_t& num)
{
    size_t argc = 2;
    napi_value argv[2] = { nullptr };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    std::vector<napi_value> result;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("GenerateParameter: Failed to get cb info.");
        return result;
    }
    std::string type = isBestPattern ? "pattern" : "skeleton";
    if (argc < 1) {
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, type, "", true);
        return result;
    }

    napi_valuetype valueType = napi_valuetype::napi_undefined;
    status = napi_typeof(env, argv[0], &valueType);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("GenerateParameter: Failed to get type of argv[0].");
        return result;
    } else if (valueType != napi_valuetype::napi_string) {
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, type, "string", true);
        return result;
    }

    napi_value flag;
    status = napi_get_boolean(env, isBestPattern, &flag);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("GenerateParameter: Create js boolean failed.");
        return result;
    }
    result.emplace_back(flag);
    for (size_t i = 0; i < argc; i++) {
        result.emplace_back(argv[i]);
    }
    num = argc + 1;
    return result;
}

napi_value SimpleDateTimeFormatAddon::SimpleDateTimeFormatConstructor(napi_env env, napi_callback_info info)
{
    size_t argc = 3;
    napi_value argv[3] = { nullptr };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok || argc < SIZE_WITHOUR_LOCALE) {
        HILOG_ERROR_I18N("SimpleDateTimeFormatConstructor: Get cb info failed.");
        return nullptr;
    }
    bool isBestPattern = false;
    status = napi_get_value_bool(env, argv[0], &isBestPattern);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("SimpleDateTimeFormatConstructor: Get value argv[0] failed.");
        return nullptr;
    }

    int32_t code = 0;
    std::string skeleton = VariableConvertor::GetString(env, argv[1], code);
    if (code != 0) {
        HILOG_ERROR_I18N("SimpleDateTimeFormatConstructor: Get argv[1] failed.");
        return nullptr;
    }

    std::unique_ptr<SimpleDateTimeFormatAddon> obj = std::make_unique<SimpleDateTimeFormatAddon>();
    if (obj == nullptr) {
        HILOG_ERROR_I18N("SimpleDateTimeFormatConstructor: Cerate SimpleDateTimeFormatAddon failed.");
        return nullptr;
    }
    status = napi_wrap(env, thisVar, reinterpret_cast<void *>(obj.get()),
        SimpleDateTimeFormatAddon::Destructor, nullptr, nullptr);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("SimpleDateTimeFormatConstructor: wrap SimpleDateTimeFormatAddon failed");
        return nullptr;
    }
    I18nErrorCode errCode = I18nErrorCode::SUCCESS;
    napi_value locale = (argc > SIZE_WITHOUR_LOCALE) ? argv[SIZE_WITH_LOCALE - 1] : nullptr;
    obj->simpleDateTimeFormat_ =
        SimpleDateTimeFormatAddon::InitSimpleDateTimeFormatContext(env, locale, skeleton, isBestPattern, errCode);
    if (errCode == I18nErrorCode::INVALID_DATE_TIME_FORMAT_SKELETON) {
        ErrorUtil::NapiThrow(env, I18N_NOT_VALID, "skeleton", "a valid date time format skeleton", true);
        return nullptr;
    } else if (errCode == I18nErrorCode::INVALID_DATE_TIME_FORMAT_PATTERN) {
        ErrorUtil::NapiThrow(env, I18N_NOT_VALID, "pattern", "a valid date time format pattern", true);
        return nullptr;
    } else if (!obj->simpleDateTimeFormat_ || errCode != I18nErrorCode::SUCCESS) {
        HILOG_ERROR_I18N("SimpleDateTimeFormatConstructor: Construct SimpleDateTimeFormat failed.");
        return nullptr;
    }
    obj.release();
    return thisVar;
}

std::unique_ptr<SimpleDateTimeFormat> SimpleDateTimeFormatAddon::InitSimpleDateTimeFormatContext(napi_env env,
    napi_value locale, const std::string& skeleton, bool isBestPattern, I18nErrorCode& errCode)
{
    if (locale == nullptr) {
        std::shared_ptr<LocaleInfo> localeInfo = nullptr;
        return std::make_unique<SimpleDateTimeFormat>(skeleton, localeInfo, isBestPattern, errCode);
    }

    if (VariableConvertor::GetLocaleType(env, locale) == LocaleType::BUILTINS_LOCALE) {
        std::string localeTag = VariableConvertor::ParseBuiltinsLocale(env, locale);
        return std::make_unique<SimpleDateTimeFormat>(skeleton, localeTag, isBestPattern, errCode);
    } else if (VariableConvertor::GetLocaleType(env, locale) == LocaleType::LOCALE_INFO) {
        std::shared_ptr<LocaleInfo> localeInfo = VariableConvertor::ParseLocaleInfo(env, locale);
        return std::make_unique<SimpleDateTimeFormat>(skeleton, localeInfo, isBestPattern, errCode);
    }
    HILOG_ERROR_I18N("SimpleDateTimeFormatAddon::InitSimpleDateTimeFormatContext: Init context failed.");
    return nullptr;
}

bool SimpleDateTimeFormatAddon::GetDateTime(napi_env env, napi_value date, int64_t& milliseconds)
{
    bool isDate = false;
    napi_status status = napi_is_date(env, date, &isDate);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("SimpleDateTimeFormatAddon: Failed to call napi_is_date.");
        return false;
    } else if (!isDate) {
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, "date", "Date", true);
        return false;
    }

    napi_value funcGetTime = nullptr;
    status = napi_get_named_property(env, date, GET_TIME_NAME.c_str(), &funcGetTime);
    if (status != napi_ok || funcGetTime == nullptr) {
        HILOG_ERROR_I18N("SimpleDateTimeFormatAddon::GetDateTime: Get named property failed.");
        return false;
    }

    napi_value value = nullptr;
    status = napi_call_function(env, date, funcGetTime, 0, nullptr, &value);
    if (status != napi_ok || value == nullptr) {
        HILOG_ERROR_I18N("SimpleDateTimeFormatAddon::GetDateTime: Call getTime failed.");
        return false;
    }

    status = napi_get_value_int64(env, value, &milliseconds);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("SimpleDateTimeFormatAddon::GetDateTime: Get milliseconds failed.");
        return false;
    }
    return true;
}
} // namespace I18n
} // namespace Global
} // namespace OHOS