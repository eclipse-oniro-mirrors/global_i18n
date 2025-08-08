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

#include "number_format_addon.h"

#include "i18n_hilog.h"
#include "js_utils.h"

namespace OHOS {
namespace Global {
namespace I18n {
NumberFormatAddon::NumberFormatAddon()
{
}

NumberFormatAddon::~NumberFormatAddon()
{
}

void NumberFormatAddon::Destructor(napi_env env, void *nativeObject, void *hint)
{
    if (!nativeObject) {
        return;
    }
    delete reinterpret_cast<NumberFormatAddon *>(nativeObject);
    nativeObject = nullptr;
}

napi_value NumberFormatAddon::InitNumberFormat(napi_env env, napi_value exports)
{
    napi_status status = napi_ok;
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("format", FormatNumber),
        DECLARE_NAPI_FUNCTION("formatRange", FormatRangeNumber),
        DECLARE_NAPI_FUNCTION("resolvedOptions", GetNumberResolvedOptions)
    };

    napi_value constructor = nullptr;
    status = napi_define_class(env, "NumberFormat", NAPI_AUTO_LENGTH, NumberFormatConstructor, nullptr,
        sizeof(properties) / sizeof(napi_property_descriptor), properties, &constructor);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("Define class failed when InitNumberFormat");
        return nullptr;
    }

    status = napi_set_named_property(env, exports, "NumberFormat", constructor);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("Set property failed when InitNumberFormat");
        return nullptr;
    }
    return exports;
}

napi_value NumberFormatAddon::NumberFormatConstructor(napi_env env, napi_callback_info info)
{
    size_t argc = 2;
    napi_value argv[2] = { nullptr };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        return nullptr;
    }
    std::vector<std::string> localeTags;
    if (argc > 0) {
        napi_valuetype valueType = napi_valuetype::napi_undefined;
        napi_typeof(env, argv[0], &valueType);
        bool isArray = false;
        napi_is_array(env, argv[0], &isArray);

        if (valueType == napi_valuetype::napi_string) {
            JSUtils::GetLocaleTags(env, argv[0], localeTags);
        } else if (isArray) {
            uint32_t arrayLength = 0;
            napi_get_array_length(env, argv[0], &arrayLength);
            napi_value element = nullptr;
            for (uint32_t i = 0; i < arrayLength; i++) {
                napi_get_element(env, argv[0], i, &element);
                JSUtils::GetLocaleTags(env, element, localeTags);
            }
        }
    }
    std::map<std::string, std::string> map = {};
    if (argc > 1) {
        JSUtils::GetNumberOptionValues(env, argv[1], map);
    }
    std::unique_ptr<NumberFormatAddon> obj = nullptr;
    obj = std::make_unique<NumberFormatAddon>();
    status =
        napi_wrap(env, thisVar, reinterpret_cast<void *>(obj.get()), NumberFormatAddon::Destructor, nullptr, nullptr);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("NumberFormatConstructor: Wrap NumberFormatAddon failed");
        return nullptr;
    }
    if (!obj->InitNumberFormatContext(env, info, localeTags, map)) {
        HILOG_ERROR_I18N("Init NumberFormat failed");
        return nullptr;
    }
    obj.release();
    return thisVar;
}

bool NumberFormatAddon::InitNumberFormatContext(napi_env env, napi_callback_info info,
    std::vector<std::string> localeTags, std::map<std::string, std::string> &map)
{
    napi_value global = nullptr;
    napi_status status = napi_get_global(env, &global);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("InitNumberFormatContext: Get global failed");
        return false;
    }
    numberfmt_ = std::make_shared<NumberFormat>(localeTags, map);

    return numberfmt_ != nullptr;
}

napi_value NumberFormatAddon::FormatNumber(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = { 0 };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    double number = 0;
    napi_get_value_double(env, argv[0], &number);
    NumberFormatAddon *obj = nullptr;
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->numberfmt_) {
        HILOG_ERROR_I18N("FormatNumber: Get NumberFormat object failed");
        return nullptr;
    }
    std::string value = obj->numberfmt_->Format(number);
    napi_value result = nullptr;
    status = napi_create_string_utf8(env, value.c_str(), NAPI_AUTO_LENGTH, &result);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("FormatNumber: Create format string failed");
        return nullptr;
    }
    return result;
}

napi_value NumberFormatAddon::FormatRangeNumber(napi_env env, napi_callback_info info)
{
    size_t argc = 2;
    napi_value argv[2] = { 0 };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("FormatRangeNumber: Get parameter info failed");
        return JSUtils::CreateEmptyString(env);
    }
    if (argc != 2) { // 2 is parameter count
        HILOG_ERROR_I18N("FormatRangeNumber: Insufficient parameters");
        return JSUtils::CreateEmptyString(env);
    }
    double start = 0;
    double end = 0;
    status = napi_get_value_double(env, argv[0], &start);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("FormatRangeNumber: Get first param value failed");
        return JSUtils::CreateEmptyString(env);
    }
    status = napi_get_value_double(env, argv[1], &end);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("FormatRangeNumber: Get second param value failed");
        return JSUtils::CreateEmptyString(env);
    }
    NumberFormatAddon *obj = nullptr;
    status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->numberfmt_) {
        HILOG_ERROR_I18N("FormatRangeNumber: Get NumberFormat object failed");
        return JSUtils::CreateEmptyString(env);
    }
    std::string value = obj->numberfmt_->FormatRange(start, end);
    napi_value result;
    status = napi_create_string_utf8(env, value.c_str(), NAPI_AUTO_LENGTH, &result);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("create string js variable failed.");
        return JSUtils::CreateEmptyString(env);
    }
    return result;
}

napi_value NumberFormatAddon::GetNumberResolvedOptions(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, &data);

    NumberFormatAddon *obj = nullptr;
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->numberfmt_) {
        HILOG_ERROR_I18N("GetNumberResolvedOptions: Get NumberFormat object failed");
        return nullptr;
    }
    napi_value result = nullptr;
    napi_create_object(env, &result);
    std::map<std::string, std::string> options = {};
    obj->numberfmt_->GetResolvedOptions(options);
    JSUtils::SetOptionProperties(env, result, options, "locale");
    JSUtils::SetOptionProperties(env, result, options, "currency");
    JSUtils::SetOptionProperties(env, result, options, "currencySign");
    JSUtils::SetOptionProperties(env, result, options, "currencyDisplay");
    JSUtils::SetOptionProperties(env, result, options, "unit");
    JSUtils::SetOptionProperties(env, result, options, "unitDisplay");
    JSUtils::SetOptionProperties(env, result, options, "signDisplay");
    JSUtils::SetOptionProperties(env, result, options, "compactDisplay");
    JSUtils::SetOptionProperties(env, result, options, "notation");
    JSUtils::SetOptionProperties(env, result, options, "style");
    JSUtils::SetOptionProperties(env, result, options, "numberingSystem");
    JSUtils::SetOptionProperties(env, result, options, "unitUsage");
    JSUtils::SetBooleanOptionProperties(env, result, options, "useGrouping");
    JSUtils::SetIntegerOptionProperties(env, result, options, "minimumIntegerDigits");
    JSUtils::SetIntegerOptionProperties(env, result, options, "minimumFractionDigits");
    JSUtils::SetIntegerOptionProperties(env, result, options, "maximumFractionDigits");
    JSUtils::SetIntegerOptionProperties(env, result, options, "minimumSignificantDigits");
    JSUtils::SetIntegerOptionProperties(env, result, options, "maximumSignificantDigits");
    JSUtils::SetOptionProperties(env, result, options, "localeMatcher");
    return result;
}

std::shared_ptr<NumberFormat> NumberFormatAddon::CopyNumberFormat()
{
    return numberfmt_;
}
} // namespace I18n
} // namespace Global
} // namespace OHOS