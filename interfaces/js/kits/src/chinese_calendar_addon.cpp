/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include <unordered_set>
#include "chinese_calendar_addon.h"
#include "error_util.h"
#include "i18n_hilog.h"
#include "locale_config.h"
#include "js_lifecycle_managers.h"
#include "js_utils.h"
#include "variable_convertor.h"

namespace OHOS {
namespace Global {
namespace I18n {

static thread_local ThreadReference* g_constructorCalendar = nullptr;

ChineseCalendarAddon::ChineseCalendarAddon() {}

ChineseCalendarAddon::~ChineseCalendarAddon() {}

void ChineseCalendarAddon::Destructor(napi_env env, void *nativeObject, void *hint)
{
    if (!nativeObject) {
        return;
    }
    delete reinterpret_cast<ChineseCalendarAddon *>(nativeObject);
    nativeObject = nullptr;
}

napi_value ChineseCalendarAddon::InitChineseCalendar(napi_env env, napi_value exports)
{
    HandleScope scope(env, "InitChineseCalendar");
    if (!scope.IsOpen()) {
        return nullptr;
    }

    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("setChineseCalendarTime", SetChineseCalendarTime),
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
        DECLARE_NAPI_FUNCTION("compareDays", CompareDays),
        DECLARE_NAPI_STATIC_FUNCTION("checkLeapMonth", CheckLeapMonth),
    };
    napi_value calendarConstructor = nullptr;
    napi_status status = napi_define_class(env, "ChineseCalendar", NAPI_AUTO_LENGTH, Constructor, nullptr,
        sizeof(properties) / sizeof(napi_property_descriptor), properties, &calendarConstructor);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("InitChineseCalendar: Failed to define class ChineseCalendar at Init");
        return nullptr;
    }
    status = napi_set_named_property(env, exports, "ChineseCalendar", calendarConstructor);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("InitChineseCalendar: Set property failed");
        return nullptr;
    }
    g_constructorCalendar = ThreadReference::CreateInstance(env, calendarConstructor, 1);
    if (!g_constructorCalendar) {
        HILOG_ERROR_I18N("InitChineseCalendar: Failed to create ref at init ChineseCalendar");
        return nullptr;
    }
    return exports;
}

napi_value ChineseCalendarAddon::GetChineseCalendar(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = { nullptr };
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("GetChineseCalendar: napi_get_cb_info failed");
        return nullptr;
    }
    if (g_constructorCalendar == nullptr) {
        HILOG_ERROR_I18N("GetChineseCalendar: g_constructorCalendar is nullptr");
        return nullptr;
    }
    napi_value constructor = nullptr;
    if (!g_constructorCalendar->GetReferenceValue(env, &constructor)) {
        HILOG_ERROR_I18N("GetChineseCalendar: GetReferenceValue failed");
        return nullptr;
    }
    napi_value result = nullptr;
    status = napi_new_instance(env, constructor, argc, argv, &result);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("GetChineseCalendar: napi_new_instance failed");
        return nullptr;
    }
    return result;
}

napi_value ChineseCalendarAddon::Constructor(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = { nullptr };
    napi_value thisVar = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("ChineseCalendarAddon::Constructorr: Get cb info failed");
        return nullptr;
    }

    if (argc > 0 && VariableConvertor::GetLocaleType(env, argv[0]) != LocaleType::BUILTINS_LOCALE) {
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, "locale", "Intl.Locale", true);
        return nullptr;
    }
    std::string localeTag = (argc > 0) ? VariableConvertor::ParseBuiltinsLocale(env, argv[0]) : "";
    if (localeTag.empty()) {
        localeTag = LocaleConfig::GetEffectiveLocale();
    }

    std::unique_ptr<ChineseCalendarAddon> obj = std::make_unique<ChineseCalendarAddon>();
    status = napi_wrap_s(env, thisVar, reinterpret_cast<void *>(obj.get()),
        ChineseCalendarAddon::Destructor, nullptr, &TYPE_TAG, nullptr);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("ChineseCalendarAddon::Constructor: Wrap ChineseCalendarAddon failed");
        return nullptr;
    }
    if (!obj->InitContext(env, localeTag)) {
        HILOG_ERROR_I18N("ChineseCalendarAddon::Constructor: InitContext failed");
        return nullptr;
    }
    obj.release();
    return thisVar;
}

bool ChineseCalendarAddon::InitContext(napi_env env, const std::string& localeTag)
{
    chineseCalendar_ = std::make_shared<ChineseCalendar>(localeTag);
    return true;
}

napi_value ChineseCalendarAddon::SetChineseCalendarTime(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = { 0 };
    napi_value thisVar = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("SetChineseCalendarTime: napi_get_cb_info failed");
        return nullptr;
    }
    if (argc < 1) {
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, "chineseCalendarTime", "", true);
        return nullptr;
    }
    ChineseCalendarTime time = ParseChineseCalendarTime(env, argv[0]);

    ChineseCalendarAddon* obj = nullptr;
    status = napi_unwrap_s(env, thisVar, &TYPE_TAG, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->chineseCalendar_) {
        HILOG_ERROR_I18N("SetChineseCalendarTime: unwrap ChineseCalendarAddon failed.");
        return nullptr;
    }
    I18nErrorCode errCode = I18nErrorCode::SUCCESS;
    obj->chineseCalendar_->SetChineseCalendarTime(time, errCode);
    if (errCode != I18nErrorCode::SUCCESS) {
        ErrorUtil::NapiThrow(env, I18N_NOT_VALID_NEW, "gregorianYear or cyclicalYear", "a valid value", true);
    }
    return nullptr;
}

ChineseCalendarTime ChineseCalendarAddon::ParseChineseCalendarTime(napi_env env, napi_value value)
{
    ChineseCalendarTime time;
    napi_valuetype type = napi_undefined;
    napi_status status = napi_typeof(env, value, &type);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("ParseChineseCalendarTime: Get value type failed");
        return time;
    }
    if (type != napi_object) {
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, "chineseCalendarTime", "a valid ChineseCalendarTime", true);
        return time;
    }

    std::unordered_set<std::string> keyOfTypeNumber
        = { "gregorianYear", "cyclicalYear", "month", "date", "hour", "minute", "second" };
    std::string valueOfKey;
    for (const auto& key : keyOfTypeNumber) {
        if (!JSUtils::GetPropertyFormObject(env, value, key, napi_number, valueOfKey)) {
            continue;
        }
        auto iter = ChineseCalendarTime::MEMBER_NAME_MAP.find(key);
        if (iter != ChineseCalendarTime::MEMBER_NAME_MAP.end()) {
            time.*(iter->second) = std::atoi(valueOfKey.c_str());
        }
    }
    std::string isLeapMonth;
    if (JSUtils::GetPropertyFormObject(env, value, "isLeapMonth", napi_boolean, isLeapMonth)) {
        time.isLeapMonth = (isLeapMonth == "true");
    }
    return time;
}

napi_value ChineseCalendarAddon::SetTime(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = { 0 };
    napi_value thisVar = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("ChineseCalendarAddon::SetTime: napi_get_cb_info failed");
        return nullptr;
    }
    if (argc < 1) {
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, "time or date", "", true);
        return nullptr;
    }
    ChineseCalendarAddon *obj = nullptr;
    status = napi_unwrap_s(env, thisVar, &TYPE_TAG, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->chineseCalendar_) {
        HILOG_ERROR_I18N("ChineseCalendarAddon::SetTime: unwrap failed");
        return nullptr;
    }

    DateType type = VariableConvertor::GetDateType(env, argv[0]);
    if (type != DateType::DATE_OBJECT && type != DateType::NUMBER) {
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, "time or date", "number or Date", true);
        return nullptr;
    }

    double milliseconds = 0;
    if (!VariableConvertor::GetTimeFromDate(env, argv[0], type, milliseconds)) {
        std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
        );
        milliseconds = ms.count();
    }
    obj->chineseCalendar_->SetTime(milliseconds);
    return nullptr;
}

napi_value ChineseCalendarAddon::Set(napi_env env, napi_callback_info info)
{
    size_t argc = 6;
    std::vector<int32_t> param(argc, 0);
    napi_value argv[6] = { 0 };
    napi_value thisVar = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("ChineseCalendarAddon::Set: napi_get_cb_info failed");
        return nullptr;
    }
    if (argc < 3) { // 3 mandatory parameters
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, "date", "", true);
        return nullptr;
    }

    ChineseCalendarAddon *obj = nullptr;
    status = napi_unwrap_s(env, thisVar, &TYPE_TAG, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->chineseCalendar_) {
        HILOG_ERROR_I18N("ChineseCalendarAddon::SetTime: unwrap failed");
        return nullptr;
    }

    for (size_t i = 0; i < argc; i++) {
        if (napi_get_value_int32(env, argv[i], &(param[i])) != napi_ok) {
            HILOG_ERROR_I18N("ChineseCalendarAddon::SetTime: Get int failed");
            return nullptr;
        }
    }

    obj->chineseCalendar_->Set(param[0], param[1], param[2]); // 0/1/2 is the index of year/month/day
    obj->chineseCalendar_->Set(UCAL_HOUR_OF_DAY, param[3]); // 3 is the index of hour
    obj->chineseCalendar_->Set(UCAL_MINUTE, param[4]); // 4 is the index of minute
    obj->chineseCalendar_->Set(UCAL_SECOND, param[5]); // 5 is the index of second
    return nullptr;
}

napi_value ChineseCalendarAddon::GetTimeZone(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_status status = napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("ChineseCalendarAddon::GetTimeZone: napi_get_cb_info failed");
        return VariableConvertor::CreateString(env, "");
    }

    ChineseCalendarAddon *obj = nullptr;
    status = napi_unwrap_s(env, thisVar, &TYPE_TAG, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->chineseCalendar_) {
        HILOG_ERROR_I18N("ChineseCalendarAddon::GetTimeZone: unwrap failed");
        return VariableConvertor::CreateString(env, "");
    }
    std::string timezone = obj->chineseCalendar_->GetTimeZone();
    return VariableConvertor::CreateString(env, timezone);
}

napi_value ChineseCalendarAddon::SetTimeZone(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = { nullptr };
    napi_value thisVar = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("ChineseCalendarAddon::SetTimeZone: napi_get_cb_info failed");
        return nullptr;
    }
    if (argc < 1) {
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, "timezone", "", true);
        return nullptr;
    }

    napi_valuetype valueType = napi_valuetype::napi_undefined;
    status = napi_typeof(env, argv[0], &valueType);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("ChineseCalendarAddon::SetTimeZone: Get type failed");
        return nullptr;
    }
    if (valueType != napi_valuetype::napi_string) {
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, "timezone", "string", true);
        return nullptr;
    }

    int32_t code = 0;
    std::string timezone = VariableConvertor::GetString(env, argv[0], code);
    if (code != 0) {
        HILOG_ERROR_I18N("ChineseCalendarAddon::SetTimeZone: Get timezone failed");
        return nullptr;
    }

    ChineseCalendarAddon *obj = nullptr;
    status = napi_unwrap_s(env, thisVar, &TYPE_TAG, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->chineseCalendar_) {
        HILOG_ERROR_I18N("ChineseCalendarAddon::SetTimeZone: unwrap failed");
        return nullptr;
    }
    obj->chineseCalendar_->SetTimeZone(timezone);
    return nullptr;
}

napi_value ChineseCalendarAddon::GetFirstDayOfWeek(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_status status = napi_get_cb_info(env, info, 0, nullptr, &thisVar, nullptr);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("ChineseCalendarAddon::GetFirstDayOfWeek: napi_get_cb_info failed");
        return VariableConvertor::CreateNumber(env, 0);
    }

    ChineseCalendarAddon *obj = nullptr;
    status = napi_unwrap_s(env, thisVar, &TYPE_TAG, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->chineseCalendar_) {
        HILOG_ERROR_I18N("ChineseCalendarAddon::GetFirstDayOfWeek: unwrap failed");
        return VariableConvertor::CreateNumber(env, 0);
    }
    int32_t firstDay = obj->chineseCalendar_->GetFirstDayOfWeek();
    return VariableConvertor::CreateNumber(env, firstDay);
}

napi_value ChineseCalendarAddon::SetFirstDayOfWeek(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = { nullptr };
    napi_value thisVar = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("ChineseCalendarAddon::SetFirstDayOfWeek: napi_get_cb_info failed");
        return nullptr;
    }
    if (argc < 1) {
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, "value", "", true);
        return nullptr;
    }

    napi_valuetype valueType = napi_valuetype::napi_undefined;
    status = napi_typeof(env, argv[0], &valueType);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("ChineseCalendarAddon::SetFirstDayOfWeek: Get type failed");
        return nullptr;
    }
    if (valueType != napi_valuetype::napi_number) {
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, "value", "number", true);
        return nullptr;
    }

    int32_t value = 0;
    if (napi_get_value_int32(env, argv[0], &value) != napi_ok) {
        HILOG_ERROR_I18N("ChineseCalendarAddon::SetFirstDayOfWeek: napi_get_value_int32 failed");
        return nullptr;
    }
    ChineseCalendarAddon *obj = nullptr;
    status = napi_unwrap_s(env, thisVar, &TYPE_TAG, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->chineseCalendar_) {
        HILOG_ERROR_I18N("ChineseCalendarAddon::SetTimeZone: unwrap failed");
        return VariableConvertor::CreateNumber(env, 0);
    }
    obj->chineseCalendar_->SetFirstDayOfWeek(value);
    return nullptr;
}

napi_value ChineseCalendarAddon::GetMinimalDaysInFirstWeek(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_status status = napi_get_cb_info(env, info, 0, nullptr, &thisVar, nullptr);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("ChineseCalendarAddon::GetFirstDayOfWeek: napi_get_cb_info failed");
        return VariableConvertor::CreateNumber(env, 0);
    }

    ChineseCalendarAddon *obj = nullptr;
    status = napi_unwrap_s(env, thisVar, &TYPE_TAG, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->chineseCalendar_) {
        HILOG_ERROR_I18N("ChineseCalendarAddon::SetTimeZone: unwrap failed");
        return VariableConvertor::CreateNumber(env, 0);
    }
    int32_t minimalDays = obj->chineseCalendar_->GetMinimalDaysInFirstWeek();
    return VariableConvertor::CreateNumber(env, minimalDays);
}

napi_value ChineseCalendarAddon::SetMinimalDaysInFirstWeek(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = { nullptr };
    napi_value thisVar = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("ChineseCalendarAddon::SetFirstDayOfWeek: napi_get_cb_info failed");
        return nullptr;
    }
    if (argc < 1) {
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, "value", "", true);
        return nullptr;
    }

    napi_valuetype valueType = napi_valuetype::napi_undefined;
    status = napi_typeof(env, argv[0], &valueType);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("ChineseCalendarAddon::SetFirstDayOfWeek: Get type failed");
        return nullptr;
    }
    if (valueType != napi_valuetype::napi_number) {
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, "value", "number", true);
        return nullptr;
    }

    int32_t value = 0;
    if (napi_get_value_int32(env, argv[0], &value) != napi_ok) {
        HILOG_ERROR_I18N("ChineseCalendarAddon::SetFirstDayOfWeek: napi_get_value_int32 failed");
        return nullptr;
    }
    ChineseCalendarAddon *obj = nullptr;
    status = napi_unwrap_s(env, thisVar, &TYPE_TAG, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->chineseCalendar_) {
        HILOG_ERROR_I18N("ChineseCalendarAddon::SetTimeZone: unwrap failed");
        return VariableConvertor::CreateNumber(env, 0);
    }
    obj->chineseCalendar_->SetMinimalDaysInFirstWeek(value);
    return nullptr;
}

napi_value ChineseCalendarAddon::Get(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = { nullptr };
    napi_value thisVar = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("ChineseCalendarAddon::Get: napi_get_cb_info failed");
        return VariableConvertor::CreateNumber(env, 0);
    }
    if (argc < 1) {
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, "field", "", true);
        return VariableConvertor::CreateNumber(env, 0);
    }

    napi_valuetype valueType = napi_valuetype::napi_undefined;
    status = napi_typeof(env, argv[0], &valueType);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("ChineseCalendarAddon::Get: Get type failed");
        return VariableConvertor::CreateNumber(env, 0);
    }
    if (valueType != napi_valuetype::napi_string) {
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, "field", "string", true);
        return VariableConvertor::CreateNumber(env, 0);
    }

    int32_t code = 0;
    std::string field = VariableConvertor::GetString(env, argv[0], code);
    if (code != 0) {
        HILOG_ERROR_I18N("ChineseCalendarAddon::Get: Get field failed");
        return VariableConvertor::CreateNumber(env, 0);
    }
    ChineseCalendarAddon *obj = nullptr;
    status = napi_unwrap_s(env, thisVar, &TYPE_TAG, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->chineseCalendar_) {
        HILOG_ERROR_I18N("ChineseCalendarAddon::Get: unwrap failed");
        return VariableConvertor::CreateNumber(env, 0);
    }
    if (I18nCalendar::FIELDS_MAP.find(field) == I18nCalendar::FIELDS_MAP.end()) {
        HILOG_ERROR_I18N("ChineseCalendarAddon::Get: Invalid field %{public}s", field.c_str());
        return VariableConvertor::CreateNumber(env, 0);
    }
    int32_t value = obj->chineseCalendar_->Get(I18nCalendar::FIELDS_MAP.at(field));
    return VariableConvertor::CreateNumber(env, value);
}

napi_value ChineseCalendarAddon::Add(napi_env env, napi_callback_info info)
{
    size_t argc = 2;
    napi_value argv[2] = { nullptr };
    napi_value thisVar = nullptr;
    if (napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr) != napi_ok) {
        HILOG_ERROR_I18N("ChineseCalendarAddon::Add: napi_get_cb_info failed");
        return nullptr;
    }
    if (argc < 2) { // parameter number is 2
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, "field or amount", "", true);
        return nullptr;
    }

    napi_valuetype valueType = napi_valuetype::napi_undefined;
    if (napi_typeof(env, argv[0], &valueType) != napi_ok) {
        HILOG_ERROR_I18N("ChineseCalendarAddon::Add: Get field type failed");
        return nullptr;
    } else if (valueType != napi_valuetype::napi_string) {
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, "field", "string", true);
        return nullptr;
    }
    int32_t code = 0;
    std::string field = VariableConvertor::GetString(env, argv[0], code);
    if (code != 0) {
        return nullptr;
    }

    if (napi_typeof(env, argv[1], &valueType) != napi_ok) {
        HILOG_ERROR_I18N("ChineseCalendarAddon::Add: Get amount type failed");
        return nullptr;
    } else if (valueType != napi_valuetype::napi_number) {
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, "amount", "number", true);
        return nullptr;
    }
    int32_t amount = 0;
    if (napi_get_value_int32(env, argv[1], &amount) != napi_ok) {
        HILOG_ERROR_I18N("ChineseCalendarAddon::Add: napi_get_value_int32 failed");
        return nullptr;
    }
    ChineseCalendarAddon *obj = nullptr;
    napi_status status = napi_unwrap_s(env, thisVar, &TYPE_TAG, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->chineseCalendar_) {
        HILOG_ERROR_I18N("ChineseCalendarAddon::Add: unwrap failed");
        return nullptr;
    }
    if (I18nCalendar::FIELDS_IN_FUNCTION_ADD.find(field) != I18nCalendar::FIELDS_IN_FUNCTION_ADD.end() &&
        I18nCalendar::FIELDS_MAP.find(field) != I18nCalendar::FIELDS_MAP.end()) {
        obj->chineseCalendar_->Add(I18nCalendar::FIELDS_MAP.at(field), amount);
    }
    return nullptr;
}

napi_value ChineseCalendarAddon::GetDisplayName(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = { 0 };
    napi_value thisVar = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("ChineseCalendarAddon::GetDisplayName: napi_get_cb_info failed");
        return VariableConvertor::CreateString(env, "");
    }
    if (argc < 1) {
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, "locale", "", true);
        return VariableConvertor::CreateString(env, "");
    }

    napi_valuetype valueType = napi_valuetype::napi_undefined;
    if (napi_typeof(env, argv[0], &valueType) != napi_ok) {
        HILOG_ERROR_I18N("ChineseCalendarAddon::GetDisplayName: Get locale type failed");
        return VariableConvertor::CreateString(env, "");
    }
    if (valueType != napi_valuetype::napi_string) {
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, "locale", "string", true);
        return VariableConvertor::CreateString(env, "");
    }
    int32_t code = 0;
    std::string locale = VariableConvertor::GetString(env, argv[0], code);
    if (code != 0) {
        HILOG_ERROR_I18N("ChineseCalendarAddon::GetDisplayName: Get locale failed");
        return VariableConvertor::CreateString(env, "");
    }

    ChineseCalendarAddon *obj = nullptr;
    status = napi_unwrap_s(env, thisVar, &TYPE_TAG, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->chineseCalendar_) {
        HILOG_ERROR_I18N("ChineseCalendarAddon::GetDisplayName: unwrap failed");
        return VariableConvertor::CreateString(env, "");
    }
    std::string displayName = obj->chineseCalendar_->GetDisplayName(locale);
    return VariableConvertor::CreateString(env, displayName);
}

napi_value ChineseCalendarAddon::GetTimeInMillis(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_status status = napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("ChineseCalendarAddon::GetTimeInMillis: napi_get_cb_info failed");
        return nullptr;
    }

    ChineseCalendarAddon *obj = nullptr;
    status = napi_unwrap_s(env, thisVar, &TYPE_TAG, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->chineseCalendar_) {
        HILOG_ERROR_I18N("ChineseCalendarAddon::GetTimeInMillis: unwrap failed");
        return nullptr;
    }
    return VariableConvertor::CreateInt64Value(env, obj->chineseCalendar_->GetTimeInMillis());
}

napi_value ChineseCalendarAddon::IsWeekend(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = { 0 };
    napi_value thisVar = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("IsWeekend: napi_get_cb_info failed");
        return VariableConvertor::CreateBoolean(env, false);
    }
    double milliseconds = 0;
    if (argc > 0) {
        DateType type = VariableConvertor::GetDateType(env, argv[0]);
        if (type != DateType::DATE_OBJECT) {
            ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, "date", "Date", true);
            return VariableConvertor::CreateBoolean(env, false);
        }
        if (!VariableConvertor::GetTimeFromDate(env, argv[0], type, milliseconds)) {
            std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()
            );
            milliseconds = ms.count();
        }
    }

    ChineseCalendarAddon *obj = nullptr;
    status = napi_unwrap_s(env, thisVar, &TYPE_TAG, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->chineseCalendar_) {
        HILOG_ERROR_I18N("ChineseCalendarAddon::IsWeekend: unwrap failed");
        return VariableConvertor::CreateBoolean(env, false);
    }
    UErrorCode error = U_ZERO_ERROR;
    bool isWeekend = (argc > 0) ? obj->chineseCalendar_->IsWeekend(milliseconds, error) :
        obj->chineseCalendar_->IsWeekend();
    return VariableConvertor::CreateBoolean(env, isWeekend);
}

napi_value ChineseCalendarAddon::CompareDays(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = { 0 };
    napi_value thisVar = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("CompareDays: napi_get_cb_info failed");
        return VariableConvertor::CreateNumber(env, 0);
    }
    if (argc < 1) {
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, "date", "", true);
        return VariableConvertor::CreateNumber(env, 0);
    }

    DateType type = VariableConvertor::GetDateType(env, argv[0]);
    if (type != DateType::DATE_OBJECT) {
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, "date", "Date", true);
        return VariableConvertor::CreateNumber(env, 0);
    }
    double milliseconds;
    if (!VariableConvertor::GetTimeFromDate(env, argv[0], type, milliseconds)) {
        std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
        );
        milliseconds = ms.count();
    }

    ChineseCalendarAddon *obj = nullptr;
    status = napi_unwrap_s(env, thisVar, &TYPE_TAG, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->chineseCalendar_) {
        HILOG_ERROR_I18N("ChineseCalendarAddon::IsWeekend: unwrap failed");
        return VariableConvertor::CreateNumber(env, 0);
    }

    int32_t days = obj->chineseCalendar_->CompareDays(milliseconds);
    return VariableConvertor::CreateNumber(env, days);
}

napi_value ChineseCalendarAddon::CheckLeapMonth(napi_env env, napi_callback_info info)
{
    size_t argc = 3;
    napi_value argv[3] = { 0 };
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("CheckLeapMonth: napi_get_cb_info failed");
        return VariableConvertor::CreateBoolean(env, false);
    }
    if (argc < 3) { // parameter number is 3
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, "gregorianYear, cyclicalYear or month", "", true);
        return VariableConvertor::CreateBoolean(env, false);
    }

    int32_t gregorianYear = 0;
    if (napi_get_value_int32(env, argv[0], &gregorianYear) != napi_ok) {
        HILOG_ERROR_I18N("CheckLeapMonth: get gregorianYear failed");
        return VariableConvertor::CreateBoolean(env, false);
    }
    int32_t cyclicalYear = 0;
    if (napi_get_value_int32(env, argv[1], &cyclicalYear) != napi_ok) {
        HILOG_ERROR_I18N("CheckLeapMonth: get cyclicalYear failed");
        return VariableConvertor::CreateBoolean(env, false);
    }

    int32_t month = 0;
    if (napi_get_value_int32(env, argv[2], &month) != napi_ok) { // 2 is month index
        HILOG_ERROR_I18N("CheckLeapMonth: get month failed");
        return VariableConvertor::CreateBoolean(env, false);
    }
    I18nErrorCode errCode = I18nErrorCode::SUCCESS;
    bool isLeapMonth = ChineseCalendar::CheckLeapMonth(gregorianYear, cyclicalYear, month, errCode);
    if (errCode != I18nErrorCode::SUCCESS) {
        ErrorUtil::NapiThrow(env, I18N_NOT_VALID_NEW, "gregorianYear or cyclicalYear", "a valid value", true);
        return VariableConvertor::CreateBoolean(env, false);
    }
    return VariableConvertor::CreateBoolean(env, isLeapMonth);
}
} // namespace I18n
} // namespace Global
} // namespace OHOS