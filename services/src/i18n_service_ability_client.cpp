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

#include "i18n_hilog.h"
#include "i18n_service_ability_load_manager.h"
#include "iremote_object.h"
#include "system_ability_definition.h"
#include "i18n_service_ability_client.h"

namespace OHOS {
namespace Global {
namespace I18n {
sptr<II18nServiceAbility> I18nServiceAbilityClient::GetProxy(I18nErrorCode &err)
{
    sptr<IRemoteObject> proxy = I18nServiceAbilityLoadManager::GetInstance()->GetI18nServiceAbility(I18N_SA_ID);
    if (proxy == nullptr) {
        HILOG_ERROR_I18N("I18nServiceAbilityClient::GetProxy load sa failed, try again.");
        proxy = I18nServiceAbilityLoadManager::GetInstance()->GetI18nServiceAbility(I18N_SA_ID);
    }
    if (proxy == nullptr) {
        HILOG_ERROR_I18N("I18nServiceAbilityClient::GetProxy load sa failed.");
        err = I18nErrorCode::LOAD_SA_FAILED;
        return nullptr;
    }
    return iface_cast<II18nServiceAbility>(proxy);
}

I18nErrorCode I18nServiceAbilityClient::SetSystemLanguage(const std::string &language)
{
    I18nErrorCode err = I18nErrorCode::SUCCESS;
    sptr<II18nServiceAbility> i18nServiceAbilityObj = GetProxy(err);
    if (i18nServiceAbilityObj == nullptr) {
        return err;
    }
    return i18nServiceAbilityObj->SetSystemLanguage(language);
}

I18nErrorCode I18nServiceAbilityClient::SetSystemRegion(const std::string &region)
{
    I18nErrorCode err = I18nErrorCode::SUCCESS;
    sptr<II18nServiceAbility> i18nServiceAbilityObj = GetProxy(err);
    if (i18nServiceAbilityObj == nullptr) {
        return err;
    }
    return i18nServiceAbilityObj->SetSystemRegion(region);
}

I18nErrorCode I18nServiceAbilityClient::SetSystemLocale(const std::string &locale)
{
    I18nErrorCode err = I18nErrorCode::SUCCESS;
    sptr<II18nServiceAbility> i18nServiceAbilityObj = GetProxy(err);
    if (i18nServiceAbilityObj == nullptr) {
        return err;
    }
    return i18nServiceAbilityObj->SetSystemLocale(locale);
}

I18nErrorCode I18nServiceAbilityClient::Set24HourClock(const std::string &flag)
{
    I18nErrorCode err = I18nErrorCode::SUCCESS;
    sptr<II18nServiceAbility> i18nServiceAbilityObj = GetProxy(err);
    if (i18nServiceAbilityObj == nullptr) {
        return err;
    }
    return i18nServiceAbilityObj->Set24HourClock(flag);
}

I18nErrorCode I18nServiceAbilityClient::SetUsingLocalDigit(bool flag)
{
    I18nErrorCode err = I18nErrorCode::SUCCESS;
    sptr<II18nServiceAbility> i18nServiceAbilityObj = GetProxy(err);
    if (i18nServiceAbilityObj == nullptr) {
        return err;
    }
    return i18nServiceAbilityObj->SetUsingLocalDigit(flag);
}

I18nErrorCode I18nServiceAbilityClient::AddPreferredLanguage(const std::string &language, int32_t index)
{
    I18nErrorCode err = I18nErrorCode::SUCCESS;
    sptr<II18nServiceAbility> i18nServiceAbilityObj = GetProxy(err);
    if (i18nServiceAbilityObj == nullptr) {
        return err;
    }
    return i18nServiceAbilityObj->AddPreferredLanguage(language, index);
}

I18nErrorCode I18nServiceAbilityClient::RemovePreferredLanguage(int32_t index)
{
    I18nErrorCode err = I18nErrorCode::SUCCESS;
    sptr<II18nServiceAbility> i18nServiceAbilityObj = GetProxy(err);
    if (i18nServiceAbilityObj == nullptr) {
        return err;
    }
    return i18nServiceAbilityObj->RemovePreferredLanguage(index);
}
} // namespace I18n
} // namespace Global
} // namespace OHOS