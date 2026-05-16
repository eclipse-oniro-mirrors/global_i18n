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

#ifndef OHOS_GLOBAL_I18N_STYLED_NUMBER_FORMAT_ADDON_H
#define OHOS_GLOBAL_I18N_STYLED_NUMBER_FORMAT_ADDON_H

#include <map>
#include <string>
#include "ani.h"
#include "number_format.h"
#include "simple_number_format_addon.h"
#include "styled_number_format.h"

namespace OHOS {
namespace Global {
namespace I18n {
class I18nStyledNumberFormatAddon {
public:
    static I18nStyledNumberFormatAddon* UnwrapAddon(ani_env *env, ani_object object);
    static ani_object Create(ani_env *env, [[maybe_unused]] ani_object object, ani_object numberFormat);
    static ani_string GetFormattedNumber(ani_env *env, ani_object object, ani_double value);
    static ani_object GetStyle(ani_env *env, ani_object object, ani_double value);
    static ani_status BindContextStyledNumberFormat(ani_env *env);
    static std::map<std::string, std::string> ParseIntlOptions(ani_env *env, ani_object obj);

private:
    bool InitStyledNumberFormatContent(ani_env *env, ani_object numberFormat);

    static const std::unordered_map<std::string, std::string> PROPERTY_NAME_TO_KEY;
    std::unique_ptr<StyledNumberFormat> styledNumberFormat_ = nullptr;
    std::shared_ptr<NumberFormat> numberFormat_ = nullptr;
};
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif
