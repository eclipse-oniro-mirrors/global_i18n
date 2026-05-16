/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifdef SUPPORT_GRAPHICS
#include "app_mgr_client.h"
#include "ability_manager_client.h"
#include <common_event_manager.h>
#include <common_event_publish_info.h>
#include <common_event_support.h>
#endif
#include "i18n_hilog.h"
#include "locale_config.h"
#include "os_account_manager.h"
#include "parameter.h"
#include "utils.h"

#include "multi_users.h"

namespace OHOS {
namespace Global {
namespace I18n {
const std::string MultiUsers::MULTI_USERS_LANGUAGE_KEY = "languageData";
const std::string MultiUsers::MULTI_USERS_LOCALE_KEY = "localeData";
const std::string MultiUsers::MULTI_USERS_HOUR_KEY = "is24HourData";
const std::string MultiUsers::INIT_KEY = "init";
const std::string MultiUsers::PREFERENCE_PATH = "/data/service/el1/public/i18n/global/GlobalParamData";
const int32_t MultiUsers::DEFAULT_LOCAL_ID = 100;
const int MultiUsers::CONFIG_LEN = 128;
std::shared_ptr<NativePreferences::Preferences> MultiUsers::preferences = nullptr;

void MultiUsers::InitMultiUser()
{
    InitPreferences();
    if (preferences == nullptr) {
        HILOG_ERROR_I18N("InitMultiUser: InitPreferences failed");
        return;
    }
    bool init = preferences->GetBool(INIT_KEY, false);
    std::string localId;
    I18nErrorCode errCode = GetForegroundLocalId(localId);
    if (errCode != I18nErrorCode::SUCCESS) {
        HILOG_ERROR_I18N("InitMultiUser: get foreground local id failed");
        return;
    }
    if (!init) {
        AddUser(localId);
        preferences->PutBool(INIT_KEY, true);
        preferences->Flush();
        HILOG_INFO_I18N("InitMultiUser: init multi user data success");
    }
}

void MultiUsers::SwitchUser(const std::string& curLocalId)
{
    if (!IsValidLocalId(curLocalId)) {
        HILOG_ERROR_I18N("SwitchUser: curLocalId is an invalid LocalId");
        return;
    }
    I18nErrorCode errCode = LoadGlobalParam(curLocalId);
    if (errCode != I18nErrorCode::SUCCESS) {
        HILOG_ERROR_I18N("SwitchUser: load global params failed");
    }
}

void MultiUsers::AddUser(const std::string& localId)
{
    if (!IsValidLocalId(localId)) {
        HILOG_ERROR_I18N("AddUser: localId is invalid");
        return;
    }
    I18nErrorCode errCode = SaveGlobalParam(localId);
    if (errCode != I18nErrorCode::SUCCESS) {
        HILOG_ERROR_I18N("AddUser: add global param failed");
    }
}

void MultiUsers::RemoveUser(const std::string& localId)
{
    if (!IsValidLocalId(localId)) {
        HILOG_ERROR_I18N("RemoveUser: localId is invalid");
        return;
    }
    I18nErrorCode errCode = RemoveGlobalParam(localId);
    if (errCode != I18nErrorCode::SUCCESS) {
        HILOG_ERROR_I18N("RemoveUser: remove global param failed");
    }
}

I18nErrorCode MultiUsers::GetForegroundLocalId(std::string& localId)
{
    int id = 0;
    int errCode = OHOS::AccountSA::OsAccountManager::GetForegroundOsAccountLocalId(id);
    if (errCode != 0) {
        HILOG_ERROR_I18N("GetForegroundLocalId: get foreground locale Id failed, errCode is %{public}d", errCode);
        return I18nErrorCode::FAILED;
    }
    localId = std::to_string(id);
    return I18nErrorCode::SUCCESS;
}

I18nErrorCode MultiUsers::SaveLanguage(const std::string& localId, const std::string& language)
{
    std::string foregroundLocalId = localId;
    I18nErrorCode errCode = I18nErrorCode::SUCCESS;
    if (localId.empty()) {
        errCode = MultiUsers::GetForegroundLocalId(foregroundLocalId);
    }
    if (errCode != I18nErrorCode::SUCCESS) {
        HILOG_ERROR_I18N("SaveLanguage: get foreground locale Id failed");
        return I18nErrorCode::FAILED;
    }

    errCode =
        WriteMultiUsersParameter(MULTI_USERS_LANGUAGE_KEY, language, foregroundLocalId, false);
    if (errCode != I18nErrorCode::SUCCESS) {
        HILOG_ERROR_I18N("SaveLanguage: save language failed");
        return I18nErrorCode::FAILED;
    }
    return I18nErrorCode::SUCCESS;
}

I18nErrorCode MultiUsers::SaveLocale(const std::string& localId, const std::string& locale)
{
    std::string foregroundLocalId = localId;
    I18nErrorCode errCode = I18nErrorCode::SUCCESS;
    if (localId.empty()) {
        errCode = MultiUsers::GetForegroundLocalId(foregroundLocalId);
    }
    if (errCode != I18nErrorCode::SUCCESS) {
        HILOG_ERROR_I18N("SaveLocale: get foreground locale Id failed");
        return I18nErrorCode::FAILED;
    }

    errCode =
        WriteMultiUsersParameter(MULTI_USERS_LOCALE_KEY, locale, foregroundLocalId, false);
    if (errCode != I18nErrorCode::SUCCESS) {
        HILOG_ERROR_I18N("SaveLocale: save locale failed");
        return I18nErrorCode::FAILED;
    }
    return I18nErrorCode::SUCCESS;
}

I18nErrorCode MultiUsers::SaveIs24Hour(const std::string& localId, const std::string& is24Hour)
{
    std::string foregroundLocalId = localId;
    I18nErrorCode errCode = I18nErrorCode::SUCCESS;
    if (localId.empty()) {
        errCode = MultiUsers::GetForegroundLocalId(foregroundLocalId);
    }
    if (errCode != I18nErrorCode::SUCCESS) {
        HILOG_ERROR_I18N("SaveLanguage: get foreground locale Id failed");
        return I18nErrorCode::FAILED;
    }
    errCode =
        WriteMultiUsersParameter(MULTI_USERS_HOUR_KEY, is24Hour, foregroundLocalId, false);
    if (errCode != I18nErrorCode::SUCCESS) {
        HILOG_ERROR_I18N("SaveIs24Hour: save is24Hour failed");
        return I18nErrorCode::FAILED;
    }
    return I18nErrorCode::SUCCESS;
}

std::string MultiUsers::GetLanguageFromUserId(int32_t userId)
{
    std::string localId = std::to_string(userId);
    std::string locale = ReadMultiUsersParameter(MULTI_USERS_LOCALE_KEY, localId);
    std::string language = ReadMultiUsersParameter(MULTI_USERS_LANGUAGE_KEY, localId);
    if (locale.empty() || language.empty()) {
        HILOG_ERROR_I18N("MultiUsers::GetLanguageFromUserId: Get language from userId %{public}d failed, "
            "locale %{public}s, language %{public}s.", userId, locale.c_str(), language.c_str());
        return LocaleConfig::GetEffectiveLanguage();
    }
    return LocaleConfig::ComputeEffectiveLanguage(locale, language);
}

std::string MultiUsers::GetSystemLanguageFromUserId(int32_t userId)
{
    if (userId == -1) {
        return LocaleConfig::GetSystemLanguage();
    }
    std::string localId = std::to_string(userId);
    std::string language = ReadMultiUsersParameter(MULTI_USERS_LANGUAGE_KEY, localId);
    if (language.empty()) {
        HILOG_ERROR_I18N("MultiUsers::GetSystemLanguageFromUserId: Get language failed.");
        return LocaleConfig::GetSystemLanguage();
    }
    return language;
}

std::string MultiUsers::GetSystemLocaleFromUserId(int32_t userId)
{
    if (userId == -1) {
        return LocaleConfig::GetEffectiveLocale();
    }
    std::string localId = std::to_string(userId);
    std::string locale = ReadMultiUsersParameter(MULTI_USERS_LOCALE_KEY, localId);
    if (locale.empty()) {
        HILOG_ERROR_I18N("MultiUsers::GetSystemLocaleFromUserId: Get locale failed.");
        return LocaleConfig::GetEffectiveLocale();
    }
    return locale;
}

I18nErrorCode MultiUsers::SaveGlobalParam(const std::string& localId)
{
    std::string language = LocaleConfig::GetSystemLanguage();
    std::string locale = LocaleConfig::GetEffectiveLocale();
    std::string is24Hour = ReadSystemParameter(LocaleConfig::HOUR_KEY.c_str(), CONFIG_LEN);
    if (is24Hour.empty()) {
        HILOG_ERROR_I18N("SaveIs24Hour: Get is24Hour failed");
        return I18nErrorCode::FAILED;
    }
    I18nErrorCode errCode = SaveLanguage(localId, language);
    if (errCode != I18nErrorCode::SUCCESS) {
        HILOG_ERROR_I18N("SaveGlobalParam: save language failed");
        return I18nErrorCode::FAILED;
    }
    errCode = SaveLocale(localId, locale);
    if (errCode != I18nErrorCode::SUCCESS) {
        HILOG_ERROR_I18N("SaveGlobalParam: save locale failed");
        return I18nErrorCode::FAILED;
    }
    errCode = SaveIs24Hour(localId, is24Hour);
    if (errCode != I18nErrorCode::SUCCESS) {
        HILOG_ERROR_I18N("SaveGlobalParam: save is24Hour failed");
        return I18nErrorCode::FAILED;
    }
    return I18nErrorCode::SUCCESS;
}

I18nErrorCode MultiUsers::LoadGlobalParam(const std::string& localId)
{
    std::string newLocale = ReadMultiUsersParameter(MULTI_USERS_LOCALE_KEY, localId);
    if (!newLocale.empty() && SetParameter(LocaleConfig::LOCALE_KEY.c_str(), newLocale.c_str()) != 0) {
        HILOG_ERROR_I18N("LoadGlobalParam: set locale failed");
        return I18nErrorCode::FAILED;
    }

    std::string newLanguage = ReadMultiUsersParameter(MULTI_USERS_LANGUAGE_KEY, localId);
    if (!newLanguage.empty() && SetParameter(LocaleConfig::LANGUAGE_KEY.c_str(), newLanguage.c_str()) != 0) {
        HILOG_ERROR_I18N("LoadGlobalParam: set language failed");
        return I18nErrorCode::FAILED;
    }

    std::string newIs24Hour = ReadMultiUsersParameter(MULTI_USERS_HOUR_KEY, localId);
    if (!newIs24Hour.empty() && SetParameter(LocaleConfig::HOUR_KEY.c_str(), newIs24Hour.c_str()) != 0) {
        HILOG_ERROR_I18N("LoadGlobalParam: set is24Hour failed");
        return I18nErrorCode::FAILED;
    }
#ifdef SUPPORT_GRAPHICS
    int32_t status = 0;
    int32_t userId = ConvertString2Int(localId, status);
    if (status == -1) {
        HILOG_ERROR_I18N("LoadGlobalParam: convert userId failed");
        return I18nErrorCode::FAILED;
    }
    std::string effectiveLanguage = LocaleConfig::ComputeEffectiveLanguage(newLocale, newLanguage);
    UpdateConfiguration(newLocale, effectiveLanguage, newIs24Hour, userId);
    return PublishCommonEvent(userId);
#endif
    return I18nErrorCode::SUCCESS;
}

I18nErrorCode MultiUsers::RemoveGlobalParam(const std::string& localId)
{
    I18nErrorCode errCode = WriteMultiUsersParameter(MULTI_USERS_LANGUAGE_KEY, "", localId, true);
    if (errCode != I18nErrorCode::SUCCESS) {
        HILOG_ERROR_I18N("RemoveGlobalParam: remove language failed");
        return I18nErrorCode::FAILED;
    }

    errCode = WriteMultiUsersParameter(MULTI_USERS_LOCALE_KEY, "", localId, true);
    if (errCode != I18nErrorCode::SUCCESS) {
        HILOG_ERROR_I18N("RemoveGlobalParam: remove locale failed");
        return I18nErrorCode::FAILED;
    }

    errCode = WriteMultiUsersParameter(MULTI_USERS_HOUR_KEY, "", localId, true);
    if (errCode != I18nErrorCode::SUCCESS) {
        HILOG_ERROR_I18N("RemoveGlobalParam: remove is24Hour failed");
        return I18nErrorCode::FAILED;
    }

    return I18nErrorCode::SUCCESS;
}

std::string MultiUsers::ReadMultiUsersParameter(const std::string& paramKey, const std::string& localId)
{
    std::string param = GetParamFromPreferences(paramKey);
    if (param.empty()) {
        return "";
    }
    std::vector<std::string> multiUsersParam;
    Split(param, ";", multiUsersParam);
    for (auto& userParam : multiUsersParam) {
        std::vector<std::string> content;
        Split(userParam, ":", content);
        // 2 is number of param
        if (content.size() != 2) {
            continue;
        }
        if (content[0] == localId) {
            return content[1];
        }
    }
    return "";
}

I18nErrorCode MultiUsers::WriteMultiUsersParameter(const std::string& paramKey, const std::string& paramValue,
    const std::string& localId, bool isDel)
{
    std::string param = GetParamFromPreferences(paramKey);
    std::vector<std::string> multiUsersParam;
    Split(param, ";", multiUsersParam);
    std::vector<std::string> newMultiUsersParam;
    bool userIsExist = false;
    for (auto& userParam : multiUsersParam) {
        std::vector<std::string> content;
        Split(userParam, ":", content);
        // 2 is number of param
        if (content.size() != 2) {
            continue;
        }
        std::string userLocalId = content[0];
        if (!isDel && userLocalId == localId) {
            content[1] = paramValue;
            Merge(content, ":", userParam);
            userIsExist = true;
        }
        newMultiUsersParam.emplace_back(userParam);
        if (isDel && userLocalId == localId) {
            newMultiUsersParam.pop_back();
        }
    }
    if (!isDel && !userIsExist) {
        newMultiUsersParam.push_back(localId + ":" + paramValue);
    }
    std::string newParam;
    Merge(newMultiUsersParam, ";", newParam);
    if (SetParamFromPreferences(paramKey, newParam) != I18nErrorCode::SUCCESS) {
        HILOG_ERROR_I18N("WriteMultiUsersParameter: set param %{public}s failed", paramKey.c_str());
        return I18nErrorCode::FAILED;
    }
    return I18nErrorCode::SUCCESS;
}

bool MultiUsers::IsValidLocalId(const std::string& localId)
{
    if (std::atoi(localId.c_str()) < DEFAULT_LOCAL_ID) {
        HILOG_ERROR_I18N("IsValidLocalId: invalid local ID");
        return false;
    }
    return true;
}

void MultiUsers::InitPreferences()
{
    if (preferences == nullptr) {
        HILOG_INFO_I18N("InitPreferences: preferences Init");
        OHOS::NativePreferences::Options opt(PREFERENCE_PATH);
        int status = 0;
        preferences = NativePreferences::PreferencesHelper::GetPreferences(opt, status);
        if (status != 0) {
            HILOG_ERROR_I18N("InitPreferences: get preferences failed");
            preferences = nullptr;
        }
    }
}

std::string MultiUsers::GetParamFromPreferences(const std::string& paramKey)
{
    InitPreferences();
    if (preferences == nullptr) {
        HILOG_ERROR_I18N("GetParamFromPreferences: preferences is nullptr");
        return "";
    }
    return preferences->GetString(paramKey, "");
}

I18nErrorCode MultiUsers::SetParamFromPreferences(const std::string& paramKey, const std::string& paramValue)
{
    InitPreferences();
    if (preferences == nullptr) {
        HILOG_ERROR_I18N("SetParamFromPreferences: preferences is nullptr");
        return I18nErrorCode::FAILED;
    }
    int status = preferences->PutString(paramKey, paramValue);
    if (status != 0) {
        HILOG_ERROR_I18N("SetParamFromPreferences: put param %{public}s failed", paramKey.c_str());
        return I18nErrorCode::FAILED;
    }
    preferences->Flush();
    return I18nErrorCode::SUCCESS;
}

#ifdef SUPPORT_GRAPHICS
void MultiUsers::UpdateConfiguration(const std::string& locale, const std::string& language,
    const std::string& is24Hour, int32_t userId)
{
    AppExecFwk::Configuration configuration;
    configuration.AddItem(AAFwk::GlobalConfigurationKey::SYSTEM_LOCALE, locale);
    configuration.AddItem(AAFwk::GlobalConfigurationKey::SYSTEM_LANGUAGE, language);
    configuration.AddItem(AAFwk::GlobalConfigurationKey::SYSTEM_HOUR, is24Hour);
    auto appMgrClient = std::make_unique<AppExecFwk::AppMgrClient>();

    AppExecFwk::AppMgrResultCode status = appMgrClient->UpdateConfiguration(configuration, userId);
    if (status != AppExecFwk::AppMgrResultCode::RESULT_OK) {
        HILOG_ERROR_I18N("MultiUsers::UpdateConfiguration: Update configuration userId %{public}d failed.", userId);
        return;
    }
    HILOG_INFO_I18N("MultiUsers::UpdateConfiguration: Update configuration userId %{public}d success.", userId);
}

I18nErrorCode MultiUsers::PublishCommonEvent(int32_t userId)
{
    OHOS::AAFwk::Want localeChangeWant;
    localeChangeWant.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_LOCALE_CHANGED);
    OHOS::EventFwk::CommonEventData localeChangeEvent(localeChangeWant);
    if (!OHOS::EventFwk::CommonEventManager::PublishCommonEventAsUser(localeChangeEvent, userId)) {
        HILOG_ERROR_I18N("MultiUsers::PublishCommonEvent: Failed to publish locale change event, userId %{public}d.",
            userId);
        return I18nErrorCode::PUBLISH_COMMON_EVENT_FAILED;
    }

    OHOS::AAFwk::Want is24HourChangeWant;
    is24HourChangeWant.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_LOCALE_CHANGED);
    OHOS::EventFwk::CommonEventData is24HourChangeEvent(is24HourChangeWant);
    is24HourChangeEvent.SetData(LocaleConfig::HOUR_EVENT_DATA);
    if (!OHOS::EventFwk::CommonEventManager::PublishCommonEventAsUser(is24HourChangeEvent, userId)) {
        HILOG_ERROR_I18N("MultiUsers::PublishCommonEvent: Failed to publish is24Hour change event, userId %{public}d.",
            userId);
        return I18nErrorCode::PUBLISH_COMMON_EVENT_FAILED;
    }

    HILOG_INFO_I18N("MultiUsers::PublishCommonEvent: Publish event finished,  userId %{public}d.", userId);
    return I18nErrorCode::SUCCESS;
}
#endif
} // namespace I18n
} // namespace Global
} // namespace OHOS
