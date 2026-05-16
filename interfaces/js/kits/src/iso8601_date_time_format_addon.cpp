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

#include "iso8601_date_time_format_addon.h"

#include "error_util.h"
#include "i18n_hilog.h"
#include "js_lifecycle_managers.h"
#include "utils.h"
#include "variable_convertor.h"

using namespace OHOS;
using namespace Global;
using namespace I18n;

ISO8601DateTimeFormatAddon::ISO8601DateTimeFormatAddon() {}

ISO8601DateTimeFormatAddon::~ISO8601DateTimeFormatAddon() {}

void ISO8601DateTimeFormatAddon::Destructor(napi_env env, void *nativeObject, void *hint)
{
    if (!nativeObject) {
        return;
    }
    delete reinterpret_cast<ISO8601DateTimeFormatAddon *>(nativeObject);
    nativeObject = nullptr;
}

std::string ParseTimezone(napi_env env, napi_value timeZone)
{
    napi_valuetype timeZoneType;
    if (napi_typeof(env, timeZone, &timeZoneType) != napi_ok || timeZoneType != napi_object) {
        HILOG_ERROR_I18N("ParseTimezone: Type error");
        return "UTC";
    }
    napi_value getIDFunction = nullptr;
    if (napi_get_named_property(env, timeZone, "getID", &getIDFunction) != napi_ok) {
        HILOG_ERROR_I18N("ParseTimezone: Get function failed");
        return "UTC";
    }
    napi_value timeZoneID = nullptr;
    if (napi_call_function(env, timeZone, getIDFunction, 0, nullptr, &timeZoneID) != napi_ok) {
        HILOG_ERROR_I18N("ParseTimezone: Call function failed");
        return "UTC";
    }
    int32_t code = 0;
    std::string result = VariableConvertor::GetString(env, timeZoneID, code);
    if (code != 0) {
        return "UTC";
    }
    return result;
}

napi_value ISO8601DateTimeFormatAddon::ISO8601DateTimeFormatConstructor(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = { nullptr };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("ISO8601DateTimeFormatConstructor: Get cb info failed.");
        return nullptr;
    }

    // Default values
    std::string dateFormat = "calendar";
    std::string timePrecision = "seconds";
    std::string separatorStyle = "extended";
    std::string timeZone = "UTC";
    bool displayTimeZone = true;

    // Parse options if provided
    if (argc > 0) {
        napi_valuetype valueType = napi_valuetype::napi_undefined;
        status = napi_typeof(env, argv[0], &valueType);
        if (status != napi_ok) {
            return nullptr;
        }

        if (valueType == napi_valuetype::napi_object) {
            // Get dateFormat
            napi_value dateFormatValue;
            if (napi_get_named_property(env, argv[0], "dateFormat", &dateFormatValue) == napi_ok) {
                int32_t code = 0;
                dateFormat = VariableConvertor::GetString(env, dateFormatValue, code);
            }

            // Get timePrecision
            napi_value timePrecisionValue;
            if (napi_get_named_property(env, argv[0], "timePrecision", &timePrecisionValue) == napi_ok) {
                int32_t code = 0;
                timePrecision = VariableConvertor::GetString(env, timePrecisionValue, code);
            }

            // Get separatorStyle
            napi_value separatorStyleValue;
            if (napi_get_named_property(env, argv[0], "separatorStyle", &separatorStyleValue) == napi_ok) {
                int32_t code = 0;
                separatorStyle = VariableConvertor::GetString(env, separatorStyleValue, code);
            }

            // Get timeZone
            napi_value timeZoneValue;
            if (napi_get_named_property(env, argv[0], "timeZone", &timeZoneValue) == napi_ok) {
                timeZone = ParseTimezone(env, timeZoneValue);
            }

            // Get displayTimeZone
            napi_value displayTimeZoneValue;
            if (napi_get_named_property(env, argv[0], "displayTimeZone", &displayTimeZoneValue) == napi_ok) {
                napi_get_value_bool(env, displayTimeZoneValue, &displayTimeZone);
            }
        }
    }

    // Convert string values to enum
    ISO8601DateTimeFormat::DateFormat dateFormatEnum = ISO8601DateTimeFormat::DateFormat::CALENDAR;
    if (dateFormat == "ordinal") {
        dateFormatEnum = ISO8601DateTimeFormat::DateFormat::ORDINAL;
    } else if (dateFormat == "week") {
        dateFormatEnum = ISO8601DateTimeFormat::DateFormat::WEEK;
    }

    ISO8601DateTimeFormat::TimePrecision timePrecisionEnum = ISO8601DateTimeFormat::TimePrecision::SECONDS;
    if (timePrecision == "dateOnly") {
        timePrecisionEnum = ISO8601DateTimeFormat::TimePrecision::DATE_ONLY;
    } else if (timePrecision == "hours") {
        timePrecisionEnum = ISO8601DateTimeFormat::TimePrecision::HOURS;
    } else if (timePrecision == "minutes") {
        timePrecisionEnum = ISO8601DateTimeFormat::TimePrecision::MINUTES;
    } else if (timePrecision == "milliSeconds") {
        timePrecisionEnum = ISO8601DateTimeFormat::TimePrecision::MILLISECONDS;
    }

    ISO8601DateTimeFormat::SeparatorStyle separatorStyleEnum = ISO8601DateTimeFormat::SeparatorStyle::EXTENDED;
    if (separatorStyle == "basic") {
        separatorStyleEnum = ISO8601DateTimeFormat::SeparatorStyle::BASIC;
    }

    std::unique_ptr<ISO8601DateTimeFormatAddon> obj = std::make_unique<ISO8601DateTimeFormatAddon>();
    if (obj == nullptr) {
        HILOG_ERROR_I18N("ISO8601DateTimeFormatConstructor: Create ISO8601DateTimeFormatAddon failed.");
        return nullptr;
    }

    status = napi_wrap_s(env, thisVar, reinterpret_cast<void *>(obj.get()),
        ISO8601DateTimeFormatAddon::Destructor, nullptr, &TYPE_TAG, nullptr);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("ISO8601DateTimeFormatConstructor: wrap ISO8601DateTimeFormatAddon failed");
        return nullptr;
    }

    I18nErrorCode errCode = I18nErrorCode::SUCCESS;
    obj->iso8601DateTimeFormat_ = std::make_shared<ISO8601DateTimeFormat>(
        dateFormatEnum, timePrecisionEnum, separatorStyleEnum, timeZone, displayTimeZone, errCode);

    if (!obj->iso8601DateTimeFormat_ || errCode != I18nErrorCode::SUCCESS) {
        HILOG_ERROR_I18N("ISO8601DateTimeFormatConstructor: Construct ISO8601DateTimeFormat failed.");
        return nullptr;
    }

    obj.release();
    return thisVar;
}

napi_value ISO8601DateTimeFormatAddon::Format(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = { 0 };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("ISO8601DateTimeFormatAddon::Format: get cb info failed.");
        return VariableConvertor::CreateString(env, "");
    }
    if (argc < 1) {
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, "date", "", true);
        return VariableConvertor::CreateString(env, "");
    }
    double milliseconds = 0;
    if (!VariableConvertor::GetDateTime(env, argv[0], milliseconds)) {
        HILOG_ERROR_I18N("ISO8601DateTimeFormatAddon::Format: get date time failed.");
        return VariableConvertor::CreateString(env, "");
    }

    ISO8601DateTimeFormatAddon* obj = nullptr;
    status = napi_unwrap_s(env, thisVar, &TYPE_TAG, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->iso8601DateTimeFormat_) {
        HILOG_ERROR_I18N("ISO8601DateTimeFormatAddon::Format: unwrap ISO8601DateTimeFormatAddon failed.");
        return VariableConvertor::CreateString(env, "");
    }
    std::string formatResult = obj->iso8601DateTimeFormat_->Format(milliseconds);
    if (formatResult.empty()) {
        HILOG_ERROR_I18N("ISO8601DateTimeFormatAddon::Format: format result is empty.");
    }
    return VariableConvertor::CreateString(env, formatResult);
}

napi_value ISO8601DateTimeFormatAddon::InitISO8601DateTimeFormat(napi_env env, napi_value exports)
{
    HandleScope scope(env, "InitISO8601DateTimeFormat");
    if (!scope.IsOpen()) {
        return nullptr;
    }
    napi_property_descriptor properties[] = { DECLARE_NAPI_FUNCTION("format", Format) };

    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, "ISO8601DateTimeFormat", NAPI_AUTO_LENGTH,
        ISO8601DateTimeFormatConstructor, nullptr, sizeof(properties) / sizeof(napi_property_descriptor),
        properties, &constructor);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("InitISO8601DateTimeFormat: Define class failed");
        return nullptr;
    }

    status = napi_set_named_property(env, exports, "ISO8601DateTimeFormat", constructor);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("InitISO8601DateTimeFormat: Set named property failed");
        return nullptr;
    }

    return exports;
}

std::shared_ptr<ISO8601DateTimeFormat> ISO8601DateTimeFormatAddon::GetDateTimeFormat()
{
    return iso8601DateTimeFormat_;
}

napi_status ISO8601DateTimeFormatAddon::UnwrapDateTimeFormat(napi_env env, napi_value value,
    ISO8601DateTimeFormatAddon** dateTimeFormat)
{
    return napi_unwrap_s(env, value, &TYPE_TAG, reinterpret_cast<void **>(dateTimeFormat));
}