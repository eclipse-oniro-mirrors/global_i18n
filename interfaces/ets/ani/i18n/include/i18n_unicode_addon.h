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

#ifndef I18N_UNICODE_ADDON_H
#define I18N_UNICODE_ADDON_H

#include "ani.h"

namespace OHOS {
namespace Global {
namespace I18n {
class I18nUnicodeAddon {
public:
    static ani_boolean IsDigitAddon(ani_env *env, [[maybe_unused]] ani_object object, ani_string ch);
    static ani_boolean IsSpaceCharAddon(ani_env *env, [[maybe_unused]] ani_object object, ani_string ch);
    static ani_boolean IsWhitespaceAddon(ani_env *env, [[maybe_unused]] ani_object object, ani_string ch);
    static ani_boolean IsRTLAddon(ani_env *env, [[maybe_unused]] ani_object object, ani_string ch);
    static ani_boolean IsIdeographAddon(ani_env *env, [[maybe_unused]] ani_object object, ani_string ch);
    static ani_boolean IsLetterAddon(ani_env *env, [[maybe_unused]] ani_object object, ani_string ch);
    static ani_boolean IsLowerCaseAddon(ani_env *env, [[maybe_unused]] ani_object object, ani_string ch);
    static ani_boolean IsUpperCaseAddon(ani_env *env, [[maybe_unused]] ani_object object, ani_string ch);
    static ani_string GetTypeAddon(ani_env *env, [[maybe_unused]] ani_object object, ani_string ch);
    static ani_status BindContextUnicode(ani_env *env);
};
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif
