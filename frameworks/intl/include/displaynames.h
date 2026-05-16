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
#ifndef GLOBAL_I18N_DISPLAYNAMES_H
#define GLOBAL_I18N_DISPLAYNAMES_H

#include <memory>
#include <unordered_set>
#include "unicode/locdspnm.h"
#include "unicode/locid.h"
#include "unicode/dtptngen.h"
#include "unicode/udisplaycontext.h"
#include "i18n_types.h"
#include "locale_info.h"

namespace OHOS {
namespace Global {
namespace I18n {
class DisplayNames {
public:
    static std::vector<std::string> SupportedLocalesOf(const std::vector<std::string> &requestLocales,
                                                       const std::map<std::string, std::string> &configs,
                                                       I18nErrorCode &status);

    DisplayNames(const std::vector<std::string> &localeTags, const std::map<std::string, std::string> &options);
    std::string Display(const std::string &code);
    std::map<std::string, std::string> ResolvedOptions();
    I18nErrorCode GetError() const;
    std::string GetErrorMessage() const;

private:
    static std::set<std::string> LookupSupportedLocales(const std::vector<std::string> &availableLocales,
                                                        const std::vector<std::string> &requestLocales);

    void ParseAllOptions(const std::map<std::string, std::string> &options);
    void InitDisplayNames(const std::string &curLocale, const std::map<std::string, std::string> &options);
    std::string DisplayLanguage(const std::string &code);
    std::string DisplayRegion(const std::string &code);
    std::string DisplayScript(const std::string &code);
    std::string DisplayCurrency(const std::string &code);
    std::string DisplayCalendar(const std::string &code);
    std::string DisplayDatetimefield(const std::string &code);

    static const int32_t MIN_CALENDAR_LENGTH;
    static const int32_t MAX_CALENDAR_LENGTH;
    static const int32_t CURRENCY_LENGTH;
    static const std::unordered_map<std::string, UDisplayContext> ICU_STYLE_OPTIONS;
    static const std::unordered_map<std::string, UDisplayContext> ICU_LANGUAGE_DISPLAY_OPTIONS;
    static const std::unordered_map<std::string, UDateTimePatternField> ICU_DATE_PATTERN_FIELD;
    static const std::unordered_map<std::string, UDateTimePGDisplayWidth> ICU_DATE_DISPLAY_WIDTH;

    std::unique_ptr<icu::LocaleDisplayNames> icuLocaldisplaynames;
    std::string localeStr;
    std::string localeMatcher;
    std::string style;
    std::string fallback;
    std::string type;
    std::string languageDisplay;
    icu::Locale locale;
    std::unique_ptr<LocaleInfo> localeInfo = nullptr;
    std::unique_ptr<icu::LocaleDisplayNames> icuDisplaynames = nullptr;
    I18nErrorCode status = I18nErrorCode::SUCCESS;
    std::string errorMessage = "";
    bool createSuccess = false;
};
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif