/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef INTERFACES_CJ_INCLUDE_I18N_ENTITY_IMPL_H_
#define INTERFACES_CJ_INCLUDE_I18N_ENTITY_IMPL_H_

#include <string>
#include <vector>
#include "i18n_struct.h"
#include "entity_recognizer.h"
#include "ffi_remote_data.h"

namespace OHOS {
namespace Global {
namespace I18n {
class CEntity : public OHOS::FFI::FFIData {
    DECL_TYPE(CEntity, FFIData)
public:
    explicit CEntity(std::string locale);
    ~CEntity() = default;
    CEntityInfoItemArr FindEntityInfo(std::string text);
    bool InitSuccess();
private:
    std::unique_ptr<EntityRecognizer> entityRecognizer_ = nullptr;
};

}  // namespace I18n
}  // namespace Global
}  // namespace OHOS

#endif  // INTERFACES_CJ_INCLUDE_I18N_ENTITY_IMPL_H_
