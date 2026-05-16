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
#include "displaynames_addon.h"

#include "error_util.h"
#include "intl_addon.h"
#include "i18n_hilog.h"
#include "js_lifecycle_managers.h"
#include "js_utils.h"

namespace OHOS {
namespace Global {
namespace I18n {
DisplayNamesAddon::DisplayNamesAddon() : env_(nullptr) {}

DisplayNamesAddon::~DisplayNamesAddon()
{
}

void DisplayNamesAddon::Destructor(napi_env env, void *nativeObject, void *hint)
{
    if (!nativeObject) {
        return;
    }
    delete reinterpret_cast<DisplayNamesAddon *>(nativeObject);
    nativeObject = nullptr;
}

napi_value DisplayNamesAddon::InitDisplayNames(napi_env env, napi_value exports)
{
    HandleScope scope(env, "InitDisplayNames");
    if (!scope.IsOpen()) {
        return nullptr;
    }

    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("of", Of),
        DECLARE_NAPI_FUNCTION("toString", ToString),
        DECLARE_NAPI_FUNCTION("resolvedOptions", ResolvedOptions),
        DECLARE_NAPI_STATIC_FUNCTION("supportedLocalesOf", SupportedLocalesOf),
    };

    napi_value constructor;
    napi_status status = napi_define_class(env, "DisplayNames", NAPI_AUTO_LENGTH, DisplayNamesConstructor, nullptr,
        sizeof(properties) / sizeof(napi_property_descriptor), properties, &constructor);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("InitDisplayNames: Define class failed when InitDisplayNames");
        return nullptr;
    }

    status = napi_set_named_property(env, exports, "DisplayNames", constructor);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("InitDisplayNames: Set property failed when InitDisplayNames");
        return nullptr;
    }
    return exports;
}

napi_value DisplayNamesAddon::DisplayNamesConstructor(napi_env env, napi_callback_info info)
{
    size_t argc = 2;
    napi_value argv[2] = { nullptr };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("DisplayNamesConstructor: napi get callback info failed.");
        return nullptr;
    }
    std::vector<std::string> localeTags;
    if (argc < 2) {  // 2 indicates the number of parameters
        HILOG_ERROR_I18N("DisplayNamesConstructor: Two arguments are needed, but only get %{public}zu.", argc);
        ErrorUtil::NapiThrowUndefined(env, "options is undefined");
        return nullptr;
    }
    int32_t code = 0;
    std::vector<std::string> localeArray = JSUtils::GetLocaleArray(env, argv[0], code);
    localeTags.assign(localeArray.begin(), localeArray.end());
    std::map<std::string, std::string> map = {};
    GetOptionValues(env, argv[1], map);
    DisplayNamesAddon* obj = new(std::nothrow) DisplayNamesAddon();
    if (obj == nullptr) {
        HILOG_ERROR_I18N("DisplayNamesConstructor: make unique ptr failed.");
        return nullptr;
    }
    status = napi_wrap_s(env, thisVar, reinterpret_cast<void *>(obj), DisplayNamesAddon::Destructor, nullptr,
        &TYPE_TAG, nullptr);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("DisplayNamesConstructor: Wrap DisplayNamesAddon failed");
        delete obj;
        return nullptr;
    }
    if (!obj->InitDisplayNamesContext(env, info, localeTags, map)) {
        return nullptr;
    }
    return thisVar;
}

bool DisplayNamesAddon::InitDisplayNamesContext(napi_env env, napi_callback_info info,
    const std::vector<std::string> &localeTags, const std::map<std::string, std::string> &map)
{
    env_ = env;
    displaynames_ = std::make_unique<DisplayNames>(localeTags, map);
    if (displaynames_ == nullptr) {
        HILOG_ERROR_I18N("InitDisplayNamesContext: make_unique DisplayNames failed");
        return false;
    }
    I18nErrorCode errorCode = displaynames_->GetError();
    if (errorCode == I18nErrorCode::MISSING_PARAM) {
        HILOG_ERROR_I18N("InitDisplayNamesContext: MISSING_PARAM failed");
        ErrorUtil::NapiThrowUndefined(env, "type is undefined");
        return false;
    } else if (errorCode == I18nErrorCode::INVALID_PARAM) {
        HILOG_ERROR_I18N("InitDisplayNamesContext: INVALID_PARAM failed");
        ErrorUtil::NapiThrowUndefined(env, "getStringOption failed");
        return false;
    } else if (errorCode == I18nErrorCode::INVALID_LOCALE_TAG) {
        HILOG_ERROR_I18N("InitDisplayNamesContext: invalid locale");
        ErrorUtil::NapiThrowUndefined(env, "invalid locale");
        return false;
    }
    return errorCode == I18nErrorCode::SUCCESS;
}

napi_value DisplayNamesAddon::SupportedLocalesOf(napi_env env, napi_callback_info info)
{
    size_t argc = 2;
    napi_value argv[2] = { nullptr };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    std::vector<std::string> resultLocales = {};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("DisplayNamesAddon::SupportedLocalesOf: napi get callback info failed.");
        return JSUtils::CreateArray(env, resultLocales);
    }
    std::vector<std::string> localeTags;
    if (argc > 0) {
        int32_t code = 0;
        std::vector<std::string> localeArray = JSUtils::GetLocaleArray(env, argv[0], code);
        if (code != 0) {
            HILOG_ERROR_I18N("DisplayNamesAddon::SupportedLocalesOf: JSUtils:GetLocaleArray failed.");
            ErrorUtil::NapiThrowUndefined(env, "kValue is not String or Object");
            return JSUtils::CreateArray(env, resultLocales);
        }
        localeTags.assign(localeArray.begin(), localeArray.end());
    }
    std::map<std::string, std::string> map = {};
    if (argc > 1) {
        JSUtils::GetOptionValue(env, argv[1], "localeMatcher", map);
    }
    I18nErrorCode i18nStatus = I18nErrorCode::SUCCESS;
    resultLocales = DisplayNames::SupportedLocalesOf(localeTags, map, i18nStatus);
    if (i18nStatus == I18nErrorCode::INVALID_PARAM) {
        HILOG_ERROR_I18N("DisplayNamesAddon::SupportedLocalesOf: SupportedLocalesOf invalid param");
        ErrorUtil::NapiThrowUndefined(env, "getStringOption failed");
    } else if (i18nStatus == I18nErrorCode::INVALID_LOCALE_TAG) {
        HILOG_ERROR_I18N("DisplayNamesAddon::SupportedLocalesOf: SupportedLocalesOf invalid locale");
        ErrorUtil::NapiThrowUndefined(env, "invalid locale");
    }
    return JSUtils::CreateArray(env, resultLocales);
}

napi_value DisplayNamesAddon::Of(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = { nullptr };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("DisplayNamesAddon::Of: napi get callback info failed.");
        return JSUtils::CreateEmptyString(env);
    }
    if (argc < 1) {
        HILOG_ERROR_I18N("DisplayNamesAddon::Of: missing code param.");
        ErrorUtil::NapiThrowUndefined(env, "code is undefined");
        return JSUtils::CreateEmptyString(env);
    }
    int32_t errorCode = 0;
    std::string code = JSUtils::GetString(env, argv[0], errorCode);
    if (errorCode != 0) {
        return JSUtils::CreateEmptyString(env);
    }

    DisplayNamesAddon *obj = nullptr;
    status = napi_unwrap_s(env, thisVar, &TYPE_TAG, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->displaynames_) {
        HILOG_ERROR_I18N("DisplayNamesAddon::Of: Get DisplayNames object failed");
        return JSUtils::CreateEmptyString(env);
    }
    std::string displayResult = obj->displaynames_->Display(code);
    I18nErrorCode i18nErrorCode = obj->displaynames_->GetError();
    if (i18nErrorCode != I18nErrorCode::SUCCESS) {
        std::string errorMessage = obj->displaynames_->GetErrorMessage();
        HILOG_ERROR_I18N("DisplayNamesAddon::Of: %{public}s.", errorMessage.c_str());
        ErrorUtil::NapiThrowUndefined(env, errorMessage);
        return JSUtils::CreateEmptyString(env);
    }
    return JSUtils::CreateString(env, displayResult);
}

napi_value DisplayNamesAddon::ToString(napi_env env, napi_callback_info info)
{
    return JSUtils::CreateString(env, "[object Intl.DisplayNames]");
}

napi_value DisplayNamesAddon::ResolvedOptions(napi_env env, napi_callback_info info)
{
    napi_value result = nullptr;
    napi_status status = napi_create_object(env, &result);
    if (status != napi_ok) {
        return JSUtils::CreateEmptyObject(env);
    }

    napi_value thisVar = nullptr;
    void *data = nullptr;
    status = napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, &data);
    if (status != napi_ok) {
        return result;
    }
    DisplayNamesAddon *obj = nullptr;
    status = napi_unwrap_s(env, thisVar, &TYPE_TAG, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->displaynames_) {
        HILOG_ERROR_I18N("DisplayNamesAddon::ResolvedOptions: Get DisplayNames object failed");
        return result;
    }
    std::map<std::string, std::string> options = obj->displaynames_->ResolvedOptions();
    JSUtils::SetOptionProperties(env, result, options, "locale");
    JSUtils::SetOptionProperties(env, result, options, "style");
    JSUtils::SetOptionProperties(env, result, options, "type");
    JSUtils::SetOptionProperties(env, result, options, "fallback");
    JSUtils::SetOptionProperties(env, result, options, "languageDisplay");
    return result;
}

void DisplayNamesAddon::GetOptionValues(napi_env env, napi_value options, std::map<std::string, std::string> &map)
{
    JSUtils::GetOptionValue(env, options, "localeMatcher", map);
    JSUtils::GetOptionValue(env, options, "style", map);
    JSUtils::GetOptionValue(env, options, "type", map);
    JSUtils::GetOptionValue(env, options, "fallback", map);
    JSUtils::GetOptionValue(env, options, "languageDisplay", map);
}
} // namespace I18n
} // namespace Global
} // namespace OHOS