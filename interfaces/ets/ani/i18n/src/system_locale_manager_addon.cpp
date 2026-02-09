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

    static const char *className = "@ohos.i18n.i18n.SystemLocaleManager";
    ani_object sysLocaleMgrObject = VariableConverter::CreateAniObject(env, className, obj.get());
    obj.release();
    return sysLocaleMgrObject;
}

static ani_object CreateLocaleItem(ani_env *env, LocaleItem localeItem)
{
    static const char *className = "@ohos.i18n.i18n.LocaleItemInner";
    ani_class cls;
    if (ANI_OK != env->FindClass(className, &cls)) {
        HILOG_ERROR_I18N("CreateLocaleItem: Find class '%{public}s' failed", className);
        return nullptr;
    }

    ani_method ctor;
    if (ANI_OK != env->Class_FindMethod(cls, "<ctor>", nullptr, &ctor)) {
        HILOG_ERROR_I18N("CreateLocaleItem: Find method '<ctor>' failed");
        return nullptr;
    }

    ani_object ret;
    if (ANI_OK != env->Object_New(cls, ctor, &ret)) {
        HILOG_ERROR_I18N("CreateLocaleItem: New object '%{public}s' failed", className);
        return nullptr;
    }

    VariableConverter::SetStringMember(env, ret, "id", localeItem.id);
    VariableConverter::SetStringMember(env, ret, "displayName", localeItem.displayName);
    VariableConverter::SetStringMember(env, ret, "localName", localeItem.localName);

    static const char *enumName = "@ohos.i18n.i18n.SuggestionType";
    if (ANI_OK != env->Object_SetPropertyByName_Ref(ret, "suggestionType", VariableConverter::GetEnumItemByIndex(env,
        enumName, static_cast<int>(localeItem.suggestionType)))) {
        HILOG_ERROR_I18N("CreateLocaleItem: Set property 'suggestionType' failed");
    }
    return ret;
}

static ani_object CreateLocaleItemArray(ani_env *env, std::vector<LocaleItem> LocaleItemList)
{
    static const char *className = "std.core.Array";
    ani_class cls;
    if (ANI_OK != env->FindClass(className, &cls)) {
        HILOG_ERROR_I18N("CreateLocaleItemArray: Find class '%{public}s' failed", className);
        return nullptr;
    }

    ani_method ctor;
    if (ANI_OK != env->Class_FindMethod(cls, "<ctor>", "i:", &ctor)) {
        HILOG_ERROR_I18N("CreateLocaleItemArray: Find method '<ctor>' failed");
        return nullptr;
    }

    ani_object ret;
    if (ANI_OK != env->Object_New(cls, ctor, &ret, LocaleItemList.size())) {
        HILOG_ERROR_I18N("CreateLocaleItemArray: New object '%{public}s' failed", className);
        return nullptr;
    }

    ani_method set;
    if (ANI_OK != env->Class_FindMethod(cls, "$_set", "iY:", &set)) {
        HILOG_ERROR_I18N("CreateLocaleItemArray: Find method '$_set' failed");
        return ret;
    }

    for (size_t i = 0; i < LocaleItemList.size(); ++i) {
        if (ANI_OK != env->Object_CallMethod_Void(ret, set, i, CreateLocaleItem(env, LocaleItemList[i]))) {
            HILOG_ERROR_I18N("CreateLocaleItemArray: Call method '$_set' failed");
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
        HILOG_ERROR_I18N("I18nSysLocaleMgrAddon::GetLanguageInfoArray: Check options reference is undefined failed");
        return nullptr;
    }
    if (isUndefined) {
        localeTag = LocaleConfig::GetSystemLocale();
    } else {
        std::string className = "@ohos.i18n.i18n.SortOptionsInner";
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
        HILOG_ERROR_I18N("I18nSysLocaleMgrAddon::GetLanguageInfoArray: Get SystemLocaleManager object failed");
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
        HILOG_ERROR_I18N("I18nSysLocaleMgrAddon::GetRegionInfoArray: Check options reference is undefined failed");
        return nullptr;
    }
    if (isUndefined) {
        localeTag = LocaleConfig::GetSystemLocale();
    } else {
        std::string className = "@ohos.i18n.i18n.SortOptionsInner";
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
        HILOG_ERROR_I18N("I18nSysLocaleMgrAddon::GetRegionInfoArray: Get SystemLocaleManager object failed");
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
    static const char *className = "@ohos.i18n.i18n.TimeZoneCityItemInner";
    ani_class cls;
    if (ANI_OK != env->FindClass(className, &cls)) {
        HILOG_ERROR_I18N("CreateTimeZoneCityItem: Find class '%{public}s' failed", className);
        return nullptr;
    }

    ani_method ctor;
    if (ANI_OK != env->Class_FindMethod(cls, "<ctor>", nullptr, &ctor)) {
        HILOG_ERROR_I18N("CreateTimeZoneCityItem: Find method '<ctor>' failed");
        return nullptr;
    }

    ani_object ret;
    if (ANI_OK != env->Object_New(cls, ctor, &ret)) {
        HILOG_ERROR_I18N("CreateTimeZoneCityItem: New object '%{public}s' failed", className);
        return nullptr;
    }

    VariableConverter::SetStringMember(env, ret, "zoneId", item.zoneId);
    VariableConverter::SetStringMember(env, ret, "cityId", item.cityId);
    VariableConverter::SetStringMember(env, ret, "cityDisplayName", item.cityDisplayName);
    VariableConverter::SetStringMember(env, ret, "zoneDisplayName", item.zoneDisplayName);
    VariableConverter::SetNumberMember(env, ret, "rawOffset", item.rawOffset);

    if (ANI_OK != env->Object_SetPropertyByName_Int(ret, "offset", item.offset)) {
        HILOG_ERROR_I18N("CreateTimeZoneCityItem: Set property 'offset' failed");
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

    static const char *className = "std.core.Array";
    ani_class cls;
    if (ANI_OK != env->FindClass(className, &cls)) {
        HILOG_ERROR_I18N("GetTimeZoneCityItemArray: Find class '%{public}s' failed", className);
        return nullptr;
    }

    ani_method ctor;
    if (ANI_OK != env->Class_FindMethod(cls, "<ctor>", "i:", &ctor)) {
        HILOG_ERROR_I18N("GetTimeZoneCityItemArray: Find method '<ctor>' failed");
        return nullptr;
    }

    ani_object ret;
    if (ANI_OK != env->Object_New(cls, ctor, &ret, timezoneCityItemList.size())) {
        HILOG_ERROR_I18N("GetTimeZoneCityItemArray: New object '%{public}s' failed", className);
        return nullptr;
    }

    ani_method set;
    if (ANI_OK != env->Class_FindMethod(cls, "$_set", "iY:", &set)) {
        HILOG_ERROR_I18N("GetTimeZoneCityItemArray: Find method '$_set' failed");
        return ret;
    }

    for (size_t i = 0; i < timezoneCityItemList.size(); ++i) {
        if (ANI_OK != env->Object_CallMethod_Void(ret, set, i, CreateTimeZoneCityItem(env, timezoneCityItemList[i]))) {
            HILOG_ERROR_I18N("GetTimeZoneCityItemArray: Call method '$_set' failed");
            return ret;
        }
    }
    return ret;
}

ani_status I18nSysLocaleMgrAddon::BindContextSysLocaleMgr(ani_env *env)
{
    static const char *className = "@ohos.i18n.i18n.SystemLocaleManager";
    ani_class cls;
    if (ANI_OK != env->FindClass(className, &cls)) {
        HILOG_ERROR_I18N("BindContextSysLocaleMgr: Find class '%{public}s' failed", className);
        return ANI_ERROR;
    }

    std::array staticMethods = {
        ani_native_function { "create", nullptr, reinterpret_cast<void *>(I18nSysLocaleMgrAddon::Create) },
        ani_native_function { "getTimeZoneCityItemArray", nullptr,
            reinterpret_cast<void *>(I18nSysLocaleMgrAddon::GetTimeZoneCityItemArray) },
    };
    if (ANI_OK != env->Class_BindStaticNativeMethods(cls, staticMethods.data(), staticMethods.size())) {
        HILOG_ERROR_I18N("Cannot bind static native methods to '%{public}s'", className);
        return ANI_ERROR;
    }

    std::array methods = {
        ani_native_function { "getLanguageInfoArray", nullptr,
            reinterpret_cast<void *>(I18nSysLocaleMgrAddon::GetLanguageInfoArray) },
        ani_native_function { "getRegionInfoArray", nullptr,
            reinterpret_cast<void *>(I18nSysLocaleMgrAddon::GetRegionInfoArray) },
    };
    if (ANI_OK != env->Class_BindNativeMethods(cls, methods.data(), methods.size())) {
        HILOG_ERROR_I18N("BindContextSysLocaleMgr: Cannot bind native methods to '%{public}s'", className);
        return ANI_ERROR;
    }
    return ANI_OK;
}
