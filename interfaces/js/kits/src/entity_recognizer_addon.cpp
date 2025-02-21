/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include <unicode/locid.h>
#include "error_util.h"
#include "i18n_hilog.h"
#include "utils.h"
#include "variable_convertor.h"
#include "entity_recognizer_addon.h"

namespace OHOS {
namespace Global {
namespace I18n {
EntityRecognizerAddon::EntityRecognizerAddon() {}

EntityRecognizerAddon::~EntityRecognizerAddon()
{
}

void EntityRecognizerAddon::Destructor(napi_env env, void *nativeObject, void *hint)
{
    if (!nativeObject) {
        return;
    }
    delete reinterpret_cast<EntityRecognizerAddon *>(nativeObject);
    nativeObject = nullptr;
}

napi_value EntityRecognizerAddon::InitEntityRecognizer(napi_env env, napi_value exports)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("findEntityInfo", FindEntityInfo)
    };
    napi_value entityConstructor = nullptr;
    napi_status status = napi_define_class(env, "EntityRecognizer", NAPI_AUTO_LENGTH, constructor, nullptr,
        sizeof(properties) / sizeof(napi_property_descriptor), properties, &entityConstructor);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("Failed to define class EntityRecognizer at Init.");
        return nullptr;
    }
    status = napi_set_named_property(env, exports, "EntityRecognizer", entityConstructor);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("Set property failed when InitEntityRecognizer");
        return nullptr;
    }
    return exports;
}

napi_value EntityRecognizerAddon::constructor(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = { nullptr };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        return nullptr;
    }
    std::string localeStr;
    if (argc < 1) {
        localeStr = LocaleConfig::GetSystemLocale();
    } else {
        napi_valuetype valueType = napi_valuetype::napi_undefined;
        napi_typeof(env, argv[0], &valueType);
        if (valueType != napi_valuetype::napi_string) {
            ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, "locale", "string", true);
            return nullptr;
        }
        int32_t code = 0;
        localeStr = VariableConvertor::GetString(env, argv[0], code);
        if (code != 0) {
            return nullptr;
        }
    }
    UErrorCode localeStatus = U_ZERO_ERROR;
    icu::Locale locale = icu::Locale::forLanguageTag(localeStr, localeStatus);
    if (!IsValidLocaleTag(locale)) {
        ErrorUtil::NapiThrow(env, I18N_NOT_VALID, "locale", "a valid locale", true);
        return nullptr;
    }
    std::unique_ptr<EntityRecognizerAddon> obj = std::make_unique<EntityRecognizerAddon>();
    status = napi_wrap(env, thisVar, reinterpret_cast<void *>(obj.get()),
        EntityRecognizerAddon::Destructor, nullptr, nullptr);
    if (status != napi_ok) {
        return nullptr;
    }
    obj->entityRecognizer_ = std::make_unique<EntityRecognizer>(locale);
    if (!obj->entityRecognizer_) {
        return nullptr;
    }
    obj.release();
    return thisVar;
}

napi_value EntityRecognizerAddon::FindEntityInfo(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = { 0 };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (status != napi_ok) {
        return nullptr;
    } else if (argc < 1) {
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, "text", "", true);
        return nullptr;
    }
    napi_valuetype valueType = napi_valuetype::napi_undefined;
    napi_typeof(env, argv[0], &valueType);
    if (valueType != napi_valuetype::napi_string) {
        ErrorUtil::NapiThrow(env, I18N_NOT_FOUND, "text", "string", true);
        return nullptr;
    }
    int32_t code = 0;
    std::string message = VariableConvertor::GetString(env, argv[0], code);
    if (code != 0) {
        return nullptr;
    }
    EntityRecognizerAddon *obj = nullptr;
    status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || obj == nullptr || obj->entityRecognizer_ == nullptr) {
        HILOG_ERROR_I18N("Get EntityRecognizer object failed");
        return nullptr;
    }
    std::vector<std::vector<int>> entityInfo = obj->entityRecognizer_->FindEntityInfo(message);
    napi_value result = GetEntityInfoItem(env, entityInfo);
    return result;
}

napi_value EntityRecognizerAddon::GetEntityInfoItem(napi_env env, std::vector<std::vector<int>>& entityInfo)
{
    napi_value result = nullptr;
    napi_status status = napi_create_array_with_length(env, entityInfo[0][0] + entityInfo[1][0], &result);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("create EntityInfo array failed.");
        return nullptr;
    }
    std::vector<std::string> types = {"phone_number", "date"};
    int index = 0;
    for (std::string::size_type t = 0; t < types.size(); t++) {
        for (int i = 0; i < entityInfo[t][0]; i++) {
            int begin = entityInfo[t][2 * i + 1];
            int end = entityInfo[t][2 * i + 2];
            std::string type = types[t];
            napi_value item = CreateEntityInfoItem(env, begin, end, type);
            status = napi_set_element(env, result, index, item);
            index++;
            if (status != napi_ok) {
                HILOG_ERROR_I18N("Failed to set item element.");
                return nullptr;
            }
        }
    }
    return result;
}

napi_value EntityRecognizerAddon::CreateEntityInfoItem(napi_env env, const int begin,
    const int end, const std::string& type)
{
    napi_value result;
    napi_status status = napi_create_object(env, &result);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("Create EntityInfoItem object failed.");
        return nullptr;
    }
    status = napi_set_named_property(env, result, "begin",
        VariableConvertor::CreateNumber(env, begin));
    if (status != napi_ok) {
        HILOG_ERROR_I18N("Failed to set element begin.");
        return nullptr;
    }
    status = napi_set_named_property(env, result, "end",
        VariableConvertor::CreateNumber(env, end));
    if (status != napi_ok) {
        HILOG_ERROR_I18N("Failed to set element end.");
        return nullptr;
    }
    status = napi_set_named_property(env, result, "type",
        VariableConvertor::CreateString(env, type));
    if (status != napi_ok) {
        HILOG_ERROR_I18N("Failed to set element type.");
        return nullptr;
    }
    return result;
}

} // namespace I18n
} // namespace Global
} // namespace OHOS