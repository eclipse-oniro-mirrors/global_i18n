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

#include "symbol_date_time_format.h"

#include "i18n_hilog.h"
#include "locale_config.h"
#include "unicode/dtfmtsym.h"
#include "unicode/parsepos.h"

namespace OHOS {
namespace Global {
namespace I18n {
const std::string SymbolDateTimeFormat::AM_SYMBOL_TAG = "am";
const std::string SymbolDateTimeFormat::PM_SYMBOL_TAG = "pm";

SymbolDateTimeFormat::SymbolDateTimeFormat(const std::vector<std::string>& localeTags,
    const std::unordered_map<std::string, std::string>& options, std::string& errMessage)
    : IntlDateTimeFormat(localeTags, options, errMessage)
{
    if (!errMessage.empty()) {
        return;
    }
    if (icuSimpleDateFormat == nullptr) {
        HILOG_ERROR_I18N("SymbolDateTimeFormat: icuSimpleDateFormat is nullptr.");
        return;
    }
    UErrorCode status = U_ZERO_ERROR;
    auto dateFormatSymbols = std::make_unique<icu::DateFormatSymbols>(icuLocale, status);
    if (U_FAILURE(status)) {
        HILOG_ERROR_I18N("SymbolDateTimeFormat: Create icu::DateFormatSymbols failed, code is %{public}s.",
            u_errorName(status));
        return;
    }
    SetAmPmSymbols(dateFormatSymbols, options);
    icuSimpleDateFormat->setDateFormatSymbols(*dateFormatSymbols);
}

SymbolDateTimeFormat::~SymbolDateTimeFormat() {}

void SymbolDateTimeFormat::ResolvedOptions(std::unordered_map<std::string, std::string>& options)
{
    IntlDateTimeFormat::ResolvedOptions(options);
    ResolvedAmPmSymbols(options);
}

void SymbolDateTimeFormat::SetAmPmSymbols(std::unique_ptr<icu::DateFormatSymbols>& dateFormatSymbols,
    const std::unordered_map<std::string, std::string>& options)
{
    auto amIter = options.find(AM_SYMBOL_TAG);
    auto pmIter = options.find(PM_SYMBOL_TAG);
    if (amIter == options.end() || pmIter == options.end()) {
        return;
    }

    icu::UnicodeString amSymbol = amIter->second.c_str();
    icu::UnicodeString pmSymbol = pmIter->second.c_str();
    const icu::UnicodeString amPmSymbols[AM_PM_SYMBOLS_LEN] = { amSymbol, pmSymbol };
    dateFormatSymbols->setAmPmStrings(amPmSymbols, AM_PM_SYMBOLS_LEN);
}

std::vector<std::string> SymbolDateTimeFormat::GetAmPmSymbols()
{
    auto symbols = icuSimpleDateFormat->getDateFormatSymbols();
    if (symbols == nullptr) {
        HILOG_ERROR_I18N("SymbolDateTimeFormat::GetAmPmSymbols: getDateFormatSymbols failed.");
        return {};
    }
    int32_t count = 0;
    const icu::UnicodeString* ampmSymbols = symbols->getAmPmStrings(count);
    if (ampmSymbols == nullptr || count != AM_PM_SYMBOLS_LEN) {
        HILOG_ERROR_I18N("SymbolDateTimeFormat::GetAmPmSymbols: getAmPmStrings failed.");
        return {};
    }
    std::string amSymbol;
    ampmSymbols[0].toUTF8String(amSymbol);
    std::string pmSymbol;
    ampmSymbols[1].toUTF8String(pmSymbol);
    return { amSymbol, pmSymbol };
}

void SymbolDateTimeFormat::ResolvedAmPmSymbols(std::unordered_map<std::string, std::string>& options)
{
    std::vector<std::string> amPmSymbols = GetAmPmSymbols();
    if (amPmSymbols.size() == 0) {
        HILOG_ERROR_I18N("SymbolDateTimeFormat::ResolvedAmPmSymbols: GetAmPmSymbols failed.");
        return;
    }
    options.insert(std::make_pair(AM_SYMBOL_TAG, amPmSymbols[0]));
    options.insert(std::make_pair(PM_SYMBOL_TAG, amPmSymbols[1]));
}

double SymbolDateTimeFormat::Parse(const std::string& text, bool lenientMode, I18nErrorCode& status)
{
    // System error - not caused by input, return FAILED
    if (icuSimpleDateFormat == nullptr) {
        HILOG_ERROR_I18N("SymbolDateTimeFormat::Parse: icuSimpleDateFormat is nullptr.");
        status = I18nErrorCode::FAILED;
        return 0;
    }
    // Input error - caused by bad input, return INVALID_PARAM
    if (text.empty()) {
        HILOG_ERROR_I18N("SymbolDateTimeFormat::Parse: text is empty.");
        status = I18nErrorCode::INVALID_PARAM;
        return 0;
    }
    icuSimpleDateFormat->setLenient(lenientMode);
    icu::UnicodeString unicodeText = icu::UnicodeString::fromUTF8(text);
    icu::ParsePosition parsePos(0);
    UDate date = icuSimpleDateFormat->parse(unicodeText, parsePos);
    // Input error - parse failure caused by invalid input
    if (parsePos.getErrorIndex() != -1) {
        HILOG_ERROR_I18N("SymbolDateTimeFormat::Parse: Parse failed for %{public}s, error index: %{public}d.",
            text.c_str(), parsePos.getErrorIndex());
        status = I18nErrorCode::INVALID_PARAM;
        return 0;
    }
    status = I18nErrorCode::SUCCESS;
    return date;
}
} // namespace I18n
} // namespace Global
} // namespace OHOS
