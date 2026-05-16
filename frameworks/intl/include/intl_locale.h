/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#ifndef OHOS_GLOBAL_I18N_INTL_LOCALE_H
#define OHOS_GLOBAL_I18N_INTL_LOCALE_H

#include <unordered_map>
#include <string>
#include "unicode/locid.h"
#include "unicode/localebuilder.h"

namespace OHOS {
namespace Global {
namespace I18n {
class IntlLocale {
public:
    IntlLocale(const std::string &localeTag, const std::unordered_map<std::string, std::string>& configs,
        std::string& errMessage);
    virtual ~IntlLocale();

    std::string GetLanguage();
    std::string GetBaseName();
    std::string GetRegion();
    std::string GetScript();
    std::string GetCalendar();
    std::string GetCollation();
    std::string GetHourCycle();
    std::string GetNumberingSystem();
    std::string GetNumeric();
    std::string GetCaseFirst();
    std::string Maximize();
    std::string Minimize();
    std::string ToString(std::string& errMessage);

    static const std::string languageTag;
    static const std::string baseNameTag;
    static const std::string regionTag;
    static const std::string scriptTag;
    static const std::string calendarTag;
    static const std::string collationTag;
    static const std::string hourCycleTag;
    static const std::string numberingSystemTag;
    static const std::string numericTag;
    static const std::string caseFirstTag;

private:
    bool CheckLocaleParam(const std::string& localeTag, const std::unordered_map<std::string, std::string>& configs);
    bool SetLocaleParam(const std::string& localeTag, const std::unordered_map<std::string, std::string>& configs,
        icu::LocaleBuilder* builder);
    bool CheckConfigsExtParam(const std::unordered_map<std::string, std::string>& configs);
    bool SetConfigsExtParam(const std::unordered_map<std::string, std::string>& configs, icu::LocaleBuilder* builder);
    bool SetExtParam(const std::unordered_map<std::string, std::string>& configs, const std::string paramTag,
        const std::string paramExtTag, icu::LocaleBuilder* builder);

    bool initSuccess = false;
    icu::Locale icuLocale;

    static bool icuInitialized;
    static bool Init();
};
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif