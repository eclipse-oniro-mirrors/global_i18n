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

#include "intl_date_time_format.h"

#include <algorithm>
#include "ohos/init_data.h"
#include "unicode/gregocal.h"
#include "i18n_hilog.h"
#include "locale_config.h"
#include "locale_helper.h"
#include "locale_info.h"
#include "utils.h"

namespace OHOS {
namespace Global {
namespace I18n {
const std::string IntlDateTimeFormat::LOCALE_TAG = "locale";
const std::string IntlDateTimeFormat::LOCALE_MATCHER_TAG = "localeMatcher";
const std::string IntlDateTimeFormat::WEEK_DAY_TAG = "weekday";
const std::string IntlDateTimeFormat::ERA_TAG = "era";
const std::string IntlDateTimeFormat::YEAR_TAG = "year";
const std::string IntlDateTimeFormat::MONTH_TAG = "month";
const std::string IntlDateTimeFormat::DAY_TAG = "day";
const std::string IntlDateTimeFormat::HOUR_TAG = "hour";
const std::string IntlDateTimeFormat::MINUTE_TAG = "minute";
const std::string IntlDateTimeFormat::SECOND_TAG = "second";
const std::string IntlDateTimeFormat::TIME_ZONE_NAME_TAG = "timeZoneName";
const std::string IntlDateTimeFormat::FORMAT_MATCHER_TAG = "formatMatcher";
const std::string IntlDateTimeFormat::HOUR12_TAG = "hour12";
const std::string IntlDateTimeFormat::TIME_ZONE_TAG = "timeZone";
const std::string IntlDateTimeFormat::CALENDAR_TAG = "calendar";
const std::string IntlDateTimeFormat::DAY_PERIOD_TAG = "dayPeriod";
const std::string IntlDateTimeFormat::NUMBERING_SYSTEM_TAG = "numberingSystem";
const std::string IntlDateTimeFormat::DATE_STYLE_TAG = "dateStyle";
const std::string IntlDateTimeFormat::TIME_STYLE_TAG = "timeStyle";
const std::string IntlDateTimeFormat::HOUR_CYCLE_TAG = "hourCycle";
const std::string IntlDateTimeFormat::FRACTIONAL_SECOND_DIGITS_TAG = "fractionalSecondDigits";
const std::string DEFAULT_NUMBERING_SYSTEM = "latn";
const size_t NUMBER_OF_PARAMETER = 2;
const int32_t MAX_VALUE_OF_FRACTIONAL_SECOND_DIGITS = 3;
const int32_t MIN_VALUE_OF_FRACTIONAL_SECOND_DIGITS = 1;
static const std::string HOUR_CYCLE_11 = "h11";
static const std::string HOUR_CYCLE_12 = "h12";
static const std::string HOUR_CYCLE_23 = "h23";
static const std::string HOUR_CYCLE_24 = "h24";

const std::unordered_map<std::string, icu::DateFormat::EStyle> DATE_TIME_STYLE_TO_ESTYLE = {
    { "full", icu::DateFormat::EStyle::kFull },
    { "long", icu::DateFormat::EStyle::kLong },
    { "medium", icu::DateFormat::EStyle::kMedium },
    { "short", icu::DateFormat::EStyle::kShort }
};

const std::unordered_set<std::string> FORMAT_MATCHER_VALUE = {
    "basic", "best fit"
};

const std::unordered_set<std::string> DATE_TIME_STYLE_VALUE = {
    "full", "long", "medium", "short"
};

const std::unordered_set<std::string> DATE_TIME_ELEMENT = {
    "weekday", "year", "month", "day", "dayPeriod", "hour", "minute", "second", "fractionalSecondDigits"
};

const std::unordered_map<std::string, std::pair<std::string, std::string>> HOUR_CYCLE_TO_PATTERN = {
    { "h11", { "KK", "K" } },
    { "h12", { "hh", "h" } },
    { "h23", { "HH", "H" } },
    { "h24", { "kk", "k" } }
};

const std::unordered_map<char16_t, std::string> HOUR_CYCLE_MAP = {
    { 'K', "h11" },
    { 'h', "h12" },
    { 'H', "h23" },
    { 'k', "h24" }
};

const std::unordered_map<UDateFormatHourCycle, std::string> UDATE_FORMAT_HOUR_CYCLE_TO_STRING = {
    { UDAT_HOUR_CYCLE_11, "h11" },
    { UDAT_HOUR_CYCLE_12, "h12" },
    { UDAT_HOUR_CYCLE_23, "h23" },
    { UDAT_HOUR_CYCLE_24, "h24" }
};

const std::vector<std::string> ICU_LONG_SHORT = {
    "long", "short",
    "longOffset", "shortOffset",
    "longGeneric", "shortGeneric"
};

const std::vector<std::string> ICU_NARROW_LONG_SHORT = {
    "narrow", "long", "short"
};

const std::vector<std::string> ICU2_DIGIT_NUMERIC = {
    "2-digit", "numeric"
};

const std::vector<std::string> ICU_NARROW_LONG_SHORT2_DIGIT_NUMERIC = {
    "narrow", "long", "short", "2-digit", "numeric"
};

const std::vector<std::pair<std::string, std::string>> ICU_WEEKDAY_PE = {
    { "EEEEE", "narrow" }, { "EEEE", "long" }, { "EEE", "short" },
    { "ccccc", "narrow" }, { "cccc", "long" }, { "ccc", "short" }
};

const std::vector<std::pair<std::string, std::string>> ICU_ERA_PE = {
    { "GGGGG", "narrow" },
    { "GGGG", "long" },
    { "GGG", "short" }
};

const std::vector<std::pair<std::string, std::string>> ICU_YEAR_PE = {
    { "yy", "2-digit" },
    { "y", "numeric" }
};

const std::vector<std::pair<std::string, std::string>> ICU_MONTH_PE = {
    { "MMMMM", "narrow" }, { "MMMM", "long" }, { "MMM", "short" }, { "MM", "2-digit" }, { "M", "numeric" },
    { "LLLLL", "narrow" }, { "LLLL", "long" }, { "LLL", "short" }, { "LL", "2-digit" }, { "L", "numeric" }
};

const std::vector<std::pair<std::string, std::string>> ICU_DAY_PE = {
    { "dd", "2-digit" },
    { "d", "numeric" }
};

const std::vector<std::pair<std::string, std::string>> ICU_DAY_PERIOD_PE = {
    { "BBBBB", "narrow" }, { "bbbbb", "narrow" }, { "BBBB", "long" },
    { "bbbb", "long" }, { "B", "short" }, { "b", "short" }
};

const std::vector<std::pair<std::string, std::string>> ICU_HOUR_PE = {
    { "HH", "2-digit" }, { "H", "numeric" }, { "hh", "2-digit" }, { "h", "numeric" },
    { "kk", "2-digit" }, { "k", "numeric" }, { "KK", "2-digit" }, { "K", "numeric" }
};

const std::vector<std::pair<std::string, std::string>> ICU_MINUTE_PE = {
    { "mm", "2-digit" },
    { "m", "numeric" }
};

const std::vector<std::pair<std::string, std::string>> ICU_SECOND_PE = {
    { "ss", "2-digit" },
    { "s", "numeric" }
};

const std::vector<std::pair<std::string, std::string>> ICU_TIME_ZONE_NAME_PE = {
    { "zzzz", "long" }, { "z", "short" },
    { "OOOO", "longOffset" }, { "O", "shortOffset" },
    { "vvvv", "longGeneric" }, { "v", "shortGeneric" }
};

IcuPatternDesc::IcuPatternDesc(std::string property, const std::vector<std::pair<std::string, std::string>> &pairs,
    std::vector<std::string> allowedValues) : property(std::move(property)), pairs(std::move(pairs)),
    allowedValues(std::move(allowedValues))
{
    for (const auto &pair : pairs) {
        propertyValueToPattern.emplace(pair.second, pair.first);
    }
}

Pattern::Pattern(const std::string& twoDigitPattern, const std::string& numericPattern)
{
    IcuPatternDesc hourPatternDesc(IntlDateTimeFormat::HOUR_TAG,
        { { twoDigitPattern, "2-digit" }, { numericPattern, "numeric" } }, { "2-digit", "numeric" });
    std::vector<IcuPatternDesc> items = Pattern::GetIcuPatternDescs();
    std::vector<IcuPatternDesc> result;
    for (auto& item : items) {
        if (item.property != IntlDateTimeFormat::HOUR_TAG) {
            result.emplace_back(item);
        } else {
            result.emplace_back(hourPatternDesc);
        }
    }
    data.assign(result.begin(), result.end());
}

std::vector<IcuPatternDesc> Pattern::GetData() const
{
    return data;
}

std::vector<IcuPatternDesc> Pattern::GetIcuPatternDescs()
{
    static const std::vector<IcuPatternDesc> icuPatternDescs = {
        IcuPatternDesc(IntlDateTimeFormat::WEEK_DAY_TAG, ICU_WEEKDAY_PE, ICU_NARROW_LONG_SHORT),
        IcuPatternDesc(IntlDateTimeFormat::ERA_TAG, ICU_ERA_PE, ICU_NARROW_LONG_SHORT),
        IcuPatternDesc(IntlDateTimeFormat::YEAR_TAG, ICU_YEAR_PE, ICU2_DIGIT_NUMERIC),
        IcuPatternDesc(IntlDateTimeFormat::MONTH_TAG, ICU_MONTH_PE, ICU_NARROW_LONG_SHORT2_DIGIT_NUMERIC),
        IcuPatternDesc(IntlDateTimeFormat::DAY_TAG, ICU_DAY_PE, ICU2_DIGIT_NUMERIC),
        IcuPatternDesc(IntlDateTimeFormat::DAY_PERIOD_TAG, ICU_DAY_PERIOD_PE, ICU_NARROW_LONG_SHORT),
        IcuPatternDesc(IntlDateTimeFormat::HOUR_TAG, ICU_HOUR_PE, ICU2_DIGIT_NUMERIC),
        IcuPatternDesc(IntlDateTimeFormat::MINUTE_TAG, ICU_MINUTE_PE, ICU2_DIGIT_NUMERIC),
        IcuPatternDesc(IntlDateTimeFormat::SECOND_TAG, ICU_SECOND_PE, ICU2_DIGIT_NUMERIC),
        IcuPatternDesc(IntlDateTimeFormat::TIME_ZONE_NAME_TAG, ICU_TIME_ZONE_NAME_PE, ICU_LONG_SHORT)
    };
    return icuPatternDescs;
}

IntlDateTimeFormat::IntlDateTimeFormat(const std::vector<std::string>& localeTags,
    const std::unordered_map<std::string, std::string>& configs, std::string& errMessage)
{
    I18nErrorCode status = I18nErrorCode::SUCCESS;
    std::vector<std::string> requestedLocales = LocaleHelper::CanonicalizeLocaleList(localeTags, status);
    if (status != I18nErrorCode::SUCCESS) {
        errMessage = "invalid locale";
        return;
    }
    if (!ParseConfigs(configs, errMessage)) {
        HILOG_ERROR_I18N("IntlDateTimeFormat::IntlDateTimeFormat: Parse configs failed.");
        return;
    }
    bool isValidLocale = false;
    for (size_t i = 0; i < requestedLocales.size(); i++) {
        std::string curLocale = requestedLocales[i];
        UErrorCode icuStatus = U_ZERO_ERROR;
        icuLocale = icu::Locale::forLanguageTag(icu::StringPiece(curLocale), icuStatus);
        if (U_FAILURE(icuStatus)) {
            HILOG_ERROR_I18N("IntlDateTimeFormat::IntlDateTimeFormat: Create icu::Locale failed, locale %{public}s.",
                curLocale.c_str());
            continue;
        }
        if (LocaleInfo::allValidLocales.count(icuLocale.getLanguage()) > 0) {
            isValidLocale = true;
            IntlDateTimeFormat::InitIcuLocale();
            break;
        }
    }
    if (!isValidLocale) {
        UErrorCode icuStatus = U_ZERO_ERROR;
        icuLocale = icu::Locale::forLanguageTag(icu::StringPiece("en-US"), icuStatus);
        if (U_FAILURE(icuStatus)) {
            HILOG_ERROR_I18N("IntlDateTimeFormat::IntlDateTimeFormat: Create en-US icu::Locale failed.");
            return;
        }
    }
    if (!IntlDateTimeFormat::InitIntlDateTimeFormat(configs, errMessage)) {
        HILOG_ERROR_I18N("IntlDateTimeFormat::IntlDateTimeFormat: Init IntlDateTimeFormat failed.");
        return;
    }
    initSuccess = true;
}

IntlDateTimeFormat::~IntlDateTimeFormat()
{
    if (icuDateTimePatternGenerator != nullptr) {
        delete icuDateTimePatternGenerator;
        icuDateTimePatternGenerator = nullptr;
    }

    if (dateIntervalFormat != nullptr) {
        delete dateIntervalFormat;
        dateIntervalFormat = nullptr;
    }
}

std::string IntlDateTimeFormat::Format(double milliseconds)
{
    if (!initSuccess || icuSimpleDateFormat == nullptr) {
        HILOG_ERROR_I18N("IntlDateTimeFormat::Format: Init failed.");
        return "";
    }
    milliseconds = LocaleHelper::TruncateDouble(milliseconds);
    icu::UnicodeString formatResult;
    icuSimpleDateFormat->format(milliseconds, formatResult);
    std::string result;
    formatResult.toUTF8String(result);
    return PseudoLocalizationProcessor(result);
}

std::vector<std::pair<std::string, std::string>> IntlDateTimeFormat::FormatToParts(double milliseconds,
    std::string& errMessage)
{
    if (!initSuccess || icuSimpleDateFormat == nullptr) {
        HILOG_ERROR_I18N("IntlDateTimeFormat::Format: Init failed.");
        return {};
    }
    icu::FieldPositionIterator fieldPositionIter;
    icu::UnicodeString formattedParts;
    UErrorCode status = U_ZERO_ERROR;
    icuSimpleDateFormat->format(milliseconds, formattedParts, &fieldPositionIter, status);
    if (U_FAILURE(status)) {
        errMessage = "format failed";
        return {};
    }

    int32_t index = 0;
    int32_t preEdgePos = 0;
    std::vector<CommonDateFormatPart> parts;
    icu::FieldPosition fieldPosition;
    while (fieldPositionIter.next(fieldPosition)) {
        int32_t fField = fieldPosition.getField();
        int32_t fBeginIndex = fieldPosition.getBeginIndex();
        int32_t fEndIndex = fieldPosition.getEndIndex();
        if (preEdgePos < fBeginIndex) {
            parts.emplace_back(CommonDateFormatPart(fField, preEdgePos, fBeginIndex, index, true));
            ++index;
        }
        parts.emplace_back(CommonDateFormatPart(fField, fBeginIndex, fEndIndex, index, false));
        preEdgePos = fEndIndex;
        ++index;
    }
    int32_t length = formattedParts.length();
    if (preEdgePos < length) {
        parts.emplace_back(CommonDateFormatPart(-1, preEdgePos, length, index, true));
    }
    std::vector<std::pair<std::string, std::string>> result(parts.size());
    for (size_t i = 0; i < parts.size(); ++i) {
        CommonDateFormatPart part = parts[i];
        std::string value;
        icu::UnicodeString tempString = formattedParts.tempSubStringBetween(part.fBeginIndex, part.fEndIndex);
        tempString.toUTF8String(value);
        std::string type;
        if (part.isPreExist) {
            type = DateTimeFormatPart::ConvertFieldIdToDateType(-1);
        } else {
            type = DateTimeFormatPart::ConvertFieldIdToDateType(part.fField);
        }
        result[i] = std::make_pair(type, value);
    }
    return result;
}

std::pair<std::string, std::vector<DateTimeFormatPart>> IntlDateTimeFormat::FormatToParts(double milliseconds)
{
    auto parts = DateTimeFormatPart::ParseToParts(icuSimpleDateFormat, milliseconds);
    std::string formatResult;
    parts.first.toUTF8String(formatResult);
    return std::pair(formatResult, parts.second);
}

std::string IntlDateTimeFormat::FormatRange(double start, double end, std::string& errMessage)
{
    if (!initSuccess) {
        HILOG_ERROR_I18N("IntlDateTimeFormat::FormatRange: Init failed.");
        return "";
    }
    if (dateIntervalFormat == nullptr) {
        if (!InitDateIntervalFormat()) {
            HILOG_ERROR_I18N("IntlDateTimeFormat::FormatRange: Init dateIntervalFormat failed.");
            return "";
        }
    }
    start = LocaleHelper::TruncateDouble(start);
    end = LocaleHelper::TruncateDouble(end);
    UErrorCode status = U_ZERO_ERROR;
    icu::DateInterval dateInterval(start, end);
    icu::FormattedDateInterval formatted = dateIntervalFormat->formatToValue(dateInterval, status);
    if (U_FAILURE(status)) {
        HILOG_ERROR_I18N("IntlDateTimeFormat::FormatRange: Format to value failed.");
        return "";
    }
    icu::UnicodeString formatResult = formatted.toString(status);
    if (U_FAILURE(status)) {
        errMessage = "format to string failed";
        return "";
    }
    bool outputRange = false;
    icu::ConstrainedFieldPosition cfpos;
    while (formatted.nextPosition(cfpos, status) != 0 && U_SUCCESS(status)) {
        if (cfpos.getCategory() == UFIELD_CATEGORY_DATE_INTERVAL_SPAN) {
            outputRange = true;
            break;
        }
    }

    if (!outputRange) {
        return Format(start);
    }
    std::string result;
    formatResult.toUTF8String(result);
    return PseudoLocalizationProcessor(result);
}

std::vector<std::pair<std::string, std::string>> IntlDateTimeFormat::FormatRangeToParts(double start,
    double end, std::string& errMessage)
{
    if (!initSuccess) {
        HILOG_ERROR_I18N("IntlDateTimeFormat::FormatRange: Init failed.");
        return {};
    }
    if (dateIntervalFormat == nullptr) {
        if (!InitDateIntervalFormat()) {
            HILOG_ERROR_I18N("IntlDateTimeFormat::FormatRange: Init dateIntervalFormat failed.");
            return {};
        }
    }
    start = LocaleHelper::TruncateDouble(start);
    end = LocaleHelper::TruncateDouble(end);
    UErrorCode status = U_ZERO_ERROR;
    icu::DateInterval dateInterval(start, end);
    icu::FormattedDateInterval formatted = dateIntervalFormat->formatToValue(dateInterval, status);
    if (U_FAILURE(status)) {
        HILOG_ERROR_I18N("IntlDateTimeFormat::FormatRange: Format to value failed.");
        return {};
    }
    icu::UnicodeString formattedParts;
    std::vector<CommonDateFormatPart> parts = CreateDateIntervalParts(formatted, formattedParts, errMessage);
    if (!errMessage.empty()) {
        return {};
    }
    std::vector<std::pair<std::string, std::string>> result(parts.size());
    for (size_t i = 0; i < parts.size(); ++i) {
        CommonDateFormatPart part = parts[i];
        std::string value;
        icu::UnicodeString tempString = formattedParts.tempSubStringBetween(part.fBeginIndex, part.fEndIndex);
        tempString.toUTF8String(value);
        std::string type;
        if (part.isPreExist) {
            type = DateTimeFormatPart::ConvertFieldIdToDateType(-1);
        } else {
            type = DateTimeFormatPart::ConvertFieldIdToDateType(part.fField);
        }
        result[i] = std::make_pair(type, value);
    }
    return result;
}

void IntlDateTimeFormat::ResolvedOptions(std::unordered_map<std::string, std::string>& configs)
{
    configs.clear();
    if (!localeString.empty()) {
        configs.insert(std::make_pair(LOCALE_TAG, localeString));
    }
    if (numberingSystem.empty()) {
        configs.insert(std::make_pair(NUMBERING_SYSTEM_TAG, DEFAULT_NUMBERING_SYSTEM));
    } else {
        configs.insert(std::make_pair(NUMBERING_SYSTEM_TAG, numberingSystem));
    }
    IntlDateTimeFormat::ResolvedCalendarAndTimeZone(configs);
    if (!hourCycle.empty()) {
        configs.insert(std::make_pair(HOUR_CYCLE_TAG, hourCycle));
        std::string hour12String = (hourCycle == HOUR_CYCLE_11 || hourCycle == HOUR_CYCLE_12) ?
            "true" : "false";
        configs.insert(std::make_pair(HOUR12_TAG, hour12String));
    }
    IntlDateTimeFormat::ResolvedDateTimeStyle(configs);
}

std::vector<std::string> IntlDateTimeFormat::SupportedLocalesOf(const std::vector<std::string>& requestLocales,
    const std::map<std::string, std::string>& configs, I18nErrorCode& status)
{
    std::vector<std::string> undefined = {};
    auto requestedLocales = LocaleHelper::CanonicalizeLocaleList(requestLocales, status);
    if (status != I18nErrorCode::SUCCESS) {
        HILOG_ERROR_I18N("IntlDateTimeFormat::SupportedLocalesOf: Call CanonicalizeLocaleList failed.");
        return undefined;
    }

    LocaleHelper::ParseOption(configs, "localeMatcher", "best fit", true, status);
    if (status != I18nErrorCode::SUCCESS) {
        return undefined;
    }
    std::set<std::string> availableLocales = LocaleInfo::GetValidLocales();
    return LocaleHelper::LookupSupportedLocales(availableLocales, requestedLocales);
}

bool IntlDateTimeFormat::ParseConfigs(const std::unordered_map<std::string, std::string>& configs,
    std::string& errMessage)
{
    auto iter = configs.find(IntlDateTimeFormat::LOCALE_MATCHER_TAG);
    if (iter != configs.end()) {
        if (!LocaleHelper::IsValidOptionName(IntlDateTimeFormat::LOCALE_MATCHER_TAG, iter->second)) {
            errMessage = "getStringOption failed";
            return false;
        }
        localeMatcher = iter->second;
    }

    iter = configs.find(IntlDateTimeFormat::CALENDAR_TAG);
    if (iter != configs.end()) {
        calendar = iter->second;
        if (!LocaleHelper::IsNormativeCalendar(calendar)) {
            errMessage = "invalid calendar";
            return false;
        }
    }

    iter = configs.find(IntlDateTimeFormat::NUMBERING_SYSTEM_TAG);
    if (iter != configs.end()) {
        numberingSystem = iter->second;
        if (!LocaleHelper::IsNormativeNumberingSystem(numberingSystem)) {
            errMessage = "invalid numberingSystem";
            return false;
        }
    }

    iter = configs.find(IntlDateTimeFormat::HOUR12_TAG);
    if (iter != configs.end()) {
        hour12 = iter->second;
    }

    iter = configs.find(IntlDateTimeFormat::HOUR_CYCLE_TAG);
    if (iter != configs.end()) {
        if (!LocaleHelper::IsValidOptionName(IntlDateTimeFormat::HOUR_CYCLE_TAG, iter->second)) {
            errMessage = "getStringOption failed";
            return false;
        }
        if (hour12.empty()) {
            hourCycle = iter->second;
        }
    }

    return true;
}

bool IntlDateTimeFormat::InitIntlDateTimeFormat(const std::unordered_map<std::string, std::string>& configs,
    std::string& errMessage)
{
    if (!IntlDateTimeFormat::InitIcuTimeZone(configs, errMessage)) {
        HILOG_ERROR_I18N("IntlDateTimeFormat::InitIntlDateTimeFormat: Init icuTimeZone failed.");
        return false;
    }
    if (!IntlDateTimeFormat::InitDateTimePatternGenerator(errMessage)) {
        HILOG_ERROR_I18N("IntlDateTimeFormat::InitIntlDateTimeFormat: Init icuDateTimePatternGenerator failed.");
        return false;
    }
    std::string effectiveHourCycle;
    if (!IntlDateTimeFormat::InitHourCycle(effectiveHourCycle)) {
        HILOG_ERROR_I18N("IntlDateTimeFormat::InitIntlDateTimeFormat: Init hour cycle failed.");
        return false;
    }
    bool isHourDefined = false;
    bool isExistComponents = false;
    if (!IntlDateTimeFormat::InitSkeleton(configs, effectiveHourCycle, isHourDefined, isExistComponents, errMessage)) {
        HILOG_ERROR_I18N("IntlDateTimeFormat::InitIntlDateTimeFormat: Init skeleton failed.");
        return false;
    }
    if (!IntlDateTimeFormat::InitDateTimeStyle(configs, effectiveHourCycle, isHourDefined, errMessage)) {
        HILOG_ERROR_I18N("IntlDateTimeFormat::InitIntlDateTimeFormat: Init date time style failed.");
        return false;
    }
    if (!IntlDateTimeFormat::InitIcuSimpleDateFormat(isExistComponents, errMessage)) {
        HILOG_ERROR_I18N("IntlDateTimeFormat::InitIntlDateTimeFormat: Init icuSimpleDateFormat failed.");
        return false;
    }
    return true;
}

void IntlDateTimeFormat::InitIcuLocale()
{
    if (!calendar.empty()) {
        if (!uloc_toLegacyType(uloc_toLegacyKey("ca"), calendar.c_str())) {
            HILOG_ERROR_I18N("IntlDateTimeFormat::InitIcuLocale: Invalid calendar %{public}s.", calendar.c_str());
        } else {
            UErrorCode icuStatus = U_ZERO_ERROR;
            icuLocale.setUnicodeKeywordValue("ca", calendar, icuStatus);
            if (U_FAILURE(icuStatus)) {
                HILOG_ERROR_I18N("IntlDateTimeFormat::InitIcuLocale: Set icuLocale calendar failed.");
            }
        }
    }

    if (!numberingSystem.empty()) {
        if (!uloc_toLegacyType(uloc_toLegacyKey("nu"), numberingSystem.c_str())) {
            HILOG_ERROR_I18N("IntlDateTimeFormat::InitIcuLocale: Invalid numberingSystem %{public}s.",
                numberingSystem.c_str());
        } else {
            UErrorCode icuStatus = U_ZERO_ERROR;
            icuLocale.setUnicodeKeywordValue("nu", numberingSystem, icuStatus);
            if (U_FAILURE(icuStatus)) {
                HILOG_ERROR_I18N("IntlDateTimeFormat::InitIcuLocale: Set icuLocale calendar failed.");
            }
        }
    }
}

bool IntlDateTimeFormat::InitIcuTimeZone(const std::unordered_map<std::string, std::string>& configs,
    std::string& errMessage)
{
    auto it = configs.find(IntlDateTimeFormat::TIME_ZONE_TAG);
    if (it != configs.end()) {
        timeZone = it->second;
    }

    if (timeZone.empty()) {
        icuTimeZone = icu::TimeZone::createDefault();
        if (icuTimeZone == nullptr) {
            HILOG_ERROR_I18N("IntlDateTimeFormat::InitIcuTimeZone: Create default time zone.");
            return false;
        }
    } else {
        icuTimeZone = icu::TimeZone::createTimeZone(timeZone.c_str());
        if (icuTimeZone == nullptr || !IsValidTimeZoneName(*icuTimeZone)) {
            errMessage = "invalid timeZone";
            return false;
        }
    }
    return true;
}

bool IntlDateTimeFormat::InitDateTimePatternGenerator(std::string& errMessage)
{
    UErrorCode icuStatus = U_ZERO_ERROR;
    icuDateTimePatternGenerator = icu::DateTimePatternGenerator::createInstance(icuLocale, icuStatus);
    if (U_FAILURE(icuStatus) || icuDateTimePatternGenerator == nullptr) {
        if (icuStatus == UErrorCode::U_MISSING_RESOURCE_ERROR) {
            errMessage = "can not find icu data resources";
        } else {
            errMessage = "create icu::DateTimePatternGenerator failed";
        }
        return false;
    }
    return true;
}

bool IntlDateTimeFormat::InitHourCycle(std::string& effectiveHourCycle)
{
    if (icuDateTimePatternGenerator == nullptr) {
        HILOG_ERROR_I18N("IntlDateTimeFormat::InitHourCycle: icuDateTimePatternGenerator is nullptr.");
        return false;
    }
    UErrorCode icuStatus = U_ZERO_ERROR;
    std::string defaultHourCycle =
        ConvertUDateFormatHourCycleToString(icuDateTimePatternGenerator->getDefaultHourCycle(icuStatus));
    if (U_FAILURE(icuStatus)) {
        HILOG_ERROR_I18N("IntlDateTimeFormat::InitHourCycle: Get default hour cycle failed.");
        return false;
    }
    effectiveHourCycle = hourCycle;
    if (hourCycle.empty()) {
        effectiveHourCycle = defaultHourCycle;
    }
    if (!hour12.empty()) {
        if (hour12.compare("true") == 0) {
            if (defaultHourCycle.compare(HOUR_CYCLE_11) == 0 || defaultHourCycle.compare(HOUR_CYCLE_23) == 0) {
                effectiveHourCycle = HOUR_CYCLE_11;
            } else {
                effectiveHourCycle = HOUR_CYCLE_12;
            }
        } else {
            if (defaultHourCycle.compare(HOUR_CYCLE_11) == 0 || defaultHourCycle.compare(HOUR_CYCLE_23) == 0) {
                effectiveHourCycle = HOUR_CYCLE_23;
            } else {
                effectiveHourCycle = HOUR_CYCLE_24;
            }
        }
    }
    return true;
}

bool IntlDateTimeFormat::InitSkeleton(const std::unordered_map<std::string, std::string>& configs,
    const std::string& effectiveHourCycle, bool& isHourDefined, bool& isExistComponents, std::string& errMessage)
{
    std::vector<IcuPatternDesc> data = GetIcuPatternDesc(effectiveHourCycle);
    skeletonOpts.clear();

    for (const IcuPatternDesc &item : data) {
        if (item.property.compare(TIME_ZONE_NAME_TAG) == 0) {
            int32_t secondDigitsString = GetFractionalSecondDigit(configs);
            skeleton.append(secondDigitsString, 'S');
            if (secondDigitsString > 0) {
                isExistComponents = true;
                skeletonOpts.emplace_back(item.property);
            }
        }

        auto iter = configs.find(item.property);
        if (iter != configs.end()) {
            std::string value = iter->second;
            if (std::find(item.allowedValues.begin(), item.allowedValues.end(), value) == item.allowedValues.end()) {
                errMessage = "Value out of range for locale options property";
                return false;
            }
            skeletonOpts.emplace_back(item.property);
            isExistComponents = true;
            auto iterPattern = item.propertyValueToPattern.find(value);
            if (iterPattern != item.propertyValueToPattern.end()) {
                skeleton += iterPattern->second;
            }
            isHourDefined = (item.property.compare(HOUR_TAG) == 0) ? true : isHourDefined;
        }
    }

    return true;
}

std::vector<IcuPatternDesc> IntlDateTimeFormat::GetIcuPatternDesc(const std::string& effectiveHourCycle)
{
    auto iter = HOUR_CYCLE_TO_PATTERN.find(effectiveHourCycle);
    if (iter == HOUR_CYCLE_TO_PATTERN.end()) {
        Pattern pattern("jj", "j");
        return pattern.GetData();
    }
    Pattern pattern(iter->second.first, iter->second.second);
    return pattern.GetData();
}

int32_t IntlDateTimeFormat::GetFractionalSecondDigit(const std::unordered_map<std::string, std::string>& configs)
{
    auto iter = configs.find(IntlDateTimeFormat::FRACTIONAL_SECOND_DIGITS_TAG);
    if (iter != configs.end()) {
        fractionalSecondDigits = iter->second;
    }
    int32_t result = 0;
    if (!fractionalSecondDigits.empty()) {
        int32_t status = 0;
        int32_t temp = ConvertString2Int(fractionalSecondDigits, status);
        if (status == 0) {
            result = temp;
        }
    }
    if (result < MIN_VALUE_OF_FRACTIONAL_SECOND_DIGITS || result > MAX_VALUE_OF_FRACTIONAL_SECOND_DIGITS) {
        return 0;
    }
    return result;
}

bool IntlDateTimeFormat::InitDateTimeStyle(const std::unordered_map<std::string, std::string>& configs,
    const std::string& effectiveHourCycle, bool isHourDefined, std::string& errMessage)
{
    auto iter = configs.find(FORMAT_MATCHER_TAG);
    if (iter != configs.end()) {
        if (FORMAT_MATCHER_VALUE.find(iter->second) == FORMAT_MATCHER_VALUE.end()) {
            errMessage = "Value out of range for locale options property";
            return false;
        } else {
            formatMatcher = iter->second;
        }
    }

    iter = configs.find(DATE_STYLE_TAG);
    if (iter != configs.end()) {
        if (DATE_TIME_STYLE_VALUE.find(iter->second) == DATE_TIME_STYLE_VALUE.end()) {
            errMessage = "Value out of range for locale options property";
            return false;
        } else {
            dateStyle = iter->second;
        }
    }

    iter = configs.find(TIME_STYLE_TAG);
    if (iter != configs.end()) {
        if (DATE_TIME_STYLE_VALUE.find(iter->second) == DATE_TIME_STYLE_VALUE.end()) {
            errMessage = "Value out of range for locale options property";
            return false;
        } else {
            timeStyle = iter->second;
        }
    }
    std::string tempHourCycle;
    if (!timeStyle.empty()) {
        tempHourCycle = effectiveHourCycle;
    }
    if (dateStyle.empty() && timeStyle.empty()) {
        IntlDateTimeFormat::InitDateTimeSkeleton(skeletonOpts, skeleton, effectiveHourCycle);
        if (isHourDefined) {
            tempHourCycle = effectiveHourCycle;
        }
    }
    hourCycle = tempHourCycle;
    return true;
}

bool IntlDateTimeFormat::InitIcuSimpleDateFormat(bool isExistComponents, std::string& errMessage)
{
    if (icuDateTimePatternGenerator == nullptr) {
        HILOG_ERROR_I18N("IntlDateTimeFormat::InitIcuSimpleDateFormat: icuDateTimePatternGenerator is nullptr.");
        return false;
    }
    icu::UnicodeString dtfSkeleton(skeleton.c_str());
    UErrorCode icuStatus = U_ZERO_ERROR;
    icu::UnicodeString bestPattern = icuDateTimePatternGenerator->getBestPattern(dtfSkeleton,
        UDATPG_MATCH_HOUR_FIELD_LENGTH, icuStatus);
    if (U_FAILURE(icuStatus)) {
        HILOG_ERROR_I18N("IntlDateTimeFormat::InitIcuSimpleDateFormat: Get best pattern failed.");
        return false;
    }
    icu::UnicodeString pattern = IntlDateTimeFormat::ChangeHourCyclePattern(bestPattern, hourCycle);
    icuSimpleDateFormat = std::make_shared<icu::SimpleDateFormat>(pattern, icuLocale, icuStatus);
    if (!dateStyle.empty() || !timeStyle.empty()) {
        if (isExistComponents) {
            errMessage = "Invalid option : option";
            return false;
        }
        icuSimpleDateFormat = IntlDateTimeFormat::GetSimpleDateFormatFromStyle();
    }
    if (U_FAILURE(icuStatus)) {
        icuSimpleDateFormat = std::shared_ptr<icu::SimpleDateFormat>();
    }
    if (IntlDateTimeFormat::InitIcuCalendar() && icuSimpleDateFormat != nullptr) {
        icuSimpleDateFormat->adoptCalendar(icuCalendar);
    }
    std::string icuLocaleName = icuLocale.getName();
    if (icuLocaleName.find("calendar=iso8601") != std::string::npos) {
        isIso8601 = true;
    }
    localeString = icuLocale.toLanguageTag<std::string>(icuStatus);
    if (U_FAILURE(icuStatus)) {
        HILOG_ERROR_I18N("IntlDateTimeFormat::InitIcuSimpleDateFormat: Get languageTag from icuLocale failed.");
        return false;
    }
    return true;
}

void IntlDateTimeFormat::InitDateTimeSkeleton(const std::vector<std::string> &options,
    std::string &skeleton, const std::string& hc)
{
    bool needDefaults = true;

    for (const auto& element : DATE_TIME_ELEMENT) {
        if (std::find(options.begin(), options.end(), element) != options.end()) {
            needDefaults = false;
            break;
        }
    }

    if (needDefaults) {
        skeleton += "yMd";
    }
}

icu::UnicodeString IntlDateTimeFormat::ChangeHourCyclePattern(const icu::UnicodeString &pattern, const std::string& hc)
{
    if (hc.empty()) {
        return pattern;
    }
    icu::UnicodeString result;
    char16_t key =  u'\0';
    auto iter = std::find_if(HOUR_CYCLE_MAP.begin(), HOUR_CYCLE_MAP.end(),
        [hc](const std::map<char16_t, std::string>::value_type item) {
            return item.second == hc;
    });
    if (iter != HOUR_CYCLE_MAP.end()) {
        key = iter->first;
    }

    bool needChange = true;
    char16_t last = u'\0';
    for (int32_t i = 0; i < pattern.length(); i++) {
        char16_t ch = pattern.charAt(i);
        if (ch == '\'') {
            needChange = !needChange;
            result.append(ch);
        } else if (HOUR_CYCLE_MAP.find(ch) != HOUR_CYCLE_MAP.end()) {
            result = (needChange && last == u'd') ? result.append(' ') : result;
            result.append(needChange ? key : ch);
        } else {
            result.append(ch);
        }
        last = ch;
    }
    return result;
}

std::shared_ptr<icu::SimpleDateFormat> IntlDateTimeFormat::GetSimpleDateFormatFromStyle()
{
    std::shared_ptr<icu::SimpleDateFormat> result;
    icu::DateFormat::EStyle icuDateStyle = GetEStyleFromDateTimeStyle(dateStyle);
    icu::DateFormat::EStyle icuTimeStyle = GetEStyleFromDateTimeStyle(timeStyle);
    result.reset(reinterpret_cast<icu::SimpleDateFormat *>(
        icu::DateFormat::createDateTimeInstance(icuDateStyle, icuTimeStyle, icuLocale)));
    UErrorCode status = U_ZERO_ERROR;
    icu::UnicodeString pattern("");
    if (result == nullptr) {
        return nullptr;
    }
    pattern = result->toPattern(pattern);
    icu::UnicodeString skeleton = icu::DateTimePatternGenerator::staticGetSkeleton(pattern, status);
    if (U_FAILURE(status)) {
        HILOG_ERROR_I18N("IntlDateTimeFormat::GetSimpleDateFormatFromStyle: Get skeleton failed.");
        return nullptr;
    }
    if (hourCycle == GetHourCycleFromPattern(pattern)) {
        return result;
    }
    skeleton = ReplaceHourCycleOfSkeleton(skeleton, hourCycle);
    if (icuDateTimePatternGenerator == nullptr) {
        HILOG_ERROR_I18N("IntlDateTimeFormat::GetSimpleDateFormatFromStyle: icuDateTimePatternGenerator is nullptr.");
        return nullptr;
    }
    pattern = icuDateTimePatternGenerator->getBestPattern(skeleton, UDATPG_MATCH_HOUR_FIELD_LENGTH, status);
    if (U_FAILURE(status)) {
        HILOG_ERROR_I18N("IntlDateTimeFormat::GetSimpleDateFormatFromStyle: Get best pattern failed.");
        return nullptr;
    }
    result = std::make_shared<icu::SimpleDateFormat>(pattern, icuLocale, status);
    if (U_FAILURE(status)) {
        HILOG_ERROR_I18N("IntlDateTimeFormat::GetSimpleDateFormatFromStyle: Create SimpleDateFormat failed.");
        return nullptr;
    }
    return result;
}

icu::DateFormat::EStyle IntlDateTimeFormat::GetEStyleFromDateTimeStyle(const std::string& style)
{
    auto iter = DATE_TIME_STYLE_TO_ESTYLE.find(style);
    if (iter == DATE_TIME_STYLE_TO_ESTYLE.end()) {
        return icu::DateFormat::kNone;
    }
    return iter->second;
}

std::string IntlDateTimeFormat::GetHourCycleFromPattern(const icu::UnicodeString& pattern)
{
    bool inQuote = false;
    for (int32_t i = 0; i < pattern.length(); i++) {
        char16_t ch = pattern[i];
        if (ch == '\'') {
            inQuote = !inQuote;
            continue;
        }
        auto iter = HOUR_CYCLE_MAP.find(ch);
        if (iter == HOUR_CYCLE_MAP.end() || inQuote) {
            continue;
        }
        return iter->second;
    }
    return "";
}

icu::UnicodeString IntlDateTimeFormat::ReplaceHourCycleOfSkeleton(const icu::UnicodeString& skeleton,
    const std::string& hc)
{
    icu::UnicodeString result;

    auto iter = std::find_if(HOUR_CYCLE_MAP.begin(), HOUR_CYCLE_MAP.end(),
        [hc](const std::map<char16_t, std::string>::value_type item) {
            return item.second == hc;
    });
    if (iter == HOUR_CYCLE_MAP.end()) {
        return skeleton;
    }
    char16_t convertChar = iter->first;

    int skeletonLength = skeleton.length();
    for (int32_t i = 0; i < skeletonLength; ++i) {
        if (skeleton[i] == 'a' || skeleton[i] == 'b' || skeleton[i] == 'B') {
            continue;
        }
        if (HOUR_CYCLE_MAP.find(skeleton[i]) != HOUR_CYCLE_MAP.end()) {
            result += convertChar;
            continue;
        }
        result += skeleton[i];
    }
    return result;
}

bool IntlDateTimeFormat::InitIcuCalendar()
{
    if (icuTimeZone == nullptr) {
        HILOG_ERROR_I18N("IntlDateTimeFormat::InitIcuCalendar: icuTimeZone is nullptr.");
        return false;
    }
    UErrorCode icuStatus = U_ZERO_ERROR;
    icuCalendar = icu::Calendar::createInstance(icuTimeZone, icuLocale, icuStatus);
    if (U_FAILURE(icuStatus) || icuCalendar == nullptr) {
        HILOG_ERROR_I18N("IntlDateTimeFormat::InitIcuCalendar: Create icuCalendar failed.");
        return false;
    }

    if (icuCalendar->getDynamicClassID() == icu::GregorianCalendar::getStaticClassID()) {
        auto gregorianCalendar = static_cast<icu::GregorianCalendar *>(icuCalendar);
        // ECMAScript start time, value = -(2**53)
        const double beginTime = -9007199254740992;
        if (gregorianCalendar == nullptr) {
            HILOG_ERROR_I18N("IntlDateTimeFormat::InitIcuCalendar: Static cast failed.");
            return false;
        }
        gregorianCalendar->setGregorianChange(beginTime, icuStatus);
        if (U_FAILURE(icuStatus)) {
            HILOG_ERROR_I18N("IntlDateTimeFormat::InitIcuCalendar: Set gregorian change failed.");
            return false;
        }
    }
    return true;
}

bool IntlDateTimeFormat::InitDateIntervalFormat()
{
    if (icuSimpleDateFormat == nullptr) {
        HILOG_ERROR_I18N("IntlDateTimeFormat::InitDateIntervalFormat: icuSimpleDateFormat is nullptr.");
        return false;
    }
    icu::UnicodeString pattern;
    icuSimpleDateFormat->toPattern(pattern);
    UErrorCode status = U_ZERO_ERROR;
    icu::UnicodeString skeleton = icu::DateTimePatternGenerator::staticGetSkeleton(pattern, status);
    if (U_FAILURE(status)) {
        HILOG_ERROR_I18N("IntlDateTimeFormat::InitDateIntervalFormat: Get skeleton failed.");
        return false;
    }

    dateIntervalFormat = icu::DateIntervalFormat::createInstance(skeleton, icuLocale, status);
    if (U_FAILURE(status) || dateIntervalFormat == nullptr) {
        HILOG_ERROR_I18N("IntlDateTimeFormat::InitDateIntervalFormat: Create dateIntervalFormat failed.");
        return false;
    }
    dateIntervalFormat->setTimeZone(*icuTimeZone);
    return true;
}

std::string IntlDateTimeFormat::ConvertUDateFormatHourCycleToString(UDateFormatHourCycle hc)
{
    auto iter = UDATE_FORMAT_HOUR_CYCLE_TO_STRING.find(hc);
    if (iter == UDATE_FORMAT_HOUR_CYCLE_TO_STRING.end()) {
        HILOG_ERROR_I18N("IntlDateTimeFormat::ConvertUDateFormatHourCycleToString: Invalid hourCycle.");
        return "";
    }
    return iter->second;
}

std::vector<CommonDateFormatPart> IntlDateTimeFormat::CreateDateIntervalParts(
    const icu::FormattedDateInterval& formatted, icu::UnicodeString& formattedValue, std::string& errMessage)
{
    UErrorCode status = U_ZERO_ERROR;
    formattedValue = formatted.toTempString(status);
    if (U_FAILURE(status)) {
        HILOG_ERROR_I18N("IntlDateTimeFormat::CreateDateIntervalParts: Get formatted value failed.");
        return {};
    }
    int32_t index = 0;
    int32_t preEndPos = 0;
    std::vector<int32_t> begin(NUMBER_OF_PARAMETER, 0);
    std::vector<int32_t> end(NUMBER_OF_PARAMETER, 0);
    std::vector<CommonDateFormatPart> parts;
    icu::ConstrainedFieldPosition cfpos;
    while (formatted.nextPosition(cfpos, status) && U_SUCCESS(status)) {
        int32_t fCategory = cfpos.getCategory();
        int32_t fField = cfpos.getField();
        int32_t fStart = cfpos.getStart();
        int32_t fLimit = cfpos.getLimit();

        if (fCategory == UFIELD_CATEGORY_DATE_INTERVAL_SPAN && (fField == 0 || fField == 1)) {
            begin[fField] = fStart;
            end[fField] = fLimit;
        }
        if (fCategory == UFIELD_CATEGORY_DATE) {
            if (preEndPos < fStart) {
                parts.emplace_back(CommonDateFormatPart(fField, preEndPos, fStart, index, true));
                index++;
            }
            parts.emplace_back(CommonDateFormatPart(fField, fStart, fLimit, index, false));
            preEndPos = fLimit;
            ++index;
        }
    }
    if (U_FAILURE(status)) {
        errMessage = "format date interval error";
        return {};
    }
    int32_t length = formattedValue.length();
    if (length > preEndPos) {
        parts.emplace_back(CommonDateFormatPart(-1, preEndPos, length, index, true));
    }
    return parts;
}

void IntlDateTimeFormat::ResolvedCalendarAndTimeZone(std::unordered_map<std::string, std::string>& configs)
{
    if (icuCalendar == nullptr) {
        HILOG_ERROR_I18N("IntlDateTimeFormat::ResolvedCalendarAndTimeZone: icuCalendar is nullptr.");
        return;
    }

    std::string calendarString;
    std::string calendarType = icuCalendar->getType();
    if (calendarType.compare("gregorian") == 0) {
        if (isIso8601) {
            calendarString = "iso8601";
        } else {
            calendarString = "gregory";
        }
    } else if (calendarType.compare("ethiopic-amete-alem") == 0) {
        calendarString = "ethioaa";
    } else if (!calendarType.empty()) {
        calendarString = calendarType;
    }
    configs.insert(std::make_pair(CALENDAR_TAG, calendarString));

    const icu::TimeZone &icuTZ = icuCalendar->getTimeZone();
    icu::UnicodeString timezone;
    icuTZ.getID(timezone);
    UErrorCode status = U_ZERO_ERROR;
    icu::UnicodeString canonicalTimezone;
    icu::TimeZone::getCanonicalID(timezone, canonicalTimezone, status);
    if (U_FAILURE(status)) {
        HILOG_ERROR_I18N("IntlDateTimeFormat::ResolvedCalendarAndTimeZone: Get canonical Id failed.");
        return;
    }
    std::string timezoneValue;
    if (canonicalTimezone == UNICODE_STRING_SIMPLE("Etc/UTC") ||
        canonicalTimezone == UNICODE_STRING_SIMPLE("Etc/GMT")) {
        timezoneValue = "UTC";
    } else {
        canonicalTimezone.toUTF8String(timezoneValue);
    }
    configs.insert(std::make_pair(TIME_ZONE_TAG, timezoneValue));
}

void IntlDateTimeFormat::ResolvedDateTimeStyle(std::unordered_map<std::string, std::string>& configs)
{
    if (dateStyle.empty() && timeStyle.empty() && icuSimpleDateFormat != nullptr) {
        icu::UnicodeString patternUnicode;
        icuSimpleDateFormat->toPattern(patternUnicode);
        std::string pattern;
        patternUnicode.toUTF8String(pattern);
        for (const auto &item : Pattern::GetIcuPatternDescs()) {
            // fractionalSecondsDigits need to be added before timeZoneName.
            if (item.property == TIME_ZONE_NAME_TAG) {
                int tmpResult = count(pattern.begin(), pattern.end(), 'S');
                int fsd = (tmpResult >= MAX_VALUE_OF_FRACTIONAL_SECOND_DIGITS) ?
                    MAX_VALUE_OF_FRACTIONAL_SECOND_DIGITS : tmpResult;
                if (fsd > 0) {
                    std::string fsdString = std::to_string(fsd);
                    configs.insert(std::make_pair(FRACTIONAL_SECOND_DIGITS_TAG, fsdString));
                }
            }
            for (const auto &pair : item.pairs) {
                if (pattern.find(pair.first) != std::string::npos) {
                    configs.insert(std::make_pair(item.property, pair.second));
                    break;
                }
            }
        }
    }
    if (!dateStyle.empty()) {
        configs.insert(std::make_pair(DATE_STYLE_TAG, dateStyle));
    }
    if (!timeStyle.empty()) {
        configs.insert(std::make_pair(TIME_STYLE_TAG, timeStyle));
    }
}

bool IntlDateTimeFormat::IsValidTimeZoneName(const icu::TimeZone &tz)
{
    UErrorCode status = U_ZERO_ERROR;
    icu::UnicodeString id;
    tz.getID(id);
    icu::UnicodeString canonical;
    icu::TimeZone::getCanonicalID(id, canonical, status);
    bool canonicalFlag = (canonical != icu::UnicodeString("Etc/Unknown", -1, US_INV));
    return U_SUCCESS(status) && canonicalFlag;
}

bool IntlDateTimeFormat::icuInitialized = IntlDateTimeFormat::Init();

bool IntlDateTimeFormat::Init()
{
    SetHwIcuDirectory();
    return true;
}
} // namespace I18n
} // namespace Global
} // namespace OHOS
