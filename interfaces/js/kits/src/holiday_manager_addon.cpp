/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include <vector>
#include "error_util.h"
#include "i18n_hilog.h"
#include "locale_config.h"
#include "variable_convertor.h"
#include "holiday_manager_addon.h"

namespace OHOS {
namespace Global {
namespace I18n {
HolidayManagerAddon::HolidayManagerAddon() : env_(nullptr) {}

HolidayManagerAddon::~HolidayManagerAddon()
{
}

void HolidayManagerAddon::Destructor(napi_env env, void *nativeObject, void *hint)
{
    if (!nativeObject) {
        return;
    }
    delete reinterpret_cast<HolidayManagerAddon *>(nativeObject);
    nativeObject = nullptr;
}

napi_value HolidayManagerAddon::InitHolidayManager(napi_env env, napi_value exports)
{
    napi_status status = napi_ok;
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("isHoliday", IsHoliday),
        DECLARE_NAPI_FUNCTION("getHolidayInfoItemArray", GetHolidayInfoItemArray)
    };

    napi_value constructor = nullptr;
    status = napi_define_class(env, "HolidayManager", NAPI_AUTO_LENGTH, HolidayManagerConstructor, nullptr,
        sizeof(properties) / sizeof(napi_property_descriptor), properties, &constructor);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("Define class failed when InitHolidayManager");
        return nullptr;
    }

    status = napi_set_named_property(env, exports, "HolidayManager", constructor);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("Set property failed when InitHolidayManager");
        return nullptr;
    }
    return exports;
}

napi_value HolidayManagerAddon::HolidayManagerConstructor(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = { 0 };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        return nullptr;
    } else if (argc < 1) {
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, "icsPath", "", true);
        return nullptr;
    }
    napi_valuetype valueType = napi_valuetype::napi_undefined;
    napi_typeof(env, argv[0], &valueType);
    if (valueType != napi_valuetype::napi_string) {
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, "icsPath", "string", true);
        return nullptr;
    }
    std::unique_ptr<HolidayManagerAddon> obj = std::make_unique<HolidayManagerAddon>();
    status =
        napi_wrap(env, thisVar, reinterpret_cast<void *>(obj.get()), HolidayManagerAddon::Destructor, nullptr, nullptr);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("Wrap HolidayManagerAddon failed");
        return nullptr;
    }
    int32_t code = 0;
    std::string path = VariableConvertor::GetString(env, argv[0], code);
    if (code) {
        return nullptr;
    }
    if (!obj->InitHolidayManagerContext(env, info, path.c_str())) {
        HILOG_ERROR_I18N("Init HolidayManager failed");
        return nullptr;
    }
    obj.release();
    return thisVar;
}

bool HolidayManagerAddon::InitHolidayManagerContext(napi_env env, napi_callback_info info, const char* path)
{
    napi_value global = nullptr;
    napi_status status = napi_get_global(env, &global);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("Get global failed");
        return false;
    }
    env_ = env;
    holidayManager_ = std::make_unique<HolidayManager>(path);
    return holidayManager_ != nullptr;
}

napi_value HolidayManagerAddon::IsHoliday(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = { 0 };
    napi_value thisVar = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, NULL);
    if (status != napi_ok) {
        return nullptr;
    }
    HolidayManagerAddon *obj = nullptr;
    status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->holidayManager_) {
        HILOG_ERROR_I18N("IsHoliday: Get HolidayManager object failed");
        return nullptr;
    }
    napi_value result = nullptr;
    bool flag = VariableConvertor::CheckNapiValueType(env, argv[0]);
    bool res = false;
    if (flag) {
        std::vector<int> list = ValidateParamDate(env, argv[0]);
        if (list.size() == 3) { // 3 is the number of parameters
            // 0, 1, and 2 indicate that the first, second, and third parameters of the list are read
            res = obj->holidayManager_->IsHoliday(list.at(0), list.at(1), list.at(2));
        }
    } else {
        res = obj->holidayManager_->IsHoliday();
    }
    status = napi_get_boolean(env, res, &result);
    return result;
}

napi_value HolidayManagerAddon::GetHolidayInfoItemArray(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = { 0 };
    argv[0] = nullptr;
    napi_value thisVar = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, NULL);
    if (status != napi_ok) {
        return nullptr;
    }
    HolidayManagerAddon *obj = nullptr;
    status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->holidayManager_) {
        HILOG_ERROR_I18N("GetHolidayInfoItemArray: Get HolidayManager object failed");
        return nullptr;
    }
    bool flag = VariableConvertor::CheckNapiValueType(env, argv[0]);
    if (flag) {
        int32_t year = ValidateParamNumber(env, argv[0]);
        std::vector<HolidayInfoItem> itemList = obj->holidayManager_->GetHolidayInfoItemArray(year);
        return GetHolidayInfoItemResult(env, itemList);
    } else {
        std::vector<HolidayInfoItem> itemList = obj->holidayManager_->GetHolidayInfoItemArray();
        return GetHolidayInfoItemResult(env, itemList);
    }
}

napi_value HolidayManagerAddon::GetHolidayInfoItemResult(napi_env env, std::vector<HolidayInfoItem> itemList)
{
    napi_value result = nullptr;
    napi_status status = napi_create_array_with_length(env, itemList.size(), &result);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("create HolidayInfoItem array failed.");
        return nullptr;
    }
    for (size_t i = 0; i < itemList.size(); i++) {
        napi_value item = CreateHolidayItem(env, itemList[i]);
        status = napi_set_element(env, result, i, item);
        if (status != napi_ok) {
            HILOG_ERROR_I18N("Failed to set HolidayInfoItem element.");
            return nullptr;
        }
    }
    return result;
}

std::vector<int> HolidayManagerAddon::ValidateParamDate(napi_env &env, napi_value &argv)
{
    std::vector<int> list;
    napi_valuetype valueType = napi_valuetype::napi_undefined;
    napi_typeof(env, argv, &valueType);
    if (valueType != napi_valuetype::napi_object) {
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, "date", "Date object", true);
        return list;
    }
    list.push_back(GetDateValue(env, argv, "getFullYear"));
    list.push_back(GetDateValue(env, argv, "getMonth") + MONTH_INCREASE_ONE);
    list.push_back(GetDateValue(env, argv, "getDate"));
    return list;
}

int HolidayManagerAddon::GetDateValue(napi_env env, napi_value value, const std::string method)
{
    int val = 0;
    if (!value) {
        return val;
    }
    napi_value funcGetDateInfo = nullptr;
    napi_status status = napi_get_named_property(env, value, method.c_str(), &funcGetDateInfo);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("Get method %{public}s failed", method.c_str());
        return val;
    }
    napi_value ret_value = nullptr;
    status = napi_call_function(env, value, funcGetDateInfo, 0, nullptr, &ret_value);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("Get milliseconds failed");
        return val;
    }

    status = napi_get_value_int32(env, ret_value, &val);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("GetDateValue: Retrieve field failed");
        return val;
    }
    return val;
}

int32_t HolidayManagerAddon::ValidateParamNumber(napi_env &env, napi_value &argv)
{
    napi_valuetype valueType = napi_valuetype::napi_undefined;
    napi_typeof(env, argv, &valueType);
    if (valueType != napi_valuetype::napi_number) {
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, "year", "number", true);
        return -1;
    }
    int32_t val = 0;
    napi_status status = napi_get_value_int32(env, argv, &val);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("ValidateParamNumber: Retrieve field failed");
        return -1;
    }
    if (val < 0) {
        ErrorUtil::NapiThrow(env, I18N_NOT_VALID, "year", "a non-negative number", true);
        return -1;
    }
    return val;
}

napi_value HolidayManagerAddon::CreateHolidayItem(napi_env env, const HolidayInfoItem &holidayItem)
{
    napi_value result;
    napi_status status = napi_create_object(env, &result);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("Create Holiday Item object failed.");
        return nullptr;
    }
    status = napi_set_named_property(env, result, "baseName",
        VariableConvertor::CreateString(env, holidayItem.baseName));
    if (status != napi_ok) {
        HILOG_ERROR_I18N("Failed to set element baseName.");
        return nullptr;
    }
    status = napi_set_named_property(env, result, "year",
                                     VariableConvertor::CreateNumber(env, holidayItem.year));
    if (status != napi_ok) {
        HILOG_ERROR_I18N("Failed to set element year.");
        return nullptr;
    }
    status = napi_set_named_property(env, result, "month",
                                     VariableConvertor::CreateNumber(env, holidayItem.month));
    if (status != napi_ok) {
        HILOG_ERROR_I18N("Failed to set element month.");
        return nullptr;
    }
    status = napi_set_named_property(env, result, "day",
                                     VariableConvertor::CreateNumber(env, holidayItem.day));
    if (status != napi_ok) {
        HILOG_ERROR_I18N("Failed to set element day.");
        return nullptr;
    }
    napi_value localNames = HolidayLocalNameItem(env, holidayItem.localNames);
    if (localNames != nullptr) {
        status = napi_set_named_property(env, result, "localNames", localNames);
        if (status != napi_ok) {
            HILOG_ERROR_I18N("Failed to set element localNames.");
            return nullptr;
        }
    }
    return result;
}

napi_value HolidayManagerAddon::HolidayLocalNameItem(napi_env env, const std::vector<HolidayLocalName> localNames)
{
    napi_value result = nullptr;
    napi_status status = napi_create_array_with_length(env, localNames.size(), &result);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("create HolidayLocalName array failed.");
        return nullptr;
    }
    for (size_t i = 0; i < localNames.size(); i++) {
        napi_value localNameItem;
        status = napi_create_object(env, &localNameItem);
        if (status != napi_ok) {
            HILOG_ERROR_I18N("Create HolidayLocalName Item object failed.");
            return nullptr;
        }
        status = napi_set_named_property(env, localNameItem, "language",
                                         VariableConvertor::CreateString(env, localNames[i].language));
        if (status != napi_ok) {
            HILOG_ERROR_I18N("Create HolidayLocalName.language Item object failed.");
            return nullptr;
        }
        status = napi_set_named_property(env, localNameItem, "name",
                                         VariableConvertor::CreateString(env, localNames[i].name));
        if (status != napi_ok) {
            HILOG_ERROR_I18N("Create HolidayLocalName.name Item object failed.");
            return nullptr;
        }
        status = napi_set_element(env, result, i, localNameItem);
        if (status != napi_ok) {
            HILOG_ERROR_I18N("Failed to set HolidayLocalName element.");
            return nullptr;
        }
    }
    return result;
}
} // namespace I18n
} // namespace Global
} // namespace OHOS