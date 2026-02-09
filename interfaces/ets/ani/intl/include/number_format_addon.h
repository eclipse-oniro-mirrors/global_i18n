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

#ifndef OHOS_GLOBAL_I18N_NUMBER_FORMAT_ADDON_H
#define OHOS_GLOBAL_I18N_NUMBER_FORMAT_ADDON_H

#include "ani.h"
#include "number_format.h"

namespace OHOS {
namespace Global {
namespace I18n {
class NumberFormatAddon {
public:
    static NumberFormatAddon* UnwrapAddon(ani_env *env, ani_object object, const std::string &name);
    static ani_string NumberFormat_Format(ani_env *env, ani_object object, ani_double num);
    static ani_string NumberFormat_FormatRange(ani_env *env, ani_object object,
        ani_double startRange, ani_double endRange);
    static ani_object NumberFormat_ResolvedOptions(ani_env *env, ani_object object);
    static ani_object NumberFormat_Create(ani_env *env, [[maybe_unused]] ani_object object);
    static ani_object NumberFormat_CreateWithParam(ani_env *env, [[maybe_unused]] ani_object object,
        ani_object locale, ani_object options);
    static ani_status BindContext_NumberFormat(ani_env *env);
    std::shared_ptr<NumberFormat> CopyNumberFormat()
    {
        return numberfmt_;
    }
private:
    std::shared_ptr<NumberFormat> numberfmt_ = nullptr;
};
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif
