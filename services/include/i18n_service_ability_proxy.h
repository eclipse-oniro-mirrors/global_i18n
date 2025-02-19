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

#ifndef OHOS_GLOBAL_I18N_I18N_SERVICE_ABILITY_PROXY_H
#define OHOS_GLOBAL_I18N_I18N_SERVICE_ABILITY_PROXY_H

#include "ii18n_service_ability.h"
#include "iremote_broker.h"
#include "iremote_proxy.h"

namespace OHOS {
namespace Global {
namespace I18n {
/**
 * @brief I18n Service Proxy class which responsible for initiating service requests.
 */
class I18nServiceAbilityProxy : public IRemoteProxy<II18nServiceAbility> {
public:
    explicit I18nServiceAbilityProxy(const sptr<IRemoteObject> &impl);
    ~I18nServiceAbilityProxy() = default;

    /**
     * @brief Initiating set system language request to i18n service.
     *
     * @param language language tag to set.
     * @return I18nErrorCode Return SUCCESS indicates that the request was successful.
     */
    I18nErrorCode SetSystemLanguage(const std::string &language);

    /**
     * @brief Initiating set system region request to i18n service.
     *
     * @param region region tag to set.
     * @return I18nErrorCode Return SUCCESS indicates that the request was successful.
     */
    I18nErrorCode SetSystemRegion(const std::string &region);

    /**
     * @brief Initiating set system locale request to i18n service.
     *
     * @param locale locale tag to set.
     * @return I18nErrorCode Return SUCCESS indicates that the request was successful.
     */
    I18nErrorCode SetSystemLocale(const std::string &locale);

    /**
     * @brief Initiating set system 12 or 24-hour clock request to i18n service.
     *
     * @param flag When flag is true, the system adopts 24-hour clock;
     * When flag is false, the system adopts 12-hour clock; When flag is empty string, the system adopts
     * system locale default behavior.
     * @return I18nErrorCode Return SUCCESS indicates that the request was successful.
     */
    I18nErrorCode Set24HourClock(const std::string &flag);

    /**
     * @brief Initiating set system digit request to i18n service.
     *
     * @param flag When flag is True, request to adopt local digit;
     * When flag is False, request to adopt arbic digit;
     * @return I18nErrorCode Return SUCCESS indicates that the request was successful.
     */
    I18nErrorCode SetUsingLocalDigit(bool flag);

    /**
     * @brief Initiating add system preferred language request to i18n service.
     *
     * @param language preferred language tag to set.
     * @param index Indicats added preferred language index.
     * @return I18nErrorCode Return SUCCESS indicates that the request was successful.
     */
    I18nErrorCode AddPreferredLanguage(const std::string &language, int32_t index);

    /**
     * @brief Initiating remove system preferred language request to i18n service.
     *
     * @param index Indicats removed preferred language index.
     * @return I18nErrorCode Return SUCCESS indicates that the request was successful.
     */
    I18nErrorCode RemovePreferredLanguage(int32_t index);

private:
    static I18nErrorCode ProcessReply(int32_t reply);
    static inline BrokerDelegator<I18nServiceAbilityProxy> delegator_;
};
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif // OHOS_GLOBAL_I18N_I18N_SERVICE_ABILITY_PROXY_H