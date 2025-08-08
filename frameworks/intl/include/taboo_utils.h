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

#ifndef OHOS_GLOBAL_I18N_TABOO_UTILS_H
#define OHOS_GLOBAL_I18N_TABOO_UTILS_H

#include "taboo.h"

namespace OHOS {
namespace Global {
namespace I18n {
class TabooUtils {
public:
    static TabooUtils* GetInstance();
    std::string ReplaceCountryName(const std::string& region, const std::string& displayLanguage,
        const std::string& name);
    std::string ReplaceLanguageName(const std::string& language, const std::string& displayLanguage,
        const std::string& name);
    std::string ReplaceTimeZoneName(const std::string& tzId, const std::string& displayLanguage,
        const std::string& name);
    std::string ReplaceCityName(const std::string& cityId, const std::string& displayLanguage,
        const std::string& name);
    std::string ReplacePhoneLocationName(const std::string& phoneNumber, const std::string& displayLanguage,
        const std::string& name);
    std::unordered_set<std::string> GetBlockedLanguages() const;
    std::unordered_set<std::string> GetBlockedRegions(const std::string& language) const;
    std::unordered_set<std::string> GetBlockedCities() const;
    std::unordered_set<std::string> GetBlockedPhoneNumbers() const;

private:
    TabooUtils();
    TabooUtils(const TabooUtils&) = delete;
    TabooUtils& operator=(const TabooUtils&) = delete;
    std::shared_ptr<Taboo> systemTaboo;
    static std::string SYSTEM_TABOO_DATA_PATH;
};
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif