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

#include "i18n_struct.h"
#include "ffi_remote_data.h"
#include "i18n_entity_ffi.h"
#include "i18n_entity_impl.h"
#include "i18n_hilog.h"

namespace OHOS {
namespace Global {
namespace I18n {
using namespace OHOS::FFI;
extern "C" {
int64_t FfiOHOSEntityImplConstructor(char* locale, int32_t* errcode)
{
    std::string localeStr(locale);
    auto instance = FFIData::Create<CEntity>(localeStr);
    if (!instance->InitSuccess()) {
        *errcode = I18N_NOT_VALID;
        HILOG_ERROR_I18N("Create CEntity fail");
        return -1;
    }
    return instance->GetID();
}

CEntityInfoItemArr FfiOHOSEntityFindEntityInfo(int64_t id, char* text)
{
    auto instance = FFIData::GetData<CEntity>(id);
    if (!instance) {
        HILOG_ERROR_I18N("The CEntity instance is nullptr");
        return { 0, nullptr };
    }
    return instance->FindEntityInfo(std::string(text));
}
}
}  // namespace I18n
}  // namespace Global
}  // namespace OHOS
