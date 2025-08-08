/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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
        HILOG_ERROR_I18N("I18nServiceAbilityClient::SetSystemLanguage: Get proxy failed.");
        return err;
    }
    int32_t code = 0;
    ErrCode errCode = i18nServiceAbilityObj->SetSystemLanguage(language, code);
    if (errCode != ERR_OK) {
        HILOG_ERROR_I18N("I18nServiceAbilityClient::SetSystemLanguage: Set language failed, errCode is %{public}d.",
            static_cast<int32_t>(errCode));
        return I18nErrorCode::FAILED;
    }
    return static_cast<I18nErrorCode>(code);
}

I18nErrorCode I18nServiceAbilityClient::SetSystemRegion(const std::string &region)
{
    I18nErrorCode err = I18nErrorCode::SUCCESS;
    sptr<II18nServiceAbility> i18nServiceAbilityObj = GetProxy(err);
    if (i18nServiceAbilityObj == nullptr) {
        HILOG_ERROR_I18N("I18nServiceAbilityClient::SetSystemRegion: Get proxy failed.");
        return err;
    }
    int32_t code = 0;
    ErrCode errCode = i18nServiceAbilityObj->SetSystemRegion(region, code);
    if (errCode != ERR_OK) {
        HILOG_ERROR_I18N("I18nServiceAbilityClient::SetSystemRegion: Set region failed, errCode is %{public}d.",
            static_cast<int32_t>(errCode));
        return I18nErrorCode::FAILED;
    }
    return static_cast<I18nErrorCode>(code);
}

I18nErrorCode I18nServiceAbilityClient::SetSystemLocale(const std::string &locale)
{
    I18nErrorCode err = I18nErrorCode::SUCCESS;
    sptr<II18nServiceAbility> i18nServiceAbilityObj = GetProxy(err);
    if (i18nServiceAbilityObj == nullptr) {
        HILOG_ERROR_I18N("I18nServiceAbilityClient::SetSystemLocale: Get proxy failed.");
        return err;
    }
    int32_t code = 0;
    ErrCode errCode = i18nServiceAbilityObj->SetSystemLocale(locale, code);
    if (errCode != ERR_OK) {
        HILOG_ERROR_I18N("I18nServiceAbilityClient::SetSystemLocale: Set locale failed, errCode is %{public}d.",
            static_cast<int32_t>(errCode));
        return I18nErrorCode::FAILED;
    }
    return static_cast<I18nErrorCode>(code);
}

I18nErrorCode I18nServiceAbilityClient::Set24HourClock(const std::string &flag)
{
    I18nErrorCode err = I18nErrorCode::SUCCESS;
    sptr<II18nServiceAbility> i18nServiceAbilityObj = GetProxy(err);
    if (i18nServiceAbilityObj == nullptr) {
        HILOG_ERROR_I18N("I18nServiceAbilityClient::Set24HourClock: Get proxy failed.");
        return err;
    }
    int32_t code = 0;
    ErrCode errCode = i18nServiceAbilityObj->Set24HourClock(flag, code);
    if (errCode != ERR_OK) {
        HILOG_ERROR_I18N("I18nServiceAbilityClient::Set24HourClock: Set 24HourClock failed, errCode is %{public}d.",
            static_cast<int32_t>(errCode));
        return I18nErrorCode::FAILED;
    }
    return static_cast<I18nErrorCode>(code);
}

I18nErrorCode I18nServiceAbilityClient::SetUsingLocalDigit(bool flag)
{
    I18nErrorCode err = I18nErrorCode::SUCCESS;
    sptr<II18nServiceAbility> i18nServiceAbilityObj = GetProxy(err);
    if (i18nServiceAbilityObj == nullptr) {
        HILOG_ERROR_I18N("I18nServiceAbilityClient::SetUsingLocalDigit: Get proxy failed.");
        return err;
    }
    int32_t code = 0;
    ErrCode errCode = i18nServiceAbilityObj->SetUsingLocalDigit(flag, code);
    if (errCode != ERR_OK) {
        HILOG_ERROR_I18N("I18nServiceAbilityClient::SetUsingLocalDigit: Set local digit failed, "
            "errCode is %{public}d.", static_cast<int32_t>(errCode));
        return I18nErrorCode::FAILED;
    }
    return static_cast<I18nErrorCode>(code);
}

I18nErrorCode I18nServiceAbilityClient::AddPreferredLanguage(const std::string &language, int32_t index)
{
    I18nErrorCode err = I18nErrorCode::SUCCESS;
    sptr<II18nServiceAbility> i18nServiceAbilityObj = GetProxy(err);
    if (i18nServiceAbilityObj == nullptr) {
        HILOG_ERROR_I18N("I18nServiceAbilityClient::AddPreferredLanguage: Get proxy failed.");
        return err;
    }
    int32_t code = 0;
    ErrCode errCode = i18nServiceAbilityObj->AddPreferredLanguage(language, index, code);
    if (errCode != ERR_OK) {
        HILOG_ERROR_I18N("I18nServiceAbilityClient::AddPreferredLanguage: Add preferred language failed, "
            "errCode is %{public}d.", static_cast<int32_t>(errCode));
        return I18nErrorCode::FAILED;
    }
    return static_cast<I18nErrorCode>(code);
}

I18nErrorCode I18nServiceAbilityClient::RemovePreferredLanguage(int32_t index)
{
    I18nErrorCode err = I18nErrorCode::SUCCESS;
    sptr<II18nServiceAbility> i18nServiceAbilityObj = GetProxy(err);
    if (i18nServiceAbilityObj == nullptr) {
        HILOG_ERROR_I18N("I18nServiceAbilityClient::RemovePreferredLanguage: Get proxy failed.");
        return err;
    }
    int32_t code = 0;
    ErrCode errCode = i18nServiceAbilityObj->RemovePreferredLanguage(index, code);
    if (errCode != ERR_OK) {
        HILOG_ERROR_I18N("I18nServiceAbilityClient::RemovePreferredLanguage: Remove preferred language failed, "
            "errCode is %{public}d.", static_cast<int32_t>(errCode));
        return I18nErrorCode::FAILED;
    }
    return static_cast<I18nErrorCode>(code);
}

I18nErrorCode I18nServiceAbilityClient::SetTemperatureType(TemperatureType type)
{
    I18nErrorCode err = I18nErrorCode::SUCCESS;
    sptr<II18nServiceAbility> i18nServiceAbilityObj = GetProxy(err);
    if (i18nServiceAbilityObj == nullptr) {
        HILOG_ERROR_I18N("I18nServiceAbilityClient::SetTemperatureType: Get proxy failed.");
        return err;
    }
    int32_t code = 0;
    ErrCode errCode = i18nServiceAbilityObj->SetTemperatureType(static_cast<int32_t>(type), code);
    if (errCode != ERR_OK) {
        HILOG_ERROR_I18N("I18nServiceAbilityClient::SetTemperatureType: Set temperature type failed, "
            "errCode is %{public}d.", static_cast<int32_t>(errCode));
        return I18nErrorCode::FAILED;
    }
    return static_cast<I18nErrorCode>(code);
}

I18nErrorCode I18nServiceAbilityClient::SetFirstDayOfWeek(WeekDay type)
{
    I18nErrorCode err = I18nErrorCode::SUCCESS;
    sptr<II18nServiceAbility> i18nServiceAbilityObj = GetProxy(err);
    if (i18nServiceAbilityObj == nullptr) {
        HILOG_ERROR_I18N("I18nServiceAbilityClient::SetFirstDayOfWeek: Get proxy failed.");
        return err;
    }
    int32_t code = 0;
    ErrCode errCode = i18nServiceAbilityObj->SetFirstDayOfWeek(static_cast<int32_t>(type), code);
    if (errCode != ERR_OK) {
        HILOG_ERROR_I18N("I18nServiceAbilityClient::SetFirstDayOfWeek: Set first day of week failed, "
            "errCode is %{public}d.", static_cast<int32_t>(errCode));
        return I18nErrorCode::FAILED;
    }
    return static_cast<I18nErrorCode>(code);
}
} // namespace I18n
} // namespace Global
} // namespace OHOS