/*
 * Copyright (c) 2021-2025 Huawei Device Co., Ltd.
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
#include "locale_config.h"

#include <cctype>
#include <cmath>
#include <new>
#include <sstream>
#include <tuple>
#include "format_utils.h"
#include "i18n_hilog.h"
#include "locale_helper.h"
#include "ohos/init_data.h"
#include "parameter.h"
#include "unicode/ucurr.h"
#include "unicode/uenum.h"
#include "unicode/utypes.h"
#include "unicode/unumberformatter.h"
#include "unicode/stringpiece.h"
#include "utils.h"
#include "number_format.h"
#include "unicode/numberrangeformatter.h"

namespace OHOS {
namespace Global {
namespace I18n {
const char* NumberFormat::DEVICE_TYPE_NAME = "const.product.devicetype";
std::mutex NumberFormat::numToCurrencyMutex;
bool NumberFormat::initISO4217 = false;
std::unordered_map<std::string, std::string> NumberFormat::numToCurrency = {};
bool NumberFormat::icuInitialized = NumberFormat::Init();
static const double DEFAULT_SAMPLE_NUMBER = 12345.67;

std::unordered_map<std::string, UNumberUnitWidth> NumberFormat::unitStyle = {
    { "long", UNumberUnitWidth::UNUM_UNIT_WIDTH_FULL_NAME },
    { "short", UNumberUnitWidth::UNUM_UNIT_WIDTH_SHORT },
    { "narrow", UNumberUnitWidth::UNUM_UNIT_WIDTH_NARROW }
};

std::unordered_map<std::string, UNumberUnitWidth> NumberFormat::currencyStyle = {
    { "symbol", UNumberUnitWidth::UNUM_UNIT_WIDTH_SHORT },
    { "code", UNumberUnitWidth::UNUM_UNIT_WIDTH_ISO_CODE },
    { "name", UNumberUnitWidth::UNUM_UNIT_WIDTH_FULL_NAME },
    { "narrowSymbol", UNumberUnitWidth::UNUM_UNIT_WIDTH_NARROW }
};

std::unordered_map<std::string, UNumberSignDisplay> NumberFormat::signAutoStyle = {
    { "auto", UNumberSignDisplay::UNUM_SIGN_AUTO },
    { "never", UNumberSignDisplay::UNUM_SIGN_NEVER },
    { "always", UNumberSignDisplay::UNUM_SIGN_ALWAYS },
    { "exceptZero", UNumberSignDisplay::UNUM_SIGN_EXCEPT_ZERO }
};

std::unordered_map<std::string, UNumberSignDisplay> NumberFormat::signAccountingStyle = {
    { "auto", UNumberSignDisplay::UNUM_SIGN_ACCOUNTING },
    { "never", UNumberSignDisplay::UNUM_SIGN_NEVER },
    { "always", UNumberSignDisplay::UNUM_SIGN_ACCOUNTING_ALWAYS },
    { "exceptZero", UNumberSignDisplay::UNUM_SIGN_ACCOUNTING_EXCEPT_ZERO }
};

std::unordered_map<std::string, std::string> NumberFormat::measurementSystem = {
    { "metric", "SI" },
    { "ussystem", "US" },
    { "uksystem", "UK" },
};

std::unordered_map<std::string, UNumberUnitWidth> NumberFormat::defaultUnitStyle = {
    { "tablet", UNumberUnitWidth::UNUM_UNIT_WIDTH_FULL_NAME },
    { "2in1", UNumberUnitWidth::UNUM_UNIT_WIDTH_FULL_NAME },
    { "tv", UNumberUnitWidth::UNUM_UNIT_WIDTH_FULL_NAME },
    { "pc", UNumberUnitWidth::UNUM_UNIT_WIDTH_FULL_NAME },
    { "wearable", UNumberUnitWidth::UNUM_UNIT_WIDTH_NARROW },
    { "liteWearable", UNumberUnitWidth::UNUM_UNIT_WIDTH_NARROW },
    { "watch", UNumberUnitWidth::UNUM_UNIT_WIDTH_NARROW }
};

std::unordered_map<std::string, UNumberUnitWidth> NumberFormat::defaultCurrencyStyle = {
    { "wearable", UNumberUnitWidth::UNUM_UNIT_WIDTH_NARROW },
    { "liteWearable", UNumberUnitWidth::UNUM_UNIT_WIDTH_NARROW },
    { "watch", UNumberUnitWidth::UNUM_UNIT_WIDTH_NARROW }
};

std::unordered_map<std::string, UNumberRoundingPriority> NumberFormat::roundingPriorityStyle = {
    { "auto", UNumberRoundingPriority::UNUM_ROUNDING_PRIORITY_STRICT },
    { "morePrecision", UNumberRoundingPriority::UNUM_ROUNDING_PRIORITY_RELAXED },
    { "lessPrecision", UNumberRoundingPriority::UNUM_ROUNDING_PRIORITY_STRICT }
};

std::unordered_map<std::string, UNumberFormatRoundingMode> NumberFormat::roundingModeStyle = {
    { "ceil", UNumberFormatRoundingMode::UNUM_ROUND_CEILING },
    { "floor", UNumberFormatRoundingMode::UNUM_ROUND_FLOOR },
    { "expand", UNumberFormatRoundingMode::UNUM_ROUND_UP },
    { "trunc", UNumberFormatRoundingMode::UNUM_ROUND_DOWN },
    { "halfCeil", UNumberFormatRoundingMode::UNUM_ROUND_HALF_CEILING },
    { "halfFloor", UNumberFormatRoundingMode::UNUM_ROUND_HALF_FLOOR },
    { "halfExpand", UNumberFormatRoundingMode::UNUM_ROUND_HALFUP },
    { "halfTrunc", UNumberFormatRoundingMode::UNUM_ROUND_HALFDOWN },
    { "halfEven", UNumberFormatRoundingMode::UNUM_ROUND_HALFEVEN },
};

std::vector<size_t> NumberFormat::roundingIncrementList = {
    1, 2, 5, 10, 20, 25, 50, 100, 200, 250, 500, 1000, 2000, 2500, 5000
};

std::map<std::string, std::string> NumberFormat::RelativeTimeFormatConfigs = {
    { "numeric", "auto" }
};

std::unordered_map<std::string, int> NumberFormat::defaultCurrencyFractionMap = {
    { "BHD", 3 },
    { "XOF", 0 },
    { "BIF", 0 },
    { "XAF", 0 },
    { "CLP", 0 },
    { "CLF", 4 },
    { "KMF", 0 },
    { "DJF", 0 },
    { "XPF", 0 },
    { "GNF", 0 },
    { "ISK", 0 },
    { "IQD", 3 },
    { "JPY", 0 },
    { "JOD", 3 },
    { "KRW", 0 },
    { "KWD", 3 },
    { "LYD", 3 },
    { "OMR", 3 },
    { "PYG", 0 },
    { "RWF", 0 },
    { "TND", 3 },
    { "UGX", 0 },
    { "UYI", 0 },
    { "UYW", 4 },
    { "VUV", 0 },
    { "VND", 0 },
};

static std::unordered_set<std::string> UNIT_USAGE_FILE = {
    "size-file-byte",
    "size-shortfile-byte"
};

static std::unordered_set<std::string> UNIT_USAGE_TEMPERATURE = {
    "temperature",
    "temperature-person",
    "temperature-weather"
};

NumberFormat::NumberFormat(const std::vector<std::string> &localeTags, std::map<std::string, std::string> &configs)
{
    SetDefaultStyle();
    ResolveLocaleTags(localeTags, configs);
    std::string systemLocale = LocaleConfig::GetEffectiveLocale();
    CreateNumberFormatWithDefaultLocale(systemLocale, configs);
}

NumberFormat::NumberFormat(const std::vector<std::string> &localeTags,
    std::map<std::string, std::string> &configs, bool flag)
{
    fromArkTs = flag;
    ResolveLocaleTags(localeTags, configs);
    std::string defaultLocale = LocaleHelper::DefaultLocale();
    CreateNumberFormatWithDefaultLocale(defaultLocale, configs);
}

void NumberFormat::ResolveLocaleTags(const std::vector<std::string> &localeTags,
    std::map<std::string, std::string> &configs)
{
    UErrorCode status = U_ZERO_ERROR;
    std::unique_ptr<icu::LocaleBuilder> builder = nullptr;
    builder = std::make_unique<icu::LocaleBuilder>();
    ParseConfigs(configs);
    for (size_t i = 0; i < localeTags.size(); i++) {
        std::string curLocale = LocaleConfig::RemoveCustExtParam(localeTags[i]);
        locale = icu::Locale::forLanguageTag(icu::StringPiece(curLocale), status);
        if (U_FAILURE(status)) {
            status = U_ZERO_ERROR;
            continue;
        }
        if (LocaleInfo::allValidLocales.count(locale.getLanguage()) > 0) {
            localeInfo = std::make_unique<LocaleInfo>(curLocale, configs);
            CreateRelativeTimeFormat(localeTags[i]);
            if (!localeInfo->InitSuccess()) {
                continue;
            }
            locale = localeInfo->GetLocale();
            localeBaseName = localeInfo->GetBaseName();
            icu::MeasureUnit::getAvailable(unitArray, MAX_UNIT_NUM, status);
            if (U_FAILURE(status)) {
                status = U_ZERO_ERROR;
                continue;
            }
            createSuccess = true;
            ParseExtParam(localeTags[i]);
            break;
        }
    }
}

void NumberFormat::CreateNumberFormatWithDefaultLocale(const std::string &systemLocale,
    std::map<std::string, std::string> &configs)
{
    if (!createSuccess) {
        std::string curLocale = LocaleConfig::RemoveCustExtParam(systemLocale);
        localeInfo = std::make_unique<LocaleInfo>(curLocale, configs);
        CreateRelativeTimeFormat(systemLocale);
        if (localeInfo != nullptr && localeInfo->InitSuccess()) {
            locale = localeInfo->GetLocale();
            localeBaseName = localeInfo->GetBaseName();
            UErrorCode status = U_ZERO_ERROR;
            icu::MeasureUnit::getAvailable(unitArray, MAX_UNIT_NUM, status);
            if (U_SUCCESS(status)) {
                createSuccess = true;
                ParseExtParam(systemLocale);
            }
        }
    }
    if (createSuccess) {
        InitNumberFormat();
    }
}

NumberFormat::~NumberFormat()
{
}

void NumberFormat::InitNumberFormat()
{
    icu::number::UnlocalizedNumberFormatter tempFormatter = icu::number::NumberFormatter::with();
    InitProperties(tempFormatter);
    styleFormatter = tempFormatter;
    numberFormat = tempFormatter.locale(locale);
}

void NumberFormat::CreateRelativeTimeFormat(const std::string& locale)
{
    std::vector<std::string> locales = { locale };
    relativeTimeFormat = std::make_unique<RelativeTimeFormat>(locales,
        RelativeTimeFormatConfigs);
}

void NumberFormat::InitProperties(icu::number::UnlocalizedNumberFormatter &formatter)
{
    InitNumberPattern(formatter);
    InitCurrencyProperties(formatter);
    InitPercentStyleProperties(formatter);
    InitUnitProperties(formatter);
    InitUseGroupingProperties(formatter);
    InitSignProperties(formatter);
    InitNotationProperties(formatter);
    InitIntegerDigits(formatter);
    if (fromArkTs) {
        HandleArktsRoundingProperties(formatter);
    } else {
        InitDigitsProperties(formatter);
        SetRoundingProperties(formatter);
    }
}

void NumberFormat::InitNumberPattern(icu::number::UnlocalizedNumberFormatter &formatter)
{
    UErrorCode status = U_ZERO_ERROR;
    icu::Locale icuLocale = icu::Locale::forLanguageTag(effectiveLocale.c_str(), status);
    if (U_FAILURE(status)) {
        HILOG_ERROR_I18N("NumberFormat::InitNumberPattern: Create icu::Locale failed.");
        return;
    }
    icu::DecimalFormatSymbols* decimalFormatSymbols = new icu::DecimalFormatSymbols(icuLocale, status);
    if (U_FAILURE(status) || decimalFormatSymbols == nullptr) {
        delete decimalFormatSymbols;
        HILOG_ERROR_I18N("NumberFormat::InitNumberPattern: Create icu::DecimalFormatSymbols failed.");
        return;
    }
    if (groupingSymbol.empty()) {
        formatter = formatter.grouping(UNumberGroupingStrategy::UNUM_GROUPING_OFF);
    } else {
        decimalFormatSymbols->setSymbol(icu::DecimalFormatSymbols::ENumberFormatSymbol::kGroupingSeparatorSymbol,
            groupingSymbol.c_str());
    }
    if (!decimalSymbol.empty()) {
        decimalFormatSymbols->setSymbol(icu::DecimalFormatSymbols::ENumberFormatSymbol::kDecimalSeparatorSymbol,
            decimalSymbol.c_str());
    }
    formatter = formatter.symbols(*decimalFormatSymbols);
    delete decimalFormatSymbols;
}

void NumberFormat::InitCurrencyProperties(icu::number::UnlocalizedNumberFormatter &formatter)
{
    if (!currency.empty()) {
        UErrorCode status = U_ZERO_ERROR;
        icu::CurrencyUnit currencyUnit(icu::UnicodeString(currency.c_str()).getBuffer(), status);
        if (U_FAILURE(status)) {
            HILOG_ERROR_I18N("NumberFormat::InitCurrencyProperties: Create currency unit failed.");
            return;
        }
        formatter = formatter.unit(currencyUnit);
        if (currencyDisplay != UNumberUnitWidth::UNUM_UNIT_WIDTH_SHORT) {
            formatter = formatter.unitWidth(currencyDisplay);
        }
    }
}

void NumberFormat::InitPercentStyleProperties(icu::number::UnlocalizedNumberFormatter &formatter)
{
    if (!styleString.empty() && styleString == "percent") {
        formatter = formatter.unit(icu::NoUnit::percent())
            .scale(icu::number::Scale::powerOfTen(2)) // 2 is the power of ten
            .precision(icu::number::Precision::fixedFraction(0));
    }
}

void NumberFormat::InitUseGroupingProperties(icu::number::UnlocalizedNumberFormatter &formatter)
{
    if (!useGrouping.empty()) {
        UNumberGroupingStrategy groupingStrategy = (useGrouping == "true") ?
            UNumberGroupingStrategy::UNUM_GROUPING_AUTO : UNumberGroupingStrategy::UNUM_GROUPING_OFF;
        formatter = formatter.grouping(groupingStrategy);
    }
}

void NumberFormat::InitNotationProperties(icu::number::UnlocalizedNumberFormatter &formatter)
{
    if (!notationString.empty()) {
        formatter = formatter.notation(notation);
    }
}

void NumberFormat::InitSignProperties(icu::number::UnlocalizedNumberFormatter &formatter)
{
    if (!currencySign.empty() || !signDisplayString.empty()) {
        formatter = formatter.sign(signDisplay);
    }
}

void NumberFormat::InitUnitProperties(icu::number::UnlocalizedNumberFormatter &formatter)
{
    if (styleString.empty() || styleString.compare("unit") != 0) {
        return;
    }
    bool foundUnit = false;
    for (icu::MeasureUnit curUnit : unitArray) {
        if (!strcmp(curUnit.getSubtype(), unit.c_str())) {
            formatter = formatter.unit(curUnit);
            foundUnit = true;
            unitType = curUnit.getType();
        }
    }
    if (!foundUnit && fromArkTs) {
        SetPerUnit(formatter);
    }

    auto iter = measurementSystem.find(measurement);
    if (iter != measurementSystem.end()) {
        unitMeasSys = iter->second;
    }

    formatter = formatter.unitWidth(unitDisplay);
    formatter = formatter.precision(icu::number::Precision::maxFraction(DEFAULT_FRACTION_DIGITS));
}

void NumberFormat::SetPerUnit(icu::number::UnlocalizedNumberFormatter &formatter)
{
    icu::MeasureUnit icuUnit;
    icu::MeasureUnit icuPerUnit;
    if (!IsWellFormedUnitIdentifier(icuUnit, icuPerUnit)) {
        HILOG_ERROR_I18N("NumberFormat::SetPerUnit: Get unit failed");
        return;
    }
    icu::MeasureUnit emptyUnit = icu::MeasureUnit();
    if (icuUnit != emptyUnit) {
        formatter = formatter.unit(icuUnit);
    }
    if (icuPerUnit != emptyUnit) {
        formatter = formatter.perUnit(icuPerUnit);
    }
}

void NumberFormat::InitDigitsProperties(icu::number::UnlocalizedNumberFormatter &formatter)
{
    int32_t status = 0;
    if (!maximumSignificantDigits.empty() || !minimumSignificantDigits.empty()) {
        int32_t maxSignificantDigits = ConvertString2Int(maximumSignificantDigits, status);
        if (status == 0) {
            formatter = formatter.precision(icu::number::Precision::maxSignificantDigits(maxSignificantDigits));
        }

        status = 0;
        int32_t minSignificantDigits = ConvertString2Int(minimumSignificantDigits, status);
        if (status == 0) {
            formatter = formatter.precision(icu::number::Precision::minSignificantDigits(minSignificantDigits));
        }
    } else {
        int32_t minFdStatus = 0;
        int32_t minFractionDigits = ConvertString2Int(minimumFractionDigits, minFdStatus);
        int32_t maxFdStatus = 0;
        int32_t maxFractionDigits = ConvertString2Int(maximumFractionDigits, maxFdStatus);
        if (minFdStatus == 0 || maxFdStatus == 0) {
            isSetFraction = true;
        }
        if (minFdStatus == 0 && maxFdStatus != 0) {
            formatter = formatter.precision(icu::number::Precision::minFraction(minFractionDigits));
        } else if (minFdStatus != 0 && maxFdStatus == 0) {
            formatter = formatter.precision(icu::number::Precision::maxFraction(maxFractionDigits));
        } else if (minFdStatus == 0 && maxFdStatus == 0) {
            formatter =
                formatter.precision(icu::number::Precision::minMaxFraction(minFractionDigits, maxFractionDigits));
        }
    }
}

void NumberFormat::InitIntegerDigits(icu::number::UnlocalizedNumberFormatter &formatter)
{
    if (!maximumSignificantDigits.empty() || !minimumSignificantDigits.empty()) {
        return;
    }
    int32_t status = 0;
    int32_t minIntegerDigits = ConvertString2Int(minimumIntegerDigits, status);
    if (status == 0 && minIntegerDigits > 1) {
        formatter = formatter.integerWidth(icu::number::IntegerWidth::zeroFillTo(minIntegerDigits));
    }
}

void NumberFormat::SetRoundingProperties(icu::number::UnlocalizedNumberFormatter &formatter)
{
    if (roundingIncrement != 1) {
        int32_t maxFractionDigits = GetMaximumFractionDigitsValue();
        double increment = pow(POW_BASE_NUMBER, -1.0 * maxFractionDigits) * roundingIncrement;
        formatter = formatter.precision(icu::number::Precision::increment(increment));
    }
    if (!roundingModeStr.empty()) {
        formatter = formatter.roundingMode(roundingMode);
    }
    if (roundingPriorityStr.empty()) {
        return;
    }

    int32_t minFractionDigits = GetMinimumFractionDigitsValue();
    int32_t maxFractionDigits = GetMaximumFractionDigitsValue();
    int32_t maxSigDigits = GetMaximumSignificantDigitsIntValue();
    int32_t minSigDigits = GetMinimumSignificantDigitsIntValue();
    auto precision = icu::number::Precision::minMaxFraction(minFractionDigits, maxFractionDigits)
        .withSignificantDigits(minSigDigits, maxSigDigits, roundingPriority);
    formatter = formatter.precision(precision);
}

void NumberFormat::HandleArktsRoundingProperties(icu::number::UnlocalizedNumberFormatter &formatter)
{
    if (!roundingModeStr.empty()) {
        formatter = formatter.roundingMode(roundingMode);
    }
    if (roundingIncrement != 1) {
        int32_t maxFractionDigits = GetMaximumFractionDigitsValue();
        double increment = pow(POW_BASE_NUMBER, -1.0 * maxFractionDigits) * roundingIncrement;
        formatter = formatter.precision(icu::number::Precision::increment(increment));
    } else {
        if (roundingPriorityStr.empty()) {
            InitDigitsProperties(formatter);
            return;
        }
        int32_t minFractionDigits = GetMinimumFractionDigitsValue();
        int32_t maxFractionDigits = GetMaximumFractionDigitsValue();
        int32_t maxSigDigits = GetMaximumSignificantDigitsIntValue();
        int32_t minSigDigits = GetMinimumSignificantDigitsIntValue();
        auto precision = icu::number::Precision::minMaxFraction(minFractionDigits, maxFractionDigits)
            .withSignificantDigits(minSigDigits, maxSigDigits, roundingPriority);
        formatter = formatter.precision(precision);
    }
}

int32_t NumberFormat::GetMinimumSignificantDigitsIntValue()
{
    int32_t minSignifDigits = 1;
    if (!minimumSignificantDigits.empty()) {
        int32_t status = 0;
        int32_t minSignDigits = ConvertString2Int(minimumSignificantDigits, status);
        minSignifDigits = (status == 0) ? minSignDigits : minSignifDigits;
    }
    return minSignifDigits;
}

int32_t NumberFormat::GetMaximumSignificantDigitsIntValue()
{
    int32_t maxSign = DEFAULT_MAX_SIGNIFICANT_DIGITS;
    if (!maximumSignificantDigits.empty()) {
        int32_t status = 0;
        int32_t maxSignDigits = ConvertString2Int(maximumSignificantDigits, status);
        maxSign = (status == 0) ? maxSignDigits : maxSign;
    }
    return maxSign;
}

int32_t NumberFormat::GetMaximumFractionDigitsValue()
{
    int32_t maxFraction = DEFAULT_FRACTION_DIGITS;
    if (!maximumFractionDigits.empty()) {
        int32_t status = 0;
        int32_t maxFractionDigits = ConvertString2Int(maximumFractionDigits, status);
        maxFraction = (status == 0) ? maxFractionDigits : maxFraction;
    }
    return maxFraction;
}

int32_t NumberFormat::GetMinimumFractionDigitsValue()
{
    int32_t minFraction = 0;
    if (!minimumFractionDigits.empty()) {
        int32_t status = 0;
        int32_t minFractionDigits = ConvertString2Int(minimumFractionDigits, status);
        minFraction = (status == 0) ? minFractionDigits : minFraction;
    }
    return minFraction;
}

void NumberFormat::ParseConfigs(std::map<std::string, std::string> &configs)
{
    if (configs.count("signDisplay") > 0) {
        signDisplayString = configs["signDisplay"];
    }
    if (signAutoStyle.count(signDisplayString) > 0) {
        signDisplay = signAutoStyle[signDisplayString];
    }
    if (configs.count("style") > 0) {
        styleString = configs["style"];
    }
    if (styleString == "unit" && configs.count("unit") > 0) {
        unit = configs["unit"];
        if (configs.count("unitDisplay") > 0) {
            unitDisplayString = configs["unitDisplay"];
            if (unitStyle.count(unitDisplayString) > 0) {
                unitDisplay = unitStyle[unitDisplayString];
            }
        }
        if (configs.count("unitUsage") > 0) {
            unitUsage = configs["unitUsage"];
        }
    }
    if (styleString == "currency" && configs.count("currency") > 0) {
        currency = GetCurrencyFromConfig(configs["currency"]);
        if (configs.count("currencySign") > 0) {
            currencySign = configs["currencySign"];
        }
        if (currencySign.compare("accounting") == 0 && signAccountingStyle.count(signDisplayString) > 0) {
            signDisplay = signAccountingStyle[signDisplayString];
        }
        if (configs.count("currencyDisplay") > 0 && currencyStyle.count(configs["currencyDisplay"]) > 0) {
            currencyDisplayString = configs["currencyDisplay"];
            currencyDisplay = currencyStyle[currencyDisplayString];
        }
    }
    ParseDigitsConfigs(configs);
    ParseRoundingConfigs(configs);
}

void NumberFormat::ParseDigitsConfigs(std::map<std::string, std::string> &configs)
{
    if (configs.count("notation") > 0) {
        notationString = configs["notation"];
        if (notationString == "scientific") {
            notation = icu::number::Notation::scientific();
        } else if (notationString == "engineering") {
            notation = icu::number::Notation::engineering();
        }
        if (notationString == "compact") {
            if (configs.count("compactDisplay") > 0) {
                compactDisplay = configs["compactDisplay"];
            }
            if (compactDisplay == "long") {
                notation = icu::number::Notation::compactLong();
            } else {
                notation = icu::number::Notation::compactShort();
            }
        }
    }
    if (configs.count("minimumIntegerDigits") > 0) {
        minimumIntegerDigits = configs["minimumIntegerDigits"];
    }
    if (configs.count("minimumFractionDigits") > 0) {
        minimumFractionDigits = configs["minimumFractionDigits"];
    }
    if (configs.count("maximumFractionDigits") > 0) {
        maximumFractionDigits = configs["maximumFractionDigits"];
    }
    if (configs.count("minimumSignificantDigits") > 0) {
        minimumSignificantDigits = configs["minimumSignificantDigits"];
    }
    if (configs.count("maximumSignificantDigits") > 0) {
        maximumSignificantDigits = configs["maximumSignificantDigits"];
    }
    if (configs.count("numberingSystem") > 0) {
        numberingSystem = configs["numberingSystem"];
    }
    if (configs.count("useGrouping") > 0) {
        useGrouping = configs["useGrouping"];
    }
    if (configs.count("localeMatcher") > 0) {
        localeMatcher = configs["localeMatcher"];
    }
}

void NumberFormat::ParseRoundingConfigs(std::map<std::string, std::string> &configs)
{
    if (configs.count("roundingMode") > 0) {
        std::string tempRoundingModeStr = configs["roundingMode"];
        auto iter = roundingModeStyle.find(tempRoundingModeStr);
        if (iter != roundingModeStyle.end()) {
            roundingMode = iter->second;
            roundingModeStr = tempRoundingModeStr;
        }
    }
    if (configs.count("roundingPriority") > 0) {
        std::string tempRoundingPriorityStr = configs["roundingPriority"];
        auto iter = roundingPriorityStyle.find(tempRoundingPriorityStr);
        if (iter != roundingPriorityStyle.end()) {
            roundingPriorityStr = tempRoundingPriorityStr;
            roundingPriority = iter->second;
        }
    }
    if (configs.count("roundingIncrement") > 0) {
        std::string increment = configs["roundingIncrement"];
        int32_t status = 0;
        int32_t incrementNum = ConvertString2Int(increment, status);
        if (status != 0) {
            HILOG_ERROR_I18N("NumberFormat::ParseRoundingConfigs: Convert string to int failed.");
            return;
        }
        auto iter = std::find(roundingIncrementList.begin(), roundingIncrementList.end(),
            static_cast<size_t>(incrementNum));
        if (iter != roundingIncrementList.end()) {
            roundingIncrement = *iter;
        }
    }
}

void NumberFormat::ParseExtParam(const std::string& localeTag)
{
    std::pair<std::string, std::string> numberPattern = LocaleConfig::GetNumberPatternFromLocale(localeTag);
    if (numberPattern.second.empty()) {
        return;
    }
    groupingSymbol = numberPattern.first;
    decimalSymbol = numberPattern.second;
    preferredTemperature =
        LocaleConfig::GetTemperatureName(LocaleConfig::GetTemperatureTypeFromLocale(localeTag));
    effectiveLocale = LocaleConfig::RemoveCustExtParam(localeTag);
    if (!numberingSystem.empty()) {
        effectiveLocale = LocaleConfig::ModifyExtParam(effectiveLocale, "nu", numberingSystem, "-u-");
    }
    measurement = LocaleConfig::GetMeasurementFromLocale(localeTag);
}

void NumberFormat::SetUnit(std::string &preferredUnit)
{
    if (preferredUnit.empty()) {
        return;
    }
    for (icu::MeasureUnit curUnit : unitArray) {
        if (!strcmp(curUnit.getSubtype(), preferredUnit.c_str())) {
            numberFormat = numberFormat.unit(curUnit);
            styleFormatter = styleFormatter.unit(curUnit);
        }
    }
}

std::string NumberFormat::Format(double number)
{
    std::string result;
    UErrorCode status = U_ZERO_ERROR;
    icu::UnicodeString formatResult;
    if (IsRelativeTimeFormat(number)) {
        icu::FormattedRelativeDateTime formattedTime = FormatToFormattedRelativeDateTime(number);
        formatResult = formattedTime.toString(status);
    } else {
        icu::number::FormattedNumber formattedNumber = FormatToFormattedNumber(number);
        formatResult = formattedNumber.toString(status);
    }
    if (U_FAILURE(status)) {
        result = "";
    } else {
        formatResult.toUTF8String(result);
    }
    return PseudoLocalizationProcessor(result);
}

std::string NumberFormat::FormatBigInt(const std::string &number)
{
    std::string result;
    icu::number::FormattedNumber formattedNumber = FormatDecimalToFormattedNumber(number);
    UErrorCode status = U_ZERO_ERROR;
    icu::UnicodeString formatResult = formattedNumber.toString(status);
    if (U_SUCCESS(status)) {
        formatResult.toUTF8String(result);
    }
    return PseudoLocalizationProcessor(result);
}

void NumberFormat::FormatToParts(std::vector<std::vector<std::string>> &result)
{
    double number = uprv_getNaN();
    icu::number::FormattedNumber formattedNumber = FormatToFormattedNumber(number);
    FormattedNumberParts(formattedNumber, result, "nan", number);
}

void NumberFormat::FormatBigIntToParts(std::string &number, std::vector<std::vector<std::string>> &result)
{
    icu::number::FormattedNumber formattedNumber = FormatDecimalToFormattedNumber(number);
    double numDouble = GetBigIntFieldType(number);
    FormattedNumberParts(formattedNumber, result, "bigint", numDouble);
}

double NumberFormat::GetBigIntFieldType(const std::string &bigIntStr)
{
    double numDouble = 0;
    if (!bigIntStr.empty() && bigIntStr.at(0) == '-') {
        numDouble = -1.0; // -1 just means number is negative
    }
    return numDouble;
}

void NumberFormat::FormatToParts(double number, std::vector<std::vector<std::string>> &result)
{
    std::string finalUnit = unit;
    if (IsRelativeTimeFormat(number, finalUnit)) {
        RelativeDateTimeFormatToParts(number, finalUnit, result);
        return;
    }
    icu::number::FormattedNumber formattedNumber = FormatToFormattedNumber(number);
    FormattedNumberParts(formattedNumber, result, "number", number);
}

void NumberFormat::FormattedNumberParts(icu::number::FormattedNumber &formattedNumber,
    std::vector<std::vector<std::string>> &result, const std::string &napiType,
    const double &number)
{
    UErrorCode status = U_ZERO_ERROR;
    icu::UnicodeString formatResult = formattedNumber.toString(status);
    if (U_FAILURE(status)) {
        HILOG_ERROR_I18N("NumberFormat::FormatToParts: UnicodeString toString failed.");
        return;
    }
    FindAllFormatParts(formattedNumber, result, number, napiType, formatResult);
}

void NumberFormat::FindAllFormatParts(icu::FormattedValue &formattedNumber,
    std::vector<std::vector<std::string>> &result, double number,
    const std::string &napiType, icu::UnicodeString &formatResult)
{
    icu::ConstrainedFieldPosition cfpo;
    int previousLimit = 0;
    bool lastFieldGroup = false;
    int groupLeapLength = 0;
    UErrorCode status = U_ZERO_ERROR;
    struct FormatPartParam param = {previousLimit, lastFieldGroup, groupLeapLength, number, napiType};
    while (formattedNumber.nextPosition(cfpo, status)) {
        if (U_FAILURE(status)) {
            HILOG_ERROR_I18N("NumberFormat::FindAllFormatParts: Get next position failed.");
            result.clear();
            return;
        }
        SetEveryFormatPartItem(cfpo, formatResult, result, param);
    }
    if (formatResult.length() > param.previousLimit) {
        std::string typeString = "literal";
        AddFormatParts(typeString, result, formatResult, param.previousLimit, formatResult.length());
    }
}

void NumberFormat::FindAllFormatParts(icu::FormattedValue &formattedNumber,
    std::vector<std::vector<std::string>> &result, const double &first,
    const double &second, icu::UnicodeString &formatResult)
{
    icu::ConstrainedFieldPosition cfpo;
    int previousLimit = 0;
    bool lastFieldGroup = false;
    int groupLeapLength = 0;
    size_t rangeSpanCount = 0;
    struct FormatPartParam param = {previousLimit, lastFieldGroup, groupLeapLength, first, "number"};
    UErrorCode status = U_ZERO_ERROR;
    while (formattedNumber.nextPosition(cfpo, status)) {
        if (U_FAILURE(status)) {
            HILOG_ERROR_I18N("NumberFormat::FindAllFormatParts: Get next position failed.");
            result.clear();
            return;
        }
        if (static_cast<UFieldCategory>(cfpo.getCategory()) == UFIELD_CATEGORY_NUMBER_RANGE_SPAN) {
            ++rangeSpanCount;
        }
        if (rangeSpanCount > 1) {
            param.number = second;
        }
        SetEveryFormatPartItem(cfpo, formatResult, result, param);
    }
    if (formatResult.length() > param.previousLimit) {
        std::string typeString = "literal";
        AddFormatParts(typeString, result, formatResult, param.previousLimit, formatResult.length());
    }
}

void NumberFormat::SetEveryFormatPartItem(icu::ConstrainedFieldPosition &cfpo, icu::UnicodeString &formatResult,
    std::vector<std::vector<std::string>> &result, FormatPartParam &param)
{
    int32_t fieldId = cfpo.getField();
    int32_t start = cfpo.getStart();
    int32_t limit = cfpo.getLimit();
    std::string typeString = "literal";
    if (static_cast<UFieldCategory>(cfpo.getCategory()) == UFIELD_CATEGORY_NUMBER_RANGE_SPAN) {
        if (param.previousLimit < start) {
            AddFormatParts(typeString, result, formatResult, param.previousLimit, start);
            param.previousLimit = start;
        }
        return;
    }
    if (static_cast<UNumberFormatFields>(fieldId) == UNUM_GROUPING_SEPARATOR_FIELD) {
        typeString = "integer";
        AddFormatParts(typeString, result, formatResult, param.previousLimit, start);
        {
            typeString = FormatUtils::GetNumberFieldType(param.napiType, fieldId, param.number);
            AddFormatParts(typeString, result, formatResult, start, limit);
        }
        param.lastFieldGroup = true;
        param.groupLeapLength = start - param.previousLimit + 1;
        param.previousLimit = limit;
        return;
    } else if (start > param.previousLimit) {
        AddFormatParts(typeString, result, formatResult, param.previousLimit, start);
    }
    if (param.lastFieldGroup) {
        start = start + param.groupLeapLength;
        param.lastFieldGroup = false;
    }
    if (styleString.compare("unit") == 0 && static_cast<UNumberFormatFields>(fieldId) == UNUM_PERCENT_FIELD) {
        typeString = "unit";
    } else {
        typeString = FormatUtils::GetNumberFieldType(param.napiType, fieldId, param.number);
    }
    if (start < param.previousLimit && param.previousLimit < limit) {
        AddFormatParts(typeString, result, formatResult, param.previousLimit, limit);
    } else {
        AddFormatParts(typeString, result, formatResult, start, limit);
    }
    param.previousLimit = limit;
}

void NumberFormat::AddFormatParts(std::string typeString, std::vector<std::vector<std::string>> &result,
    icu::UnicodeString &formatResult, int32_t start, int32_t end)
{
    std::string value = GetSubString(formatResult, start, end);
    std::vector<std::string> info;
    info.push_back(typeString);
    info.push_back(value);
    result.push_back(info);
}

std::string NumberFormat::GetSubString(icu::UnicodeString &formatResult, int32_t start, int32_t end)
{
    icu::UnicodeString substring = formatResult.tempSubStringBetween(start, end);
    std::string val;
    substring.toUTF8String(val);
    return val;
}

void NumberFormat::RelativeDateTimeFormatToParts(double number, std::string &finalUnit,
    std::vector<std::vector<std::string>> &result)
{
    if (!createSuccess) {
        return;
    }
    double finalNumber = number;
    if (relativeTimeFormat == nullptr) {
        return;
    }
    relativeTimeFormat->FormatToParts(finalNumber, finalUnit, result);
}

bool NumberFormat::IsRelativeTimeFormat(double number)
{
    double finalNumber = number;
    std::string finalUnit = unit;
    return IsRelativeTimeFormat(finalNumber, finalUnit);
}

bool NumberFormat::IsRelativeTimeFormat(double number, std::string &unitForConvert)
{
    double finalNumber = number;
    if (unitUsage.compare("elapsed-time-second") == 0 && ConvertDate(finalNumber, unitForConvert) &&
        relativeTimeFormat != nullptr) {
        return true;
    }
    return false;
}

icu::number::FormattedNumber NumberFormat::FormatDecimalToFormattedNumber(const std::string &number)
{
    if (!createSuccess) {
        return {};
    }
    UErrorCode status = U_ZERO_ERROR;
    icu::number::FormattedNumber formattedNumber =
        numberFormat.formatDecimal(icu::StringPiece(number), status);
    if (U_FAILURE(status)) {
        HILOG_ERROR_I18N("FormatDecimalToFormattedNumber: format %{public}s failed.", number.c_str());
        return {};
    }
    return formattedNumber;
}

icu::number::FormattedNumber NumberFormat::FormatToFormattedNumber(double number)
{
    if (!createSuccess) {
        return {};
    }
    double temp = 0;
    NumberFormat::DealWithUnitUsage(false, number, temp);
    UErrorCode icuStatus = U_ZERO_ERROR;
    icu::number::FormattedNumber formattedNumber = numberFormat.formatDouble(number, icuStatus);
    if (U_FAILURE(icuStatus)) {
        return {};
    }
    return formattedNumber;
}

icu::FormattedRelativeDateTime NumberFormat::FormatToFormattedRelativeDateTime(double number)
{
    if (!createSuccess) {
        return {};
    }

    double finalNumber = number;
    std::string finalUnit = unit;
    if (unitUsage.compare("elapsed-time-second") == 0 && ConvertDate(finalNumber, finalUnit) &&
        relativeTimeFormat != nullptr) {
        return relativeTimeFormat->FormatToFormattedValue(finalNumber, finalUnit);
    }
    return {};
}

std::string NumberFormat::FormatRange(double start, double end)
{
    if (!createSuccess) {
        HILOG_ERROR_I18N("FormatRange: init numberRangeFormat failed.");
        return PseudoLocalizationProcessor("");
    }
    if (end < start) {
        HILOG_ERROR_I18N("FormatRange: parameter:end less then parameter:start");
        return PseudoLocalizationProcessor("");
    }
    NumberFormat::DealWithUnitUsage(true, start, end);
    icu::number::LocalizedNumberRangeFormatter tempNumberRangeFormat = icu::number::NumberRangeFormatter::with()
        .numberFormatterBoth(styleFormatter)
        .locale(locale);
    UErrorCode status = U_ZERO_ERROR;
    icu::number::FormattedNumberRange numberRange = tempNumberRangeFormat.formatFormattableRange(start, end, status);
    if (U_FAILURE(status)) {
        HILOG_ERROR_I18N("FormatRange: formatFormattableRange failed.");
        return PseudoLocalizationProcessor("");
    }
    icu::UnicodeString res = numberRange.toString(status);
    if (U_FAILURE(status)) {
        HILOG_ERROR_I18N("FormatRange: icu::UnicodeString.toString() failed.");
        return PseudoLocalizationProcessor("");
    }
    std::string result;
    res.toUTF8String(result);
    return PseudoLocalizationProcessor(result);
}

std::string NumberFormat::FormatJsRange(double first, double second)
{
    if (!createSuccess) {
        HILOG_ERROR_I18N("FormatRange: init numberRangeFormat failed.");
        return PseudoLocalizationProcessor("");
    }
    icu::number::LocalizedNumberRangeFormatter tempNumberRangeFormat = icu::number::NumberRangeFormatter::with()
        .numberFormatterBoth(styleFormatter)
        .locale(locale);
    UErrorCode status = U_ZERO_ERROR;
    icu::number::FormattedNumberRange numberRange =
        tempNumberRangeFormat.formatFormattableRange(first, second, status);
    if (U_FAILURE(status)) {
        HILOG_ERROR_I18N("FormatRange: formatFormattableRange failed.");
        return PseudoLocalizationProcessor("");
    }
    icu::UnicodeString unicodeStr = numberRange.toString(status);
    if (U_FAILURE(status)) {
        HILOG_ERROR_I18N("FormatRange: icu::UnicodeString.toString() failed.");
        return PseudoLocalizationProcessor("");
    }
    std::string result;
    unicodeStr.toUTF8String(result);
    return PseudoLocalizationProcessor(result);
}

void NumberFormat::FormatRangeToParts(const double &first, const double &second,
    std::vector<std::vector<std::string>> &result)
{
    FormatableRangeToParts(first, second, result);
}

void NumberFormat::FormatableRangeToParts(double first, double second,
    std::vector<std::vector<std::string>> &result)
{
    if (!createSuccess) {
        HILOG_ERROR_I18N("NumberFormat::FormatableRangeToParts: init numberRangeFormat failed.");
        return;
    }
    icu::number::LocalizedNumberRangeFormatter tempNumberRangeFormat = icu::number::NumberRangeFormatter::with()
        .numberFormatterBoth(styleFormatter)
        .locale(locale);
    UErrorCode status = U_ZERO_ERROR;
    icu::number::FormattedNumberRange numberRange =
        tempNumberRangeFormat.formatFormattableRange(first, second, status);
    if (U_FAILURE(status)) {
        HILOG_ERROR_I18N("NumberFormat::FormatableRangeToParts: formatFormattableRange failed.");
        return;
    }
    icu::UnicodeString res = numberRange.toString(status);
    if (U_FAILURE(status)) {
        HILOG_ERROR_I18N("NumberFormat::FormatableRangeToParts: icu::UnicodeString.toString() failed.");
        return;
    }
    FindAllFormatParts(numberRange, result, first, second, res);
}

void NumberFormat::GetResolvedOptions(std::map<std::string, std::string> &map, bool setDefault)
{
    map.insert(std::make_pair("locale", localeBaseName));
    if (!styleString.empty()) {
        map.insert(std::make_pair("style", styleString));
    } else {
        if (setDefault) {
            map.insert(std::make_pair("style", "decimal"));
        }
    }
    if (!currency.empty()) {
        map.insert(std::make_pair("currency", currency));
    }
    if (!currencySign.empty()) {
        map.insert(std::make_pair("currencySign", currencySign));
    } else if (setDefault) {
        map.insert(std::make_pair("currencySign", "standard"));
    }
    if (!currencyDisplayString.empty()) {
        map.insert(std::make_pair("currencyDisplay", currencyDisplayString));
    } else if (setDefault) {
        map.insert(std::make_pair("currencyDisplay", "symbol"));
    }
    if (!signDisplayString.empty()) {
        map.insert(std::make_pair("signDisplay", signDisplayString));
    } else if (setDefault) {
        map.insert(std::make_pair("signDisplay", "auto"));
    }
    if (!compactDisplay.empty()) {
        map.insert(std::make_pair("compactDisplay", compactDisplay));
    }
    if (!unitDisplayString.empty()) {
        map.insert(std::make_pair("unitDisplay", unitDisplayString));
    }
    if (!unitUsage.empty()) {
        map.insert(std::make_pair("unitUsage", unitUsage));
    }
    if (!unit.empty()) {
        map.insert(std::make_pair("unit", unit));
    }
    GetDigitsResolvedOptions(map, setDefault);
    GetDigitsResolvedOptionsExt(map, setDefault);
}

std::vector<std::string> NumberFormat::SupportedLocalesOf(const std::vector<std::string> &requestLocales,
    const std::map<std::string, std::string> &configs, I18nErrorCode &status)
{
    return LocaleHelper::SupportedLocalesOf(requestLocales, configs, status);
}

void NumberFormat::GetDigitsResolvedOptions(std::map<std::string, std::string> &result, bool setDefault)
{
    if (!numberingSystem.empty()) {
        result.insert(std::make_pair("numberingSystem", numberingSystem));
    } else if (!(localeInfo->GetNumberingSystem()).empty()) {
        result.insert(std::make_pair("numberingSystem", localeInfo->GetNumberingSystem()));
    } else {
        UErrorCode status = U_ZERO_ERROR;
        auto numSys = std::unique_ptr<icu::NumberingSystem>(icu::NumberingSystem::createInstance(locale, status));
        if (U_SUCCESS(status)) {
            result.insert(std::make_pair("numberingSystem", numSys->getName()));
        }
    }
    if (!useGrouping.empty()) {
        result.insert(std::make_pair("useGrouping", useGrouping));
    } else if (setDefault) {
        result.insert(std::make_pair("useGrouping", "true"));
    }
    if (!minimumIntegerDigits.empty()) {
        result.insert(std::make_pair("minimumIntegerDigits", minimumIntegerDigits));
    } else if (setDefault) {
        result.insert(std::make_pair("minimumIntegerDigits", "1"));
    }
    if (!minimumFractionDigits.empty()) {
        result.insert(std::make_pair("minimumFractionDigits", minimumFractionDigits));
    } else if (setDefault) {
        GetMinimumFractionDigitsOption(result);
    }
    if (!maximumFractionDigits.empty()) {
        result.insert(std::make_pair("maximumFractionDigits", maximumFractionDigits));
    } else if (setDefault) {
        GetMaximumFractionDigitsOption(result);
    }
    if (!minimumSignificantDigits.empty()) {
        result.insert(std::make_pair("minimumSignificantDigits", minimumSignificantDigits));
    }
    if (!maximumSignificantDigits.empty()) {
        result.insert(std::make_pair("maximumSignificantDigits", maximumSignificantDigits));
    }
}

void NumberFormat::GetDigitsResolvedOptionsExt(std::map<std::string, std::string> &result, bool setDefault)
{
    if (!localeMatcher.empty()) {
        result.insert(std::make_pair("localeMatcher", localeMatcher));
    }
    if (!notationString.empty()) {
        result.insert(std::make_pair("notation", notationString));
    } else if (setDefault) {
        result.insert(std::make_pair("notation", "standard"));
    }
    if (!roundingModeStr.empty()) {
        result.insert(std::make_pair("roundingMode", roundingModeStr));
    } else if (setDefault) {
        result.insert(std::make_pair("roundingMode", "halfExpand"));
    }
    if (!roundingPriorityStr.empty()) {
        result.insert(std::make_pair("roundingPriority", roundingPriorityStr));
    } else if (setDefault) {
        result.insert(std::make_pair("roundingPriority", "auto"));
    }
    if (roundingIncrement != 1) {
        result.insert(std::make_pair("roundingIncrement", std::to_string(roundingIncrement)));
    } else if (setDefault) {
        result.insert(std::make_pair("roundingIncrement", "1"));
    }
}

void NumberFormat::GetMinimumFractionDigitsOption(std::map<std::string, std::string> &result)
{
    std::string style = "decimal";
    if (result.find("style") != result.end()) {
        style = result["style"];
    }
    if (style == "decimal" || style == "percent") {
        result.insert(std::make_pair("minimumFractionDigits", "0"));
    } else if (style == "currency") {
        int fraction = GetCurrencyFractionDigits(result);
        result.insert(std::make_pair("minimumFractionDigits", std::to_string(fraction)));
    }
}

void NumberFormat::GetMaximumFractionDigitsOption(std::map<std::string, std::string> &result)
{
    std::string style = "decimal";
    if (result.find("style") != result.end()) {
        style = result["style"];
    }
    if (style == "decimal") {
        result.insert(std::make_pair("maximumFractionDigits", "3"));
    } else if (style == "percent") {
        result.insert(std::make_pair("maximumFractionDigits", "0"));
    } else if (style == "currency") {
        int fraction = GetCurrencyFractionDigits(result);
        result.insert(std::make_pair("maximumFractionDigits", std::to_string(fraction)));
    }
}

int NumberFormat::GetCurrencyFractionDigits(std::map<std::string, std::string> &resultMap)
{
    int result = 2; // 2 is default value
    std::string currency;
    if (resultMap.find("currency") != resultMap.end()) {
        currency = resultMap["currency"];
        currency = GetCurrencyFromConfig(currency);
    }
    if (defaultCurrencyFractionMap.find(currency) != defaultCurrencyFractionMap.end()) {
        result = defaultCurrencyFractionMap[currency];
    }
    return result;
}

void NumberFormat::SetPrecisionWithByte(double number, const std::string& finalUnit)
{
    if (isSetFraction) {
        return;
    }
    int32_t FractionDigits = -1;
    // 100 is the threshold between different decimal
    if (finalUnit == "byte" || number >= 100) {
        FractionDigits = 0;
    } else if (number < 1) {
        // 2 is the number of significant digits in the decimal
        FractionDigits = 2;
    // 10 is the threshold between different decimal
    } else if (number < 10) {
        if (unitUsage == "size-shortfile-byte") {
            FractionDigits = 1;
        } else {
            // 2 is the number of significant digits in the decimal
            FractionDigits = 2;
        }
    } else {
        if (unitUsage == "size-shortfile-byte") {
            FractionDigits = 0;
        } else {
            // 2 is the number of significant digits in the decimal
            FractionDigits = 2;
        }
    }
    if (FractionDigits != -1) {
        numberFormat = numberFormat.precision(icu::number::Precision::minMaxFraction(FractionDigits, FractionDigits));
        styleFormatter =
            styleFormatter.precision(icu::number::Precision::minMaxFraction(FractionDigits, FractionDigits));
    }
}

std::string NumberFormat::GetCurrency() const
{
    return currency;
}

std::string NumberFormat::GetCurrencySign() const
{
    return currencySign;
}

std::string NumberFormat::GetStyle() const
{
    return styleString;
}

std::string NumberFormat::GetNumberingSystem() const
{
    return numberingSystem;
}

std::string NumberFormat::GetUseGrouping() const
{
    return useGrouping;
}

std::string NumberFormat::GetMinimumIntegerDigits() const
{
    return minimumIntegerDigits;
}

std::string NumberFormat::GetMinimumFractionDigits() const
{
    return minimumFractionDigits;
}

std::string NumberFormat::GetMaximumFractionDigits() const
{
    return maximumFractionDigits;
}

std::string NumberFormat::GetMinimumSignificantDigits() const
{
    return minimumSignificantDigits;
}

std::string NumberFormat::GetMaximumSignificantDigits() const
{
    return maximumSignificantDigits;
}

std::string NumberFormat::GetLocaleMatcher() const
{
    return localeMatcher;
}

void NumberFormat::SetUnitUsage(const std::string& value)
{
    unitUsage = value;
}

bool NumberFormat::Init()
{
    SetHwIcuDirectory();
    return true;
}

void NumberFormat::SetDefaultStyle()
{
    char value[LocaleConfig::CONFIG_LEN];
    int code = GetParameter(DEVICE_TYPE_NAME, "", value, LocaleConfig::CONFIG_LEN);
    if (code > 0) {
        std::string deviceType = value;
        if (defaultUnitStyle.find(deviceType) != defaultUnitStyle.end()) {
            unitDisplay = defaultUnitStyle[deviceType];
        }
        if (defaultCurrencyStyle.find(deviceType) != defaultCurrencyStyle.end()) {
            currencyDisplay = defaultCurrencyStyle[deviceType];
        }
    }
}

bool NumberFormat::ReadISO4217Datas()
{
    if (initISO4217) {
        return true;
    }
    std::lock_guard<std::mutex> readDataLock(numToCurrencyMutex);
    if (initISO4217) {
        return true;
    }
    UErrorCode status = U_ZERO_ERROR;
    const char *currentCurrency;
    UEnumeration *currencies = ucurr_openISOCurrencies(UCURR_ALL, &status);
    if (U_FAILURE(status)) {
        return false;
    }

    UChar code[CURRENCY_LEN + 1];  // +1 includes the NUL
    int32_t length = 0;
    while ((currentCurrency = uenum_next(currencies, &length, &status)) != NULL) {
        u_charsToUChars(currentCurrency, code, length + 1);  // +1 includes the NUL
        int32_t numCode = ucurr_getNumericCode(code);
        if (numCode == 0) {
            continue;
        }
        std::stringstream ss;
        ss << std::setw(CURRENCY_LEN) << std::setfill('0') << numCode; // fill with '0'
        numToCurrency.insert(std::make_pair<std::string, std::string>(ss.str(), currentCurrency));
    }
    uenum_close(currencies);
    initISO4217 = true;
    return !numToCurrency.empty();
}

std::string NumberFormat::GetCurrencyFromConfig(const std::string& currency)
{
    if (currency.size() != CURRENCY_LEN) {
        HILOG_ERROR_I18N("Invalid currency code : %{public}s", currency.c_str());
        return "";
    }
    bool isAlpha = true;
    for (auto c : currency) {
        isAlpha = std::isalpha(c);
        if (!isAlpha) {
            break;
        }
    }
    if (isAlpha) {
        return currency;
    }
    if (ReadISO4217Datas() && numToCurrency.find(currency) != numToCurrency.end()) {
        return numToCurrency[currency];
    }
    HILOG_ERROR_I18N("Invalid currency code : %{public}s", currency.c_str());
    return "";
}

std::string NumberFormat::GetIcuDefaultLocale()
{
    return "en-US";
}

std::string NumberFormat::CheckNumberFormatOptions(std::map<std::string, std::string> &options, int32_t &code)
{
    std::string message = CheckFormatOptions(options, code);
    if (!message.empty()) {
        return message;
    }
    message = CheckRoundingOptions(options, code);
    if (!message.empty()) {
        return message;
    }
    std::vector<std::tuple<std::string, int, int>> vect = {
        {"minimumIntegerDigits", 1, 21}, // 21 is maximum of minimumIntegerDigits
        {"minimumFractionDigits", 0, 20}, // 20 is maximum of minimumFractionDigits
        {"maximumFractionDigits", 0, 20}, // 20 is maximum of maximumFractionDigits
        {"minimumSignificantDigits", 1, 21}, // 21 is maximum of minimumSignificantDigits
        {"maximumSignificantDigits", 1, 21}, // 21 is maximum of maximumSignificantDigits
    };
    for (auto iter = vect.begin(); iter != vect.end(); ++iter) {
        std::string optionName = std::get<0>(*iter);
        int min = std::get<1>(*iter); // 1 is item index
        int max = std::get<2>(*iter); // 2 is item index
        CheckNumberFormatOptionsExt(options, code, optionName, min, max);
        if (code != 0) {
            return "";
        }
    }
    return "";
}

std::string NumberFormat::CheckFormatOptions(std::map<std::string, std::string> &options, int32_t &code)
{
    if (options.find("style") != options.end()) {
        std::string style = options["style"];
        if (style.compare("currency") == 0 && options.find("currency") == options.end()) {
            code = -1;
            return "style is currency but currency is undefined";
        }
        if (style.compare("unit") == 0 && options.find("unit") == options.end()) {
            code = -1;
            return "style is unit but unit is undefined";
        }
    }
    if (options.find("currency") != options.end()) {
        std::string currency = options["currency"];
        std::string checkCurr = GetCurrencyFromConfig(currency);
        if (checkCurr.empty()) {
            code = -1;
            return "not a wellformed code";
        }
    }
    if (options.find("localeMatcher") != options.end()) {
        I18nErrorCode status = I18nErrorCode::SUCCESS;
        std::string localeMatcher = LocaleHelper::ParseOption(options,
            "localeMatcher", "best fit", false, status);
        if (status != I18nErrorCode::SUCCESS) {
            code = -1;
            return "getStringOption failed";
        }
    }
    if (options.find("currencySign") != options.end()) {
        std::string currencySign = options["currencySign"];
        if (currencySign.compare("standard") != 0 && currencySign.compare("accounting") != 0) {
            code = -1;
            return "getStringOption failed";
        }
    }
    if (options.find("numberingSystem") != options.end()) {
        std::string numberSystemStr = options["numberingSystem"];
        std::vector<std::string> numberSystems = GetAvailableNumberSystems();
        auto iter = std::find(numberSystems.begin(), numberSystems.end(), numberSystemStr);
        if (iter == numberSystems.end()) {
            code = -1;
            return "getStringOption failed";
        }
    }
    return "";
}

std::string NumberFormat::CheckRoundingOptions(std::map<std::string, std::string> &options, int32_t &code)
{
    if (options.find("roundingMode") != options.end()) {
        std::string roundingModeStr = options["roundingMode"];
        if (roundingModeStyle.find(roundingModeStr) == roundingModeStyle.end()) {
            code = -1;
            return "getStringOption failed";
        }
    }
    if (options.find("roundingPriority") != options.end()) {
        std::string roundingPriorityStr = options["roundingPriority"];
        if (roundingPriorityStyle.find(roundingPriorityStr) == roundingPriorityStyle.end()) {
            code = -1;
            return "getStringOption failed";
        }
    }
    if (options.find("roundingIncrement") != options.end()) {
        std::string roundingIncrementStr = options["roundingIncrement"];
        std::vector<std::string> availableValues;
        std::for_each(roundingIncrementList.begin(), roundingIncrementList.end(),
            [&availableValues](int num) { availableValues.push_back(std::to_string(num)); });
        auto iter = std::find(availableValues.begin(), availableValues.end(), roundingIncrementStr);
        if (iter == availableValues.end()) {
            code = -1;
            return "getStringOption failed";
        }
    }
    return "";
}

std::vector<std::string> NumberFormat::GetAvailableNumberSystems()
{
    std::vector<std::string> result;
    UErrorCode status = U_ZERO_ERROR;
    icu::StringEnumeration* iter = icu::NumberingSystem::getAvailableNames(status);
    std::unique_ptr<icu::StringEnumeration> stringEnumeratePtr(iter);
    if (U_FAILURE(status) || stringEnumeratePtr == nullptr) {
        HILOG_ERROR_I18N("GetAvailableNumberSystems: Get NumberingSystems failed.");
        return result;
    }
    icu::UnicodeString temp;
    while ((temp = stringEnumeratePtr->next(nullptr, status)) != nullptr) {
        std::string numberSys;
        temp.toUTF8String(numberSys);
        result.push_back(numberSys);
    }
    return result;
}

void NumberFormat::CheckNumberFormatOptionsExt(std::map<std::string, std::string> &map, int32_t &code,
    const std::string &optionName, int min, int max)
{
    if (map.find(optionName) != map.end()) {
        std::string optionNameStr = map[optionName];
        int32_t status = 0;
        int32_t optionValue = ConvertString2Int(optionNameStr, status);
        if (status != 0) {
            code = -1;
            return;
        }
        if (optionValue < min || optionValue > max) {
            code = -1;
        }
    }
}

void NumberFormat::DealWithUnitUsage(bool isFormatRange, double& firstNumber, double& secondNumber)
{
    if (!createSuccess) {
        HILOG_ERROR_I18N("NumberFormat::DealWithUnitUsage: init numberRangeFormat failed.");
        return;
    }
    if (unitUsage.empty()) {
        return;
    }
    double finalNumber = firstNumber;
    std::string finalUnit = unit;
    if (!NumberFormat::DealWithUnitConvert(finalNumber, finalUnit)) {
        HILOG_ERROR_I18N("NumberFormat::DealWithUnitUsage: Deal with unit convert failed.");
        return;
    }
    if (isFormatRange) {
        if (!Convert(secondNumber, unit, unitMeasSys, finalUnit, unitMeasSys)) {
            HILOG_ERROR_I18N("NumberFormat::DealWithUnitUsage: Convert secondNumber failed.");
            return;
        }
    }
    firstNumber = finalNumber;
    SetUnit(finalUnit);
}

bool NumberFormat::DealWithUnitConvert(double& finalNumber, std::string& finalUnit)
{
    if (UNIT_USAGE_FILE.find(unitUsage) != UNIT_USAGE_FILE.end()) {
        if (!ConvertByte(finalNumber, finalUnit)) {
            HILOG_ERROR_I18N("NumberFormat::DealWithUnitConvert: Convert byte failed.");
            return false;
        }
        SetPrecisionWithByte(finalNumber, finalUnit);
    } else if (UNIT_USAGE_TEMPERATURE.find(unitUsage) != UNIT_USAGE_TEMPERATURE.end()) {
        if (preferredTemperature.empty() ||
            Convert(finalNumber, unit, unitMeasSys, preferredTemperature, unitMeasSys) == 0) {
            return false;
        }
        finalUnit = preferredTemperature;
    } else {
        if (localeInfo == nullptr) {
            HILOG_ERROR_I18N("NumberFormat::DealWithUnitConvert: localeInfo is nullptr.");
            return false;
        }
        std::vector<std::string> preferredUnits;
        if (unitUsage == "default") {
            GetDefaultPreferredUnit(localeInfo->GetRegion(), unitType, preferredUnits);
        } else {
            GetPreferredUnit(localeInfo->GetRegion(), unitUsage, preferredUnits);
        }
        std::map<double, std::string> preferredValuesOverOne;
        std::map<double, std::string> preferredValuesUnderOne;
        double num = finalNumber;
        for (size_t i = 0; i < preferredUnits.size(); i++) {
            if (!Convert(num, unit, unitMeasSys, preferredUnits[i], unitMeasSys)) {
                HILOG_ERROR_I18N("NumberFormat::DealWithUnitConvert: Convert %{public}s failed.", unit.c_str());
                continue;
            }
            if (num >= 1) {
                preferredValuesOverOne.insert(std::make_pair(num, preferredUnits[i]));
            } else {
                preferredValuesUnderOne.insert(std::make_pair(num, preferredUnits[i]));
            }
        }
        if (preferredValuesOverOne.empty() && preferredValuesUnderOne.empty()) {
            return false;
        }
        if (preferredValuesOverOne.size() > 0) {
            std::tie(finalNumber, finalUnit) = *preferredValuesOverOne.begin();
        } else if (preferredValuesUnderOne.size() > 0) {
            std::tie(finalNumber, finalUnit) = *preferredValuesUnderOne.begin();
        }
    }
    return true;
}

bool NumberFormat::ToMeasureUnit(const std::string &sanctionedUnit,
    icu::MeasureUnit &measureUnit)
{
    for (auto &unitItem : unitArray) {
        if (std::strcmp(sanctionedUnit.c_str(), unitItem.getSubtype()) == 0) {
            measureUnit = unitItem;
            return true;
        }
    }
    return false;
}

bool NumberFormat::IsWellFormedUnitIdentifier(icu::MeasureUnit &icuUnit,
    icu::MeasureUnit &icuPerUnit)
{
    icu::MeasureUnit emptyUnit = icu::MeasureUnit();
    auto pos = unit.find("-per-");
    if (ToMeasureUnit(unit, icuUnit) && pos == std::string::npos) {
        icuPerUnit = emptyUnit;
        return true;
    }
    size_t afterPos = pos + NumberFormat::PERUNIT_STRING;
    if (pos == std::string::npos || unit.find("-per-", afterPos) != std::string::npos) {
        HILOG_ERROR_I18N("IsWellFormedUnitIdentifier: invalid unit: %{public}s", unit.c_str());
        return false;
    }
    std::string numerator = unit.substr(0, pos);
    if (!ToMeasureUnit(numerator, icuUnit)) {
        HILOG_ERROR_I18N("IsWellFormedUnitIdentifier: before unit: %{public}s invalid", numerator.c_str());
        return false;
    }
    std::string denominator = unit.substr(pos + NumberFormat::PERUNIT_STRING);
    if (!ToMeasureUnit(denominator, icuPerUnit)) {
        HILOG_ERROR_I18N("IsWellFormedUnitIdentifier: after unit: %{public}s invalid", denominator.c_str());
        return false;
    }
    return true;
}

std::string NumberFormat::GetNumberPatternSample(const std::string& localeTag,
    const std::pair<std::string, std::string>& numberPattern)
{
    UErrorCode status = U_ZERO_ERROR;
    icu::Locale icuLocale = icu::Locale::forLanguageTag(icu::StringPiece(localeTag), status);
    if (U_FAILURE(status)) {
        HILOG_ERROR_I18N("NumberFormat::GetNumberPatternSample: Create icu::Locale failed.");
        return "";
    }

    icu::DecimalFormatSymbols* decimalFormatSymbols = new icu::DecimalFormatSymbols(icuLocale, status);
    if (U_FAILURE(status) || decimalFormatSymbols == nullptr) {
        delete decimalFormatSymbols;
        HILOG_ERROR_I18N("NumberFormat::GetNumberPatternSample: Create icu::DecimalFormatSymbols failed.");
        return "";
    }
    icu::number::UnlocalizedNumberFormatter tempFormatter = icu::number::NumberFormatter::with();
    std::string groupingSymbol = numberPattern.first;
    if (groupingSymbol.empty()) {
        tempFormatter = tempFormatter.grouping(UNumberGroupingStrategy::UNUM_GROUPING_OFF);
    } else {
        decimalFormatSymbols->setSymbol(icu::DecimalFormatSymbols::ENumberFormatSymbol::kGroupingSeparatorSymbol,
            groupingSymbol.c_str());
    }
    std::string decimalSymbol = numberPattern.second;
    if (!decimalSymbol.empty()) {
        decimalFormatSymbols->setSymbol(icu::DecimalFormatSymbols::ENumberFormatSymbol::kDecimalSeparatorSymbol,
            decimalSymbol.c_str());
    }
    tempFormatter = tempFormatter.symbols(*decimalFormatSymbols);
    delete decimalFormatSymbols;
    icu::number::LocalizedNumberFormatter numberFormat = tempFormatter.locale(icuLocale);
    icu::number::FormattedNumber formattedNumber = numberFormat.formatDouble(DEFAULT_SAMPLE_NUMBER, status);
    if (U_FAILURE(status)) {
        HILOG_ERROR_I18N("NumberFormat::GetNumberPatternSample: Format double failed.");
        return "";
    }
    icu::UnicodeString formatResult = formattedNumber.toString(status);
    if (U_FAILURE(status)) {
        HILOG_ERROR_I18N("NumberFormat::GetNumberPatternSample: ToString failed.");
        return "";
    }
    std::string result;
    formatResult.toUTF8String(result);
    return result;
}
} // namespace I18n
} // namespace Global
} // namespace OHOS
