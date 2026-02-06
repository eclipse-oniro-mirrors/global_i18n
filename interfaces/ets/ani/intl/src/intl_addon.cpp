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

#include "intl_addon.h"

#include "i18n_hilog.h"
#include "number_format_addon.h"
#include "variable_converter.h"

using namespace OHOS;
using namespace Global;
using namespace I18n;

static IntlAddon* UnwrapAddon(ani_env *env, ani_object object, const std::string &name)
{
    ani_long ptr;
    if (ANI_OK != env->Object_GetFieldByName_Long(object, name.c_str(), &ptr)) {
        HILOG_ERROR_I18N("Get Long '%{public}s' failed", name.c_str());
        return nullptr;
    }
    return reinterpret_cast<IntlAddon*>(ptr);
}

ani_string IntlAddon::DateTimeFormat_Format(ani_env *env, ani_object object, ani_object date)
{
    static const char *className = "Lescompat/Date;";
    ani_class cls;
    if (ANI_OK != env->FindClass(className, &cls)) {
        HILOG_ERROR_I18N("Find class '%{public}s' failed", className);
        return nullptr;
    }

    ani_method getms;
    if (ANI_OK != env->Class_FindMethod(cls, "valueOf", nullptr, &getms)) {
        HILOG_ERROR_I18N("Find method 'valueOf' failed");
        return nullptr;
    }

    ani_double milliseconds;
    if (ANI_OK != env->Object_CallMethod_Double(date, getms, &milliseconds)) {
        HILOG_ERROR_I18N("Call method 'valueOf' failed");
        return nullptr;
    }

    IntlAddon *obj = UnwrapAddon(env, object, "nativeDateTimeFormat");
    if (obj == nullptr || obj->datefmt_ == nullptr) {
        HILOG_ERROR_I18N("FormatDateTime: Get DateTimeFormat object failed");
        return nullptr;
    }

    std::string str = obj->datefmt_->Format(milliseconds);
    return VariableConverter::StringToAniStr(env, str);
}

ani_string IntlAddon::DateTimeFormat_FormatRange(ani_env *env, ani_object object,
    ani_object startDate, ani_object endDate)
{
    static const char *className = "Lescompat/Date;";
    ani_class cls;
    if (ANI_OK != env->FindClass(className, &cls)) {
        HILOG_ERROR_I18N("Find class '%{public}s' failed", className);
        return nullptr;
    }

    ani_method getms;
    if (ANI_OK != env->Class_FindMethod(cls, "valueOf", nullptr, &getms)) {
        HILOG_ERROR_I18N("Find method 'valueOf' failed");
        return nullptr;
    }

    ani_double firstMilliseconds;
    if (ANI_OK != env->Object_CallMethod_Double(startDate, getms, &firstMilliseconds)) {
        HILOG_ERROR_I18N("Call method 'valueOf' failed");
        return nullptr;
    }

    ani_double secondMilliseconds;
    if (ANI_OK != env->Object_CallMethod_Double(endDate, getms, &secondMilliseconds)) {
        HILOG_ERROR_I18N("Call method 'valueOf' failed");
        return nullptr;
    }

    IntlAddon *obj = UnwrapAddon(env, object, "nativeDateTimeFormat");
    if (obj == nullptr || obj->datefmt_ == nullptr) {
        HILOG_ERROR_I18N("FormatDateTimeRange: Get DateTimeFormat object failed");
        return nullptr;
    }

    std::string str = obj->datefmt_->FormatRange(firstMilliseconds, secondMilliseconds);
    return VariableConverter::StringToAniStr(env, str);
}

ani_object IntlAddon::DateTimeFormat_ResolvedOptions(ani_env *env, ani_object object)
{
    IntlAddon *obj = UnwrapAddon(env, object, "nativeDateTimeFormat");
    if (obj == nullptr || obj->datefmt_ == nullptr) {
        HILOG_ERROR_I18N("GetDateTimeResolvedOptions: Get DateTimeFormat object failed");
        return nullptr;
    }
    std::map<std::string, std::string> options = {};
    obj->datefmt_->GetResolvedOptions(options);

    static const char *className = "L@ohos/intl/intl/DateTimeOptionsInner;";
    ani_class cls;
    if (ANI_OK != env->FindClass(className, &cls)) {
        HILOG_ERROR_I18N("Find class '%{public}s' failed", className);
        return nullptr;
    }

    ani_method ctor;
    if (ANI_OK != env->Class_FindMethod(cls, "<ctor>", nullptr, &ctor)) {
        HILOG_ERROR_I18N("Find method '<ctor>' failed");
        return nullptr;
    }

    ani_object dateTimeOptions;
    if (ANI_OK != env->Object_New(cls, ctor, &dateTimeOptions)) {
        HILOG_ERROR_I18N("New object '%{public}s' failed", className);
        return nullptr;
    }

    std::vector<std::string> keys = {
        "locale", "dateStyle", "timeStyle", "hourCycle", "timeZone", "numberingSystem",
        "weekday", "era", "year", "month", "day", "hour", "minute", "second",
        "timeZoneName", "dayPeriod", "localeMatcher", "formatMatcher"
    };

    for (const auto& key : keys) {
        if (options.find(key) != options.end()) {
            VariableConverter::SetStringMember(env, dateTimeOptions, key, options[key]);
        }
    }
    VariableConverter::SetBooleanMember(env, dateTimeOptions, "hour12", options["hour12"]);
    return dateTimeOptions;
}


ani_double IntlAddon::Collator_Compare(ani_env *env, ani_object object, ani_string first, ani_string second)
{
    IntlAddon *obj = UnwrapAddon(env, object, "nativeCollator");
    if (obj == nullptr || obj->collator_ == nullptr) {
        HILOG_ERROR_I18N("CompareString: Get Collator object failed");
        return -1;
    }

    CompareResult compareResult = obj->collator_->Compare(
        VariableConverter::AniStrToString(env, first), VariableConverter::AniStrToString(env, second));
    return static_cast<double>(compareResult);
}

ani_object IntlAddon::Collator_ResolvedOptions(ani_env *env, ani_object object)
{
    IntlAddon *obj = UnwrapAddon(env, object, "nativeCollator");
    if (obj == nullptr || obj->collator_ == nullptr) {
        HILOG_ERROR_I18N("GetCollatorResolvedOptions: Get Collator object failed");
        return nullptr;
    }
    std::map<std::string, std::string> options = {};
    obj->collator_->ResolvedOptions(options);

    static const char *className = "L@ohos/intl/intl/CollatorOptionsInner;";
    ani_class cls;
    if (ANI_OK != env->FindClass(className, &cls)) {
        HILOG_ERROR_I18N("Find class '%{public}s' failed", className);
        return nullptr;
    }

    ani_method ctor;
    if (ANI_OK != env->Class_FindMethod(cls, "<ctor>", nullptr, &ctor)) {
        HILOG_ERROR_I18N("Find method '<ctor>' failed");
        return nullptr;
    }

    ani_object collatorOptions;
    if (ANI_OK != env->Object_New(cls, ctor, &collatorOptions)) {
        HILOG_ERROR_I18N("New object '%{public}s' failed", className);
        return nullptr;
    }

    std::vector<std::string> keys = {"localeMatcher", "usage", "sensitivity", "collation", "caseFirst"};
    for (const auto& key : keys) {
        if (options.find(key) != options.end()) {
            VariableConverter::SetStringMember(env, collatorOptions, key, options[key]);
        }
    }
    VariableConverter::SetBooleanMember(env, collatorOptions, "ignorePunctuation", options["ignorePunctuation"]);
    VariableConverter::SetBooleanMember(env, collatorOptions, "numeric", options["numeric"]);
    return collatorOptions;
}

ani_object IntlAddon::DateTimeFormat_Create(ani_env *env, [[maybe_unused]] ani_object object)
{
    std::unique_ptr<IntlAddon> obj = std::make_unique<IntlAddon>();
    std::vector<std::string> localeTags;
    std::map<std::string, std::string> map = {};

    obj->datefmt_ = DateTimeFormat::CreateInstance(localeTags, map);
    static const char *className = "L@ohos/intl/intl/DateTimeFormat;";
    ani_object dateTimeFormatObject = VariableConverter::CreateAniObject(env, className, obj.get());
    obj.release();
    return dateTimeFormatObject;
}

static std::map<std::string, std::string> GetDateTimeFormatOptions(ani_env *env, ani_object options)
{
    std::map<std::string, std::string> map;
    ani_boolean isUndefined;
    if (ANI_OK != env->Reference_IsUndefined(options, &isUndefined)) {
        HILOG_ERROR_I18N("Reference IsUndefined failed");
        return map;
    }
    if (!isUndefined) {
        std::vector<std::string> stringKeys = {
            "locale", "dateStyle", "timeStyle", "hourCycle", "timeZone", "numberingSystem",
            "weekday", "era", "year", "month", "day", "hour", "minute", "second",
            "timeZoneName", "dayPeriod", "localeMatcher", "formatMatcher"
        };
        for (const auto& key : stringKeys) {
            std::string value;
            if (VariableConverter::GetStringMember(env, options, key, value)) {
                map.insert(std::make_pair(key, value));
            }
        }

        bool hour12;
        if (VariableConverter::GetBooleanMember(env, options, "hour12", hour12)) {
            map.insert(std::make_pair("hour12", hour12 ? "true" : "false"));
        }
    }
    return map;
}

ani_object IntlAddon::DateTimeFormat_CreateWithParam(ani_env *env, [[maybe_unused]] ani_object object,
    ani_object locale, ani_object options)
{
    std::vector<std::string> localeTags = VariableConverter::GetLocaleTags(env, locale);
    std::map<std::string, std::string> map = GetDateTimeFormatOptions(env, options);

    std::unique_ptr<IntlAddon> obj = std::make_unique<IntlAddon>();
    obj->datefmt_ = DateTimeFormat::CreateInstance(localeTags, map);

    static const char *className = "L@ohos/intl/intl/DateTimeFormat;";
    ani_object dateTimeFormatObject = VariableConverter::CreateAniObject(env, className, obj.get());
    obj.release();
    return dateTimeFormatObject;
}

ani_object IntlAddon::Collator_Create(ani_env *env, [[maybe_unused]] ani_object object)
{
    std::vector<std::string> localeTags;
    std::map<std::string, std::string> map = {};
    std::unique_ptr<IntlAddon> obj = std::make_unique<IntlAddon>();
    obj->collator_ = std::make_unique<Collator>(localeTags, map);

    static const char *className = "L@ohos/intl/intl/Collator;";
    ani_object collatorObject = VariableConverter::CreateAniObject(env, className, obj.get());
    obj.release();
    return collatorObject;
}

static std::map<std::string, std::string> GetCollatorOptions(ani_env *env, ani_object options)
{
    std::map<std::string, std::string> map;
    ani_boolean isUndefined;
    if (ANI_OK != env->Reference_IsUndefined(options, &isUndefined)) {
        HILOG_ERROR_I18N("Reference IsUndefined failed");
        return map;
    }
    if (!isUndefined) {
        std::vector<std::string> stringKeys = {"localeMatcher", "usage", "sensitivity", "collation", "caseFirst"};
        for (const auto& key : stringKeys) {
            std::string value;
            if (VariableConverter::GetStringMember(env, options, key, value)) {
                map.insert(std::make_pair(key, value));
            }
        }

        bool ignorePunctuation;
        if (VariableConverter::GetBooleanMember(env, options, "ignorePunctuation", ignorePunctuation)) {
            map.insert(std::make_pair("ignorePunctuation", ignorePunctuation ? "true" : "false"));
        }

        bool numeric;
        if (VariableConverter::GetBooleanMember(env, options, "numeric", numeric)) {
            map.insert(std::make_pair("numeric", numeric ? "true" : "false"));
        }
    }
    return map;
}

ani_object IntlAddon::Collator_CreateWithParam(ani_env *env, [[maybe_unused]] ani_object object,
    ani_object locale, ani_object options)
{
    std::vector<std::string> localeTags = VariableConverter::GetLocaleTags(env, locale);
    std::map<std::string, std::string> map = GetCollatorOptions(env, options);

    std::unique_ptr<IntlAddon> obj = std::make_unique<IntlAddon>();
    obj->collator_ = std::make_unique<Collator>(localeTags, map);

    static const char *className = "L@ohos/intl/intl/Collator;";
    ani_object collatorObject = VariableConverter::CreateAniObject(env, className, obj.get());
    obj.release();
    return collatorObject;
}

ani_status IntlAddon::BindContext_DateTimeFormat(ani_env *env)
{
    static const char *className = "L@ohos/intl/intl/DateTimeFormat;";
    ani_class cls;
    if (ANI_OK != env->FindClass(className, &cls)) {
        HILOG_ERROR_I18N("Find class '%{public}s' failed", className);
        return ANI_ERROR;
    }

    std::array methods = {
        ani_native_function {"create", ":L@ohos/intl/intl/DateTimeFormat;",
            reinterpret_cast<void *>(DateTimeFormat_Create) },
        ani_native_function {"create",
            "Lstd/core/Object;L@ohos/intl/intl/DateTimeOptions;:L@ohos/intl/intl/DateTimeFormat;",
            reinterpret_cast<void *>(DateTimeFormat_CreateWithParam) },
        ani_native_function {"format", nullptr, reinterpret_cast<void *>(DateTimeFormat_Format) },
        ani_native_function {"formatRange", nullptr, reinterpret_cast<void *>(DateTimeFormat_FormatRange) },
        ani_native_function {"resolvedOptions", nullptr, reinterpret_cast<void *>(DateTimeFormat_ResolvedOptions) },
    };

    if (ANI_OK != env->Class_BindNativeMethods(cls, methods.data(), methods.size())) {
        HILOG_ERROR_I18N("Cannot bind native methods to '%{public}s'", className);
        return ANI_ERROR;
    };
    return ANI_OK;
}

ani_status IntlAddon::BindContext_Collator(ani_env *env)
{
    static const char *className = "L@ohos/intl/intl/Collator;";
    ani_class cls;
    if (ANI_OK != env->FindClass(className, &cls)) {
        HILOG_ERROR_I18N("Find class '%{public}s' failed", className);
        return ANI_ERROR;
    }

    std::array methods = {
        ani_native_function {"create", ":L@ohos/intl/intl/Collator;", reinterpret_cast<void *>(Collator_Create) },
        ani_native_function {"create",
            "Lstd/core/Object;L@ohos/intl/intl/CollatorOptions;:L@ohos/intl/intl/Collator;",
            reinterpret_cast<void *>(Collator_CreateWithParam) },
        ani_native_function {"compare", nullptr, reinterpret_cast<void *>(Collator_Compare) },
        ani_native_function {"resolvedOptions", nullptr, reinterpret_cast<void *>(Collator_ResolvedOptions) },
    };

    if (ANI_OK != env->Class_BindNativeMethods(cls, methods.data(), methods.size())) {
        HILOG_ERROR_I18N("Cannot bind native methods to '%{public}s'", className);
        return ANI_ERROR;
    };
    return ANI_OK;
}

ANI_EXPORT ani_status ANI_Constructor(ani_vm *vm, uint32_t *result)
{
    ani_env *env;
    if (ANI_OK != vm->GetEnv(ANI_VERSION_1, &env)) {
        HILOG_ERROR_I18N("Unsupported ANI_VERSION_1");
        return ANI_ERROR;
    }

    auto status = IntlAddon::BindContext_DateTimeFormat(env);
    if (status != ANI_OK) {
        return status;
    }

    status = NumberFormatAddon::BindContext_NumberFormat(env);
    if (status != ANI_OK) {
        return status;
    }

    status = IntlAddon::BindContext_Collator(env);
    if (status != ANI_OK) {
        return status;
    }

    *result = ANI_VERSION_1;
    return ANI_OK;
}
