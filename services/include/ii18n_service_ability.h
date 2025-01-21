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

#ifndef OHOS_GLOBAL_I18N_II18N_SERVICE_ABILITY_H
#define OHOS_GLOBAL_I18N_II18N_SERVICE_ABILITY_H

#include <string>
#include "i18n_types.h"
#include "iremote_broker.h"

namespace OHOS {
namespace Global {
namespace I18n {
class II18nServiceAbility : public IRemoteBroker {
public:
    // The following are the service interfaces provided by i18n service.
    virtual I18nErrorCode SetSystemLanguage(const std::string &language) = 0;
    virtual I18nErrorCode SetSystemRegion(const std::string &region) = 0;
    virtual I18nErrorCode SetSystemLocale(const std::string &locale) = 0;
    virtual I18nErrorCode Set24HourClock(const std::string &flag) = 0;
    virtual I18nErrorCode SetUsingLocalDigit(bool flag) = 0;
    virtual I18nErrorCode AddPreferredLanguage(const std::string &language, int32_t index) = 0;
    virtual I18nErrorCode RemovePreferredLanguage(int32_t index) = 0;

    // I18n service request code.
    enum class ILocaleConfigAbilityCode {
        SET_SYSTEM_LANGUAGE = 0,
        SET_SYSTEM_REGION = 1,
        SET_SYSTEM_LOCALE = 2,
        SET_24_HOUR_CLOCK = 3,
        SET_USING_LOCAL_DIGIT = 4,
        ADD_PREFERRED_LANGUAGE = 5,
        REMOVE_PREFERRED_LANGUAGE = 6,
    };

    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.global.II18nServiceAbility");
};
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif // OHOS_GLOBAL_I18N_II18N_SERVICE_ABILITY_H