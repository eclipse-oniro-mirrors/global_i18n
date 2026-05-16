/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include <string>
#include <unordered_map>

#include "i18n_hilog.h"

namespace OHOS {
namespace Global {
namespace I18n {
static const std::unordered_map<int32_t, std::string> ErrorCodeToMsg {
    {I18N_NO_PERMISSION,
        "Permission verification failed. The application does not have the permission required to call the API."},
    {I18N_NOT_SYSTEM_APP,
        "Permission verification failed. A non-system application calls a system API."},
    {I18N_NOT_VALID, "Invalid parameter"},
    {I18N_NOT_VALID_NEW, "Invalid parameter"},
    {I18N_NOT_FOUND, "Parameter error"},
    {I18N_OPTION_NOT_VALID, "Invalid option name"}
};

void ErrorUtil::NapiThrow(napi_env env, int32_t errCode, const std::string& valueName,
    const std::string& valueContent, bool throwError)
{
    if (!throwError) {
        return;
    }
    napi_value code = nullptr;
    napi_status status = napi_create_string_latin1(env, std::to_string(errCode).c_str(), NAPI_AUTO_LENGTH, &code);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("ErrorUtil::NapiThrow: create string %{public}d failed", errCode);
        return;
    }
    napi_value message = nullptr;
    auto iter = ErrorCodeToMsg.find(errCode);
    std::string errMsg = iter != ErrorCodeToMsg.end() ? iter->second : "";
    std::string allErrMsg;

    if (errCode == I18N_NO_PERMISSION || errCode == I18N_NOT_SYSTEM_APP) {
        allErrMsg = errMsg;
    } else if (errCode == I18N_NOT_VALID || errCode == I18N_NOT_VALID_NEW) {
        allErrMsg = errMsg + ", the " + valueName + " must be " + valueContent + ".";
    } else if (valueContent.length() == 0) {
        allErrMsg = errMsg + ", the " + valueName + " cannot be empty.";
    } else {
        allErrMsg = errMsg + ", the type of " + valueName + " must be " + valueContent + ".";
    }

    status = napi_create_string_latin1(env, allErrMsg.c_str(), NAPI_AUTO_LENGTH, &message);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("ErrorUtil::NapiThrow: create string %{public}s failed", allErrMsg.c_str());
        return;
    }
    napi_value error = nullptr;
    status = napi_create_error(env, code, message, &error);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("ErrorUtil::NapiThrow: create error failed");
        return;
    }
    napi_throw(env, error);
}

void ErrorUtil::NapiNotFoundError(napi_env env, int32_t errCode, const std::string& valueName,
    bool throwError)
{
    NapiThrow(env, errCode, valueName, "", throwError);
}

void ErrorUtil::NapiThrowUndefined(napi_env env, const std::string& value)
{
    napi_value code = nullptr;
    napi_status status = napi_create_string_latin1(env, "undefined", NAPI_AUTO_LENGTH, &code);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("ErrorUtil::NapiThrowUndefined: create string undefined failed");
        return;
    }
    napi_value message = nullptr;
    status = napi_create_string_latin1(env, value.c_str(), NAPI_AUTO_LENGTH, &message);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("ErrorUtil::NapiThrowUndefined: create string %{public}s failed", value.c_str());
        return;
    }
    napi_value error = nullptr;
    status = napi_create_error(env, code, message, &error);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("ErrorUtil::NapiThrowUndefined: create error failed");
        return;
    }
    napi_throw(env, error);
}
} // namespace I18n
} // namespace Global
} // namespace OHOS