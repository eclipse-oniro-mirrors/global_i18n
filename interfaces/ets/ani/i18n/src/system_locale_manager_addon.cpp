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

#include "system_locale_manager_addon.h"

#include "error_util.h"
#include "i18n_hilog.h"
#include "locale_config.h"
#include "variable_converter.h"

using namespace OHOS;
using namespace Global;
using namespace I18n;

I18nSysLocaleMgrAddon* I18nSysLocaleMgrAddon::UnwrapAddon(ani_env *env, ani_object object)
{
    ani_long ptr;
    if (ANI_OK != env->Object_GetFieldByName_Long(object, "nativeSysLocaleMgr", &ptr)) {
        HILOG_ERROR_I18N("Get Long 'nativeSysLocaleMgr' failed");
        return nullptr;
    }
    return reinterpret_cast<I18nSysLocaleMgrAddon*>(ptr);
}

ani_object I18nSysLocaleMgrAddon::Create(ani_env *env, [[maybe_unused]] ani_object object)
{
    std::unique_ptr<I18nSysLocaleMgrAddon> obj = std::make_unique<I18nSysLocaleMgrAddon>();
    obj->systemLocaleManager_ = std::make_unique<SystemLocaleManager>();

    static const char *className = "L@ohos/i18n/i18n/SystemLocaleManager;";
    ani_object sysLocaleMgrObject = VariableConverter::CreateAniObject(env, className, obj.get());
    obj.release();
    return sysLocaleMgrObject;
}

static ani_object CreateLocaleItem(ani_env *env, LocaleItem localeItem)
{
    static const char *className = "L@ohos/i18n/i18n/LocaleItemInner;";
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

    VariableConverter::SetStringMember(env, ret, "id", localeItem.id);
    VariableConverter::SetStringMember(env, ret, "displayName", localeItem.displayName);
    VariableConverter::SetStringMember(env, ret, "localName", localeItem.localName);

    static const char *enumName = "L@ohos/i18n/i18n/SuggestionType;";
    if (ANI_OK != env->Object_SetPropertyByName_Ref(ret, "suggestionType", VariableConverter::GetEnumItemByIndex(env,
        enumName, static_cast<int>(localeItem.suggestionType)))) {
        HILOG_ERROR_I18N("Set property 'suggestionType' failed");
    }
    return ret;
}

static ani_object CreateLocaleItemArray(ani_env *env, std::vector<LocaleItem> LocaleItemList)
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

    ani_object ret;
    if (ANI_OK != env->Object_New(cls, ctor, &ret, LocaleItemList.size())) {
        HILOG_ERROR_I18N("New object '%{public}s' failed", className);
        return nullptr;
    }

    ani_method set;
    if (ANI_OK != env->Class_FindMethod(cls, "$_set", "ILstd/core/Object;:V", &set)) {
        HILOG_ERROR_I18N("Find method '$_set' failed");
        return ret;
    }

    for (size_t i = 0; i < LocaleItemList.size(); ++i) {
        if (ANI_OK != env->Object_CallMethod_Void(ret, set, i, CreateLocaleItem(env, LocaleItemList[i]))) {
            HILOG_ERROR_I18N("Call method '$_set' failed");
            return ret;
        }
    }
    return ret;
}

ani_object I18nSysLocaleMgrAddon::GetLanguageInfoArray(ani_env *env, ani_object object,
    ani_object languages, ani_object options)
{
    std::vector<std::string> languageList;
    if (!VariableConverter::ParseStringArray(env, languages, languageList)) {
        return nullptr;
    }

    SortOptions sortOptions;
    std::string localeTag;
    bool isUseLocalName = true;
    bool isSuggestedFirst = true;

    ani_boolean isUndefined;
    if (ANI_OK != env->Reference_IsUndefined(options, &isUndefined)) {
        HILOG_ERROR_I18N("Reference IsUndefined failed");
        return nullptr;
    }
    if (isUndefined) {
        localeTag = LocaleConfig::GetSystemLocale();
    } else {
        std::string className = "L@ohos/i18n/i18n/SortOptionsInner;";
        if (!VariableConverter::GetStringMember(env, options, "locale", localeTag)) {
            localeTag = LocaleConfig::GetSystemLocale();
        }
        if (!VariableConverter::GetBooleanMember(env, options, "isUseLocalName", isUseLocalName)) {
            isUseLocalName = true;
        }
        if (!VariableConverter::GetBooleanMember(env, options, "isSuggestedFirst", isSuggestedFirst)) {
            isSuggestedFirst = true;
        }
    }

    sortOptions.localeTag = localeTag;
    sortOptions.isUseLocalName = isUseLocalName;
    sortOptions.isSuggestedFirst = isSuggestedFirst;

    I18nSysLocaleMgrAddon* obj = UnwrapAddon(env, object);
    if (obj == nullptr || obj->systemLocaleManager_ == nullptr) {
        HILOG_ERROR_I18N("GetLanguageInfoArray: Get SystemLocaleManager object failed");
        return nullptr;
    }

    I18nErrorCode err;
    std::vector<LocaleItem> localeItemList =
        obj->systemLocaleManager_->GetLanguageInfoArray(languageList, sortOptions, err);
    if (err == I18nErrorCode::NOT_SYSTEM_APP) {
        ErrorUtil::AniThrow(env, I18N_NOT_SYSTEM_APP, "", "");
        return nullptr;
    }
    return CreateLocaleItemArray(env, localeItemList);
}

ani_object I18nSysLocaleMgrAddon::GetRegionInfoArray(ani_env *env, ani_object object,
    ani_object regions, ani_object options)
{
    std::vector<std::string> countryList;
    if (!VariableConverter::ParseStringArray(env, regions, countryList)) {
        return nullptr;
    }

    SortOptions sortOptions;
    std::string localeTag;
    bool isUseLocalName = false;
    bool isSuggestedFirst = true;

    ani_boolean isUndefined;
    if (ANI_OK != env->Reference_IsUndefined(options, &isUndefined)) {
        HILOG_ERROR_I18N("Reference IsUndefined failed");
        return nullptr;
    }
    if (isUndefined) {
        localeTag = LocaleConfig::GetSystemLocale();
    } else {
        std::string className = "L@ohos/i18n/i18n/SortOptionsInner;";
        if (!VariableConverter::GetStringMember(env, options, "locale", localeTag)) {
            localeTag = LocaleConfig::GetSystemLocale();
        }
        if (!VariableConverter::GetBooleanMember(env, options, "isUseLocalName", isUseLocalName)) {
            isUseLocalName = false;
        }
        if (!VariableConverter::GetBooleanMember(env, options, "isSuggestedFirst", isSuggestedFirst)) {
            isSuggestedFirst = true;
        }
    }

    sortOptions.localeTag = localeTag;
    sortOptions.isUseLocalName = isUseLocalName;
    sortOptions.isSuggestedFirst = isSuggestedFirst;

    I18nSysLocaleMgrAddon* obj = UnwrapAddon(env, object);
    if (obj == nullptr || obj->systemLocaleManager_ == nullptr) {
        HILOG_ERROR_I18N("GetRegionInfoArray: Get SystemLocaleManager object failed");
        return nullptr;
    }

    I18nErrorCode err;
    std::vector<LocaleItem> localeItemList =
        obj->systemLocaleManager_->GetCountryInfoArray(countryList, sortOptions, err);
    if (err == I18nErrorCode::NOT_SYSTEM_APP) {
        ErrorUtil::AniThrow(env, I18N_NOT_SYSTEM_APP, "", "");
        return nullptr;
    }
    return CreateLocaleItemArray(env, localeItemList);
}

static ani_object CreateTimeZoneCityItem(ani_env *env, TimeZoneCityItem item)
{
    static const char *className = "L@ohos/i18n/i18n/TimeZoneCityItemInner;";
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

    VariableConverter::SetStringMember(env, ret, "zoneId", item.zoneId);
    VariableConverter::SetStringMember(env, ret, "cityId", item.cityId);
    VariableConverter::SetStringMember(env, ret, "cityDisplayName", item.cityDisplayName);
    VariableConverter::SetStringMember(env, ret, "zoneDisplayName", item.zoneDisplayName);
    VariableConverter::SetNumberMember(env, ret, "rawOffset", item.rawOffset);

    if (ANI_OK != env->Object_SetPropertyByName_Double(ret, "offset", item.offset)) {
        HILOG_ERROR_I18N("Set property 'offset' failed");
    }
    return ret;
}

ani_object I18nSysLocaleMgrAddon::GetTimeZoneCityItemArray(ani_env *env, [[maybe_unused]] ani_object object)
{
    I18nErrorCode err;
    std::vector<TimeZoneCityItem> timezoneCityItemList = SystemLocaleManager::GetTimezoneCityInfoArray(err);
    if (err == I18nErrorCode::NOT_SYSTEM_APP) {
        ErrorUtil::AniThrow(env, I18N_NOT_SYSTEM_APP, "", "");
        return nullptr;
    }

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

    ani_object ret;
    if (ANI_OK != env->Object_New(cls, ctor, &ret, timezoneCityItemList.size())) {
        HILOG_ERROR_I18N("New object '%{public}s' failed", className);
        return nullptr;
    }

    ani_method set;
    if (ANI_OK != env->Class_FindMethod(cls, "$_set", "ILstd/core/Object;:V", &set)) {
        HILOG_ERROR_I18N("Find method '$_set' failed");
        return ret;
    }

    for (size_t i = 0; i < timezoneCityItemList.size(); ++i) {
        if (ANI_OK != env->Object_CallMethod_Void(ret, set, i, CreateTimeZoneCityItem(env, timezoneCityItemList[i]))) {
            HILOG_ERROR_I18N("Call method '$_set' failed");
            return ret;
        }
    }
    return ret;
}

ani_status I18nSysLocaleMgrAddon::BindContextSysLocaleMgr(ani_env *env)
{
    static const char *className = "L@ohos/i18n/i18n/SystemLocaleManager;";
    ani_class cls;
    if (ANI_OK != env->FindClass(className, &cls)) {
        HILOG_ERROR_I18N("Find class '%{public}s' failed", className);
        return ANI_ERROR;
    }

    std::array methods = {
        ani_native_function { "create", nullptr, reinterpret_cast<void *>(I18nSysLocaleMgrAddon::Create) },
        ani_native_function { "getLanguageInfoArray", nullptr,
            reinterpret_cast<void *>(I18nSysLocaleMgrAddon::GetLanguageInfoArray) },
        ani_native_function { "getRegionInfoArray", nullptr,
            reinterpret_cast<void *>(I18nSysLocaleMgrAddon::GetRegionInfoArray) },
        ani_native_function { "getTimeZoneCityItemArray", nullptr,
            reinterpret_cast<void *>(I18nSysLocaleMgrAddon::GetTimeZoneCityItemArray) },
    };

    if (ANI_OK != env->Class_BindNativeMethods(cls, methods.data(), methods.size())) {
        HILOG_ERROR_I18N("Cannot bind native methods to '%{public}s'", className);
        return ANI_ERROR;
    };

    return ANI_OK;
}
