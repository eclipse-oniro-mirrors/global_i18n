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

#include "date_time_format_part.h"

#include "i18n_hilog.h"

namespace OHOS {
namespace Global {
namespace I18n {
const std::string DEFAULT_DATE_TYPE = "literal";

const std::unordered_map<int32_t, std::string> FIELD_ID_TO_DATE_TYPE = {
    { UDAT_YEAR_FIELD, "year" },
    { UDAT_EXTENDED_YEAR_FIELD, "year" },
    { UDAT_YEAR_NAME_FIELD, "yearName" },
    { UDAT_MONTH_FIELD, "month" },
    { UDAT_STANDALONE_MONTH_FIELD, "month" },
    { UDAT_DATE_FIELD, "day" },
    { UDAT_HOUR_OF_DAY1_FIELD, "hour" },
    { UDAT_HOUR_OF_DAY0_FIELD, "hour" },
    { UDAT_HOUR1_FIELD, "hour" },
    { UDAT_HOUR0_FIELD, "hour" },
    { UDAT_MINUTE_FIELD, "minute" },
    { UDAT_SECOND_FIELD, "second" },
    { UDAT_DAY_OF_WEEK_FIELD, "weekday" },
    { UDAT_DOW_LOCAL_FIELD, "weekday" },
    { UDAT_STANDALONE_DAY_FIELD, "weekday" },
    { UDAT_AM_PM_FIELD, "dayPeriod" },
    { UDAT_AM_PM_MIDNIGHT_NOON_FIELD, "dayPeriod" },
    { UDAT_FLEXIBLE_DAY_PERIOD_FIELD, "dayPeriod" },
    { UDAT_TIMEZONE_FIELD, "timeZoneName" },
    { UDAT_TIMEZONE_RFC_FIELD, "timeZoneName" },
    { UDAT_TIMEZONE_GENERIC_FIELD, "timeZoneName" },
    { UDAT_TIMEZONE_SPECIAL_FIELD, "timeZoneName" },
    { UDAT_TIMEZONE_LOCALIZED_GMT_OFFSET_FIELD, "timeZoneName" },
    { UDAT_TIMEZONE_ISO_FIELD, "timeZoneName" },
    { UDAT_TIMEZONE_ISO_LOCAL_FIELD, "timeZoneName" },
    { UDAT_ERA_FIELD, "era" },
    { UDAT_FRACTIONAL_SECOND_FIELD, "fractionalSecond" },
    { UDAT_RELATED_YEAR_FIELD, "relatedYear" }
};

DateTimeFormatPart::DateTimeFormatPart(int32_t fieldId, size_t start, size_t length) : start(start), length(length)
{
    partName = ConvertFieldIdToDateType(fieldId);
}

DateTimeFormatPart::~DateTimeFormatPart()
{
}

std::string DateTimeFormatPart::ConvertFieldIdToDateType(int32_t fieldId)
{
    auto iter = FIELD_ID_TO_DATE_TYPE.find(fieldId);
    if (iter == FIELD_ID_TO_DATE_TYPE.end()) {
        return DEFAULT_DATE_TYPE;
    }
    return iter->second;
}

std::pair<icu::UnicodeString, std::vector<DateTimeFormatPart>> DateTimeFormatPart::ParseToParts(
    std::shared_ptr<icu::SimpleDateFormat> icuSimpleDateFormat, double milliseconds)
{
    if (icuSimpleDateFormat == nullptr) {
        HILOG_ERROR_I18N("DateTimeFormatPart::ParseToParts: icuSimpleDateFormat is nullptr.");
        return {};
    }
    icu::FieldPositionIterator fieldPositionIter;
    icu::UnicodeString formatResult;
    UErrorCode status = U_ZERO_ERROR;
    icuSimpleDateFormat->format(milliseconds, formatResult, &fieldPositionIter, status);
    if (U_FAILURE(status)) {
        HILOG_ERROR_I18N("DateTimeFormatPart::ParseToParts: Format failed.");
        return {};
    }
    std::vector<DateTimeFormatPart> parts;
    int32_t preEdgePos = 0;
    icu::FieldPosition fieldPosition;
    while (fieldPositionIter.next(fieldPosition)) {
        int32_t field = fieldPosition.getField();
        int32_t beginIndex = fieldPosition.getBeginIndex();
        int32_t endIndex = fieldPosition.getEndIndex();
        if (preEdgePos < beginIndex) {
            parts.emplace_back(DateTimeFormatPart(-1, static_cast<size_t>(preEdgePos),
                static_cast<size_t>(beginIndex - preEdgePos)));
        }
        parts.emplace_back(DateTimeFormatPart(field, static_cast<size_t>(beginIndex),
            static_cast<size_t>(endIndex - beginIndex)));
        preEdgePos = endIndex;
    }
    int32_t length = formatResult.length();
    if (preEdgePos < length) {
        parts.emplace_back(DateTimeFormatPart(-1, static_cast<size_t>(preEdgePos),
            static_cast<size_t>(length - preEdgePos)));
    }

    return std::make_pair(formatResult, parts);
}

void DateTimeFormatPart::SetStart(size_t value)
{
    start = value;
}

std::string DateTimeFormatPart::GetPartName() const
{
    return partName;
}

size_t DateTimeFormatPart::GetStart() const
{
    return start;
}

size_t DateTimeFormatPart::GetLength() const
{
    return length;
}
} // namespace I18n
} // namespace Global
} // namespace OHOS