/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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
#include <sstream>
#include "i18n_hilog.h"
#include "ohos/init_data.h"
#include "parameter.h"
#include "unicode/ucurr.h"
#include "unicode/uenum.h"
#include "unicode/utypes.h"
#include "unicode/unumberformatter.h"
#include "utils.h"
#include "number_format.h"

namespace OHOS {
namespace Global {
namespace I18n {
const char* NumberFormat::DEVICE_TYPE_NAME = "const.product.devicetype";
std::mutex NumberFormat::numToCurrencyMutex;
bool NumberFormat::initISO4217 = false;
std::unordered_map<std::string, std::string> NumberFormat::numToCurrency = {};
bool NumberFormat::icuInitialized = NumberFormat::Init();

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

std::unordered_map<UMeasurementSystem, std::string> NumberFormat::measurementSystem = {
    { UMeasurementSystem::UMS_SI, "SI" },
    { UMeasurementSystem::UMS_US, "US" },
    { UMeasurementSystem::UMS_UK, "UK" },
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

NumberFormat::NumberFormat(const std::vector<std::string> &localeTags, std::map<std::string, std::string> &configs)
{
    SetDefaultStyle();
    UErrorCode status = U_ZERO_ERROR;
    std::unique_ptr<icu::LocaleBuilder> builder = nullptr;
    builder = std::make_unique<icu::LocaleBuilder>();
    ParseConfigs(configs);
    for (size_t i = 0; i < localeTags.size(); i++) {
        std::string curLocale = localeTags[i];
        locale = icu::Locale::forLanguageTag(icu::StringPiece(curLocale), status);
        if (U_FAILURE(status)) {
            status = U_ZERO_ERROR;
            continue;
        }
        if (LocaleInfo::allValidLocales.count(locale.getLanguage()) > 0) {
            localeInfo = std::make_unique<LocaleInfo>(curLocale, configs);
            CreateRelativeTimeFormat(curLocale);
            if (!localeInfo->InitSuccess()) {
                continue;
            }
            locale = localeInfo->GetLocale();
            localeBaseName = localeInfo->GetBaseName();
            icu::MeasureUnit::getAvailable(unitArray, MAX_UNIT_NUM, status);
            if (!U_SUCCESS(status)) {
                status = U_ZERO_ERROR;
                continue;
            }
            createSuccess = true;
            break;
        }
    }
    if (!createSuccess) {
        std::string systemLocale = LocaleConfig::GetSystemLocale();
        localeInfo = std::make_unique<LocaleInfo>(systemLocale, configs);
        CreateRelativeTimeFormat(systemLocale);
        if (localeInfo->InitSuccess()) {
            locale = localeInfo->GetLocale();
            localeBaseName = localeInfo->GetBaseName();
            icu::MeasureUnit::getAvailable(unitArray, MAX_UNIT_NUM, status);
            if (U_SUCCESS(status)) {
                createSuccess = true;
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
    icu::number::UnlocalizedNumberFormatter formatter = icu::number::NumberFormatter::with();
    InitProperties(formatter);
    numberFormat = formatter.locale(locale);
    numberRangeFormat = icu::number::NumberRangeFormatter::with()
        .numberFormatterBoth(formatter)
        .locale(locale);
}

void NumberFormat::CreateRelativeTimeFormat(const std::string& locale)
{
    if (unitUsage == "elapsed-time-second") {
        std::vector<std::string> locales = { locale };
        relativeTimeFormat = std::make_unique<RelativeTimeFormat>(locales,
            RelativeTimeFormatConfigs);
    }
}

void NumberFormat::InitProperties(icu::number::UnlocalizedNumberFormatter &formatter)
{
    InitCurrencyProperties(formatter);
    InitPercentStyleProperties(formatter);
    InitUnitProperties(formatter);
    InitUseGroupingProperties(formatter);
    InitSignProperties(formatter);
    InitNotationProperties(formatter);
    InitDigitsProperties(formatter);
}

void NumberFormat::InitCurrencyProperties(icu::number::UnlocalizedNumberFormatter &formatter)
{
    if (!currency.empty()) {
        UErrorCode status = U_ZERO_ERROR;
        icu::CurrencyUnit currencyUnit(icu::UnicodeString(currency.c_str()).getBuffer(), status);
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
    for (icu::MeasureUnit curUnit : unitArray) {
        if (!strcmp(curUnit.getSubtype(), unit.c_str())) {
            formatter = formatter.unit(curUnit);
            unitType = curUnit.getType();
        }
    }
    UErrorCode status = U_ZERO_ERROR;
    UMeasurementSystem measSys = ulocdata_getMeasurementSystem(localeBaseName.c_str(), &status);
    bool isValidMeaSys = measurementSystem.find(measSys) != measurementSystem.end();
    if (U_SUCCESS(status) && measSys >= 0 && isValidMeaSys) {
        unitMeasSys = measurementSystem[measSys];
    }
    formatter = formatter.unitWidth(unitDisplay);
    formatter = formatter.precision(icu::number::Precision::maxFraction(DEFAULT_FRACTION_DIGITS));
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
        int32_t minIntegerDigits = ConvertString2Int(minimumIntegerDigits, status);
        if (status == 0 && minIntegerDigits > 1) {
            formatter = formatter.integerWidth(icu::number::IntegerWidth::zeroFillTo(minIntegerDigits));
        }

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
    SetRoundingProperties(formatter);
    HandleRoundingPriority(formatter);
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
}

void NumberFormat::HandleRoundingPriority(icu::number::UnlocalizedNumberFormatter &formatter)
{
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
        auto iter = std::find(roundingIncrementList.begin(), roundingIncrementList.end(),
            static_cast<size_t>(incrementNum));
        if (status == 0 && iter != roundingIncrementList.end()) {
            roundingIncrement = *iter;
        }
    }
}

void NumberFormat::SetUnit(std::string &preferredUnit)
{
    if (preferredUnit.empty()) {
        return;
    }
    for (icu::MeasureUnit curUnit : unitArray) {
        if (!strcmp(curUnit.getSubtype(), preferredUnit.c_str())) {
            numberFormat = numberFormat.unit(curUnit);
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

bool NumberFormat::IsRelativeTimeFormat(double number)
{
    double finalNumber = number;
    std::string finalUnit = unit;
    if (unitUsage.compare("elapsed-time-second") == 0 && ConvertDate(finalNumber, finalUnit) &&
        relativeTimeFormat != nullptr) {
        return true;
    }
    return false;
}

icu::number::FormattedNumber NumberFormat::FormatToFormattedNumber(double number)
{
    if (!createSuccess) {
        return {};
    }
    double finalNumber = number;
    std::string finalUnit = unit;
    if ((unitUsage.compare("size-file-byte") == 0 || unitUsage.compare("size-shortfile-byte") == 0) &&
        ConvertByte(finalNumber, finalUnit)) {
        SetUnit(finalUnit);
        SetPrecisionWithByte(finalNumber, finalUnit);
    } else if (!unitUsage.empty()) {
        std::vector<std::string> preferredUnits;
        if (unitUsage.compare("default") == 0) {
            GetDefaultPreferredUnit(localeInfo->GetRegion(), unitType, preferredUnits);
        } else {
            GetPreferredUnit(localeInfo->GetRegion(), unitUsage, preferredUnits);
        }
        std::map<double, std::string> preferredValuesOverOne;
        std::map<double, std::string> preferredValuesUnderOne;
        double num = number;
        for (size_t i = 0; i < preferredUnits.size(); i++) {
            int status = Convert(num, unit, unitMeasSys, preferredUnits[i], unitMeasSys);
            if (!status) {
                continue;
            }
            if (num >= 1) {
                preferredValuesOverOne.insert(std::make_pair(num, preferredUnits[i]));
            } else {
                preferredValuesUnderOne.insert(std::make_pair(num, preferredUnits[i]));
            }
        }
        std::string preferredUnit;
        if (preferredValuesOverOne.size() > 0) {
            finalNumber = preferredValuesOverOne.begin()->first;
            preferredUnit = preferredValuesOverOne.begin()->second;
        } else if (preferredValuesUnderOne.size() > 0) {
            finalNumber = preferredValuesUnderOne.rbegin()->first;
            preferredUnit = preferredValuesUnderOne.rbegin()->second;
        }
        SetUnit(preferredUnit);
    }
    UErrorCode icuStatus = U_ZERO_ERROR;
    icu::number::FormattedNumber formattedNumber = numberFormat.formatDouble(finalNumber, icuStatus);
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
    UErrorCode status = U_ZERO_ERROR;
    icu::number::FormattedNumberRange numberRange =
        numberRangeFormat.formatFormattableRange(start, end, status);
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

void NumberFormat::GetResolvedOptions(std::map<std::string, std::string> &map)
{
    map.insert(std::make_pair("locale", localeBaseName));
    if (!styleString.empty()) {
        map.insert(std::make_pair("style", styleString));
    }
    if (!currency.empty()) {
        map.insert(std::make_pair("currency", currency));
    }
    if (!currencySign.empty()) {
        map.insert(std::make_pair("currencySign", currencySign));
    }
    if (!currencyDisplayString.empty()) {
        map.insert(std::make_pair("currencyDisplay", currencyDisplayString));
    }
    if (!signDisplayString.empty()) {
        map.insert(std::make_pair("signDisplay", signDisplayString));
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
    GetDigitsResolvedOptions(map);
}

void NumberFormat::GetDigitsResolvedOptions(std::map<std::string, std::string> &map)
{
    if (!numberingSystem.empty()) {
        map.insert(std::make_pair("numberingSystem", numberingSystem));
    } else if (!(localeInfo->GetNumberingSystem()).empty()) {
        map.insert(std::make_pair("numberingSystem", localeInfo->GetNumberingSystem()));
    } else {
        UErrorCode status = U_ZERO_ERROR;
        auto numSys = std::unique_ptr<icu::NumberingSystem>(icu::NumberingSystem::createInstance(locale, status));
        if (U_SUCCESS(status)) {
            map.insert(std::make_pair("numberingSystem", numSys->getName()));
        }
    }
    if (!useGrouping.empty()) {
        map.insert(std::make_pair("useGrouping", useGrouping));
    }
    if (!minimumIntegerDigits.empty()) {
        map.insert(std::make_pair("minimumIntegerDigits", minimumIntegerDigits));
    }
    if (!minimumFractionDigits.empty()) {
        map.insert(std::make_pair("minimumFractionDigits", minimumFractionDigits));
    }
    if (!maximumFractionDigits.empty()) {
        map.insert(std::make_pair("maximumFractionDigits", maximumFractionDigits));
    }
    if (!minimumSignificantDigits.empty()) {
        map.insert(std::make_pair("minimumSignificantDigits", minimumSignificantDigits));
    }
    if (!maximumSignificantDigits.empty()) {
        map.insert(std::make_pair("maximumSignificantDigits", maximumSignificantDigits));
    }
    if (!localeMatcher.empty()) {
        map.insert(std::make_pair("localeMatcher", localeMatcher));
    }
    if (!notationString.empty()) {
        map.insert(std::make_pair("notation", notationString));
    }
    if (!roundingModeStr.empty()) {
        map.insert(std::make_pair("roundingMode", roundingModeStr));
    }
    if (!roundingPriorityStr.empty()) {
        map.insert(std::make_pair("roundingPriority", roundingPriorityStr));
    }
    if (roundingIncrement != 1) {
        map.insert(std::make_pair("roundingIncrement", std::to_string(roundingIncrement)));
    }
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

bool NumberFormat::Init()
{
    SetHwIcuDirectory();
    return true;
}

void NumberFormat::SetDefaultStyle()
{
    char value[BUFFER_LEN];
    int code = GetParameter(DEVICE_TYPE_NAME, "", value, BUFFER_LEN);
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
} // namespace I18n
} // namespace Global
} // namespace OHOS
