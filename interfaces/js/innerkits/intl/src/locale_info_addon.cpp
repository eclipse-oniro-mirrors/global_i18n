/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#include "locale_info_addon.h"

#include <vector>

#include "error_util.h"
#include "i18n_hilog.h"
#include "js_lifecycle_managers.h"
#include "js_utils.h"

namespace OHOS {
namespace Global {
namespace I18n {
static thread_local ThreadReference *g_constructor = nullptr;

LocaleInfoAddon::LocaleInfoAddon() : env_(nullptr) {}

LocaleInfoAddon::~LocaleInfoAddon()
{
}

void LocaleInfoAddon::Destructor(napi_env env, void *nativeObject, void *hint)
{
    if (!nativeObject) {
        return;
    }
    delete reinterpret_cast<LocaleInfoAddon *>(nativeObject);
    nativeObject = nullptr;
}

napi_value LocaleInfoAddon::SetProperty(napi_env env, napi_callback_info info)
{
    // do nothing but provided as an input parameter for DECLARE_NAPI_GETTER_SETTER;
    napi_value result = nullptr;
    NAPI_CALL(env, napi_get_undefined(env, &result));
    return result;
}

napi_value LocaleInfoAddon::InitLocale(napi_env env, napi_value exports)
{
    HandleScope scope(env, "InitLocale");
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
    napi_status status = napi_define_class(env, "Locale", NAPI_AUTO_LENGTH, LocaleConstructor, nullptr,
        sizeof(properties) / sizeof(napi_property_descriptor), properties, &constructor);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("Define class failed when InitLocale");
        return nullptr;
    }
    status = napi_set_named_property(env, exports, "Locale", constructor);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("Set property failed when InitLocale");
        return nullptr;
    }
    g_constructor = ThreadReference::CreateInstance(env, constructor, 1);
    if (!g_constructor) {
        HILOG_ERROR_I18N("LocaleInfoAddon::InitLocale: Failed to create ThreadReference");
        return nullptr;
    }
    return exports;
}

napi_value LocaleInfoAddon::LocaleConstructor(napi_env env, napi_callback_info info)
{
    size_t argc = 2;
    napi_value argv[2] = { nullptr };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        return nullptr;
    }
    std::string localeTag;
    if (argc > 0) {
        int32_t code = 0;
        localeTag = JSUtils::GetString(env, argv[0], code);
        if (code != 0) {
            return nullptr;
        }
    }
    std::map<std::string, std::string> map = {};
    if (argc > 1) {
        JSUtils::GetOptionValue(env, argv[1], "calendar", map);
        JSUtils::GetOptionValue(env, argv[1], "collation", map);
        JSUtils::GetOptionValue(env, argv[1], "hourCycle", map);
        JSUtils::GetOptionValue(env, argv[1], "numberingSystem", map);
        JSUtils::GetBoolOptionValue(env, argv[1], "numeric", map);
        JSUtils::GetOptionValue(env, argv[1], "caseFirst", map);
    }
    std::unique_ptr<LocaleInfoAddon> obj = std::make_unique<LocaleInfoAddon>();
    if (obj == nullptr) {
        return nullptr;
    }
    status =
        napi_wrap(env, thisVar, reinterpret_cast<void *>(obj.get()), LocaleInfoAddon::Destructor, nullptr, nullptr);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("LocaleConstructor: Wrap LocaleInfoAddon failed");
        return nullptr;
    }
    if (!obj->InitLocaleContext(env, info, localeTag, map)) {
        return nullptr;
    }
    obj.release();
    return thisVar;
}

bool LocaleInfoAddon::InitLocaleContext(napi_env env, napi_callback_info info, const std::string localeTag,
    std::map<std::string, std::string> &map)
{
    napi_value global = nullptr;
    napi_status status = napi_get_global(env, &global);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("InitLocaleContext: Get global failed");
        return false;
    }
    env_ = env;
    locale_ = std::make_unique<LocaleInfo>(localeTag, map);
    return locale_ != nullptr;
}

bool LocaleInfoAddon::IsLocaleInfo(napi_env env, napi_value argv)
{
    napi_value constructor = nullptr;
    if (g_constructor == nullptr) {
        HILOG_ERROR_I18N("LocaleInfoAddon::IsLocaleInfo: g_constructor is nullptr.");
        return false;
    }
    if (!g_constructor->GetReferenceValue(env, &constructor)) {
        HILOG_ERROR_I18N("LocaleInfoAddon::IsLocaleInfo: Failed to create reference.");
        return false;
    }

    bool isLocaleInfo = false;
    napi_status status = napi_instanceof(env, argv, constructor, &isLocaleInfo);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("LocaleInfoAddon::IsLocaleInfo: Failed to get instance of argv.");
        return false;
    }
    return isLocaleInfo;
}

std::string LocaleInfoAddon::GetLanguageInner(std::shared_ptr<LocaleInfo> localeInfo)
{
    if (localeInfo != nullptr) {
        return localeInfo->GetLanguage();
    }
    return "";
}

std::string LocaleInfoAddon::GetScriptInner(std::shared_ptr<LocaleInfo> localeInfo)
{
    if (localeInfo != nullptr) {
        return localeInfo->GetScript();
    }
    return "";
}

std::string LocaleInfoAddon::GetRegionInner(std::shared_ptr<LocaleInfo> localeInfo)
{
    if (localeInfo != nullptr) {
        return localeInfo->GetRegion();
    }
    return "";
}

std::string LocaleInfoAddon::GetBaseNameInner(std::shared_ptr<LocaleInfo> localeInfo)
{
    if (localeInfo != nullptr) {
        return localeInfo->GetBaseName();
    }
    return "";
}

std::string LocaleInfoAddon::GetCalendarInner(std::shared_ptr<LocaleInfo> localeInfo)
{
    if (localeInfo != nullptr) {
        return localeInfo->GetCalendar();
    }
    return "";
}

std::string LocaleInfoAddon::GetCollationInner(std::shared_ptr<LocaleInfo> localeInfo)
{
    if (localeInfo != nullptr) {
        return localeInfo->GetCollation();
    }
    return "";
}

std::string LocaleInfoAddon::GetHourCycleInner(std::shared_ptr<LocaleInfo> localeInfo)
{
    if (localeInfo != nullptr) {
        return localeInfo->GetHourCycle();
    }
    return "";
}

std::string LocaleInfoAddon::GetNumberingSystemInner(std::shared_ptr<LocaleInfo> localeInfo)
{
    if (localeInfo != nullptr) {
        return localeInfo->GetNumberingSystem();
    }
    return "";
}

std::string LocaleInfoAddon::GetCaseFirstInner(std::shared_ptr<LocaleInfo> localeInfo)
{
    if (localeInfo != nullptr) {
        return localeInfo->GetCaseFirst();
    }
    return "";
}

std::string LocaleInfoAddon::ToStringInner(std::shared_ptr<LocaleInfo> localeInfo)
{
    if (localeInfo != nullptr) {
        return localeInfo->ToString();
    }
    return "";
}

std::string LocaleInfoAddon::MaximizeInner(std::shared_ptr<LocaleInfo> localeInfo)
{
    if (localeInfo != nullptr) {
        return localeInfo->Maximize();
    }
    return "";
}

std::string LocaleInfoAddon::MinimizeInner(std::shared_ptr<LocaleInfo> localeInfo)
{
    if (localeInfo != nullptr) {
        return localeInfo->Minimize();
    }
    return "";
}

napi_value LocaleInfoAddon::GetLocaleInfoAttribute(napi_env env, napi_callback_info info,
    GetLocaleInfoProperty func)
{
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, &data);
    if (status != napi_ok) {
        return nullptr;
    }
    LocaleInfoAddon *obj = nullptr;
    status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->locale_) {
        HILOG_ERROR_I18N("GetLocaleInfoAttribute: Get Locale object failed");
        return nullptr;
    }
    std::string value = func(obj->locale_);
    napi_value result = nullptr;
    status = napi_create_string_utf8(env, value.c_str(), NAPI_AUTO_LENGTH, &result);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("GetLocaleInfoAttribute: Create attribute string failed");
        return nullptr;
    }
    return result;
}

napi_value LocaleInfoAddon::GetLanguage(napi_env env, napi_callback_info info)
{
    return GetLocaleInfoAttribute(env, info, GetLanguageInner);
}

napi_value LocaleInfoAddon::GetScript(napi_env env, napi_callback_info info)
{
    return GetLocaleInfoAttribute(env, info, GetScriptInner);
}

napi_value LocaleInfoAddon::GetRegion(napi_env env, napi_callback_info info)
{
    return GetLocaleInfoAttribute(env, info, GetRegionInner);
}

napi_value LocaleInfoAddon::GetBaseName(napi_env env, napi_callback_info info)
{
    return GetLocaleInfoAttribute(env, info, GetBaseNameInner);
}

napi_value LocaleInfoAddon::GetCalendar(napi_env env, napi_callback_info info)
{
    return GetLocaleInfoAttribute(env, info, GetCalendarInner);
}

napi_value LocaleInfoAddon::GetCollation(napi_env env, napi_callback_info info)
{
    return GetLocaleInfoAttribute(env, info, GetCollationInner);
}

napi_value LocaleInfoAddon::GetHourCycle(napi_env env, napi_callback_info info)
{
    return GetLocaleInfoAttribute(env, info, GetHourCycleInner);
}

napi_value LocaleInfoAddon::GetNumberingSystem(napi_env env, napi_callback_info info)
{
    return GetLocaleInfoAttribute(env, info, GetNumberingSystemInner);
}

napi_value LocaleInfoAddon::GetNumeric(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, &data);
    if (status != napi_ok) {
        return nullptr;
    }
    LocaleInfoAddon *obj = nullptr;
    status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->locale_) {
        HILOG_ERROR_I18N("GetNumeric: Get Locale object failed");
        return nullptr;
    }
    std::string value = obj->locale_->GetNumeric();
    bool optionBoolValue = (value == "true");
    napi_value result = nullptr;
    status = napi_get_boolean(env, optionBoolValue, &result);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("Create numeric boolean value failed");
        return nullptr;
    }
    return result;
}

napi_value LocaleInfoAddon::GetCaseFirst(napi_env env, napi_callback_info info)
{
    return GetLocaleInfoAttribute(env, info, GetCaseFirstInner);
}

napi_value LocaleInfoAddon::ToString(napi_env env, napi_callback_info info)
{
    return GetLocaleInfoAttribute(env, info, ToStringInner);
}

napi_value LocaleInfoAddon::GetMaximizeOrMinimize(napi_env env, napi_callback_info info,
    GetLocaleInfoProperty func)
{
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, &data);
    if (status != napi_ok) {
        return nullptr;
    }
    LocaleInfoAddon *obj = nullptr;
    status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->locale_) {
        HILOG_ERROR_I18N("Maximize: Get Locale object failed");
        return nullptr;
    }
    std::string localeTag = func(obj->locale_);

    napi_value constructor = nullptr;
    if (g_constructor == nullptr) {
        HILOG_ERROR_I18N("Maximize: g_constructor is nullptr.");
        return nullptr;
    }
    if (!g_constructor->GetReferenceValue(env, &constructor)) {
        HILOG_ERROR_I18N("Maximize: Get locale constructor reference failed");
        return nullptr;
    }
    napi_value result = nullptr;
    napi_value arg = nullptr;
    status = napi_create_string_utf8(env, localeTag.c_str(), NAPI_AUTO_LENGTH, &arg);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("Maximize: Create localeTag string failed");
        return nullptr;
    }
    status = napi_new_instance(env, constructor, 1, &arg, &result);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("Maximize: Create new locale instance failed");
        return nullptr;
    }
    return result;
}

napi_value LocaleInfoAddon::Maximize(napi_env env, napi_callback_info info)
{
    return GetMaximizeOrMinimize(env, info, MaximizeInner);
}

napi_value LocaleInfoAddon::Minimize(napi_env env, napi_callback_info info)
{
    return GetMaximizeOrMinimize(env, info, MinimizeInner);
}

std::shared_ptr<LocaleInfo> LocaleInfoAddon::GetLocaleInfo()
{
    if (!this->locale_) {
        return nullptr;
    }
    return this->locale_;
}
} // namespace I18n
} // namespace Global
} // namespace OHOS