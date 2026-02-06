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

#ifndef OHOS_GLOBAL_I18N_CALENDAR_ADDON_H
#define OHOS_GLOBAL_I18N_CALENDAR_ADDON_H

#include "ani.h"
#include "i18n_calendar.h"

namespace OHOS {
namespace Global {
namespace I18n {
class I18nCalendarAddon {
public:
    static I18nCalendarAddon* UnwrapAddon(ani_env *env, ani_object object);
    static ani_object GetCalendar(ani_env *env, ani_string locale, ani_string type);
    static void SetTimeByDate(ani_env *env, ani_object object, ani_object date);
    static void SetTime(ani_env *env, ani_object object, ani_double time);
    static void Set(ani_env *env, ani_object object, ani_double year, ani_double month,
        ani_double date, ani_object hour, ani_object minute, ani_object second);
    static void SetTimeZone(ani_env *env, ani_object object, ani_string timezone);
    static ani_string GetTimeZone(ani_env *env, ani_object object);
    static ani_double GetFirstDayOfWeek(ani_env *env, ani_object object);
    static void SetFirstDayOfWeek(ani_env *env, ani_object object, ani_double value);
    static ani_double GetMinimalDaysInFirstWeek(ani_env *env, ani_object object);
    static void SetMinimalDaysInFirstWeek(ani_env *env, ani_object object, ani_double value);
    static ani_double Get(ani_env *env, ani_object object, ani_string field);
    static ani_string GetDisplayName(ani_env *env, ani_object object, ani_string locale);
    static ani_boolean IsWeekend(ani_env *env, ani_object object, ani_object date);
    static void Add(ani_env *env, ani_object object, ani_string field, ani_double amount);
    static ani_double GetTimeInMillis(ani_env *env, ani_object object);
    static ani_double CompareDays(ani_env *env, ani_object object, ani_object date);
    static ani_status BindContextCalendar(ani_env *env);
private:
    std::unique_ptr<I18nCalendar> calendar_ = nullptr;
};
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif
