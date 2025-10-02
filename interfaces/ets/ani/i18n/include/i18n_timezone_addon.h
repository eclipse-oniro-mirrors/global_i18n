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

#ifndef I18N_TIMEZONE_ADDON_H
#define I18N_TIMEZONE_ADDON_H

#include "ani.h"
#include "i18n_timezone.h"

namespace OHOS {
namespace Global {
namespace I18n {
class I18nTimeZoneAddon {
public:
    static I18nTimeZoneAddon* UnwrapAddon(ani_env *env, ani_object object);
    static ani_object GetTimeZone(ani_env *env, ani_string zoneID);
    static ani_string GetID(ani_env *env, ani_object object);
    static ani_string GetDisplayName(ani_env *env, ani_object object, ani_string locale, ani_object isDST);
    static ani_double GetRawOffset(ani_env *env, ani_object object);
    static ani_double GetOffset(ani_env *env, ani_object object, ani_object date);
    static ani_object GetAvailableIDs(ani_env *env, [[maybe_unused]] ani_object object);
    static ani_object GetAvailableZoneCityIDs(ani_env *env, [[maybe_unused]] ani_object object);
    static ani_object GetCityDisplayName(ani_env *env, [[maybe_unused]] ani_object object,
        ani_string cityID, ani_string locale);
    static ani_object GetTimezoneFromCity(ani_env *env, [[maybe_unused]] ani_object object, ani_string cityID);
    static ani_object GetTimezonesByLocation(ani_env *env, [[maybe_unused]] ani_object object,
        ani_double longitude, ani_double latitude);
    static ani_status BindContextTimeZone(ani_env *env);
private:
    std::unique_ptr<I18nTimeZone> timezone_ = nullptr;
};

} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif
