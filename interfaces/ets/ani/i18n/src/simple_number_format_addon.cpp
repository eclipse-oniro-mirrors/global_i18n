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

#include "simple_number_format_addon.h"

#include "error_util.h"
#include "i18n_hilog.h"
#include "variable_converter.h"

using namespace OHOS;
using namespace Global;
using namespace I18n;

I18nSimpleNumberFormatAddon* I18nSimpleNumberFormatAddon::UnwrapAddon(ani_env *env, ani_object object)
{
    ani_long ptr;
    if (ANI_OK != env->Object_GetFieldByName_Long(object, "nativeSimpleNumberFormat", &ptr)) {
        HILOG_ERROR_I18N("Get Long 'nativeSimpleNumberFormat' failed");
        return nullptr;
    }
    return reinterpret_cast<I18nSimpleNumberFormatAddon*>(ptr);
}

ani_object I18nSimpleNumberFormatAddon::GetSimpleNumberFormatBySkeleton(ani_env *env,
    ani_string skeleton, ani_object locale)
{
    ani_boolean isUndefined;
    if (ANI_OK != env->Reference_IsUndefined(locale, &isUndefined)) {
        HILOG_ERROR_I18N("GetSimpleNumberFormatBySkeleton: Check locale reference is undefined failed");
        return nullptr;
    }
    std::string localeTag = "";
    if (!isUndefined) {
        localeTag = VariableConverter::GetLocaleTagFromBuiltinLocale(env, locale);
    }
    std::shared_ptr<LocaleInfo> localeInfo = localeTag.empty() ? nullptr : std::make_shared<LocaleInfo>(localeTag);

    std::unique_ptr<I18nSimpleNumberFormatAddon> obj = std::make_unique<I18nSimpleNumberFormatAddon>();
    if (!obj) {
        HILOG_ERROR_I18N("GetSimpleNumberFormatBySkeleton: create I18nSimpleNumberFormatAddon failed.");
        return nullptr;
    }

    I18nErrorCode errCode = I18nErrorCode::SUCCESS;
    obj->simpleNumberFormat_ = std::make_shared<SimpleNumberFormat>(
        VariableConverter::AniStrToString(env, skeleton), localeInfo, errCode);
    if (errCode == I18nErrorCode::INVALID_NUMBER_FORMAT_SKELETON) {
        ErrorUtil::AniThrow(env, I18N_NOT_VALID, "skeleton", "a valid number format skeleton");
        return nullptr;
    } else if (!obj->simpleNumberFormat_ || errCode != I18nErrorCode::SUCCESS) {
        HILOG_ERROR_I18N("GetSimpleNumberFormatBySkeleton: Construct SimpleNumberFormat failed.");
        return nullptr;
    }

    static const char *className = "@ohos.i18n.i18n.SimpleNumberFormat";
    ani_object simpleNumberFormatObject = VariableConverter::CreateAniObject(env, className, obj.get());
    obj.release();
    return simpleNumberFormatObject;
}

ani_string I18nSimpleNumberFormatAddon::Format(ani_env *env, ani_object object, ani_double value)
{
    I18nSimpleNumberFormatAddon* obj = UnwrapAddon(env, object);
    if (obj == nullptr || obj->simpleNumberFormat_  == nullptr) {
        HILOG_ERROR_I18N("SimpleNumberFormatAddon::Format: Unwrap SimpleNumberFormatAddon failed.");
        return nullptr;
    }

    std::string formatResult = obj->simpleNumberFormat_->Format(value);
    if (formatResult.empty()) {
        HILOG_ERROR_I18N("SimpleNumberFormatAddon::Format: Format result is empty.");
    }
    return VariableConverter::StringToAniStr(env, formatResult);
}

ani_status I18nSimpleNumberFormatAddon::BindContextSimpleNumberFormat(ani_env *env)
{
    static const char *className = "@ohos.i18n.i18n.SimpleNumberFormat";
    ani_class cls;
    if (ANI_OK != env->FindClass(className, &cls)) {
        HILOG_ERROR_I18N("BindContextSimpleNumberFormat: Find class '%{public}s' failed", className);
        return ANI_ERROR;
    }

    std::array methods = {
        ani_native_function { "format", nullptr, reinterpret_cast<void *>(I18nSimpleNumberFormatAddon::Format) },
    };

    if (ANI_OK != env->Class_BindNativeMethods(cls, methods.data(), methods.size())) {
        HILOG_ERROR_I18N("BindContextSimpleNumberFormat: Cannot bind native methods to '%{public}s'", className);
        return ANI_ERROR;
    }

    return ANI_OK;
}
