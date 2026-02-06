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

#include "number_format_addon.h"

#include "i18n_hilog.h"
#include "utils.h"
#include "variable_converter.h"

using namespace OHOS;
using namespace Global;
using namespace I18n;

NumberFormatAddon* NumberFormatAddon::UnwrapAddon(ani_env *env, ani_object object, const std::string &name)
{
    ani_long ptr;
    if (ANI_OK != env->Object_GetFieldByName_Long(object, name.c_str(), &ptr)) {
        HILOG_ERROR_I18N("Get Long '%{public}s' failed", name.c_str());
        return nullptr;
    }
    return reinterpret_cast<NumberFormatAddon*>(ptr);
}

ani_string NumberFormatAddon::NumberFormat_Format(ani_env *env, ani_object object, ani_double number)
{
    NumberFormatAddon *obj = UnwrapAddon(env, object, "nativeNumberFormat");
    if (obj == nullptr || obj->numberfmt_ == nullptr) {
        HILOG_ERROR_I18N("FormatNumber: Get NumberFormat object failed");
        return nullptr;
    }

    std::string str = obj->numberfmt_->Format(number);
    return VariableConverter::StringToAniStr(env, str);
}

ani_string NumberFormatAddon::NumberFormat_FormatRange(ani_env *env, ani_object object,
    ani_double startRange, ani_double endRange)
{
    NumberFormatAddon *obj = UnwrapAddon(env, object, "nativeNumberFormat");
    if (obj == nullptr || obj->numberfmt_ == nullptr) {
        HILOG_ERROR_I18N("FormatRangeNumber: Get NumberFormat object failed");
        return nullptr;
    }

    std::string str = obj->numberfmt_->FormatRange(startRange, endRange);
    return VariableConverter::StringToAniStr(env, str);
}

ani_object NumberFormatAddon::NumberFormat_ResolvedOptions(ani_env *env, ani_object object)
{
    NumberFormatAddon *obj = UnwrapAddon(env, object, "nativeNumberFormat");
    if (obj == nullptr || obj->numberfmt_ == nullptr) {
        HILOG_ERROR_I18N("GetNumberResolvedOptions: Get NumberFormat object failed");
        return nullptr;
    }
    std::map<std::string, std::string> options = {};
    obj->numberfmt_->GetResolvedOptions(options);

    static const char *className = "L@ohos/intl/intl/NumberOptionsInner;";
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

    ani_object numberOptions;
    if (ANI_OK != env->Object_New(cls, ctor, &numberOptions)) {
        HILOG_ERROR_I18N("New object '%{public}s' failed", className);
        return nullptr;
    }

    std::vector<std::string> keys = {
        "locale", "currency", "currencySign", "currencyDisplay", "unit", "unitDisplay", "unitUsage",
        "signDisplay", "compactDisplay", "notation", "localeMatcher", "style", "numberingSystem",
        "roundingPriority", "roundingMode", "minimumIntegerDigits", "minimumFractionDigits", "maximumFractionDigits",
        "minimumSignificantDigits", "maximumSignificantDigits", "roundingIncrement"
    };

    for (const auto& key : keys) {
        if (options.find(key) != options.end()) {
            if (key == "minimumIntegerDigits" || key == "minimumFractionDigits" ||
                key == "maximumFractionDigits" || key == "minimumSignificantDigits" ||
                key == "maximumSignificantDigits" || key == "roundingIncrement") {
                VariableConverter::SetNumberMember(env, numberOptions, key, options[key]);
            } else {
                VariableConverter::SetStringMember(env, numberOptions, key, options[key]);
            }
        }
    }
    VariableConverter::SetBooleanMember(env, numberOptions, "useGrouping", options["useGrouping"]);
    return numberOptions;
}

ani_object NumberFormatAddon::NumberFormat_Create(ani_env *env, [[maybe_unused]] ani_object object)
{
    std::vector<std::string> localeTags;
    std::map<std::string, std::string> map = {};
    std::unique_ptr<NumberFormatAddon> obj = std::make_unique<NumberFormatAddon>();
    obj->numberfmt_ = std::make_unique<NumberFormat>(localeTags, map);

    static const char *className = "L@ohos/intl/intl/NumberFormat;";
    ani_object numberFormatObject = VariableConverter::CreateAniObject(env, className, obj.get());
    obj.release();
    return numberFormatObject;
}

static std::map<std::string, std::string> GetNumberFormatOptions(ani_env *env, ani_object options)
{
    std::map<std::string, std::string> map;
    ani_boolean isUndefined;
    if (ANI_OK != env->Reference_IsUndefined(options, &isUndefined)) {
        HILOG_ERROR_I18N("Reference IsUndefined failed");
        return map;
    }
    if (!isUndefined) {
        std::vector<std::string> stringKeys = {
            "locale", "currency", "currencySign", "currencyDisplay", "unit", "unitDisplay", "unitUsage",
            "signDisplay", "compactDisplay", "notation", "localeMatcher", "style", "numberingSystem"
        };
        for (const auto& key : stringKeys) {
            std::string value;
            if (VariableConverter::GetStringMember(env, options, key, value)) {
                map.insert(std::make_pair(key, value));
            }
        }

        std::vector<std::pair<std::string, int>> intKeys = {
            {"minimumIntegerDigits", 0}, {"minimumFractionDigits", 0}, {"maximumFractionDigits", 0},
            {"minimumSignificantDigits", 0}, {"maximumSignificantDigits", 0}, {"roundingIncrement", 0}
        };
        for (auto& [key, value] : intKeys) {
            if (VariableConverter::GetNumberMember(env, options, key, value)) {
                map.insert(std::make_pair(key, std::to_string(value)));
            }
        }

        bool useGrouping;
        if (VariableConverter::GetBooleanMember(env, options, "useGrouping", useGrouping)) {
            map.insert(std::make_pair("useGrouping", useGrouping ? "true" : "false"));
        }
    }
    return map;
}

ani_object NumberFormatAddon::NumberFormat_CreateWithParam(ani_env *env, [[maybe_unused]] ani_object object,
    ani_object locale, ani_object options)
{
    std::vector<std::string> localeTags = VariableConverter::GetLocaleTags(env, locale);
    std::map<std::string, std::string> map = GetNumberFormatOptions(env, options);

    std::unique_ptr<NumberFormatAddon> obj = std::make_unique<NumberFormatAddon>();
    obj->numberfmt_ = std::make_unique<NumberFormat>(localeTags, map);

    static const char *className = "L@ohos/intl/intl/NumberFormat;";
    ani_object numberFormatObject = VariableConverter::CreateAniObject(env, className, obj.get());
    obj.release();
    return numberFormatObject;
}

ani_status NumberFormatAddon::BindContext_NumberFormat(ani_env *env)
{
    static const char *className = "L@ohos/intl/intl/NumberFormat;";
    ani_class cls;
    if (ANI_OK != env->FindClass(className, &cls)) {
        HILOG_ERROR_I18N("Find class '%{public}s' failed", className);
        return ANI_ERROR;
    }

    std::array methods = {
        ani_native_function {"create", ":L@ohos/intl/intl/NumberFormat;",
            reinterpret_cast<void *>(NumberFormat_Create) },
        ani_native_function {"create",
            "Lstd/core/Object;L@ohos/intl/intl/NumberOptions;:L@ohos/intl/intl/NumberFormat;",
            reinterpret_cast<void *>(NumberFormat_CreateWithParam) },
        ani_native_function {"format", nullptr, reinterpret_cast<void *>(NumberFormat_Format) },
        ani_native_function {"formatRange", nullptr, reinterpret_cast<void *>(NumberFormat_FormatRange) },
        ani_native_function {"resolvedOptions", nullptr, reinterpret_cast<void *>(NumberFormat_ResolvedOptions) },
    };

    if (ANI_OK != env->Class_BindNativeMethods(cls, methods.data(), methods.size())) {
        HILOG_ERROR_I18N("Cannot bind native methods to '%{public}s'", className);
        return ANI_ERROR;
    };
    return ANI_OK;
}
