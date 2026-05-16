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

#include <map>
#include <string>
#include <vector>
#include "ani.h"
#include "i18n_types.h"
#include "number_format.h"

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
    static bool GetEnumIndexMember(ani_env *env, ani_object options, const std::string &name, int32_t &value);
    static bool GetObjectMember(ani_env *env, ani_object options, const std::string &name, ani_object &value);
    static std::string GetLocaleTagFromBuiltinLocale(ani_env *env, ani_object obj);
    static ani_object CreateArray(ani_env *env, const std::vector<std::string> &strs);
    static bool ParseStringArray(ani_env *env, ani_object obj, std::vector<std::string> &strs);
    static ani_enum_item GetEnumItemByIndex(ani_env* env, const char* enumClassName, const int index);
    static ani_long GetDateValue(ani_env *env, ani_object date, const std::string &method);
    static ani_int GetDateInt(ani_env *env, ani_object date, const std::string &method);
    static std::vector<std::string> GetLocaleTags(ani_env *env, ani_object locale);
    static ani_object CreateAniMap(ani_env* env, const std::unordered_map<std::string, std::string> &map);
    static void SetObjectBooleanMember(ani_env *env, ani_object obj, const std::string &name, ani_boolean value);
    static void SetObjectNumberMember(ani_env *env, ani_object obj, const std::string &name, ani_int value);
    static std::map<std::string, std::string> ParseIntlOptions(ani_env *env, ani_object numberFormat);
    static std::shared_ptr<NumberFormat> UnWrapIntlDateTimeFormat(ani_env *env, ani_object numberFormat);
    static bool IsIntlNumberFormat(ani_env *env, ani_object obj);
    static NumberFormatType GetNumberFormatType(ani_env *env, ani_object value);
    static bool IsIntlDateTimeFormat(ani_env *env, ani_object dateTimeFormat);
    static DateTimeFormatType GetDateTimeFormatType(ani_env *env, ani_object value);
    template<typename T>
    static ani_object CreateAniObject(ani_env* env, const char* className, T* ptr);
};
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif
