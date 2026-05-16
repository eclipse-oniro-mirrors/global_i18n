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
#ifndef OHOS_GLOBAL_I18N_SYMBOL_NUMBER_FORMAT_H
#define OHOS_GLOBAL_I18N_SYMBOL_NUMBER_FORMAT_H

#include "number_format.h"

namespace OHOS {
namespace Global {
namespace I18n {
class SymbolNumberFormat : public NumberFormat {
public:
    SymbolNumberFormat(const std::vector<std::string>& localeTags,
        std::map<std::string, std::string>& configs, std::string& errMessage, int32_t& code);
    ~SymbolNumberFormat();

    NumberFormatParts FormatToPartsDouble(double value);
    NumberFormatParts FormatToPartsDecStr(const std::string& value);
    std::string FormatRangeDoubleDouble(double start, double end);
    std::string FormatRangeDoubleDecStr(double start, const std::string& end);
    std::string FormatRangeDecStrDouble(const std::string& start, double end);
    std::string FormatRangeDecStrDecStr(const std::string& start, const std::string& end);
    NumberFormatParts FormatToRangePartsDoubleDouble(double start, double end);
    NumberFormatParts FormatToRangePartsDoubleDecStr(double start, const std::string& end);
    NumberFormatParts FormatToRangePartsDecStrDouble(const std::string& start, double end);
    NumberFormatParts FormatToRangePartsDecStrDecStr(const std::string& start, const std::string& end);

    void ResolvedOptions(std::map<std::string, std::string>& options);
    double Parse(const std::string& text, bool lenientMode, I18nErrorCode& status);

    static const std::string GROUPING_SYMBOL_TAG;
    static const std::string INFINITY_SYMBOL_TAG;
    static const std::string PLUS_SYMBOL_TAG;
    static const std::string MINUS_SYMBOL_TAG;
    static const std::string NAN_SYMBOL_TAG;
    static const std::string ZERO_SYMBOL_TAG;

private:
    void SetNumberSymbols(const std::map<std::string, std::string>& options);
    void ResolvedNumberSymbols(std::map<std::string, std::string>& options);
    icu::Formattable StrToFormattable(const std::string& value);

    std::string pulsSign;
    icu::NumberFormat* numberFormatForParse_ = nullptr;
};
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif
