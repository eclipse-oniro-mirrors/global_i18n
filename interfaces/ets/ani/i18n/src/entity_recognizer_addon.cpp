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

#include "entity_recognizer_addon.h"

#include "error_util.h"
#include "i18n_hilog.h"
#include "locale_config.h"
#include "variable_converter.h"

using namespace OHOS;
using namespace Global;
using namespace I18n;

static const int INDEX_ONE_OF_VECTOR = 0;
static const int INDEX_TWO_OF_VECTOR = 1;

I18nEntityRecognizerAddon* I18nEntityRecognizerAddon::UnwrapAddon(ani_env *env, ani_object object)
{
    ani_long ptr;
    if (ANI_OK != env->Object_GetFieldByName_Long(object, "nativeEntityRecognizer", &ptr)) {
        HILOG_ERROR_I18N("Get Long 'nativeEntityRecognizer' failed");
        return nullptr;
    }
    return reinterpret_cast<I18nEntityRecognizerAddon*>(ptr);
}

ani_object I18nEntityRecognizerAddon::Create(ani_env *env, [[maybe_unused]] ani_object object, ani_string locale)
{
    std::string localeStr;
    ani_boolean isUndefined;
    if (ANI_OK != env->Reference_IsUndefined(locale, &isUndefined)) {
        HILOG_ERROR_I18N("Reference IsUndefined failed");
        return nullptr;
    }
    if (isUndefined) {
        localeStr = LocaleConfig::GetSystemLocale();
    } else {
        localeStr = VariableConverter::AniStrToString(env, locale);
    }

    UErrorCode localeStatus = U_ZERO_ERROR;
    icu::Locale icuLocale = icu::Locale::forLanguageTag(localeStr, localeStatus);
    if (U_FAILURE(localeStatus)) {
        HILOG_ERROR_I18N("EntityRecognizerAddon::Create: Create icu::Locale failed.");
        return nullptr;
    } else if (!IsValidLocaleTag(icuLocale)) {
        ErrorUtil::AniThrow(env, I18N_NOT_VALID, "locale", "a valid locale");
        return nullptr;
    }
    std::unique_ptr<I18nEntityRecognizerAddon> obj = std::make_unique<I18nEntityRecognizerAddon>();
    obj->entityRecognizer_ = std::make_unique<EntityRecognizer>(icuLocale);

    static const char *className = "L@ohos/i18n/i18n/EntityRecognizer;";
    ani_object entityRecognizerObject = VariableConverter::CreateAniObject(env, className, obj.get());
    obj.release();
    return entityRecognizerObject;
}

static ani_object CreateEntityInfoItem(ani_env *env, const int begin, const int end, const std::string &type)
{
    static const char *className = "L@ohos/i18n/i18n/EntityInfoItemInner;";
    ani_class cls;
    if (ANI_OK != env->FindClass(className, &cls)) {
        HILOG_ERROR_I18N("Find class '%{public}s' failed", className);
        return nullptr;
    }

    ani_method ctor;
    if (ANI_OK != env->Class_FindMethod(cls, "<ctor>", nullptr, &ctor)) {
        HILOG_ERROR_I18N("Find method '<ctor>' failed");
        return nullptr;
    }

    ani_object ret;
    if (ANI_OK != env->Object_New(cls, ctor, &ret)) {
        HILOG_ERROR_I18N("New object '%{public}s' failed", className);
        return nullptr;
    }

    VariableConverter::SetStringMember(env, ret, "type", type);
    if (ANI_OK != env->Object_SetPropertyByName_Double(ret, "begin", begin)) {
        HILOG_ERROR_I18N("Set property 'begin' failed");
    }
    if (ANI_OK != env->Object_SetPropertyByName_Double(ret, "end", end)) {
        HILOG_ERROR_I18N("Set property 'end' failed");
    }
    return ret;
}

static ani_object GetEntityInfoItem(ani_env *env, std::vector<std::vector<int>>& entityInfo)
{
    static const char *className = "Lescompat/Array;";
    ani_class cls;
    if (ANI_OK != env->FindClass(className, &cls)) {
        HILOG_ERROR_I18N("Find class '%{public}s' failed", className);
        return nullptr;
    }

    ani_method ctor;
    if (ANI_OK != env->Class_FindMethod(cls, "<ctor>", "I:V", &ctor)) {
        HILOG_ERROR_I18N("Find method '<ctor>' failed");
        return nullptr;
    }

    if (entityInfo.size() <= INDEX_TWO_OF_VECTOR ||
        entityInfo[INDEX_ONE_OF_VECTOR].size() <= INDEX_ONE_OF_VECTOR ||
        entityInfo[INDEX_TWO_OF_VECTOR].size() <= INDEX_ONE_OF_VECTOR) {
        HILOG_ERROR_I18N("Invalid entityInfo structure, potential out of bounds access");
        return nullptr;
    }

    ani_object ret;
    if (ANI_OK != env->Object_New(cls, ctor, &ret, (entityInfo[INDEX_ONE_OF_VECTOR][INDEX_ONE_OF_VECTOR] +
            entityInfo[INDEX_TWO_OF_VECTOR][INDEX_ONE_OF_VECTOR]))) {
        HILOG_ERROR_I18N("New object '%{public}s' failed", className);
        return nullptr;
    }

    ani_method set;
    if (ANI_OK != env->Class_FindMethod(cls, "$_set", "ILstd/core/Object;:V", &set)) {
        HILOG_ERROR_I18N("Find method '$_set' failed");
        return ret;
    }

    std::vector<std::string> types = {"phone_number", "date"};
    int index = 0;
    for (size_t t = 0; t < types.size(); ++t) {
        for (int i = 0; i < entityInfo[t][INDEX_ONE_OF_VECTOR]; ++i) {
            int begin = entityInfo[t][2 * i + 1];
            int end = entityInfo[t][2 * i + 2];
            std::string type = types[t];
            if (ANI_OK != env->Object_CallMethod_Void(ret, set, index, CreateEntityInfoItem(env, begin, end, type))) {
                HILOG_ERROR_I18N("Call method '$_set' failed");
                return ret;
            }
            ++index;
        }
    }
    return ret;
}

ani_object I18nEntityRecognizerAddon::FindEntityInfo(ani_env *env, ani_object object, ani_string text)
{
    I18nEntityRecognizerAddon* obj = UnwrapAddon(env, object);
    if (obj == nullptr || obj->entityRecognizer_ == nullptr) {
        HILOG_ERROR_I18N("Get EntityRecognizer object failed");
        return nullptr;
    }

    std::string message = VariableConverter::AniStrToString(env, text);
    std::vector<std::vector<int>> entityInfo = obj->entityRecognizer_->FindEntityInfo(message);
    return GetEntityInfoItem(env, entityInfo);
}

ani_status I18nEntityRecognizerAddon::BindContextEntityRecognizer(ani_env *env)
{
    static const char *className = "L@ohos/i18n/i18n/EntityRecognizer;";
    ani_class cls;
    if (ANI_OK != env->FindClass(className, &cls)) {
        HILOG_ERROR_I18N("Find class '%{public}s' failed", className);
        return ANI_ERROR;
    }

    std::array methods = {
        ani_native_function { "create", nullptr, reinterpret_cast<void *>(I18nEntityRecognizerAddon::Create) },
        ani_native_function { "findEntityInfo", nullptr,
            reinterpret_cast<void *>(I18nEntityRecognizerAddon::FindEntityInfo) },
    };

    if (ANI_OK != env->Class_BindNativeMethods(cls, methods.data(), methods.size())) {
        HILOG_ERROR_I18N("Cannot bind native methods to '%{public}s'", className);
        return ANI_ERROR;
    };

    return ANI_OK;
}
