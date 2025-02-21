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
#include "hilog/log.h"
#include "variable_convertor.h"

namespace OHOS {
namespace Global {
namespace I18n {
static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, 0xD001E00, "VariableConvertor" };
using namespace OHOS::HiviewDFX;

bool VariableConvertor::CheckNapiValueType(napi_env env, napi_value value)
{
    if (value != nullptr) {
        napi_valuetype valueType = napi_valuetype::napi_undefined;
        napi_typeof(env, value, &valueType);
        if (valueType != napi_valuetype::napi_undefined && valueType != napi_valuetype::napi_null) {
            return true;
        }
    }
    return false;
}

void VariableConvertor::GetOptionValue(napi_env env, napi_value options, const std::string &optionName,
    std::string &value)
{
    napi_value optionValue = nullptr;
    napi_valuetype type = napi_undefined;
    napi_status status = napi_typeof(env, options, &type);
    if (status != napi_ok && type != napi_object) {
        HiLog::Error(LABEL, "Get option failed, option is not an object");
        return;
    }
    bool hasProperty = false;
    napi_status propStatus = napi_has_named_property(env, options, optionName.c_str(), &hasProperty);
    if (propStatus == napi_ok && hasProperty) {
        status = napi_get_named_property(env, options, optionName.c_str(), &optionValue);
        if (status == napi_ok) {
            size_t len;
            napi_get_value_string_utf8(env, optionValue, nullptr, 0, &len);
            std::vector<char> optionBuf(len + 1);
            status = napi_get_value_string_utf8(env, optionValue, optionBuf.data(), len + 1, &len);
            if (status != napi_ok) {
                HiLog::Error(LABEL, "Failed to get string item");
                return;
            }
            value = optionBuf.data();
        }
    }
}

bool VariableConvertor::GetBoolOptionValue(napi_env env, napi_value &options, const std::string &optionName,
    bool &boolVal)
{
    napi_valuetype type = napi_undefined;
    napi_status status = napi_typeof(env, options, &type);
    if (status != napi_ok && type != napi_object) {
        HiLog::Error(LABEL, "option is not an object");
        ErrorUtil::NapiThrow(env, I18N_NOT_VALID, true);
        return false;
    }
    bool hasProperty = false;
    status = napi_has_named_property(env, options, optionName.c_str(), &hasProperty);
    if (status != napi_ok || !hasProperty) {
        HiLog::Info(LABEL, "option don't have property %{public}s", optionName.c_str());
        return false;
    }
    napi_value optionValue = nullptr;
    status = napi_get_named_property(env, options, optionName.c_str(), &optionValue);
    if (status != napi_ok) {
        HiLog::Info(LABEL, "get option %{public}s failed", optionName.c_str());
        return false;
    }
    napi_get_value_bool(env, optionValue, &boolVal);
    return true;
}

std::string VariableConvertor::GetString(napi_env &env, napi_value &value, int32_t &code)
{
    size_t len = 0;
    napi_status status = napi_get_value_string_utf8(env, value, nullptr, 0, &len);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Get string failed");
        code = 1;
        return "";
    }
    std::vector<char> buf(len + 1);
    status = napi_get_value_string_utf8(env, value, buf.data(), len + 1, &len);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Create string failed");
        code = 1;
        return "";
    }
    return buf.data();
}

bool VariableConvertor::GetStringArrayFromJsParam(napi_env env, napi_value &jsArray, std::vector<std::string> &strArray)
{
    if (jsArray == nullptr) {
        HiLog::Error(LABEL, "js string array param not found.");
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, true);
        return false;
    }
    bool isArray = false;
    napi_status status = napi_is_array(env, jsArray, &isArray);
    if (status != napi_ok || !isArray) {
        HiLog::Error(LABEL, "js string array is not an Array.");
        ErrorUtil::NapiThrow(env, I18N_NOT_VALID, true);
        return false;
    }
    uint32_t arrayLength = 0;
    napi_get_array_length(env, jsArray, &arrayLength);
    napi_value element = nullptr;
    int32_t code = 0;
    for (uint32_t i = 0; i < arrayLength; ++i) {
        napi_get_element(env, jsArray, i, &element);
        std::string str = GetString(env, element, code);
        if (code != 0) {
            HiLog::Error(LABEL, "can't get string from js array param.");
            ErrorUtil::NapiThrow(env, I18N_NOT_VALID, true);
            return false;
        }
        strArray.push_back(str);
    }
    return true;
}

napi_value VariableConvertor::CreateString(napi_env env, const std::string &str)
{
    napi_value result;
    napi_status status = napi_create_string_utf8(env, str.c_str(), NAPI_AUTO_LENGTH, &result);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "create string js variable failed.");
        return nullptr;
    }
    return result;
}

napi_value VariableConvertor::CreateNumber(napi_env env, const int32_t &num)
{
    napi_value result;
    napi_status status = napi_create_int32(env, num, &result);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "create number js variable failed.");
        return nullptr;
    }
    return result;
}
} // namespace I18n
} // namespace Global
} // namespace OHOS