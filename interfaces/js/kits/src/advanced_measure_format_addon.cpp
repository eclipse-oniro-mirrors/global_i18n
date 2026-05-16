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
#include "advanced_measure_format_addon.h"

#include "error_util.h"
#include "intl_number_format_addon.h"
#include "js_lifecycle_managers.h"
#include "js_utils.h"
#include "variable_convertor.h"


namespace OHOS {
namespace Global {
namespace I18n {
AdvancedMeasureFormatAddon::AdvancedMeasureFormatAddon() {}

AdvancedMeasureFormatAddon::~AdvancedMeasureFormatAddon() {}

void AdvancedMeasureFormatAddon::Destructor(napi_env env, void *nativeObject, void *hint)
{
    if (!nativeObject) {
        return;
    }
    delete reinterpret_cast<AdvancedMeasureFormatAddon *>(nativeObject);
    nativeObject = nullptr;
}

napi_value AdvancedMeasureFormatAddon::InitAdvancedMeasureFormat(napi_env env, napi_value exports)
{
    HandleScope scope(env, "InitAdvancedMeasureFormat");
    if (!scope.IsOpen()) {
        return nullptr;
    }

    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("format", Format)
    };
    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, "AdvancedMeasureFormat", NAPI_AUTO_LENGTH, Constructor, nullptr,
        sizeof(properties) / sizeof(napi_property_descriptor), properties, &constructor);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("InitAdvancedMeasureFormat: Define class AdvancedMeasureFormat failed.");
        return nullptr;
    }
    status = napi_set_named_property(env, exports, "AdvancedMeasureFormat", constructor);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("InitAdvancedMeasureFormat: Set property AdvancedMeasureFormat failed.");
        return nullptr;
    }
    return exports;
}

napi_value AdvancedMeasureFormatAddon::Constructor(napi_env env, napi_callback_info info)
{
    size_t argc = 2;
    napi_value argv[2] = { nullptr };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("AdvancedMeasureFormatAddon::Constructor: Get callback info error.");
        return nullptr;
    } else if (argc < 1) {
        ErrorUtil::NapiNotFoundError(env, I18N_NOT_FOUND, "numberFormat", true);
        return nullptr;
    }

    IntlNumberFormatAddon* intlNumberFormatAddon = nullptr;
    status = IntlNumberFormatAddon::UnwrapIntlNumberFormat(env, argv[0], &intlNumberFormatAddon);
    if (status != napi_ok || intlNumberFormatAddon == nullptr) {
        HILOG_ERROR_I18N("AdvancedMeasureFormatAddon::Constructor: Unwrap IntlNumberFormatAddon failed.");
        return nullptr;
    }

    AdvancedMeasureFormatOptions options = ParseOptions(env, argv[1]);

    std::unique_ptr<AdvancedMeasureFormatAddon> obj = std::make_unique<AdvancedMeasureFormatAddon>();
    status = napi_wrap_s(env, thisVar, reinterpret_cast<void *>(obj.get()), Destructor, nullptr, &TYPE_TAG, nullptr);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("AdvancedMeasureFormatAddon::Constructor: Wrap AdvancedMeasureFormatAddon failed.");
        return nullptr;
    }
    if (!obj->InitContext(env, intlNumberFormatAddon->GetNumberFormat(), options)) {
        HILOG_ERROR_I18N("AdvancedMeasureFormatAddon::Constructor: Init context failed.");
        return nullptr;
    }
    obj.release();
    return thisVar;
}

bool AdvancedMeasureFormatAddon::InitContext(napi_env env, std::shared_ptr<NumberFormat> numberFormat,
    AdvancedMeasureFormatOptions options)
{
    if (numberFormat == nullptr) {
        HILOG_ERROR_I18N("AdvancedMeasureFormatAddon::InitContext: numberFormat is nullptr.");
        return false;
    }
    std::map<std::string, std::string> numberFormatOptions;
    numberFormat->GetResolvedOptions(numberFormatOptions, true);
    auto iter = numberFormatOptions.find("locale");
    if (iter == numberFormatOptions.end()) {
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, "numberFormat", "Intl.NumberFormat", true);
        return false;
    }
    auto nativeNumberFormat =
        std::make_shared<NumberFormat>(std::vector<std::string>{ iter->second }, numberFormatOptions);
    advancedMeasureFormat_ = std::make_shared<AdvancedMeasureFormat>(nativeNumberFormat, options);
    return advancedMeasureFormat_ != nullptr;
}

AdvancedMeasureFormatOptions AdvancedMeasureFormatAddon::ParseOptions(napi_env env, napi_value object)
{
    AdvancedMeasureFormatOptions options;
    if (object == nullptr) {
        HILOG_ERROR_I18N("AdvancedMeasureFormatAddon::ParseOptions: object is nullptr.");
        return options;
    }

    napi_valuetype type = napi_undefined;
    napi_status status = napi_typeof(env, object, &type);
    if (status != napi_ok && type != napi_object) {
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, "options", "AdvancedMeasureFormatOptions", true);
        return options;
    }

    int32_t errCode = 0;
    napi_value unitUsage = JSUtils::GetNapiPropertyFormObject(env, object, "unitUsage", errCode);
    if (errCode != 0) {
        HILOG_ERROR_I18N("AdvancedMeasureFormatAddon::ParseOptions: Get property failed.");
        return options;
    }

    int32_t unitUsageValue = 0;
    status = napi_get_value_int32(env, unitUsage, &unitUsageValue);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("AdvancedMeasureFormatAddon::ParseOptions: get unitUsageValue failed.");
        return options;
    }
    options.unitUsage = static_cast<UnitUsage>(unitUsageValue);
    return options;
}

napi_value AdvancedMeasureFormatAddon::Format(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = { 0 };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("AdvancedMeasureFormatAddon::Format: Get parameter info failed.");
        return VariableConvertor::CreateString(env, "");
    } else if (argc < 1) {
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, "num", "", true);
        return VariableConvertor::CreateString(env, "");
    }

    AdvancedMeasureFormatAddon *obj = nullptr;
    status = napi_unwrap_s(env, thisVar, &TYPE_TAG, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->advancedMeasureFormat_) {
        HILOG_ERROR_I18N("AdvancedMeasureFormatAddon::Format: Unwrap failed.");
        return VariableConvertor::CreateString(env, "");
    }

    napi_valuetype type = napi_undefined;
    status = napi_typeof(env, argv[0], &type);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("AdvancedMeasureFormatAddon::Format: Get type failed.");
        return VariableConvertor::CreateString(env, "");
    }

    std::string formatResult;
    if (type == napi_valuetype::napi_number) {
        formatResult = FormatNumber(env, argv[0], obj->advancedMeasureFormat_);
    }
    return VariableConvertor::CreateString(env, formatResult);
}

std::string AdvancedMeasureFormatAddon::FormatNumber(napi_env env, napi_value value,
    std::shared_ptr<AdvancedMeasureFormat> advancedMeasureFormat)
{
    double number = 0;
    napi_status status = napi_get_value_double(env, value, &number);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("AdvancedMeasureFormatAddon::FormatNumber: Get double failed.");
        return "";
    }
    return advancedMeasureFormat->FormatNumber(number);
}
} // namespace I18n
} // namespace Global
} // namespace OHOS