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

#ifndef I18N_ZONE_RULES_ADDON_H
#define I18N_ZONE_RULES_ADDON_H

#include "ani.h"
#include "zone_rules.h"
#include <memory>

namespace OHOS {
namespace Global {
namespace I18n {
class ZoneRulesAddon {
public:
    static ZoneRulesAddon* UnwrapAddon(ani_env *env, ani_object object);
    static ani_status BindContextZoneRules(ani_env *env);
    static ani_object GetZoneRulesObject(ani_env *env, ani_object object, std::string tzId);
    static ani_object NextTransition(ani_env *env, ani_object object, ani_object date);

private:
    std::unique_ptr<ZoneRules> zoneRules_ = nullptr;
};
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif