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

#ifndef OHOS_GLOBAL_I18N_VARIABLE_CONVERTER_H
#define OHOS_GLOBAL_I18N_VARIABLE_CONVERTER_H

#include <string>
#include <vector>
#include "ani.h"

namespace OHOS {
namespace Global {
namespace I18n {
class VariableConverter {
public:
    static std::string AniStrToString(ani_env *env, ani_ref aniStr);
    static ani_string StringToAniStr(ani_env *env, const std::string &str);
    static void SetBooleanMember(ani_env *env, ani_object obj, const std::string &name, const std::string &value);
    static bool GetBooleanMember(ani_env *env, ani_object options, const std::string &name, bool& value);
    static void SetNumberMember(ani_env *env, ani_object obj, const std::string &name, const ani_int value);
    static void SetNumberMember(ani_env *env, ani_object obj, const std::string &name, const std::string &value);
    static bool GetNumberMember(ani_env *env, ani_object options, const std::string &name, int& value);
    static void SetStringMember(ani_env *env, ani_object obj, const std::string &name, const std::string &value);
    static bool GetStringMember(ani_env *env, ani_object options, const std::string &name, std::string &value);
    static ani_object CreateArray(ani_env *env, const std::vector<std::string> &strs);
    static bool ParseStringArray(ani_env *env, ani_object obj, std::vector<std::string> &strs);
    static ani_enum_item GetEnumItemByIndex(ani_env* env, const char* enumClassName, const int index);
    static ani_double GetDateValue(ani_env *env, ani_object date, const std::string &method);
    static std::vector<std::string> GetLocaleTags(ani_env *env, ani_object locale);
    template<typename T>
    static ani_object CreateAniObject(ani_env* env, const char* className, T* ptr);
};
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif
