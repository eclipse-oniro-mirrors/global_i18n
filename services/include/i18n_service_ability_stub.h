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

#ifndef OHOS_GLOBAL_I18N_I18N_SERVICE_ABILITY_STUB_H
#define OHOS_GLOBAL_I18N_I18N_SERVICE_ABILITY_STUB_H

#include <map>
#include "i18n_types.h"
#include "ii18n_service_ability.h"
#include "iremote_stub.h"
#include "message_parcel.h"
#include "refbase.h"

namespace OHOS {
namespace Global {
namespace I18n {
class I18nServiceAbilityStub : public IRemoteStub<II18nServiceAbility> {
public:
    I18nServiceAbilityStub();
    ~I18nServiceAbilityStub();

    /**
     * @brief Process request from i18n service proxy. Call request handling function corresponding request code.
     *
     * @param code Indicates request code.
     * @param data Indicates request data.
     * @param reply Indicates request reply.
     * @param option Indicates request option.
     * @return int32_t Indicates request status.
     */
    int32_t OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

    /**
     * @brief Trigger unload i18n service after one request finished, but service will not unload
     * until there no request in 10s.
     */
    virtual void UnloadI18nServiceAbility() = 0;

private:
    using I18nServiceAbilityInnerFunc = int32_t (I18nServiceAbilityStub::*)(MessageParcel &data, MessageParcel &reply);
    std::map<uint32_t, I18nServiceAbilityInnerFunc> innerFuncMap;

    /**
     * @brief Construct map from request codes to request handling functions.
     */
    void InitInnerFuncMap();

    /**
     * @brief Check whether caller's token type is system app or shell, and has UPDATE_CONFIGURATION permission.
     *
     * @return I18nErrorCode Return SUCCESS indicates that the caller has the permission
     * to request i18n system api service.
     */
    I18nErrorCode CheckPermission();

    // The following are the request handling functions.
    int32_t SetSystemLanguageInner(MessageParcel &data, MessageParcel &reply);
    int32_t SetSystemRegionInner(MessageParcel &data, MessageParcel &reply);
    int32_t SetSystemLocaleInner(MessageParcel &data, MessageParcel &reply);
    int32_t Set24HourClockInner(MessageParcel &data, MessageParcel &reply);
    int32_t SetUsingLocalDigitInner(MessageParcel &data, MessageParcel &reply);
    int32_t AddPreferredLanguageInner(MessageParcel &data, MessageParcel &reply);
    int32_t RemovePreferredLanguageInner(MessageParcel &data, MessageParcel &reply);
};
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif // OHOS_GLOBAL_I18N_I18N_SERVICE_ABILITY_STUB_H