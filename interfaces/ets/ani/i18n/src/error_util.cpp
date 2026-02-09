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

#include "error_util.h"

#include <unordered_map>
#include "i18n_hilog.h"

using namespace OHOS;
using namespace Global;
using namespace I18n;

static const std::unordered_map<int32_t, std::string> ErrorCodeToMsg {
    {I18N_NO_PERMISSION,
        "Permission verification failed. The application does not have the permission required to call the API."},
    {I18N_NOT_SYSTEM_APP,
        "Permission verification failed. A non-system application calls a system API."},
    {I18N_NOT_VALID, "Invalid parameter"},
    {I18N_NOT_FOUND, "Parameter error"},
    {I18N_OPTION_NOT_VALID, "Invalid option name"}
};

ani_object ErrorUtil::WrapAniError(ani_env *env, const std::string &msg)
{
    if (env == nullptr) {
        HILOG_ERROR_I18N("ErrorUtil::WrapAniError: env is null");
        return nullptr;
    }

    ani_string aniMsg = nullptr;
    ani_status status = ANI_ERROR;
    if ((status = env->String_NewUTF8(msg.c_str(), msg.size(), &aniMsg)) != ANI_OK) {
        HILOG_ERROR_I18N("ErrorUtil::WrapAniError: String_NewUTF8 failed %{public}d", status);
        return nullptr;
    }

    ani_ref undefRef;
    if ((status = env->GetUndefined(&undefRef)) != ANI_OK) {
        HILOG_ERROR_I18N("ErrorUtil::WrapAniError: GetUndefined failed %{public}d", status);
        return nullptr;
    }

    ani_class cls {};
    if ((status = env->FindClass("escompat.Error", &cls)) != ANI_OK) {
        HILOG_ERROR_I18N("ErrorUtil::WrapAniError: FindClass escompat.Error failed %{public}d", status);
        return nullptr;
    }

    ani_method method {};
    if ((status = env->Class_FindMethod(cls, "<ctor>", "C{std.core.String}C{escompat.ErrorOptions}:", &method)) !=
        ANI_OK) {
        HILOG_ERROR_I18N("ErrorUtil::WrapAniError: Class_FindMethod failed %{public}d", status);
        return nullptr;
    }

    ani_object obj = nullptr;
    if ((status = env->Object_New(cls, method, &obj, aniMsg, undefRef)) != ANI_OK) {
        HILOG_ERROR_I18N("ErrorUtil::WrapAniError: Object_New failed %{public}d", status);
        return nullptr;
    }
    return obj;
}

ani_object ErrorUtil::CreateError(ani_env *env, ani_int code, const std::string &msg)
{
    ani_class cls {};
    ani_method method {};
    ani_object obj = nullptr;
    ani_status status = ANI_ERROR;
    if (env == nullptr) {
        HILOG_ERROR_I18N("null env");
        return nullptr;
    }
    if ((status = env->FindClass("@ohos.base.BusinessError", &cls)) != ANI_OK) {
        HILOG_ERROR_I18N("ErrorUtil::CreateError: FindClass failed %{public}d", status);
        return nullptr;
    }
    if ((status = env->Class_FindMethod(cls, "<ctor>", "iC{escompat.Error}:", &method)) != ANI_OK) {
        HILOG_ERROR_I18N("ErrorUtil::CreateError: Class_FindMethod failed %{public}d", status);
        return nullptr;
    }
    ani_object error = WrapAniError(env, msg);
    if (error == nullptr) {
        HILOG_ERROR_I18N("ErrorUtil::CreateError: error is null");
        return nullptr;
    }
    if ((status = env->Object_New(cls, method, &obj, code, error)) != ANI_OK) {
        HILOG_ERROR_I18N("ErrorUtil::CreateError: Object_New failed %{public}d", status);
        return nullptr;
    }
    return obj;
}

void ErrorUtil::AniThrow(ani_env *env, int32_t errCode, const std::string &valueName,
    const std::string &valueContent)
{
    auto iter = ErrorCodeToMsg.find(errCode);
    std::string errMsg = iter != ErrorCodeToMsg.end() ? iter->second : "";
    std::string allErrMsg;

    if (errCode == I18N_NO_PERMISSION || errCode == I18N_NOT_SYSTEM_APP) {
        allErrMsg = errMsg;
    } else if (errCode == I18N_NOT_VALID) {
        allErrMsg = errMsg + ", the " + valueName + " must be " + valueContent + ".";
    } else if (valueContent.length() == 0) {
        allErrMsg = errMsg + ", the " + valueName + " cannot be empty.";
    } else {
        allErrMsg = errMsg + ", the type of " + valueName + " must be " + valueContent + ".";
    }

    ani_object error = CreateError(env, errCode, allErrMsg);
    if (ANI_OK != env->ThrowError(static_cast<ani_error>(error))) {
        HILOG_ERROR_I18N("ErrorUtil::AniThrow: Throw error failed");
        return;
    }
}
