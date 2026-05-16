/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
#ifndef OHOS_GLOBAL_I18N_CHINESE_CALENDAR_ADDON_H
#define OHOS_GLOBAL_I18N_CHINESE_CALENDAR_ADDON_H

#include <memory>
#include "chinese_calendar.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"

namespace OHOS {
namespace Global {
namespace I18n {
class ChineseCalendarAddon {
public:
    ChineseCalendarAddon();
    ~ChineseCalendarAddon();
    static void Destructor(napi_env env, void *nativeObject, void *hint);
    static napi_value InitChineseCalendar(napi_env env, napi_value exports);
    static napi_value GetChineseCalendar(napi_env env, napi_callback_info info);
    static napi_value CheckLeapMonth(napi_env env, napi_callback_info info);

private:
    static napi_value Constructor(napi_env env, napi_callback_info info);
    bool InitContext(napi_env env, const std::string& localeTag);
    static napi_value SetChineseCalendarTime(napi_env env, napi_callback_info info);
    static ChineseCalendarTime ParseChineseCalendarTime(napi_env env, napi_value value);
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

    static constexpr napi_type_tag TYPE_TAG = { 0x8b76a3b1c9ca49c2, 0xbffcec846bf0c633 };

    std::shared_ptr<ChineseCalendar> chineseCalendar_ = nullptr;
};
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif