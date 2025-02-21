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
#include "js_utils.h"
#include "locale_config.h"
#include "variable_convertor.h"
#include "i18n_normalizer_addon.h"

namespace OHOS {
namespace Global {
namespace I18n {
static thread_local napi_ref* g_normalizerConstructor = nullptr;

const char *I18nNormalizerAddon::NORMALIZER_MODE_NFC_NAME = "NFC";
const char *I18nNormalizerAddon::NORMALIZER_MODE_NFD_NAME = "NFD";
const char *I18nNormalizerAddon::NORMALIZER_MODE_NFKC_NAME = "NFKC";
const char *I18nNormalizerAddon::NORMALIZER_MODE_NFKD_NAME = "NFKD";

I18nNormalizerAddon::I18nNormalizerAddon() {}
I18nNormalizerAddon::~I18nNormalizerAddon()
{
}

void I18nNormalizerAddon::Destructor(napi_env env, void *nativeObject, void *hint)
{
    if (!nativeObject) {
        return;
    }
    delete reinterpret_cast<I18nNormalizerAddon *>(nativeObject);
    nativeObject = nullptr;
}

napi_value I18nNormalizerAddon::InitI18nNormalizer(napi_env env, napi_value exports)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("normalize", Normalize),
    };
    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, "Normalizer", NAPI_AUTO_LENGTH, I18nNormalizerConstructor, nullptr,
        sizeof(properties) / sizeof(napi_property_descriptor), properties, &constructor);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("InitI18nNormalizer: Failed to define class Normalizer at Init");
        return nullptr;
    }
    exports = I18nNormalizerAddon::InitNormalizer(env, exports);
    g_normalizerConstructor = new (std::nothrow) napi_ref;
    if (!g_normalizerConstructor) {
        HILOG_ERROR_I18N("InitI18nNormalizer: Failed to create Normalizer ref at init");
        return nullptr;
    }
    status = napi_create_reference(env, constructor, 1, g_normalizerConstructor);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("InitI18nNormalizer: Failed to create reference g_normalizerConstructor at init.");
        return nullptr;
    }
    return exports;
}

napi_value I18nNormalizerAddon::InitNormalizer(napi_env env, napi_value exports)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_STATIC_FUNCTION("getInstance", GetI18nNormalizerInstance)
    };
    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, "I18nNormalizer", NAPI_AUTO_LENGTH, JSUtils::DefaultConstructor,
        nullptr, sizeof(properties) / sizeof(napi_property_descriptor), properties, &constructor);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("InitNormalizer: Failed to define class Normalizer.");
        return nullptr;
    }
    status = napi_set_named_property(env, exports, "Normalizer", constructor);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("InitNormalizer: Set property failed When InitNormalizer.");
        return nullptr;
    }
    return exports;
}

napi_value I18nNormalizerAddon::I18nNormalizerConstructor(napi_env env, napi_callback_info info)
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
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, "mode", "", true);
        return nullptr;
    }
    napi_valuetype valueType = napi_valuetype::napi_undefined;
    napi_typeof(env, argv[0], &valueType);
    
    if (valueType != napi_valuetype::napi_number) {
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, "mode", "number", true);
        return nullptr;
    }
    int32_t normalizerMode;
    status = napi_get_value_int32(env, argv[0], &normalizerMode);
    if (status != napi_ok) {
        return nullptr;
    }
    if (normalizerMode != NORMALIZER_MODE_NFC && normalizerMode != NORMALIZER_MODE_NFD &&
        normalizerMode != NORMALIZER_MODE_NFKC && normalizerMode != NORMALIZER_MODE_NFKD) {
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, "mode", "a valid mode", true);
        return nullptr;
    }

    std::unique_ptr<I18nNormalizerAddon> obj = std::make_unique<I18nNormalizerAddon>();
    status =
        napi_wrap(env, thisVar, reinterpret_cast<void *>(obj.get()), I18nNormalizerAddon::Destructor, nullptr, nullptr);
    if (status != napi_ok) {
        return nullptr;
    }
    I18nNormalizerMode mode = I18nNormalizerMode(normalizerMode);
    I18nErrorCode errorCode = I18nErrorCode::SUCCESS;
    obj->normalizer_ = std::make_unique<I18nNormalizer>(mode, errorCode);
    if (errorCode != I18nErrorCode::SUCCESS || !obj->normalizer_) {
        return nullptr;
    }
    obj.release();
    return thisVar;
}

napi_value I18nNormalizerAddon::Normalize(napi_env env, napi_callback_info info)
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
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, "text", "", true);
        return nullptr;
    }
    napi_valuetype valueType = napi_valuetype::napi_undefined;
    napi_typeof(env, argv[0], &valueType);
    if (valueType != napi_valuetype::napi_string) {
        HILOG_ERROR_I18N("Invalid parameter type");
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, "text", "string", true);
        return nullptr;
    }
    int32_t code = 0;
    std::string text = VariableConvertor::GetString(env, argv[0], code);
    if (code != 0) {
        return nullptr;
    }

    I18nNormalizerAddon *obj = nullptr;
    status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || obj == nullptr || obj->normalizer_ == nullptr) {
        HILOG_ERROR_I18N("Get Normalizer object failed");
        return nullptr;
    }
    I18nErrorCode errorCode = I18nErrorCode::SUCCESS;
    std::string normalizedText = obj->normalizer_->Normalize(text.c_str(), static_cast<int32_t>(text.length()),
        errorCode);
    if (errorCode != I18nErrorCode::SUCCESS) {
        return nullptr;
    }
    napi_value result = nullptr;
    status = napi_create_string_utf8(env, normalizedText.c_str(), NAPI_AUTO_LENGTH, &result);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("Create result failed");
        return nullptr;
    }
    return result;
}

napi_status I18nNormalizerAddon::SetEnumValue(napi_env env, napi_value enumObj, const char* enumName, int32_t enumVal)
{
    napi_value name = nullptr;
    napi_status status = napi_create_string_utf8(env, enumName, NAPI_AUTO_LENGTH, &name);
    if (status != napi_ok) {
        return status;
    }
    napi_value value = nullptr;
    status = napi_create_int32(env, enumVal, &value);
    if (status != napi_ok) {
        return status;
    }
    status = napi_set_property(env, enumObj, name, value);
    if (status != napi_ok) {
        return status;
    }
    status = napi_set_property(env, enumObj, value, name);
    if (status != napi_ok) {
        return status;
    }
    return napi_ok;
}

napi_value I18nNormalizerAddon::CreateI18NNormalizerModeEnum(napi_env env, napi_status &initStatus)
{
    napi_value i18nNormalizerModel = nullptr;
    napi_status status = napi_create_object(env, &i18nNormalizerModel);
    if (status != napi_ok) {
        initStatus = napi_generic_failure;
        return nullptr;
    }
    status = SetEnumValue(env, i18nNormalizerModel, NORMALIZER_MODE_NFC_NAME, NORMALIZER_MODE_NFC);
    if (status != napi_ok) {
        initStatus = napi_generic_failure;
        return nullptr;
    }
    status = SetEnumValue(env, i18nNormalizerModel, NORMALIZER_MODE_NFD_NAME, NORMALIZER_MODE_NFD);
    if (status != napi_ok) {
        initStatus = napi_generic_failure;
        return nullptr;
    }
    status = SetEnumValue(env, i18nNormalizerModel, NORMALIZER_MODE_NFKC_NAME, NORMALIZER_MODE_NFKC);
    if (status != napi_ok) {
        initStatus = napi_generic_failure;
        return nullptr;
    }
    status = SetEnumValue(env, i18nNormalizerModel, NORMALIZER_MODE_NFKD_NAME, NORMALIZER_MODE_NFKD);
    if (status != napi_ok) {
        initStatus = napi_generic_failure;
        return nullptr;
    }
    return i18nNormalizerModel;
}

napi_value I18nNormalizerAddon::GetI18nNormalizerInstance(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = { nullptr };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("Failed to get parameter of Normalizer.createInstance");
        return nullptr;
    }

    napi_value constructor = nullptr;
    status = napi_get_reference_value(env, *g_normalizerConstructor, &constructor);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("Failed to create reference of normalizer Constructor");
        return nullptr;
    }

    napi_value result = nullptr;
    status = napi_new_instance(env, constructor, argc, argv, &result);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("create normalizer instance failed");
        return nullptr;
    }
    return result;
}
} // namespace I18n
} // namespace Global
} // namespace OHOS