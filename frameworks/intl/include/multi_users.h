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
#ifndef OHOS_GLOBAL_I18N_MULTI_USERS_H
#define OHOS_GLOBAL_I18N_MULTI_USERS_H

#include <string>
#include "i18n_types.h"
#include "preferences.h"
#include "preferences_helper.h"

namespace OHOS {
namespace Global {
namespace I18n {
class MultiUsers {
public:
    static void InitMultiUser();
    static void SwitchUser(const std::string& curLocalId);
    static void AddUser(const std::string& localId);
    static void RemoveUser(const std::string& localId);
    static I18nErrorCode SaveLanguage(const std::string& localId, const std::string& language);
    static I18nErrorCode SaveLocale(const std::string& localId, const std::string& locale);
    static I18nErrorCode SaveIs24Hour(const std::string& localId, const std::string& is24Hour);
    static std::string GetLanguageFromUserId(int32_t userId);
    static std::string GetSystemLanguageFromUserId(int32_t userId);
    static std::string GetSystemLocaleFromUserId(int32_t userId);

private:
    static I18nErrorCode SaveGlobalParam(const std::string& localId);
    static I18nErrorCode LoadGlobalParam(const std::string& localId);
    static I18nErrorCode GetForegroundLocalId(std::string& localId);
    static I18nErrorCode RemoveGlobalParam(const std::string& localId);
    static std::string ReadMultiUsersParameter(const std::string& paramKey, const std::string& localId);
    static I18nErrorCode WriteMultiUsersParameter(const std::string& paramKey, const std::string& paramValue,
        const std::string& localId, bool isDel);
    static bool IsValidLocalId(const std::string& localId);
    static void InitPreferences();
    static std::string GetParamFromPreferences(const std::string& paramKey);
    static I18nErrorCode SetParamFromPreferences(const std::string& paramKey, const std::string& paramValue);

#ifdef SUPPORT_GRAPHICS
    static void UpdateConfiguration(const std::string& locale, const std::string& language,
        const std::string& is24Hour, int32_t userId);
    static I18nErrorCode PublishCommonEvent(int32_t userId);
#endif

    static const std::string MULTI_USERS_LANGUAGE_KEY;
    static const std::string MULTI_USERS_LOCALE_KEY;
    static const std::string MULTI_USERS_HOUR_KEY;
    static const std::string INIT_KEY;
    static const std::string PREFERENCE_PATH;
    static const int32_t DEFAULT_LOCAL_ID;
    static const int CONFIG_LEN;
    static std::shared_ptr<NativePreferences::Preferences> preferences;
};
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif