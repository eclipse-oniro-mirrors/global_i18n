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

#ifndef OHOS_GLOBAL_I18N_ENTITY_RECOGNIZER_ADDON_H
#define OHOS_GLOBAL_I18N_ENTITY_RECOGNIZER_ADDON_H

#include "ani.h"
#include "entity_recognizer.h"

namespace OHOS {
namespace Global {
namespace I18n {
class I18nEntityRecognizerAddon {
public:
    static I18nEntityRecognizerAddon* UnwrapAddon(ani_env *env, ani_object object);
    static ani_object Create(ani_env *env, [[maybe_unused]] ani_object object, ani_string locale);
    static ani_object FindEntityInfo(ani_env *env, ani_object object, ani_string text);
    static ani_status BindContextEntityRecognizer(ani_env *env);
private:
    std::unique_ptr<EntityRecognizer> entityRecognizer_ = nullptr;
};
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif
