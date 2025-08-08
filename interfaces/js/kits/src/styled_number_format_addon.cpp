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

#include "styled_number_format_addon.h"

#include "error_util.h"
#include "i18n_hilog.h"
#include "variable_convertor.h"

namespace OHOS {
namespace Global {
namespace I18n {
StyledNumberFormatAddon::StyledNumberFormatAddon()
{
}

StyledNumberFormatAddon::~StyledNumberFormatAddon()
{
    for (auto iter = styledNumberFormatOptions_.begin(); iter != styledNumberFormatOptions_.end(); ++iter) {
        napi_delete_reference(env_, iter->second);
    }
}

void StyledNumberFormatAddon::Destructor(napi_env env, void *nativeObject, void *hint)
{
    if (!nativeObject) {
        return;
    }
    delete reinterpret_cast<StyledNumberFormatAddon *>(nativeObject);
    nativeObject = nullptr;
}

napi_value StyledNumberFormatAddon::InitStyledNumberFormat(napi_env env, napi_value exports)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("format", Format)
    };
    napi_value styledNumberFormatConstructor = nullptr;
    napi_status status = napi_define_class(env, "StyledNumberFormat", NAPI_AUTO_LENGTH, constructor, nullptr,
        sizeof(properties) / sizeof(napi_property_descriptor), properties, &styledNumberFormatConstructor);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("Failed to define class StyledNumberFormat at Init.");
        return nullptr;
    }
    status = napi_set_named_property(env, exports, "StyledNumberFormat", styledNumberFormatConstructor);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("Set property failed when InitStyledNumberFormat");
        return nullptr;
    }
    return exports;
}

napi_value StyledNumberFormatAddon::constructor(napi_env env, napi_callback_info info)
{
    size_t argc = 2;
    napi_value argv[2] = { nullptr };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("StyledNumberFormatAddon::constructor get callback info error");
        return nullptr;
    }

    if (argc < 1) {
        ErrorUtil::NapiNotFoundError(env, I18N_NOT_FOUND, "numberFormat", true);
        return nullptr;
    }

    NumberFormatAddon *numberFormatAddon = nullptr;
    SimpleNumberFormatAddon *simpleNumberFormatAddon = nullptr;

    GetNumberFmtOrSimpleNumberFmt(env, argv[0], &numberFormatAddon, &simpleNumberFormatAddon);

    if (!numberFormatAddon && !simpleNumberFormatAddon) {
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, "numberFormat", "intl.NumberFormat or SimpleNumberFormat", true);
        return nullptr;
    }

    std::unique_ptr<StyledNumberFormatAddon> obj = std::make_unique<StyledNumberFormatAddon>();
    if (!obj) {
        HILOG_ERROR_I18N("StyledNumberFormatAddon::constructor make unique ptr error");
        return nullptr;
    }
    status = napi_wrap(env, thisVar, reinterpret_cast<void *>(obj.get()), Destructor, nullptr, nullptr);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("StyledNumberFormatAddon::constructor napi_wrap error");
        return nullptr;
    }
    if (!obj->InitStyledNumberFormatContent(env, numberFormatAddon, simpleNumberFormatAddon, argv[1])) {
        return nullptr;
    }
    obj.release();
    return thisVar;
}

napi_value StyledNumberFormatAddon::Format(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = { 0 };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("StyledNumberFormatAddon::Format: Get cb info failed.");
        return VariableConvertor::CreateString(env, "");
    } else if (argc < 1) {
        ErrorUtil::NapiNotFoundError(env, I18N_NOT_FOUND, "value", true);
        return VariableConvertor::CreateString(env, "");
    }

    napi_valuetype valueType = napi_valuetype::napi_undefined;
    status = napi_typeof(env, argv[0], &valueType);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("StyledNumberFormatAddon::Format: Failed to get type of argv[0].");
        return VariableConvertor::CreateString(env, "");
    } else if (valueType != napi_valuetype::napi_number) {
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, "value", "number", true);
        return VariableConvertor::CreateString(env, "");
    }

    double number = 0;
    napi_get_value_double(env, argv[0], &number);
    StyledNumberFormatAddon *obj = nullptr;
    status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->styledNumberFormat_) {
        HILOG_ERROR_I18N("Format: Get StyledNumberFormat object failed");
        return VariableConvertor::CreateString(env, "");
    }

    std::string formattedNumber = obj->styledNumberFormat_->Format(number);
    std::vector<StyledNumberFormat::NumberPart> numberParts = obj->styledNumberFormat_->ParseToParts(number);

    napi_value styleOption = CreateStyleOptions(env, numberParts, obj->styledNumberFormatOptions_);
    
    napi_value styledString = CreateStyledString(env, formattedNumber, styleOption);
    return styledString;
}

bool StyledNumberFormatAddon::InitStyledNumberFormatContent(napi_env env, NumberFormatAddon* numberFormatAddon,
    SimpleNumberFormatAddon* simpleNumberFormatAddon, napi_value options)
{
    if (numberFormatAddon) {
        styledNumberFormat_ =
            std::make_unique<StyledNumberFormat>(true, numberFormatAddon->CopyNumberFormat(), nullptr);
    } else if (simpleNumberFormatAddon) {
        styledNumberFormat_ =
            std::make_unique<StyledNumberFormat>(false, nullptr, simpleNumberFormatAddon->CopySimpleNumberFormat());
    }

    if (options) {
        SetTextStyle(env, options, "integer");
        SetTextStyle(env, options, "decimal");
        SetTextStyle(env, options, "fraction");
        SetTextStyle(env, options, "unit");
    }

    env_ = env;
    return styledNumberFormat_ != nullptr;
}

void StyledNumberFormatAddon::SetTextStyle(napi_env env, napi_value options, const std::string &optionName)
{
    napi_value optionValue = nullptr;
    napi_valuetype type = napi_undefined;
    napi_status status = napi_typeof(env, options, &type);
    if (status != napi_ok && type != napi_object) {
        HILOG_ERROR_I18N("setTextStyle: option is not an object");
        return;
    }
    bool hasProperty = false;
    status = napi_has_named_property(env, options, optionName.c_str(), &hasProperty);
    if (status != napi_ok || !hasProperty) {
        HILOG_ERROR_I18N("setTextStyle: Has not named %{public}s property", optionName.c_str());
        return;
    }
    status = napi_get_named_property(env, options, optionName.c_str(), &optionValue);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("setTextStyle: Get named property for %{public}s failed.", optionName.c_str());
        return;
    }

    styledNumberFormatOptions_[optionName] = nullptr;
    if (!optionValue) {
        HILOG_ERROR_I18N("setTextStyle: %{public}s is nullptr.", optionName.c_str());
        return;
    }
    status = napi_create_reference(env, optionValue, 1, &styledNumberFormatOptions_[optionName]);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("setTextStyle: create reference for %{public}s failed.", optionName.c_str());
        styledNumberFormatOptions_[optionName] = nullptr;
        return;
    }
}

napi_value StyledNumberFormatAddon::CreateStyledString(napi_env env, const std::string &formattedNumber,
    napi_value styleOption)
{
    size_t argc = 2;    // StyledString need 2 args
    napi_value argv[2] = {nullptr};
    argv[0] = VariableConvertor::CreateString(env, formattedNumber);
    argv[1] = styleOption;

    napi_value global = nullptr;
    napi_status status = napi_get_global(env, &global);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("CreateStyledString: get global object failed");
        return nullptr;
    }
    napi_value constructor = nullptr;
    status = napi_get_named_property(env, global, "StyledString", &constructor);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("CreateStyledString: get StyledString failed");
        return nullptr;
    }

    napi_value styleString = nullptr;
    status = napi_new_instance(env, constructor, argc, argv, &styleString);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("CreateStyledString: napi_new_instance failed");
        return nullptr;
    }
    return styleString;
}

napi_value StyledNumberFormatAddon::CreateStyleOptions(napi_env env,
    const std::vector<StyledNumberFormat::NumberPart> &numberParts,
    const std::unordered_map<std::string, napi_ref> &styledNumberFormatOptions)
{
    napi_value result = nullptr;
    napi_status status = napi_create_array(env, &result);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("CreateStyleOption: create array failed");
        return nullptr;
    }

    unsigned int index = 0;
    for (size_t i = 0; i < numberParts.size(); ++i) {
        auto iter = styledNumberFormatOptions.find(numberParts[i].part_name);
        if (iter == styledNumberFormatOptions.end()) {
            continue;
        }
        napi_ref textStyleRef = iter->second;
        napi_value option = CreateStyleOption(env, numberParts[i], textStyleRef);
        status = napi_set_element(env, result, index, option);
        if (status != napi_ok) {
            HILOG_ERROR_I18N("CreateStyleoption: set array %{public}u failed", index);
            return nullptr;
        }
        index += 1;
    }
    return result;
}

napi_value StyledNumberFormatAddon::CreateStyleOption(napi_env env,
    StyledNumberFormat::NumberPart numberPart, napi_ref textStyleRef)
{
    if (!textStyleRef) {
        HILOG_ERROR_I18N("CreateStyleoption: options do not have %{public}s",
            numberPart.part_name.c_str());
        return nullptr;
    }
    napi_value textStyle = nullptr;
    napi_status status = napi_get_reference_value(env, textStyleRef, &textStyle);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("CreateStyleoption: Failed to create reference at textStyle %{public}s",
            numberPart.part_name.c_str());
        return nullptr;
    }
    napi_value option = nullptr;
    status = napi_create_object(env, &option);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("CreateStyleoption: create object failed");
        return nullptr;
    }
    napi_value start = nullptr;
    status = napi_create_int32(env, numberPart.start, &start);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("CreateStyleoption: create int32 failed");
        return nullptr;
    }
    napi_value length = nullptr;
    status = napi_create_int32(env, numberPart.length, &length);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("CreateStyleoption: create int32 failed");
        return nullptr;
    }
    napi_value styledKey = nullptr;
    status = napi_create_int32(env, 0, &styledKey);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("CreateStyleoption: create int32 failed");
        return nullptr;
    }
    SetNamedProperty(env, option, "start", start);
    SetNamedProperty(env, option, "length", length);
    SetNamedProperty(env, option, "styledKey", styledKey);
    SetNamedProperty(env, option, "styledValue", textStyle);
    return option;
}

void StyledNumberFormatAddon::SetNamedProperty(napi_env env, napi_value &option, const std::string& name,
    napi_value &property)
{
    napi_status status = napi_set_named_property(env, option, name.c_str(), property);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("CreateStyleoption: set property %{public}s failed", name.c_str());
    }
}

void StyledNumberFormatAddon::GetNumberFmtOrSimpleNumberFmt(napi_env env, napi_value arg,
    NumberFormatAddon **numberFormatAddon, SimpleNumberFormatAddon **simpleNumberFormatAddon)
{
    napi_valuetype valueType = napi_valuetype::napi_undefined;
    napi_status status = napi_typeof(env, arg, &valueType);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("StyledNumberFormatAddon::constructor Failed to get type.");
        return;
    } else if (valueType != napi_valuetype::napi_object) {
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, "intl.NumberFormat or SimpleNumberFormat", "", true);
        return;
    }

    napi_value numFmtConstructor = nullptr;
    napi_value intlModule;
    status = napi_load_module(env, "@ohos.intl", &intlModule);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("StyledNumberFormatAddon::constructor Failed to get intl module.");
        return;
    }
    status = napi_get_named_property(env, intlModule, "NumberFormat", &numFmtConstructor);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("StyledNumberFormatAddon::constructo: get NumberFormat  constructor failed");
        return;
    }

    bool isNumberFormat = true;
    status = napi_instanceof(env, arg, numFmtConstructor, &isNumberFormat);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("StyledNumberFormatAddon::constructor Failed to get instance of argv 0.");
        return;
    }
    if (isNumberFormat) {
        status = napi_unwrap(env, arg, reinterpret_cast<void **>(numberFormatAddon));
        if (status != napi_ok) {
            *numberFormatAddon = nullptr;
        }
    } else {
        status = napi_unwrap(env, arg, reinterpret_cast<void **>(simpleNumberFormatAddon));
        if (status != napi_ok) {
            *simpleNumberFormatAddon = nullptr;
        }
    }
}
} // namespace I18n
} // namespace Global
} // namespace OHOS