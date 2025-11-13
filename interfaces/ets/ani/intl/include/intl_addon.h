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

#ifndef INTL_ADDON_H
#define INTL_ADDON_H

#include "ani.h"
#include "collator.h"
#include "date_time_format.h"

namespace OHOS {
namespace Global {
namespace I18n {
class IntlAddon {
public:
    static ani_string DateTimeFormat_Format(ani_env *env, ani_object object, ani_object date);
    static ani_string DateTimeFormat_FormatRange(ani_env *env, ani_object object,
        ani_object startDate, ani_object endDate);
    static ani_object DateTimeFormat_ResolvedOptions(ani_env *env, ani_object object);
    static ani_int Collator_Compare(ani_env *env, ani_object object, ani_string first, ani_string second);
    static ani_object Collator_ResolvedOptions(ani_env *env, ani_object object);

    static ani_object DateTimeFormat_Create(ani_env *env, [[maybe_unused]] ani_object object);
    static ani_object DateTimeFormat_CreateWithParam(ani_env *env, [[maybe_unused]] ani_object object,
        ani_object locale, ani_object options);
    static ani_object Collator_Create(ani_env *env, [[maybe_unused]] ani_object object);
    static ani_object Collator_CreateWithParam(ani_env *env, [[maybe_unused]] ani_object object,
        ani_object locale, ani_object options);

    static ani_status BindContext_DateTimeFormat(ani_env *env);
    static ani_status BindContext_Collator(ani_env *env);
    static ani_status BindContext_PluralRules(ani_env *env);
    static ani_status BindContext_RelativeTimeFormat(ani_env *env);
private:
    std::unique_ptr<Collator> collator_ = nullptr;
    std::unique_ptr<DateTimeFormat> datefmt_ = nullptr;
};
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif
