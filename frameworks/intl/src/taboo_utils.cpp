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
#include "taboo_utils.h"

namespace OHOS {
namespace Global {
namespace I18n {
std::string TabooUtils::SYSTEM_TABOO_DATA_PATH = "/system/etc/taboo_res/";

TabooUtils* TabooUtils::GetInstance()
{
    static TabooUtils tabooUtils;
    return &tabooUtils;
}

TabooUtils::TabooUtils()
{
    systemTaboo = std::make_shared<Taboo>(SYSTEM_TABOO_DATA_PATH);
}

std::string TabooUtils::ReplaceCountryName(const std::string& region, const std::string& displayLanguage,
    const std::string& name)
{
    if (systemTaboo == nullptr) {
        HILOG_ERROR_I18N("TabooUtils::ReplaceCountryName: systemTaboo is nullptr.");
        return name;
    }
    return systemTaboo->ReplaceCountryName(region, displayLanguage, name);
}

std::string TabooUtils::ReplaceLanguageName(const std::string& language, const std::string& displayLanguage,
    const std::string& name)
{
    if (systemTaboo == nullptr) {
        HILOG_ERROR_I18N("TabooUtils::ReplaceLanguageName: systemTaboo is nullptr.");
        return name;
    }
    return systemTaboo->ReplaceLanguageName(language, displayLanguage, name);
}

std::string TabooUtils::ReplaceTimeZoneName(const std::string& tzId, const std::string& displayLanguage,
    const std::string& name)
{
    if (systemTaboo == nullptr) {
        HILOG_ERROR_I18N("TabooUtils::ReplaceTimeZoneName: systemTaboo is nullptr.");
        return name;
    }
    return systemTaboo->ReplaceTimeZoneName(tzId, displayLanguage, name);
}

std::string TabooUtils::ReplaceCityName(const std::string& cityId, const std::string& displayLanguage,
    const std::string& name)
{
    if (systemTaboo == nullptr) {
        HILOG_ERROR_I18N("TabooUtils::ReplaceCityName: systemTaboo is nullptr.");
        return name;
    }
    return systemTaboo->ReplaceCityName(cityId, displayLanguage, name);
}

std::string TabooUtils::ReplacePhoneLocationName(const std::string& phoneNumber, const std::string& displayLanguage,
    const std::string& name)
{
    if (systemTaboo == nullptr) {
        HILOG_ERROR_I18N("TabooUtils::ReplacePhoneLocationName: systemTaboo is nullptr.");
        return name;
    }
    return systemTaboo->ReplacePhoneLocationName(phoneNumber, displayLanguage, name);
}

std::unordered_set<std::string> TabooUtils::GetBlockedLanguages() const
{
    if (systemTaboo == nullptr) {
        HILOG_ERROR_I18N("TabooUtils::GetBlockedLanguages: systemTaboo is nullptr.");
        return {};
    }
    return systemTaboo->GetBlockedLanguages();
}

std::unordered_set<std::string> TabooUtils::GetBlockedRegions(const std::string& language) const
{
    if (systemTaboo == nullptr) {
        HILOG_ERROR_I18N("TabooUtils::GetBlockedRegions: systemTaboo is nullptr.");
        return {};
    }
    return systemTaboo->GetBlockedRegions(language);
}

std::unordered_set<std::string> TabooUtils::GetBlockedCities() const
{
    if (systemTaboo == nullptr) {
        HILOG_ERROR_I18N("TabooUtils::GetBlockedCities: systemTaboo is nullptr.");
        return {};
    }
    return systemTaboo->GetBlockedCities();
}

std::unordered_set<std::string> TabooUtils::GetBlockedPhoneNumbers() const
{
    if (systemTaboo == nullptr) {
        HILOG_ERROR_I18N("TabooUtils::GetBlockedPhoneNumbers: systemTaboo is nullptr.");
        return {};
    }
    return systemTaboo->GetBlockedPhoneNumbers();
}
} // namespace I18n
} // namespace Global
} // namespace OHOS