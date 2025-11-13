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

#ifndef OHOS_GLOBAL_I18N_LOCALE_INFO_ADDON_H
#define OHOS_GLOBAL_I18N_LOCALE_INFO_ADDON_H

#include "ani.h"
#include "locale_info.h"

namespace OHOS {
namespace Global {
namespace I18n {
class LocaleInfoAddon {
public:
    static LocaleInfoAddon* UnwrapAddon(ani_env *env, ani_object object, const std::string &name);
    template<typename T>
    static T GetLocaleProperty(ani_env *env, ani_object object,
        const std::string &propertyName, T (LocaleInfo::*getter)() const);
    static ani_string Locale_ToString(ani_env *env, ani_object object);
    static ani_object Locale_Maximize(ani_env *env, ani_object object);
    static ani_object Locale_Minimize(ani_env *env, ani_object object);
    static ani_string Locale_GetLanguage(ani_env *env, ani_object object);
    static ani_string Locale_GetScript(ani_env *env, ani_object object);
    static ani_string Locale_GetRegion(ani_env *env, ani_object object);
    static ani_string Locale_GetBaseName(ani_env *env, ani_object object);
    static ani_string Locale_GetCaseFirst(ani_env *env, ani_object object);
    static ani_string Locale_GetCalendar(ani_env *env, ani_object object);
    static ani_string Locale_GetCollation(ani_env *env, ani_object object);
    static ani_string Locale_GetHourCycle(ani_env *env, ani_object object);
    static ani_string Locale_GetNumberingSystem(ani_env *env, ani_object object);
    static ani_boolean Locale_GetNumeric(ani_env *env, ani_object object);
    static ani_object Locale_Create(ani_env *env, [[maybe_unused]] ani_object object);
    static ani_object Locale_CreateWithParam(ani_env *env, [[maybe_unused]] ani_object object,
        ani_string locale, ani_object options);
    static ani_status BindContext_Locale(ani_env *env);

    std::shared_ptr<LocaleInfo> GetLocaleInfo()
    {
        if (!this->locale_) {
            return nullptr;
        }
        return this->locale_;
    }
private:
    std::shared_ptr<LocaleInfo> locale_ = nullptr;
};
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif
