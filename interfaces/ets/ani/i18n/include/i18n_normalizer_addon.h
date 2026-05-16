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

#ifndef OHOS_GLOBAL_I18N_NORMALIZER_H
#define OHOS_GLOBAL_I18N_NORMALIZER_H

#include "ani.h"
#include "i18n_normalizer.h"

namespace OHOS {
namespace Global {
namespace I18n {
class I18nNormalizerAddon {
public:
    static I18nNormalizerAddon* UnwrapAddon(ani_env *env, ani_object object);
    static ani_object GetInstance(ani_env *env, [[maybe_unused]] ani_object object, ani_enum_item mode);
    static ani_string Normalize(ani_env *env, ani_object object, ani_string text);
    static ani_status BindContextNormalizer(ani_env *env);
private:
    static const int32_t NORMALIZER_MODE_NFC = 1;
    static const int32_t NORMALIZER_MODE_NFD = 2;
    static const int32_t NORMALIZER_MODE_NFKC = 3;
    static const int32_t NORMALIZER_MODE_NFKD = 4;

    std::unique_ptr<I18nNormalizer> normalizer_ = nullptr;
};
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif
