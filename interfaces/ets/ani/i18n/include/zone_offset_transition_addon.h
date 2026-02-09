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

#ifndef I18N_ZONE_OFFSET_TRANSITION_ADDON_H
#define I18N_ZONE_OFFSET_TRANSITION_ADDON_H

#include "ani.h"
#include "zone_offset_transition.h"
#include <memory>

namespace OHOS {
namespace Global {
namespace I18n {
class ZoneOffsetTransitionAddon {
public:
    static ZoneOffsetTransitionAddon* UnwrapAddon(ani_env *env, ani_object object);
    static ani_status BindContextZoneOffsetTransition(ani_env *env);
    static ani_object GetZoneOffsetTransObject(ani_env *env, ZoneOffsetTransition* zoneOffsetTrans);
    static ani_double GetMilliseconds(ani_env *env, ani_object object);
    static ani_int GetOffsetAfter(ani_env *env, ani_object object);
    static ani_int GetOffsetBefore(ani_env *env, ani_object object);

private:
    std::unique_ptr<ZoneOffsetTransition> zoneOffsetTransition_ = nullptr;
};
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif