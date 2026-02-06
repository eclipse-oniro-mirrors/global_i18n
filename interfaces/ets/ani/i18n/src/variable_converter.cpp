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

#include "variable_converter.h"

#include "entity_recognizer_addon.h"
#include "holiday_manager_addon.h"
#include "i18n_addon.h"
#include "i18n_calendar_addon.h"
#include "i18n_hilog.h"
#include "i18n_normalizer_addon.h"
#include "i18n_timezone_addon.h"
#include "intl_addon.h"
#include "locale_info_addon.h"
#include "number_format_addon.h"
#include "simple_date_time_format_addon.h"
#include "simple_number_format_addon.h"
#include "system_locale_manager_addon.h"

using namespace OHOS;
using namespace Global;
using namespace I18n;

std::string VariableConverter::AniStrToString(ani_env *env, ani_ref aniStr)
{
    ani_string ani_str = static_cast<ani_string>(aniStr);
    ani_size strSize;
    if (ANI_OK != env->String_GetUTF8Size(ani_str, &strSize)) {
        HILOG_ERROR_I18N("Get utf8 size failed");
        return "";
    }

    std::vector<char> buffer(strSize + 1);
    ani_size bytes_written = 0;

    if (ANI_OK != env->String_GetUTF8(ani_str, buffer.data(), buffer.size(), &bytes_written)) {
        HILOG_ERROR_I18N("Create string failed");
        return "";
    }
    return std::string(buffer.data(), bytes_written);
}

ani_string VariableConverter::StringToAniStr(ani_env *env, const std::string &str)
{
    ani_string ret;
    if (ANI_OK != env->String_NewUTF8(str.c_str(), str.size(), &ret)) {
        HILOG_ERROR_I18N("Create ani string failed");
        return nullptr;
    }
    return ret;
}

void VariableConverter::SetBooleanMember(ani_env *env, ani_object obj,
    const std::string &name, const std::string &value)
{
    if (value.empty()) {
        return;
    }

    static const char *className = "Lstd/core/Boolean;";
    ani_class cls;
    if (ANI_OK != env->FindClass(className, &cls)) {
        HILOG_ERROR_I18N("Find class '%{public}s' failed", className);
        return;
    }

    ani_method ctor;
    if (ANI_OK != env->Class_FindMethod(cls, "<ctor>", "Z:V", &ctor)) {
        HILOG_ERROR_I18N("Find method '<ctor>' failed");
        return;
    }

    ani_boolean valueBool = value == "true";
    ani_object boolObj;
    if (ANI_OK != env->Object_New(cls, ctor, &boolObj, valueBool)) {
        HILOG_ERROR_I18N("New object '%{public}s' failed", className);
        return;
    }

    if (ANI_OK != env->Object_SetPropertyByName_Ref(obj, name.c_str(), boolObj)) {
        HILOG_ERROR_I18N("Set property '%{public}s' failed", name.c_str());
        return;
    }
}

bool VariableConverter::GetBooleanMember(ani_env *env, ani_object options, const std::string &name, bool& value)
{
    ani_ref ref;
    if (ANI_OK != env->Object_GetPropertyByName_Ref(options, name.c_str(), &ref)) {
        HILOG_ERROR_I18N("Get property '%{public}s' failed", name.c_str());
        return false;
    }

    ani_boolean isUndefined;
    if (ANI_OK != env->Reference_IsUndefined(ref, &isUndefined)) {
        HILOG_ERROR_I18N("Reference IsUndefined failed");
        return false;
    }
    if (isUndefined) {
        return false;
    }

    ani_boolean ret;
    if (ANI_OK != env->Object_CallMethodByName_Boolean(static_cast<ani_object>(ref), "unboxed", ":Z", &ret)) {
        HILOG_ERROR_I18N("Call method by name failed");
        return false;
    }
    value = ret;
    return true;
}

void VariableConverter::SetNumberMember(ani_env *env, ani_object obj, const std::string &name, const ani_int value)
{
    static const char *className = "Lstd/core/Int;";
    ani_class cls;
    if (ANI_OK != env->FindClass(className, &cls)) {
        HILOG_ERROR_I18N("Find class '%{public}s' failed", className);
        return;
    }

    ani_method ctor;
    if (ANI_OK != env->Class_FindMethod(cls, "<ctor>", "I:V", &ctor)) {
        HILOG_ERROR_I18N("Find method '<ctor>' failed");
        return;
    }

    ani_object intObj;
    if (ANI_OK != env->Object_New(cls, ctor, &intObj, value)) {
        HILOG_ERROR_I18N("New object '%{public}s' failed", className);
        return;
    }

    if (ANI_OK != env->Object_SetPropertyByName_Ref(obj, name.c_str(), intObj)) {
        HILOG_ERROR_I18N("Set property '%{public}s' failed", name.c_str());
        return;
    }
}

void VariableConverter::SetNumberMember(ani_env *env, ani_object obj, const std::string &name, const std::string &value)
{
    if (value.empty()) {
        return;
    }

    static const char *className = "Lstd/core/Int;";
    ani_class cls;
    if (ANI_OK != env->FindClass(className, &cls)) {
        HILOG_ERROR_I18N("Find class '%{public}s' failed", className);
        return;
    }

    ani_method ctor;
    if (ANI_OK != env->Class_FindMethod(cls, "<ctor>", "I:V", &ctor)) {
        HILOG_ERROR_I18N("Find method '<ctor>' failed");
        return;
    }

    int32_t status = 0;
    ani_int digits = ConvertString2Int(value, status);
    if (status != 0) {
        return;
    }

    ani_object intObj;
    if (ANI_OK != env->Object_New(cls, ctor, &intObj, digits)) {
        HILOG_ERROR_I18N("New object '%{public}s' failed", className);
        return;
    }

    if (ANI_OK != env->Object_SetPropertyByName_Ref(obj, name.c_str(), intObj)) {
        HILOG_ERROR_I18N("Set property '%{public}s' failed", name.c_str());
        return;
    }
}

bool VariableConverter::GetNumberMember(ani_env *env, ani_object options, const std::string &name, int& value)
{
    ani_ref ref;
    if (ANI_OK != env->Object_GetPropertyByName_Ref(options, name.c_str(), &ref)) {
        HILOG_ERROR_I18N("Get property '%{public}s' failed", name.c_str());
        return false;
    }

    ani_boolean isUndefined;
    if (ANI_OK != env->Reference_IsUndefined(ref, &isUndefined)) {
        HILOG_ERROR_I18N("Reference IsUndefined failed");
        return false;
    }
    if (isUndefined) {
        return false;
    }

    ani_double valueDouble;
    if (ANI_OK != env->Object_CallMethodByName_Double(static_cast<ani_object>(ref), "unboxed", ":D", &valueDouble)) {
        HILOG_ERROR_I18N("Unboxed Double failed");
        return false;
    }
    value = static_cast<int>(valueDouble);
    return true;
}

void VariableConverter::SetStringMember(ani_env *env, ani_object obj, const std::string &name, const std::string &value)
{
    if (value.empty()) {
        return;
    }

    if (ANI_OK != env->Object_SetPropertyByName_Ref(obj, name.c_str(), VariableConverter::StringToAniStr(env, value))) {
        HILOG_ERROR_I18N("Set property '%{public}s' failed", name.c_str());
        return;
    }
}

bool VariableConverter::GetStringMember(ani_env *env, ani_object options, const std::string &name, std::string &value)
{
    ani_ref ref;
    if (ANI_OK != env->Object_GetPropertyByName_Ref(options, name.c_str(), &ref)) {
        HILOG_ERROR_I18N("Get property '%{public}s' failed", name.c_str());
        return false;
    }

    ani_boolean isUndefined;
    if (ANI_OK != env->Reference_IsUndefined(ref, &isUndefined)) {
        HILOG_ERROR_I18N("Reference IsUndefined failed");
        return false;
    }
    if (isUndefined) {
        return false;
    }

    value = VariableConverter::AniStrToString(env, ref);
    return true;
}

ani_object VariableConverter::CreateArray(ani_env *env, const std::vector<std::string> &strs)
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
    if (ANI_OK != env->Object_New(cls, ctor, &ret, strs.size())) {
        HILOG_ERROR_I18N("New object '%{public}s' failed", className);
        return nullptr;
    }

    ani_method set;
    if (ANI_OK != env->Class_FindMethod(cls, "$_set", "ILstd/core/Object;:V", &set)) {
        HILOG_ERROR_I18N("Find method '$_set' failed");
        return ret;
    }

    for (size_t i = 0; i < strs.size(); ++i) {
        if (ANI_OK != env->Object_CallMethod_Void(ret, set, i, VariableConverter::StringToAniStr(env, strs[i]))) {
            HILOG_ERROR_I18N("Call method '$_set' failed");
            return ret;
        }
    }
    return ret;
}

bool VariableConverter::ParseStringArray(ani_env *env, ani_object obj, std::vector<std::string> &strs)
{
    static const char *className = "Lescompat/Array;";
    ani_class cls;
    if (ANI_OK != env->FindClass(className, &cls)) {
        HILOG_ERROR_I18N("Find class '%{public}s' failed", className);
        return false;
    }

    ani_method getLengthMethod;
    if (ANI_OK != env->Class_FindGetter(cls, "length", &getLengthMethod)) {
        HILOG_ERROR_I18N("Find getter 'length' failed");
        return false;
    }

    ani_double length;
    if (ANI_OK != env->Object_CallMethod_Double(obj, getLengthMethod, &length)) {
        HILOG_ERROR_I18N("Get 'length' failed");
        return false;
    }

    ani_method get;
    if (ANI_OK != env->Class_FindMethod(cls, "$_get", "I:Lstd/core/Object;", &get)) {
        HILOG_ERROR_I18N("Find method '$_get' failed");
        return false;
    }

    for (int i = 0; i < static_cast<int>(length); ++i) {
        ani_ref value;
        if (ANI_OK != env->Object_CallMethod_Ref(obj, get, &value, i)) {
            HILOG_ERROR_I18N("Call method '$_get' failed");
            return false;
        }
        strs.push_back(VariableConverter::AniStrToString(env, static_cast<ani_string>(value)));
    }
    return true;
}

ani_enum_item VariableConverter::GetEnumItemByIndex(ani_env* env, const char* enumClassName, const int index)
{
    ani_enum aniEnum;
    if (ANI_OK != env->FindEnum(enumClassName, &aniEnum)) {
        HILOG_ERROR_I18N("Find enum '%{public}s' failed", enumClassName);
        return nullptr;
    }

    ani_enum_item enumItem;
    if (ANI_OK != env->Enum_GetEnumItemByIndex(aniEnum, index, &enumItem)) {
        HILOG_ERROR_I18N("Get enumItem '%{public}s' failed", enumClassName);
        return nullptr;
    }
    return enumItem;
}

ani_double VariableConverter::GetDateValue(ani_env *env, ani_object date, const std::string &method)
{
    static const char* className = "Lescompat/Date;";
    ani_class cls;
    if (ANI_OK != env->FindClass(className, &cls)) {
        HILOG_ERROR_I18N("Find class '%{public}s' failed", className);
        return -1;
    }

    ani_method get;
    if (ANI_OK != env->Class_FindMethod(cls, method.c_str(), nullptr, &get)) {
        HILOG_ERROR_I18N("Find method '%{public}s' failed", method.c_str());
        return -1;
    }

    ani_double ret;
    if (ANI_OK != env->Object_CallMethod_Double(date, get, &ret)) {
        HILOG_ERROR_I18N("Call method '%{public}s' failed", method.c_str());
        return -1;
    }
    return ret;
}

std::vector<std::string> VariableConverter::GetLocaleTags(ani_env *env, ani_object locale)
{
    std::vector<std::string> localeTags;
    ani_class stringClass;
    if (ANI_OK != env->FindClass("Lstd/core/String;", &stringClass)) {
        HILOG_ERROR_I18N("Find class 'Lstd/core/String' failed");
        return localeTags;
    }

    ani_boolean isString;
    if (ANI_OK != env->Object_InstanceOf(locale, stringClass, &isString)) {
        HILOG_ERROR_I18N("Get Instance failed");
        return localeTags;
    }
    if (isString) {
        localeTags.push_back(AniStrToString(env, locale));
        return localeTags;
    }
    static const char *className = "Lescompat/Array;";
    ani_class cls;
    if (ANI_OK != env->FindClass(className, &cls)) {
        HILOG_ERROR_I18N("Find class '%{public}s' failed", className);
        return localeTags;
    }

    ani_method getLengthMethod;
    if (ANI_OK != env->Class_FindGetter(cls, "length", &getLengthMethod)) {
        HILOG_ERROR_I18N("Find getter 'length' failed");
        return localeTags;
    }

    ani_double length;
    if (ANI_OK != env->Object_CallMethod_Double(locale, getLengthMethod, &length)) {
        HILOG_ERROR_I18N("Get 'length' failed");
        return localeTags;
    }

    ani_method get;
    if (ANI_OK != env->Class_FindMethod(cls, "$_get", "I:Lstd/core/Object;", &get)) {
        HILOG_ERROR_I18N("Find method '$_get' failed");
        return localeTags;
    }

    for (int i = 0; i < static_cast<int>(length); ++i) {
        ani_ref value;
        if (ANI_OK != env->Object_CallMethod_Ref(locale, get, &value, i)) {
            HILOG_ERROR_I18N("Call method '$_get' failed");
            return localeTags;
        }
        localeTags.push_back(AniStrToString(env, value));
    }
    return localeTags;
}

template<typename T>
ani_object VariableConverter::CreateAniObject(ani_env* env, const char* className, T* ptr)
{
    ani_class cls;
    if (ANI_OK != env->FindClass(className, &cls)) {
        HILOG_ERROR_I18N("Find class '%{public}s' failed", className);
        return nullptr;
    }

    ani_method ctor;
    if (ANI_OK != env->Class_FindMethod(cls, "<ctor>", "J:V", &ctor)) {
        HILOG_ERROR_I18N("Find method '<ctor>' failed");
        return nullptr;
    }

    ani_object obj;
    if (ANI_OK != env->Object_New(cls, ctor, &obj, reinterpret_cast<ani_long>(ptr))) {
        HILOG_ERROR_I18N("New object '%{public}s' failed", className);
        return nullptr;
    }
    return obj;
}

template ani_object VariableConverter::CreateAniObject<I18nBreakIteratorAddon>(
    ani_env*, const char*, I18nBreakIteratorAddon*);
template ani_object VariableConverter::CreateAniObject<I18nCalendarAddon>(ani_env*, const char*, I18nCalendarAddon*);
template ani_object VariableConverter::CreateAniObject<I18nEntityRecognizerAddon>(
    ani_env*, const char*, I18nEntityRecognizerAddon*);
template ani_object VariableConverter::CreateAniObject<I18nHolidayManagerAddon>(
    ani_env*, const char*, I18nHolidayManagerAddon*);
template ani_object VariableConverter::CreateAniObject<I18nIndexUtilAddon>(ani_env*, const char*, I18nIndexUtilAddon*);
template ani_object VariableConverter::CreateAniObject<I18nNormalizerAddon>(
    ani_env*, const char*, I18nNormalizerAddon*);
template ani_object VariableConverter::CreateAniObject<I18nPhoneNumberFormatAddon>(
    ani_env*, const char*, I18nPhoneNumberFormatAddon*);
template ani_object VariableConverter::CreateAniObject<I18nSimpleDateTimeFormatAddon>(
    ani_env*, const char*, I18nSimpleDateTimeFormatAddon*);
template ani_object VariableConverter::CreateAniObject<I18nSimpleNumberFormatAddon>(
    ani_env*, const char*, I18nSimpleNumberFormatAddon*);
template ani_object VariableConverter::CreateAniObject<I18nSysLocaleMgrAddon>(
    ani_env*, const char*, I18nSysLocaleMgrAddon*);
template ani_object VariableConverter::CreateAniObject<I18nTimeZoneAddon>(ani_env*, const char*, I18nTimeZoneAddon*);
template ani_object VariableConverter::CreateAniObject<I18nTransliteratorAddon>(
    ani_env*, const char*, I18nTransliteratorAddon*);
template ani_object VariableConverter::CreateAniObject<IntlAddon>(ani_env*, const char*, IntlAddon*);
template ani_object VariableConverter::CreateAniObject<LocaleInfoAddon>(ani_env*, const char*, LocaleInfoAddon*);
template ani_object VariableConverter::CreateAniObject<NumberFormatAddon>(ani_env*, const char*, NumberFormatAddon*);
