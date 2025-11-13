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
#include "ii18n_service_ability.h"
#include "iremote_object.h"
#include "system_ability_definition.h"
#include "i18n_service_ability_client.h"

namespace OHOS {
namespace Global {
namespace I18n {
template<typename Method, typename... Args>
I18nErrorCode I18nServiceAbilityClient::CallI18nServiceMethod(Method method, const char* methodName, Args&&... args)
{
    I18nErrorCode err = I18nErrorCode::SUCCESS;
    sptr<II18nServiceAbility> i18nServiceAbilityObj = GetProxy(err);
    if (i18nServiceAbilityObj == nullptr) {
        HILOG_ERROR_I18N("I18nServiceAbilityClient::%{public}s: Get proxy failed.", methodName);
        return err;
    }
    int32_t code = 0;
    ErrCode errCode = (i18nServiceAbilityObj->*method)(std::forward<Args>(args)..., code);
    if (errCode != ERR_OK) {
        HILOG_ERROR_I18N("I18nServiceAbilityClient::%{public}s: call i18n service failed, errCode is %{public}d.",
            methodName, static_cast<int32_t>(errCode));
        return I18nErrorCode::FAILED;
    }
    return static_cast<I18nErrorCode>(code);
}

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
    return CallI18nServiceMethod(&II18nServiceAbility::SetSystemLanguage, "SetSystemLanguage", language);
}

I18nErrorCode I18nServiceAbilityClient::SetSystemRegion(const std::string &region)
{
    return CallI18nServiceMethod(&II18nServiceAbility::SetSystemRegion, "SetSystemRegion", region);
}

I18nErrorCode I18nServiceAbilityClient::SetSystemLocale(const std::string &locale)
{
    return CallI18nServiceMethod(&II18nServiceAbility::SetSystemLocale, "SetSystemLocale", locale);
}

I18nErrorCode I18nServiceAbilityClient::Set24HourClock(const std::string &flag)
{
    return CallI18nServiceMethod(&II18nServiceAbility::Set24HourClock, "Set24HourClock", flag);
}

I18nErrorCode I18nServiceAbilityClient::SetUsingLocalDigit(bool flag)
{
    return CallI18nServiceMethod(&II18nServiceAbility::SetUsingLocalDigit, "SetUsingLocalDigit", flag);
}

I18nErrorCode I18nServiceAbilityClient::AddPreferredLanguage(const std::string &language, int32_t index)
{
    return CallI18nServiceMethod(&II18nServiceAbility::AddPreferredLanguage, "AddPreferredLanguage", language, index);
}

I18nErrorCode I18nServiceAbilityClient::RemovePreferredLanguage(int32_t index)
{
    return CallI18nServiceMethod(&II18nServiceAbility::RemovePreferredLanguage, "RemovePreferredLanguage", index);
}

I18nErrorCode I18nServiceAbilityClient::SetTemperatureType(TemperatureType type)
{
    return CallI18nServiceMethod(&II18nServiceAbility::SetTemperatureType, "SetTemperatureType", type);
}

I18nErrorCode I18nServiceAbilityClient::SetFirstDayOfWeek(WeekDay type)
{
    return CallI18nServiceMethod(&II18nServiceAbility::SetFirstDayOfWeek, "SetFirstDayOfWeek", type);
}

I18nErrorCode I18nServiceAbilityClient::GetSystemCollations(
    std::unordered_map<std::string, std::string>& systemCollations)
{
    return CallI18nServiceMethod(&II18nServiceAbility::GetSystemCollations, "GetSystemCollations", systemCollations);
}

I18nErrorCode I18nServiceAbilityClient::GetUsingCollation(std::string& usingCollation)
{
    return CallI18nServiceMethod(&II18nServiceAbility::GetUsingCollation, "GetUsingCollation", usingCollation);
}

I18nErrorCode I18nServiceAbilityClient::SetSystemCollation(const std::string& identifier)
{
    return CallI18nServiceMethod(&II18nServiceAbility::SetSystemCollation, "SetSystemCollation", identifier);
}

I18nErrorCode I18nServiceAbilityClient::GetSystemNumberingSystems(
    std::unordered_map<std::string, std::string>& systemNumberingSystems)
{
    return CallI18nServiceMethod(&II18nServiceAbility::GetSystemNumberingSystems, "GetSystemNumberingSystems",
        systemNumberingSystems);
}

I18nErrorCode I18nServiceAbilityClient::GetUsingNumberingSystem(std::string& usingNumberingSystem)
{
    return CallI18nServiceMethod(&II18nServiceAbility::GetUsingNumberingSystem, "GetUsingNumberingSystem",
        usingNumberingSystem);
}

I18nErrorCode I18nServiceAbilityClient::SetSystemNumberingSystem(const std::string& identifier)
{
    return CallI18nServiceMethod(&II18nServiceAbility::SetSystemNumberingSystem, "SetSystemNumberingSystem",
        identifier);
}

I18nErrorCode I18nServiceAbilityClient::GetSystemNumberPatterns(
    std::unordered_map<std::string, std::string>& systemNumberPatterns)
{
    return CallI18nServiceMethod(&II18nServiceAbility::GetSystemNumberPatterns, "GetSystemNumberPatterns",
        systemNumberPatterns);
}

I18nErrorCode I18nServiceAbilityClient::GetUsingNumberPattern(std::string& usingNumberPattern)
{
    return CallI18nServiceMethod(&II18nServiceAbility::GetUsingNumberPattern, "GetUsingNumberPattern",
        usingNumberPattern);
}

I18nErrorCode I18nServiceAbilityClient::SetSystemNumberPattern(const std::string& pattern)
{
    return CallI18nServiceMethod(&II18nServiceAbility::SetSystemNumberPattern, "SetSystemNumberPattern", pattern);
}

I18nErrorCode I18nServiceAbilityClient::GetSystemMeasurements(
    std::unordered_map<std::string, std::string>& systemMeasurements)
{
    return CallI18nServiceMethod(&II18nServiceAbility::GetSystemMeasurements, "GetSystemMeasurements",
        systemMeasurements);
}

I18nErrorCode I18nServiceAbilityClient::GetUsingMeasurement(std::string& identifier)
{
    return CallI18nServiceMethod(&II18nServiceAbility::GetUsingMeasurement, "GetUsingMeasurement", identifier);
}

I18nErrorCode I18nServiceAbilityClient::SetSystemMeasurement(const std::string& identifier)
{
    return CallI18nServiceMethod(&II18nServiceAbility::SetSystemMeasurement, "SetSystemMeasurement", identifier);
}

I18nErrorCode I18nServiceAbilityClient::GetSystemNumericalDatePatterns(
    std::unordered_map<std::string, std::string>& numericalDatePatterns)
{
    return CallI18nServiceMethod(&II18nServiceAbility::GetSystemNumericalDatePatterns,
        "GetSystemNumericalDatePatterns", numericalDatePatterns);
}

I18nErrorCode I18nServiceAbilityClient::GetUsingNumericalDatePattern(std::string& identifier)
{
    return CallI18nServiceMethod(&II18nServiceAbility::GetUsingNumericalDatePattern, "GetUsingNumericalDatePattern",
        identifier);
}

I18nErrorCode I18nServiceAbilityClient::SetSystemNumericalDatePattern(const std::string& identifier)
{
    return CallI18nServiceMethod(&II18nServiceAbility::SetSystemNumericalDatePattern, "SetSystemNumericalDatePattern",
        identifier);
}

std::string I18nServiceAbilityClient::GetLanguageFromUserId(int32_t userId)
{
    I18nErrorCode err = I18nErrorCode::SUCCESS;
    sptr<II18nServiceAbility> i18nServiceAbilityObj = GetProxy(err);
    if (i18nServiceAbilityObj == nullptr) {
        HILOG_ERROR_I18N("I18nServiceAbilityClient::GetLanguageFromUserId: Get proxy failed.");
        return "";
    }
    std::string language;
    ErrCode errCode = i18nServiceAbilityObj->GetLanguageFromUserId(userId, language);
    if (errCode != ERR_OK) {
        HILOG_ERROR_I18N("I18nServiceAbilityClient::GetLanguageFromUserId: Get language from userID failed, "
            "errCode is %{public}d.", static_cast<int32_t>(errCode));
        return "";
    }
    return language;
}
} // namespace I18n
} // namespace Global
} // namespace OHOS