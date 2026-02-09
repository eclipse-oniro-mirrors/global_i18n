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

#include "i18n_normalizer_addon.h"

#include "error_util.h"
#include "i18n_hilog.h"
#include "variable_converter.h"

using namespace OHOS;
using namespace Global;
using namespace I18n;
I18nNormalizerAddon* I18nNormalizerAddon::UnwrapAddon(ani_env *env, ani_object object)
{
    ani_long ptr;
    if (ANI_OK != env->Object_GetFieldByName_Long(object, "nativeNormalizer", &ptr)) {
        HILOG_ERROR_I18N("I18nNormalizerAddon::UnwrapAddon: Get field nativeNormalizer to long failed");
        return nullptr;
    }
    return reinterpret_cast<I18nNormalizerAddon*>(ptr);
}

ani_object I18nNormalizerAddon::GetInstance(ani_env *env, [[maybe_unused]] ani_object object, ani_enum_item mode)
{
    int32_t normalizerMode;
    if (ANI_OK != env->EnumItem_GetValue_Int(mode, &normalizerMode)) {
        HILOG_ERROR_I18N("I18nNormalizerAddon::GetInstance: Enum get value 'normalizerMode' failed");
        return nullptr;
    }

    if (normalizerMode != NORMALIZER_MODE_NFC && normalizerMode != NORMALIZER_MODE_NFD &&
        normalizerMode != NORMALIZER_MODE_NFKC && normalizerMode != NORMALIZER_MODE_NFKD) {
        HILOG_ERROR_I18N("I18nNormalizerAddon::GetInstance: Mode is not valid");
        ErrorUtil::AniThrow(env, I18N_NOT_FOUND, "mode", "a valid mode");
        return nullptr;
    }

    std::unique_ptr<I18nNormalizerAddon> obj = std::make_unique<I18nNormalizerAddon>();
    I18nNormalizerMode modeEnum = I18nNormalizerMode(normalizerMode);
    I18nErrorCode errorCode = I18nErrorCode::SUCCESS;
    obj->normalizer_ = std::make_unique<I18nNormalizer>(modeEnum, errorCode);
    if (errorCode != I18nErrorCode::SUCCESS || !obj->normalizer_) {
        return nullptr;
    }

    static const char* className = "@ohos.i18n.i18n.Normalizer";
    ani_object normalizerObject = VariableConverter::CreateAniObject(env, className, obj.get());
    obj.release();
    return normalizerObject;
}

ani_string I18nNormalizerAddon::Normalize(ani_env *env, ani_object object, ani_string text)
{
    I18nNormalizerAddon* obj = UnwrapAddon(env, object);
    if (obj == nullptr || obj->normalizer_ == nullptr) {
        HILOG_ERROR_I18N("I18nNormalizerAddon::Normalize: Get Normalizer object failed");
        return nullptr;
    }

    std::string textStr = VariableConverter::AniStrToString(env, text);
    I18nErrorCode errorCode = I18nErrorCode::SUCCESS;
    std::string normalizedText = obj->normalizer_->Normalize(textStr.c_str(), static_cast<int32_t>(textStr.length()),
        errorCode);
    if (errorCode != I18nErrorCode::SUCCESS) {
        return nullptr;
    }
    return VariableConverter::StringToAniStr(env, normalizedText);
}

ani_status I18nNormalizerAddon::BindContextNormalizer(ani_env *env)
{
    static const char *className = "@ohos.i18n.i18n.Normalizer";
    ani_class cls;
    if (ANI_OK != env->FindClass(className, &cls)) {
        HILOG_ERROR_I18N("BindContextNormalizer: Find class '%{public}s' failed", className);
        return ANI_ERROR;
    }

    std::array staticMethods = {
        ani_native_function { "getInstance", nullptr, reinterpret_cast<void *>(I18nNormalizerAddon::GetInstance) },
    };
    if (ANI_OK != env->Class_BindStaticNativeMethods(cls, staticMethods.data(), staticMethods.size())) {
        HILOG_ERROR_I18N("BindContextNormalizer: Cannot bind static native methods to '%{public}s'", className);
        return ANI_ERROR;
    }

    std::array methods = {
        ani_native_function { "normalize", nullptr, reinterpret_cast<void *>(I18nNormalizerAddon::Normalize) },
    };
    if (ANI_OK != env->Class_BindNativeMethods(cls, methods.data(), methods.size())) {
        HILOG_ERROR_I18N("BindContextNormalizer: Cannot bind native methods to '%{public}s'", className);
        return ANI_ERROR;
    }
    return ANI_OK;
}
