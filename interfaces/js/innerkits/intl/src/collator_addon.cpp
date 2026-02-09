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
#include "collator_addon.h"

#include "error_util.h"
#include "intl_addon.h"
#include "i18n_hilog.h"
#include "js_lifecycle_managers.h"
#include "js_utils.h"

namespace OHOS {
namespace Global {
namespace I18n {

CollatorAddon::CollatorAddon() : env_(nullptr) {}

CollatorAddon::~CollatorAddon()
{
}

void CollatorAddon::Destructor(napi_env env, void *nativeObject, void *hint)
{
    if (!nativeObject) {
        return;
    }
    delete reinterpret_cast<CollatorAddon *>(nativeObject);
    nativeObject = nullptr;
}

napi_value CollatorAddon::InitCollator(napi_env env, napi_value exports)
{
    HandleScope scope(env, "InitCollator");
    if (!scope.IsOpen()) {
        return nullptr;
    }

    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("compare", CompareString),
        DECLARE_NAPI_FUNCTION("resolvedOptions", GetCollatorResolvedOptions),
        DECLARE_NAPI_FUNCTION("toString", ToString),
        DECLARE_NAPI_STATIC_FUNCTION("supportedLocalesOf", SupportedLocalesOf),
    };

    napi_value constructor;
    napi_status status = napi_define_class(env, "Collator", NAPI_AUTO_LENGTH, CollatorConstructor, nullptr,
        sizeof(properties) / sizeof(napi_property_descriptor), properties, &constructor);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("Define class failed when InitCollator");
        return nullptr;
    }

    status = napi_set_named_property(env, exports, "Collator", constructor);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("Set property failed when InitCollator");
        return nullptr;
    }
    return exports;
}

napi_value CollatorAddon::CollatorConstructor(napi_env env, napi_callback_info info)
{
    size_t argc = 2;
    napi_value argv[2] = { nullptr };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("CollatorConstructor: napi_get_cb_info failed");
        return nullptr;
    }

    napi_value new_target;
    status = napi_get_new_target(env, info, &new_target);
    if (status == napi_pending_exception || new_target == nullptr) {
        return CreateCollatorWithoutNew(env, argc, argv);
    }

    std::vector<std::string> localeTags;
    if (argc > 0) {
        int32_t code = 0;
        std::vector<std::string> localeArray = JSUtils::GetLocaleArray(env, argv[0], code);
        localeTags.assign(localeArray.begin(), localeArray.end());
    }
    std::map<std::string, std::string> map = {};
    if (argc > 1) {
        GetCollatorOptionValue(env, argv[1], map);
    }
    CollatorAddon* obj = new(std::nothrow) CollatorAddon();
    if (obj == nullptr) {
        HILOG_ERROR_I18N("CollatorConstructor: make_unique CollatorAddon failed");
        return nullptr;
    }
    status = napi_wrap_s(env, thisVar, reinterpret_cast<void *>(obj), CollatorAddon::Destructor, nullptr,
        &TYPE_TAG, nullptr);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("CollatorConstructor: Wrap CollatorAddon failed");
        delete obj;
        return nullptr;
    }
    if (!obj->InitCollatorContext(env, info, localeTags, map)) {
        HILOG_ERROR_I18N("CollatorConstructor: Init CollatorAddon failed");
        return nullptr;
    }
    return thisVar;
}

napi_value CollatorAddon::CreateCollatorWithoutNew(napi_env env, size_t argc, napi_value *argv)
{
    napi_value exception;
    napi_status status = napi_get_and_clear_last_exception(env, &exception);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("CreateCollatorWithoutNew: napi_get_and_clear_last_exception failed");
        return nullptr;
    }
    napi_value constructor = JSUtils::GetConstructor(env, "Collator");
    if (constructor == nullptr) {
        HILOG_ERROR_I18N("CreateCollatorWithoutNew: get constructor failed");
        return nullptr;
    }
    napi_value instance = nullptr;
    status = napi_new_instance(env, constructor, argc, argv, &instance);
    if (status != napi_ok || instance == nullptr) {
        HILOG_ERROR_I18N("CreateCollatorWithoutNew: napi_new_instance failed");
        return nullptr;
    }
    return instance;
}

bool CollatorAddon::InitCollatorContext(napi_env env, napi_callback_info info, std::vector<std::string> localeTags,
    std::map<std::string, std::string> &map)
{
    env_ = env;
    collator_ = std::make_unique<Collator>(localeTags, map, "en-US");
    if (collator_ == nullptr) {
        HILOG_ERROR_I18N("CollatorAddon: CollatorConstructor make_unique Collator failed");
        return false;
    }
    I18nErrorCode errorCode = collator_->GetError();
    if (errorCode == I18nErrorCode::INVALID_PARAM) {
        HILOG_ERROR_I18N("CollatorAddon: CollatorConstructor invalid param");
        ErrorUtil::NapiThrowUndefined(env, "getStringOption failed");
        return false;
    } else if (errorCode == I18nErrorCode::INVALID_LOCALE_TAG) {
        HILOG_ERROR_I18N("CollatorAddon: CollatorConstructor invalid locale");
        ErrorUtil::NapiThrowUndefined(env, "invalid locale");
        return false;
    }
    return errorCode == I18nErrorCode::SUCCESS;
}

napi_value CollatorAddon::SupportedLocalesOf(napi_env env, napi_callback_info info)
{
    size_t argc = 2;
    napi_value argv[2] = { nullptr };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    std::vector<std::string> resultLocales = {};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("SupportedLocalesOf: napi get callback info failed.");
        return JSUtils::CreateArray(env, resultLocales);
    }
    std::vector<std::string> localeTags;
    if (argc > 0) {
        int32_t code = 0;
        std::vector<std::string> localeArray = JSUtils::GetLocaleArray(env, argv[0], code);
        localeTags.assign(localeArray.begin(), localeArray.end());
    }
    std::map<std::string, std::string> map = {};
    if (argc > 1) {
        GetCollatorOptionValue(env, argv[1], map);
    }
    I18nErrorCode i18nStatus = I18nErrorCode::SUCCESS;
    resultLocales = Collator::SupportedLocalesOf(localeTags, map, i18nStatus);
    if (i18nStatus == I18nErrorCode::INVALID_PARAM) {
        HILOG_ERROR_I18N("SupportedLocalesOf: SupportedLocalesOf invalid param");
        ErrorUtil::NapiThrowUndefined(env, "getStringOption failed");
    } else if (i18nStatus == I18nErrorCode::INVALID_LOCALE_TAG) {
        HILOG_ERROR_I18N("SupportedLocalesOf: SupportedLocalesOf invalid locale");
        ErrorUtil::NapiThrowUndefined(env, "invalid locale");
    }
    return JSUtils::CreateArray(env, resultLocales);
}

napi_value CollatorAddon::CompareString(napi_env env, napi_callback_info info)
{
    size_t argc = 2;
    napi_value argv[2] = { 0 };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        return nullptr;
    }
    std::vector<char> first;
    if (!GetStringParameter(env, argv[0], first)) {
        return nullptr;
    }

    std::vector<char> second;
    if (!GetStringParameter(env, argv[1], second)) {
        return nullptr;
    }

    CollatorAddon *obj = nullptr;
    status = napi_unwrap_s(env, thisVar, &TYPE_TAG, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->collator_) {
        HILOG_ERROR_I18N("CompareString: Get Collator object failed");
        return nullptr;
    }

    CompareResult compareResult = obj->collator_->Compare(first.data(), second.data());
    napi_value result = nullptr;
    status = napi_create_int32(env, compareResult, &result);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("CompareString: Create compare result failed");
        return nullptr;
    }

    return result;
}

napi_value CollatorAddon::ToString(napi_env env, napi_callback_info info)
{
    return JSUtils::CreateString(env, "[object Intl.Collator]");
}

napi_value CollatorAddon::GetCollatorResolvedOptions(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, &data);
    if (status != napi_ok) {
        return JSUtils::CreateEmptyObject(env);
    }
    CollatorAddon *obj = nullptr;
    status = napi_unwrap_s(env, thisVar, &TYPE_TAG, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->collator_) {
        HILOG_ERROR_I18N("GetCollatorResolvedOptions: Get Collator object failed");
        return JSUtils::CreateEmptyObject(env);
    }
    napi_value result = nullptr;
    status = napi_create_object(env, &result);
    if (status != napi_ok) {
        return JSUtils::CreateEmptyObject(env);
    }
    std::map<std::string, std::string> options = {};
    obj->collator_->ResolvedOptions(options);
    JSUtils::SetOptionProperties(env, result, options, "localeMatcher");
    JSUtils::SetOptionProperties(env, result, options, "locale");
    JSUtils::SetOptionProperties(env, result, options, "usage");
    JSUtils::SetOptionProperties(env, result, options, "sensitivity");
    JSUtils::SetBooleanOptionProperties(env, result, options, "ignorePunctuation");
    JSUtils::SetBooleanOptionProperties(env, result, options, "numeric");
    JSUtils::SetOptionProperties(env, result, options, "caseFirst");
    JSUtils::SetOptionProperties(env, result, options, "collation");
    return result;
}
} // namespace I18n
} // namespace Global
} // namespace OHOS