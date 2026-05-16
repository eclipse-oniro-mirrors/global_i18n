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

#include "intl_plural_rules_addon.h"
#include "i18n_hilog.h"
#include "js_lifecycle_managers.h"
#include "js_utils.h"

namespace OHOS {
namespace Global {
namespace I18n {
void IntlPluralRulesAddon::Destructor(napi_env env, void* nativeObject, void* finalize_hint)
{
    if (!nativeObject) {
        return;
    }
    delete reinterpret_cast<IntlPluralRulesAddon *>(nativeObject);
    nativeObject = nullptr;
}

napi_value IntlPluralRulesAddon::InitIntlPluralRules(napi_env env, napi_value exports)
{
    HandleScope scope(env, "InitIntlPluralRules");
    if (!scope.IsOpen()) {
        return nullptr;
    }

    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("select", Select),
        DECLARE_NAPI_FUNCTION("resolvedOptions", ResolvedOptions),
        DECLARE_NAPI_FUNCTION("toString", ToString),
        DECLARE_NAPI_STATIC_FUNCTION("supportedLocalesOf", SupportedLocalesOf),
    };

    napi_value constructor;
    napi_status status = napi_define_class(env, "PluralRules", NAPI_AUTO_LENGTH, IntlPluralRulesConstructor,
        nullptr, sizeof(properties) / sizeof(napi_property_descriptor), properties, &constructor);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("InitIntlPluralRules: Define class failed.");
        return nullptr;
    }

    status = napi_set_named_property(env, exports, "PluralRules", constructor);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("InitIntlPluralRules: Set property failed.");
        return nullptr;
    }
    return exports;
}

napi_value IntlPluralRulesAddon::IntlPluralRulesConstructor(napi_env env, napi_callback_info info)
{
    size_t argc = 2;
    napi_value argv[2] = { nullptr };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("IntlPluralRulesConstructor: napi get callback info failed.");
        return nullptr;
    }

    napi_value new_target;
    status = napi_get_new_target(env, info, &new_target);
    if (status == napi_pending_exception || new_target == nullptr) {
        HILOG_ERROR_I18N("IntlPluralRulesConstructor: newTarget is undefined.");
        napi_value exception;
        napi_get_and_clear_last_exception(env, &exception);
        napi_throw_type_error(env, nullptr, "newTarget is undefined");
        return nullptr;
    } else if (status != napi_ok) {
        HILOG_ERROR_I18N("IntlPluralRulesConstructor: get newTarget failed.");
        return nullptr;
    }

    std::vector<std::string> localeTags;
    if (argc > 0) {
        int32_t code = 0;
        localeTags = JSUtils::GetLocaleArray(env, argv[0], code);
    }
    std::unordered_map<std::string, std::string> configs;
    if (argc > 1) {
        configs = ParseConfigs(env, argv[1]);
    }
    IntlPluralRulesAddon* obj = new (std::nothrow) IntlPluralRulesAddon();
    if (obj == nullptr) {
        HILOG_ERROR_I18N("IntlPluralRulesConstructor: Create obj failed.");
        return nullptr;
    }
    status = napi_wrap_s(env, thisVar, reinterpret_cast<void *>(obj), IntlPluralRulesAddon::Destructor,
        nullptr, &TYPE_TAG, nullptr);
    if (status != napi_ok) {
        delete obj;
        HILOG_ERROR_I18N("IntlPluralRulesConstructor: Wrap IntlPluralRulesAddon failed.");
        return nullptr;
    }
    if (!obj->InitIntlPluralRulesContext(env, localeTags, configs)) {
        HILOG_ERROR_I18N("IntlPluralRulesConstructor: Init IntlPluralRules failed.");
        return nullptr;
    }
    return thisVar;
}

bool IntlPluralRulesAddon::InitIntlPluralRulesContext(napi_env env, const std::vector<std::string>& localeTags,
    const std::unordered_map<std::string, std::string>& configs)
{
    ErrorMessage errorMessage;
    intlPluralRules = std::make_unique<IntlPluralRules>(localeTags, configs, errorMessage);
    if (errorMessage.type != ErrorType::NO_ERROR) {
        HILOG_ERROR_I18N("InitIntlPluralRulesContext: IntlPluralRules constructor failed.");
        napi_throw_range_error(env, nullptr, errorMessage.message.c_str());
        return false;
    }
    return intlPluralRules != nullptr;
}

napi_value IntlPluralRulesAddon::Select(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = { nullptr };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("IntlPluralRulesAddon::Select: napi get callback info failed.");
        return nullptr;
    }
    if (argc < 1) {
        HILOG_ERROR_I18N("IntlPluralRulesAddon::Select: missing code param.");
        return nullptr;
    }
    napi_valuetype valueType = napi_valuetype::napi_undefined;
    status = napi_typeof(env, argv[0], &valueType);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("IntlPluralRulesAddon::Select: Get parameter type failed.");
        return nullptr;
    }
    int32_t errorCode = 0;
    double number = JSUtils::GetDoubleFromNapiValue(env, argv[0], valueType, errorCode);
    if (errorCode != 0) {
        HILOG_ERROR_I18N("IntlPluralRulesAddon::Select: Get number value failed.");
        return nullptr;
    }

    IntlPluralRulesAddon *obj = nullptr;
    status = napi_unwrap_s(env, thisVar, &TYPE_TAG, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->intlPluralRules) {
        HILOG_ERROR_I18N("IntlPluralRulesAddon::Select: Get IntlPluralRules object failed.");
        return nullptr;
    }

    ErrorMessage errorMessage;
    std::string formatedValue = obj->intlPluralRules->Select(number, errorMessage);
    if (errorMessage.type != ErrorType::NO_ERROR) {
        HILOG_ERROR_I18N("IntlPluralRulesAddon::Select: IntlPluralRules select failed.");
        napi_throw_range_error(env, nullptr, errorMessage.message.c_str());
        return nullptr;
    }
    return JSUtils::CreateString(env, formatedValue);
}

napi_value IntlPluralRulesAddon::ResolvedOptions(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, &data);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("IntlPluralRulesAddon::ResolvedOptions: get cb info failed.");
        return nullptr;
    }
    IntlPluralRulesAddon *obj = nullptr;
    status = napi_unwrap_s(env, thisVar, &TYPE_TAG, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->intlPluralRules) {
        HILOG_ERROR_I18N("IntlPluralRulesAddon::ResolvedOptions: Get IntlPluralRules object failed.");
        return nullptr;
    }
    IntlPluralRules::ResolvedValue resolvedValue = obj->intlPluralRules->ResolvedOptions();

    napi_value result = nullptr;
    status = napi_create_object(env, &result);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("IntlPluralRulesAddon::ResolvedOptions: create object failed.");
        return nullptr;
    }
    JSUtils::SetNamedStringProperties(env, result, "locale", resolvedValue.locale);
    JSUtils::SetNamedStringProperties(env, result, "type", resolvedValue.type);
    JSUtils::SetNamedIntegerProperties(env, result, "minimumIntegerDigits", resolvedValue.minimumIntegerDigits);
    if (resolvedValue.roundingType == RoundingType::FRACTIONDIGITS) {
        JSUtils::SetNamedIntegerProperties(env, result, "minimumFractionDigits", resolvedValue.minimumDigits);
        JSUtils::SetNamedIntegerProperties(env, result, "maximumFractionDigits", resolvedValue.maximumDigits);
    } else if (resolvedValue.roundingType == RoundingType::SIGNIFICANTDIGITS) {
        JSUtils::SetNamedIntegerProperties(env, result, "minimumSignificantDigits", resolvedValue.minimumDigits);
        JSUtils::SetNamedIntegerProperties(env, result, "maximumSignificantDigits", resolvedValue.maximumDigits);
    }
    JSUtils::SetNamedVectorProperties(env, result, "pluralCategories", resolvedValue.pluralCategories);
    return result;
}

napi_value IntlPluralRulesAddon::ToString(napi_env env, napi_callback_info info)
{
    return JSUtils::CreateString(env, "[object Intl.PluralRules]");
}

napi_value IntlPluralRulesAddon::SupportedLocalesOf(napi_env env, napi_callback_info info)
{
    size_t argc = 2;
    napi_value argv[2] = { nullptr };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("IntlPluralRulesAddon::SupportedLocalesOf: napi get callback info failed.");
        return JSUtils::CreateEmptyArray(env);
    }
    std::vector<std::string> localeTags;
    if (argc > 0) {
        int32_t code = 0;
        localeTags = JSUtils::GetLocaleArray(env, argv[0], code);
    }
    std::unordered_map<std::string, std::string> options = {};
    if (argc > 1) {
        JSUtils::GetOptionValue(env, argv[1], "localeMatcher", options);
    }
    ErrorMessage errorMessage;
    std::vector<std::string> supportedLocales =
        IntlPluralRules::SupportedLocalesOf(localeTags, options, errorMessage);
    if (errorMessage.type != ErrorType::NO_ERROR) {
        HILOG_ERROR_I18N("IntlPluralRulesAddon::SupportedLocalesOf failed.");
        napi_throw_range_error(env, nullptr, errorMessage.message.c_str());
        return nullptr;
    }
    return JSUtils::CreateArray(env, supportedLocales);
}

std::unordered_map<std::string, std::string> IntlPluralRulesAddon::ParseConfigs(napi_env env, napi_value options)
{
    std::unordered_map<std::string, std::string> configs;
    JSUtils::GetOptionValue(env, options, "localeMatcher", configs);
    JSUtils::GetOptionValue(env, options, "type", configs);
    JSUtils::GetDoubleOptionValue(env, options, "minimumIntegerDigits", configs);
    JSUtils::GetDoubleOptionValue(env, options, "minimumFractionDigits", configs);
    JSUtils::GetDoubleOptionValue(env, options, "maximumFractionDigits", configs);
    JSUtils::GetDoubleOptionValue(env, options, "minimumSignificantDigits", configs);
    JSUtils::GetDoubleOptionValue(env, options, "maximumSignificantDigits", configs);
    return configs;
}
} // namespace I18n
} // namespace Global
} // namespace OHOS