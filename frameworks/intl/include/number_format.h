/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#ifndef OHOS_GLOBAL_I18N_NUMBER_FORMAT_H
#define OHOS_GLOBAL_I18N_NUMBER_FORMAT_H

#include <map>
#include <mutex>
#include <set>
#include <unordered_map>
#include <vector>
#include "unicode/numberformatter.h"
#include "unicode/locid.h"
#include "unicode/numfmt.h"
#include "unicode/unum.h"
#include "unicode/decimfmt.h"
#include "unicode/localebuilder.h"
#include "unicode/numsys.h"
#include "unicode/measfmt.h"
#include "unicode/measunit.h"
#include "unicode/measure.h"
#include "unicode/currunit.h"
#include "unicode/fmtable.h"
#include "unicode/ures.h"
#include "unicode/ulocdata.h"
#include "number_utils.h"
#include "number_utypes.h"
#include "locale_info.h"
#include "measure_data.h"
#include "relative_time_format.h"
#include "unicode/numberrangeformatter.h"

namespace OHOS {
namespace Global {
namespace I18n {
class NumberFormat {
public:
    NumberFormat(const std::vector<std::string> &localeTag, std::map<std::string, std::string> &configs);
    virtual ~NumberFormat();
    std::string Format(double number);
    bool IsRelativeTimeFormat(double number);
    icu::number::FormattedNumber FormatToFormattedNumber(double number);
    icu::FormattedRelativeDateTime FormatToFormattedRelativeDateTime(double number);
    std::string FormatRange(double start, double end);
    void GetResolvedOptions(std::map<std::string, std::string> &map);
    std::string GetCurrency() const;
    std::string GetCurrencySign() const;
    std::string GetStyle() const;
    std::string GetNumberingSystem() const;
    std::string GetUseGrouping() const;
    std::string GetMinimumIntegerDigits() const;
    std::string GetMinimumFractionDigits() const;
    std::string GetMaximumFractionDigits() const;
    std::string GetMinimumSignificantDigits() const;
    std::string GetMaximumSignificantDigits() const;
    std::string GetLocaleMatcher() const;

private:
    icu::Locale locale;
    std::string currency;
    std::string currencySign = "standard";
    std::string currencyDisplayString;
    std::string unit;
    std::string unitDisplayString;
    std::string styleString;
    std::string numberingSystem;
    std::string useGrouping;
    std::string notationString;
    std::string signDisplayString = "auto";
    std::string compactDisplay;
    std::string minimumIntegerDigits;
    std::string minimumFractionDigits;
    std::string maximumFractionDigits;
    std::string minimumSignificantDigits;
    std::string maximumSignificantDigits;
    std::string localeBaseName;
    std::string localeMatcher;
    std::string unitUsage;
    std::string unitType;
    std::string unitMeasSys;
    std::string roundingPriorityStr;
    std::string roundingModeStr;
    size_t roundingIncrement = 1;
    bool createSuccess = false;
    bool isSetFraction = false;
    std::unique_ptr<LocaleInfo> localeInfo = nullptr;
    std::unique_ptr<RelativeTimeFormat> relativeTimeFormat = nullptr;
    icu::number::LocalizedNumberFormatter numberFormat;
    icu::number::LocalizedNumberRangeFormatter numberRangeFormat;
    icu::number::Notation notation = icu::number::Notation::simple();
    UNumberRoundingPriority roundingPriority = UNumberRoundingPriority::UNUM_ROUNDING_PRIORITY_STRICT;
    UNumberFormatRoundingMode roundingMode = UNumberFormatRoundingMode::UNUM_ROUND_HALFUP;
    UNumberUnitWidth unitDisplay = UNumberUnitWidth::UNUM_UNIT_WIDTH_SHORT;
    UNumberUnitWidth currencyDisplay = UNumberUnitWidth::UNUM_UNIT_WIDTH_SHORT;
    UNumberSignDisplay signDisplay = UNumberSignDisplay::UNUM_SIGN_AUTO;
    static const int MAX_UNIT_NUM = 500;
    static const int DEFAULT_FRACTION_DIGITS = 3;
    static const int DEFAULT_MAX_SIGNIFICANT_DIGITS = 21;
    static const char *DEVICE_TYPE_NAME;
    static const int BUFFER_LEN = 10;
    static const int POW_BASE_NUMBER = 10;
    static const int CURRENCY_LEN = 3;
    static std::mutex numToCurrencyMutex;
    static bool initISO4217;
    static std::unordered_map<std::string, std::string> numToCurrency;
    icu::MeasureUnit unitArray[MAX_UNIT_NUM];
    static bool icuInitialized;
    static bool Init();
    static std::unordered_map<std::string, UNumberUnitWidth> unitStyle;
    static std::unordered_map<std::string, UNumberUnitWidth> currencyStyle;
    static std::unordered_map<std::string, UNumberSignDisplay> signAutoStyle;
    static std::unordered_map<std::string, UNumberSignDisplay> signAccountingStyle;
    static std::unordered_map<UMeasurementSystem, std::string> measurementSystem;
    static std::unordered_map<std::string, UNumberUnitWidth> defaultUnitStyle;
    static std::unordered_map<std::string, UNumberUnitWidth> defaultCurrencyStyle;
    static std::unordered_map<std::string, UNumberRoundingPriority> roundingPriorityStyle;
    static std::unordered_map<std::string, UNumberFormatRoundingMode> roundingModeStyle;
    static std::vector<size_t> roundingIncrementList;
    static std::map<std::string, std::string> RelativeTimeFormatConfigs;
    static bool ReadISO4217Datas();
    std::string GetCurrencyFromConfig(const std::string& currency);
    void CreateRelativeTimeFormat(const std::string& locale);
    void ParseConfigs(std::map<std::string, std::string> &configs);
    void ParseDigitsConfigs(std::map<std::string, std::string> &configs);
    void ParseRoundingConfigs(std::map<std::string, std::string> &configs);
    void GetDigitsResolvedOptions(std::map<std::string, std::string> &map);
    void InitProperties(icu::number::UnlocalizedNumberFormatter &formatter);
    void InitUnitProperties(icu::number::UnlocalizedNumberFormatter &formatter);
    void InitCurrencyProperties(icu::number::UnlocalizedNumberFormatter &formatter);
    void InitPercentStyleProperties(icu::number::UnlocalizedNumberFormatter &formatter);
    void InitUseGroupingProperties(icu::number::UnlocalizedNumberFormatter &formatter);
    void InitNotationProperties(icu::number::UnlocalizedNumberFormatter &formatter);
    void InitSignProperties(icu::number::UnlocalizedNumberFormatter &formatter);
    void InitDigitsProperties(icu::number::UnlocalizedNumberFormatter &formatter);
    void InitNumberFormat();
    void SetUnit(std::string &preferredUnit);
    void SetDefaultStyle();
    void SetPrecisionWithByte(double number, const std::string& finalUnit);
    void HandleRoundingPriority(icu::number::UnlocalizedNumberFormatter &formatter);
    void SetRoundingProperties(icu::number::UnlocalizedNumberFormatter &formatter);
    int32_t GetMinimumSignificantDigitsIntValue();
    int32_t GetMaximumSignificantDigitsIntValue();
    int32_t GetMaximumFractionDigitsValue();
    int32_t GetMinimumFractionDigitsValue();
};
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif