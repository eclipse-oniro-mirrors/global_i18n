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
#include "intl_locale_addon.h"

#include "error_util.h"
#include "i18n_hilog.h"
#include "js_lifecycle_managers.h"
#include "js_utils.h"

namespace OHOS {
namespace Global {
namespace I18n {
const std::string NOT_STRING_OBJECT_ERROR_MESSAGE = "tag is not String or Object";

IntlLocaleAddon::IntlLocaleAddon()
{
}

IntlLocaleAddon::~IntlLocaleAddon()
{
}

void IntlLocaleAddon::Destructor(napi_env env, void *nativeObject, void *hint)
{
    if (!nativeObject) {
        return;
    }
    delete reinterpret_cast<IntlLocaleAddon*>(nativeObject);
    nativeObject = nullptr;
}

napi_value IntlLocaleAddon::SetProperty(napi_env env, napi_callback_info info)
{
    // do nothing but provided as an input parameter for DECLARE_NAPI_GETTER_SETTER;
    napi_value result = nullptr;
    napi_status status = napi_get_undefined(env, &result);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("IntlLocaleAddon::SetProperty: Create undefined failed.");
        return nullptr;
    }
    return result;
}

napi_value IntlLocaleAddon::InitIntlLocale(napi_env env, napi_value exports)
{
    HandleScope scope(env, "InitIntlLocale");
    if (!scope.IsOpen()) {
        return nullptr;
    }

    napi_property_descriptor properties[] = {
        DECLARE_NAPI_GETTER_SETTER("language", GetLanguage, SetProperty),
        DECLARE_NAPI_GETTER_SETTER("baseName", GetBaseName, SetProperty),
        DECLARE_NAPI_GETTER_SETTER("region", GetRegion, SetProperty),
        DECLARE_NAPI_GETTER_SETTER("script", GetScript, SetProperty),
        DECLARE_NAPI_GETTER_SETTER("calendar", GetCalendar, SetProperty),
        DECLARE_NAPI_GETTER_SETTER("collation", GetCollation, SetProperty),
        DECLARE_NAPI_GETTER_SETTER("hourCycle", GetHourCycle, SetProperty),
        DECLARE_NAPI_GETTER_SETTER("numberingSystem", GetNumberingSystem, SetProperty),
        DECLARE_NAPI_GETTER_SETTER("numeric", GetNumeric, SetProperty),
        DECLARE_NAPI_GETTER_SETTER("caseFirst", GetCaseFirst, SetProperty),
        DECLARE_NAPI_FUNCTION("toString", ToString),
        DECLARE_NAPI_FUNCTION("minimize", Minimize),
        DECLARE_NAPI_FUNCTION("maximize", Maximize),
    };

    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, "Locale", NAPI_AUTO_LENGTH, IntlLocaleConstructor, nullptr,
        sizeof(properties) / sizeof(napi_property_descriptor), properties, &constructor);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("IntlLocaleAddon::InitIntlLocale: Define class failed when InitIntlLocale.");
        return nullptr;
    }

    status = napi_set_named_property(env, exports, "Locale", constructor);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("IntlLocaleAddon::InitIntlLocale: Set property failed when InitIntlLocale.");
        return nullptr;
    }
    return exports;
}

napi_value IntlLocaleAddon::IntlLocaleConstructor(napi_env env, napi_callback_info info)
{
    size_t argc = 2;
    napi_value argv[2] = { nullptr };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("IntlLocaleAddon::IntlLocaleConstructor: Get cb info failed.");
        return nullptr;
    } else if (argc < 1) {
        ErrorUtil::NapiThrowUndefined(env, NOT_STRING_OBJECT_ERROR_MESSAGE);
        return nullptr;
    }

    napi_valuetype type = napi_valuetype::napi_undefined;
    status = napi_typeof(env, argv[0], &type);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("IntlLocaleAddon::IntlLocaleConstructor: Get argv[0] type failed.");
        return nullptr;
    } else if (type != napi_valuetype::napi_string && type != napi_valuetype::napi_object) {
        ErrorUtil::NapiThrowUndefined(env, NOT_STRING_OBJECT_ERROR_MESSAGE);
        return nullptr;
    }

    std::string localeTag = IntlLocaleAddon::ParseLocaleTag(env, argv[0], type);

    std::unordered_map<std::string, std::string> configs = {};
    if (argc > 1) {
        IntlLocaleAddon::ParseConfigs(env, argv[1], configs);
    }
    IntlLocaleAddon* obj = new (std::nothrow) IntlLocaleAddon();
    if (obj == nullptr) {
        HILOG_ERROR_I18N("IntlLocaleAddon::IntlLocaleConstructor: Create IntlLocaleAddon failed.");
        return nullptr;
    }
    status = napi_wrap_s(env, thisVar, reinterpret_cast<void *>(obj), IntlLocaleAddon::Destructor, nullptr,
        &TYPE_TAG, nullptr);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("IntlLocaleAddon::IntlLocaleConstructor: Wrap IntlLocaleAddon failed.");
        delete obj;
        return nullptr;
    }
    if (!obj->InitLocaleContext(env, localeTag, configs)) {
        HILOG_ERROR_I18N("IntlLocaleAddon::IntlLocaleConstructor: Init locale context failed.");
        return nullptr;
    }
    return thisVar;
}

bool IntlLocaleAddon::IsIntlLocale(napi_env env, napi_value argv)
{
    napi_value constructor = JSUtils::GetConstructor(env, "Locale");
    if (constructor == nullptr) {
        HILOG_ERROR_I18N("IntlLocaleAddon::IsIntlLocale: get constructor failed");
        return false;
    }
    bool isIntlLocale = false;
    status = napi_instanceof(env, argv, constructor, &isIntlLocale);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("IntlLocaleAddon::IsIntlLocale: Failed to get instance of argv.");
        return false;
    }
    return isIntlLocale;
}

bool IntlLocaleAddon::InitLocaleContext(napi_env env, const std::string localeTag,
    std::unordered_map<std::string, std::string>& configs)
{
    std::string errMessage;
    intlLocale = std::make_unique<IntlLocale>(localeTag, configs, errMessage);
    if (!errMessage.empty()) {
        ErrorUtil::NapiThrowUndefined(env, errMessage);
        return false;
    }
    return intlLocale != nullptr;
}

std::string IntlLocaleAddon::LocaleToString(napi_env env, napi_value argv)
{
    napi_value funcToString = nullptr;
    napi_status status = napi_get_named_property(env, argv, "toString", &funcToString);
    if (status != napi_ok || funcToString == nullptr) {
        HILOG_ERROR_I18N("IntlLocaleAddon::LocaleToString: Get named property failed.");
        return "";
    }

    napi_value value = nullptr;
    status = napi_call_function(env, argv, funcToString, 0, nullptr, &value);
    if (status != napi_ok || value == nullptr) {
        HILOG_ERROR_I18N("IntlLocaleAddon::LocaleToString: Call toString failed.");
        return "";
    }

    int32_t code = 0;
    std::string localeTag = JSUtils::GetString(env, value, code);
    if (code != 0) {
        HILOG_ERROR_I18N("IntlLocaleAddon::LocaleToString: Get string from value failed.");
        return "";
    }
    return localeTag;
}

napi_value IntlLocaleAddon::GetLanguage(napi_env env, napi_callback_info info)
{
    return GetIntlLocaleAttribute(env, info, GetLanguageInner, napi_valuetype::napi_string);
}

napi_value IntlLocaleAddon::GetBaseName(napi_env env, napi_callback_info info)
{
    return GetIntlLocaleAttribute(env, info, GetBaseNameInner, napi_valuetype::napi_string);
}

napi_value IntlLocaleAddon::GetRegion(napi_env env, napi_callback_info info)
{
    return GetIntlLocaleAttribute(env, info, GetRegionInner, napi_valuetype::napi_string);
}

napi_value IntlLocaleAddon::GetScript(napi_env env, napi_callback_info info)
{
    return GetIntlLocaleAttribute(env, info, GetScriptInner, napi_valuetype::napi_string);
}

napi_value IntlLocaleAddon::GetCalendar(napi_env env, napi_callback_info info)
{
    return GetIntlLocaleAttribute(env, info, GetCalendarInner, napi_valuetype::napi_string);
}

napi_value IntlLocaleAddon::GetCollation(napi_env env, napi_callback_info info)
{
    return GetIntlLocaleAttribute(env, info, GetCollationInner, napi_valuetype::napi_string);
}

napi_value IntlLocaleAddon::GetHourCycle(napi_env env, napi_callback_info info)
{
    return GetIntlLocaleAttribute(env, info, GetHourCycleInner, napi_valuetype::napi_string);
}

napi_value IntlLocaleAddon::GetNumberingSystem(napi_env env, napi_callback_info info)
{
    return GetIntlLocaleAttribute(env, info, GetNumberingSystemInner, napi_valuetype::napi_string);
}

napi_value IntlLocaleAddon::GetNumeric(napi_env env, napi_callback_info info)
{
    return GetIntlLocaleAttribute(env, info, GetNumericInner, napi_valuetype::napi_boolean);
}

napi_value IntlLocaleAddon::GetCaseFirst(napi_env env, napi_callback_info info)
{
    return GetIntlLocaleAttribute(env, info, GetCaseFirstInner, napi_valuetype::napi_string);
}

napi_value IntlLocaleAddon::ToString(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, &data);
    if (status != napi_ok) {
        return nullptr;
    }
    IntlLocaleAddon *obj = nullptr;
    status = napi_unwrap_s(env, thisVar, &TYPE_TAG, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->intlLocale) {
        HILOG_ERROR_I18N("IntlLocaleAddon::ToString: Get IntlLocale object failed.");
        return nullptr;
    }
    std::string errMessage;
    std::string value = obj->intlLocale->ToString(errMessage);
    if (!errMessage.empty()) {
        ErrorUtil::NapiThrowUndefined(env, errMessage);
        return nullptr;
    }

    napi_value result = nullptr;
    status = napi_create_string_utf8(env, value.c_str(), NAPI_AUTO_LENGTH, &result);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("IntlLocaleAddon::ToString: Create string failed.");
        return nullptr;
    }
    return result;
}

napi_value IntlLocaleAddon::Minimize(napi_env env, napi_callback_info info)
{
    napi_value argv = GetIntlLocaleAttribute(env, info, MinimizeInner, napi_valuetype::napi_string);
    return CreateIntlLocaleObject(env, argv);
}

napi_value IntlLocaleAddon::Maximize(napi_env env, napi_callback_info info)
{
    napi_value argv = GetIntlLocaleAttribute(env, info, MaximizeInner, napi_valuetype::napi_string);
    return CreateIntlLocaleObject(env, argv);
}

std::string IntlLocaleAddon::GetLanguageInner(std::shared_ptr<IntlLocale> intlLocale)
{
    if (intlLocale == nullptr) {
        return "";
    }
    return intlLocale->GetLanguage();
}

std::string IntlLocaleAddon::GetBaseNameInner(std::shared_ptr<IntlLocale> intlLocale)
{
    if (intlLocale == nullptr) {
        return "";
    }
    return intlLocale->GetBaseName();
}

std::string IntlLocaleAddon::GetRegionInner(std::shared_ptr<IntlLocale> intlLocale)
{
    if (intlLocale == nullptr) {
        return "";
    }
    return intlLocale->GetRegion();
}

std::string IntlLocaleAddon::GetScriptInner(std::shared_ptr<IntlLocale> intlLocale)
{
    if (intlLocale == nullptr) {
        return "";
    }
    return intlLocale->GetScript();
}

std::string IntlLocaleAddon::GetCalendarInner(std::shared_ptr<IntlLocale> intlLocale)
{
    if (intlLocale == nullptr) {
        return "";
    }
    return intlLocale->GetCalendar();
}

std::string IntlLocaleAddon::GetCollationInner(std::shared_ptr<IntlLocale> intlLocale)
{
    if (intlLocale == nullptr) {
        return "";
    }
    return intlLocale->GetCollation();
}

std::string IntlLocaleAddon::GetHourCycleInner(std::shared_ptr<IntlLocale> intlLocale)
{
    if (intlLocale == nullptr) {
        return "";
    }
    return intlLocale->GetHourCycle();
}

std::string IntlLocaleAddon::GetNumberingSystemInner(std::shared_ptr<IntlLocale> intlLocale)
{
    if (intlLocale == nullptr) {
        return "";
    }
    return intlLocale->GetNumberingSystem();
}

std::string IntlLocaleAddon::GetNumericInner(std::shared_ptr<IntlLocale> intlLocale)
{
    if (intlLocale == nullptr) {
        return "";
    }
    return intlLocale->GetNumeric();
}

std::string IntlLocaleAddon::GetCaseFirstInner(std::shared_ptr<IntlLocale> intlLocale)
{
    if (intlLocale == nullptr) {
        return "";
    }
    return intlLocale->GetCaseFirst();
}

std::string IntlLocaleAddon::MaximizeInner(std::shared_ptr<IntlLocale> intlLocale)
{
    if (intlLocale == nullptr) {
        return "";
    }
    return intlLocale->Maximize();
}

std::string IntlLocaleAddon::MinimizeInner(std::shared_ptr<IntlLocale> intlLocale)
{
    if (intlLocale == nullptr) {
        return "";
    }
    return intlLocale->Minimize();
}

napi_value IntlLocaleAddon::GetIntlLocaleAttribute(napi_env env, napi_callback_info info,
    GetIntlLocaleProperty func, napi_valuetype type)
{
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, &data);
    if (status != napi_ok) {
        return nullptr;
    }
    IntlLocaleAddon *obj = nullptr;
    status = napi_unwrap_s(env, thisVar, &TYPE_TAG, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->intlLocale) {
        HILOG_ERROR_I18N("IntlLocaleAddon::GetIntlLocaleAttribute: Get Locale object failed.");
        return nullptr;
    }
    std::string value = func(obj->intlLocale);
    if (value.empty()) {
        return nullptr;
    }
    napi_value result = nullptr;
    switch (type) {
        case napi_valuetype::napi_boolean: {
            bool boolValue = (value.compare("true") == 0);
            status = napi_get_boolean(env, boolValue, &result);
            if (status != napi_ok || result == nullptr) {
                HILOG_ERROR_I18N("IntlLocaleAddon::GetIntlLocaleAttribute: Create attribute boolean failed.");
                return nullptr;
            }
            break;
        }
        case napi_valuetype::napi_string: {
            status = napi_create_string_utf8(env, value.c_str(), NAPI_AUTO_LENGTH, &result);
            if (status != napi_ok || result == nullptr) {
                HILOG_ERROR_I18N("IntlLocaleAddon::GetIntlLocaleAttribute: Create attribute string failed.");
                return JSUtils::CreateEmptyString(env);
            }
            break;
        }
        default:
            return JSUtils::CreateEmptyString(env);
    }
    return result;
}

napi_value IntlLocaleAddon::CreateIntlLocaleObject(napi_env env, napi_value argv)
{
    if (argv == nullptr) {
        HILOG_ERROR_I18N("IntlLocaleAddon::CreateIntlLocaleObject: argv is nullptr.");
        return nullptr;
    }
    napi_value constructor = JSUtils::GetConstructor(env, "Locale");
    if (constructor == nullptr) {
        HILOG_ERROR_I18N("IntlLocaleAddon::CreateIntlLocaleObject: get constructor failed");
        return nullptr;
    }
    napi_value result = nullptr;
    status = napi_new_instance(env, constructor, 1, &argv, &result);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("IntlLocaleAddon::CreateIntlLocaleObject: Create new IntlLocale instance failed.");
        return nullptr;
    }
    return result;
}

std::string IntlLocaleAddon::ParseLocaleTag(napi_env env, napi_value value, napi_valuetype type)
{
    std::string localeTag;
    if (type == napi_valuetype::napi_object && IntlLocaleAddon::IsIntlLocale(env, value)) {
        localeTag = IntlLocaleAddon::LocaleToString(env, value);
    } else {
        int32_t code = 0;
        localeTag = JSUtils::GetString(env, value, code);
        if (code != 0) {
            HILOG_ERROR_I18N("IntlLocaleAddon::ParseLocaleTag: Get string from value failed.");
            return "";
        }
    }
    return localeTag;
}

void IntlLocaleAddon::ParseConfigs(napi_env env, napi_value options,
    std::unordered_map<std::string, std::string>& configs)
{
    napi_valuetype type = napi_undefined;
    napi_status status = napi_typeof(env, options, &type);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("IntlLocaleAddon::ParseConfigs: Get options type failed.");
        return;
    }
    if (type != napi_object) {
        HILOG_ERROR_I18N("IntlLocaleAddon::ParseConfigs: options is not object.");
        return;
    }
    std::string value;
    if (JSUtils::GetPropertyFormObject(env, options, IntlLocale::languageTag, napi_string, value)) {
        configs.insert({IntlLocale::languageTag, value});
    }
    if (JSUtils::GetPropertyFormObject(env, options, IntlLocale::baseNameTag, napi_string, value)) {
        configs.insert({IntlLocale::baseNameTag, value});
    }
    if (JSUtils::GetPropertyFormObject(env, options, IntlLocale::regionTag, napi_string, value)) {
        configs.insert({IntlLocale::regionTag, value});
    }
    if (JSUtils::GetPropertyFormObject(env, options, IntlLocale::scriptTag, napi_string, value)) {
        configs.insert({IntlLocale::scriptTag, value});
    }
    if (JSUtils::GetPropertyFormObject(env, options, IntlLocale::calendarTag, napi_string, value)) {
        configs.insert({IntlLocale::calendarTag, value});
    }
    if (JSUtils::GetPropertyFormObject(env, options, IntlLocale::collationTag, napi_string, value)) {
        configs.insert({IntlLocale::collationTag, value});
    }
    if (JSUtils::GetPropertyFormObject(env, options, IntlLocale::hourCycleTag, napi_string, value)) {
        configs.insert({IntlLocale::hourCycleTag, value});
    }
    if (JSUtils::GetPropertyFormObject(env, options, IntlLocale::numberingSystemTag, napi_string, value)) {
        configs.insert({IntlLocale::numberingSystemTag, value});
    }
    if (JSUtils::GetPropertyFormObject(env, options, IntlLocale::numericTag, napi_boolean, value)) {
        configs.insert({IntlLocale::numericTag, value});
    }
    if (JSUtils::GetPropertyFormObject(env, options, IntlLocale::caseFirstTag, napi_string, value)) {
        configs.insert({IntlLocale::caseFirstTag, value});
    }
}

std::shared_ptr<IntlLocale> IntlLocaleAddon::GetIntlLocale()
{
    if (!this->intlLocale) {
        return nullptr;
    }
    return this->intlLocale;
}
} // namespace I18n
} // namespace Global
} // namespace OHOS