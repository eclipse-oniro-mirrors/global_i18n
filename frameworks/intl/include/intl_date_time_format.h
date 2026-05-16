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
#ifndef OHOS_GLOBAL_I18N_INTL_DATE_TIME_FORMAT_H
#define OHOS_GLOBAL_I18N_INTL_DATE_TIME_FORMAT_H


#include <map>
#include <string>
#include <unordered_map>
#include <vector>
#include "unicode/datefmt.h"
#include "unicode/dtitvfmt.h"
#include "unicode/dtptngen.h"
#include "unicode/localebuilder.h"
#include "unicode/locid.h"
#include "i18n_types.h"
#include "date_time_format_part.h"

namespace OHOS {
namespace Global {
namespace I18n {
class IcuPatternDesc {
public:
    IcuPatternDesc(std::string property, const std::vector<std::pair<std::string, std::string>> &pairs,
        std::vector<std::string> allowedValues);

    virtual ~IcuPatternDesc() = default;

    std::string property;
    std::vector<std::pair<std::string, std::string>> pairs;
    std::unordered_map<std::string, std::string> propertyValueToPattern;
    std::vector<std::string> allowedValues;
};

class Pattern {
public:
    Pattern(const std::string& twoDigitPattern, const std::string& numericPattern);
    virtual ~Pattern() = default;

    std::vector<IcuPatternDesc> GetData() const;
    static std::vector<IcuPatternDesc> GetIcuPatternDescs();

private:
    std::vector<IcuPatternDesc> data;
};

struct CommonDateFormatPart {
    int32_t fField = 0;
    int32_t fBeginIndex = 0;
    int32_t fEndIndex = 0;
    int32_t index = 0;
    bool isPreExist = false;

    CommonDateFormatPart() = default;
    CommonDateFormatPart(int32_t fField, int32_t fBeginIndex, int32_t fEndIndex, int32_t index, bool isPreExist)
        : fField(fField), fBeginIndex(fBeginIndex), fEndIndex(fEndIndex), index(index), isPreExist(isPreExist)
    {
    }

    ~CommonDateFormatPart() = default;
};

class IntlDateTimeFormat {
public:
    IntlDateTimeFormat(const std::vector<std::string>& localeTags,
        const std::unordered_map<std::string, std::string>& configs, std::string& errMessage);
    virtual ~IntlDateTimeFormat();

    std::string Format(double milliseconds);
    std::vector<std::pair<std::string, std::string>> FormatToParts(double milliseconds, std::string& errMessage);
    std::string FormatRange(double start, double end, std::string& errMessage);
    std::pair<std::string, std::vector<DateTimeFormatPart>> FormatToParts(double milliseconds);
    std::vector<std::pair<std::string, std::string>> FormatRangeToParts(double start, double end,
        std::string& errMessage);
    void ResolvedOptions(std::unordered_map<std::string, std::string>& configs);
    static std::vector<std::string> SupportedLocalesOf(const std::vector<std::string>& requestLocales,
        const std::map<std::string, std::string>& configs, I18nErrorCode& status);

    static const std::string LOCALE_TAG;
    static const std::string LOCALE_MATCHER_TAG;
    static const std::string WEEK_DAY_TAG;
    static const std::string ERA_TAG;
    static const std::string YEAR_TAG;
    static const std::string MONTH_TAG;
    static const std::string DAY_TAG;
    static const std::string HOUR_TAG;
    static const std::string MINUTE_TAG;
    static const std::string SECOND_TAG;
    static const std::string TIME_ZONE_NAME_TAG;
    static const std::string FORMAT_MATCHER_TAG;
    static const std::string HOUR12_TAG;
    static const std::string TIME_ZONE_TAG;
    static const std::string CALENDAR_TAG;
    static const std::string DAY_PERIOD_TAG;
    static const std::string NUMBERING_SYSTEM_TAG;
    static const std::string DATE_STYLE_TAG;
    static const std::string TIME_STYLE_TAG;
    static const std::string HOUR_CYCLE_TAG;
    static const std::string FRACTIONAL_SECOND_DIGITS_TAG;

private:
    bool ParseConfigs(const std::unordered_map<std::string, std::string>& configs,
        std::string& errMessage);
    void InitIcuLocale();
    bool InitIntlDateTimeFormat(const std::unordered_map<std::string, std::string>& configs, std::string& errMessage);
    bool InitIcuTimeZone(const std::unordered_map<std::string, std::string>& configs, std::string& errMessage);
    bool InitDateTimePatternGenerator(std::string& errMessage);
    bool InitHourCycle(std::string& effectiveHourCycle);
    int32_t GetFractionalSecondDigit(const std::unordered_map<std::string, std::string>& configs);
    bool InitSkeleton(const std::unordered_map<std::string, std::string>& configs,
        const std::string& effectiveHourCycle, bool& isHourDefined, bool& isExistComponents, std::string& errMessage);
    bool InitDateTimeStyle(const std::unordered_map<std::string, std::string>& configs,
        const std::string& effectiveHourCycle, bool isHourDefined, std::string& errMessage);
    void InitDateTimeSkeleton(const std::vector<std::string> &options, std::string &skeleton, const std::string& hc);
    bool InitIcuSimpleDateFormat(bool isExistComponents, std::string& errMessage);
    std::shared_ptr<icu::SimpleDateFormat> GetSimpleDateFormatFromStyle();
    bool InitIcuCalendar();
    bool InitDateIntervalFormat();
    void ResolvedCalendarAndTimeZone(std::unordered_map<std::string, std::string>& configs);
    void ResolvedDateTimeStyle(std::unordered_map<std::string, std::string>& configs);

    static std::vector<IcuPatternDesc> GetIcuPatternDesc(const std::string& effectiveHourCycle);
    static icu::DateFormat::EStyle GetEStyleFromDateTimeStyle(const std::string& style);
    static std::string GetHourCycleFromPattern(const icu::UnicodeString& pattern);
    static icu::UnicodeString ChangeHourCyclePattern(const icu::UnicodeString& pattern, const std::string& hc);
    static icu::UnicodeString ReplaceHourCycleOfSkeleton(const icu::UnicodeString& skeleton,
        const std::string& hc);
    static std::string ConvertUDateFormatHourCycleToString(UDateFormatHourCycle hc);
    static std::vector<CommonDateFormatPart> CreateDateIntervalParts(const icu::FormattedDateInterval& formatted,
        icu::UnicodeString& formattedParts, std::string& errMessage);
    static bool IsValidTimeZoneName(const icu::TimeZone &tz);

    std::string localeMatcher = "best fit";
    std::string weekday;
    std::string era;
    std::string year;
    std::string month;
    std::string day;
    std::string hour;
    std::string minute;
    std::string second;
    std::string timeZoneName;
    std::string formatMatcher = "best fit";
    std::string hour12;
    std::string timeZone;
    std::string calendar;
    std::string dayPeriod;
    std::string numberingSystem;
    std::string dateStyle;
    std::string timeStyle;
    std::string hourCycle;
    std::string fractionalSecondDigits;
    std::string localeString;
    std::vector<std::string> skeletonOpts;
    bool initSuccess = false;
    bool isIso8601 = false;
    std::string skeleton;
    icu::Locale icuLocale;
    icu::DateTimePatternGenerator* icuDateTimePatternGenerator = nullptr;
    icu::TimeZone* icuTimeZone = nullptr;
    icu::Calendar* icuCalendar = nullptr;
    std::shared_ptr<icu::SimpleDateFormat> icuSimpleDateFormat = nullptr;
    icu::DateIntervalFormat* dateIntervalFormat = nullptr;
    static bool icuInitialized;
    static bool Init();
};
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif