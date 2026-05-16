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

#include "symbol_number_format.h"

#include "i18n_hilog.h"
#include "unicode/decimfmt.h"
#include "unicode/numfmt.h"

namespace OHOS {
namespace Global {
namespace I18n {
const std::string SymbolNumberFormat::GROUPING_SYMBOL_TAG = "groupingSeparator";
const std::string SymbolNumberFormat::INFINITY_SYMBOL_TAG = "infinity";
const std::string SymbolNumberFormat::PLUS_SYMBOL_TAG = "plusSign";
const std::string SymbolNumberFormat::MINUS_SYMBOL_TAG = "minusSign";
const std::string SymbolNumberFormat::NAN_SYMBOL_TAG = "nan";
const std::string SymbolNumberFormat::ZERO_SYMBOL_TAG = "zero";

static const std::unordered_map<std::string, icu::DecimalFormatSymbols::ENumberFormatSymbol> TAG_TO_ICU_SYMBOL = {
    { SymbolNumberFormat::GROUPING_SYMBOL_TAG,
        icu::DecimalFormatSymbols::ENumberFormatSymbol::kGroupingSeparatorSymbol },
    { SymbolNumberFormat::INFINITY_SYMBOL_TAG, icu::DecimalFormatSymbols::ENumberFormatSymbol::kInfinitySymbol },
    { SymbolNumberFormat::PLUS_SYMBOL_TAG, icu::DecimalFormatSymbols::ENumberFormatSymbol::kPlusSignSymbol },
    { SymbolNumberFormat::MINUS_SYMBOL_TAG, icu::DecimalFormatSymbols::ENumberFormatSymbol::kMinusSignSymbol },
    { SymbolNumberFormat::NAN_SYMBOL_TAG, icu::DecimalFormatSymbols::ENumberFormatSymbol::kNaNSymbol },
    { SymbolNumberFormat::ZERO_SYMBOL_TAG, icu::DecimalFormatSymbols::ENumberFormatSymbol::kZeroDigitSymbol }
};

SymbolNumberFormat::SymbolNumberFormat(const std::vector<std::string>& localeTags,
    std::map<std::string, std::string>& options, std::string& errMessage, int32_t& code)
    : NumberFormat(localeTags, options, errMessage, code)
{
    if (!errMessage.empty() || code != 0) {
        return;
    }
    SetNumberSymbols(options);
    styleRangeFormatter = styleFormatter;
    numberFormat = styleFormatter.locale(locale);
    std::map<std::string, std::string> symbols;
    ResolvedNumberSymbols(symbols);
    auto iter = symbols.find(PLUS_SYMBOL_TAG);
    pulsSign = (iter == symbols.end()) ? "+" : iter->second;
    UErrorCode icuStatus = U_ZERO_ERROR;
    numberFormatForParse_ = icu::NumberFormat::createInstance(locale, UNumberFormatStyle::UNUM_DECIMAL, icuStatus);
    if (U_FAILURE(icuStatus) || numberFormatForParse_ == nullptr) {
        HILOG_ERROR_I18N("SymbolNumberFormat: Create numberFormatForParse_, code is %{public}s.",
            u_errorName(icuStatus));
    }
}

SymbolNumberFormat::~SymbolNumberFormat()
{
    if (numberFormatForParse_ != nullptr) {
        delete numberFormatForParse_;
        numberFormatForParse_ = nullptr;
    }
}

NumberFormatParts SymbolNumberFormat::FormatToPartsDouble(double value)
{
    icu::number::FormattedNumber formatted = FormatDoubleToFormattedNumber(value);
    return ParseNumberFormatPart(formatted, pulsSign, false);
}

NumberFormatParts SymbolNumberFormat::FormatToPartsDecStr(const std::string& value)
{
    icu::number::FormattedNumber formatted = FormatDecimalToFormattedNumber(value);
    return ParseNumberFormatPart(formatted, pulsSign, false);
}

std::string SymbolNumberFormat::FormatRangeDoubleDouble(double start, double end)
{
    return FormatFormattableRange(icu::Formattable(start), icu::Formattable(end));
}
        
std::string SymbolNumberFormat::FormatRangeDoubleDecStr(double start, const std::string& end)
{
    return FormatFormattableRange(icu::Formattable(start), StrToFormattable(end));
}

std::string SymbolNumberFormat::FormatRangeDecStrDouble(const std::string& start, double end)
{
    return FormatFormattableRange(StrToFormattable(start), icu::Formattable(end));
}

std::string SymbolNumberFormat::FormatRangeDecStrDecStr(const std::string& start, const std::string& end)
{
    return FormatFormattableRange(StrToFormattable(start), StrToFormattable(end));
}

NumberFormatParts SymbolNumberFormat::FormatToRangePartsDoubleDouble(double start, double end)
{
    return FormatFormattableRangeToParts(icu::Formattable(start), icu::Formattable(end), pulsSign);
}

NumberFormatParts SymbolNumberFormat::FormatToRangePartsDoubleDecStr(double start, const std::string& end)
{
    return FormatFormattableRangeToParts(icu::Formattable(start), StrToFormattable(end), pulsSign);
}

NumberFormatParts SymbolNumberFormat::FormatToRangePartsDecStrDouble(const std::string& start, double end)
{
    return FormatFormattableRangeToParts(StrToFormattable(start), icu::Formattable(end), pulsSign);
}

NumberFormatParts SymbolNumberFormat::FormatToRangePartsDecStrDecStr(const std::string& start, const std::string& end)
{
    return FormatFormattableRangeToParts(StrToFormattable(start), StrToFormattable(end), pulsSign);
}

void SymbolNumberFormat::ResolvedOptions(std::map<std::string, std::string>& options)
{
    NumberFormat::GetResolvedOptions(options, true);
    ResolvedNumberSymbols(options);
}

void SymbolNumberFormat::SetNumberSymbols(const std::map<std::string, std::string>& options)
{
    if (numberFormatSymbol == nullptr) {
        UErrorCode status = U_ZERO_ERROR;
        numberFormatSymbol = new icu::DecimalFormatSymbols(locale, status);
        if (U_FAILURE(status) || numberFormatSymbol == nullptr) {
            HILOG_ERROR_I18N("SymbolNumberFormat::SetNumberSymbols: Create icu::DecimalFormatSymbols failed.");
            return;
        }
    }
    
    for (const auto& option : options) {
        auto iter = TAG_TO_ICU_SYMBOL.find(option.first);
        if (iter == TAG_TO_ICU_SYMBOL.end()) {
            continue;
        }
        numberFormatSymbol->setSymbol(iter->second, option.second.c_str());
    }
    styleFormatter = styleFormatter.symbols(*numberFormatSymbol);
}

void SymbolNumberFormat::ResolvedNumberSymbols(std::map<std::string, std::string>& options)
{
    if (numberFormatSymbol == nullptr) {
        return;
    }
    for (const auto& item : TAG_TO_ICU_SYMBOL) {
        icu::UnicodeString symbol = numberFormatSymbol->getSymbol(item.second);
        std::string symbolValue;
        symbol.toUTF8String(symbolValue);
        options.insert(std::make_pair(item.first, symbolValue));
    }
}

icu::Formattable SymbolNumberFormat::StrToFormattable(const std::string& value)
{
    UErrorCode status = U_ZERO_ERROR;
    const icu::StringPiece sp {value.data(), static_cast<int32_t>(value.size())};
    icu::Formattable result(sp, status);
    if (U_FAILURE(status)) {
        HILOG_ERROR_I18N("SymbolNumberFormat::StrToFormattable: Create icu::Formattable failed.");
        return icu::Formattable();
    }
    return result;
}

double SymbolNumberFormat::Parse(const std::string& text, bool lenientMode, I18nErrorCode& status)
{
    if (numberFormatForParse_ == nullptr) {
        HILOG_ERROR_I18N("SymbolNumberFormat::Parse: numberFormatForParse_ is nullptr.");
        return 0;
    }
    if (text.empty()) {
        HILOG_ERROR_I18N("SymbolNumberFormat::Parse: text is empty.");
        status = I18nErrorCode::INVALID_PARAM;
        return 0;
    }
    numberFormatForParse_->setLenient(lenientMode);
    icu::UnicodeString unicodeText = icu::UnicodeString::fromUTF8(text);
    icu::ParsePosition parsePos(0);
    icu::Formattable formattable;
    numberFormatForParse_->parse(unicodeText, formattable, parsePos);
    if (parsePos.getErrorIndex() != -1) {
        HILOG_ERROR_I18N("SymbolNumberFormat::Parse: Parse failed for %{public}s, error index: %{public}d.",
            text.c_str(), parsePos.getErrorIndex());
        status = I18nErrorCode::INVALID_PARAM;
        return 0;
    }
    if (!formattable.isNumeric()) {
        HILOG_ERROR_I18N("SymbolNumberFormat::Parse: formattable is not numeric.");
        return 0;
    }
    auto type = formattable.getType();
    double result = 0;
    switch (type) {
        case icu::Formattable::Type::kLong:
            result = static_cast<double>(formattable.getLong());
            break;
        case icu::Formattable::Type::kInt64 :
            result = static_cast<double>(formattable.getInt64());
            break;
        case icu::Formattable::Type::kDouble:
            result = static_cast<double>(formattable.getDouble());
            break;
        default:
            HILOG_ERROR_I18N("SymbolNumberFormat::Parse: Get data failed.");
    }
    status = I18nErrorCode::SUCCESS;
    return result;
}
} // namespace I18n
} // namespace Global
} // namespace OHOS
