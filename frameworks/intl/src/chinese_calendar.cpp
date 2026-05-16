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
#include "chinese_calendar.h"
#include "chnsecal.h"
#include "i18n_hilog.h"

namespace OHOS {
namespace Global {
namespace I18n {
const std::string DEFAULT_LOCALE_TAG = "zh-Hans-CN";
const int32_t START_GREGORY_YEAR = 1900;
const int32_t END_GREGORY_YEAR = 2100;
const int32_t START_CHINESE_YEAR = 1;
const int32_t END_CHINESE_YEAR = 60;

const std::vector<std::tuple<int32_t, int32_t, int32_t>> ERA_RANGES = {
    { 1900, 1924, 76 },
    { 1924, 1984, 77 },
    { 1984, 2044, 78 },
    { 2044, 2100, 79 }
};

const std::unordered_map<std::string, int32_t ChineseCalendarTime::*> ChineseCalendarTime::MEMBER_NAME_MAP = {
    { "gregorianYear", &ChineseCalendarTime::gregorianYear },
    { "cyclicalYear", &ChineseCalendarTime::cyclicalYear },
    { "month", &ChineseCalendarTime::month },
    { "date", &ChineseCalendarTime::date },
    { "hour", &ChineseCalendarTime::hour },
    { "minute", &ChineseCalendarTime::minute },
    { "second", &ChineseCalendarTime::second }
};

ChineseCalendar::ChineseCalendar(const std::string& localeTag)
    : I18nCalendar(localeTag, CalendarType::CHINESE)
{
}

ChineseCalendar::~ChineseCalendar()
{
}

void ChineseCalendar::SetChineseCalendarTime(const ChineseCalendarTime& time, I18nErrorCode& errCode)
{
    int32_t era = ParseEra(time.gregorianYear, time.cyclicalYear, errCode);
    if (errCode != I18nErrorCode::SUCCESS) {
        return;
    }
    icu::Calendar* icuCalendar = GetIcuCalendar();
    if (icuCalendar == nullptr) {
        HILOG_ERROR_I18N("SetChineseCalendarTime: icuCalendar is nullptr.");
        return;
    }

    icuCalendar->set(UCAL_ERA, era);
    icuCalendar->set(UCAL_YEAR, time.cyclicalYear);
    icuCalendar->set(UCAL_MONTH, time.month);
    icuCalendar->set(UCAL_DATE, time.date);
    icuCalendar->set(UCAL_HOUR_OF_DAY, time.hour);
    icuCalendar->set(UCAL_MINUTE, time.minute);
    icuCalendar->set(UCAL_SECOND, time.second);
    icuCalendar->set(UCAL_IS_LEAP_MONTH, time.isLeapMonth ? 1 : 0);
}

bool ChineseCalendar::CheckLeapMonth(int32_t gregorianYear, int32_t cyclicalYear, int32_t month, I18nErrorCode& errCode)
{
    int32_t era = ParseEra(gregorianYear, cyclicalYear, errCode);
    if (errCode != I18nErrorCode::SUCCESS) {
        return false;
    }
    UErrorCode status = U_ZERO_ERROR;
    icu::Locale locale = icu::Locale::forLanguageTag(DEFAULT_LOCALE_TAG, status);
    if (U_FAILURE(status)) {
        HILOG_ERROR_I18N("CheckLeapMonth: Create locale failed, code is %{public}s.", u_errorName(status));
        return false;
    }

    std::unique_ptr<icu::ChineseCalendar> chineseCalendar = std::make_unique<icu::ChineseCalendar>(locale, status);
    if (U_FAILURE(status)) {
        HILOG_ERROR_I18N("CheckLeapMonth: Create chineseCalendar failed, code is %{public}s.", u_errorName(status));
        return false;
    }

    chineseCalendar->set(UCAL_ERA, era);
    chineseCalendar->set(UCAL_YEAR, cyclicalYear);
    chineseCalendar->set(UCAL_MONTH, month);
    chineseCalendar->set(UCAL_DATE, 1);
    chineseCalendar->set(UCAL_IS_LEAP_MONTH, 1);
    std::string monthCode = chineseCalendar->getTemporalMonthCode(status);
    if (U_FAILURE(status) || monthCode.empty()) {
        HILOG_ERROR_I18N("CheckLeapMonth: getTemporalMonthCode failed, code is %{public}s.", u_errorName(status));
        return false;
    }
    return monthCode[monthCode.size() - 1] == 'L';
}

int32_t ChineseCalendar::ParseEra(int32_t gregorianYear, int32_t cyclicalYear, I18nErrorCode& errCode)
{
    if (!ChineseCalendar::IsValidYear(gregorianYear, cyclicalYear)) {
        errCode = I18nErrorCode::INVALID_PARAM;
        return 0;
    }
    int32_t startRange;
    int32_t endRange;
    int32_t era;
    for (const auto& ERA_RANGE : ERA_RANGES) {
        std::tie(startRange, endRange, era) = ERA_RANGE;
        if (gregorianYear < startRange || gregorianYear > endRange) {
            continue;
        }
        if (gregorianYear == endRange && cyclicalYear == START_CHINESE_YEAR) {
            return era + 1;
        }
        return era;
    }
    return 0;
}

bool ChineseCalendar::IsValidYear(int32_t gregorianYear, int32_t cyclicalYear)
{
    if (gregorianYear < START_GREGORY_YEAR || gregorianYear > END_GREGORY_YEAR) {
        HILOG_ERROR_I18N("ChineseCalendar::IsValidYear: Invalid gregorianYear %{public}d", gregorianYear);
        return false;
    }
    if (cyclicalYear < START_CHINESE_YEAR || cyclicalYear > END_CHINESE_YEAR) {
        HILOG_ERROR_I18N("ChineseCalendar::IsValidYear: Invalid cyclicalYear %{public}d", cyclicalYear);
        return false;
    }
    return true;
}
} // namespace I18n
} // namespace Global
} // namespace OHOS