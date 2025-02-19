/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include <memory>
#include "i18n_calendar.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"

namespace OHOS {
namespace Global {
namespace I18n {
class I18nCalendarAddon {
public:
    I18nCalendarAddon();
    ~I18nCalendarAddon();
    static void Destructor(napi_env env, void *nativeObject, void *hint);
    static napi_value InitI18nCalendar(napi_env env, napi_value exports);
    static napi_value GetCalendar(napi_env env, napi_callback_info info);

private:
    static napi_value InitCalendar(napi_env env, napi_value exports);
    static napi_value I18nCalendarConstructor(napi_env env, napi_callback_info info);
    // process js function call
    static napi_value SetTime(napi_env env, napi_callback_info info);
    static napi_value Set(napi_env env, napi_callback_info info);
    static napi_value GetTimeZone(napi_env env, napi_callback_info info);
    static napi_value SetTimeZone(napi_env env, napi_callback_info info);
    static napi_value GetFirstDayOfWeek(napi_env env, napi_callback_info info);
    static napi_value SetFirstDayOfWeek(napi_env env, napi_callback_info info);
    static napi_value GetMinimalDaysInFirstWeek(napi_env env, napi_callback_info info);
    static napi_value SetMinimalDaysInFirstWeek(napi_env env, napi_callback_info info);
    static napi_value Get(napi_env env, napi_callback_info info);
    static napi_value Add(napi_env env, napi_callback_info info);
    static napi_value GetDisplayName(napi_env env, napi_callback_info info);
    static napi_value GetTimeInMillis(napi_env env, napi_callback_info info);
    static napi_value IsWeekend(napi_env env, napi_callback_info info);
    static napi_value CompareDays(napi_env env, napi_callback_info info);

    // auxiliary functions
    static CalendarType GetCalendarType(napi_env env, napi_value value);
    static std::string GetAddField(napi_env &env, napi_value &value, int32_t &code);
    static napi_value GetDate(napi_env env, napi_value value);
    bool InitCalendarContext(napi_env env, napi_callback_info info, const std::string &localeTag, CalendarType type);
    void SetField(napi_env env, napi_value value, UCalendarDateFields field);
    void SetMilliseconds(napi_env env, napi_value value);

    std::unique_ptr<I18nCalendar> calendar_ = nullptr;
};
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif