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

#include "styled_date_time_format_addon.h"

#include "error_util.h"
#include "i18n_hilog.h"
#include "js_lifecycle_managers.h"
#include "js_utils.h"
#include "variable_convertor.h"

namespace OHOS {
namespace Global {
namespace I18n {
StyledDateTimeFormatAddon::StyledDateTimeFormatAddon()
{
}

StyledDateTimeFormatAddon::~StyledDateTimeFormatAddon()
{
    for (auto iter = styledFormatOptions_.begin(); iter != styledFormatOptions_.end(); ++iter) {
        napi_delete_reference(env_, iter->second);
    }
}

void StyledDateTimeFormatAddon::Destructor(napi_env env, void *nativeObject, void *hint)
{
    if (!nativeObject) {
        return;
    }
    delete reinterpret_cast<StyledDateTimeFormatAddon *>(nativeObject);
    nativeObject = nullptr;
}

napi_value StyledDateTimeFormatAddon::InitStyledDateTimeFormat(napi_env env, napi_value exports)
{
    HandleScope scope(env, "InitStyledDateTimeFormat");
    if (!scope.IsOpen()) {
        return nullptr;
    }

    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("format", Format)
    };
    napi_value styledConstructor = nullptr;
    napi_status status = napi_define_class(env, "StyledDateTimeFormat", NAPI_AUTO_LENGTH, constructor, nullptr,
        sizeof(properties) / sizeof(napi_property_descriptor), properties, &styledConstructor);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("InitStyledDateTimeFormat: Define class StyledDateTimeFormat failed.");
        return nullptr;
    }
    status = napi_set_named_property(env, exports, "StyledDateTimeFormat", styledConstructor);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("InitStyledDateTimeFormat: Set property StyledDateTimeFormat failed.");
        return nullptr;
    }
    return exports;
}

napi_value StyledDateTimeFormatAddon::constructor(napi_env env, napi_callback_info info)
{
    size_t argc = 2;
    napi_value argv[2] = { nullptr };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("StyledDateTimeFormatAddon::constructor: Get callback info error.");
        return nullptr;
    } else if (argc < 1) {
        ErrorUtil::NapiNotFoundError(env, I18N_NOT_FOUND, "dateTimeFormat", true);
        return nullptr;
    }

    IntlDateTimeFormatAddon* intlFormat = nullptr;
    SimpleDateTimeFormatAddon* simpleFormat = nullptr;
    ParseDateTimeFormat(env, argv[0], &intlFormat, &simpleFormat);
    if (!intlFormat && !simpleFormat) {
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, "dateTimeFormat",
            "SimpleDateTimeFormat or Intl.DateTimeFormat", true);
        return nullptr;
    }

    std::unique_ptr<StyledDateTimeFormatAddon> obj = std::make_unique<StyledDateTimeFormatAddon>();
    status = napi_wrap_s(env, thisVar, reinterpret_cast<void *>(obj.get()), Destructor, nullptr, &TYPE_TAG, nullptr);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("StyledDateTimeFormatAddon::constructor: napi_wrap error.");
        return nullptr;
    }
    if (!obj->InitStyledFormatContext(env, argv[1], intlFormat, simpleFormat)) {
        return nullptr;
    }
    obj.release();
    return thisVar;
}

void StyledDateTimeFormatAddon::ParseDateTimeFormat(napi_env env, napi_value value,
    IntlDateTimeFormatAddon** intlFormat, SimpleDateTimeFormatAddon** simpleFormat)
{
    napi_valuetype valueType = napi_valuetype::napi_undefined;
    napi_status status = napi_typeof(env, value, &valueType);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("StyledDateTimeFormatAddon::ParseDateTimeFormat: Failed to get type.");
        return;
    } else if (valueType != napi_valuetype::napi_object) {
        HILOG_ERROR_I18N("StyledDateTimeFormatAddon::ParseDateTimeFormat: Check value failed.");
        return;
    }

    if (IntlDateTimeFormatAddon::IsIntlDateTimeFormat(env, value)) {
        status = IntlDateTimeFormatAddon::UnwrapIntlDateTimeFormat(env, value, intlFormat);
        if (status != napi_ok && intlFormat != nullptr) {
            *intlFormat = nullptr;
            HILOG_ERROR_I18N("StyledDateTimeFormatAddon::ParseDateTimeFormat: intlFormat unwrap failed.");
        }
    } else if (SimpleDateTimeFormatAddon::IsSimpleDateTimeFormat(env, value)) {
        status = SimpleDateTimeFormatAddon::UnwrapSimpleDateTimeFormat(env, value, simpleFormat);
        if (status != napi_ok && simpleFormat != nullptr) {
            *simpleFormat = nullptr;
            HILOG_ERROR_I18N("StyledDateTimeFormatAddon::ParseDateTimeFormat: simpleFormat unwrap failed.");
        }
    }
}

bool StyledDateTimeFormatAddon::InitStyledFormatContext(napi_env env, napi_value options,
    IntlDateTimeFormatAddon* intlFormat, SimpleDateTimeFormatAddon* simpleFormat)
{
    if (intlFormat) {
        styledDateTimeFormat_ = std::make_unique<StyledDateTimeFormat>(nullptr, intlFormat->GetDateTimeFormat());
    } else if (simpleFormat) {
        styledDateTimeFormat_ = std::make_unique<StyledDateTimeFormat>(simpleFormat->GetDateTimeFormat(), nullptr);
    }

    ParseOptions(env, options);

    env_ = env;
    return styledDateTimeFormat_ != nullptr;
}

void StyledDateTimeFormatAddon::ParseOptions(napi_env env, napi_value options)
{
    napi_valuetype type = napi_undefined;
    napi_status status = napi_typeof(env, options, &type);
    if (status != napi_ok && type != napi_object) {
        HILOG_ERROR_I18N("StyledDateTimeFormatAddon::ParseOptions: option is not an object");
        return;
    }
    std::unordered_set<std::string> supportedTypes = StyledDateTimeFormat::GetSupportedTypes();
    for (const auto& type : supportedTypes) {
        ParseOption(env, options, type);
    }
}

void StyledDateTimeFormatAddon::ParseOption(napi_env env, napi_value options, const std::string& optionName)
{
    bool hasProperty = false;
    napi_status status = napi_has_named_property(env, options, optionName.c_str(), &hasProperty);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("StyledDateTimeFormatAddon::ParseOption: Call napi_has_named_property failed,"
            "optionName %{public}s", optionName.c_str());
        return;
    }
    if (!hasProperty) {
        return;
    }
    napi_value optionValue = nullptr;
    status = napi_get_named_property(env, options, optionName.c_str(), &optionValue);
    if (status != napi_ok || optionValue == nullptr) {
        HILOG_ERROR_I18N("StyledDateTimeFormatAddon::ParseOption: Get property %{public}s failed.",
            optionName.c_str());
        return;
    }

    napi_ref optionRef = nullptr;
    status = napi_create_reference(env, optionValue, 1, &optionRef);
    if (status != napi_ok || optionRef == nullptr) {
        HILOG_ERROR_I18N("StyledDateTimeFormatAddon::ParseOption: create reference for %{public}s failed.",
            optionName.c_str());
        return;
    }
    styledFormatOptions_[optionName] = optionRef;
}

napi_value StyledDateTimeFormatAddon::Format(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = { nullptr };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("StyledNumberFormatAddon::Format: Get cb info failed.");
        return VariableConvertor::CreateStyledString(env, "", nullptr);
    } else if (argc < 1) {
        ErrorUtil::NapiNotFoundError(env, I18N_NOT_FOUND, "date", true);
        return VariableConvertor::CreateStyledString(env, "", nullptr);
    }
    double milliseconds = 0;
    if (!VariableConvertor::GetDateTime(env, argv[0], milliseconds)) {
        HILOG_ERROR_I18N("SimpleDateTimeFormatAddon::Format: Get date time failed.");
        return VariableConvertor::CreateString(env, "");
    }

    StyledDateTimeFormatAddon *obj = nullptr;
    status = napi_unwrap_s(env, thisVar, &TYPE_TAG, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->styledDateTimeFormat_) {
        HILOG_ERROR_I18N("SimpleDateTimeFormatAddon::Format: Get StyledDateTimeFormat object failed.");
        return VariableConvertor::CreateStyledString(env, "", nullptr);
    }

    auto formatResult = obj->styledDateTimeFormat_->Format(milliseconds);
    napi_value styleOption = CreateStyleOptions(env, formatResult.second, obj->styledFormatOptions_);
    return VariableConvertor::CreateStyledString(env, formatResult.first, styleOption);
}

napi_value StyledDateTimeFormatAddon::CreateStyleOptions(napi_env env, const std::vector<DateTimeFormatPart>& parts,
    const std::unordered_map<std::string, napi_ref>& styledOptions)
{
    napi_value result = nullptr;
    napi_status status = napi_create_array(env, &result);
    if (status != napi_ok || result == nullptr) {
        HILOG_ERROR_I18N("StyledDateTimeFormatAddon::CreateStyleOptions: Create array failed.");
        return nullptr;
    }

    size_t index = 0;
    for (auto& part : parts) {
        std::string partName = part.GetPartName();
        auto iter = styledOptions.find(partName);
        if (iter == styledOptions.end()) {
            continue;
        }
        napi_ref textStyleRef = iter->second;
        napi_value option = CreateStyleOption(env, part, textStyleRef);
        status = napi_set_element(env, result, index, option);
        if (status != napi_ok) {
            HILOG_ERROR_I18N("StyledDateTimeFormatAddon::CreateStyleOptions: Set array %{public}zu failed", index);
            return nullptr;
        }
        index += 1;
    }
    return result;
}

napi_value StyledDateTimeFormatAddon::CreateStyleOption(napi_env env, const DateTimeFormatPart& part,
    napi_ref textStyleRef)
{
    if (!textStyleRef) {
        HILOG_ERROR_I18N("StyledDateTimeFormatAddon::CreateStyleOption: textStyleRef is nullptr.");
        return nullptr;
    }

    napi_value textStyle = nullptr;
    napi_status status = napi_get_reference_value(env, textStyleRef, &textStyle);
    if (status != napi_ok || textStyle == nullptr) {
        HILOG_ERROR_I18N("StyledDateTimeFormatAddon::CreateStyleOption: Get textStyle failed.");
        return nullptr;
    }
    napi_value option = nullptr;
    status = napi_create_object(env, &option);
    if (status != napi_ok || option == nullptr) {
        HILOG_ERROR_I18N("StyledDateTimeFormatAddon::CreateStyleOption: Create option failed.");
        return nullptr;
    }
    napi_value start = nullptr;
    status = napi_create_int32(env, part.GetStart(), &start);
    if (status != napi_ok || start == nullptr) {
        HILOG_ERROR_I18N("StyledDateTimeFormatAddon::CreateStyleOption: Create start failed.");
        return nullptr;
    }
    napi_value length = nullptr;
    status = napi_create_int32(env, part.GetLength(), &length);
    if (status != napi_ok || length == nullptr) {
        HILOG_ERROR_I18N("StyledDateTimeFormatAddon::CreateStyleOption: Create length failed.");
        return nullptr;
    }
    napi_value styledKey = nullptr;
    status = napi_create_int32(env, 0, &styledKey);
    if (status != napi_ok || styledKey == nullptr) {
        HILOG_ERROR_I18N("StyledDateTimeFormatAddon::CreateStyleOption: Create styledKey failed.");
        return nullptr;
    }
    SetTextStyle(env, option, "start", start);
    SetTextStyle(env, option, "length", length);
    SetTextStyle(env, option, "styledKey", styledKey);
    SetTextStyle(env, option, "styledValue", textStyle);
    return option;
}

void StyledDateTimeFormatAddon::SetTextStyle(napi_env env, napi_value &option, const std::string& name,
    napi_value &property)
{
    napi_status status = napi_set_named_property(env, option, name.c_str(), property);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("StyledDateTimeFormatAddon::SetTextStyle: set property %{public}s failed", name.c_str());
    }
}
} // namespace I18n
} // namespace Global
} // namespace OHOS