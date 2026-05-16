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

#include "advanced_measure_format_addon.h"
#include "entity_recognizer_addon.h"
#include "holiday_manager_addon.h"
#include "i18n_addon.h"
#include "i18n_calendar_addon.h"
#include "i18n_hilog.h"
#include "i18n_normalizer_addon.h"
#include "i18n_timezone_addon.h"
#include "intl_addon.h"
#include "number_format_addon.h"
#include "simple_date_time_format_addon.h"
#include "simple_number_format_addon.h"
#include "styled_date_time_format_addon.h"
#include "styled_number_format_addon.h"
#include "system_locale_manager_addon.h"
#include "zone_rules_addon.h"
#include "zone_offset_transition_addon.h"
#include <ani_signature_builder.h>

using namespace OHOS;
using namespace Global;
using namespace I18n;

std::string VariableConverter::AniStrToString(ani_env *env, ani_ref aniStr)
{
    ani_string ani_str = static_cast<ani_string>(aniStr);
    ani_size strSize;
    if (ANI_OK != env->String_GetUTF8Size(ani_str, &strSize)) {
        HILOG_ERROR_I18N("AniStrToString: Get utf8 size failed");
        return "";
    }

    std::vector<char> buffer(strSize + 1);
    ani_size bytes_written = 0;

    if (ANI_OK != env->String_GetUTF8(ani_str, buffer.data(), buffer.size(), &bytes_written)) {
        HILOG_ERROR_I18N("AniStrToString: Create string failed");
        return "";
    }
    return std::string(buffer.data(), bytes_written);
}

ani_string VariableConverter::StringToAniStr(ani_env *env, const std::string &str)
{
    ani_string ret;
    if (ANI_OK != env->String_NewUTF8(str.c_str(), str.size(), &ret)) {
        HILOG_ERROR_I18N("VariableConverter::StringToAniStr: Create ani string failed");
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

    static const char *className = "std.core.Boolean";
    ani_class cls;
    if (ANI_OK != env->FindClass(className, &cls)) {
        HILOG_ERROR_I18N("VariableConverter::SetBooleanMember: Find class '%{public}s' failed", className);
        return;
    }

    ani_method ctor;
    if (ANI_OK != env->Class_FindMethod(cls, "<ctor>", "z:", &ctor)) {
        HILOG_ERROR_I18N("VariableConverter::SetBooleanMember: Find method '<ctor>' failed");
        return;
    }

    ani_boolean valueBool = value == "true";
    ani_object boolObj;
    if (ANI_OK != env->Object_New(cls, ctor, &boolObj, valueBool)) {
        HILOG_ERROR_I18N("VariableConverter::SetBooleanMember: New object '%{public}s' failed", className);
        return;
    }

    if (ANI_OK != env->Object_SetPropertyByName_Ref(obj, name.c_str(), boolObj)) {
        HILOG_ERROR_I18N("VariableConverter::SetBooleanMember: Set property '%{public}s' failed", name.c_str());
        return;
    }
}

bool VariableConverter::GetBooleanMember(ani_env *env, ani_object options, const std::string &name, bool& value)
{
    ani_ref ref;
    if (ANI_OK != env->Object_GetPropertyByName_Ref(options, name.c_str(), &ref)) {
        HILOG_ERROR_I18N("VariableConverter::GetBooleanMember: Get property '%{public}s' failed", name.c_str());
        return false;
    }

    ani_boolean isUndefined;
    if (ANI_OK != env->Reference_IsUndefined(ref, &isUndefined)) {
        HILOG_ERROR_I18N("VariableConverter::GetBooleanMember: Check reference is undefined failed");
        return false;
    }
    if (isUndefined) {
        return false;
    }

    ani_boolean ret;
    if (ANI_OK != env->Object_CallMethodByName_Boolean(static_cast<ani_object>(ref), "toBoolean", ":z", &ret)) {
        HILOG_ERROR_I18N("VariableConverter::GetBooleanMember: Call method by name failed");
        return false;
    }
    value = ret;
    return true;
}

void VariableConverter::SetNumberMember(ani_env *env, ani_object obj, const std::string &name, const ani_int value)
{
    static const char *className = "std.core.Int";
    ani_class cls;
    if (ANI_OK != env->FindClass(className, &cls)) {
        HILOG_ERROR_I18N("VariableConverter::SetNumberMember: Find class '%{public}s' failed", className);
        return;
    }

    ani_method ctor;
    if (ANI_OK != env->Class_FindMethod(cls, "<ctor>", "i:", &ctor)) {
        HILOG_ERROR_I18N("VariableConverter::SetNumberMember: Find method '<ctor>' failed");
        return;
    }

    ani_object intObj;
    if (ANI_OK != env->Object_New(cls, ctor, &intObj, value)) {
        HILOG_ERROR_I18N("VariableConverter::SetNumberMember: New object '%{public}s' failed", className);
        return;
    }

    if (ANI_OK != env->Object_SetPropertyByName_Ref(obj, name.c_str(), intObj)) {
        HILOG_ERROR_I18N("VariableConverter::SetNumberMember: Set property '%{public}s' failed", name.c_str());
        return;
    }
}

void VariableConverter::SetNumberMember(ani_env *env, ani_object obj, const std::string &name, const std::string &value)
{
    if (value.empty()) {
        return;
    }

    static const char *className = "std.core.Int";
    ani_class cls;
    if (ANI_OK != env->FindClass(className, &cls)) {
        HILOG_ERROR_I18N("VariableConverter::SetNumberMember: Find class '%{public}s' failed", className);
        return;
    }

    ani_method ctor;
    if (ANI_OK != env->Class_FindMethod(cls, "<ctor>", "i:", &ctor)) {
        HILOG_ERROR_I18N("VariableConverter::SetNumberMember: Find method '<ctor>' failed");
        return;
    }

    int32_t status = 0;
    ani_int digits = ConvertString2Int(value, status);
    if (status != 0) {
        return;
    }

    ani_object intObj;
    if (ANI_OK != env->Object_New(cls, ctor, &intObj, digits)) {
        HILOG_ERROR_I18N("VariableConverter::SetNumberMember: New object '%{public}s' failed", className);
        return;
    }

    if (ANI_OK != env->Object_SetPropertyByName_Ref(obj, name.c_str(), intObj)) {
        HILOG_ERROR_I18N("VariableConverter::SetNumberMember: Set property '%{public}s' failed", name.c_str());
        return;
    }
}

bool VariableConverter::GetNumberMember(ani_env *env, ani_object options, const std::string &name, int& value)
{
    ani_ref ref;
    if (ANI_OK != env->Object_GetPropertyByName_Ref(options, name.c_str(), &ref)) {
        HILOG_ERROR_I18N("VariableConverter::GetNumberMember: Get property '%{public}s' failed", name.c_str());
        return false;
    }

    ani_boolean isUndefined;
    if (ANI_OK != env->Reference_IsUndefined(ref, &isUndefined)) {
        HILOG_ERROR_I18N("VariableConverter::GetNumberMember: Check reference is undefined failed");
        return false;
    }
    if (isUndefined) {
        return false;
    }

    ani_int valueInt;
    if (ANI_OK != env->Object_CallMethodByName_Int(static_cast<ani_object>(ref), "toInt", ":i", &valueInt)) {
        HILOG_ERROR_I18N("VariableConverter::GetNumberMember: Unbox Double failed");
        return false;
    }
    value = valueInt;
    return true;
}

void VariableConverter::SetStringMember(ani_env *env, ani_object obj, const std::string &name, const std::string &value)
{
    if (value.empty()) {
        return;
    }

    if (ANI_OK != env->Object_SetPropertyByName_Ref(obj, name.c_str(), VariableConverter::StringToAniStr(env, value))) {
        HILOG_ERROR_I18N("VariableConverter::SetStringMember: Set property '%{public}s' failed", name.c_str());
        return;
    }
}

bool VariableConverter::GetStringMember(ani_env *env, ani_object options, const std::string &name, std::string &value)
{
    ani_ref ref;
    if (ANI_OK != env->Object_GetPropertyByName_Ref(options, name.c_str(), &ref)) {
        HILOG_ERROR_I18N("VariableConverter::GetStringMember: Get property '%{public}s' failed", name.c_str());
        return false;
    }

    ani_boolean isUndefined;
    if (ANI_OK != env->Reference_IsUndefined(ref, &isUndefined)) {
        HILOG_ERROR_I18N("VariableConverter::GetStringMember: Check reference is undefined failed");
        return false;
    }
    if (isUndefined) {
        return false;
    }

    value = VariableConverter::AniStrToString(env, ref);
    return true;
}

bool VariableConverter::GetEnumIndexMember(ani_env *env, ani_object options, const std::string &name, int32_t &value)
{
    ani_ref ref;
    if (ANI_OK != env->Object_GetPropertyByName_Ref(options, name.c_str(), &ref)) {
        HILOG_ERROR_I18N("VariableConverter::GetEnumIndexMember: Get property '%{public}s' failed", name.c_str());
        return false;
    }

    ani_boolean isUndefined;
    if (ANI_OK != env->Reference_IsUndefined(ref, &isUndefined)) {
        HILOG_ERROR_I18N("VariableConverter::GetStringMember: Check reference is undefined failed");
        return false;
    }
    if (isUndefined) {
        return false;
    }

    ani_int index;
    if (ANI_OK != env->EnumItem_GetValue_Int(static_cast<ani_enum_item>(ref), &index)) {
        HILOG_ERROR_I18N("VariableConverter::GetStringMember: Get value failed");
    }

    value = index;
    return true;
}

std::string VariableConverter::GetLocaleTagFromBuiltinLocale(ani_env *env, ani_object obj)
{
    static const char *className = "std.core.Intl.Locale";
    ani_class cls;
    auto status = env->FindClass(className, &cls);
    if (status != ANI_OK) {
        HILOG_ERROR_I18N("GetLocaleTagFromBuiltinLocale: Find class Intl.Locale failed: %{public}d", status);
        return "";
    }

    ani_method toString;
    status = env->Class_FindMethod(cls, "toString", ":C{std.core.String}", &toString);
    if (status != ANI_OK) {
        HILOG_ERROR_I18N("GetLocaleTagFromBuiltinLocale: Find method toString failed: %{public}d", status);
        return "";
    }

    ani_ref value;
    status = env->Object_CallMethod_Ref(obj, toString, &value);
    if (status != ANI_OK) {
        HILOG_ERROR_I18N("GetLocaleTagFromBuiltinLocale: Call method toString failed: %{public}d", status);
        return "";
    }
    return VariableConverter::AniStrToString(env, value);
}

ani_object VariableConverter::CreateArray(ani_env *env, const std::vector<std::string> &strs)
{
    static const char *className = "std.core.Array";
    ani_class cls;
    if (ANI_OK != env->FindClass(className, &cls)) {
        HILOG_ERROR_I18N("VariableConverter::CreateArray: Find class '%{public}s' failed", className);
        return nullptr;
    }

    ani_method ctor;
    if (ANI_OK != env->Class_FindMethod(cls, "<ctor>", "i:", &ctor)) {
        HILOG_ERROR_I18N("VariableConverter::CreateArray: Find method '<ctor>' failed");
        return nullptr;
    }

    ani_object ret;
    if (ANI_OK != env->Object_New(cls, ctor, &ret, strs.size())) {
        HILOG_ERROR_I18N("VariableConverter::CreateArray: New object '%{public}s' failed", className);
        return nullptr;
    }

    ani_method set;
    if (ANI_OK != env->Class_FindMethod(cls, "$_set", "iY:", &set)) {
        HILOG_ERROR_I18N("VariableConverter::CreateArray: Find method '$_set' failed");
        return ret;
    }

    for (size_t i = 0; i < strs.size(); ++i) {
        if (ANI_OK != env->Object_CallMethod_Void(ret, set, i, VariableConverter::StringToAniStr(env, strs[i]))) {
            HILOG_ERROR_I18N("VariableConverter::CreateArray: Call method '$_set' failed");
            return ret;
        }
    }
    return ret;
}

bool VariableConverter::ParseStringArray(ani_env *env, ani_object obj, std::vector<std::string> &strs)
{
    static const char *className = "std.core.Array";
    ani_class cls;
    if (ANI_OK != env->FindClass(className, &cls)) {
        HILOG_ERROR_I18N("VariableConverter::ParseStringArray: Find class '%{public}s' failed", className);
        return false;
    }

    ani_method getLengthMethod;
    if (ANI_OK != env->Class_FindMethod(cls, arkts::ani_signature::Builder::BuildGetterName("length").c_str(),
        nullptr, &getLengthMethod)) {
        HILOG_ERROR_I18N("VariableConverter::ParseStringArray: Find getter 'length' failed");
        return false;
    }

    ani_int length;
    if (ANI_OK != env->Object_CallMethod_Int(obj, getLengthMethod, &length)) {
        HILOG_ERROR_I18N("VariableConverter::ParseStringArray: Get 'length' failed");
        return false;
    }

    ani_method get;
    if (ANI_OK != env->Class_FindMethod(cls, "$_get", "i:Y", &get)) {
        HILOG_ERROR_I18N("VariableConverter::ParseStringArray: Find method '$_get' failed");
        return false;
    }

    for (int i = 0; i < static_cast<int>(length); ++i) {
        ani_ref value;
        if (ANI_OK != env->Object_CallMethod_Ref(obj, get, &value, i)) {
            HILOG_ERROR_I18N("VariableConverter::ParseStringArray: Call method '$_get' failed");
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
        HILOG_ERROR_I18N("VariableConverter::GetEnumItemByIndex: Find enum '%{public}s' failed", enumClassName);
        return nullptr;
    }

    ani_enum_item enumItem;
    if (ANI_OK != env->Enum_GetEnumItemByIndex(aniEnum, index, &enumItem)) {
        HILOG_ERROR_I18N("VariableConverter::GetEnumItemByIndex: Get enumItem '%{public}s' failed", enumClassName);
        return nullptr;
    }
    return enumItem;
}

ani_long VariableConverter::GetDateValue(ani_env *env, ani_object date, const std::string &method)
{
    static const char* className = "std.core.Date";
    ani_class cls;
    if (ANI_OK != env->FindClass(className, &cls)) {
        HILOG_ERROR_I18N("VariableConverter::GetDateValue: Find class '%{public}s' failed", className);
        return -1;
    }

    ani_method get;
    if (ANI_OK != env->Class_FindMethod(cls, method.c_str(), nullptr, &get)) {
        HILOG_ERROR_I18N("VariableConverter::GetDateValue: Find method '%{public}s' failed", method.c_str());
        return -1;
    }

    ani_long ret;
    if (ANI_OK != env->Object_CallMethod_Long(date, get, &ret)) {
        HILOG_ERROR_I18N("VariableConverter::GetDateValue: Call method '%{public}s' failed", method.c_str());
        return -1;
    }
    return ret;
}

ani_int VariableConverter::GetDateInt(ani_env *env, ani_object date, const std::string &method)
{
    static const char* className = "std.core.Date";
    ani_class cls;
    if (ANI_OK != env->FindClass(className, &cls)) {
        HILOG_ERROR_I18N("GetDateInt: Find class '%{public}s' failed", className);
        return -1;
    }

    ani_method get;
    if (ANI_OK != env->Class_FindMethod(cls, method.c_str(), nullptr, &get)) {
        HILOG_ERROR_I18N("GetDateInt: Find method '%{public}s' failed", method.c_str());
        return -1;
    }

    ani_int ret;
    if (ANI_OK != env->Object_CallMethod_Int(date, get, &ret)) {
        HILOG_ERROR_I18N("GetDateInt: Call method '%{public}s' failed", method.c_str());
        return -1;
    }
    return ret;
}

std::vector<std::string> VariableConverter::GetLocaleTags(ani_env *env, ani_object locale)
{
    std::vector<std::string> localeTags;
    ani_class stringClass;
    if (ANI_OK != env->FindClass("std.core.String", &stringClass)) {
        HILOG_ERROR_I18N("VariableConverter::GetLocaleTags: Find class 'std.core.String' failed");
        return localeTags;
    }

    ani_boolean isString;
    if (ANI_OK != env->Object_InstanceOf(locale, stringClass, &isString)) {
        HILOG_ERROR_I18N("VariableConverter::GetLocaleTags: Get Instance failed");
        return localeTags;
    }
    if (isString) {
        localeTags.push_back(AniStrToString(env, locale));
        return localeTags;
    }
    static const char *className = "std.core.Array";
    ani_class cls;
    if (ANI_OK != env->FindClass(className, &cls)) {
        HILOG_ERROR_I18N("VariableConverter::GetLocaleTags: Find class '%{public}s' failed", className);
        return localeTags;
    }

    ani_method getLengthMethod;
    if (ANI_OK != env->Class_FindMethod(cls, arkts::ani_signature::Builder::BuildGetterName("length").c_str(),
        nullptr, &getLengthMethod)) {
        HILOG_ERROR_I18N("VariableConverter::GetLocaleTags: Find getter 'length' failed");
        return localeTags;
    }

    ani_int length;
    if (ANI_OK != env->Object_CallMethod_Int(locale, getLengthMethod, &length)) {
        HILOG_ERROR_I18N("VariableConverter::GetLocaleTags: Get 'length' failed");
        return localeTags;
    }

    ani_method get;
    if (ANI_OK != env->Class_FindMethod(cls, "$_get", "i:Y", &get)) {
        HILOG_ERROR_I18N("VariableConverter::GetLocaleTags: Find method '$_get' failed");
        return localeTags;
    }

    for (int i = 0; i < static_cast<int>(length); ++i) {
        ani_ref value;
        if (ANI_OK != env->Object_CallMethod_Ref(locale, get, &value, i)) {
            HILOG_ERROR_I18N("VariableConverter::GetLocaleTags: Call method '$_get' failed");
            return localeTags;
        }
        localeTags.push_back(AniStrToString(env, value));
    }
    return localeTags;
}

ani_object VariableConverter::CreateAniMap(ani_env* env,
    const std::unordered_map<std::string, std::string> &map)
{
    ani_class mapClass;
    if (ANI_OK != env->FindClass("std.core.Map", &mapClass)) {
        HILOG_ERROR_I18N("CreateAniMap: Find class 'std.core.Map' failed");
        return nullptr;
    }
    ani_method ctor;
    if (ANI_OK != env->Class_FindMethod(mapClass, "<ctor>", ":", &ctor)) {
        HILOG_ERROR_I18N("CreateAniMap: Find method '<ctor>' failed");
        return nullptr;
    }
    ani_object obj;
    if (ANI_OK != env->Object_New(mapClass, ctor, &obj)) {
        HILOG_ERROR_I18N("CreateAniMap: New object 'std.core.Map' failed");
        return nullptr;
    }
    ani_ref value;
    for (auto iter = map.begin(); iter != map.end(); ++iter) {
        ani_string key = StringToAniStr(env, iter->first);
        ani_string val = StringToAniStr(env, iter->second);
        ani_object keyObj = static_cast<ani_object>(key);
        ani_object valObj = static_cast<ani_object>(val);
        if (ANI_OK != env->Object_CallMethodByName_Ref(obj, "set",
            "YY:C{std.core.Map}", &value, keyObj, valObj)) {
            HILOG_ERROR_I18N("CreateAniMap: Call method 'set' failed");
            return nullptr;
        }
    }
    return obj;
}

template<typename T>
ani_object VariableConverter::CreateAniObject(ani_env* env, const char* className, T* ptr)
{
    ani_class cls;
    if (ANI_OK != env->FindClass(className, &cls)) {
        HILOG_ERROR_I18N("VariableConverter::CreateAniObject: Find class '%{public}s' failed", className);
        return nullptr;
    }

    ani_method ctor;
    if (ANI_OK != env->Class_FindMethod(cls, "<ctor>", "l:", &ctor)) {
        HILOG_ERROR_I18N("VariableConverter::CreateAniObject: Find method '<ctor>' of %{public}s failed", className);
        return nullptr;
    }

    ani_object obj;
    if (ANI_OK != env->Object_New(cls, ctor, &obj, reinterpret_cast<ani_long>(ptr))) {
        HILOG_ERROR_I18N("VariableConverter::CreateAniObject: New object '%{public}s' failed", className);
        return nullptr;
    }
    return obj;
}

template ani_object VariableConverter::CreateAniObject<AdvancedMeasureFormatAddon>(
    ani_env*, const char*, AdvancedMeasureFormatAddon*);
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
template ani_object VariableConverter::CreateAniObject<I18nStyledDateTimeFormatAddon>(
    ani_env*, const char*, I18nStyledDateTimeFormatAddon*);
template ani_object VariableConverter::CreateAniObject<I18nStyledNumberFormatAddon>(
    ani_env*, const char*, I18nStyledNumberFormatAddon*);
template ani_object VariableConverter::CreateAniObject<I18nSysLocaleMgrAddon>(
    ani_env*, const char*, I18nSysLocaleMgrAddon*);
template ani_object VariableConverter::CreateAniObject<I18nTimeZoneAddon>(ani_env*, const char*, I18nTimeZoneAddon*);
template ani_object VariableConverter::CreateAniObject<I18nTransliteratorAddon>(
    ani_env*, const char*, I18nTransliteratorAddon*);
template ani_object VariableConverter::CreateAniObject<IntlAddon>(ani_env*, const char*, IntlAddon*);
template ani_object VariableConverter::CreateAniObject<NumberFormatAddon>(ani_env*, const char*, NumberFormatAddon*);
template ani_object VariableConverter::CreateAniObject<ZoneRulesAddon>(ani_env*, const char*, ZoneRulesAddon*);
template ani_object VariableConverter::CreateAniObject<ZoneOffsetTransitionAddon>(
    ani_env*, const char*, ZoneOffsetTransitionAddon*);
