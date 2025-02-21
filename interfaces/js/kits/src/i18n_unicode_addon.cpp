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
#include "character.h"
#include "error_util.h"
#include "hilog/log.h"
#include "i18n_unicode_addon.h"
#include "js_utils.h"
#include "variable_convertor.h"

namespace OHOS {
namespace Global {
namespace I18n {
static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, 0xD001E00, "I18nUnicodeAddonJs" };
using namespace OHOS::HiviewDFX;

I18nUnicodeAddon::I18nUnicodeAddon() {}

I18nUnicodeAddon::~I18nUnicodeAddon() {}

void I18nUnicodeAddon::Destructor(napi_env env, void *nativeObject, void *hint)
{
    if (!nativeObject) {
        return;
    }
    delete reinterpret_cast<I18nUnicodeAddon *>(nativeObject);
    nativeObject = nullptr;
}

napi_value I18nUnicodeAddon::InitI18nUnicode(napi_env env, napi_value exports)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_STATIC_FUNCTION("isDigit", IsDigitAddon),
        DECLARE_NAPI_STATIC_FUNCTION("isSpaceChar", IsSpaceCharAddon),
        DECLARE_NAPI_STATIC_FUNCTION("isWhitespace", IsWhiteSpaceAddon),
        DECLARE_NAPI_STATIC_FUNCTION("isRTL", IsRTLCharacterAddon),
        DECLARE_NAPI_STATIC_FUNCTION("isIdeograph", IsIdeoGraphicAddon),
        DECLARE_NAPI_STATIC_FUNCTION("isLetter", IsLetterAddon),
        DECLARE_NAPI_STATIC_FUNCTION("isLowerCase", IsLowerCaseAddon),
        DECLARE_NAPI_STATIC_FUNCTION("isUpperCase", IsUpperCaseAddon),
        DECLARE_NAPI_STATIC_FUNCTION("getType", GetTypeAddon),
    };
    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, "Unicode", NAPI_AUTO_LENGTH, JSUtils::DefaultConstructor, nullptr,
        sizeof(properties) / sizeof(napi_property_descriptor), properties, &constructor);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "InitI18nUnicode: Define class failed when InitUnicode.");
        return nullptr;
    }

    status = napi_set_named_property(env, exports, "Unicode", constructor);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "InitI18nUnicode: Set property failed when InitUnicode.");
        return nullptr;
    }
    return exports;
}

napi_value I18nUnicodeAddon::InitCharacter(napi_env env, napi_value exports)
{
    napi_status status = napi_ok;
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("isDigit", IsDigitAddon),
        DECLARE_NAPI_FUNCTION("isSpaceChar", IsSpaceCharAddon),
        DECLARE_NAPI_FUNCTION("isWhitespace", IsWhiteSpaceAddon),
        DECLARE_NAPI_FUNCTION("isRTL", IsRTLCharacterAddon),
        DECLARE_NAPI_FUNCTION("isIdeograph", IsIdeoGraphicAddon),
        DECLARE_NAPI_FUNCTION("isLetter", IsLetterAddon),
        DECLARE_NAPI_FUNCTION("isLowerCase", IsLowerCaseAddon),
        DECLARE_NAPI_FUNCTION("isUpperCase", IsUpperCaseAddon),
        DECLARE_NAPI_FUNCTION("getType", GetTypeAddon)
    };

    napi_value constructor = nullptr;
    status = napi_define_class(env, "Character", NAPI_AUTO_LENGTH, ObjectConstructor, nullptr,
        sizeof(properties) / sizeof(napi_property_descriptor), properties, &constructor);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Define class failed when InitCharacter");
        return nullptr;
    }

    status = napi_set_named_property(env, exports, "Character", constructor);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Set property failed when InitCharacter");
        return nullptr;
    }
    return exports;
}

napi_value I18nUnicodeAddon::IsDigitAddon(napi_env env, napi_callback_info info)
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
        napi_throw_type_error(env, nullptr, "Parameter type does not match");
        return nullptr;
    }
    int32_t code = 0;
    std::string character = VariableConvertor::GetString(env, argv[0], code);
    if (code) {
        return nullptr;
    }
    bool isDigit = IsDigit(character);
    napi_value result = nullptr;
    status = napi_get_boolean(env, isDigit, &result);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Create isDigit boolean value failed");
        return nullptr;
    }
    return result;
}

napi_value I18nUnicodeAddon::IsSpaceCharAddon(napi_env env, napi_callback_info info)
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
        napi_throw_type_error(env, nullptr, "Parameter type does not match");
        return nullptr;
    }
    int32_t code = 0;
    std::string character = VariableConvertor::GetString(env, argv[0], code);
    if (code) {
        return nullptr;
    }
    bool isSpaceChar = IsSpaceChar(character);
    napi_value result = nullptr;
    status = napi_get_boolean(env, isSpaceChar, &result);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Create isSpaceChar boolean value failed");
        return nullptr;
    }
    return result;
}

napi_value I18nUnicodeAddon::IsWhiteSpaceAddon(napi_env env, napi_callback_info info)
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
        napi_throw_type_error(env, nullptr, "Parameter type does not match");
        return nullptr;
    }
    int32_t code = 0;
    std::string character = VariableConvertor::GetString(env, argv[0], code);
    if (code) {
        return nullptr;
    }
    bool isWhiteSpace = IsWhiteSpace(character);
    napi_value result = nullptr;
    status = napi_get_boolean(env, isWhiteSpace, &result);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Create isWhiteSpace boolean value failed");
        return nullptr;
    }
    return result;
}

napi_value I18nUnicodeAddon::IsRTLCharacterAddon(napi_env env, napi_callback_info info)
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
        napi_throw_type_error(env, nullptr, "Parameter type does not match");
        return nullptr;
    }
    int32_t code = 0;
    std::string character = VariableConvertor::GetString(env, argv[0], code);
    if (code) {
        return nullptr;
    }
    bool isRTLCharacter = IsRTLCharacter(character);
    napi_value result = nullptr;
    status = napi_get_boolean(env, isRTLCharacter, &result);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Create isRTLCharacter boolean value failed");
        return nullptr;
    }
    return result;
}

napi_value I18nUnicodeAddon::IsIdeoGraphicAddon(napi_env env, napi_callback_info info)
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
        napi_throw_type_error(env, nullptr, "Parameter type does not match");
        return nullptr;
    }
    int32_t code = 0;
    std::string character = VariableConvertor::GetString(env, argv[0], code);
    if (code) {
        return nullptr;
    }
    bool isIdeoGraphic = IsIdeoGraphic(character);
    napi_value result = nullptr;
    status = napi_get_boolean(env, isIdeoGraphic, &result);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Create isIdeoGraphic boolean value failed");
        return nullptr;
    }
    return result;
}

napi_value I18nUnicodeAddon::IsLetterAddon(napi_env env, napi_callback_info info)
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
        napi_throw_type_error(env, nullptr, "Parameter type does not match");
        return nullptr;
    }
    int32_t code = 0;
    std::string character = VariableConvertor::GetString(env, argv[0], code);
    if (code) {
        return nullptr;
    }
    bool isLetter = IsLetter(character);
    napi_value result = nullptr;
    status = napi_get_boolean(env, isLetter, &result);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Create isLetter boolean value failed");
        return nullptr;
    }
    return result;
}

napi_value I18nUnicodeAddon::IsLowerCaseAddon(napi_env env, napi_callback_info info)
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
        napi_throw_type_error(env, nullptr, "Parameter type does not match");
        return nullptr;
    }
    int32_t code = 0;
    std::string character = VariableConvertor::GetString(env, argv[0], code);
    if (code) {
        return nullptr;
    }
    bool isLowerCase = IsLowerCase(character);
    napi_value result = nullptr;
    status = napi_get_boolean(env, isLowerCase, &result);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Create isLowerCase boolean value failed");
        return nullptr;
    }
    return result;
}

napi_value I18nUnicodeAddon::IsUpperCaseAddon(napi_env env, napi_callback_info info)
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
        napi_throw_type_error(env, nullptr, "Parameter type does not match");
        return nullptr;
    }
    int32_t code = 0;
    std::string character = VariableConvertor::GetString(env, argv[0], code);
    if (code) {
        return nullptr;
    }
    bool isUpperCase = IsUpperCase(character);
    napi_value result = nullptr;
    status = napi_get_boolean(env, isUpperCase, &result);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Create isUpperCase boolean value failed");
        return nullptr;
    }
    return result;
}

napi_value I18nUnicodeAddon::GetTypeAddon(napi_env env, napi_callback_info info)
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
        napi_throw_type_error(env, nullptr, "Parameter type does not match");
        return nullptr;
    }
    int32_t code = 0;
    std::string character = VariableConvertor::GetString(env, argv[0], code);
    if (code) {
        return nullptr;
    }
    std::string type = GetType(character);
    napi_value result = nullptr;
    status = napi_create_string_utf8(env, type.c_str(), NAPI_AUTO_LENGTH, &result);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Create getType string value failed");
        return nullptr;
    }
    return result;
}

napi_value I18nUnicodeAddon::ObjectConstructor(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, &data);
    if (status != napi_ok) {
        return nullptr;
    }
    std::unique_ptr<I18nUnicodeAddon> obj = nullptr;
    obj = std::make_unique<I18nUnicodeAddon>();
    status =
        napi_wrap(env, thisVar, reinterpret_cast<void *>(obj.get()), I18nUnicodeAddon::Destructor, nullptr, nullptr);
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Wrap I18nAddon failed");
        return nullptr;
    }
    obj.release();
    return thisVar;
}
} // namespace I18n
} // namespace Global
} // namespace OHOS