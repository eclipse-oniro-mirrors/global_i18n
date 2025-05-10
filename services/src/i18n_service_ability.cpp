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

#include "accesstoken_kit.h"
#include "i18n_hilog.h"
#include "i18n_service_ability_load_manager.h"
#include "ipc_skeleton.h"
#include "locale_config.h"
#include "mem_mgr_client.h"
#include "mem_mgr_proxy.h"
#include "multi_users.h"
#include "os_account_manager.h"
#include "preferred_language.h"
#include "preferences.h"
#include "preferences_helper.h"
#include "system_ability_definition.h"
#include "tokenid_kit.h"
#include "i18n_service_ability.h"

namespace OHOS {
namespace Global {
namespace I18n {
REGISTER_SYSTEM_ABILITY_BY_ID(I18nServiceAbility, I18N_SA_ID, false);
static const std::string UNLOAD_TASK = "i18n_service_unload";
static const uint32_t DELAY_MILLISECONDS_FOR_UNLOAD_SA = 10000;
static const int32_t UID_TRANSFORM_DIVISOR = 200000;
static const int32_t USER_DEFAULT = 100;

I18nServiceAbility::I18nServiceAbility(int32_t saId, bool runOnCreate) : SystemAbility(saId, runOnCreate), taskNumber(0)
{
    HILOG_INFO_I18N("I18nServiceAbility object init success.");
}

I18nServiceAbility::~I18nServiceAbility()
{
    HILOG_INFO_I18N("I18nServiceAbility object release.");
}

ErrCode I18nServiceAbility::SetSystemLanguage(const std::string& language, int32_t& code)
{
    int pid = getpid();
    StartTask(pid);
    I18nErrorCode errCode = CheckPermission();
    if (errCode != I18nErrorCode::SUCCESS) {
        HILOG_ERROR_I18N("I18nServiceAbility::SetSystemLanguage: Check permission failed.");
        code = static_cast<int32_t>(errCode);
        FinishTask(pid);
        return ERR_OK;
    }
#ifdef SUPPORT_MULTI_USER
    int32_t userId = GetCallingUserId();
    errCode = LocaleConfig::SetSystemLanguage(language, userId);
#else
    errCode = LocaleConfig::SetSystemLanguage(language);
#endif
    code = static_cast<int32_t>(errCode);
    FinishTask(pid);
    return ERR_OK;
}

ErrCode I18nServiceAbility::SetSystemRegion(const std::string& region, int32_t& code)
{
    int pid = getpid();
    StartTask(pid);
    I18nErrorCode errCode = CheckPermission();
    if (errCode != I18nErrorCode::SUCCESS) {
        HILOG_ERROR_I18N("I18nServiceAbility::SetSystemRegion: Check permission failed.");
        code = static_cast<int32_t>(errCode);
        FinishTask(pid);
        return ERR_OK;
    }
#ifdef SUPPORT_MULTI_USER
    int32_t userId = GetCallingUserId();
    errCode = LocaleConfig::SetSystemRegion(region, userId);
#else
    errCode = LocaleConfig::SetSystemRegion(region);
#endif
    code = static_cast<int32_t>(errCode);
    FinishTask(pid);
    return ERR_OK;
}

ErrCode I18nServiceAbility::SetSystemLocale(const std::string& locale, int32_t& code)
{
    int pid = getpid();
    StartTask(pid);
    I18nErrorCode errCode = CheckPermission();
    if (errCode != I18nErrorCode::SUCCESS) {
        HILOG_ERROR_I18N("I18nServiceAbility::SetSystemLocale: Check permission failed.");
        code = static_cast<int32_t>(errCode);
        FinishTask(pid);
        return ERR_OK;
    }
#ifdef SUPPORT_MULTI_USER
    int32_t userId = GetCallingUserId();
    errCode = LocaleConfig::SetSystemLocale(locale, userId);
#else
    errCode = LocaleConfig::SetSystemLocale(locale);
#endif
    code = static_cast<int32_t>(errCode);
    FinishTask(pid);
    return ERR_OK;
}

ErrCode I18nServiceAbility::Set24HourClock(const std::string& flag, int32_t& code)
{
    int pid = getpid();
    StartTask(pid);
    I18nErrorCode errCode = CheckPermission();
    if (errCode != I18nErrorCode::SUCCESS) {
        HILOG_ERROR_I18N("I18nServiceAbility::Set24HourClock: Check permission failed.");
        code = static_cast<int32_t>(errCode);
        FinishTask(pid);
        return ERR_OK;
    }
#ifdef SUPPORT_MULTI_USER
    int32_t userId = GetCallingUserId();
    errCode = LocaleConfig::Set24HourClock(flag, userId);
#else
    errCode = LocaleConfig::Set24HourClock(flag);
#endif
    code = static_cast<int32_t>(errCode);
    FinishTask(pid);
    return ERR_OK;
}

ErrCode I18nServiceAbility::SetUsingLocalDigit(bool flag, int32_t& code)
{
    int pid = getpid();
    StartTask(pid);
    I18nErrorCode errCode = CheckPermission();
    if (errCode != I18nErrorCode::SUCCESS) {
        HILOG_ERROR_I18N("I18nServiceAbility::SetUsingLocalDigit: Check permission failed.");
        code = static_cast<int32_t>(errCode);
        FinishTask(pid);
        return ERR_OK;
    }
#ifdef SUPPORT_MULTI_USER
    int32_t userId = GetCallingUserId();
    errCode = LocaleConfig::SetUsingLocalDigit(flag, userId);
#else
    errCode = LocaleConfig::SetUsingLocalDigit(flag);
#endif
    code = static_cast<int32_t>(errCode);
    FinishTask(pid);
    return ERR_OK;
}

ErrCode I18nServiceAbility::AddPreferredLanguage(const std::string& language, int32_t index, int32_t& code)
{
    int pid = getpid();
    StartTask(pid);
    I18nErrorCode errCode = CheckPermission();
    if (errCode != I18nErrorCode::SUCCESS) {
        HILOG_ERROR_I18N("I18nServiceAbility::AddPreferredLanguage: Check permission failed.");
        code = static_cast<int32_t>(errCode);
        FinishTask(pid);
        return ERR_OK;
    }
    errCode = PreferredLanguage::AddPreferredLanguage(language, index);
    code = static_cast<int32_t>(errCode);
    FinishTask(pid);
    return ERR_OK;
}

ErrCode I18nServiceAbility::RemovePreferredLanguage(int32_t index, int32_t& code)
{
    int pid = getpid();
    StartTask(pid);
    I18nErrorCode errCode = CheckPermission();
    if (errCode != I18nErrorCode::SUCCESS) {
        HILOG_ERROR_I18N("I18nServiceAbility::RemovePreferredLanguage: Check permission failed.");
        code = static_cast<int32_t>(errCode);
        FinishTask(pid);
        return ERR_OK;
    }
    errCode = PreferredLanguage::RemovePreferredLanguage(index);
    code = static_cast<int32_t>(errCode);
    FinishTask(pid);
    return ERR_OK;
}

ErrCode I18nServiceAbility::SetTemperatureType(int32_t type, int32_t& code)
{
    int pid = getpid();
    StartTask(pid);
    I18nErrorCode errCode = CheckPermission();
    if (errCode != I18nErrorCode::SUCCESS) {
        HILOG_ERROR_I18N("I18nServiceAbility::SetTemperatureType: Check permission failed.");
        code = static_cast<int32_t>(errCode);
        FinishTask(pid);
        return ERR_OK;
    }
    TemperatureType temperatureType = static_cast<TemperatureType>(type);
#ifdef SUPPORT_MULTI_USER
    int32_t userId = GetCallingUserId();
    errCode = LocaleConfig::SetTemperatureType(temperatureType, userId);
#else
    errCode = LocaleConfig::SetTemperatureType(temperatureType);
#endif
    code = static_cast<int32_t>(errCode);
    FinishTask(pid);
    return ERR_OK;
}

ErrCode I18nServiceAbility::SetFirstDayOfWeek(int32_t type, int32_t& code)
{
    int pid = getpid();
    StartTask(pid);
    I18nErrorCode errCode = CheckPermission();
    if (errCode != I18nErrorCode::SUCCESS) {
        HILOG_ERROR_I18N("I18nServiceAbility::SetFirstDayOfWeek: Check permission failed.");
        code = static_cast<int32_t>(errCode);
        FinishTask(pid);
        return ERR_OK;
    }
    WeekDay weekDay = static_cast<WeekDay>(type);
#ifdef SUPPORT_MULTI_USER
    int32_t userId = GetCallingUserId();
    errCode = LocaleConfig::SetFirstDayOfWeek(weekDay, userId);
#else
    errCode = LocaleConfig::SetFirstDayOfWeek(weekDay);
#endif
    code = static_cast<int32_t>(errCode);
    FinishTask(pid);
    return ERR_OK;
}

void I18nServiceAbility::DelayUnloadI18nServiceAbility()
{
    if (handler != nullptr) {
        handler->RemoveTask(UNLOAD_TASK);
    }
    auto task = [this]() {
        auto i18nSaLoadManager = DelayedSingleton<I18nServiceAbilityLoadManager>::GetInstance();
        if (i18nSaLoadManager != nullptr) {
            HILOG_INFO_I18N("I18nServiceAbility::UnloadI18nServiceAbility start to unload i18n sa.");
            i18nSaLoadManager->UnloadI18nService(I18N_SA_ID);
        }
    };
    if (handler != nullptr) {
        handler->PostTask(task, UNLOAD_TASK, DELAY_MILLISECONDS_FOR_UNLOAD_SA);
    }
}

void I18nServiceAbility::OnStart(const SystemAbilityOnDemandReason& startReason)
{
    HILOG_INFO_I18N("I18nServiceAbility start.");
    i18nServiceEvent = std::make_unique<I18nServiceEvent>();
    if (i18nServiceEvent != nullptr) {
        i18nServiceEvent->SubscriberEvent();
        i18nServiceEvent->CheckStartReason(startReason);
    }
#ifdef SUPPORT_MULTI_USER
    MultiUsers::InitMultiUser();
#endif
    bool status = Publish(this);
    if (status) {
        HILOG_INFO_I18N("I18nServiceAbility Publish success.");
    } else {
        HILOG_INFO_I18N("I18nServiceAbility Publish failed.");
    }
    handler = std::make_shared<AppExecFwk::EventHandler>(AppExecFwk::EventRunner::Create(true));
    DelayUnloadI18nServiceAbility();
    int pid = getpid();
    Memory::MemMgrClient::GetInstance().NotifyProcessStatus(pid, 1, 1, I18N_SA_ID);
}

void I18nServiceAbility::OnStop()
{
    int pid = getpid();
    Memory::MemMgrClient::GetInstance().NotifyProcessStatus(pid, 1, 0, I18N_SA_ID);
    HILOG_INFO_I18N("I18nServiceAbility Stop.");
}

int32_t I18nServiceAbility::OnIdle(const SystemAbilityOnDemandReason& idleReason)
{
    if (UpdateTaskNumber(ModifyTaskNumber::QUERY) == 0) {
        HILOG_INFO_I18N("I18nServiceAbility::OnIdle: Immediately unload.");
        return 0;
    }
    HILOG_INFO_I18N("I18nServiceAbility::OnIdle: Delay unload.");
    return DELAY_MILLISECONDS_FOR_UNLOAD_SA;
}

int32_t I18nServiceAbility::UpdateTaskNumber(ModifyTaskNumber action)
{
    std::lock_guard<std::mutex> taskLock(taskNumberMutex);
    taskNumber += static_cast<int32_t>(action);
    return taskNumber;
}

void I18nServiceAbility::StartTask(int32_t pid)
{
    if (Memory::MemMgrClient::GetInstance().SetCritical(pid, true, I18N_SA_ID) != 0) {
        HILOG_ERROR_I18N("I18nServiceAbility::StartTask: Set critical true failed.");
    }
    UpdateTaskNumber(ModifyTaskNumber::INCREASE);
    DelayUnloadI18nServiceAbility();
}

void I18nServiceAbility::FinishTask(int32_t pid)
{
    if (Memory::MemMgrClient::GetInstance().SetCritical(pid, false, I18N_SA_ID) != 0) {
        HILOG_ERROR_I18N("I18nServiceAbility::StartTask: Set critical true failed.");
    }
    UpdateTaskNumber(ModifyTaskNumber::DECREASE);
}

int32_t I18nServiceAbility::GetCallingUserId()
{
    int32_t userId = OHOS::IPCSkeleton::GetCallingUid() / UID_TRANSFORM_DIVISOR;
    if (userId == 0) {
        auto err = OHOS::AccountSA::OsAccountManager::GetForegroundOsAccountLocalId(userId);
        if (err != 0) {
            HILOG_ERROR_I18N("I18nServiceAbility::GetCallingUserId: GetForegroundOsAccountLocalId failed.");
            userId = USER_DEFAULT;
        }
    }
    return userId;
}

/**
 * check whether request process has correct tokenType and permission.
 */
I18nErrorCode I18nServiceAbility::CheckPermission()
{
    uint64_t callerFullToken = IPCSkeleton::GetCallingFullTokenID();
    uint32_t callerToken = IPCSkeleton::GetCallingTokenID();
    bool isSystemApp = Security::AccessToken::TokenIdKit::IsSystemAppByFullTokenID(callerFullToken);
    Security::AccessToken::ATokenTypeEnum tokenType =
        Security::AccessToken::AccessTokenKit::GetTokenTypeFlag(callerToken);
    bool isShell = tokenType == Security::AccessToken::ATokenTypeEnum::TOKEN_SHELL;
    bool isNative = tokenType == Security::AccessToken::ATokenTypeEnum::TOKEN_NATIVE;
    if (!isSystemApp && !isShell && !isNative) {
        HILOG_ERROR_I18N("I18nServiceAbility caller process is not System app, Shell or Native.");
        return I18nErrorCode::NOT_SYSTEM_APP;
    }
    int result = Security::AccessToken::AccessTokenKit::VerifyAccessToken(callerToken,
        "ohos.permission.UPDATE_CONFIGURATION");
    if (result != Security::AccessToken::PermissionState::PERMISSION_GRANTED) {
        HILOG_ERROR_I18N("I18nServiceAbility caller process doesn't have UPDATE_CONFIGURATION permission.");
        return I18nErrorCode::NO_PERMISSION;
    }
    return I18nErrorCode::SUCCESS;
}
} // namespace I18n
} // namespace Global
} // namespace OHOS