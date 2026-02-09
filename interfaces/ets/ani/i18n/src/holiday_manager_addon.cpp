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

#include "holiday_manager_addon.h"

#include "error_util.h"
#include "i18n_hilog.h"
#include "variable_converter.h"

using namespace OHOS;
using namespace Global;
using namespace I18n;

static const int SIZE_OF_LIST = 3;
static const int INDEX_ONE_OF_LIST = 0;
static const int INDEX_TWO_OF_LIST = 1;
static const int INDEX_THREE_OF_LIST = 2;

I18nHolidayManagerAddon* I18nHolidayManagerAddon::UnwrapAddon(ani_env *env, ani_object object)
{
    ani_long ptr;
    if (ANI_OK != env->Object_GetFieldByName_Long(object, "nativeHolidayManager", &ptr)) {
        HILOG_ERROR_I18N("Get Long 'nativeHolidayManager' failed");
        return nullptr;
    }
    return reinterpret_cast<I18nHolidayManagerAddon*>(ptr);
}

ani_object I18nHolidayManagerAddon::Create(ani_env *env, [[maybe_unused]] ani_object object, ani_string icsPath)
{
    std::unique_ptr<I18nHolidayManagerAddon> obj = std::make_unique<I18nHolidayManagerAddon>();
    obj->holidayManager_ = std::make_unique<HolidayManager>(VariableConverter::AniStrToString(env, icsPath).c_str());

    static const char *className = "@ohos.i18n.i18n.HolidayManager";
    ani_object holidayManagerObject = VariableConverter::CreateAniObject(env, className, obj.get());
    obj.release();
    return holidayManagerObject;
}

ani_boolean I18nHolidayManagerAddon::IsHoliday(ani_env *env, ani_object object, ani_object date)
{
    I18nHolidayManagerAddon* obj = UnwrapAddon(env, object);
    if (obj == nullptr || obj->holidayManager_ == nullptr) {
        HILOG_ERROR_I18N("18nHolidayManagerAddon::IsHoliday: Get HolidayManager object failed");
        return false;
    }

    bool res = false;
    ani_boolean isUndefined;
    if (ANI_OK != env->Reference_IsUndefined(date, &isUndefined)) {
        HILOG_ERROR_I18N("18nHolidayManagerAddon::IsHoliday: Check date reference is undefined failed");
        return false;
    }
    if (isUndefined) {
        res = obj->holidayManager_->IsHoliday();
    } else {
        std::vector<int> list;
        list.push_back(VariableConverter::GetDateInt(env, date, "getFullYear"));
        list.push_back(VariableConverter::GetDateInt(env, date, "getMonth") + MONTH_INCREASE_ONE);
        list.push_back(VariableConverter::GetDateInt(env, date, "getDate"));
        if (list.size() == SIZE_OF_LIST) {
            res = obj->holidayManager_->IsHoliday(list.at(INDEX_ONE_OF_LIST),
                list.at(INDEX_TWO_OF_LIST), list.at(INDEX_THREE_OF_LIST));
        }
    }
    return res;
}

static ani_object CreateHolidayLocalName(ani_env *env, HolidayLocalName localName)
{
    static const char *className = "@ohos.i18n.i18n.HolidayLocalNameInner";
    ani_class cls;
    if (ANI_OK != env->FindClass(className, &cls)) {
        HILOG_ERROR_I18N("CreateHolidayLocalName: Find class '%{public}s' failed", className);
        return nullptr;
    }

    ani_method ctor;
    if (ANI_OK != env->Class_FindMethod(cls, "<ctor>", nullptr, &ctor)) {
        HILOG_ERROR_I18N("CreateHolidayLocalName: Find method '<ctor>' failed");
        return nullptr;
    }

    ani_object ret;
    if (ANI_OK != env->Object_New(cls, ctor, &ret)) {
        HILOG_ERROR_I18N("CreateHolidayLocalName: New object '%{public}s' failed", className);
        return nullptr;
    }

    VariableConverter::SetStringMember(env, ret, "language", localName.language);
    VariableConverter::SetStringMember(env, ret, "name", localName.name);
    return ret;
}

static ani_object CreateHolidayLocalNameArray(ani_env *env, std::vector<HolidayLocalName> localNames)
{
    static const char *className = "std.core.Array";
    ani_class cls;
    if (ANI_OK != env->FindClass(className, &cls)) {
        HILOG_ERROR_I18N("CreateHolidayLocalNameArray: Find class '%{public}s' failed", className);
        return nullptr;
    }

    ani_method ctor;
    if (ANI_OK != env->Class_FindMethod(cls, "<ctor>", "i:", &ctor)) {
        HILOG_ERROR_I18N("CreateHolidayLocalNameArray: Find method '<ctor>' failed");
        return nullptr;
    }

    ani_object ret;
    if (ANI_OK != env->Object_New(cls, ctor, &ret, localNames.size())) {
        HILOG_ERROR_I18N("CreateHolidayLocalNameArray: New object '%{public}s' failed", className);
        return nullptr;
    }

    ani_method set;
    if (ANI_OK != env->Class_FindMethod(cls, "$_set", "iY:", &set)) {
        HILOG_ERROR_I18N("CreateHolidayLocalNameArray: Find method '$_set' failed");
        return ret;
    }

    for (size_t i = 0; i < localNames.size(); ++i) {
        if (ANI_OK != env->Object_CallMethod_Void(ret, set, i, CreateHolidayLocalName(env, localNames[i]))) {
            HILOG_ERROR_I18N("CreateHolidayLocalNameArray: Call method '$_set' failed");
            return ret;
        }
    }
    return ret;
}

static ani_object CreateHolidayInfoItem(ani_env *env, HolidayInfoItem item)
{
    static const char *className = "@ohos.i18n.i18n.HolidayInfoItemInner";
    ani_class cls;
    if (ANI_OK != env->FindClass(className, &cls)) {
        HILOG_ERROR_I18N("CreateHolidayInfoItem: Find class '%{public}s' failed", className);
        return nullptr;
    }

    ani_method ctor;
    if (ANI_OK != env->Class_FindMethod(cls, "<ctor>", nullptr, &ctor)) {
        HILOG_ERROR_I18N("CreateHolidayInfoItem: Find method '<ctor>' failed");
        return nullptr;
    }

    ani_object ret;
    if (ANI_OK != env->Object_New(cls, ctor, &ret)) {
        HILOG_ERROR_I18N("CreateHolidayInfoItem: New object '%{public}s' failed", className);
        return nullptr;
    }

    VariableConverter::SetStringMember(env, ret, "baseName", item.baseName);

    if (ANI_OK != env->Object_SetPropertyByName_Double(ret, "year", item.year)) {
        HILOG_ERROR_I18N("CreateHolidayInfoItem: Set property 'year' failed");
    }

    if (ANI_OK != env->Object_SetPropertyByName_Double(ret, "month", item.month)) {
        HILOG_ERROR_I18N("CreateHolidayInfoItem: Set property 'month' failed");
    }

    if (ANI_OK != env->Object_SetPropertyByName_Double(ret, "day", item.day)) {
        HILOG_ERROR_I18N("CreateHolidayInfoItem: Set property 'day' failed");
    }

    if (ANI_OK != env->Object_SetPropertyByName_Ref(ret, "localNames",
        CreateHolidayLocalNameArray(env, item.localNames))) {
        HILOG_ERROR_I18N("CreateHolidayInfoItem: Set property 'localNames' failed");
    }
    return ret;
}

static ani_object GetHolidayInfoItemResult(ani_env *env, std::vector<HolidayInfoItem> itemList)
{
    static const char *className = "std.core.Array";
    ani_class cls;
    if (ANI_OK != env->FindClass(className, &cls)) {
        HILOG_ERROR_I18N("GetHolidayInfoItemResult: Find class '%{public}s' failed", className);
        return nullptr;
    }

    ani_method ctor;
    if (ANI_OK != env->Class_FindMethod(cls, "<ctor>", "i:", &ctor)) {
        HILOG_ERROR_I18N("GetHolidayInfoItemResult: Find method '<ctor>' failed");
        return nullptr;
    }

    ani_object ret;
    if (ANI_OK != env->Object_New(cls, ctor, &ret, itemList.size())) {
        HILOG_ERROR_I18N("GetHolidayInfoItemResult: New object '%{public}s' failed", className);
        return nullptr;
    }

    ani_method set;
    if (ANI_OK != env->Class_FindMethod(cls, "$_set", "iY:", &set)) {
        HILOG_ERROR_I18N("GetHolidayInfoItemResult: Find method '$_set' failed");
        return ret;
    }

    for (size_t i = 0; i < itemList.size(); ++i) {
        if (ANI_OK != env->Object_CallMethod_Void(ret, set, i, CreateHolidayInfoItem(env, itemList[i]))) {
            HILOG_ERROR_I18N("GetHolidayInfoItemResult: Call method '$_set' failed");
            return ret;
        }
    }
    return ret;
}

ani_object I18nHolidayManagerAddon::GetHolidayInfoItemArray(ani_env *env, ani_object object, ani_object year)
{
    I18nHolidayManagerAddon* obj = UnwrapAddon(env, object);
    if (obj == nullptr || obj->holidayManager_ == nullptr) {
        HILOG_ERROR_I18N("GetHolidayInfoItemArray: Get HolidayManager object failed");
        return nullptr;
    }

    std::vector<HolidayInfoItem> itemList;
    ani_boolean isUndefined;
    if (ANI_OK != env->Reference_IsUndefined(year, &isUndefined)) {
        HILOG_ERROR_I18N("GetHolidayInfoItemArray: Check year reference is undefined failed");
        return nullptr;
    }
    if (isUndefined) {
        itemList = obj->holidayManager_->GetHolidayInfoItemArray();
    } else {
        ani_int yearValue;
        if (ANI_OK != env->Object_CallMethodByName_Int(year, "toInt", ":i", &yearValue)) {
            HILOG_ERROR_I18N("Unbox Int failed");
            return nullptr;
        }
        if (yearValue < 0) {
            ErrorUtil::AniThrow(env, I18N_NOT_VALID, "year", "a non-negative number");
            itemList = obj->holidayManager_->GetHolidayInfoItemArray(-1);
        } else {
            itemList = obj->holidayManager_->GetHolidayInfoItemArray(yearValue);
        }
    }
    return GetHolidayInfoItemResult(env, itemList);
}

ani_status I18nHolidayManagerAddon::BindContextHolidayManager(ani_env *env)
{
    static const char *className = "@ohos.i18n.i18n.HolidayManager";
    ani_class cls;
    if (ANI_OK != env->FindClass(className, &cls)) {
        HILOG_ERROR_I18N("BindContextHolidayManager: Find class '%{public}s' failed", className);
        return ANI_ERROR;
    }

    std::array staticMethods = {
        ani_native_function { "create", nullptr, reinterpret_cast<void *>(I18nHolidayManagerAddon::Create) },
    };
    if (ANI_OK != env->Class_BindStaticNativeMethods(cls, staticMethods.data(), staticMethods.size())) {
        HILOG_ERROR_I18N("BindContextHolidayManager: Cannot bind static native methods to '%{public}s'", className);
        return ANI_ERROR;
    }

    std::array methods = {
        ani_native_function { "isHoliday", nullptr, reinterpret_cast<void *>(I18nHolidayManagerAddon::IsHoliday) },
        ani_native_function { "getHolidayInfoItemArray", nullptr,
            reinterpret_cast<void *>(I18nHolidayManagerAddon::GetHolidayInfoItemArray) },
    };
    if (ANI_OK != env->Class_BindNativeMethods(cls, methods.data(), methods.size())) {
        HILOG_ERROR_I18N("BindContextHolidayManager: Cannot bind native methods to '%{public}s'", className);
        return ANI_ERROR;
    }
    return ANI_OK;
}
