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
#include "intl_date_time_format_addon.h"

#include <chrono>
#include <unordered_set>
#include "error_util.h"
#include "i18n_hilog.h"
#include "intl_date_time_format.h"
#include "js_lifecycle_managers.h"
#include "js_utils.h"
#include "utils.h"

namespace OHOS {
namespace Global {
namespace I18n {
static constexpr int NUMBER_OF_PARAMETER = 2;

IntlDateTimeFormatAddon::IntlDateTimeFormatAddon()
{
}

IntlDateTimeFormatAddon::~IntlDateTimeFormatAddon()
{
}

void IntlDateTimeFormatAddon::Destructor(napi_env /* env */, void* nativeObject, void* /* hint */)
{
    if (!nativeObject) {
        return;
    }
    delete reinterpret_cast<IntlDateTimeFormatAddon*>(nativeObject);
    nativeObject = nullptr;
}

napi_value IntlDateTimeFormatAddon::InitIntlDateTimeFormat(napi_env env, napi_value exports)
{
    HandleScope scope(env, "InitIntlDateTimeFormat");
    if (!scope.IsOpen()) {
        return nullptr;
    }

    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("format", Format),
        DECLARE_NAPI_FUNCTION("formatToParts", FormatToParts),
        DECLARE_NAPI_FUNCTION("formatRange", FormatRange),
        DECLARE_NAPI_FUNCTION("formatRangeToParts", FormatRangeToParts),
        DECLARE_NAPI_FUNCTION("toString", ToString),
        DECLARE_NAPI_FUNCTION("resolvedOptions", ResolvedOptions),
        DECLARE_NAPI_STATIC_FUNCTION("supportedLocalesOf", SupportedLocalesOf),
    };

    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, "DateTimeFormat", NAPI_AUTO_LENGTH, IntlDateTimeFormatConstructor,
        nullptr, sizeof(properties) / sizeof(napi_property_descriptor), properties, &constructor);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("IntlDateTimeFormatAddon::InitIntlDateTimeFormat: Define class failed when InitIntlLocale.");
        return nullptr;
    }

    status = napi_set_named_property(env, exports, "DateTimeFormat", constructor);
    if (status != napi_ok) {
        HILOG_ERROR_I18N(
            "IntlDateTimeFormatAddon::InitIntlDateTimeFormat: Set property failed when InitIntlDateTimeFormat.");
        return nullptr;
    }
    return exports;
}

napi_value IntlDateTimeFormatAddon::IntlDateTimeFormatConstructor(napi_env env, napi_callback_info info)
{
    size_t argc = 2;
    napi_value argv[2] = { nullptr };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("IntlDateTimeFormatAddon::IntlDateTimeFormatConstructor: Get cb info failed.");
        return nullptr;
    }

    napi_value new_target;
    status = napi_get_new_target(env, info, &new_target);
    if (status == napi_pending_exception || new_target == nullptr) {
        return CreateDateTimeFormatWithoutNew(env, argc, argv);
    } else if (status != napi_ok) {
        return nullptr;
    }

    std::vector<std::string> localeTags;
    if (argc > 0) {
        int32_t code = 0;
        std::vector<std::string> localeArray = JSUtils::GetLocaleArray(env, argv[0], code);
        localeTags.assign(localeArray.begin(), localeArray.end());
    }

    std::unordered_map<std::string, std::string> configs;
    if (argc > 1) {
        ParseConfigs(env, argv[1], configs);
    }
    IntlDateTimeFormatAddon* obj = new (std::nothrow) IntlDateTimeFormatAddon();
    if (obj == nullptr) {
        HILOG_ERROR_I18N("IntlDateTimeFormatAddon::IntlDateTimeFormatConstructor: Create obj failed.");
        return nullptr;
    }
    status = napi_wrap_s(env, thisVar, reinterpret_cast<void *>(obj), IntlDateTimeFormatAddon::Destructor,
        nullptr, &TYPE_TAG, nullptr);
    if (status != napi_ok) {
        delete obj;
        HILOG_ERROR_I18N(
            "IntlDateTimeFormatAddon::IntlDateTimeFormatConstructor: Wrap IntlDateTimeFormatAddon failed");
        return nullptr;
    }
    if (!obj->InitIntlDateTimeFormatContext(env, localeTags, configs)) {
        HILOG_ERROR_I18N("IntlDateTimeFormatAddon::IntlDateTimeFormatConstructor: Init IntlDateTimeFormat failed");
        return nullptr;
    }
    return thisVar;
}

napi_value IntlDateTimeFormatAddon::CreateDateTimeFormatWithoutNew(napi_env env, size_t argc, napi_value *argv)
{
    napi_value exception;
    napi_status status = napi_get_and_clear_last_exception(env, &exception);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("IntlDateTimeFormatAddon::CreateDateTimeFormatWithoutNew: Clear last exception failed");
        return nullptr;
    }
    napi_value constructor = JSUtils::GetConstructor(env, "DateTimeFormat");
    if (constructor == nullptr) {
        HILOG_ERROR_I18N("CreateDateTimeFormatWithoutNew: get constructor failed");
        return nullptr;
    }
    napi_value instance = nullptr;
    status = napi_new_instance(env, constructor, argc, argv, &instance);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("IntlDateTimeFormatAddon::CreateDateTimeFormatWithoutNew: napi_new_instance failed");
        return nullptr;
    }
    return instance;
}

void IntlDateTimeFormatAddon::ParseConfigs(napi_env env, napi_value options,
    std::unordered_map<std::string, std::string>& configs)
{
    napi_valuetype type = napi_undefined;
    napi_status status = napi_typeof(env, options, &type);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("IntlDateTimeFormatAddon::ParseConfigs: Get options type failed.");
        return;
    }
    if (type != napi_object) {
        HILOG_ERROR_I18N("IntlDateTimeFormatAddon::ParseConfigs: options is not object.");
        return;
    }
    std::string value;
    std::unordered_set<std::string> keyOfTypeString = { IntlDateTimeFormat::LOCALE_MATCHER_TAG,
        IntlDateTimeFormat::WEEK_DAY_TAG, IntlDateTimeFormat::ERA_TAG, IntlDateTimeFormat::YEAR_TAG,
        IntlDateTimeFormat::MONTH_TAG, IntlDateTimeFormat::DAY_TAG, IntlDateTimeFormat::HOUR_TAG,
        IntlDateTimeFormat::MINUTE_TAG, IntlDateTimeFormat::SECOND_TAG, IntlDateTimeFormat::TIME_ZONE_NAME_TAG,
        IntlDateTimeFormat::FORMAT_MATCHER_TAG, IntlDateTimeFormat::TIME_ZONE_TAG, IntlDateTimeFormat::DATE_STYLE_TAG,
        IntlDateTimeFormat::TIME_STYLE_TAG, IntlDateTimeFormat::HOUR_CYCLE_TAG, IntlDateTimeFormat::DAY_PERIOD_TAG,
        IntlDateTimeFormat::CALENDAR_TAG, IntlDateTimeFormat::NUMBERING_SYSTEM_TAG
    };
    for (const auto& key : keyOfTypeString) {
        if (JSUtils::GetPropertyFormObject(env, options, key, napi_string, value)) {
            configs.insert(std::make_pair(key, value));
        }
    }
    if (JSUtils::GetPropertyFormObject(env, options, IntlDateTimeFormat::HOUR12_TAG, napi_boolean, value)) {
        configs.insert({IntlDateTimeFormat::HOUR12_TAG, value});
    }
    if (JSUtils::GetPropertyFormObject(env, options, IntlDateTimeFormat::FRACTIONAL_SECOND_DIGITS_TAG, napi_number,
        value)) {
        configs.insert({IntlDateTimeFormat::FRACTIONAL_SECOND_DIGITS_TAG, value});
    }
}

bool IntlDateTimeFormatAddon::InitIntlDateTimeFormatContext(napi_env env, const std::vector<std::string>& localeTags,
    std::unordered_map<std::string, std::string>& configs)
{
    std::string errMessage;
    intlDateTimeFormat = std::make_unique<IntlDateTimeFormat>(localeTags, configs, errMessage);
    if (!errMessage.empty()) {
        ErrorUtil::NapiThrowUndefined(env, errMessage);
        return false;
    }
    return intlDateTimeFormat != nullptr;
}

napi_value IntlDateTimeFormatAddon::Format(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = { nullptr };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("IntlDateTimeFormatAddon::Format: get cb info failed.");
        return JSUtils::CreateEmptyString(env);
    }
    double milliseconds = 0;
    if (argc < 1 || !GetDateTime(env, argv[0], milliseconds)) {
        std::chrono::milliseconds ms = std::chrono::duration_cast< std::chrono::milliseconds >(
            std::chrono::system_clock::now().time_since_epoch()
        );
        milliseconds = ms.count();
    }

    IntlDateTimeFormatAddon* obj = nullptr;
    status = napi_unwrap_s(env, thisVar, &TYPE_TAG, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->intlDateTimeFormat) {
        HILOG_ERROR_I18N("IntlDateTimeFormatAddon::Format: unwrap IntlDateTimeFormatAddon failed.");
        return JSUtils::CreateEmptyString(env);
    }
    std::string formatResult = obj->intlDateTimeFormat->Format(milliseconds);
    napi_value result = nullptr;
    status = napi_create_string_utf8(env, formatResult.c_str(), NAPI_AUTO_LENGTH, &result);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("IntlDateTimeFormatAddon::::Format: Create string failed.");
        return JSUtils::CreateEmptyString(env);
    }
    return result;
}

napi_value IntlDateTimeFormatAddon::FormatToParts(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = { nullptr };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("IntlDateTimeFormatAddon::FormatToParts: get cb info failed.");
        return JSUtils::CreateEmptyArray(env);
    }
    double milliseconds = 0;
    if (argc < 1 || !GetDateTime(env, argv[0], milliseconds)) {
        std::chrono::milliseconds ms = std::chrono::duration_cast< std::chrono::milliseconds >(
            std::chrono::system_clock::now().time_since_epoch()
        );
        milliseconds = ms.count();
    }

    IntlDateTimeFormatAddon* obj = nullptr;
    status = napi_unwrap_s(env, thisVar, &TYPE_TAG, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->intlDateTimeFormat) {
        HILOG_ERROR_I18N("IntlDateTimeFormatAddon::FormatToParts: unwrap IntlDateTimeFormatAddon failed.");
        return JSUtils::CreateEmptyArray(env);
    }
    std::string errMessage;
    std::vector<std::pair<std::string, std::string>> formatResult =
        obj->intlDateTimeFormat->FormatToParts(milliseconds, errMessage);
    if (!errMessage.empty()) {
        ErrorUtil::NapiThrowUndefined(env, errMessage);
        return nullptr;
    }
    return CreateFormatPart(env, formatResult);
}

napi_value IntlDateTimeFormatAddon::FormatRange(napi_env env, napi_callback_info info)
{
    size_t argc = NUMBER_OF_PARAMETER;
    napi_value argv[NUMBER_OF_PARAMETER] = { nullptr };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("IntlDateTimeFormatAddon::FormatRange: get cb info failed.");
        return JSUtils::CreateEmptyString(env);
    } else if (argc < NUMBER_OF_PARAMETER) {
        ErrorUtil::NapiThrowUndefined(env, "startDate or endDate is undefined");
        return nullptr;
    }

    double start = 0;
    if (!GetDateTime(env, argv[0], start)) {
        HILOG_ERROR_I18N("IntlDateTimeFormatAddon::FormatRange: get date time from argv[0] failed.");
        return JSUtils::CreateEmptyString(env);
    }
    double end = 0;
    if (!GetDateTime(env, argv[1], end)) {
        HILOG_ERROR_I18N("IntlDateTimeFormatAddon::FormatRange: get date time from argv[1] failed.");
        return JSUtils::CreateEmptyString(env);
    }

    IntlDateTimeFormatAddon* obj = nullptr;
    status = napi_unwrap_s(env, thisVar, &TYPE_TAG, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->intlDateTimeFormat) {
        HILOG_ERROR_I18N("IntlDateTimeFormatAddon::FormatRange: unwrap IntlDateTimeFormatAddon failed.");
        return JSUtils::CreateEmptyString(env);
    }
    std::string errMessage;
    std::string formatResult = obj->intlDateTimeFormat->FormatRange(start, end, errMessage);
    if (!errMessage.empty()) {
        ErrorUtil::NapiThrowUndefined(env, errMessage);
        return nullptr;
    }
    napi_value result = nullptr;
    status = napi_create_string_utf8(env, formatResult.c_str(), NAPI_AUTO_LENGTH, &result);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("IntlDateTimeFormatAddon::FormatRange: Create string failed.");
        return JSUtils::CreateEmptyString(env);
    }
    return result;
}

napi_value IntlDateTimeFormatAddon::FormatRangeToParts(napi_env env, napi_callback_info info)
{
    size_t argc = NUMBER_OF_PARAMETER;
    napi_value argv[NUMBER_OF_PARAMETER] = { nullptr };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("IntlDateTimeFormatAddon::FormatRangeToParts: get cb info failed.");
        return JSUtils::CreateEmptyArray(env);
    } else if (argc < NUMBER_OF_PARAMETER) {
        ErrorUtil::NapiThrowUndefined(env, "startDate or endDate is undefined");
        return nullptr;
    }

    double start = 0;
    if (!GetDateTime(env, argv[0], start)) {
        HILOG_ERROR_I18N("IntlDateTimeFormatAddon::FormatRangeToParts: get date time from argv[0] failed.");
        return JSUtils::CreateEmptyArray(env);
    }
    double end = 0;
    if (!GetDateTime(env, argv[1], end)) {
        HILOG_ERROR_I18N("IntlDateTimeFormatAddon::FormatRangeToParts: get date time from argv[1] failed.");
        return JSUtils::CreateEmptyArray(env);
    }

    IntlDateTimeFormatAddon* obj = nullptr;
    status = napi_unwrap_s(env, thisVar, &TYPE_TAG, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->intlDateTimeFormat) {
        HILOG_ERROR_I18N("IntlDateTimeFormatAddon::FormatRangeToParts: unwrap IntlDateTimeFormatAddon failed.");
        return JSUtils::CreateEmptyArray(env);
    }
    std::string errMessage;
    std::vector<std::pair<std::string, std::string>> formatResult =
        obj->intlDateTimeFormat->FormatRangeToParts(start, end, errMessage);
    if (!errMessage.empty()) {
        ErrorUtil::NapiThrowUndefined(env, errMessage);
        return nullptr;
    }
    return CreateFormatPart(env, formatResult);
}

napi_value IntlDateTimeFormatAddon::ToString(napi_env env, napi_callback_info info)
{
    return JSUtils::CreateString(env, "[object Intl.DateTimeFormat]");
}

napi_value IntlDateTimeFormatAddon::ResolvedOptions(napi_env env, napi_callback_info info)
{
    size_t argc = NUMBER_OF_PARAMETER;
    napi_value argv[NUMBER_OF_PARAMETER] = { nullptr };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("IntlDateTimeFormatAddon::ResolvedOptions: get cb info failed.");
        return JSUtils::CreateEmptyObject(env);
    }
    IntlDateTimeFormatAddon* obj = nullptr;
    status = napi_unwrap_s(env, thisVar, &TYPE_TAG, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->intlDateTimeFormat) {
        HILOG_ERROR_I18N("IntlDateTimeFormatAddon::ResolvedOptions: unwrap IntlDateTimeFormatAddon failed.");
        return JSUtils::CreateEmptyObject(env);
    }
    std::unordered_map<std::string, std::string> configs;
    obj->intlDateTimeFormat->ResolvedOptions(configs);
    return CreateResolvedOptions(env, configs);
}

napi_value IntlDateTimeFormatAddon::SupportedLocalesOf(napi_env env, napi_callback_info info)
{
    size_t argc = NUMBER_OF_PARAMETER;
    napi_value argv[NUMBER_OF_PARAMETER] = { nullptr };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    std::vector<std::string> resultLocales = {};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("IntlDateTimeFormatAddon::SupportedLocalesOf: napi get callback info failed.");
        return JSUtils::CreateArray(env, resultLocales);
    }
    std::vector<std::string> localeTags;
    if (argc > 0) {
        int32_t code = 0;
        std::vector<std::string> localeArray = JSUtils::GetLocaleArray(env, argv[0], code);
        localeTags.assign(localeArray.begin(), localeArray.end());
    }
    std::map<std::string, std::string> map = {};
    if (argc > 1) {
        JSUtils::GetOptionValue(env, argv[1], "localeMatcher", map);
    }
    I18nErrorCode i18nStatus = I18nErrorCode::SUCCESS;
    resultLocales = IntlDateTimeFormat::SupportedLocalesOf(localeTags, map, i18nStatus);
    if (i18nStatus == I18nErrorCode::INVALID_PARAM) {
        ErrorUtil::NapiThrowUndefined(env, "getStringOption failed");
        return nullptr;
    } else if (i18nStatus == I18nErrorCode::INVALID_LOCALE_TAG) {
        ErrorUtil::NapiThrowUndefined(env, "invalid locale");
        return nullptr;
    }
    return JSUtils::CreateArray(env, resultLocales);
}

bool IntlDateTimeFormatAddon::GetDateTime(napi_env env, napi_value time, double& milliseconds)
{
    bool isDate = false;
    napi_status status = napi_is_date(env, time, &isDate);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("IntlDateTimeFormatAddon::GetDateTime: Call napi_is_date failed.");
        return false;
    }
    if (isDate) {
        status = napi_get_date_value(env, time, &milliseconds);
        if (status != napi_ok) {
            HILOG_ERROR_I18N("IntlDateTimeFormatAddon::GetDateTime: Get date value failed.");
            return false;
        }
        return true;
    }

    napi_valuetype type = napi_undefined;
    status = napi_typeof(env, time, &type);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("IntlDateTimeFormatAddon::GetDateTime: Get time type failed.");
        return false;
    }
    if (type != napi_valuetype::napi_number) {
        HILOG_ERROR_I18N("IntlDateTimeFormatAddon::GetDateTime: Check type failed.");
        return false;
    }
    status = napi_get_value_double(env, time, &milliseconds);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("IntlDateTimeFormatAddon::GetDateTime: Get get value double failed.");
        return false;
    }
    return true;
}

napi_value IntlDateTimeFormatAddon::CreateFormatPart(napi_env env,
    const std::vector<std::pair<std::string, std::string>>& formatResult)
{
    size_t length = formatResult.size();
    napi_value result = nullptr;
    napi_status status = napi_create_array_with_length(env, length, &result);
    if (status != napi_ok || result == nullptr) {
        HILOG_ERROR_I18N("IntlDateTimeFormatAddon::CreateFormatPart: Create array failed.");
        return JSUtils::CreateEmptyArray(env);
    }
    size_t index = 0;
    for (const auto& item : formatResult) {
        napi_value object = nullptr;
        status = napi_create_object(env, &object);
        if (status != napi_ok || object == nullptr) {
            HILOG_ERROR_I18N("IntlDateTimeFormatAddon::CreateFormatPart: Create object failed.");
            return JSUtils::CreateEmptyArray(env);
        }
        napi_value type = nullptr;
        status = napi_create_string_utf8(env, item.first.c_str(), NAPI_AUTO_LENGTH, &type);
        if (status != napi_ok || type == nullptr) {
            HILOG_ERROR_I18N("IntlDateTimeFormatAddon::CreateFormatPart: Create type failed.");
            return JSUtils::CreateEmptyArray(env);
        }
        status = napi_set_named_property(env, object, "type", type);
        if (status != napi_ok) {
            HILOG_ERROR_I18N("IntlDateTimeFormatAddon::CreateFormatPart: Set property type failed.");
            return JSUtils::CreateEmptyArray(env);
        }
        napi_value value = nullptr;
        status = napi_create_string_utf8(env, item.second.c_str(), NAPI_AUTO_LENGTH, &value);
        if (status != napi_ok || value == nullptr) {
            HILOG_ERROR_I18N("IntlDateTimeFormatAddon::CreateFormatPart: Create type failed.");
            return JSUtils::CreateEmptyArray(env);
        }
        status = napi_set_named_property(env, object, "value", value);
        if (status != napi_ok) {
            HILOG_ERROR_I18N("IntlDateTimeFormatAddon::CreateFormatPart: Set property type failed.");
            return JSUtils::CreateEmptyArray(env);
        }
        status = napi_set_element(env, result, index, object);
        if (status != napi_ok) {
            HILOG_ERROR_I18N("IntlDateTimeFormatAddon::CreateFormatPart: Set element failed, index is %{public}zu.",
                index);
            return JSUtils::CreateEmptyArray(env);
        }
        index++;
    }
    return result;
}

napi_value IntlDateTimeFormatAddon::CreateResolvedOptions(napi_env env,
    const std::unordered_map<std::string, std::string>& configs)
{
    napi_value object = nullptr;
    napi_status status = napi_create_object(env, &object);
    if (status != napi_ok || object == nullptr) {
        HILOG_ERROR_I18N("IntlDateTimeFormatAddon::CreateFormatPart: Create object failed.");
        return JSUtils::CreateEmptyObject(env);
    }

    for (auto& config : configs) {
        std::string key = config.first;
        std::string value = config.second;
        napi_value propertyValue = nullptr;
        if (key.compare(IntlDateTimeFormat::HOUR12_TAG) == 0) {
            bool boolValue = (value.compare("true") == 0) ? true : false;
            status = napi_get_boolean(env, boolValue, &propertyValue);
        } else if (key.compare(IntlDateTimeFormat::FRACTIONAL_SECOND_DIGITS_TAG) == 0) {
            int32_t code = 0;
            int32_t intValue = ConvertString2Int(value, code);
            if (code != 0) {
                HILOG_ERROR_I18N("IntlDateTimeFormatAddon::CreateResolvedOptions: Convert string failed.");
                continue;
            }
            status = napi_create_int32(env, intValue, &propertyValue);
        } else {
            status = napi_create_string_utf8(env, value.c_str(), NAPI_AUTO_LENGTH, &propertyValue);
        }
        if (status != napi_ok || propertyValue == nullptr) {
            HILOG_ERROR_I18N("IntlDateTimeFormatAddon::CreateResolvedOptions: Create propertyValue failed.");
            continue;
        }
        status = napi_set_named_property(env, object, key.c_str(), propertyValue);
        if (status != napi_ok) {
            HILOG_ERROR_I18N("IntlDateTimeFormatAddon::CreateResolvedOptions: Set property failed.");
        }
    }
    return object;
}

std::shared_ptr<IntlDateTimeFormat> IntlDateTimeFormatAddon::GetDateTimeFormat()
{
    return intlDateTimeFormat;
}

bool IntlDateTimeFormatAddon::IsIntlDateTimeFormat(napi_env env, napi_value argv)
{
    napi_value constructor = JSUtils::GetConstructor(env, "DateTimeFormat");
    if (constructor == nullptr) {
        HILOG_ERROR_I18N("IntlDateTimeFormatAddon::IsIntlDateTimeFormat: get constructor failed");
        return false;
    }
    bool isIntlDateTimeFormat = false;
    napi_status status = napi_instanceof(env, argv, constructor, &isIntlDateTimeFormat);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("IntlDateTimeFormatAddon::IsIntlDateTimeFormat: Failed to get instance of argv.");
        return false;
    }
    return isIntlDateTimeFormat;
}

napi_status IntlDateTimeFormatAddon::UnwrapIntlDateTimeFormat(napi_env env, napi_value value,
    IntlDateTimeFormatAddon** dateTimeFormat)
{
    return napi_unwrap_s(env, value, &TYPE_TAG, reinterpret_cast<void **>(dateTimeFormat));
}
} // namespace I18n
} // namespace Global
} // namespace OHOS