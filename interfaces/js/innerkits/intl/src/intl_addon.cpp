/*
 * Copyright (c) 2021-2025 Huawei Device Co., Ltd.
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
#include "intl_addon.h"

#include <set>
#include <vector>

#include "node_api.h"

#include "collator_addon.h"
#include "displaynames_addon.h"
#include "error_util.h"
#include "i18n_hilog.h"
#include "intl_date_time_format_addon.h"
#include "intl_number_format_addon.h"
#include "intl_plural_rules_addon.h"
#include "intl_relative_time_format_addon.h"
#include "js_lifecycle_managers.h"
#include "js_utils.h"
#include "locale_info_addon.h"
#include "number_format_addon.h"
#include "utils.h"

namespace OHOS {
namespace Global {
namespace I18n {
IntlAddon::IntlAddon() : env_(nullptr) {}

IntlAddon::~IntlAddon()
{
}

void IntlAddon::Destructor(napi_env env, void *nativeObject, void *hint)
{
    if (!nativeObject) {
        return;
    }
    delete reinterpret_cast<IntlAddon *>(nativeObject);
    nativeObject = nullptr;
}

napi_value IntlAddon::InitDateTimeFormat(napi_env env, napi_value exports)
{
    HandleScope scope(env, "InitDateTimeFormat");
    if (!scope.IsOpen()) {
        return nullptr;
    }

    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("format", FormatDateTime),
        DECLARE_NAPI_FUNCTION("formatRange", FormatDateTimeRange),
        DECLARE_NAPI_FUNCTION("resolvedOptions", GetDateTimeResolvedOptions)
    };

    napi_value constructor = nullptr;
    napi_status status = status = napi_define_class(env, "DateTimeFormat", NAPI_AUTO_LENGTH, DateTimeFormatConstructor,
        nullptr, sizeof(properties) / sizeof(napi_property_descriptor), properties, &constructor);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("Define class failed when InitDateTimeFormat");
        return nullptr;
    }

    status = napi_set_named_property(env, exports, "DateTimeFormat", constructor);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("Set property failed when InitDateTimeFormat");
        return nullptr;
    }
    return exports;
}

napi_value IntlAddon::InitRelativeTimeFormat(napi_env env, napi_value exports)
{
    HandleScope scope(env, "InitRelativeTimeFormat");
    if (!scope.IsOpen()) {
        return nullptr;
    }

    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("format", FormatRelativeTime),
        DECLARE_NAPI_FUNCTION("formatToParts", FormatToParts),
        DECLARE_NAPI_FUNCTION("resolvedOptions", GetRelativeTimeResolvedOptions)
    };

    napi_value constructor = nullptr;
    napi_status status = status = napi_define_class(env, "RelativeTimeFormat", NAPI_AUTO_LENGTH,
        RelativeTimeFormatConstructor, nullptr, sizeof(properties) / sizeof(napi_property_descriptor),
        properties, &constructor);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("Define class failed when InitRelativeTimeFormat");
        return nullptr;
    }

    status = napi_set_named_property(env, exports, "RelativeTimeFormat", constructor);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("Set property failed when InitRelativeTimeFormat");
        return nullptr;
    }
    return exports;
}

void GetDateOptionValues(napi_env env, napi_value options, std::map<std::string, std::string> &map)
{
    JSUtils::GetOptionValue(env, options, "calendar", map);
    JSUtils::GetOptionValue(env, options, "dateStyle", map);
    JSUtils::GetOptionValue(env, options, "timeStyle", map);
    JSUtils::GetOptionValue(env, options, "hourCycle", map);
    JSUtils::GetOptionValue(env, options, "timeZone", map);
    JSUtils::GetOptionValue(env, options, "timeZoneName", map);
    JSUtils::GetOptionValue(env, options, "numberingSystem", map);
    JSUtils::GetBoolOptionValue(env, options, "hour12", map);
    JSUtils::GetOptionValue(env, options, "weekday", map);
    JSUtils::GetOptionValue(env, options, "era", map);
    JSUtils::GetOptionValue(env, options, "year", map);
    JSUtils::GetOptionValue(env, options, "month", map);
    JSUtils::GetOptionValue(env, options, "day", map);
    JSUtils::GetOptionValue(env, options, "hour", map);
    JSUtils::GetOptionValue(env, options, "minute", map);
    JSUtils::GetOptionValue(env, options, "second", map);
    JSUtils::GetOptionValue(env, options, "localeMatcher", map);
    JSUtils::GetOptionValue(env, options, "formatMatcher", map);
    JSUtils::GetOptionValue(env, options, "dayPeriod", map);
}

void GetRelativeTimeOptionValues(napi_env env, napi_value options, std::map<std::string, std::string> &map)
{
    JSUtils::GetOptionValue(env, options, "localeMatcher", map);
    JSUtils::GetOptionValue(env, options, "numeric", map);
    JSUtils::GetOptionValue(env, options, "style", map);
}

std::string GetLocaleTag(napi_env env, napi_value argv)
{
    std::string localeTag = "";
    std::vector<char> buf;
    if (argv != nullptr) {
        napi_valuetype valueType = napi_valuetype::napi_undefined;
        napi_status status = napi_typeof(env, argv, &valueType);
        if (status != napi_ok) {
            return "";
        }
        if (valueType != napi_valuetype::napi_string) {
            HILOG_ERROR_I18N("GetLocaleTag: Parameter type does not match");
            return "";
        }
        size_t len = 0;
        status = napi_get_value_string_utf8(env, argv, nullptr, 0, &len);
        if (status != napi_ok) {
            HILOG_ERROR_I18N("GetLocaleTag -> string: Get locale tag length failed");
            return "";
        }
        buf.resize(len + 1);
        status = napi_get_value_string_utf8(env, argv, buf.data(), len + 1, &len);
        if (status != napi_ok) {
            HILOG_ERROR_I18N("GetLocaleTag: Get locale tag failed");
            return "";
        }
        localeTag = buf.data();
    } else {
        localeTag = "";
    }
    return localeTag;
}

napi_value IntlAddon::DateTimeFormatConstructor(napi_env env, napi_callback_info info)
{
    size_t argc = 2;
    napi_value argv[2] = { nullptr };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        return nullptr;
    }
    std::vector<std::string> localeTags;
    if (argc > 0) {
        int32_t code = 0;
        std::vector<std::string> localeArray = JSUtils::GetLocaleArray(env, argv[0], code);
        if (code != 0) {
            return nullptr;
        }
        localeTags.assign(localeArray.begin(), localeArray.end());
    }
    std::map<std::string, std::string> map = {};
    if (argc > 1) {
        GetDateOptionValues(env, argv[1], map);
    }
    std::unique_ptr<IntlAddon> obj = std::make_unique<IntlAddon>();
    if (obj == nullptr) {
        return nullptr;
    }
    status = napi_wrap_s(env, thisVar, reinterpret_cast<void *>(obj.get()), IntlAddon::Destructor, nullptr,
        &TYPE_TAG, nullptr);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("DateTimeFormatConstructor: Wrap IntlAddon failed");
        return nullptr;
    }
    if (!obj->InitDateTimeFormatContext(env, info, localeTags, map)) {
        HILOG_ERROR_I18N("DateTimeFormatConstructor: Init DateTimeFormat failed");
        return nullptr;
    }
    obj.release();
    return thisVar;
}

bool IntlAddon::InitDateTimeFormatContext(napi_env env, napi_callback_info info, std::vector<std::string> localeTags,
    std::map<std::string, std::string> &map)
{
    napi_value global = nullptr;
    napi_status status = napi_get_global(env, &global);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("InitDateTimeFormatContext: Get global failed");
        return false;
    }
    env_ = env;
    datefmt_ = DateTimeFormat::CreateInstance(localeTags, map);

    return datefmt_ != nullptr;
}

napi_value IntlAddon::RelativeTimeFormatConstructor(napi_env env, napi_callback_info info)
{
    size_t argc = 2;
    napi_value argv[2] = { nullptr };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        return nullptr;
    }
    std::vector<std::string> localeTags;
    if (argc > 0) {
        int32_t code = 0;
        std::vector<std::string> localeArray = JSUtils::GetLocaleArray(env, argv[0], code);
        if (code != 0) {
            return nullptr;
        }
        localeTags.assign(localeArray.begin(), localeArray.end());
    }
    std::map<std::string, std::string> map = {};
    if (argc > 1) {
        GetRelativeTimeOptionValues(env, argv[1], map);
    }
    std::unique_ptr<IntlAddon> obj = std::make_unique<IntlAddon>();
    if (obj == nullptr) {
        return nullptr;
    }
    status = napi_wrap_s(env, thisVar, reinterpret_cast<void *>(obj.get()), IntlAddon::Destructor, nullptr,
        &TYPE_TAG, nullptr);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("RelativeTimeFormatConstructor: Wrap IntlAddon failed");
        return nullptr;
    }
    if (!obj->InitRelativeTimeFormatContext(env, info, localeTags, map)) {
        HILOG_ERROR_I18N("Init RelativeTimeFormat failed");
        return nullptr;
    }
    obj.release();
    return thisVar;
}

bool IntlAddon::InitRelativeTimeFormatContext(napi_env env, napi_callback_info info,
    std::vector<std::string> localeTags, std::map<std::string, std::string> &map)
{
    env_ = env;
    relativetimefmt_ = std::make_unique<RelativeTimeFormat>(localeTags, map);

    return relativetimefmt_ != nullptr;
}

napi_value IntlAddon::FormatDateTime(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = { 0 };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        return nullptr;
    }
    int64_t milliseconds = GetMilliseconds(env, argv, 0);
    if (milliseconds == -1) {
        return nullptr;
    }
    IntlAddon *obj = nullptr;
    status = napi_unwrap_s(env, thisVar, &TYPE_TAG, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->datefmt_) {
        HILOG_ERROR_I18N("FormatDateTime: Get DateTimeFormat object failed");
        return nullptr;
    }
    std::string value = obj->datefmt_->Format(milliseconds);
    napi_value result = nullptr;
    status = napi_create_string_utf8(env, value.c_str(), NAPI_AUTO_LENGTH, &result);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("FormatDateTime: Create format string failed");
        return nullptr;
    }
    return result;
}

napi_value IntlAddon::FormatDateTimeRange(napi_env env, napi_callback_info info)
{
    size_t argc = 2;
    napi_value argv[2] = { nullptr };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        return nullptr;
    }
    if (argc < FUNC_ARGS_COUNT) {
        HILOG_ERROR_I18N("Parameter wrong");
        return nullptr;
    }
    int64_t firstMilliseconds = GetMilliseconds(env, argv, 0);
    int64_t secondMilliseconds = GetMilliseconds(env, argv, 1);
    if (firstMilliseconds == -1 || secondMilliseconds == -1) {
        return nullptr;
    }
    IntlAddon *obj = nullptr;
    status = napi_unwrap_s(env, thisVar, &TYPE_TAG, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->datefmt_) {
        HILOG_ERROR_I18N("FormatDateTimeRange: Get DateTimeFormat object failed");
        return nullptr;
    }
    std::string value = obj->datefmt_->FormatRange(firstMilliseconds, secondMilliseconds);
    napi_value result = nullptr;
    status = napi_create_string_utf8(env, value.c_str(), NAPI_AUTO_LENGTH, &result);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("FormatDateTimeRange: Create format string failed");
        return nullptr;
    }
    return result;
}

int64_t IntlAddon::GetMilliseconds(napi_env env, napi_value *argv, int index)
{
    napi_value funcGetDateInfo = nullptr;
    napi_status status = napi_get_named_property(env, argv[index], "getTime", &funcGetDateInfo);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("Get Milliseconds property failed");
        return -1;
    }
    napi_value ret_value = nullptr;
    status = napi_call_function(env, argv[index], funcGetDateInfo, 0, nullptr, &ret_value);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("Get Milliseconds function failed");
        return -1;
    }
    int64_t milliseconds = 0;
    status = napi_get_value_int64(env, ret_value, &milliseconds);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("Get Milliseconds failed");
        return -1;
    }
    return milliseconds;
}

napi_value IntlAddon::GetRelativeTimeResolvedOptions(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, &data);
    if (status != napi_ok) {
        return nullptr;
    }
    IntlAddon *obj = nullptr;
    status = napi_unwrap_s(env, thisVar, &TYPE_TAG, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->relativetimefmt_) {
        HILOG_ERROR_I18N("GetRelativeTimeResolvedOptions: Get RelativeTimeFormat object failed");
        return nullptr;
    }
    napi_value result = nullptr;
    status = napi_create_object(env, &result);
    if (status != napi_ok) {
        return nullptr;
    }
    std::map<std::string, std::string> options = {};
    obj->relativetimefmt_->GetResolvedOptions(options);
    JSUtils::SetOptionProperties(env, result, options, "locale");
    JSUtils::SetOptionProperties(env, result, options, "style");
    JSUtils::SetOptionProperties(env, result, options, "numeric");
    JSUtils::SetOptionProperties(env, result, options, "numberingSystem");
    return result;
}

napi_value IntlAddon::GetDateTimeResolvedOptions(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, &data);
    if (status != napi_ok) {
        return nullptr;
    }
    IntlAddon *obj = nullptr;
    status = napi_unwrap_s(env, thisVar, &TYPE_TAG, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->datefmt_) {
        HILOG_ERROR_I18N("GetDateTimeResolvedOptions: Get DateTimeFormat object failed");
        return nullptr;
    }
    napi_value result = nullptr;
    status = napi_create_object(env, &result);
    if (status != napi_ok) {
        return nullptr;
    }
    std::map<std::string, std::string> options = {};
    obj->datefmt_->GetResolvedOptions(options);
    JSUtils::SetOptionProperties(env, result, options, "locale");
    JSUtils::SetOptionProperties(env, result, options, "calendar");
    JSUtils::SetOptionProperties(env, result, options, "dateStyle");
    JSUtils::SetOptionProperties(env, result, options, "timeStyle");
    JSUtils::SetOptionProperties(env, result, options, "hourCycle");
    JSUtils::SetOptionProperties(env, result, options, "timeZone");
    JSUtils::SetOptionProperties(env, result, options, "timeZoneName");
    JSUtils::SetOptionProperties(env, result, options, "numberingSystem");
    JSUtils::SetBooleanOptionProperties(env, result, options, "hour12");
    JSUtils::SetOptionProperties(env, result, options, "weekday");
    JSUtils::SetOptionProperties(env, result, options, "era");
    JSUtils::SetOptionProperties(env, result, options, "year");
    JSUtils::SetOptionProperties(env, result, options, "month");
    JSUtils::SetOptionProperties(env, result, options, "day");
    JSUtils::SetOptionProperties(env, result, options, "hour");
    JSUtils::SetOptionProperties(env, result, options, "minute");
    JSUtils::SetOptionProperties(env, result, options, "second");
    JSUtils::SetOptionProperties(env, result, options, "dayPeriod");
    JSUtils::SetOptionProperties(env, result, options, "localeMatcher");
    JSUtils::SetOptionProperties(env, result, options, "formatMatcher");
    return result;
}

void GetCollatorLocaleMatcher(napi_env env, napi_value options, std::map<std::string, std::string> &map)
{
    JSUtils::GetOptionValue(env, options, "localeMatcher", map);
    auto it = map.find("localeMatcher");
    if (it != map.end()) {
        std::string localeMatcher = it->second;
        if (localeMatcher != "lookup" && localeMatcher != "best fit") {
            HILOG_ERROR_I18N("invalid localeMatcher");
            return;
        }
    } else {
        map.insert(std::make_pair("localeMatcher", "best fit"));
    }
}

void GetCollatorUsage(napi_env env, napi_value options, std::map<std::string, std::string> &map)
{
    JSUtils::GetOptionValue(env, options, "usage", map);
    auto it = map.find("usage");
    if (it != map.end()) {
        std::string usage = it->second;
        if (usage != "sort" && usage != "search") {
            HILOG_ERROR_I18N("invalid usage");
            return;
        }
    } else {
        map.insert(std::make_pair("usage", "sort"));
    }
}

void GetCollatorSensitivity(napi_env env, napi_value options, std::map<std::string, std::string> &map)
{
    JSUtils::GetOptionValue(env, options, "sensitivity", map);
    auto it = map.find("sensitivity");
    if (it != map.end()) {
        std::string sensitivity = it->second;
        if (sensitivity != "base" && sensitivity != "accent" && sensitivity != "case" && sensitivity != "variant") {
            HILOG_ERROR_I18N("invalid sensitivity");
            return;
        }
    } else {
        map.insert(std::make_pair("sensitivity", "variant"));
    }
}

void GetCollatorIgnorePunctuation(napi_env env, napi_value options, std::map<std::string, std::string> &map)
{
    JSUtils::GetBoolOptionValue(env, options, "ignorePunctuation", map);
    auto it = map.find("ignorePunctuation");
    if (it != map.end()) {
        std::string ignorePunctuation = it->second;
        if (ignorePunctuation != "true" && ignorePunctuation != "false") {
            HILOG_ERROR_I18N("invalid ignorePunctuation");
            return;
        }
    } else {
        map.insert(std::make_pair("ignorePunctuation", "false"));
    }
}

void GetCollatorNumeric(napi_env env, napi_value options, std::map<std::string, std::string> &map)
{
    JSUtils::GetBoolOptionValue(env, options, "numeric", map);
    auto it = map.find("numeric");
    if (it != map.end()) {
        std::string numeric = it->second;
        if (numeric != "true" && numeric != "false") {
            HILOG_ERROR_I18N("invalid numeric");
            return;
        }
    }
}

void GetCollatorCaseFirst(napi_env env, napi_value options, std::map<std::string, std::string> &map)
{
    JSUtils::GetOptionValue(env, options, "caseFirst", map);
    auto it = map.find("caseFirst");
    if (it != map.end()) {
        std::string caseFirst = it->second;
        if (caseFirst != "upper" && caseFirst != "lower" && caseFirst != "false") {
            HILOG_ERROR_I18N("invalid caseFirst");
            return;
        }
    }
}

void GetCollatorCollation(napi_env env, napi_value options, std::map<std::string, std::string> &map)
{
    JSUtils::GetOptionValue(env, options, "collation", map);
    auto it = map.find("collation");
    if (it != map.end()) {
        std::string collation = it->second;
        std::set<std::string> validCollation;
        validCollation.insert("big5han");
        validCollation.insert("compat");
        validCollation.insert("dict");
        validCollation.insert("direct");
        validCollation.insert("ducet");
        validCollation.insert("eor");
        validCollation.insert("gb2312");
        validCollation.insert("phonebk");
        validCollation.insert("phonetic");
        validCollation.insert("pinyin");
        validCollation.insert("reformed");
        validCollation.insert("searchjl");
        validCollation.insert("stroke");
        validCollation.insert("trad");
        validCollation.insert("unihan");
        validCollation.insert("zhuyin");
        if (validCollation.find(collation) == validCollation.end()) {
            map["collation"] = "default";
        }
    }
}

void GetCollatorOptionValue(napi_env env, napi_value options, std::map<std::string, std::string> &map)
{
    GetCollatorLocaleMatcher(env, options, map);
    GetCollatorUsage(env, options, map);
    GetCollatorSensitivity(env, options, map);
    GetCollatorIgnorePunctuation(env, options, map);
    GetCollatorNumeric(env, options, map);
    GetCollatorCaseFirst(env, options, map);
    GetCollatorCollation(env, options, map);
}

napi_value IntlAddon::InitCollator(napi_env env, napi_value exports)
{
    HandleScope scope(env, "InitCollator");
    if (!scope.IsOpen()) {
        return nullptr;
    }

    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("compare", CompareString),
        DECLARE_NAPI_FUNCTION("resolvedOptions", GetCollatorResolvedOptions)
    };

    napi_value constructor;
    napi_status status = napi_define_class(env, "Collator", NAPI_AUTO_LENGTH, CollatorConstructor, nullptr,
        sizeof(properties) / sizeof(napi_property_descriptor), properties, &constructor);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("Define class failed when InitCollator");
        return nullptr;
    }

    status = napi_set_named_property(env, exports, "Collator", constructor);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("Set property failed when InitCollator");
        return nullptr;
    }
    return exports;
}

napi_value IntlAddon::CollatorConstructor(napi_env env, napi_callback_info info)
{
    size_t argc = 2;
    napi_value argv[2] = { nullptr };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        return nullptr;
    }
    std::vector<std::string> localeTags;
    if (argc > 0) {
        int32_t code = 0;
        std::vector<std::string> localeArray = JSUtils::GetLocaleArray(env, argv[0], code);
        if (code != 0) {
            return nullptr;
        }
        localeTags.assign(localeArray.begin(), localeArray.end());
    }
    std::map<std::string, std::string> map = {};
    if (argc > 1) {
        GetCollatorOptionValue(env, argv[1], map);
    }
    std::unique_ptr<IntlAddon> obj = std::make_unique<IntlAddon>();
    if (obj == nullptr) {
        return nullptr;
    }
    status = napi_wrap_s(env, thisVar, reinterpret_cast<void *>(obj.get()), IntlAddon::Destructor, nullptr,
        &TYPE_TAG, nullptr);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("CollatorConstructor: Wrap IntlAddon failed");
        return nullptr;
    }
    if (!obj->InitCollatorContext(env, info, localeTags, map)) {
        HILOG_ERROR_I18N("CollatorConstructor: Init DateTimeFormat failed");
        return nullptr;
    }
    obj.release();
    return thisVar;
}

bool IntlAddon::InitCollatorContext(napi_env env, napi_callback_info info, std::vector<std::string> localeTags,
    std::map<std::string, std::string> &map)
{
    napi_value global = nullptr;
    napi_status status = napi_get_global(env, &global);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("InitCollatorContext: Get global failed");
        return false;
    }
    env_ = env;
    collator_ = std::make_unique<Collator>(localeTags, map);

    return collator_ != nullptr;
}

bool GetStringParameter(napi_env env, napi_value value, std::vector<char> &buf)
{
    napi_valuetype valueType = napi_valuetype::napi_undefined;
    napi_status status = napi_typeof(env, value, &valueType);
    if (status != napi_ok) {
        return false;
    }
    if (valueType != napi_valuetype::napi_string) {
        HILOG_ERROR_I18N("Parameter type does not match");
        return false;
    }
    size_t len = 0;
    status = napi_get_value_string_utf8(env, value, nullptr, 0, &len);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("Get first length failed");
        return false;
    }
    buf.resize(len + 1);
    status = napi_get_value_string_utf8(env, value, buf.data(), len + 1, &len);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("Get first failed");
        return false;
    }
    return true;
}

napi_value IntlAddon::FormatRelativeTime(napi_env env, napi_callback_info info)
{
    size_t argc = 2;
    napi_value argv[2] = { 0 };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        return nullptr;
    }
    double number;
    status = napi_get_value_double(env, argv[0], &number);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("FormatRelativeTime: Get number failed");
        return nullptr;
    }
    std::vector<char> unit;
    if (!GetStringParameter(env, argv[1], unit)) {
        return nullptr;
    }
    IntlAddon *obj = nullptr;
    status = napi_unwrap_s(env, thisVar, &TYPE_TAG, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->relativetimefmt_) {
        HILOG_ERROR_I18N("FormatRelativeTime: Get RelativeTimeFormat object failed");
        return nullptr;
    }
    std::string value = obj->relativetimefmt_->Format(number, unit.data());
    napi_value result = nullptr;
    status = napi_create_string_utf8(env, value.c_str(), NAPI_AUTO_LENGTH, &result);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("FormatRelativeTime: Create format string failed");
        return nullptr;
    }
    return result;
}

void IntlAddon::FillInArrayElement(napi_env env, napi_value &result, napi_status &status,
    const std::vector<std::vector<std::string>> &timeVector)
{
    for (size_t i = 0; i < timeVector.size(); i++) {
        napi_value value = nullptr;
        status = napi_create_string_utf8(env, timeVector[i][1].c_str(), NAPI_AUTO_LENGTH, &value);
        if (status != napi_ok) {
            HILOG_ERROR_I18N("Failed to create string item imeVector[i][1].");
            return;
        }
        napi_value type = nullptr;
        status = napi_create_string_utf8(env, timeVector[i][0].c_str(), NAPI_AUTO_LENGTH, &type);
        if (status != napi_ok) {
            HILOG_ERROR_I18N("Failed to create string item timeVector[i][0].");
            return;
        }
        napi_value unit = nullptr;
        size_t unitIndex = 2;
        if (timeVector[i].size() > unitIndex) {
            status = napi_create_string_utf8(env, timeVector[i][unitIndex].c_str(), NAPI_AUTO_LENGTH, &unit);
            if (status != napi_ok) {
                HILOG_ERROR_I18N("Failed to create string item timeVector[i][unitIndex].");
                return;
            }
        } else {
            status = napi_get_undefined(env, &unit);
            if (status != napi_ok) {
                return;
            }
        }
        std::unordered_map<std::string, napi_value> propertys {
            { "type", type },
            { "value", value },
            { "unit", unit }
        };
        int32_t code = 0;
        napi_value formatInfo = JSUtils::CreateObject(env, propertys, code);
        if (code != 0) {
            return;
        }
        status = napi_set_element(env, result, i, formatInfo);
        if (status != napi_ok) {
            HILOG_ERROR_I18N("Failed to set array item");
            return;
        }
    }
}

napi_value IntlAddon::FormatToParts(napi_env env, napi_callback_info info)
{
    size_t argc = 2;
    napi_value argv[2] = { 0 };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        return nullptr;
    }
    double number = 0;
    status = napi_get_value_double(env, argv[0], &number);
    if (status != napi_ok) {
        return nullptr;
    }
    std::vector<char> unit;
    if (!GetStringParameter(env, argv[1], unit)) {
        return nullptr;
    }
    IntlAddon *obj = nullptr;
    status = napi_unwrap_s(env, thisVar, &TYPE_TAG, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->relativetimefmt_) {
        HILOG_ERROR_I18N("FormatToParts: Get RelativeTimeFormat object failed");
        return nullptr;
    }
    std::vector<std::vector<std::string>> timeVector;
    obj->relativetimefmt_->FormatToParts(number, unit.data(), timeVector);
    napi_value result = nullptr;
    status = napi_create_array_with_length(env, timeVector.size(), &result);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("Failed to create array");
        return nullptr;
    }
    FillInArrayElement(env, result, status, timeVector);
    return result;
}

napi_value IntlAddon::CompareString(napi_env env, napi_callback_info info)
{
    size_t argc = 2;
    napi_value argv[2] = { 0 };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        return nullptr;
    }
    std::vector<char> first;
    if (!GetStringParameter(env, argv[0], first)) {
        return nullptr;
    }

    std::vector<char> second;
    if (!GetStringParameter(env, argv[1], second)) {
        return nullptr;
    }

    IntlAddon *obj = nullptr;
    status = napi_unwrap_s(env, thisVar, &TYPE_TAG, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->collator_) {
        HILOG_ERROR_I18N("CompareString: Get Collator object failed");
        return nullptr;
    }

    CompareResult compareResult = obj->collator_->Compare(first.data(), second.data());
    napi_value result = nullptr;
    status = napi_create_int32(env, compareResult, &result);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("Create compare result failed");
        return nullptr;
    }

    return result;
}

napi_value IntlAddon::GetCollatorResolvedOptions(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, &data);
    if (status != napi_ok) {
        return nullptr;
    }
    IntlAddon *obj = nullptr;
    status = napi_unwrap_s(env, thisVar, &TYPE_TAG, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->collator_) {
        HILOG_ERROR_I18N("GetCollatorResolvedOptions: Get Collator object failed");
        return nullptr;
    }
    napi_value result = nullptr;
    status = napi_create_object(env, &result);
    if (status != napi_ok) {
        return nullptr;
    }
    std::map<std::string, std::string> options = {};
    obj->collator_->ResolvedOptions(options);
    JSUtils::SetOptionProperties(env, result, options, "localeMatcher");
    JSUtils::SetOptionProperties(env, result, options, "locale");
    JSUtils::SetOptionProperties(env, result, options, "usage");
    JSUtils::SetOptionProperties(env, result, options, "sensitivity");
    JSUtils::SetBooleanOptionProperties(env, result, options, "ignorePunctuation");
    JSUtils::SetBooleanOptionProperties(env, result, options, "numeric");
    JSUtils::SetOptionProperties(env, result, options, "caseFirst");
    JSUtils::SetOptionProperties(env, result, options, "collation");
    return result;
}

void GetPluralRulesType(napi_env env, napi_value options, std::map<std::string, std::string> &map)
{
    JSUtils::GetOptionValue(env, options, "type", map);
    auto it = map.find("type");
    if (it != map.end()) {
        std::string type = it->second;
        if (type != "cardinal" && type != "ordinal") {
            HILOG_ERROR_I18N("invalid type");
            return;
        }
    } else {
        map.insert(std::make_pair("type", "cardinal"));
    }
}

void GetPluralRulesInteger(napi_env env, napi_value options, std::map<std::string, std::string> &map)
{
    JSUtils::GetIntegerOptionValue(env, options, "minimumIntegerDigits", map);
    auto it = map.find("minimumIntegerDigits");
    if (it != map.end()) {
        std::string minimumIntegerDigits = it->second;
        int32_t status = 0;
        int n = ConvertString2Int(minimumIntegerDigits, status);
        if (status == -1 || n < 1 || n > 21) {  // the valid range of minimumIntegerDigits is [1, 21]
            HILOG_ERROR_I18N("invalid minimumIntegerDigits");
            return;
        }
    } else {
        map.insert(std::make_pair("minimumIntegerDigits", std::to_string(1)));
    }
}

void GetPluralRulesFractions(napi_env env, napi_value options, std::map<std::string, std::string> &map)
{
    JSUtils::GetIntegerOptionValue(env, options, "minimumFractionDigits", map);
    auto it = map.find("minimumFractionDigits");
    if (it != map.end()) {
        std::string minimumFractionDigits = it->second;
        int32_t status = 0;
        int n = ConvertString2Int(minimumFractionDigits, status);
        if (status == -1 || n < 0 || n > 20) {  // the valid range of minimumFractionDigits is [0, 20]
            HILOG_ERROR_I18N("invalid minimumFractionDigits");
            return;
        }
    }

    JSUtils::GetIntegerOptionValue(env, options, "maximumFractionDigits", map);
    it = map.find("maximumFractionDigits");
    if (it != map.end()) {
        std::string maximumFractionDigits = it->second;
        int32_t status = 0;
        int n = ConvertString2Int(maximumFractionDigits, status);
        if (status == -1 || n < 0 || n > 20) {  // the valid range of maximumFractionDigits is [0, 20]
            HILOG_ERROR_I18N("invalid maximumFractionDigits");
            return;
        }
    }
}

void GetPluralRulesSignificant(napi_env env, napi_value options, std::map<std::string, std::string> &map)
{
    int minSignificant = -1;
    JSUtils::GetIntegerOptionValue(env, options, "minimumSignificantDigits", map);
    auto it = map.find("minimumSignificantDigits");
    if (it != map.end()) {
        std::string minSignificantStr = it->second;
        int32_t status = 0;
        int minSignificantInt = ConvertString2Int(minSignificantStr, status);
        // the valid range of minSignificantInt is [1, 21]
        if (status == -1 || minSignificantInt < 1 || minSignificantInt > 21) {
            HILOG_ERROR_I18N("invalid minimumSignificantDigits");
            return;
        }
        minSignificant = minSignificantInt;
    } else {
        minSignificant = 1;
    }

    JSUtils::GetIntegerOptionValue(env, options, "maximumSignificantDigits", map);
    it = map.find("maximumSignificantDigits");
    if (it != map.end()) {
        std::string maxSignificantStr = it->second;
        int32_t status = 0;
        int maxSignificant = ConvertString2Int(maxSignificantStr, status);
        // the valid range of minSignificant is [minSignificant, 21]
        if (status == -1 || maxSignificant < minSignificant || maxSignificant > 21) {
            HILOG_ERROR_I18N("invalid maximumSignificantDigits");
            return;
        }
    }
}

void GetPluralRulesOptionValues(napi_env env, napi_value options, std::map<std::string, std::string> &map)
{
    GetCollatorLocaleMatcher(env, options, map);
    GetPluralRulesType(env, options, map);
    GetPluralRulesInteger(env, options, map);
    GetPluralRulesFractions(env, options, map);
    GetPluralRulesSignificant(env, options, map);
}

napi_value IntlAddon::InitPluralRules(napi_env env, napi_value exports)
{
    HandleScope scope(env, "InitPluralRules");
    if (!scope.IsOpen()) {
        return nullptr;
    }

    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("select", Select)
    };

    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, "PluralRules", NAPI_AUTO_LENGTH, PluralRulesConstructor, nullptr,
        sizeof(properties) / sizeof(napi_property_descriptor), properties, &constructor);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("Define class failed when InitPluralRules");
        return nullptr;
    }

    status = napi_set_named_property(env, exports, "PluralRules", constructor);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("Set property failed when InitPluralRules");
        return nullptr;
    }
    return exports;
}

napi_value IntlAddon::PluralRulesConstructor(napi_env env, napi_callback_info info)
{
    size_t argc = 2;
    napi_value argv[2] = { nullptr };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        return nullptr;
    }
    std::vector<std::string> localeTags;
    if (argc > 0) {
        int32_t code = 0;
        std::vector<std::string> localeArray = JSUtils::GetLocaleArray(env, argv[0], code);
        if (code != 0) {
            return nullptr;
        }
        localeTags.assign(localeArray.begin(), localeArray.end());
    }
    std::map<std::string, std::string> map = {};
    if (argc > 1) {
        GetPluralRulesOptionValues(env, argv[1], map);
    }
    std::unique_ptr<IntlAddon> obj = std::make_unique<IntlAddon>();
    if (obj == nullptr) {
        return nullptr;
    }
    status = napi_wrap_s(env, thisVar, reinterpret_cast<void *>(obj.get()), IntlAddon::Destructor, nullptr,
        &TYPE_TAG, nullptr);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("PluralRulesConstructor: Wrap IntlAddon failed");
        return nullptr;
    }
    if (!obj->InitPluralRulesContext(env, info, localeTags, map)) {
        HILOG_ERROR_I18N("PluralRulesConstructor: Init DateTimeFormat failed");
        return nullptr;
    }
    obj.release();
    return thisVar;
}

bool IntlAddon::InitPluralRulesContext(napi_env env, napi_callback_info info, std::vector<std::string> localeTags,
    std::map<std::string, std::string> &map)
{
    napi_value global = nullptr;
    napi_status status = napi_get_global(env, &global);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("InitPluralRulesContext: Get global failed");
        return false;
    }
    env_ = env;
    pluralrules_ = std::make_unique<PluralRules>(localeTags, map);

    return pluralrules_ != nullptr;
}

napi_value IntlAddon::Select(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = { 0 };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        return nullptr;
    }
    napi_valuetype valueType = napi_valuetype::napi_undefined;
    status = napi_typeof(env, argv[0], &valueType);
    if (status != napi_ok) {
        return nullptr;
    }
    if (valueType != napi_valuetype::napi_number) {
        HILOG_ERROR_I18N("Select: Parameter type does not match");
        return nullptr;
    }

    double number = 0;
    status = napi_get_value_double(env, argv[0], &number);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("Select: Get number failed");
        return nullptr;
    }

    IntlAddon *obj = nullptr;
    status = napi_unwrap_s(env, thisVar, &TYPE_TAG, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->pluralrules_) {
        HILOG_ERROR_I18N("Get PluralRules object failed");
        return nullptr;
    }

    std::string res = obj->pluralrules_->Select(number);
    napi_value result = nullptr;
    status = napi_create_string_utf8(env, res.c_str(), NAPI_AUTO_LENGTH, &result);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("get select result failed");
        return nullptr;
    }
    return result;
}

napi_value IntlAddon::Init(napi_env env, napi_value exports)
{
    napi_value val = LocaleInfoAddon::InitLocale(env, exports);
    val = IntlAddon::InitDateTimeFormat(env, val);
    val = IntlAddon::InitCollator(env, val);
    val = IntlAddon::InitRelativeTimeFormat(env, val);
    val = IntlAddon::InitPluralRules(env, val);
    val = NumberFormatAddon::InitNumberFormat(env, val);
    return val;
}
} // namespace I18n
} // namespace Global
} // namespace OHOS

bool SetBuildInIntlModule(napi_env &env, napi_value &intlModule, napi_value &buildinIntlModule, const std::string& key)
{
    OHOS::Global::I18n::HandleScope scope(env, "SetBuildInIntlModule");
    if (!scope.IsOpen()) {
        return false;
    }

    napi_value buildinClass = nullptr;
    napi_status status = napi_get_named_property(env, buildinIntlModule, key.c_str(), &buildinClass);
    if (status != napi_ok || buildinClass == nullptr) {
        HILOG_ERROR_I18N("RegisterIntl: get Buildin Intl class %{public}s failed", key.c_str());
        return false;
    }
    status = napi_set_named_property(env, intlModule, key.c_str(), buildinClass);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("RegisterIntl: set Buildin Intl class %{public}s failed", key.c_str());
        return false;
    }
    return true;
}

bool SetBuildInIntl(napi_env &env, napi_value &global, napi_value &intlModule)
{
    OHOS::Global::I18n::HandleScope scope(env, "SetBuildInIntl");
    if (!scope.IsOpen()) {
        return false;
    }

    bool hasProperty = false;
    napi_status status = napi_has_named_property(env, global, "Intl", &hasProperty);
    if (status != napi_ok || !hasProperty) {
        HILOG_ERROR_I18N("RegisterIntl: Has not named Intl property");
        return false;
    }

    napi_value buildinIntlModule = nullptr;
    status = napi_get_named_property(env, global, "Intl", &buildinIntlModule);
    if (status != napi_ok || buildinIntlModule == nullptr) {
        HILOG_ERROR_I18N("RegisterIntl: get Buildin Intl Module failed");
        return false;
    }

    const std::vector<std::string> buildinClasses = {"DurationFormat", "ListFormat", "Locale",
        "Segmenter", "getCanonicalLocales", "supportedValuesOf"};

    for (std::string buildinClass : buildinClasses) {
        if (!SetBuildInIntlModule(env, intlModule, buildinIntlModule, buildinClass)) {
            return false;
        }
    }
    return true;
}

bool BindingIntlModules(napi_env env, napi_value &intlModule)
{
    intlModule = OHOS::Global::I18n::CollatorAddon::InitCollator(env, intlModule);
    if (intlModule == nullptr) {
        HILOG_ERROR_I18N("RegisterIntl: init Intl Collator Module failed");
        return false;
    }
    intlModule = OHOS::Global::I18n::DisplayNamesAddon::InitDisplayNames(env, intlModule);
    if (intlModule == nullptr) {
        HILOG_ERROR_I18N("RegisterIntl: init Intl DisplayNames Module failed");
        return false;
    }
    intlModule = OHOS::Global::I18n::IntlNumberFormatAddon::InitIntlNumberFormat(env, intlModule);
    if (intlModule == nullptr) {
        HILOG_ERROR_I18N("RegisterIntl: init Intl NumberFormat Module failed");
        return false;
    }
    intlModule = OHOS::Global::I18n::IntlDateTimeFormatAddon::InitIntlDateTimeFormat(env, intlModule);
    if (intlModule == nullptr) {
        HILOG_ERROR_I18N("RegisterIntl: init Intl DateTimeFormat Module failed");
        return false;
    }
    intlModule = OHOS::Global::I18n::IntlPluralRulesAddon::InitIntlPluralRules(env, intlModule);
    if (intlModule == nullptr) {
        HILOG_ERROR_I18N("RegisterIntl: init Intl PluralRules Module failed");
        return false;
    }
    intlModule = OHOS::Global::I18n::IntlRelativeTimeFormatAddon::InitIntlRelativeTimeFormat(env, intlModule);
    if (intlModule == nullptr) {
        HILOG_ERROR_I18N("RegisterIntl: init Intl RelativeTimeFormat Module failed");
        return false;
    }
    return true;
}

bool RegisterIntl(napi_env env)
{
    OHOS::Global::I18n::HandleScope scope(env, "RegisterIntl");
    if (!scope.IsOpen()) {
        return false;
    }

    if (env == nullptr) {
        HILOG_ERROR_I18N("RegisterIntl: env is null");
        return false;
    }

    napi_value global = nullptr;
    napi_status status = napi_get_global(env, &global);
    if (status != napi_ok || global == nullptr) {
        HILOG_ERROR_I18N("RegisterIntl: get global obj failed");
        return false;
    }

    napi_value intlModule = nullptr;
    status = napi_create_object(env, &intlModule);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("RegisterIntl: Define Intl object failed.");
        return false;
    }

    if (!BindingIntlModules(env, intlModule)) {
        return false;
    }

    if (!SetBuildInIntl(env, global, intlModule)) {
        return false;
    }

    status = napi_set_named_property(env, global, "Intl", intlModule);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("RegisterIntl: Set property failed when Init Intl module.");
        return false;
    }
    return true;
}
