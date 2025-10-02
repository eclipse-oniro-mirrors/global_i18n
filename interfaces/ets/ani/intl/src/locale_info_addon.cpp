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

#include "locale_info_addon.h"

#include "i18n_hilog.h"
#include "variable_converter.h"

using namespace OHOS;
using namespace Global;
using namespace I18n;

LocaleInfoAddon* LocaleInfoAddon::UnwrapAddon(ani_env *env, ani_object object, const std::string &name)
{
    ani_long ptr;
    if (ANI_OK != env->Object_GetFieldByName_Long(object, name.c_str(), &ptr)) {
        HILOG_ERROR_I18N("Get Long '%{public}s' failed", name.c_str());
        return nullptr;
    }
    return reinterpret_cast<LocaleInfoAddon*>(ptr);
}

template<typename T>
T LocaleInfoAddon::GetLocaleProperty(ani_env *env, ani_object object,
    const std::string &propertyName, T (LocaleInfo::*getter)() const)
{
    LocaleInfoAddon* obj = UnwrapAddon(env, object, "nativeLocale");
    if (obj == nullptr || obj->locale_ == nullptr) {
        HILOG_ERROR_I18N("%{public}s: Get Locale object failed", propertyName.c_str());
        return T{};
    }
    return (obj->locale_.get()->*getter)();
}

ani_string LocaleInfoAddon::Locale_ToString(ani_env *env, ani_object object)
{
    std::string value = GetLocaleProperty(env, object, "toString", &LocaleInfo::ToString);
    return VariableConverter::StringToAniStr(env, value);
}

ani_object LocaleInfoAddon::Locale_Maximize(ani_env *env, ani_object object)
{
    LocaleInfoAddon* obj = UnwrapAddon(env, object, "nativeLocale");
    if (obj == nullptr || obj->locale_ == nullptr) {
        HILOG_ERROR_I18N("Maximize: Get Locale object failed");
        return nullptr;
    }

    std::string localeTag = obj->locale_->Maximize();
    return Locale_CreateWithParam(env, nullptr, VariableConverter::StringToAniStr(env, localeTag), nullptr);
}

ani_object LocaleInfoAddon::Locale_Minimize(ani_env *env, ani_object object)
{
    LocaleInfoAddon* obj = UnwrapAddon(env, object, "nativeLocale");
    if (obj == nullptr || obj->locale_ == nullptr) {
        HILOG_ERROR_I18N("Minimize: Get Locale object failed");
        return nullptr;
    }

    std::string localeTag = obj->locale_->Minimize();
    return Locale_CreateWithParam(env, nullptr, VariableConverter::StringToAniStr(env, localeTag), nullptr);
}

ani_string LocaleInfoAddon::Locale_GetLanguage(ani_env *env, ani_object object)
{
    std::string value = GetLocaleProperty(env, object, "getLanguage", &LocaleInfo::GetLanguage);
    return VariableConverter::StringToAniStr(env, value);
}

ani_string LocaleInfoAddon::Locale_GetScript(ani_env *env, ani_object object)
{
    std::string value = GetLocaleProperty(env, object, "getScript", &LocaleInfo::GetScript);
    return VariableConverter::StringToAniStr(env, value);
}

ani_string LocaleInfoAddon::Locale_GetRegion(ani_env *env, ani_object object)
{
    std::string value = GetLocaleProperty(env, object, "getRegion", &LocaleInfo::GetRegion);
    return VariableConverter::StringToAniStr(env, value);
}

ani_string LocaleInfoAddon::Locale_GetBaseName(ani_env *env, ani_object object)
{
    std::string value = GetLocaleProperty(env, object, "getBaseName", &LocaleInfo::GetBaseName);
    return VariableConverter::StringToAniStr(env, value);
}

ani_string LocaleInfoAddon::Locale_GetCaseFirst(ani_env *env, ani_object object)
{
    std::string value = GetLocaleProperty(env, object, "getCaseFirst", &LocaleInfo::GetCaseFirst);
    return VariableConverter::StringToAniStr(env, value);
}

ani_string LocaleInfoAddon::Locale_GetCalendar(ani_env *env, ani_object object)
{
    std::string value = GetLocaleProperty(env, object, "getCalendar", &LocaleInfo::GetCalendar);
    return VariableConverter::StringToAniStr(env, value);
}

ani_string LocaleInfoAddon::Locale_GetCollation(ani_env *env, ani_object object)
{
    std::string value = GetLocaleProperty(env, object, "getCollation", &LocaleInfo::GetCollation);
    return VariableConverter::StringToAniStr(env, value);
}

ani_string LocaleInfoAddon::Locale_GetHourCycle(ani_env *env, ani_object object)
{
    std::string value = GetLocaleProperty(env, object, "getHourCycle", &LocaleInfo::GetHourCycle);
    return VariableConverter::StringToAniStr(env, value);
}

ani_string LocaleInfoAddon::Locale_GetNumberingSystem(ani_env *env, ani_object object)
{
    std::string value = GetLocaleProperty(env, object, "getNumberingSystem", &LocaleInfo::GetNumberingSystem);
    return VariableConverter::StringToAniStr(env, value);
}

ani_boolean LocaleInfoAddon::Locale_GetNumeric(ani_env *env, ani_object object)
{
    LocaleInfoAddon* obj = UnwrapAddon(env, object, "nativeLocale");
    if (obj == nullptr || obj->locale_ == nullptr) {
        HILOG_ERROR_I18N("getNumeric: Get Locale object failed");
        return false;
    }
    return (obj->locale_->GetNumeric() == "true");
}

ani_object LocaleInfoAddon::Locale_Create(ani_env *env, [[maybe_unused]] ani_object object)
{
    std::map<std::string, std::string> map = {};
    std::unique_ptr<LocaleInfoAddon> obj = std::make_unique<LocaleInfoAddon>();
    obj->locale_ = std::make_unique<LocaleInfo>("", map);

    static const char *className = "L@ohos/intl/intl/Locale;";
    ani_object localeObject = VariableConverter::CreateAniObject(env, className, obj.get());
    obj.release();
    return localeObject;
}

ani_object LocaleInfoAddon::Locale_CreateWithParam(ani_env *env, [[maybe_unused]] ani_object object,
    ani_string locale, ani_object options)
{
    std::map<std::string, std::string> map = {};
    ani_boolean isUndefined;
    if (ANI_OK != env->Reference_IsUndefined(options, &isUndefined)) {
        HILOG_ERROR_I18N("Reference IsUndefined failed");
        return nullptr;
    }
    if (!isUndefined) {
        const std::vector<std::string> keys = {
            "calendar", "collation", "hourCycle", "numberingSystem", "caseFirst"
        };

        for (const auto& key : keys) {
            std::string value;
            if (VariableConverter::GetStringMember(env, options, key, value)) {
                map.insert(std::make_pair(key, value));
            }
        }

        bool numeric;
        if (VariableConverter::GetBooleanMember(env, options, "numeric", numeric)) {
            map.insert(std::make_pair("numeric", numeric ? "true" : "false"));
        }
    }

    std::unique_ptr<LocaleInfoAddon> obj = std::make_unique<LocaleInfoAddon>();
    obj->locale_ = std::make_unique<LocaleInfo>(VariableConverter::AniStrToString(env, locale), map);
    static const char *className = "L@ohos/intl/intl/Locale;";
    ani_object localeObject = VariableConverter::CreateAniObject(env, className, obj.get());
    obj.release();
    return localeObject;
}

ani_status LocaleInfoAddon::BindContext_Locale(ani_env *env)
{
    static const char *className = "L@ohos/intl/intl/Locale;";
    ani_class cls;
    if (ANI_OK != env->FindClass(className, &cls)) {
        HILOG_ERROR_I18N("Find class '%{public}s' failed", className);
        return ANI_ERROR;
    }

    std::array methods = {
        ani_native_function {"create", ":L@ohos/intl/intl/Locale;", reinterpret_cast<void *>(Locale_Create) },
        ani_native_function {"create", "Lstd/core/String;L@ohos/intl/intl/LocaleOptions;:L@ohos/intl/intl/Locale;",
            reinterpret_cast<void *>(Locale_CreateWithParam) },
        ani_native_function {"toString", nullptr, reinterpret_cast<void *>(Locale_ToString) },
        ani_native_function {"maximize", nullptr, reinterpret_cast<void *>(Locale_Maximize) },
        ani_native_function {"minimize", nullptr, reinterpret_cast<void *>(Locale_Minimize) },
        ani_native_function {"getLanguage", nullptr, reinterpret_cast<void *>(Locale_GetLanguage) },
        ani_native_function {"getScript", nullptr, reinterpret_cast<void *>(Locale_GetScript) },
        ani_native_function {"getRegion", nullptr, reinterpret_cast<void *>(Locale_GetRegion) },
        ani_native_function {"getBaseName", nullptr, reinterpret_cast<void *>(Locale_GetBaseName) },
        ani_native_function {"getCaseFirst", nullptr, reinterpret_cast<void *>(Locale_GetCaseFirst) },
        ani_native_function {"getCalendar", nullptr, reinterpret_cast<void *>(Locale_GetCalendar) },
        ani_native_function {"getCollation", nullptr, reinterpret_cast<void *>(Locale_GetCollation) },
        ani_native_function {"getHourCycle", nullptr, reinterpret_cast<void *>(Locale_GetHourCycle) },
        ani_native_function {"getNumberingSystem", nullptr, reinterpret_cast<void *>(Locale_GetNumberingSystem) },
        ani_native_function {"getNumeric", nullptr, reinterpret_cast<void *>(Locale_GetNumeric) },
    };

    if (ANI_OK != env->Class_BindNativeMethods(cls, methods.data(), methods.size())) {
        HILOG_ERROR_I18N("Cannot bind native methods to '%{public}s'", className);
        return ANI_ERROR;
    };
    return ANI_OK;
}
