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
#include "date_time_format.h"
#include "i18n_hilog.h"
#include "locale_config.h"
#include "ohos/init_data.h"
#include "parameter.h"
#include "utils.h"

namespace OHOS {
namespace Global {
namespace I18n {
const char* DateTimeFormat::DEVICE_TYPE_NAME = "const.product.devicetype";
static const std::string NUMERIC_STR = "numeric";
static const icu::UnicodeString YMD_SKELETON = "yyyyMd";
static const icu::UnicodeString YM_SKELETON = "yyyyM";
static const icu::UnicodeString MD_SKELETON = "Md";
static const icu::UnicodeString D_SKELETON = "d";
static const icu::UnicodeString DD_SKELETON = "dd";
static const icu::UnicodeString LTR_SYMBOL = "\u200e";
static const icu::UnicodeString RTL_SYMBOL = "\u200f";
static const size_t YMD_INDEX = 0;
static const size_t YM_INDEX = 1;
static const size_t MD_INDEX = 2;
static const double DEFAULT_TIME = 1752768000000;

using namespace icu;
bool DateTimeFormat::icuInitialized = DateTimeFormat::Init();

std::map<std::string, DateFormat::EStyle> DateTimeFormat::dateTimeStyle = {
    { "full", DateFormat::EStyle::kFull },
    { "long", DateFormat::EStyle::kLong },
    { "medium", DateFormat::EStyle::kMedium },
    { "short", DateFormat::EStyle::kShort }
};

std::unordered_map<std::string, DateTimeFormat::DefaultStyle> DateTimeFormat::DeviceToStyle = {
    { "tablet", DefaultStyle::LONG },
    { "2in1", DefaultStyle::LONG },
    { "tv", DefaultStyle::LONG },
    { "pc", DefaultStyle::LONG },
    { "liteWearable", DefaultStyle::SHORT },
    { "wearable", DefaultStyle::SHORT },
    { "watch", DefaultStyle::SHORT }
};

std::unordered_map<DateTimeFormat::DefaultStyle, DateFormat::EStyle> DateTimeFormat::DefaultDTStyle = {
    { DefaultStyle::LONG, DateFormat::EStyle::kMedium },
    { DefaultStyle::DEFAULT, DateFormat::EStyle::kShort },
    { DefaultStyle::SHORT, DateFormat::EStyle::kShort },
};

std::map<icu::UnicodeString, std::string> PATTERN_TO_EXT_PARAM = {
    { "d/MM/y", "0001" },
    { "d/M/yy", "0002" },
    { "y-MM-d", "0003" },
    { "d/M/y", "0004" },
    { "d/MM/yy", "0005" },
    { "d.MM.yy", "0006" },
    { "yy/MM/d", "0007" },
    { "d.MM.y", "0008" },
    { "d-MM-y", "0009" },
    { "y/M/d", "0010" },
    { "d-M-y", "0011" },
};

std::unordered_map<std::string, std::vector<icu::UnicodeString>> EXT_PARAM_TO_PATTERNS = {
    { "0001", { "d/MM/y", "MM/y", "d/MM" } },
    { "0002", { "d/M/yy", "M/yy", "d/M" } },
    { "0003", { "y-MM-d", "y-MM", "MM-d" } },
    { "0004", { "d/M/y", "M/y", "d/M" } },
    { "0005", { "d/MM/yy", "MM/yy", "d/MM" } },
    { "0006", { "d.MM.yy", "MM.yy", "d.MM" } },
    { "0007", { "yy/MM/d", "yy/MM", "MM/d" } },
    { "0008", { "d.MM.y", "MM.y", "d.MM" } },
    { "0009", { "d-MM-y", "MM-y", "d-MM" } },
    { "0010", { "y/M/d", "y/M", "M/d" } },
    { "0011", { "d-M-y", "M-y", "d-M" } },
};

DateTimeFormat::DateTimeFormat(const std::vector<std::string> &localeTags, std::map<std::string, std::string> &configs)
{
    defaultStyle = GetDefaultStyle();
    ParseConfigsPartOne(configs);
    ParseConfigsPartTwo(configs);
    for (size_t i = 0; i < localeTags.size(); i++) {
        std::string curLocale = localeTags[i];
        UErrorCode status = U_ZERO_ERROR;
        locale = Locale::forLanguageTag(StringPiece(curLocale), status);
        if (U_FAILURE(status)) {
            continue;
        }
        if (LocaleInfo::allValidLocales.count(locale.getLanguage()) > 0) {
            finalLocale = curLocale;
            break;
        }
    }
    if (finalLocale.empty()) {
        finalLocale = LocaleConfig::GetEffectiveLocale();
    }
    std::string localeWithoutCustExtParam = LocaleConfig::RemoveCustExtParam(finalLocale);
    createSuccess = InitWithLocale(localeWithoutCustExtParam, configs);
    if (!createSuccess) {
        HILOG_ERROR_I18N("DateTimeFormat::DateTimeFormat: Init dateTimeFormat failed.");
    }
}

DateTimeFormat::~DateTimeFormat()
{
}

bool DateTimeFormat::CheckInitSuccess()
{
    return (dateIntvFormat == nullptr || calendar == nullptr || dateFormat == nullptr || localeInfo == nullptr) ?
        false : true;
}

std::unique_ptr<DateTimeFormat> DateTimeFormat::CreateInstance(const std::vector<std::string> &localeTags,
                                                               std::map<std::string, std::string> &configs)
{
    std::unique_ptr<DateTimeFormat> dateTimeFormat = std::make_unique<DateTimeFormat>(localeTags, configs);
    if (dateTimeFormat == nullptr || !dateTimeFormat->CheckInitSuccess()) {
        return nullptr;
    }
    return dateTimeFormat;
}

bool DateTimeFormat::InitWithLocale(const std::string &curLocale, std::map<std::string, std::string> &configs)
{
    localeInfo = std::make_unique<LocaleInfo>(curLocale, configs);
    if (localeInfo == nullptr || !localeInfo->InitSuccess()) {
        return false;
    }
    locale = localeInfo->GetLocale();
    localeTag = localeInfo->GetBaseName();
    if (hourCycle.empty()) {
        hourCycle = localeInfo->GetHourCycle();
    }
    if (hour12.empty() && hourCycle.empty() && !LocaleConfig::IsEmpty24HourClock()) {
        bool is24HourClock = LocaleConfig::Is24HourClock();
        hour12 = is24HourClock ? "false" : "true";
    }
    ComputeHourCycleChars();
    ComputeSkeleton();
    UErrorCode status = U_ZERO_ERROR;
    if (!configs.size()) {
        InitDateFormatWithoutConfigs(status);
    } else {
        InitDateFormat(status);
    }
    if (U_FAILURE(status)) {
        HILOG_ERROR_I18N("DateTimeFormat::InitWithLocale: InitDateFormat failed.");
        return false;
    }
    status = U_ZERO_ERROR;
    calendar = std::unique_ptr<icu::Calendar>(Calendar::createInstance(locale, status));
    if (U_FAILURE(status)) {
        HILOG_ERROR_I18N("DateTimeFormat::InitWithLocale: Create calendar instance failed.");
        return false;
    }
    return true;
}

void DateTimeFormat::InitDateFormatWithoutConfigs(UErrorCode &status)
{
    DateFormat::EStyle style = DefaultDTStyle[defaultStyle];
    if (style == DateFormat::EStyle::kShort) {
        std::string number = LocaleConfig::GetPatternNumberFromLocale(finalLocale);
        auto iter = EXT_PARAM_TO_PATTERNS.find(number);
        if (iter != EXT_PARAM_TO_PATTERNS.end()) {
            pattern = iter->second[YMD_INDEX];
            icu::UnicodeString dayPattern = GetDayPatternFromLocale(finalLocale);
            pattern.findAndReplace(D_SKELETON, dayPattern);
        } else {
            pattern = DateTimeFormat::GetPatternFromLocale(finalLocale);
        }
        if (InitFormatter()) {
            return;
        }
    }
    dateFormat = std::unique_ptr<icu::DateFormat>(DateFormat::createDateInstance(style, locale));
    SimpleDateFormat *simDateFormat = static_cast<SimpleDateFormat*>(dateFormat.get());
    if (simDateFormat != nullptr) {
        simDateFormat->toPattern(pattern);
    }
    dateIntvFormat =
        std::unique_ptr<DateIntervalFormat>(DateIntervalFormat::createInstance(pattern, locale, status));
}

void DateTimeFormat::FixPatternPartOne()
{
    if (hour12 == "true") {
        pattern.findAndReplace(icu::UnicodeString::fromUTF8(StringPiece("H")),
            icu::UnicodeString::fromUTF8(StringPiece("h")));
        pattern.findAndReplace(icu::UnicodeString::fromUTF8(StringPiece("k")),
            icu::UnicodeString::fromUTF8(StringPiece("h")));
        pattern.findAndReplace(icu::UnicodeString::fromUTF8(StringPiece("K")),
            icu::UnicodeString::fromUTF8(StringPiece("h")));
    } else if (hour12 == "false") {
        pattern.findAndReplace(icu::UnicodeString::fromUTF8(StringPiece("h")),
            icu::UnicodeString::fromUTF8(StringPiece("H")));
        pattern.findAndReplace(icu::UnicodeString::fromUTF8(StringPiece("k")),
            icu::UnicodeString::fromUTF8(StringPiece("H")));
        pattern.findAndReplace(icu::UnicodeString::fromUTF8(StringPiece("K")),
            icu::UnicodeString::fromUTF8(StringPiece("H")));
        RemoveAmPmChar();
    } else if (hourCycle != "") {
        FixPatternPartTwo();
    }
}

void DateTimeFormat::FixPatternPartTwo()
{
    if (hourCycle == "h11") {
        pattern.findAndReplace(icu::UnicodeString::fromUTF8(StringPiece("h")),
            icu::UnicodeString::fromUTF8(StringPiece("k")));
        pattern.findAndReplace(icu::UnicodeString::fromUTF8(StringPiece("H")),
            icu::UnicodeString::fromUTF8(StringPiece("k")));
        pattern.findAndReplace(icu::UnicodeString::fromUTF8(StringPiece("K")),
            icu::UnicodeString::fromUTF8(StringPiece("k")));
    } else if (hourCycle == "h12") {
        pattern.findAndReplace(icu::UnicodeString::fromUTF8(StringPiece("H")),
            icu::UnicodeString::fromUTF8(StringPiece("h")));
        pattern.findAndReplace(icu::UnicodeString::fromUTF8(StringPiece("k")),
            icu::UnicodeString::fromUTF8(StringPiece("h")));
        pattern.findAndReplace(icu::UnicodeString::fromUTF8(StringPiece("K")),
            icu::UnicodeString::fromUTF8(StringPiece("h")));
    } else if (hourCycle == "h23") {
        pattern.findAndReplace(icu::UnicodeString::fromUTF8(StringPiece("h")),
            icu::UnicodeString::fromUTF8(StringPiece("K")));
        pattern.findAndReplace(icu::UnicodeString::fromUTF8(StringPiece("H")),
            icu::UnicodeString::fromUTF8(StringPiece("K")));
        pattern.findAndReplace(icu::UnicodeString::fromUTF8(StringPiece("k")),
            icu::UnicodeString::fromUTF8(StringPiece("K")));
        RemoveAmPmChar();
    } else if (hourCycle == "h24") {
        pattern.findAndReplace(icu::UnicodeString::fromUTF8(StringPiece("h")),
            icu::UnicodeString::fromUTF8(StringPiece("H")));
        pattern.findAndReplace(icu::UnicodeString::fromUTF8(StringPiece("k")),
            icu::UnicodeString::fromUTF8(StringPiece("H")));
        pattern.findAndReplace(icu::UnicodeString::fromUTF8(StringPiece("K")),
            icu::UnicodeString::fromUTF8(StringPiece("H")));
        RemoveAmPmChar();
    }
}

void DateTimeFormat::RemoveAmPmChar()
{
    std::string patternString = "";
    pattern.toUTF8String(patternString);
    size_t amPmCharStartIdx = 0;
    size_t amPmCharEndIdx = 0;
    for (size_t i = 0; i < patternString.length(); i++) {
        if (patternString[i] != 'a') {
            continue;
        }
        if ((i + 1) < patternString.length() && patternString[i + 1] == 't') {
            continue;
        }
        if (!i) {
            amPmCharStartIdx = i;
        } else {
            amPmCharStartIdx = i - 1;
            while (amPmCharStartIdx > 0 && patternString[amPmCharStartIdx] == ' ') {
                amPmCharStartIdx -= 1;
            }
            if (amPmCharStartIdx || patternString[amPmCharStartIdx] != ' ') {
                amPmCharStartIdx += 1;
            }
        }
        amPmCharEndIdx = i + 1;
        while (amPmCharEndIdx < patternString.length() && patternString[amPmCharEndIdx] == ' ') {
            amPmCharEndIdx += 1;
        }
        break;
    }
    size_t length = amPmCharEndIdx - amPmCharStartIdx;
    if (length) {
        if (!amPmCharStartIdx || amPmCharEndIdx == patternString.length()) {
            patternString = patternString.replace(amPmCharStartIdx, length, "");
        } else {
            patternString = patternString.replace(amPmCharStartIdx, length, " ");
        }
        pattern = icu::UnicodeString(patternString.data(), patternString.length());
    }
}

void DateTimeFormat::InitDateFormat(UErrorCode &status)
{
    if (!dateStyle.empty() || !timeStyle.empty()) {
        bool initSuccess = false;
        InitDateFormatWithDateTimeStyle(initSuccess, status);
        if (initSuccess) {
            return;
        }
        dateFormat = std::make_unique<SimpleDateFormat>(pattern, locale, status);
        if (U_FAILURE(status)) {
            HILOG_ERROR_I18N("DateTimeFormat::InitDateFormat: Create SimpleDateFormat instance failed.");
            return;
        }
    } else {
        auto patternGenerator =
            std::unique_ptr<DateTimePatternGenerator>(DateTimePatternGenerator::createInstance(locale, status));
        if (U_FAILURE(status) || patternGenerator == nullptr) {
            HILOG_ERROR_I18N("DateTimeFormat::InitDateFormat: Create DateTimePatternGenerator instance failed.");
            return;
        }
        ComputePattern();
        pattern =
            patternGenerator->replaceFieldTypes(patternGenerator->getBestPattern(pattern, status), pattern, status);
        pattern = patternGenerator->getBestPattern(pattern, status);
        SetDayPeriod();
        if (DateTimeFormat::InitPatternByNumeric()) {
            return;
        }
        status = U_ZERO_ERROR;
        dateFormat = std::make_unique<SimpleDateFormat>(pattern, locale, status);
        if (U_FAILURE(status)) {
            HILOG_ERROR_I18N("DateTimeFormat::InitDateFormat: Create SimpleDateFormat object failed.");
            return;
        }
    }
    status = U_ZERO_ERROR;
    dateIntvFormat =
        std::unique_ptr<DateIntervalFormat>(DateIntervalFormat::createInstance(pattern, locale, status));
}

void DateTimeFormat::InitDateFormatWithDateTimeStyle(bool& initSuccess, UErrorCode &status)
{
    DateFormat::EStyle dateStyleValue = DateFormat::EStyle::kNone;
    DateFormat::EStyle timeStyleValue = DateFormat::EStyle::kNone;
    if (!dateStyle.empty()) {
        if (dateTimeStyle.count(dateStyle) > 0) {
            dateStyleValue = dateTimeStyle[dateStyle];
        } else if (dateStyle == "auto") {
            dateStyleValue = DefaultDTStyle[defaultStyle];
        }
    }
    if (!timeStyle.empty()) {
        if (dateTimeStyle.count(timeStyle) > 0) {
            timeStyleValue = dateTimeStyle[timeStyle];
        } else if (timeStyle == "auto") {
            timeStyleValue = DefaultDTStyle[defaultStyle];
        }
    }
    icu::DateFormat *tempDateFormat = DateFormat::createDateTimeInstance(dateStyleValue, timeStyleValue, locale);
    if (tempDateFormat == nullptr) {
        status = U_INVALID_FORMAT_ERROR;
        HILOG_ERROR_I18N("DateTimeFormat::InitDateFormatWithDateTimeStyle: Create DateFormat instance failed.");
        return;
    }
    SimpleDateFormat* simpleDateFormat = static_cast<SimpleDateFormat*>(tempDateFormat);
    std::unique_ptr<SimpleDateFormat> simDateFormatPtr = std::unique_ptr<SimpleDateFormat>(simpleDateFormat);
    if (simDateFormatPtr != nullptr) {
        simDateFormatPtr->toPattern(pattern);
    }
    FixPatternPartOne();
    if (dateStyleValue == DateFormat::EStyle::kShort) {
        initSuccess = InitPatternByDateStyle();
    }
}

void DateTimeFormat::SetDayPeriod()
{
    if (dayPeriod == "short" || (dayPeriod == "auto" && defaultStyle ==  DefaultStyle::DEFAULT)) {
        pattern.findAndReplace(icu::UnicodeString::fromUTF8(StringPiece("a")),
            icu::UnicodeString::fromUTF8(StringPiece("B")));
    } else if (dayPeriod == "long" || (dayPeriod == "auto" && defaultStyle ==  DefaultStyle::LONG)) {
        pattern.findAndReplace(icu::UnicodeString::fromUTF8(StringPiece("a")),
            icu::UnicodeString::fromUTF8(StringPiece("BBBB")));
    } else if (dayPeriod == "narrow" || (dayPeriod == "auto" && defaultStyle ==  DefaultStyle::SHORT)) {
        pattern.findAndReplace(icu::UnicodeString::fromUTF8(StringPiece("a")),
            icu::UnicodeString::fromUTF8(StringPiece("BBBBB")));
    }
}

void DateTimeFormat::ParseConfigsPartOne(std::map<std::string, std::string> &configs)
{
    if (configs.count("dateStyle") > 0) {
        dateStyle = configs["dateStyle"];
    }
    if (configs.count("timeStyle") > 0) {
        timeStyle = configs["timeStyle"];
    }
    if (configs.count("year") > 0) {
        year = configs["year"];
    }
    if (configs.count("month") > 0) {
        month = configs["month"];
    }
    if (configs.count("day") > 0) {
        day = configs["day"];
    }
    if (configs.count("hour") > 0) {
        hour = configs["hour"];
    }
    if (configs.count("minute") > 0) {
        minute = configs["minute"];
    }
    if (configs.count("second") > 0) {
        second = configs["second"];
    }
}

void DateTimeFormat::ParseConfigsPartTwo(std::map<std::string, std::string> &configs)
{
    if (configs.count("hourCycle") > 0) {
        hourCycle = configs["hourCycle"];
    }
    if (configs.count("timeZone") > 0) {
        timeZone = configs["timeZone"];
    }
    if (configs.count("numberingSystem") > 0) {
        numberingSystem = configs["numberingSystem"];
    }
    if (configs.count("hour12") > 0) {
        hour12 = configs["hour12"];
    }
    if (configs.count("weekday") > 0) {
        weekday = configs["weekday"];
    }
    if (configs.count("era") > 0) {
        era = configs["era"];
    }
    if (configs.count("timeZoneName") > 0) {
        timeZoneName = configs["timeZoneName"];
    }
    if (configs.count("dayPeriod") > 0) {
        dayPeriod = configs["dayPeriod"];
    }
    if (configs.count("localeMatcher") > 0) {
        localeMatcher = configs["localeMatcher"];
    }
    if (configs.count("formatMatcher") > 0) {
        formatMatcher = configs["formatMatcher"];
    }
}

void DateTimeFormat::ComputeSkeleton()
{
    if (year.empty() && month.empty() && day.empty() && hour.empty() && minute.empty() && second.empty() &&
        weekday.empty()) {
        pattern.append("yMd");
    }
    AddOptions(year, yearChar);
    AddOptions(month, monthChar);
    AddOptions(day, dayChar);
    AddOptions(hour, hourChar);
    AddOptions(minute, minuteChar);
    AddOptions(second, secondChar);
    if ((hourCycle == "h12" || hourCycle == "h11" || hour12 == "true") && !hour.empty()) {
        pattern.append(amPmChar);
    }
    AddOptions(timeZoneName, timeZoneChar);
    AddOptions(weekday, weekdayChar);
    AddOptions(era, eraChar);
}

void DateTimeFormat::AddOptions(std::string option, char16_t optionChar)
{
    if (!option.empty()) {
        pattern.append(optionChar);
    }
}

void DateTimeFormat::ComputeHourCycleChars()
{
    if (!hour12.empty()) {
        if (hour12 == "true") {
            hourNumericString = "h";
            hourTwoDigitString = "hh";
        } else {
            hourNumericString = "H";
            hourTwoDigitString = "HH";
        }
    } else {
        if (hourCycle == "h11") {
            hourNumericString = "K";
            hourTwoDigitString = "KK";
        } else if (hourCycle == "h12") {
            hourNumericString = "h";
            hourTwoDigitString = "hh";
        } else if (hourCycle == "h23") {
            hourNumericString = "H";
            hourTwoDigitString = "HH";
        } else if (hourCycle == "h24") {
            hourNumericString = "k";
            hourTwoDigitString = "kk";
        }
    }
}

void DateTimeFormat::ComputePattern()
{
    ComputePartOfPattern(year, yearChar, "yy", "yyyy");
    ComputePartOfPattern(day, dayChar, "dd", "d");
    ComputePartOfPattern(hour, hourChar, hourTwoDigitString, hourNumericString);
    ComputePartOfPattern(minute, minuteChar, "mm", "mm");
    ComputePartOfPattern(second, secondChar, "ss", "ss");
    if (!month.empty()) {
        UnicodeString monthOfPattern = UnicodeString(monthChar);
        int32_t length = monthOfPattern.length();
        if (month == "numeric" && length != NUMERIC_LENGTH) {
            pattern.findAndReplace(monthOfPattern, UnicodeString::fromUTF8(StringPiece("M")));
        } else if (month == "2-digit" && length != TWO_DIGIT_LENGTH) {
            pattern.findAndReplace(monthOfPattern, UnicodeString::fromUTF8(StringPiece("MM")));
        } else if ((month == "long" && length != LONG_LENGTH) || (month == "auto" &&
            defaultStyle ==  DefaultStyle::LONG)) {
            pattern.findAndReplace(monthOfPattern, UnicodeString::fromUTF8(StringPiece("MMMM")));
        } else if ((month == "short" && length != SHORT_LENGTH) || (month == "auto" &&
            (defaultStyle ==  DefaultStyle::DEFAULT || defaultStyle ==  DefaultStyle::SHORT))) {
            pattern.findAndReplace(monthOfPattern, UnicodeString::fromUTF8(StringPiece("MMM")));
        } else if (month == "narrow" && length != NARROW_LENGTH) {
            pattern.findAndReplace(monthOfPattern, UnicodeString::fromUTF8(StringPiece("MMMMM")));
        }
    }

    ComputeTimeZoneOfPattern(timeZoneName, timeZoneChar, "zzzz", "O");
    ComputeWeekdayOfPattern(weekday, weekdayChar, "EEEE", "E", "EEEEE");
    ComputeEraOfPattern(era, eraChar, "GGGG", "G", "GGGGG");
}

void DateTimeFormat::ComputePartOfPattern(std::string option, char16_t character, std::string twoDigitChar,
    std::string numericChar)
{
    if (!option.empty()) {
        UnicodeString curPartOfPattern = UnicodeString(character);
        if (option == "2-digit") {
            pattern.findAndReplace(curPartOfPattern, UnicodeString::fromUTF8(StringPiece(twoDigitChar)));
        } else if (option == "numeric") {
            pattern.findAndReplace(curPartOfPattern, UnicodeString::fromUTF8(StringPiece(numericChar)));
        }
    }
}

void DateTimeFormat::ComputeTimeZoneOfPattern(std::string option, char16_t character, std::string longChar,
    std::string shortChar)
{
    if (!option.empty()) {
        UnicodeString timeZoneOfPattern = UnicodeString(character);
        if (option == "long" || (option == "auto" && defaultStyle ==  DefaultStyle::LONG)) {
            pattern.findAndReplace(timeZoneOfPattern, UnicodeString::fromUTF8(StringPiece(longChar)));
        } else if (option == "short" || (option == "auto" && (defaultStyle ==  DefaultStyle::DEFAULT ||
            defaultStyle ==  DefaultStyle::SHORT))) {
            pattern.findAndReplace(timeZoneOfPattern, UnicodeString::fromUTF8(StringPiece(shortChar)));
        }
    }
}

void DateTimeFormat::ComputeWeekdayOfPattern(std::string option, char16_t character, std::string longChar,
    std::string shortChar, std::string narrowChar)
{
    if (!option.empty()) {
        UnicodeString curPartOfPattern = UnicodeString(character);
        int32_t length = curPartOfPattern.length();
        if ((option == "long" && length != LONG_ERA_LENGTH) || (option == "auto" &&
            defaultStyle ==  DefaultStyle::LONG)) {
            pattern.findAndReplace(curPartOfPattern, UnicodeString::fromUTF8(StringPiece(longChar)));
        } else if ((option == "short" && length != SHORT_ERA_LENGTH) || (option == "auto" &&
            (defaultStyle ==  DefaultStyle::DEFAULT || defaultStyle ==  DefaultStyle::SHORT))) {
            pattern.findAndReplace(curPartOfPattern, UnicodeString::fromUTF8(StringPiece(shortChar)));
        } else if (option == "narrow" && length != NARROW_LENGTH) {
            pattern.findAndReplace(curPartOfPattern, UnicodeString::fromUTF8(StringPiece(narrowChar)));
        }
    }
}

void DateTimeFormat::ComputeEraOfPattern(std::string option, char16_t character, std::string longChar,
    std::string shortChar, std::string narrowChar)
{
    if (!option.empty()) {
        UnicodeString curPartOfPattern = UnicodeString(character);
        int32_t length = curPartOfPattern.length();
        if ((option == "long" && length != LONG_ERA_LENGTH) || (option == "auto" &&
            defaultStyle ==  DefaultStyle::LONG)) {
            pattern.findAndReplace(curPartOfPattern, UnicodeString::fromUTF8(StringPiece(longChar)));
        } else if ((option == "short" && length != SHORT_ERA_LENGTH) || (option == "auto" &&
            defaultStyle ==  DefaultStyle::DEFAULT)) {
            pattern.findAndReplace(curPartOfPattern, UnicodeString::fromUTF8(StringPiece(shortChar)));
        } else if ((option == "narrow" && length != NARROW_LENGTH) || (option == "auto" &&
            defaultStyle ==  DefaultStyle::SHORT)) {
            pattern.findAndReplace(curPartOfPattern, UnicodeString::fromUTF8(StringPiece(narrowChar)));
        }
    }
}

std::string DateTimeFormat::Format(int64_t milliseconds)
{
    if (!createSuccess || calendar == nullptr || dateFormat == nullptr) {
        return PseudoLocalizationProcessor("");
    }
    UErrorCode status = U_ZERO_ERROR;
    std::string result;
    UnicodeString dateString;
    calendar->clear();
    std::string timezoneStr = timeZone.empty() ? LocaleConfig::GetSystemTimezone() : timeZone;
    auto zone = std::unique_ptr<TimeZone>(TimeZone::createTimeZone(timezoneStr.c_str()));
    if (zone != nullptr) {
        calendar->setTimeZone(*zone);
        dateFormat->setTimeZone(*zone);
    }
    calendar->setTime((UDate)milliseconds, status);
    dateFormat->format(calendar->getTime(status), dateString, status);
    dateString.toUTF8String(result);
    return PseudoLocalizationProcessor(result);
}

std::string DateTimeFormat::FormatRange(int64_t fromMilliseconds, int64_t toMilliseconds)
{
    if (!createSuccess || calendar == nullptr || dateIntvFormat == nullptr) {
        return PseudoLocalizationProcessor("");
    }
    UErrorCode status = U_ZERO_ERROR;
    std::string result;
    UnicodeString dateString;
    calendar->clear();
    std::string timezoneStr = timeZone.empty() ? LocaleConfig::GetSystemTimezone() : timeZone;
    auto zone = std::unique_ptr<TimeZone>(TimeZone::createTimeZone(timezoneStr.c_str()));
    if (zone != nullptr) {
        calendar->setTimeZone(*zone);
        dateIntvFormat->setTimeZone(*zone);
    }
    calendar->setTime((UDate)fromMilliseconds, status);
    if (U_FAILURE(status)) {
        return PseudoLocalizationProcessor("");
    }
    auto toCalendar = std::unique_ptr<Calendar>(Calendar::createInstance(locale, status));
    if (U_FAILURE(status) || toCalendar == nullptr) {
        return PseudoLocalizationProcessor("");
    }
    toCalendar->clear();
    if (zone != nullptr) {
        toCalendar->setTimeZone(*zone);
    }
    toCalendar->setTime((UDate)toMilliseconds, status);
    if (U_FAILURE(status)) {
        return PseudoLocalizationProcessor("");
    }
    FieldPosition pos = 0;
    dateIntvFormat->format(*calendar, *toCalendar, dateString, pos, status);
    if (U_FAILURE(status)) {
        return PseudoLocalizationProcessor("");
    }
    dateString.toUTF8String(result);
    return PseudoLocalizationProcessor(result);
}

void DateTimeFormat::GetResolvedOptions(std::map<std::string, std::string> &map)
{
    if (!createSuccess || localeInfo == nullptr || dateFormat == nullptr) {
        return;
    }
    map.insert(std::make_pair("locale", localeTag));
    if (!(localeInfo->GetCalendar()).empty()) {
        map.insert(std::make_pair("calendar", localeInfo->GetCalendar()));
    } else {
        map.insert(std::make_pair("calendar", calendar->getType()));
    }
    if (!dateStyle.empty()) {
        map.insert(std::make_pair("dateStyle", dateStyle));
    }
    if (!timeStyle.empty()) {
        map.insert(std::make_pair("timeStyle", timeStyle));
    }
    if (!hourCycle.empty()) {
        map.insert(std::make_pair("hourCycle", hourCycle));
    } else if (!(localeInfo->GetHourCycle()).empty()) {
        map.insert(std::make_pair("hourCycle", localeInfo->GetHourCycle()));
    }
    if (!timeZone.empty()) {
        map.insert(std::make_pair("timeZone", timeZone));
    } else {
        UnicodeString timeZoneID("");
        std::string timeZoneString;
        dateFormat->getTimeZone().getID(timeZoneID).toUTF8String(timeZoneString);
        map.insert(std::make_pair("timeZone", timeZoneString));
    }
    if (!timeZoneName.empty()) {
        map.insert(std::make_pair("timeZoneName", timeZoneName));
    }
    if (!numberingSystem.empty()) {
        map.insert(std::make_pair("numberingSystem", numberingSystem));
    } else if (!(localeInfo->GetNumberingSystem()).empty()) {
        map.insert(std::make_pair("numberingSystem", localeInfo->GetNumberingSystem()));
    } else {
        UErrorCode status = U_ZERO_ERROR;
        auto numSys = std::unique_ptr<NumberingSystem>(NumberingSystem::createInstance(locale, status));
        if (U_SUCCESS(status)) {
            map.insert(std::make_pair("numberingSystem", numSys->getName()));
        }
    }
    GetAdditionalResolvedOptions(map);
}

void DateTimeFormat::GetAdditionalResolvedOptions(std::map<std::string, std::string> &map)
{
    if (!hour12.empty()) {
        map.insert(std::make_pair("hour12", hour12));
    }
    if (!weekday.empty()) {
        map.insert(std::make_pair("weekday", weekday));
    }
    if (!era.empty()) {
        map.insert(std::make_pair("era", era));
    }
    if (!year.empty()) {
        map.insert(std::make_pair("year", year));
    }
    if (!month.empty()) {
        map.insert(std::make_pair("month", month));
    }
    if (!day.empty()) {
        map.insert(std::make_pair("day", day));
    }
    if (!hour.empty()) {
        map.insert(std::make_pair("hour", hour));
    }
    if (!minute.empty()) {
        map.insert(std::make_pair("minute", minute));
    }
    if (!second.empty()) {
        map.insert(std::make_pair("second", second));
    }
    if (!dayPeriod.empty()) {
        map.insert(std::make_pair("dayPeriod", dayPeriod));
    }
    if (!localeMatcher.empty()) {
        map.insert(std::make_pair("localeMatcher", localeMatcher));
    }
    if (!formatMatcher.empty()) {
        map.insert(std::make_pair("formatMatcher", formatMatcher));
    }
}

std::string DateTimeFormat::GetDateStyle() const
{
    return dateStyle;
}

std::string DateTimeFormat::GetTimeStyle() const
{
    return timeStyle;
}

std::string DateTimeFormat::GetHourCycle() const
{
    return hourCycle;
}

std::string DateTimeFormat::GetTimeZone() const
{
    return timeZone;
}

std::string DateTimeFormat::GetTimeZoneName() const
{
    return timeZoneName;
}

std::string DateTimeFormat::GetNumberingSystem() const
{
    return numberingSystem;
}

std::string DateTimeFormat::GetHour12() const
{
    return hour12;
}

std::string DateTimeFormat::GetWeekday() const
{
    return weekday;
}

std::string DateTimeFormat::GetEra() const
{
    return era;
}

std::string DateTimeFormat::GetYear() const
{
    return year;
}

std::string DateTimeFormat::GetMonth() const
{
    return month;
}

std::string DateTimeFormat::GetDay() const
{
    return day;
}

std::string DateTimeFormat::GetHour() const
{
    return hour;
}

std::string DateTimeFormat::GetMinute() const
{
    return minute;
}

std::string DateTimeFormat::GetSecond() const
{
    return second;
}

bool DateTimeFormat::Init()
{
    SetHwIcuDirectory();
    return true;
}

DateTimeFormat::DefaultStyle DateTimeFormat::GetDefaultStyle()
{
    char value[LocaleConfig::CONFIG_LEN];
    int code = GetParameter(DEVICE_TYPE_NAME, "", value, LocaleConfig::CONFIG_LEN);
    if (code > 0) {
        std::string deviceType = value;
        if (DeviceToStyle.find(deviceType) != DeviceToStyle.end()) {
            return DeviceToStyle[deviceType];
        }
    }
    return DefaultStyle::DEFAULT;
}

bool DateTimeFormat::InitPatternByDateStyle()
{
    icu::UnicodeString tempPattern = GetBestPattern(DateFormat::EStyle::kShort, DateFormat::EStyle::kNone, locale);
    std::string number = LocaleConfig::GetPatternNumberFromLocale(finalLocale);
    auto iter = EXT_PARAM_TO_PATTERNS.find(number);
    icu::UnicodeString replacePattern;
    if (iter != EXT_PARAM_TO_PATTERNS.end()) {
        replacePattern = iter->second[YMD_INDEX];
        icu::UnicodeString dayPattern = GetDayPatternFromLocale(finalLocale);
        replacePattern.findAndReplace(D_SKELETON, dayPattern);
        ReplacePattern(tempPattern, replacePattern);
    }
    return InitFormatter();
}

bool DateTimeFormat::InitPatternByNumeric()
{
    icu::UnicodeString tempSkeleton;
    size_t index = -1;
    if (year == NUMERIC_STR && month == NUMERIC_STR && day == NUMERIC_STR) {
        tempSkeleton = YMD_SKELETON;
        index = YMD_INDEX;
    } else if (year == NUMERIC_STR && month == NUMERIC_STR && day.empty()) {
        tempSkeleton = YM_SKELETON;
        index = YM_INDEX;
    } else if (year.empty() && month == NUMERIC_STR && day == NUMERIC_STR) {
        tempSkeleton = MD_SKELETON;
        index = MD_INDEX;
    }
    if (tempSkeleton.isEmpty()) {
        return false;
    }
    std::string number = LocaleConfig::GetPatternNumberFromLocale(finalLocale);
    auto iter = EXT_PARAM_TO_PATTERNS.find(number);
    icu::UnicodeString replacePattern;
    icu::UnicodeString tempPattern = GetBestPattern(tempSkeleton, locale);
    if (iter != EXT_PARAM_TO_PATTERNS.end()) {
        replacePattern = iter->second[index];
        icu::UnicodeString dayPattern = GetDayPatternFromLocale(finalLocale);
        replacePattern.findAndReplace(D_SKELETON, dayPattern);
        ReplacePattern(tempPattern, replacePattern);
    }
    return InitFormatter();
}

void DateTimeFormat::ReplacePattern(const icu::UnicodeString& tempPattern, const icu::UnicodeString& replacePattern)
{
    if (tempPattern.isEmpty()) {
        HILOG_ERROR_I18N("DateTimeFormat::ReplacePattern: tempPattern is empty.");
        return;
    }
    int32_t startIndex = pattern.indexOf(tempPattern);
    if (startIndex == -1) {
        HILOG_ERROR_I18N("DateTimeFormat::ReplacePattern: startIndex is -1.");
        return;
    }
    int32_t length = tempPattern.length();
    pattern.replace(startIndex, length, replacePattern);
}

std::unordered_map<std::string, std::string> DateTimeFormat::GetPatternsFromLocale(const std::string& localeTag)
{
    std::unordered_map<std::string, std::string> result;
    for (const auto& item : EXT_PARAM_TO_PATTERNS) {
        icu::UnicodeString pattern = item.second[YMD_INDEX];
        std::string tempPattern;
        pattern.toUTF8String(tempPattern);
        result.insert(std::make_pair(tempPattern, item.first));
    }

    icu::UnicodeString pattern = GetSingleDayPatternFromLocale(localeTag);
    icu::UnicodeString patternWithoutSymbol = pattern;
    patternWithoutSymbol.findAndReplace(LTR_SYMBOL, "");
    if (patternWithoutSymbol.indexOf(RTL_SYMBOL) != -1) {
        patternWithoutSymbol.findAndReplace(RTL_SYMBOL, "");
        patternWithoutSymbol.reverse();
    }
    std::string tempPatternWithoutSymbol;
    patternWithoutSymbol.toUTF8String(tempPatternWithoutSymbol);
    auto iter = result.find(tempPatternWithoutSymbol);
    if (iter != result.end()) {
        result.erase(iter);
    }

    std::string tempPattern;
    pattern.toUTF8String(tempPattern);
    result.insert(std::make_pair(tempPattern, ""));
    return result;
}

icu::UnicodeString DateTimeFormat::GetPatternFromLocale(const std::string& localeTag)
{
    UErrorCode status = U_ZERO_ERROR;
    icu::Locale locale = icu::Locale::forLanguageTag(StringPiece(localeTag), status);
    if (U_FAILURE(status)) {
        HILOG_ERROR_I18N("DateTimeFormat::GetPatternFromLocale: Create icu locale failed.");
        return "";
    }

    icu::DateFormat* icuDateFormet = icu::DateFormat::createDateInstance(icu::DateFormat::SHORT, locale);
    if (icuDateFormet == nullptr) {
        HILOG_ERROR_I18N("DateTimeFormat::GetPatternFromLocale: Create icuDateFormet failed.");
        return "";
    }
    icu::SimpleDateFormat* icuSimpleDateFormat = reinterpret_cast<icu::SimpleDateFormat *>(icuDateFormet);
    icu::UnicodeString result;
    icuSimpleDateFormat->toPattern(result);
    delete icuSimpleDateFormat;
    return result;
}

icu::UnicodeString DateTimeFormat::GetYMDPatternFromNumber(const std::string& number)
{
    auto iter = EXT_PARAM_TO_PATTERNS.find(number);
    if (iter == EXT_PARAM_TO_PATTERNS.end()) {
        HILOG_ERROR_I18N("DateTimeFormat::GetYMDPatternFromNumber: Invalid number %{public}s.", number.c_str());
        return "";
    }
    std::vector<icu::UnicodeString> patterns = iter->second;
    return patterns[YMD_INDEX];
}

std::string DateTimeFormat::GetDateSampleFromPattern(const icu::UnicodeString& pattern, const std::string& localeTag)
{
    std::string localeWithoutCustExtParam = LocaleConfig::RemoveCustExtParam(localeTag);
    UErrorCode status = U_ZERO_ERROR;
    icu::Locale icuLocale = icu::Locale::forLanguageTag(StringPiece(localeWithoutCustExtParam), status);
    if (U_FAILURE(status)) {
        HILOG_ERROR_I18N("DateTimeFormat::GetDateSampleFromPattern: Create icu locale failed.");
        return "";
    }
    std::unique_ptr<icu::SimpleDateFormat> simpleDateFormat =
        std::make_unique<icu::SimpleDateFormat>(pattern, icuLocale, status);
    if (U_FAILURE(status)) {
        HILOG_ERROR_I18N("DateTimeFormat::GetDateSampleFromPattern: Create simpleDateFormat failed.");
        return "";
    }
    icu::UnicodeString formatResult;
    simpleDateFormat->format(DEFAULT_TIME, formatResult, status);
    if (U_FAILURE(status)) {
        HILOG_ERROR_I18N("DateTimeFormat::GetDateSampleFromPattern: Format failed.");
        return "";
    }
    std::string result;
    formatResult.toUTF8String(result);
    return result;
}

bool DateTimeFormat::IsValidPatternNumber(const std::string& number)
{
    if (EXT_PARAM_TO_PATTERNS.find(number) == EXT_PARAM_TO_PATTERNS.end()) {
        return false;
    }
    return true;
}

icu::UnicodeString DateTimeFormat::GetBestPattern(const icu::UnicodeString& skeleton, const icu::Locale& icuLocale)
{
    UErrorCode status = U_ZERO_ERROR;
    auto patternGenerator =
        std::unique_ptr<DateTimePatternGenerator>(DateTimePatternGenerator::createInstance(icuLocale, status));
    if (U_FAILURE(status) || patternGenerator == nullptr) {
        HILOG_ERROR_I18N("DateTimeFormat::GetBestPattern: Create DateTimePatternGenerator instance failed.");
        return "";
    }
    icu::UnicodeString tempPattern = patternGenerator->getBestPattern(skeleton, status);
    if (U_FAILURE(status)) {
        HILOG_ERROR_I18N("DateTimeFormat::GetBestPattern: Get best pattern from skeleton failed.");
        return "";
    }
    tempPattern = patternGenerator->replaceFieldTypes(tempPattern, skeleton, status);
    if (U_FAILURE(status)) {
        HILOG_ERROR_I18N("DateTimeFormat::ReplacePatternByNumeric: Replace field types failed.");
        return "";
    }
    tempPattern = patternGenerator->getBestPattern(tempPattern, status);
    if (U_FAILURE(status)) {
        HILOG_ERROR_I18N("DateTimeFormat::ReplacePatternByNumeric: Get best pattern from tempPattern failed.");
        return "";
    }
    return tempPattern;
}

icu::UnicodeString DateTimeFormat::GetBestPattern(icu::DateFormat::EStyle dateStyle, icu::DateFormat::EStyle timeStyle,
    const icu::Locale& icuLocale)
{
    icu::DateFormat* tempDateFormat = DateFormat::createDateTimeInstance(dateStyle, timeStyle, icuLocale);
    if (tempDateFormat == nullptr) {
        HILOG_ERROR_I18N("DateTimeFormat::GetBestPattern: Create DateFormat instance failed.");
        return "";
    }
    SimpleDateFormat* simpleDateFormat = static_cast<SimpleDateFormat*>(tempDateFormat);
    std::unique_ptr<SimpleDateFormat> simDateFormatPtr = std::unique_ptr<SimpleDateFormat>(simpleDateFormat);
    if (simDateFormatPtr == nullptr) {
        HILOG_ERROR_I18N("DateTimeFormat::GetBestPattern: Create simpleDateFormat failed.");
        return "";
    }
    icu::UnicodeString pattern;
    simDateFormatPtr->toPattern(pattern);
    return pattern;
}

bool DateTimeFormat::InitFormatter()
{
    UErrorCode status = U_ZERO_ERROR;
    dateFormat = std::make_unique<SimpleDateFormat>(pattern, locale, status);
    if (U_FAILURE(status)) {
        HILOG_ERROR_I18N("DateTimeFormat::InitFormatter: Create SimpleDateFormat failed.");
        return false;
    }

    dateIntvFormat =
        std::unique_ptr<DateIntervalFormat>(DateIntervalFormat::createInstance(pattern, locale, status));
    if (U_FAILURE(status)) {
        HILOG_ERROR_I18N("DateTimeFormat::InitFormatter: Create DateIntervalFormat failed.");
        return false;
    }
    return true;
}

icu::UnicodeString DateTimeFormat::GetDayPatternFromLocale(const std::string& localeTag)
{
    icu::UnicodeString pattern = DateTimeFormat::GetPatternFromLocale(localeTag);
    int32_t index = pattern.indexOf(DD_SKELETON);
    if (index == -1) {
        return D_SKELETON;
    }
    return DD_SKELETON;
}

icu::UnicodeString DateTimeFormat::GetSingleDayPatternFromLocale(const std::string& localeTag)
{
    icu::UnicodeString pattern = GetPatternFromLocale(localeTag);
    pattern.findAndReplace(DD_SKELETON, D_SKELETON);
    return pattern;
}
} // namespace I18n
} // namespace Global
} // namespace OHOS
