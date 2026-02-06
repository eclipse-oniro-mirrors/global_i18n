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

#include "i18n_unicode_addon.h"

#include "character.h"
#include "i18n_hilog.h"
#include "variable_converter.h"

using namespace OHOS;
using namespace Global;
using namespace I18n;

ani_boolean I18nUnicodeAddon::IsDigitAddon(ani_env *env, [[maybe_unused]] ani_object object, ani_string ch)
{
    return IsDigit(VariableConverter::AniStrToString(env, ch));
}

ani_boolean I18nUnicodeAddon::IsSpaceCharAddon(ani_env *env, [[maybe_unused]] ani_object object, ani_string ch)
{
    return IsSpaceChar(VariableConverter::AniStrToString(env, ch));
}

ani_boolean I18nUnicodeAddon::IsWhitespaceAddon(ani_env *env, [[maybe_unused]] ani_object object, ani_string ch)
{
    return IsWhiteSpace(VariableConverter::AniStrToString(env, ch));
}

ani_boolean I18nUnicodeAddon::IsRTLAddon(ani_env *env, [[maybe_unused]] ani_object object, ani_string ch)
{
    return IsRTLCharacter(VariableConverter::AniStrToString(env, ch));
}

ani_boolean I18nUnicodeAddon::IsIdeographAddon(ani_env *env, [[maybe_unused]] ani_object object, ani_string ch)
{
    return IsIdeoGraphic(VariableConverter::AniStrToString(env, ch));
}

ani_boolean I18nUnicodeAddon::IsLetterAddon(ani_env *env, [[maybe_unused]] ani_object object, ani_string ch)
{
    return IsLetter(VariableConverter::AniStrToString(env, ch));
}

ani_boolean I18nUnicodeAddon::IsLowerCaseAddon(ani_env *env, [[maybe_unused]] ani_object object, ani_string ch)
{
    return IsLowerCase(VariableConverter::AniStrToString(env, ch));
}

ani_boolean I18nUnicodeAddon::IsUpperCaseAddon(ani_env *env, [[maybe_unused]] ani_object object, ani_string ch)
{
    return IsUpperCase(VariableConverter::AniStrToString(env, ch));
}

ani_string I18nUnicodeAddon::GetTypeAddon(ani_env *env, [[maybe_unused]] ani_object object, ani_string ch)
{
    std::string type = GetType(VariableConverter::AniStrToString(env, ch));
    return VariableConverter::StringToAniStr(env, type);
}

ani_status I18nUnicodeAddon::BindContextUnicode(ani_env *env)
{
    static const char *className = "L@ohos/i18n/i18n/Unicode;";
    ani_class cls;
    if (ANI_OK != env->FindClass(className, &cls)) {
        HILOG_ERROR_I18N("Find class '%{public}s' failed", className);
        return ANI_ERROR;
    }

    std::array methods = {
        ani_native_function { "isDigit", nullptr, reinterpret_cast<void *>(IsDigitAddon) },
        ani_native_function { "isSpaceChar", nullptr, reinterpret_cast<void *>(IsSpaceCharAddon) },
        ani_native_function { "isWhitespace", nullptr, reinterpret_cast<void *>(IsWhitespaceAddon) },
        ani_native_function { "isRTL", nullptr, reinterpret_cast<void *>(IsRTLAddon) },
        ani_native_function { "isIdeograph", nullptr, reinterpret_cast<void *>(IsIdeographAddon) },
        ani_native_function { "isLetter", nullptr, reinterpret_cast<void *>(IsLetterAddon) },
        ani_native_function { "isLowerCase", nullptr, reinterpret_cast<void *>(IsLowerCaseAddon) },
        ani_native_function { "isUpperCase", nullptr, reinterpret_cast<void *>(IsUpperCaseAddon) },
        ani_native_function { "getType", nullptr, reinterpret_cast<void *>(GetTypeAddon) },
    };

    if (ANI_OK != env->Class_BindNativeMethods(cls, methods.data(), methods.size())) {
        HILOG_ERROR_I18N("Cannot bind native methods to '%{public}s'", className);
        return ANI_ERROR;
    };

    return ANI_OK;
}
