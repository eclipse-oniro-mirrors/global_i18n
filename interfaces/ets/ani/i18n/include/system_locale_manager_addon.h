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

#ifndef OHOS_GLOBAL_I18N_SYSTEM_LOCALE_MANAGER_H
#define OHOS_GLOBAL_I18N_SYSTEM_LOCALE_MANAGER_H

#include "ani.h"
#include "system_locale_manager.h"

namespace OHOS {
namespace Global {
namespace I18n {
class I18nSysLocaleMgrAddon {
public:
    static I18nSysLocaleMgrAddon* UnwrapAddon(ani_env *env, ani_object object);
    static ani_object Create(ani_env *env, [[maybe_unused]] ani_object object);
    static ani_object GetLanguageInfoArray(ani_env *env, ani_object object, ani_object languages, ani_object options);
    static ani_object GetRegionInfoArray(ani_env *env, ani_object object, ani_object regions, ani_object options);
    static ani_object GetTimeZoneCityItemArray(ani_env *env, [[maybe_unused]] ani_object object);
    static ani_status BindContextSysLocaleMgr(ani_env *env);
private:
    std::unique_ptr<SystemLocaleManager> systemLocaleManager_ = nullptr;
};
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif
