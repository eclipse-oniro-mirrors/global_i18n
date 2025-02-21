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
#include <unordered_map>
#include <vector>

#include "error_util.h"
#include "hilog/log.h"
#include "holiday_manager_addon.h"
#include "entity_recognizer_addon.h"
#include "i18n_calendar_addon.h"
#include "i18n_normalizer_addon.h"
#include "i18n_system_addon.h"
#include "i18n_timezone_addon.h"
#include "i18n_unicode_addon.h"
#include "js_utils.h"
#include "node_api.h"
#include "system_locale_manager_addon.h"
#include "unicode/datefmt.h"
#include "unicode/locid.h"
#include "unicode/smpdtfmt.h"
#include "unicode/translit.h"
#include "utils.h"
#include "variable_convertor.h"
#include "i18n_addon.h"

namespace OHOS {
namespace Global {
namespace I18n {
static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, 0xD001E00, "I18nJs" };
using namespace OHOS::HiviewDFX;

static thread_local napi_ref* g_brkConstructor = nullptr;
static thread_local napi_ref g_indexUtilConstructor = nullptr;
static thread_local napi_ref* g_transConstructor = nullptr;

I18nAddon::I18nAddon() : env_(nullptr) {}

I18nAddon::~I18nAddon()
{
    PhoneNumberFormat::CloseDynamicHandler();
}

void I18nAddon::Destructor(napi_env env, void *nativeObject, void *hint)
{
    if (!nativeObject) {
        return;
    }
    delete reinterpret_cast<I18nAddon *>(nativeObject);
    nativeObject = nullptr;
}

napi_value I18nAddon::InitI18nUtil(napi_env env, napi_value exports)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_STATIC_FUNCTION("unitConvert", UnitConvert),
        DECLARE_NAPI_STATIC_FUNCTION("getDateOrder", GetDateOrder),
        DECLARE_NAPI_STATIC_FUNCTION("getTimePeriodName", GetTimePeriodName)
    };
    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, "I18NUtil", NAPI_AUTO_LENGTH, JSUtils::DefaultConstructor, nullptr,
        sizeof(properties) / sizeof(napi_property_descriptor), properties, &constructor);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "InitI18nUtil: Define class failed when InitI18NUtil.");
        return nullptr;
    }

    status = napi_set_named_property(env, exports, "I18NUtil", constructor);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "InitI18nUtil: Set property failed when InitI18NUtil.");
        return nullptr;
    }
    return exports;
}

napi_value I18nAddon::Init(napi_env env, napi_value exports)
{
    napi_status initStatus = napi_ok;
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("getDisplayLanguage", I18nSystemAddon::GetDisplayLanguage),
        DECLARE_NAPI_FUNCTION("getDisplayCountry", I18nSystemAddon::GetDisplayCountry),
        DECLARE_NAPI_FUNCTION("getSystemLanguage", I18nSystemAddon::GetSystemLanguage),
        DECLARE_NAPI_FUNCTION("getSystemRegion", I18nSystemAddon::GetSystemRegion),
        DECLARE_NAPI_FUNCTION("getSystemLocale", I18nSystemAddon::GetSystemLocale),
        DECLARE_NAPI_FUNCTION("getCalendar", I18nCalendarAddon::GetCalendar),
        DECLARE_NAPI_FUNCTION("isRTL", IsRTL),
        DECLARE_NAPI_FUNCTION("getLineInstance", GetLineInstance),
        DECLARE_NAPI_FUNCTION("getInstance", GetIndexUtil),
        DECLARE_NAPI_FUNCTION("addPreferredLanguage", I18nSystemAddon::AddPreferredLanguage),
        DECLARE_NAPI_FUNCTION("removePreferredLanguage", I18nSystemAddon::RemovePreferredLanguage),
        DECLARE_NAPI_FUNCTION("getPreferredLanguageList", I18nSystemAddon::GetPreferredLanguageList),
        DECLARE_NAPI_FUNCTION("getFirstPreferredLanguage", I18nSystemAddon::GetFirstPreferredLanguage),
        DECLARE_NAPI_FUNCTION("is24HourClock", I18nSystemAddon::Is24HourClock),
        DECLARE_NAPI_FUNCTION("set24HourClock", I18nSystemAddon::Set24HourClock),
        DECLARE_NAPI_FUNCTION("getTimeZone", I18nTimeZoneAddon::GetI18nTimeZone),
        DECLARE_NAPI_PROPERTY("NormalizerMode", I18nNormalizerAddon::CreateI18NNormalizerModeEnum(env, initStatus))
    };
    initStatus = napi_define_properties(env, exports, sizeof(properties) / sizeof(napi_property_descriptor),
        properties);
    if (initStatus != napi_ok) {
        HiLog::Error(LABEL, "Failed to set properties at init");
        return nullptr;
    }
    return exports;
}

void GetOptionValue(napi_env env, napi_value options, const std::string &optionName,
    std::string &value)
{
    napi_value optionValue = nullptr;
    napi_valuetype type = napi_undefined;
    napi_status status = napi_typeof(env, options, &type);
    if (status != napi_ok && type != napi_object) {
        HiLog::Error(LABEL, "Get option failed, option is not an object");
        return;
    }
    bool hasProperty = false;
    napi_status propStatus = napi_has_named_property(env, options, optionName.c_str(), &hasProperty);
    if (propStatus == napi_ok && hasProperty) {
        status = napi_get_named_property(env, options, optionName.c_str(), &optionValue);
        if (status == napi_ok) {
            size_t len;
            napi_get_value_string_utf8(env, optionValue, nullptr, 0, &len);
            std::vector<char> optionBuf(len + 1);
            status = napi_get_value_string_utf8(env, optionValue, optionBuf.data(), len + 1, &len);
            if (status != napi_ok) {
                HiLog::Error(LABEL, "Failed to get string item");
                return;
            }
            value = optionBuf.data();
        }
    }
}

void GetOptionMap(napi_env env, napi_value option, std::map<std::string, std::string> &map)
{
    if (VariableConvertor::CheckNapiValueType(env, option)) {
        size_t len;
        napi_get_value_string_utf8(env, option, nullptr, 0, &len);
        std::vector<char> styleBuf(len + 1);
        napi_status status = napi_get_value_string_utf8(env, option, styleBuf.data(), len + 1, &len);
        if (status != napi_ok) {
            HiLog::Error(LABEL, "Failed to get string item");
            return;
        }
        map.insert(std::make_pair("unitDisplay", styleBuf.data()));
    }
}

napi_value I18nAddon::UnitConvert(napi_env env, napi_callback_info info)
{
    size_t argc = 5;
    napi_value argv[5] = { 0 };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        return nullptr;
    }
    std::string fromUnit;
    GetOptionValue(env, argv[0], "unit", fromUnit);
    std::string fromMeasSys;
    GetOptionValue(env, argv[0], "measureSystem", fromMeasSys);
    std::string toUnit;
    GetOptionValue(env, argv[1], "unit", toUnit);
    std::string toMeasSys;
    GetOptionValue(env, argv[1], "measureSystem", toMeasSys);
    double number = 0;
    napi_get_value_double(env, argv[2], &number); // 2 is the index of value
    int convertStatus = Convert(number, fromUnit, fromMeasSys, toUnit, toMeasSys);
    size_t len;
    napi_get_value_string_utf8(env, argv[3], nullptr, 0, &len); // 3 is the index of value
    std::vector<char> localeBuf(len + 1);
    // 3 is the index of value
    status = napi_get_value_string_utf8(env, argv[3], localeBuf.data(), len + 1, &len);
    if (status != napi_ok) {
        return nullptr;
    }
    std::vector<std::string> localeTags;
    localeTags.push_back(localeBuf.data());
    std::map<std::string, std::string> map = {};
    map.insert(std::make_pair("style", "unit"));
    if (!convertStatus) {
        map.insert(std::make_pair("unit", fromUnit));
    } else {
        map.insert(std::make_pair("unit", toUnit));
    }
    // 4 is the index of value
    GetOptionMap(env, argv[4], map);
    std::unique_ptr<NumberFormat> numberFmt = nullptr;
    numberFmt = std::make_unique<NumberFormat>(localeTags, map);
    std::string value = numberFmt->Format(number);
    napi_value result;
    status = napi_create_string_utf8(env, value.c_str(), NAPI_AUTO_LENGTH, &result);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Failed to create string item");
        return nullptr;
    }
    return result;
}

napi_value I18nAddon::GetDateOrder(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = { 0 };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        return nullptr;
    }
    size_t len = 0;
    napi_get_value_string_utf8(env, argv[0], nullptr, 0, &len);
    std::vector<char> languageBuf(len + 1);
    status = napi_get_value_string_utf8(env, argv[0], languageBuf.data(), len + 1, &len);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Failed to get locale string for GetDateOrder");
        return nullptr;
    }
    UErrorCode icuStatus = U_ZERO_ERROR;
    icu::Locale locale = icu::Locale::forLanguageTag(languageBuf.data(), icuStatus);
    if (icuStatus != U_ZERO_ERROR) {
        HiLog::Error(LABEL, "Failed to create locale for GetDateOrder");
        return nullptr;
    }
    icu::SimpleDateFormat* formatter = dynamic_cast<icu::SimpleDateFormat*>
        (icu::DateFormat::createDateInstance(icu::DateFormat::EStyle::kDefault, locale));
    if (icuStatus != U_ZERO_ERROR || formatter == nullptr) {
        HiLog::Error(LABEL, "Failed to create SimpleDateFormat");
        return nullptr;
    }
    std::string tempValue;
    icu::UnicodeString unistr;
    formatter->toPattern(unistr);
    delete formatter;
    unistr.toUTF8String<std::string>(tempValue);
    std::string value = ModifyOrder(tempValue);
    napi_value result;
    status = napi_create_string_utf8(env, value.c_str(), NAPI_AUTO_LENGTH, &result);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Failed to create string item");
        return nullptr;
    }
    return result;
}

std::string I18nAddon::ModifyOrder(std::string &pattern)
{
    int order[3] = { 0 }; // total 3 elements 'y', 'M'/'L', 'd'
    int lengths[4] = { 0 }; // first elements is the currently found elememnts, thus 4 elements totally.
    bool flag = true;
    for (size_t i = 0; i < pattern.length(); ++i) {
        char ch = pattern[i];
        if (flag && std::isalpha(ch)) {
            ProcessNormal(ch, order, 3, lengths, 4); // 3, 4 are lengths of these arrays
        } else if (ch == '\'') {
            flag = !flag;
        }
    }
    std::unordered_map<char, int> pattern2index = {
        { 'y', 1 },
        { 'L', 2 },
        { 'd', 3 },
    };
    std::string ret;
    for (int i = 0; i < 3; ++i) { // 3 is the size of orders
        auto it = pattern2index.find(order[i]);
        if (it == pattern2index.end()) {
            continue;
        }
        int index = it->second;
        if ((lengths[index] > 0) && (lengths[index] <= 6)) { // 6 is the max length of a filed
            ret.append(lengths[index], order[i]);
        }
        if (i < 2) { // 2 is the size of the order minus one
            ret.append(1, '-');
        }
    }
    return ret;
}

napi_value I18nAddon::GetTimePeriodName(napi_env env, napi_callback_info info)
{
    napi_value result;
    int32_t hour;
    std::string localeTag;
    if (GetParamOfGetTimePeriodName(env, info, localeTag, hour) == -1) {
        HiLog::Error(LABEL, "GetTimePeriodName param error");
        napi_create_string_utf8(env, "", NAPI_AUTO_LENGTH, &result);
        return result;
    }

    UErrorCode icuStatus = U_ZERO_ERROR;
    icu::Locale locale = icu::Locale::forLanguageTag(localeTag.data(), icuStatus);
    if (U_FAILURE(icuStatus) || !IsValidLocaleTag(locale)) {
        HiLog::Error(LABEL, "GetTimePeriodName does not support this locale");
        ErrorUtil::NapiThrow(env, I18N_NOT_VALID, true);
    }
    icu::SimpleDateFormat* formatter = dynamic_cast<icu::SimpleDateFormat*>
        (icu::DateFormat::createDateInstance(icu::DateFormat::EStyle::kDefault, locale));
    if (!formatter) {
        HiLog::Error(LABEL, "GetTimePeriodName Failed to create SimpleDateFormat");
        napi_create_string_utf8(env, "", NAPI_AUTO_LENGTH, &result);
        return result;
    }
    formatter->applyPattern("B");

    std::string temp;
    icu::UnicodeString name;
    icu::Calendar *calendar = icu::Calendar::createInstance(locale, icuStatus);
    calendar->set(UCalendarDateFields::UCAL_HOUR_OF_DAY, hour);
    formatter->format(calendar->getTime(icuStatus), name);
    name.toUTF8String(temp);
    napi_create_string_utf8(env, temp.c_str(), NAPI_AUTO_LENGTH, &result);
    delete formatter;
    delete calendar;
    return result;
}

int I18nAddon::GetParamOfGetTimePeriodName(napi_env env, napi_callback_info info, std::string &tag, int32_t &hour)
{
    size_t argc = 2;
    napi_value argv[2] = { 0 };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "GetTimePeriodName can't get parameters from getTimePerioudName.");
        return -1;
    }

    napi_valuetype valueType = napi_valuetype::napi_undefined;
    napi_typeof(env, argv[0], &valueType);
    if (valueType != napi_valuetype::napi_number) {
        HiLog::Error(LABEL, "GetTimePeriodName Parameter type does not match");
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, true);
    }
    status = napi_get_value_int32(env, argv[0], &hour);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "GetTimePeriodName can't get number from js param");
        return -1;
    }

    valueType = napi_valuetype::napi_undefined;
    napi_typeof(env, argv[1], &valueType);
    if (valueType == napi_valuetype::napi_null || valueType == napi_valuetype::napi_undefined) {
        tag = LocaleConfig::GetSystemLocale();
    } else if (valueType == napi_valuetype::napi_string) {
        int code = 0;
        tag = VariableConvertor::GetString(env, argv[1], code);
        if (code) {
            HiLog::Error(LABEL, "GetTimePeriodName can't get string from js param");
            return -1;
        }
    } else {
        HiLog::Error(LABEL, "GetTimePeriodName Parameter type does not match");
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, true);
    }
    return 0;
}

void I18nAddon::ProcessNormal(char ch, int *order, size_t orderSize, int *lengths, size_t lengsSize)
{
    char adjust;
    int index = -1;
    if (ch == 'd') {
        adjust = 'd';
        index = 3; // 3 is the index of 'd'
    } else if ((ch == 'L') || (ch == 'M')) {
        adjust = 'L';
        index = 2; // 2 is the index of 'L'
    } else if (ch == 'y') {
        adjust = 'y';
        index = 1;
    } else {
        return;
    }
    if ((index < 0) || (index >= static_cast<int>(lengsSize))) {
        return;
    }
    if (lengths[index] == 0) {
        if (lengths[0] >= 3) { // 3 is the index of order
            return;
        }
        order[lengths[0]] = static_cast<int>(adjust);
        ++lengths[0];
        lengths[index] = 1;
    } else {
        ++lengths[index];
    }
}

napi_value I18nAddon::InitI18nTransliterator(napi_env env, napi_value exports)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("transform", Transform),
    };
    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, "Transliterator", NAPI_AUTO_LENGTH, I18nTransliteratorConstructor,
        nullptr, sizeof(properties) / sizeof(napi_property_descriptor), properties, &constructor);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "InitI18nTransliterator: Failed to define transliterator class at Init");
        return nullptr;
    }
    exports = I18nAddon::InitTransliterator(env, exports);
    g_transConstructor = new (std::nothrow) napi_ref;
    if (!g_transConstructor) {
        HiLog::Error(LABEL, "InitI18nTransliterator: Failed to create trans ref at init");
        return nullptr;
    }
    status = napi_create_reference(env, constructor, 1, g_transConstructor);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "InitI18nTransliterator: Failed to create trans reference at init");
        return nullptr;
    }
    return exports;
}

napi_value I18nAddon::InitTransliterator(napi_env env, napi_value exports)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_STATIC_FUNCTION("getAvailableIDs", GetAvailableIDs),
        DECLARE_NAPI_STATIC_FUNCTION("getInstance", GetTransliteratorInstance)
    };
    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, "I18nTransliterator", NAPI_AUTO_LENGTH, JSUtils::DefaultConstructor,
        nullptr, sizeof(properties) / sizeof(napi_property_descriptor), properties, &constructor);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "InitTransliterator: Failed to define class Transliterator.");
        return nullptr;
    }
    status = napi_set_named_property(env, exports, "Transliterator", constructor);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "InitTransliterator: Set property failed When InitTransliterator.");
        return nullptr;
    }
    return exports;
}

napi_value I18nAddon::I18nTransliteratorConstructor(napi_env env, napi_callback_info info)
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
    napi_typeof(env, argv[0], &valueType);
    if (valueType != napi_valuetype::napi_string) {
        napi_throw_type_error(env, nullptr, "I18nTransliteratorConstructor: Parameter type does not match");
        return nullptr;
    }
    int32_t code = 0;
    std::string idTag = VariableConvertor::GetString(env, argv[0], code);
    if (code) {
        return nullptr;
    }
    std::unique_ptr<I18nAddon> obj = nullptr;
    obj = std::make_unique<I18nAddon>();
    status =
        napi_wrap(env, thisVar, reinterpret_cast<void *>(obj.get()), I18nAddon::Destructor, nullptr, nullptr);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "I18nTransliteratorConstructor: TransliteratorConstructor: Wrap II18nAddon failed");
        return nullptr;
    }
    if (!obj->InitTransliteratorContext(env, info, idTag)) {
        obj.release();
        return nullptr;
    }
    obj.release();
    return thisVar;
}

bool I18nAddon::InitTransliteratorContext(napi_env env, napi_callback_info info, const std::string &idTag)
{
    UErrorCode status = U_ZERO_ERROR;
    icu::UnicodeString unistr = icu::UnicodeString::fromUTF8(idTag);
    icu::Transliterator *trans = icu::Transliterator::createInstance(unistr, UTransDirection::UTRANS_FORWARD, status);
    if ((status != U_ZERO_ERROR) || (trans == nullptr)) {
        return false;
    }
    transliterator_ = std::unique_ptr<icu::Transliterator>(trans);
    return transliterator_ != nullptr;
}

napi_value I18nAddon::Transform(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = { nullptr };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    I18nAddon *obj = nullptr;
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->transliterator_) {
        HiLog::Error(LABEL, "Get Transliterator object failed");
        return nullptr;
    }
    if (!argv[0]) {
        return nullptr;
    }
    napi_valuetype valueType = napi_valuetype::napi_undefined;
    napi_typeof(env, argv[0], &valueType);
    if (valueType != napi_valuetype::napi_string) {
        napi_throw_type_error(env, nullptr, "Parameter type does not match");
        return nullptr;
    }
    size_t len = 0;
    status = napi_get_value_string_utf8(env, argv[0], nullptr, 0, &len);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Get field length failed");
        return nullptr;
    }
    std::vector<char> buf(len + 1);
    status = napi_get_value_string_utf8(env, argv[0], buf.data(), len + 1, &len);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Get string value failed");
        return nullptr;
    }
    icu::UnicodeString unistr = icu::UnicodeString::fromUTF8(buf.data());
    obj->transliterator_->transliterate(unistr);
    std::string temp;
    unistr.toUTF8String(temp);
    napi_value value;
    status = napi_create_string_utf8(env, temp.c_str(), NAPI_AUTO_LENGTH, &value);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Get field length failed");
        return nullptr;
    }
    return value;
}

napi_value I18nAddon::GetAvailableIDs(napi_env env, napi_callback_info info)
{
    size_t argc = 0;
    napi_value *argv = nullptr;
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        return nullptr;
    }
    UErrorCode icuStatus = U_ZERO_ERROR;
    icu::StringEnumeration *strenum = icu::Transliterator::getAvailableIDs(icuStatus);
    if (icuStatus != U_ZERO_ERROR) {
        HiLog::Error(LABEL, "Failed to get available ids");
        if (strenum) {
            delete strenum;
        }
        return nullptr;
    }

    napi_value result = nullptr;
    napi_create_array(env, &result);
    uint32_t i = 0;
    const char *temp = nullptr;
    if (strenum == nullptr) {
        return nullptr;
    }
    while ((temp = strenum->next(nullptr, icuStatus)) != nullptr) {
        if (icuStatus != U_ZERO_ERROR) {
            break;
        }
        napi_value val = nullptr;
        napi_create_string_utf8(env, temp, strlen(temp), &val);
        napi_set_element(env, result, i, val);
        ++i;
    }
    delete strenum;
    return result;
}

napi_value I18nAddon::GetTransliteratorInstance(napi_env env, napi_callback_info info)
{
    size_t argc = 1; // retrieve 2 arguments
    napi_value argv[1] = { 0 };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    napi_value constructor = nullptr;
    napi_status status = napi_get_reference_value(env, *g_transConstructor, &constructor);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Failed to create reference at GetCalendar");
        return nullptr;
    }
    napi_value result = nullptr;
    status = napi_new_instance(env, constructor, 1, argv, &result); // 2 arguments
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Get Transliterator create instance failed");
        return nullptr;
    }
    return result;
}

napi_value I18nAddon::IsRTL(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = { 0 };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        return nullptr;
    }
    size_t len = 0;
    napi_get_value_string_utf8(env, argv[0], nullptr, 0, &len);
    std::vector<char> localeBuf(len + 1);
    status = napi_get_value_string_utf8(env, argv[0], localeBuf.data(), len + 1, &len);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Failed to get string item");
        return nullptr;
    }
    bool isRTL = LocaleConfig::IsRTL(localeBuf.data());
    napi_value result = nullptr;
    status = napi_get_boolean(env, isRTL, &result);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "IsRTL failed");
        return nullptr;
    }
    return result;
}

napi_value I18nAddon::InitPhoneNumberFormat(napi_env env, napi_value exports)
{
    napi_status status = napi_ok;
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("isValidNumber", IsValidPhoneNumber),
        DECLARE_NAPI_FUNCTION("format", FormatPhoneNumber),
        DECLARE_NAPI_FUNCTION("getLocationName", GetLocationName)
    };

    napi_value constructor;
    status = napi_define_class(env, "PhoneNumberFormat", NAPI_AUTO_LENGTH, PhoneNumberFormatConstructor, nullptr,
                               sizeof(properties) / sizeof(napi_property_descriptor), properties, &constructor);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Define class failed when InitPhoneNumberFormat");
        return nullptr;
    }

    status = napi_set_named_property(env, exports, "PhoneNumberFormat", constructor);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Set property failed when InitPhoneNumberFormat");
        return nullptr;
    }
    return exports;
}

napi_value I18nAddon::PhoneNumberFormatConstructor(napi_env env, napi_callback_info info)
{
    size_t argc = 2;
    napi_value argv[2] = { 0 };
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
    size_t len = 0;
    status = napi_get_value_string_utf8(env, argv[0], nullptr, 0, &len);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Get country tag length failed");
        return nullptr;
    }
    std::vector<char> country (len + 1);
    status = napi_get_value_string_utf8(env, argv[0], country.data(), len + 1, &len);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Get country tag failed");
        return nullptr;
    }
    std::map<std::string, std::string> options;
    std::string typeStr;
    GetOptionValue(env, argv[1], "type", typeStr);
    options.insert(std::make_pair("type", typeStr));
    std::unique_ptr<I18nAddon> obj = nullptr;
    obj = std::make_unique<I18nAddon>();
    status = napi_wrap(env, thisVar, reinterpret_cast<void *>(obj.get()),
                       I18nAddon::Destructor, nullptr, nullptr);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Wrap I18nAddon failed");
        return nullptr;
    }
    if (!obj->InitPhoneNumberFormatContext(env, info, country.data(), options)) {
        return nullptr;
    }
    obj.release();
    return thisVar;
}

bool I18nAddon::InitPhoneNumberFormatContext(napi_env env, napi_callback_info info, const std::string &country,
                                             const std::map<std::string, std::string> &options)
{
    napi_value global = nullptr;
    napi_status status = napi_get_global(env, &global);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Get global failed");
        return false;
    }
    env_ = env;
    phonenumberfmt_ = PhoneNumberFormat::CreateInstance(country, options);

    return phonenumberfmt_ != nullptr;
}

napi_value I18nAddon::IsValidPhoneNumber(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = { 0 };
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
        HiLog::Error(LABEL, "Get phone number length failed");
        return nullptr;
    }
    std::vector<char> buf(len + 1);
    status = napi_get_value_string_utf8(env, argv[0], buf.data(), len + 1, &len);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Get phone number failed");
        return nullptr;
    }

    I18nAddon *obj = nullptr;
    status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->phonenumberfmt_) {
        HiLog::Error(LABEL, "GetPhoneNumberFormat object failed");
        return nullptr;
    }

    bool isValid = obj->phonenumberfmt_->isValidPhoneNumber(buf.data());

    napi_value result = nullptr;
    status = napi_get_boolean(env, isValid, &result);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Create boolean failed");
        return nullptr;
    }

    return result;
}

napi_value I18nAddon::GetLocationName(napi_env env, napi_callback_info info)
{
    size_t argc = 2;
    napi_value argv[2] = {0, 0};
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);

    int32_t code = 0;
    std::string number = VariableConvertor::GetString(env, argv[0], code);
    if (code) {
        return nullptr;
    }
    std::string language = VariableConvertor::GetString(env, argv[1], code);
    if (code) {
        return nullptr;
    }

    I18nAddon *obj = nullptr;
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->phonenumberfmt_) {
        HiLog::Error(LABEL, "GetPhoneNumberFormat object failed");
        return nullptr;
    }

    std::string resStr = obj->phonenumberfmt_->getLocationName(number.data(), language.data());
    napi_value result = nullptr;
    status = napi_create_string_utf8(env, resStr.c_str(), NAPI_AUTO_LENGTH, &result);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Create result string failed");
        return nullptr;
    }

    return result;
}

napi_value I18nAddon::FormatPhoneNumber(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = { 0 };
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
        HiLog::Error(LABEL, "Get phone number length failed");
        return nullptr;
    }
    std::vector<char> buf(len + 1);
    status = napi_get_value_string_utf8(env, argv[0], buf.data(), len + 1, &len);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Get phone number failed");
        return nullptr;
    }

    I18nAddon *obj = nullptr;
    status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->phonenumberfmt_) {
        HiLog::Error(LABEL, "Get PhoneNumberFormat object failed");
        return nullptr;
    }

    std::string formattedPhoneNumber = obj->phonenumberfmt_->format(buf.data());

    napi_value result = nullptr;
    status = napi_create_string_utf8(env, formattedPhoneNumber.c_str(), NAPI_AUTO_LENGTH, &result);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Create format phone number failed");
        return nullptr;
    }
    return result;
}

napi_value I18nAddon::InitI18nIndexUtil(napi_env env, napi_value exports)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("getIndexList", GetIndexList),
        DECLARE_NAPI_FUNCTION("addLocale", AddLocale),
        DECLARE_NAPI_FUNCTION("getIndex", GetIndex)
    };

    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, "IndexUtil", NAPI_AUTO_LENGTH, I18nIndexUtilConstructor, nullptr,
        sizeof(properties) / sizeof(napi_property_descriptor), properties, &constructor);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "InitI18nIndexUtil: Define class failed when InitI18nIndexUtil.");
        return nullptr;
    }
    exports = I18nAddon::InitIndexUtil(env, exports);
    status = napi_create_reference(env, constructor, 1, &g_indexUtilConstructor);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "InitI18nIndexUtil: Failed to create reference at init.");
        return nullptr;
    }
    return exports;
}

napi_value I18nAddon::InitIndexUtil(napi_env env, napi_value exports)
{
    napi_property_descriptor properties[] = {};
    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, "I18nIndexUtil", NAPI_AUTO_LENGTH, JSUtils::DefaultConstructor,
        nullptr, sizeof(properties) / sizeof(napi_property_descriptor), properties, &constructor);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "InitIndexUtil: Failed to define class IndexUtil.");
        return nullptr;
    }
    status = napi_set_named_property(env, exports, "IndexUtil", constructor);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "InitIndexUtil: Set property failed When InitIndexUtil.");
        return nullptr;
    }
    return exports;
}

napi_value I18nAddon::I18nBreakIteratorConstructor(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = { nullptr };
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
    std::unique_ptr<I18nAddon> obj = nullptr;
    obj = std::make_unique<I18nAddon>();
    status =
        napi_wrap(env, thisVar, reinterpret_cast<void *>(obj.get()), I18nAddon::Destructor, nullptr, nullptr);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "BreakIteratorConstructor: Wrap II18nAddon failed");
        return nullptr;
    }
    obj->brkiter_ = std::make_unique<I18nBreakIterator>(localeTag);
    if (!obj->brkiter_) {
        HiLog::Error(LABEL, "Wrap BreakIterator failed");
        return nullptr;
    }
    obj.release();
    return thisVar;
}

napi_value I18nAddon::InitI18nBreakIterator(napi_env env, napi_value exports)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("current", Current),
        DECLARE_NAPI_FUNCTION("first", First),
        DECLARE_NAPI_FUNCTION("last", Last),
        DECLARE_NAPI_FUNCTION("next", Next),
        DECLARE_NAPI_FUNCTION("previous", Previous),
        DECLARE_NAPI_FUNCTION("setLineBreakText", SetText),
        DECLARE_NAPI_FUNCTION("following", Following),
        DECLARE_NAPI_FUNCTION("getLineBreakText", GetText),
        DECLARE_NAPI_FUNCTION("isBoundary", IsBoundary)
    };
    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, "BreakIterator", NAPI_AUTO_LENGTH, I18nBreakIteratorConstructor,
        nullptr, sizeof(properties) / sizeof(napi_property_descriptor), properties, &constructor);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "InitI18nBreakIterator: Failed to define class BreakIterator at Init");
        return nullptr;
    }
    exports = I18nAddon::InitBreakIterator(env, exports);
    g_brkConstructor = new (std::nothrow) napi_ref;
    if (!g_brkConstructor) {
        HiLog::Error(LABEL, "InitI18nBreakIterator: Failed to create brkiterator ref at init");
        return nullptr;
    }
    status = napi_create_reference(env, constructor, 1, g_brkConstructor);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "InitI18nBreakIterator: Failed to create reference g_brkConstructor at init");
        return nullptr;
    }
    return exports;
}

napi_value I18nAddon::InitBreakIterator(napi_env env, napi_value exports)
{
    napi_property_descriptor properties[] = {};
    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, "I18nBreakIterator", NAPI_AUTO_LENGTH, JSUtils::DefaultConstructor,
        nullptr, sizeof(properties) / sizeof(napi_property_descriptor), properties, &constructor);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "InitBreakIterator: Failed to define class BreakIterator.");
        return nullptr;
    }
    status = napi_set_named_property(env, exports, "BreakIterator", constructor);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "InitBreakIterator: Set property failed When InitBreakIterator.");
        return nullptr;
    }
    return exports;
}

napi_value I18nAddon::GetLineInstance(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = { nullptr };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    napi_value constructor = nullptr;
    napi_status status = napi_get_reference_value(env, *g_brkConstructor, &constructor);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Failed to create reference at GetLineInstance");
        return nullptr;
    }
    if (!argv[0]) {
        return nullptr;
    }
    napi_value result = nullptr;
    status = napi_new_instance(env, constructor, 1, argv, &result); // 1 arguments
    if (status != napi_ok) {
        HiLog::Error(LABEL, "GetLineInstance create instance failed");
        return nullptr;
    }
    return result;
}

napi_value I18nAddon::Current(napi_env env, napi_callback_info info)
{
    size_t argc = 0;
    napi_value *argv = nullptr;
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    I18nAddon *obj = nullptr;
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->brkiter_) {
        HiLog::Error(LABEL, "Get BreakIterator object failed");
        return nullptr;
    }
    int value = obj->brkiter_->Current();
    napi_value result = nullptr;
    status = napi_create_int32(env, value, &result);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Create int32_t value failed");
        return nullptr;
    }
    return result;
}

napi_value I18nAddon::First(napi_env env, napi_callback_info info)
{
    size_t argc = 0;
    napi_value *argv = nullptr;
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    I18nAddon *obj = nullptr;
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->brkiter_) {
        HiLog::Error(LABEL, "Get BreakIterator object failed");
        return nullptr;
    }
    int value = obj->brkiter_->First();
    napi_value result = nullptr;
    status = napi_create_int32(env, value, &result);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Create int32_t value failed");
        return nullptr;
    }
    return result;
}

napi_value I18nAddon::Last(napi_env env, napi_callback_info info)
{
    size_t argc = 0;
    napi_value *argv = nullptr;
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    I18nAddon *obj = nullptr;
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->brkiter_) {
        HiLog::Error(LABEL, "Get BreakIterator object failed");
        return nullptr;
    }
    int value = obj->brkiter_->Last();
    napi_value result = nullptr;
    status = napi_create_int32(env, value, &result);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Create int32_t value failed");
        return nullptr;
    }
    return result;
}

napi_value I18nAddon::Previous(napi_env env, napi_callback_info info)
{
    size_t argc = 0;
    napi_value *argv = nullptr;
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    I18nAddon *obj = nullptr;
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->brkiter_) {
        HiLog::Error(LABEL, "Get BreakIterator object failed");
        return nullptr;
    }
    int value = obj->brkiter_->Previous();
    napi_value result = nullptr;
    status = napi_create_int32(env, value, &result);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Create int32_t value failed");
        return nullptr;
    }
    return result;
}

napi_value I18nAddon::Next(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = { nullptr };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    I18nAddon *obj = nullptr;
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->brkiter_) {
        HiLog::Error(LABEL, "Get BreakIterator object failed");
        return nullptr;
    }
    int value = 1;
    if (VariableConvertor::CheckNapiValueType(env, argv[0])) {
        napi_valuetype valueType = napi_valuetype::napi_undefined;
        napi_typeof(env, argv[0], &valueType);
        if (valueType != napi_valuetype::napi_number) {
            napi_throw_type_error(env, nullptr, "Parameter type does not match");
            return nullptr;
        }
        status = napi_get_value_int32(env, argv[0], &value);
        if (status != napi_ok) {
            HiLog::Error(LABEL, "Retrieve next value failed");
            return nullptr;
        }
    }
    value = obj->brkiter_->Next(value);
    napi_value result = nullptr;
    status = napi_create_int32(env, value, &result);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Create int32_t value failed");
        return nullptr;
    }
    return result;
}

napi_value I18nAddon::SetText(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = { nullptr };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    I18nAddon *obj = nullptr;
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->brkiter_) {
        HiLog::Error(LABEL, "Get BreakIterator object failed");
        return nullptr;
    }
    if (!argv[0]) {
        return nullptr;
    }
    napi_valuetype valueType = napi_valuetype::napi_undefined;
    napi_typeof(env, argv[0], &valueType);
    if (valueType != napi_valuetype::napi_string) {
        napi_throw_type_error(env, nullptr, "Parameter type does not match");
        return nullptr;
    }
    size_t len = 0;
    status = napi_get_value_string_utf8(env, argv[0], nullptr, 0, &len);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Get field length failed");
        return nullptr;
    }
    std::vector<char> buf(len + 1);
    status = napi_get_value_string_utf8(env, argv[0], buf.data(), len + 1, &len);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Get string value failed");
        return nullptr;
    }
    obj->brkiter_->SetText(buf.data());
    return nullptr;
}

napi_value I18nAddon::GetText(napi_env env, napi_callback_info info)
{
    size_t argc = 0;
    napi_value *argv = nullptr;
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    I18nAddon *obj = nullptr;
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->brkiter_) {
        HiLog::Error(LABEL, "Get BreakIterator object failed");
        return nullptr;
    }
    napi_value value = nullptr;
    std::string temp;
    obj->brkiter_->GetText(temp);
    status = napi_create_string_utf8(env, temp.c_str(), NAPI_AUTO_LENGTH, &value);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Get field length failed");
        return nullptr;
    }
    return value;
}

napi_value I18nAddon::Following(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = { nullptr };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    I18nAddon *obj = nullptr;
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->brkiter_) {
        HiLog::Error(LABEL, "Get BreakIterator object failed");
        return nullptr;
    }
    if (!argv[0]) {
        return nullptr;
    }
    napi_valuetype valueType = napi_valuetype::napi_undefined;
    napi_typeof(env, argv[0], &valueType);
    if (valueType != napi_valuetype::napi_number) {
        napi_throw_type_error(env, nullptr, "Parameter type does not match");
        return nullptr;
    }
    int value;
    status = napi_get_value_int32(env, argv[0], &value);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Retrieve following value failed");
        return nullptr;
    }
    value = obj->brkiter_->Following(value);
    napi_value result = nullptr;
    status = napi_create_int32(env, value, &result);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Create int32_t value failed");
        return nullptr;
    }
    return result;
}

napi_value I18nAddon::IsBoundary(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = { nullptr };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    I18nAddon *obj = nullptr;
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->brkiter_) {
        HiLog::Error(LABEL, "Get BreakIterator object failed");
        return nullptr;
    }
    if (!argv[0]) {
        return nullptr;
    }
    napi_valuetype valueType = napi_valuetype::napi_undefined;
    int value;
    napi_typeof(env, argv[0], &valueType);
    if (valueType != napi_valuetype::napi_number) {
        napi_throw_type_error(env, nullptr, "Parameter type does not match");
        return nullptr;
    }
    status = napi_get_value_int32(env, argv[0], &value);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Retrieve following value failed");
        return nullptr;
    }
    bool boundary = obj->brkiter_->IsBoundary(value);
    napi_value result = nullptr;
    status = napi_get_boolean(env, boundary, &result);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Create boolean failed");
        return nullptr;
    }
    return result;
}

napi_value I18nAddon::I18nIndexUtilConstructor(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = { nullptr };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        return nullptr;
    }
    std::string localeTag = "";
    if (argc > 0) {
        napi_valuetype valueType = napi_valuetype::napi_undefined;
        napi_typeof(env, argv[0], &valueType);
        if (valueType != napi_valuetype::napi_string) {
            napi_throw_type_error(env, nullptr, "Parameter type does not match");
            return nullptr;
        }
        size_t len = 0;
        status = napi_get_value_string_utf8(env, argv[0], nullptr, 0, &len);
        if (status != napi_ok) {
            HiLog::Error(LABEL, "IndexUtilConstructor: Get locale length failed");
            return nullptr;
        }
        std::vector<char> localeBuf(len + 1);
        status = napi_get_value_string_utf8(env, argv[0], localeBuf.data(), len + 1, &len);
        if (status != napi_ok) {
            HiLog::Error(LABEL, "IndexUtilConstructor: Get locale failed");
            return nullptr;
        }
        localeTag = localeBuf.data();
    }
    std::unique_ptr<I18nAddon> obj = nullptr;
    obj = std::make_unique<I18nAddon>();
    status =
        napi_wrap(env, thisVar, reinterpret_cast<void *>(obj.get()), I18nAddon::Destructor, nullptr, nullptr);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "IndexUtilConstructor: Wrap II18nAddon failed");
        return nullptr;
    }
    if (!obj->InitIndexUtilContext(env, info, localeTag)) {
        return nullptr;
    }
    obj.release();
    return thisVar;
}

bool I18nAddon::InitIndexUtilContext(napi_env env, napi_callback_info info, const std::string &localeTag)
{
    napi_value global = nullptr;
    napi_status status = napi_get_global(env, &global);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Get global failed");
        return false;
    }
    env_ = env;
    indexUtil_ = std::make_unique<IndexUtil>(localeTag);
    return indexUtil_ != nullptr;
}

napi_value I18nAddon::GetIndexUtil(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = { 0 };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    napi_value constructor = nullptr;
    napi_status status = napi_get_reference_value(env, g_indexUtilConstructor, &constructor);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Failed to create reference at GetIndexUtil");
        return nullptr;
    }
    napi_value result = nullptr;
    if (!VariableConvertor::CheckNapiValueType(env, argv[0])) {
        status = napi_new_instance(env, constructor, 0, argv, &result);
    } else {
        status = napi_new_instance(env, constructor, 1, argv, &result);
    }
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Get calendar create instance failed");
        return nullptr;
    }
    return result;
}

napi_value I18nAddon::GetIndexList(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, &data);

    I18nAddon *obj = nullptr;
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->indexUtil_) {
        HiLog::Error(LABEL, "GetPhoneNumberFormat object failed");
        return nullptr;
    }

    std::vector<std::string> indexList = obj->indexUtil_->GetIndexList();
    napi_value result = nullptr;
    status = napi_create_array_with_length(env, indexList.size(), &result);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Failed to create array");
        return nullptr;
    }
    for (size_t i = 0; i < indexList.size(); i++) {
        napi_value element = nullptr;
        status = napi_create_string_utf8(env, indexList[i].c_str(), NAPI_AUTO_LENGTH, &element);
        if (status != napi_ok) {
            HiLog::Error(LABEL, "Failed to create string item");
            return nullptr;
        }
        status = napi_set_element(env, result, i, element);
        if (status != napi_ok) {
            HiLog::Error(LABEL, "Failed to set array item");
            return nullptr;
        }
    }
    return result;
}

napi_value I18nAddon::AddLocale(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = { 0 };
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
        HiLog::Error(LABEL, "Get locale length failed");
        return nullptr;
    }
    std::vector<char> buf(len + 1);
    status = napi_get_value_string_utf8(env, argv[0], buf.data(), len + 1, &len);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Get locale failed");
        return nullptr;
    }
    I18nAddon *obj = nullptr;
    status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->indexUtil_) {
        HiLog::Error(LABEL, "Get IndexUtil object failed");
        return nullptr;
    }
    obj->indexUtil_->AddLocale(buf.data());
    return nullptr;
}

napi_value I18nAddon::GetIndex(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = { 0 };
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
        HiLog::Error(LABEL, "Get String length failed");
        return nullptr;
    }
    std::vector<char> buf(len + 1);
    status = napi_get_value_string_utf8(env, argv[0], buf.data(), len + 1, &len);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Get String failed");
        return nullptr;
    }
    I18nAddon *obj = nullptr;
    status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->indexUtil_) {
        HiLog::Error(LABEL, "Get IndexUtil object failed");
        return nullptr;
    }
    std::string index = obj->indexUtil_->GetIndex(buf.data());
    napi_value result = nullptr;
    status = napi_create_string_utf8(env, index.c_str(), NAPI_AUTO_LENGTH, &result);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "GetIndex Failed");
        return nullptr;
    }
    return result;
}

napi_value I18nAddon::ObjectConstructor(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, &data);
    if (status != napi_ok) {
        return nullptr;
    }
    std::unique_ptr<I18nAddon> obj = nullptr;
    obj = std::make_unique<I18nAddon>();
    status =
        napi_wrap(env, thisVar, reinterpret_cast<void *>(obj.get()), I18nAddon::Destructor, nullptr, nullptr);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Wrap I18nAddon failed");
        return nullptr;
    }
    obj.release();
    return thisVar;
}

napi_value I18nAddon::InitUtil(napi_env env, napi_value exports)
{
    napi_status status = napi_ok;
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("unitConvert", UnitConvert)
    };

    napi_value constructor = nullptr;
    status = napi_define_class(env, "Util", NAPI_AUTO_LENGTH, ObjectConstructor, nullptr,
        sizeof(properties) / sizeof(napi_property_descriptor), properties, &constructor);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Define class failed when InitUtil");
        return nullptr;
    }

    status = napi_set_named_property(env, exports, "Util", constructor);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Set property failed when InitUtil");
        return nullptr;
    }
    return exports;
}

napi_value Init(napi_env env, napi_value exports)
{
    napi_value val = I18nAddon::Init(env, exports);
    val = I18nAddon::InitPhoneNumberFormat(env, val);
    val = I18nAddon::InitI18nBreakIterator(env, val);
    val = I18nCalendarAddon::InitI18nCalendar(env, val);
    val = I18nAddon::InitI18nIndexUtil(env, val);
    val = I18nAddon::InitI18nUtil(env, val);
    val = I18nTimeZoneAddon::InitI18nTimeZone(env, val);
    val = I18nAddon::InitI18nTransliterator(env, val);
    val = I18nUnicodeAddon::InitCharacter(env, val);
    val = I18nUnicodeAddon::InitI18nUnicode(env, val);
    val = I18nAddon::InitUtil(env, val);
    val = I18nNormalizerAddon::InitI18nNormalizer(env, val);
    val = SystemLocaleManagerAddon::InitSystemLocaleManager(env, val);
    val = I18nSystemAddon::InitI18nSystem(env, val);
    val = HolidayManagerAddon::InitHolidayManager(env, val);
    val = EntityRecognizerAddon::InitEntityRecognizer(env, val);
    return val;
}

static napi_module g_i18nModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = Init,
    .nm_modname = "i18n",
    .nm_priv = nullptr,
    .reserved = { 0 }
};

extern "C" __attribute__((constructor)) void I18nRegister()
{
    napi_module_register(&g_i18nModule);
}
} // namespace I18n
} // namespace Global
} // namespace OHOS
