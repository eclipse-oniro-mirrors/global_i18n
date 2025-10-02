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

#include "i18n_timezone_addon.h"

#include "i18n_hilog.h"
#include "variable_converter.h"

using namespace OHOS;
using namespace Global;
using namespace I18n;

I18nTimeZoneAddon* I18nTimeZoneAddon::UnwrapAddon(ani_env *env, ani_object object)
{
    ani_long ptr;
    if (ANI_OK != env->Object_GetFieldByName_Long(object, "nativeTimeZone", &ptr)) {
        HILOG_ERROR_I18N("Get Long 'nativeTimeZone' failed");
        return nullptr;
    }
    return reinterpret_cast<I18nTimeZoneAddon*>(ptr);
}

ani_object I18nTimeZoneAddon::GetTimeZone(ani_env *env, ani_string zoneID)
{
    std::unique_ptr<I18nTimeZoneAddon> obj = std::make_unique<I18nTimeZoneAddon>();
    ani_boolean isUndefined;
    if (ANI_OK != env->Reference_IsUndefined(zoneID, &isUndefined)) {
        HILOG_ERROR_I18N("Reference IsUndefined failed");
        return nullptr;
    }
    std::string zoneIDStr = "";
    if (!isUndefined) {
        zoneIDStr = VariableConverter::AniStrToString(env, zoneID);
    }
    obj->timezone_ = I18nTimeZone::CreateInstance(zoneIDStr, true);

    static const char* className = "L@ohos/i18n/i18n/TimeZone;";
    ani_object timeZoneObject = VariableConverter::CreateAniObject(env, className, obj.get());
    obj.release();
    return timeZoneObject;
}

ani_string I18nTimeZoneAddon::GetID(ani_env *env, ani_object object)
{
    I18nTimeZoneAddon* obj = UnwrapAddon(env, object);
    if (obj == nullptr || obj->timezone_ == nullptr) {
        HILOG_ERROR_I18N("GetID: Get TimeZone object failed");
        return nullptr;
    }

    std::string result = obj->timezone_->GetID();
    return VariableConverter::StringToAniStr(env, result);
}

ani_string I18nTimeZoneAddon::GetDisplayName(ani_env *env, ani_object object, ani_string locale, ani_object isDST)
{
    I18nTimeZoneAddon* obj = UnwrapAddon(env, object);
    if (obj == nullptr || obj->timezone_ == nullptr) {
        HILOG_ERROR_I18N("GetDisplayName: Get TimeZone object failed");
        return nullptr;
    }

    bool localeFlag = false;
    ani_boolean isUndefined;
    if (ANI_OK != env->Reference_IsUndefined(locale, &isUndefined)) {
        HILOG_ERROR_I18N("Reference IsUndefined failed");
        return nullptr;
    }
    if (!isUndefined) {
        localeFlag = true;
    }

    bool DSTFlag = false;
    ani_boolean isDSTBool;
    if (ANI_OK != env->Reference_IsUndefined(isDST, &isUndefined)) {
        HILOG_ERROR_I18N("Reference IsUndefined failed");
        return nullptr;
    }
    if (!isUndefined) {
        DSTFlag = true;
        if (ANI_OK != env->Object_CallMethodByName_Boolean(isDST, "unboxed", ":Z", &isDSTBool)) {
            HILOG_ERROR_I18N("Unbox Boolean failed");
            return nullptr;
        }
    }

    std::string result;
    if (localeFlag && DSTFlag) {
        result = obj->timezone_->GetDisplayName(VariableConverter::AniStrToString(env, locale), isDSTBool);
    }
    if (localeFlag && !DSTFlag) {
        result = obj->timezone_->GetDisplayName(VariableConverter::AniStrToString(env, locale));
    }
    if (!localeFlag && !DSTFlag) {
        result = obj->timezone_->GetDisplayName();
    }
    return VariableConverter::StringToAniStr(env, result);
}

ani_double I18nTimeZoneAddon::GetRawOffset(ani_env *env, ani_object object)
{
    I18nTimeZoneAddon* obj = UnwrapAddon(env, object);
    if (obj == nullptr || obj->timezone_ == nullptr) {
        HILOG_ERROR_I18N("GetRawOffset: Get TimeZone object failed");
        return -1;
    }
    return obj->timezone_->GetRawOffset();
}

ani_double I18nTimeZoneAddon::GetOffset(ani_env *env, ani_object object, ani_object date)
{
    I18nTimeZoneAddon* obj = UnwrapAddon(env, object);
    if (obj == nullptr || obj->timezone_ == nullptr) {
        HILOG_ERROR_I18N("GetOffset: Get TimeZone object failed");
        return -1;
    }

    double dateDouble = 0;
    ani_boolean isUndefined;
    if (ANI_OK != env->Reference_IsUndefined(date, &isUndefined)) {
        HILOG_ERROR_I18N("Reference IsUndefined failed");
        return -1;
    }
    if (!isUndefined) {
        if (ANI_OK != env->Object_CallMethodByName_Double(date, "unboxed", ":D", &dateDouble)) {
            HILOG_ERROR_I18N("Unbox Double failed");
            return -1;
        }
    } else {
        auto time = std::chrono::system_clock::now();
        auto since_epoch = time.time_since_epoch();
        auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(since_epoch);
        dateDouble = static_cast<double>(millis.count());
    }
    return obj->timezone_->GetOffset(dateDouble);
}

ani_object I18nTimeZoneAddon::GetAvailableIDs(ani_env *env, [[maybe_unused]] ani_object object)
{
    I18nErrorCode errorCode = I18nErrorCode::SUCCESS;
    std::set<std::string> timezoneIDs = I18nTimeZone::GetAvailableIDs(errorCode);
    if (errorCode != I18nErrorCode::SUCCESS) {
        return nullptr;
    }

    std::vector<std::string> vector(timezoneIDs.begin(), timezoneIDs.end());
    return VariableConverter::CreateArray(env, vector);
}

ani_object I18nTimeZoneAddon::GetAvailableZoneCityIDs(ani_env *env, [[maybe_unused]] ani_object object)
{
    std::unordered_set<std::string> cityIDs = I18nTimeZone::GetAvailableZoneCityIDs();
    std::vector<std::string> vector(cityIDs.begin(), cityIDs.end());
    return VariableConverter::CreateArray(env, vector);
}

ani_object I18nTimeZoneAddon::GetCityDisplayName(ani_env *env, [[maybe_unused]] ani_object object,
    ani_string cityID, ani_string locale)
{
    std::string cityIDStr = VariableConverter::AniStrToString(env, cityID);
    std::string localeStr = VariableConverter::AniStrToString(env, locale);
    std::string name = I18nTimeZone::GetCityDisplayName(cityIDStr, localeStr);
    return VariableConverter::StringToAniStr(env, name);
}

ani_object I18nTimeZoneAddon::GetTimezoneFromCity(ani_env *env, [[maybe_unused]] ani_object object, ani_string cityID)
{
    std::string cityIDStr = VariableConverter::AniStrToString(env, cityID);
    std::unique_ptr<I18nTimeZoneAddon> obj = std::make_unique<I18nTimeZoneAddon>();
    obj->timezone_ = I18nTimeZone::CreateInstance(cityIDStr, false);

    static const char* className = "L@ohos/i18n/i18n/TimeZone;";
    ani_object timeZone_object = VariableConverter::CreateAniObject(env, className, obj.get());
    obj.release();
    return timeZone_object;
}

ani_object I18nTimeZoneAddon::GetTimezonesByLocation(ani_env *env, [[maybe_unused]] ani_object object,
    ani_double longitude, ani_double latitude)
{
    std::vector<std::string> tempList = I18nTimeZone::GetTimezoneIdByLocation(longitude, latitude);
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
    if (ANI_OK != env->Object_New(cls, ctor, &ret, tempList.size())) {
        HILOG_ERROR_I18N("New object '%{public}s' failed", className);
        return nullptr;
    }

    ani_method set;
    if (ANI_OK != env->Class_FindMethod(cls, "$_set", "ILstd/core/Object;:V", &set)) {
        HILOG_ERROR_I18N("Find method '$_set' failed");
        return nullptr;
    }

    for (size_t i = 0; i < tempList.size(); ++i) {
        ani_object timezone = GetTimezoneFromCity(env, object, VariableConverter::StringToAniStr(env, tempList[i]));
        if (ANI_OK != env->Object_CallMethod_Void(ret, set, i, timezone)) {
            HILOG_ERROR_I18N("Call method '$_set' failed");
            return nullptr;
        }
    }
    return ret;
}

ani_status I18nTimeZoneAddon::BindContextTimeZone(ani_env *env)
{
    static const char *className = "L@ohos/i18n/i18n/TimeZone;";
    ani_class cls;
    if (ANI_OK != env->FindClass(className, &cls)) {
        HILOG_ERROR_I18N("Find class '%{public}s' failed", className);
        return ANI_ERROR;
    }

    std::array methods = {
        ani_native_function { "getID", nullptr, reinterpret_cast<void *>(I18nTimeZoneAddon::GetID) },
        ani_native_function { "getDisplayName", nullptr, reinterpret_cast<void *>(I18nTimeZoneAddon::GetDisplayName) },
        ani_native_function { "getRawOffset", nullptr, reinterpret_cast<void *>(I18nTimeZoneAddon::GetRawOffset) },
        ani_native_function { "getOffset", nullptr, reinterpret_cast<void *>(I18nTimeZoneAddon::GetOffset) },
        ani_native_function { "getAvailableIDs", nullptr,
            reinterpret_cast<void *>(I18nTimeZoneAddon::GetAvailableIDs) },
        ani_native_function { "getAvailableZoneCityIDs", nullptr,
            reinterpret_cast<void *>(I18nTimeZoneAddon::GetAvailableZoneCityIDs) },
        ani_native_function { "getCityDisplayName", nullptr,
            reinterpret_cast<void *>(I18nTimeZoneAddon::GetCityDisplayName) },
        ani_native_function { "getTimezoneFromCity", nullptr,
            reinterpret_cast<void *>(I18nTimeZoneAddon::GetTimezoneFromCity) },
        ani_native_function { "getTimezonesByLocation", nullptr,
            reinterpret_cast<void *>(I18nTimeZoneAddon::GetTimezonesByLocation) },
    };

    if (ANI_OK != env->Class_BindNativeMethods(cls, methods.data(), methods.size())) {
        HILOG_ERROR_I18N("Cannot bind native methods to '%{public}s'", className);
        return ANI_ERROR;
    };

    return ANI_OK;
}
