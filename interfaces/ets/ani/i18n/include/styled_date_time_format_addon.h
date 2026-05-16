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

#ifndef OHOS_GLOBAL_I18N_STYLED_DATE_TIME_FORMAT_ADDON_H
#define OHOS_GLOBAL_I18N_STYLED_DATE_TIME_FORMAT_ADDON_H

#include <map>
#include <string>
#include "ani.h"
#include "styled_date_time_format.h"

namespace OHOS {
namespace Global {
namespace I18n {
class I18nStyledDateTimeFormatAddon {
public:
    static ani_object FormatToPats(ani_env *env, ani_object object, ani_object date);
    static ani_status BindContextStyledDateTimeFormat(ani_env *env);

private:

    static I18nStyledDateTimeFormatAddon* UnwrapAddon(ani_env *env, ani_object object);
    bool InitStyledFormatContext(ani_env *env, ani_object dateTimeFormat);
    static ani_object Create(ani_env *env, [[maybe_unused]] ani_object object, ani_object dateTimeFormat);

    std::unique_ptr<StyledDateTimeFormat> styledDateTimeFormat_ = nullptr;
};
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif
