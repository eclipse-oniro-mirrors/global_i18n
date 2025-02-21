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

#include <unordered_map>
#include <unordered_set>

#include "error_util.h"
#include "hilog/log.h"
#include "i18n_calendar_addon.h"
#include "js_utils.h"
#include "variable_convertor.h"

namespace OHOS {
namespace Global {
namespace I18n {
static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, 0xD001E00, "I18nCalendarAddonJs" };
using namespace OHOS::HiviewDFX;

static thread_local napi_ref* g_constructor = nullptr;
static std::unordered_map<std::string, UCalendarDateFields> g_fieldsMap {
    { "era", UCAL_ERA },
    { "year", UCAL_YEAR },
    { "month", UCAL_MONTH },
    { "week_of_year", UCAL_WEEK_OF_YEAR },
    { "week_of_month", UCAL_WEEK_OF_MONTH },
    { "date", UCAL_DATE },
    { "day_of_year", UCAL_DAY_OF_YEAR },
    { "day_of_week", UCAL_DAY_OF_WEEK },
    { "day_of_week_in_month", UCAL_DAY_OF_WEEK_IN_MONTH },
    { "ap_pm", UCAL_AM_PM },
    { "hour", UCAL_HOUR },
    { "hour_of_day", UCAL_HOUR_OF_DAY },
    { "minute", UCAL_MINUTE },
    { "second", UCAL_SECOND },
    { "millisecond", UCAL_MILLISECOND },
    { "zone_offset", UCAL_ZONE_OFFSET },
    { "dst_offset", UCAL_DST_OFFSET },
    { "year_woy", UCAL_YEAR_WOY },
    { "dow_local", UCAL_DOW_LOCAL },
    { "extended_year", UCAL_EXTENDED_YEAR },
    { "julian_day", UCAL_JULIAN_DAY },
    { "milliseconds_in_day", UCAL_MILLISECONDS_IN_DAY },
    { "is_leap_month", UCAL_IS_LEAP_MONTH },
};
static std::unordered_set<std::string> g_fieldsInFunctionAdd {
    "year", "month", "date", "hour", "minute", "second", "millisecond",
    "week_of_year", "week_of_month", "day_of_year", "day_of_week",
    "day_of_week_in_month", "hour_of_day", "milliseconds_in_day",
};
static std::unordered_map<std::string, CalendarType> g_typeMap {
    { "buddhist", CalendarType::BUDDHIST },
    { "chinese", CalendarType::CHINESE },
    { "coptic", CalendarType::COPTIC },
    { "ethiopic", CalendarType::ETHIOPIC },
    { "hebrew", CalendarType::HEBREW },
    { "gregory", CalendarType::GREGORY },
    { "indian", CalendarType::INDIAN },
    { "islamic_civil", CalendarType::ISLAMIC_CIVIL },
    { "islamic_tbla", CalendarType::ISLAMIC_TBLA },
    { "islamic_umalqura", CalendarType::ISLAMIC_UMALQURA },
    { "japanese", CalendarType::JAPANESE },
    { "persion", CalendarType::PERSIAN },
};

I18nCalendarAddon::I18nCalendarAddon() {}

I18nCalendarAddon::~I18nCalendarAddon() {}

void I18nCalendarAddon::Destructor(napi_env env, void *nativeObject, void *hint)
{
    if (!nativeObject) {
        return;
    }
    delete reinterpret_cast<I18nCalendarAddon *>(nativeObject);
    nativeObject = nullptr;
}

napi_value I18nCalendarAddon::InitI18nCalendar(napi_env env, napi_value exports)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("setTime", SetTime),
        DECLARE_NAPI_FUNCTION("set", Set),
        DECLARE_NAPI_FUNCTION("getTimeZone", GetTimeZone),
        DECLARE_NAPI_FUNCTION("setTimeZone", SetTimeZone),
        DECLARE_NAPI_FUNCTION("getFirstDayOfWeek", GetFirstDayOfWeek),
        DECLARE_NAPI_FUNCTION("setFirstDayOfWeek", SetFirstDayOfWeek),
        DECLARE_NAPI_FUNCTION("getMinimalDaysInFirstWeek", GetMinimalDaysInFirstWeek),
        DECLARE_NAPI_FUNCTION("setMinimalDaysInFirstWeek", SetMinimalDaysInFirstWeek),
        DECLARE_NAPI_FUNCTION("get", Get),
        DECLARE_NAPI_FUNCTION("add", Add),
        DECLARE_NAPI_FUNCTION("getDisplayName", GetDisplayName),
        DECLARE_NAPI_FUNCTION("getTimeInMillis", GetTimeInMillis),
        DECLARE_NAPI_FUNCTION("isWeekend", IsWeekend),
        DECLARE_NAPI_FUNCTION("compareDays", CompareDays)
    };
    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, "Calendar", NAPI_AUTO_LENGTH, I18nCalendarConstructor, nullptr,
        sizeof(properties) / sizeof(napi_property_descriptor), properties, &constructor);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Failed to define class at Init");
        return nullptr;
    }
    exports = I18nCalendarAddon::InitCalendar(env, exports);
    g_constructor = new (std::nothrow) napi_ref;
    if (!g_constructor) {
        HiLog::Error(LABEL, "Failed to create ref at init");
        return nullptr;
    }
    status = napi_create_reference(env, constructor, 1, g_constructor);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Failed to create reference at init");
        return nullptr;
    }
    return exports;
}

napi_value I18nCalendarAddon::InitCalendar(napi_env env, napi_value exports)
{
    napi_property_descriptor properties[] = {};
    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, "I18nCalendar", NAPI_AUTO_LENGTH, JSUtils::DefaultConstructor, nullptr,
        sizeof(properties) / sizeof(napi_property_descriptor), properties, &constructor);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "InitCalendar: Failed to define class Calendar.");
        return nullptr;
    }
    status = napi_set_named_property(env, exports, "Calendar", constructor);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "InitCalendar: Set property failed When InitCalendar.");
        return nullptr;
    }
    return exports;
}

napi_value I18nCalendarAddon::GetCalendar(napi_env env, napi_callback_info info)
{
    size_t argc = 2; // retrieve 2 arguments
    napi_value argv[2] = { 0 };
    argv[0] = nullptr;
    argv[1] = nullptr;
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    napi_value constructor = nullptr;
    napi_status status = napi_get_reference_value(env, *g_constructor, &constructor);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Failed to create reference at GetCalendar");
        return nullptr;
    }
    napi_valuetype valueType = napi_valuetype::napi_undefined;
    napi_typeof(env, argv[1], &valueType);
    if (valueType != napi_valuetype::napi_string) {
        status = napi_create_string_utf8(env, "", NAPI_AUTO_LENGTH, argv + 1);
        if (status != napi_ok) {
            return nullptr;
        }
    }
    napi_value result = nullptr;
    status = napi_new_instance(env, constructor, 2, argv, &result); // 2 arguments
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Get calendar create instance failed");
        return nullptr;
    }
    return result;
}

napi_value I18nCalendarAddon::I18nCalendarConstructor(napi_env env, napi_callback_info info)
{
    size_t argc = 2;
    napi_value argv[2] = { 0 };
    argv[0] = nullptr;
    argv[1] = nullptr;
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        return nullptr;
    }
    napi_valuetype valueType = napi_valuetype::napi_undefined;
    napi_typeof(env, argv[0], &valueType);
    if (valueType != napi_valuetype::napi_string) {
        napi_throw_type_error(env, nullptr, "Parameter type does not match");
        return nullptr;
    }
    int32_t code = 0;
    std::string localeTag = VariableConvertor::GetString(env, argv[0], code);
    if (code) {
        return nullptr;
    }
    CalendarType type = GetCalendarType(env, argv[1]);
    std::unique_ptr<I18nCalendarAddon> obj = nullptr;
    obj = std::make_unique<I18nCalendarAddon>();
    status =
        napi_wrap(env, thisVar, reinterpret_cast<void *>(obj.get()), I18nCalendarAddon::Destructor, nullptr, nullptr);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "CalendarConstructor: Wrap II18nAddon failed");
        return nullptr;
    }
    if (!obj->InitCalendarContext(env, info, localeTag, type)) {
        return nullptr;
    }
    obj.release();
    return thisVar;
}

napi_value I18nCalendarAddon::SetTime(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = { 0 };
    argv[0] = nullptr;
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (!argv[0]) {
        return nullptr;
    }
    I18nCalendarAddon *obj = nullptr;
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->calendar_) {
        HiLog::Error(LABEL, "Get calendar object failed");
        return nullptr;
    }
    napi_valuetype type = napi_valuetype::napi_undefined;
    status = napi_typeof(env, argv[0], &type);
    if (status != napi_ok) {
        return nullptr;
    }
    if (type == napi_valuetype::napi_number) {
        obj->SetMilliseconds(env, argv[0]);
        return nullptr;
    } else {
        napi_value val = GetDate(env, argv[0]);
        if (!val) {
            return nullptr;
        }
        obj->SetMilliseconds(env, val);
        return nullptr;
    }
}

napi_value I18nCalendarAddon::Set(napi_env env, napi_callback_info info)
{
    size_t argc = 6; // Set may have 6 arguments
    napi_value argv[6] = { 0 };
    for (size_t i = 0; i < argc; ++i) {
        argv[i] = nullptr;
    }
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    napi_valuetype valueType = napi_valuetype::napi_undefined;
    napi_status status = napi_ok;
    int32_t times[3] = { 0 }; // There are at least 3 arguments.
    for (int i = 0; i < 3; ++i) { // There are at least 3 arguments.
        napi_typeof(env, argv[i], &valueType);
        if (valueType != napi_valuetype::napi_number) {
            napi_throw_type_error(env, nullptr, "Parameter type does not match");
            return nullptr;
        }
        status = napi_get_value_int32(env, argv[i], times + i);
        if (status != napi_ok) {
            HiLog::Error(LABEL, "Retrieve time value failed");
            return nullptr;
        }
    }
    I18nCalendarAddon *obj = nullptr;
    status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->calendar_) {
        HiLog::Error(LABEL, "Get calendar object failed");
        return nullptr;
    }
    obj->calendar_->Set(times[0], times[1], times[2]); // 2 is the index of date
    obj->SetField(env, argv[3], UCalendarDateFields::UCAL_HOUR_OF_DAY); // 3 is the index of hour
    obj->SetField(env, argv[4], UCalendarDateFields::UCAL_MINUTE); // 4 is the index of minute
    obj->SetField(env, argv[5], UCalendarDateFields::UCAL_SECOND); // 5 is the index of second
    return nullptr;
}

napi_value I18nCalendarAddon::GetTimeZone(napi_env env, napi_callback_info info)
{
    size_t argc = 0;
    napi_value *argv = nullptr;
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    I18nCalendarAddon *obj = nullptr;
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->calendar_) {
        HiLog::Error(LABEL, "Get calendar object failed");
        return nullptr;
    }
    std::string temp = obj->calendar_->GetTimeZone();
    napi_value result = nullptr;
    status = napi_create_string_utf8(env, temp.c_str(), NAPI_AUTO_LENGTH, &result);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Create timezone string failed");
        return nullptr;
    }
    return result;
}

napi_value I18nCalendarAddon::SetTimeZone(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = { 0 };
    argv[0] = nullptr;
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    napi_valuetype valueType = napi_valuetype::napi_undefined;
    napi_typeof(env, argv[0], &valueType);
    if (valueType != napi_valuetype::napi_string) {
        napi_throw_type_error(env, nullptr, "Parameter type does not match");
        return nullptr;
    }
    size_t len = 0;
    napi_status status = napi_get_value_string_utf8(env, argv[0], nullptr, 0, &len);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Get timezone length failed");
        return nullptr;
    }
    std::vector<char> buf(len + 1);
    status = napi_get_value_string_utf8(env, argv[0], buf.data(), len + 1, &len);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Get timezone failed");
        return nullptr;
    }
    std::string timezone(buf.data());
    I18nCalendarAddon *obj = nullptr;
    status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->calendar_) {
        HiLog::Error(LABEL, "Get calendar object failed");
        return nullptr;
    }
    obj->calendar_->SetTimeZone(timezone);
    return nullptr;
}

napi_value I18nCalendarAddon::GetFirstDayOfWeek(napi_env env, napi_callback_info info)
{
    size_t argc = 0;
    napi_value *argv = nullptr;
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    I18nCalendarAddon *obj = nullptr;
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->calendar_) {
        HiLog::Error(LABEL, "Get calendar object failed");
        return nullptr;
    }
    int32_t temp = obj->calendar_->GetFirstDayOfWeek();
    napi_value result = nullptr;
    status = napi_create_int32(env, temp, &result);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Create int32 failed");
        return nullptr;
    }
    return result;
}

napi_value I18nCalendarAddon::SetFirstDayOfWeek(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = { 0 };
    argv[0] = nullptr;
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    napi_valuetype valueType = napi_valuetype::napi_undefined;
    napi_typeof(env, argv[0], &valueType);
    if (valueType != napi_valuetype::napi_number) {
        napi_throw_type_error(env, nullptr, "Parameter type does not match");
        return nullptr;
    }
    int32_t value = 0;
    napi_status status = napi_get_value_int32(env, argv[0], &value);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Get int32 failed");
        return nullptr;
    }
    I18nCalendarAddon *obj = nullptr;
    status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->calendar_) {
        HiLog::Error(LABEL, "Get calendar object failed");
        return nullptr;
    }
    obj->calendar_->SetFirstDayOfWeek(value);
    return nullptr;
}

napi_value I18nCalendarAddon::GetMinimalDaysInFirstWeek(napi_env env, napi_callback_info info)
{
    size_t argc = 0;
    napi_value *argv = nullptr;
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    I18nCalendarAddon *obj = nullptr;
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->calendar_) {
        HiLog::Error(LABEL, "Get calendar object failed");
        return nullptr;
    }
    int32_t temp = obj->calendar_->GetMinimalDaysInFirstWeek();
    napi_value result = nullptr;
    status = napi_create_int32(env, temp, &result);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Create int32 failed");
        return nullptr;
    }
    return result;
}

napi_value I18nCalendarAddon::SetMinimalDaysInFirstWeek(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = { 0 };
    argv[0] = nullptr;
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    napi_valuetype valueType = napi_valuetype::napi_undefined;
    napi_typeof(env, argv[0], &valueType);
    if (valueType != napi_valuetype::napi_number) {
        napi_throw_type_error(env, nullptr, "Parameter type does not match");
        return nullptr;
    }
    int32_t value = 0;
    napi_status status = napi_get_value_int32(env, argv[0], &value);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Get int32 failed");
        return nullptr;
    }
    I18nCalendarAddon *obj = nullptr;
    status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->calendar_) {
        HiLog::Error(LABEL, "Get calendar object failed");
        return nullptr;
    }
    obj->calendar_->SetMinimalDaysInFirstWeek(value);
    return nullptr;
}

napi_value I18nCalendarAddon::Get(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = { 0 };
    argv[0] = nullptr;
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    napi_valuetype valueType = napi_valuetype::napi_undefined;
    napi_typeof(env, argv[0], &valueType);
    if (valueType != napi_valuetype::napi_string) {
        napi_throw_type_error(env, nullptr, "Parameter type does not match");
        return nullptr;
    }
    size_t len = 0;
    napi_status status = napi_get_value_string_utf8(env, argv[0], nullptr, 0, &len);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Get field length failed");
        return nullptr;
    }
    std::vector<char> buf(len + 1);
    status = napi_get_value_string_utf8(env, argv[0], buf.data(), len + 1, &len);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Get field failed");
        return nullptr;
    }
    std::string field(buf.data());
    if (g_fieldsMap.find(field) == g_fieldsMap.end()) {
        HiLog::Error(LABEL, "Invalid field");
        return nullptr;
    }
    I18nCalendarAddon *obj = nullptr;
    status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->calendar_) {
        HiLog::Error(LABEL, "Get calendar object failed");
        return nullptr;
    }
    int32_t value = obj->calendar_->Get(g_fieldsMap[field]);
    napi_value result = nullptr;
    status = napi_create_int32(env, value, &result);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Create int32 failed");
        return nullptr;
    }
    return result;
}

napi_value I18nCalendarAddon::Add(napi_env env, napi_callback_info info)
{
    size_t argc = 2;
    napi_value argv[2] = { 0 };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "can not obtain add function param.");
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, true);
        return nullptr;
    }
    napi_valuetype valueType = napi_valuetype::napi_undefined;
    napi_typeof(env, argv[0], &valueType);
    if (valueType != napi_valuetype::napi_string) {
        HiLog::Error(LABEL, "Parameter type does not match");
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, true);
        return nullptr;
    }
    int32_t code = 0;
    std::string field = GetAddField(env, argv[0], code);
    if (code) {
        return nullptr;
    }
    napi_typeof(env, argv[1], &valueType);
    if (valueType != napi_valuetype::napi_number) {
        HiLog::Error(LABEL, "Parameter type does not match");
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, true);
        return nullptr;
    }
    int32_t amount;
    status = napi_get_value_int32(env, argv[1], &amount);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Can not obtain add function param.");
        ErrorUtil::NapiThrow(env, I18N_NOT_VALID, true);
        return nullptr;
    }
    I18nCalendarAddon *obj = nullptr;
    status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->calendar_) {
        HiLog::Error(LABEL, "Get calendar object failed");
        return nullptr;
    }
    obj->calendar_->Add(g_fieldsMap[field], amount);
    return nullptr;
}

napi_value I18nCalendarAddon::GetDisplayName(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = { 0 };
    argv[0] = nullptr;
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    napi_valuetype valueType = napi_valuetype::napi_undefined;
    napi_typeof(env, argv[0], &valueType);
    if (valueType != napi_valuetype::napi_string) {
        napi_throw_type_error(env, nullptr, "Parameter type does not match");
        return nullptr;
    }
    int32_t code = 0;
    std::string localeTag = VariableConvertor::GetString(env, argv[0], code);
    if (code) {
        return nullptr;
    }
    I18nCalendarAddon *obj = nullptr;
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->calendar_) {
        HiLog::Error(LABEL, "Get calendar object failed");
        return nullptr;
    }
    if (!obj->calendar_) {
        return nullptr;
    }
    std::string name = obj->calendar_->GetDisplayName(localeTag);
    napi_value result = nullptr;
    status = napi_create_string_utf8(env, name.c_str(), NAPI_AUTO_LENGTH, &result);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Create calendar name string failed");
        return nullptr;
    }
    return result;
}

napi_value I18nCalendarAddon::GetTimeInMillis(napi_env env, napi_callback_info info)
{
    bool flag = true;
    size_t argc = 0;
    napi_value *argv = nullptr;
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    I18nCalendarAddon *obj = nullptr;
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->calendar_) {
        HiLog::Error(LABEL, "Get calendar object failed");
        flag = false;
    }
    UDate temp = 0;
    if (flag) {
        temp = obj->calendar_->GetTimeInMillis();
    }
    napi_value result = nullptr;
    status = napi_create_double(env, temp, &result);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Create UDate failed");
        napi_create_double(env, 0, &result);
    }
    return result;
}

napi_value I18nCalendarAddon::IsWeekend(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = { 0 };
    argv[0] = nullptr;
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    I18nCalendarAddon *obj = nullptr;
    bool isWeekEnd = false;
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&obj));
    do {
        if (status != napi_ok || !obj || !obj->calendar_) {
            HiLog::Error(LABEL, "Get calendar object failed");
            break;
        }
        if (!VariableConvertor::CheckNapiValueType(env, argv[0])) {
            isWeekEnd = obj->calendar_->IsWeekend();
        } else {
            napi_value funcGetDateInfo = nullptr;
            status = napi_get_named_property(env, argv[0], "valueOf", &funcGetDateInfo);
            if (status != napi_ok) {
                HiLog::Error(LABEL, "Get method now failed");
                break;
            }
            napi_value value = nullptr;
            status = napi_call_function(env, argv[0], funcGetDateInfo, 0, nullptr, &value);
            if (status != napi_ok) {
                HiLog::Error(LABEL, "Get milliseconds failed");
                break;
            }
            double milliseconds = 0;
            status = napi_get_value_double(env, value, &milliseconds);
            if (status != napi_ok) {
                HiLog::Error(LABEL, "Retrieve milliseconds failed");
                break;
            }
            UErrorCode error = U_ZERO_ERROR;
            isWeekEnd = obj->calendar_->IsWeekend(milliseconds, error);
        }
    } while (false);
    napi_value result = nullptr;
    status = napi_get_boolean(env, isWeekEnd, &result);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Create boolean failed");
        return nullptr;
    }
    return result;
}

napi_value I18nCalendarAddon::CompareDays(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = { 0 };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    napi_value result = nullptr;
    UDate milliseconds = 0;
    napi_status status = napi_get_date_value(env, argv[0], &milliseconds);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "compareDays function param is not Date");
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, true);
        return nullptr;
    }

    I18nCalendarAddon *obj = nullptr;
    status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->calendar_) {
        HiLog::Error(LABEL, "Get calendar object failed");
        status = napi_create_int32(env, 0, &result); // if error return 0
        return result;
    }
    
    int32_t diff_date = obj->calendar_->CompareDays(milliseconds);
    status = napi_create_int32(env, diff_date, &result);
    return result;
}

bool I18nCalendarAddon::InitCalendarContext(napi_env env, napi_callback_info info, const std::string &localeTag,
    CalendarType type)
{
    calendar_ = std::make_unique<I18nCalendar>(localeTag, type);
    return calendar_ != nullptr;
}

CalendarType I18nCalendarAddon::GetCalendarType(napi_env env, napi_value value)
{
    CalendarType type = CalendarType::UNDEFINED;
    if (value != nullptr) {
        napi_valuetype valueType = napi_valuetype::napi_undefined;
        napi_typeof(env, value, &valueType);
        if (valueType != napi_valuetype::napi_string) {
            napi_throw_type_error(env, nullptr, "Parameter type does not match");
            return type;
        }
        int32_t code = 0;
        std::string calendarType = VariableConvertor::GetString(env, value, code);
        if (code) {
            return type;
        }
        if (g_typeMap.find(calendarType) != g_typeMap.end()) {
            type = g_typeMap[calendarType];
        }
    }
    return type;
}

void I18nCalendarAddon::SetField(napi_env env, napi_value value, UCalendarDateFields field)
{
    if (!VariableConvertor::CheckNapiValueType(env, value)) {
        return;
    }
    int32_t val = 0;
    napi_valuetype valueType = napi_valuetype::napi_undefined;
    napi_typeof(env, value, &valueType);
    if (valueType != napi_valuetype::napi_number) {
        napi_throw_type_error(env, nullptr, "Parameter type does not match");
        return;
    }
    napi_status status = napi_get_value_int32(env, value, &val);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Retrieve field failed");
        return;
    }
    if (calendar_ != nullptr) {
        calendar_->Set(field, val);
    }
}

std::string I18nCalendarAddon::GetAddField(napi_env &env, napi_value &value, int32_t &code)
{
    std::string field = VariableConvertor::GetString(env, value, code);
    if (code != 0) {
        HiLog::Error(LABEL, "can't get string from js array param.");
        ErrorUtil::NapiThrow(env, I18N_NOT_VALID, true);
        return field;
    }
    if (g_fieldsInFunctionAdd.find(field) == g_fieldsInFunctionAdd.end()) {
        code = 1;
        HiLog::Error(LABEL, "Parameter rangs do not match");
        ErrorUtil::NapiThrow(env, I18N_NOT_VALID, true);
        return field;
    }
    return field;
}

napi_value I18nCalendarAddon::GetDate(napi_env env, napi_value value)
{
    if (!value) {
        return nullptr;
    }
    napi_value funcGetDateInfo = nullptr;
    napi_status status = napi_get_named_property(env, value, "valueOf", &funcGetDateInfo);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Get method valueOf failed");
        return nullptr;
    }
    napi_value ret_value = nullptr;
    status = napi_call_function(env, value, funcGetDateInfo, 0, nullptr, &ret_value);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Get milliseconds failed");
        return nullptr;
    }
    return ret_value;
}

void I18nCalendarAddon::SetMilliseconds(napi_env env, napi_value value)
{
    if (!value) {
        return;
    }
    double milliseconds = 0;
    napi_valuetype valueType = napi_valuetype::napi_undefined;
    napi_typeof(env, value, &valueType);
    if (valueType != napi_valuetype::napi_number) {
        napi_throw_type_error(env, nullptr, "Parameter type does not match");
        return;
    }
    napi_status status = napi_get_value_double(env, value, &milliseconds);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Retrieve milliseconds failed");
        return;
    }
    if (calendar_ != nullptr) {
        calendar_->SetTime(milliseconds);
    }
}

} // namespace I18n
} // namespace Global
} // namespace OHOS