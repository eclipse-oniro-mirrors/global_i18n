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

#ifndef OHOS_GLOBAL_I18N_SIMPLE_NUMBER_FORMAT_ADDON_H
#define OHOS_GLOBAL_I18N_SIMPLE_NUMBER_FORMAT_ADDON_H

#include "ani.h"
#include "simple_number_format.h"

namespace OHOS {
namespace Global {
namespace I18n {
class I18nSimpleNumberFormatAddon {
public:
    static I18nSimpleNumberFormatAddon* UnwrapAddon(ani_env *env, ani_object object);
    static ani_object GetSimpleNumberFormatBySkeleton(ani_env *env, ani_string skeleton, ani_object locale);
    static ani_string Format(ani_env *env, ani_object object, ani_double value);
    static ani_status BindContextSimpleNumberFormat(ani_env *env);

    std::shared_ptr<SimpleNumberFormat> CopySimpleNumberFormat()
    {
        return simpleNumberFormat_;
    }
private:
    std::shared_ptr<SimpleNumberFormat> simpleNumberFormat_ = nullptr;
};
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif
