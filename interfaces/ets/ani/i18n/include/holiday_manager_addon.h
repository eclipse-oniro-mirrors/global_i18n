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

#ifndef OHOS_GLOBAL_I18N_HOLIDAY_MANAGER_H
#define OHOS_GLOBAL_I18N_HOLIDAY_MANAGER_H

#include "ani.h"
#include "holiday_manager.h"

namespace OHOS {
namespace Global {
namespace I18n {
class I18nHolidayManagerAddon {
public:
    static I18nHolidayManagerAddon* UnwrapAddon(ani_env *env, ani_object object);
    static ani_object Create(ani_env *env, [[maybe_unused]] ani_object object, ani_string icsPath);
    static ani_boolean IsHoliday(ani_env *env, ani_object object, ani_object date);
    static ani_object GetHolidayInfoItemArray(ani_env *env, ani_object object, ani_object year);
    static ani_status BindContextHolidayManager(ani_env *env);
private:
    static const int32_t MONTH_INCREASE_ONE = 1;
    std::unique_ptr<HolidayManager> holidayManager_ = nullptr;
};
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif
