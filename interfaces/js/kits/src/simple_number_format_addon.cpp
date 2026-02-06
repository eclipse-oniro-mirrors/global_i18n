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
#include "simple_number_format_addon.h"

namespace OHOS {
namespace Global {
namespace I18n {
static thread_local ThreadReference* g_SimpleNumberFormatConstructor = nullptr;

SimpleNumberFormatAddon::SimpleNumberFormatAddon() {}

SimpleNumberFormatAddon::~SimpleNumberFormatAddon() {}

void SimpleNumberFormatAddon::Destructor(napi_env env, void *nativeObject, void *hint)
{
    if (!nativeObject) {
        return;
    }
    delete reinterpret_cast<SimpleNumberFormatAddon *>(nativeObject);
    nativeObject = nullptr;
}

napi_value SimpleNumberFormatAddon::InitSimpleNumberFormat(napi_env env, napi_value exports)
{
    HandleScope scope(env, "InitSimpleNumberFormat");
    if (!scope.IsOpen()) {
        return nullptr;
    }

    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("format", Format),
    };

    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, "SimpleNumberFormat", NAPI_AUTO_LENGTH,
        SimpleNumberFormatConstructor, nullptr, sizeof(properties) / sizeof(napi_property_descriptor),
        properties, &constructor);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("InitSimpleNumberFormat: Failed to define class SimpleNumberFormat at Init.");
        return nullptr;
    }

    g_SimpleNumberFormatConstructor = ThreadReference::CreateInstance(env, constructor, 1);
    if (!g_SimpleNumberFormatConstructor) {
        HILOG_ERROR_I18N("InitSimpleNumberFormat: Failed to create SimpleNumberFormat ref at init.");
        return nullptr;
    }

    status = napi_set_named_property(env, exports, "SimpleNumberFormat", constructor);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("InitSimpleNumberFormat: Set property failed at init.");
        return nullptr;
    }
    return exports;
}

napi_value SimpleNumberFormatAddon::GetSimpleNumberFormatBySkeleton(napi_env env, napi_callback_info info)
{
    size_t argc = 2;
    napi_value argv[2] = { nullptr };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("GetSimpleNumberFormatBySkeleton: Get cb info failed.");
        return nullptr;
    }

    napi_value constructor = nullptr;
    if (g_SimpleNumberFormatConstructor == nullptr) {
        HILOG_ERROR_I18N("Failed to create g_SimpleNumberFormatConstructor");
        return nullptr;
    }
    if (!g_SimpleNumberFormatConstructor->GetReferenceValue(env, &constructor)) {
        HILOG_ERROR_I18N("GetSimpleNumberFormatBySkeleton: Failed to create reference.");
        return nullptr;
    }

    napi_value result = nullptr;
    status = napi_new_instance(env, constructor, argc, argv, &result);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("GetSimpleNumberFormatBySkeleton: Create instance failed.");
        return nullptr;
    }
    return result;
}

napi_value SimpleNumberFormatAddon::Format(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = { 0 };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("SimpleNumberFormatAddon::Format: Get cb info failed.");
        return VariableConvertor::CreateString(env, "");
    } else if (argc < 1) {
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, "value", "", true);
        return VariableConvertor::CreateString(env, "");
    }

    napi_valuetype valueType = napi_valuetype::napi_undefined;
    status = napi_typeof(env, argv[0], &valueType);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("SimpleNumberFormatAddon::Format: Failed to get type of argv[0].");
        return VariableConvertor::CreateString(env, "");
    } else if (valueType != napi_valuetype::napi_number) {
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, "value", "number", true);
        return VariableConvertor::CreateString(env, "");
    }

    double value = 0;
    status = napi_get_value_double(env, argv[0], &value);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("SimpleNumberFormatAddon::Format: Failed to get value argv[0].");
        return VariableConvertor::CreateString(env, "");
    }

    SimpleNumberFormatAddon* obj = nullptr;
    status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->simpleNumberFormat_) {
        HILOG_ERROR_I18N("SimpleNumberFormatAddon::Format: Unwrap SimpleNumberFormatAddon failed.");
        return VariableConvertor::CreateString(env, "");
    }

    std::string formatResult = obj->simpleNumberFormat_->Format(value);
    if (formatResult.empty()) {
        HILOG_ERROR_I18N("SimpleNumberFormatAddon::Format: Format result is empty.");
    }
    return VariableConvertor::CreateString(env, formatResult);
}

napi_value SimpleNumberFormatAddon::SimpleNumberFormatConstructor(napi_env env, napi_callback_info info)
{
    size_t argc = 2;
    napi_value argv[2] = { nullptr };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("SimpleNumberFormatConstructor: Get cb info failed.");
        return nullptr;
    } else if (argc < 1) {
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, "skeleton", "", true);
        return nullptr;
    }

    VariableConvertor::VerifyType(env, "skeleton", napi_valuetype::napi_string, argv[0]);

    int32_t code = 0;
    std::string skeleton = VariableConvertor::GetString(env, argv[0], code);
    if (code != 0) {
        HILOG_ERROR_I18N("SimpleNumberFormatConstructor: Get argv[0] failed.");
        return nullptr;
    }

    std::unique_ptr<SimpleNumberFormatAddon> obj = std::make_unique<SimpleNumberFormatAddon>();
    if (!obj) {
        HILOG_ERROR_I18N("SimpleNumberFormatConstructor: create SimpleNumberFormatAddon failed.");
        return nullptr;
    }
    status = napi_wrap(env, thisVar, reinterpret_cast<void *>(obj.get()),
        SimpleNumberFormatAddon::Destructor, nullptr, nullptr);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("SimpleNumberFormatConstructor: Wrap SimpleNumberFormatAddon failed.");
        return nullptr;
    }

    I18nErrorCode errCode = I18nErrorCode::SUCCESS;
    napi_value locale = (argc > 1) ? argv[1] : nullptr;
    obj->simpleNumberFormat_ =
        SimpleNumberFormatAddon::InitSimpleNumberFormatContext(env, locale, skeleton, errCode);
    if (errCode == I18nErrorCode::INVALID_NUMBER_FORMAT_SKELETON) {
        ErrorUtil::NapiThrow(env, I18N_NOT_VALID, "skeleton", "a valid number format skeleton", true);
        return nullptr;
    } else if (!obj->simpleNumberFormat_ || errCode != I18nErrorCode::SUCCESS) {
        HILOG_ERROR_I18N("SimpleNumberFormatConstructor: Construct SimpleNumberFormat failed.");
        return nullptr;
    }
    obj.release();
    return thisVar;
}

std::unique_ptr<SimpleNumberFormat> SimpleNumberFormatAddon::InitSimpleNumberFormatContext(napi_env env,
    napi_value locale, const std::string& skeleton, I18nErrorCode& errCode)
{
    if (locale == nullptr) {
        std::shared_ptr<LocaleInfo> localeInfo = nullptr;
        return std::make_unique<SimpleNumberFormat>(skeleton, localeInfo, errCode);
    }

    if (VariableConvertor::GetLocaleType(env, locale) == LocaleType::BUILTINS_LOCALE) {
        std::string localeTag = VariableConvertor::ParseBuiltinsLocale(env, locale);
        return std::make_unique<SimpleNumberFormat>(skeleton, localeTag, errCode);
    } else if (VariableConvertor::GetLocaleType(env, locale) == LocaleType::LOCALE_INFO) {
        std::shared_ptr<LocaleInfo> localeInfo = VariableConvertor::ParseLocaleInfo(env, locale);
        return std::make_unique<SimpleNumberFormat>(skeleton, localeInfo, errCode);
    }
    HILOG_ERROR_I18N("SimpleNumberFormatAddon::InitSimpleNumberFormatContext: Init context failed.");
    return nullptr;
}

std::shared_ptr<SimpleNumberFormat> SimpleNumberFormatAddon::CopySimpleNumberFormat()
{
    return simpleNumberFormat_;
}
} // namespace I18n
} // namespace Global
} // namespace OHOS