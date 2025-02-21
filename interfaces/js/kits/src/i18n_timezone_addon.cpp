/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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
#include "character.h"
#include "error_util.h"
#include "hilog/log.h"
#include "i18n_timezone_addon.h"
#include "js_utils.h"
#include "variable_convertor.h"

namespace OHOS {
namespace Global {
namespace I18n {
static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, 0xD001E00, "I18nTimeZoneAddonJs" };
using namespace OHOS::HiviewDFX;

static thread_local napi_ref* g_timezoneConstructor = nullptr;

I18nTimeZoneAddon::I18nTimeZoneAddon() {}

I18nTimeZoneAddon::~I18nTimeZoneAddon() {}

void I18nTimeZoneAddon::Destructor(napi_env env, void *nativeObject, void *hint)
{
    if (!nativeObject) {
        return;
    }
    delete reinterpret_cast<I18nTimeZoneAddon *>(nativeObject);
    nativeObject = nullptr;
}

napi_value I18nTimeZoneAddon::GetI18nTimeZone(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = { nullptr };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (!VariableConvertor::CheckNapiValueType(env, argv[0])) {
        napi_create_string_utf8(env, "", NAPI_AUTO_LENGTH, &argv[0]);
    }
    return StaticGetTimeZone(env, argv, true);
}

napi_value I18nTimeZoneAddon::InitI18nTimeZone(napi_env env, napi_value exports)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("getID", GetID),
        DECLARE_NAPI_FUNCTION("getDisplayName", GetTimeZoneDisplayName),
        DECLARE_NAPI_FUNCTION("getRawOffset", GetRawOffset),
        DECLARE_NAPI_FUNCTION("getOffset", GetOffset),
    };
    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, "TimeZone", NAPI_AUTO_LENGTH, I18nTimeZoneConstructor, nullptr,
        sizeof(properties) / sizeof(napi_property_descriptor), properties, &constructor);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "InitI18nTimeZone: Failed to define class TimeZone at Init");
        return nullptr;
    }
    exports = I18nTimeZoneAddon::InitTimeZone(env, exports);
    g_timezoneConstructor = new (std::nothrow) napi_ref;
    if (!g_timezoneConstructor) {
        HiLog::Error(LABEL, "InitI18nTimeZone: Failed to create TimeZone ref at init");
        return nullptr;
    }
    status = napi_create_reference(env, constructor, 1, g_timezoneConstructor);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "InitI18nTimeZone: Failed to create reference g_timezoneConstructor at init");
        return nullptr;
    }
    return exports;
}

napi_value I18nTimeZoneAddon::InitTimeZone(napi_env env, napi_value exports)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_STATIC_FUNCTION("getAvailableIDs", GetAvailableTimezoneIDs),
        DECLARE_NAPI_STATIC_FUNCTION("getAvailableZoneCityIDs", GetAvailableZoneCityIDs),
        DECLARE_NAPI_STATIC_FUNCTION("getCityDisplayName", GetCityDisplayName),
        DECLARE_NAPI_STATIC_FUNCTION("getTimezoneFromCity", GetTimezoneFromCity),
        DECLARE_NAPI_STATIC_FUNCTION("getTimezonesByLocation", GetTimezonesByLocation)
    };
    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, "I18nTimeZone", NAPI_AUTO_LENGTH, JSUtils::DefaultConstructor, nullptr,
        sizeof(properties) / sizeof(napi_property_descriptor), properties, &constructor);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "InitTimeZone: Failed to define class TimeZone.");
        return nullptr;
    }
    status = napi_set_named_property(env, exports, "TimeZone", constructor);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "InitTimeZone: Set property failed When InitTimeZone.");
        return nullptr;
    }
    return exports;
}

napi_value I18nTimeZoneAddon::I18nTimeZoneConstructor(napi_env env, napi_callback_info info)
{
    size_t argc = 2;
    napi_value argv[2] = { nullptr };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        return nullptr;
    }
    std::string zoneID = "";
    napi_valuetype valueType = napi_valuetype::napi_undefined;
    if (argc > 0) {
        napi_typeof(env, argv[0], &valueType);
        if (valueType != napi_valuetype::napi_string) {
            return nullptr;
        }
        int32_t code = 0;
        zoneID = VariableConvertor::GetString(env, argv[0], code);
        if (code != 0) {
            return nullptr;
        }
    }
    if (argc < FUNC_ARGS_COUNT) {
        return nullptr;
    }
    napi_typeof(env, argv[1], &valueType);
    if (valueType != napi_valuetype::napi_boolean) {
        return nullptr;
    }
    bool isZoneID = false;
    status = napi_get_value_bool(env, argv[1], &isZoneID);
    if (status != napi_ok) {
        return nullptr;
    }
    std::unique_ptr<I18nTimeZoneAddon> obj = std::make_unique<I18nTimeZoneAddon>();
    status =
        napi_wrap(env, thisVar, reinterpret_cast<void *>(obj.get()), I18nTimeZoneAddon::Destructor, nullptr, nullptr);
    if (status != napi_ok) {
        return nullptr;
    }
    obj->timezone_ = I18nTimeZone::CreateInstance(zoneID, isZoneID);
    if (!obj->timezone_) {
        return nullptr;
    }
    obj.release();
    return thisVar;
}

napi_value I18nTimeZoneAddon::GetAvailableTimezoneIDs(napi_env env, napi_callback_info info)
{
    I18nErrorCode errorCode = I18nErrorCode::SUCCESS;
    std::set<std::string> timezoneIDs = I18nTimeZone::GetAvailableIDs(errorCode);
    if (errorCode != I18nErrorCode::SUCCESS) {
        return nullptr;
    }
    napi_value result = nullptr;
    napi_status status = napi_create_array_with_length(env, timezoneIDs.size(), &result);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Failed to create array");
        return nullptr;
    }
    size_t index = 0;
    for (std::set<std::string>::iterator it = timezoneIDs.begin(); it != timezoneIDs.end(); ++it) {
        napi_value value = nullptr;
        status = napi_create_string_utf8(env, (*it).c_str(), NAPI_AUTO_LENGTH, &value);
        if (status != napi_ok) {
            HiLog::Error(LABEL, "Failed to create string item");
            return nullptr;
        }
        status = napi_set_element(env, result, index, value);
        if (status != napi_ok) {
            HiLog::Error(LABEL, "Failed to set array item");
            return nullptr;
        }
        ++index;
    }
    return result;
}

napi_value I18nTimeZoneAddon::GetAvailableZoneCityIDs(napi_env env, napi_callback_info info)
{
    std::set<std::string> cityIDs = I18nTimeZone::GetAvailableZoneCityIDs();
    napi_value result = nullptr;
    napi_status status = napi_create_array_with_length(env, cityIDs.size(), &result);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Failed to create array");
        return nullptr;
    }
    size_t index = 0;
    for (auto it = cityIDs.begin(); it != cityIDs.end(); ++it) {
        napi_value value = nullptr;
        status = napi_create_string_utf8(env, (*it).c_str(), NAPI_AUTO_LENGTH, &value);
        if (status != napi_ok) {
            HiLog::Error(LABEL, "GetAvailableZoneCityIDs: Failed to create string item");
            return nullptr;
        }
        status = napi_set_element(env, result, index, value);
        if (status != napi_ok) {
            HiLog::Error(LABEL, "GetAvailableZoneCityIDs: Failed to set array item");
            return nullptr;
        }
        ++index;
    }
    return result;
}

napi_value I18nTimeZoneAddon::GetCityDisplayName(napi_env env, napi_callback_info info)
{
    size_t argc = 2;
    napi_value argv[2] = { 0 };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        return nullptr;
    }
    if (argc < FUNC_ARGS_COUNT) {
        return nullptr;
    }
    napi_valuetype valueType = napi_valuetype::napi_undefined;
    napi_typeof(env, argv[0], &valueType);
    if (valueType != napi_valuetype::napi_string) {
        HiLog::Error(LABEL, "Invalid parameter type");
        return nullptr;
    }
    int32_t code = 0;
    std::string cityID = VariableConvertor::GetString(env, argv[0], code);
    if (code != 0) {
        return nullptr;
    }
    std::string locale = VariableConvertor::GetString(env, argv[1], code);
    if (code != 0) {
        return nullptr;
    }
    std::string name = I18nTimeZone::GetCityDisplayName(cityID, locale);
    napi_value result = nullptr;
    status = napi_create_string_utf8(env, name.c_str(), NAPI_AUTO_LENGTH, &result);
    if (status != napi_ok) {
        return nullptr;
    }
    return result;
}

napi_value I18nTimeZoneAddon::GetTimezoneFromCity(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = { nullptr };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    return StaticGetTimeZone(env, argv, false);
}

napi_value I18nTimeZoneAddon::GetTimezonesByLocation(napi_env env, napi_callback_info info)
{
    size_t argc = 2;
    napi_value argv[2] = {0, 0};
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        return nullptr;
    }
    if (argc < FUNC_ARGS_COUNT) {
        HiLog::Error(LABEL, "GetTimezonesByLocation: Missing parameter");
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, true);
        return nullptr;
    }
    double x;
    double y;
    if (!CheckParamTypeAndScope(env, argv, x, y)) {
        ErrorUtil::NapiThrow(env, I18N_NOT_VALID, true);
        return nullptr;
    }
    napi_value timezoneList = nullptr;
    napi_create_array(env, &timezoneList);
    std::vector<std::string> tempList = I18nTimeZone::GetTimezoneIdByLocation(x, y);
    for (size_t i = 0; i < tempList.size(); i++) {
        napi_value timezoneId = nullptr;
        status = napi_create_string_utf8(env, tempList[i].c_str(), NAPI_AUTO_LENGTH, &timezoneId);
        if (status != napi_ok) {
            return nullptr;
        }
        napi_value argTimeZoneId[1] = { timezoneId };
        napi_value timezone = StaticGetTimeZone(env, argTimeZoneId, true);
        status = napi_set_element(env, timezoneList, i, timezone);
        if (status != napi_ok) {
            return nullptr;
        }
    }

    return timezoneList;
}

bool I18nTimeZoneAddon::CheckParamTypeAndScope(napi_env env, napi_value *argv, double &x, double &y)
{
    napi_status status = napi_get_value_double(env, argv[0], &x);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "GetTimezonesByLocation: Parse first argument x failed");
        return false;
    }
    status = napi_get_value_double(env, argv[1], &y);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "GetTimezonesByLocation: Parse second argument y failed");
        return false;
    }
    // -180 and 179.9 is the scope of longitude, -90 and 89.9 is the scope of latitude
    if (x < -180 || x > 179.9 || y < -90 || y > 89.9) {
        HiLog::Error(LABEL, "GetTimezonesByLocation: Args x or y exceed it's scope.");
        return false;
    }
    return true;
}

napi_value I18nTimeZoneAddon::GetID(napi_env env, napi_callback_info info)
{
    size_t argc = 0;
    napi_value *argv = nullptr;
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    I18nTimeZoneAddon *obj = nullptr;
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->timezone_) {
        HiLog::Error(LABEL, "Get TimeZone object failed");
        return nullptr;
    }
    std::string result = obj->timezone_->GetID();
    napi_value value = nullptr;
    status = napi_create_string_utf8(env, result.c_str(), NAPI_AUTO_LENGTH, &value);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Create result failed");
        return nullptr;
    }
    return value;
}

napi_value I18nTimeZoneAddon::GetTimeZoneDisplayName(napi_env env, napi_callback_info info)
{
    size_t argc = 2;
    napi_value argv[2] = { 0 };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        return nullptr;
    }

    I18nTimeZoneAddon *obj = nullptr;
    status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->timezone_) {
        HiLog::Error(LABEL, "Get TimeZone object failed");
        return nullptr;
    }

    std::string locale;
    bool isDST = false;
    int32_t parameterStatus = GetParameter(env, argv, locale, isDST);

    std::string result;
    if (parameterStatus == -1) {  // -1 represents Invalid parameter.
        napi_throw_type_error(env, nullptr, "Parameter type does not match");
        return nullptr;
    } else if (parameterStatus == 0) {
        result = obj->timezone_->GetDisplayName();
    } else if (parameterStatus == 1) {  // 1 represents one string parameter.
        result = obj->timezone_->GetDisplayName(locale);
    } else if (parameterStatus == 2) {  // 2 represents one boolean parameter.
        result = obj->timezone_->GetDisplayName(isDST);
    } else {
        result = obj->timezone_->GetDisplayName(locale, isDST);
    }

    napi_value value = nullptr;
    status = napi_create_string_utf8(env, result.c_str(), NAPI_AUTO_LENGTH, &value);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Create result failed");
        return nullptr;
    }
    return value;
}

napi_value I18nTimeZoneAddon::GetRawOffset(napi_env env, napi_callback_info info)
{
    size_t argc = 0;
    napi_value *argv = nullptr;
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    I18nTimeZoneAddon *obj = nullptr;
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->timezone_) {
        HiLog::Error(LABEL, "Get TimeZone object failed");
        return nullptr;
    }
    int32_t result = obj->timezone_->GetRawOffset();
    napi_value value = nullptr;
    status = napi_create_int32(env, result, &value);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Create result failed");
        return nullptr;
    }
    return value;
}

napi_value I18nTimeZoneAddon::GetOffset(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = { 0 };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        return nullptr;
    }

    double date = 0;
    if (VariableConvertor::CheckNapiValueType(env, argv[0])) {
        napi_valuetype valueType = napi_valuetype::napi_undefined;
        napi_typeof(env, argv[0], &valueType);
        if (valueType != napi_valuetype::napi_number) {
            HiLog::Error(LABEL, "Invalid parameter type");
            return nullptr;
        }
        status = napi_get_value_double(env, argv[0], &date);
        if (status != napi_ok) {
            HiLog::Error(LABEL, "Get parameter date failed");
            return nullptr;
        }
    } else {
        auto time = std::chrono::system_clock::now();
        auto since_epoch = time.time_since_epoch();
        auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(since_epoch);
        date = (double)millis.count();
    }

    I18nTimeZoneAddon *obj = nullptr;
    status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->timezone_) {
        HiLog::Error(LABEL, "Get TimeZone object failed");
        return nullptr;
    }
    int32_t result = obj->timezone_->GetOffset(date);
    napi_value value = nullptr;
    status = napi_create_int32(env, result, &value);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Create result failed");
        return nullptr;
    }
    return value;
}

napi_value I18nTimeZoneAddon::StaticGetTimeZone(napi_env env, napi_value *argv, bool isZoneID)
{
    napi_value constructor = nullptr;
    napi_status status = napi_get_reference_value(env, *g_timezoneConstructor, &constructor);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Failed to create reference at StaticGetTimeZone");
        return nullptr;
    }
    napi_value newArgv[2] = { 0 };
    newArgv[0] = argv[0];
    status = napi_get_boolean(env, isZoneID, &newArgv[1]);
    if (status != napi_ok) {
        return nullptr;
    }
    napi_value result = nullptr;
    status = napi_new_instance(env, constructor, 2, newArgv, &result); // 2 is parameter num
    if (status != napi_ok) {
        HiLog::Error(LABEL, "StaticGetTimeZone create instance failed");
        return nullptr;
    }
    return result;
}

int32_t I18nTimeZoneAddon::GetParameter(napi_env env, napi_value *argv, std::string &localeStr, bool &isDST)
{
    napi_status status = napi_ok;
    if (VariableConvertor::CheckNapiValueType(env, argv[1])) {
        napi_valuetype valueType0 = napi_valuetype::napi_undefined;
        napi_valuetype valueType1 = napi_valuetype::napi_undefined;
        napi_typeof(env, argv[0], &valueType0);  // 0 represents first parameter
        napi_typeof(env, argv[1], &valueType1);  // 1 represents second parameter
        bool firstParamFlag = VariableConvertor::CheckNapiValueType(env, argv[0]);
        if (valueType1 == napi_valuetype::napi_boolean) {
            status = napi_get_value_bool(env, argv[1], &isDST);
            if (status != napi_ok) {
                return -1;  // -1 represents Invalid parameter.
            } else if (!firstParamFlag) {
                return 2;  // 2 represents one boolean parameter.
            }
            if (valueType0 == napi_valuetype::napi_string &&
                 GetStringFromJS(env, argv[0], localeStr)) {
                return 3;  // 3 represents one string parameter and one bool parameter.
            }
        }
        return -1;  // -1 represents Invalid parameter.
    }
    return GetFirstParameter(env, argv[0], localeStr, isDST);
}

bool I18nTimeZoneAddon::GetStringFromJS(napi_env env, napi_value argv, std::string &jsString)
{
    size_t len = 0;
    napi_status status = napi_get_value_string_utf8(env, argv, nullptr, 0, &len);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Failed to get string length");
        return false;
    }
    std::vector<char> argvBuf(len + 1);
    status = napi_get_value_string_utf8(env, argv, argvBuf.data(), len + 1, &len);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Failed to get string item");
        return false;
    }
    jsString = argvBuf.data();
    return true;
}

int32_t I18nTimeZoneAddon::GetFirstParameter(napi_env env, napi_value value, std::string &localeStr, bool &isDST)
{
    if (!VariableConvertor::CheckNapiValueType(env, value)) {
        return 0;  // 0 represents no parameter.
    } else {
        napi_status status = napi_ok;
        napi_valuetype valueType = napi_valuetype::napi_undefined;
        napi_typeof(env, value, &valueType);
        if (valueType == napi_valuetype::napi_string) {
            bool valid = GetStringFromJS(env, value, localeStr);
            // -1 represents Invalid parameter.
            // 1 represents one string parameter.
            return !valid ? -1 : 1;
        } else if (valueType == napi_valuetype::napi_boolean) {
            status = napi_get_value_bool(env, value, &isDST);
            // -1 represents Invalid parameter.
            // 2 represents one boolean parameter.
            return (status != napi_ok) ? -1 : 2;
        }
        return -1;  // -1 represents Invalid parameter.
    }
}
} // namespace I18n
} // namespace Global
} // namespace OHOS