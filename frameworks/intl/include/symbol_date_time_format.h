/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
#ifndef OHOS_GLOBAL_I18N_SYMBOL_DATE_TIME_FORMAT_H
#define OHOS_GLOBAL_I18N_SYMBOL_DATE_TIME_FORMAT_H

#include "intl_date_time_format.h"
#include "i18n_types.h"

namespace OHOS {
namespace Global {
namespace I18n {
class SymbolDateTimeFormat : public IntlDateTimeFormat {
public:
    SymbolDateTimeFormat(const std::vector<std::string>& localeTags,
        const std::unordered_map<std::string, std::string>& configs, std::string& errMessage);
    ~SymbolDateTimeFormat();
    void ResolvedOptions(std::unordered_map<std::string, std::string>& options) override;
    double Parse(const std::string& text, bool lenientMode, I18nErrorCode& status);

    static const std::string AM_SYMBOL_TAG;
    static const std::string PM_SYMBOL_TAG;
    static constexpr int32_t AM_PM_SYMBOLS_LEN = 2;

private:
    void SetAmPmSymbols(std::unique_ptr<icu::DateFormatSymbols>& dateFormatSymbols,
        const std::unordered_map<std::string, std::string>& options);
    std::vector<std::string> GetAmPmSymbols();
    void ResolvedAmPmSymbols(std::unordered_map<std::string, std::string>& options);
};
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif