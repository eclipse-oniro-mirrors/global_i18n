/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include <singleton.h>
#include "i18n_hilog.h"
#include "i18n_service_ability_load_manager.h"
#include "iremote_object.h"
#include "i18n_service_ability_load_callback.h"

namespace OHOS {
namespace Global {
namespace I18n {
void I18nServiceAbilityLoadCallback::OnLoadSystemAbilitySuccess(int32_t systemAbilityId,
    const sptr<IRemoteObject> &remoteObject)
{
    auto loadMgr = DelayedSingleton<I18nServiceAbilityLoadManager>::GetInstance();
    if (loadMgr == nullptr) {
        HILOG_ERROR_I18N("I18nServiceAbilityLoadCallback::OnLoadSystemAbilitySuccess get load manager failed.");
        return;
    }
    loadMgr->LoadSystemAbilitySuccess();
}

void I18nServiceAbilityLoadCallback::OnLoadSystemAbilityFail(int32_t systemAbilityId)
{
    auto loadMgr = DelayedSingleton<I18nServiceAbilityLoadManager>::GetInstance();
    if (loadMgr == nullptr) {
        HILOG_ERROR_I18N("I18nServiceAbilityLoadCallback::OnLoadSystemAbilityFail get load manager failed.");
        return;
    }
    loadMgr->LoadSystemAbilityFail();
}
} // namespace I18n
} // namespace Global
} // namespace OHOS