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

#ifndef OHOS_GLOBAL_I18N_ISO8601_DATE_TIME_FORMAT_ADDON_H
#define OHOS_GLOBAL_I18N_ISO8601_DATE_TIME_FORMAT_ADDON_H

#include "ani.h"
#include "iso8601_date_time_format.h"

namespace OHOS {
namespace Global {
namespace I18n {
class ISO8601DateTimeFormatAddon {
public:
    static ISO8601DateTimeFormatAddon* UnwrapAddon(ani_env *env, ani_object object);
    static ani_object Create(ani_env *env, [[maybe_unused]] ani_object object, ani_object options);
    static ani_string Format(ani_env *env, ani_object object, ani_object date);
    static ani_status BindContextISO8601DateTimeFormat(ani_env *env);

private:
    static std::string ParseTimezone(ani_env *env, ani_object timeZone);
    std::shared_ptr<ISO8601DateTimeFormat> iso8601DateTimeFormat_ = nullptr;
};
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif