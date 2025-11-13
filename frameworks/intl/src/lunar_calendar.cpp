/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#include "i18n_hilog.h"
#include "unicode/gregocal.h"
#include "lunar_calendar.h"

namespace OHOS {
namespace Global {
namespace I18n {
std::unordered_map<int32_t, int32_t> LunarCalendar::daysOfMonth {
    { 1, 31 },
    { 2, 28 },
    { 3, 31 },
    { 4, 30 },
    { 5, 31 },
    { 6, 30 },
    { 7, 31 },
    { 8, 31 },
    { 9, 30 },
    { 10, 31 },
    { 11, 30 },
    { 12, 31 }
};

std::unordered_map<int32_t, int32_t> LunarCalendar::accDaysOfMonth {
    { 1, 0 },
    { 2, 31 },
    { 3, 59 },
    { 4, 90 },
    { 5, 120 },
    { 6, 151 },
    { 7, 181 },
    { 8, 212 },
    { 9, 243 },
    { 10, 273 },
    { 11, 304 },
    { 12, 334 }
};

std::vector<uint32_t> LunarCalendar::lunarDateInfo {
    0x4bd8, 0x4ae0, 0xa570, 0x54d5, 0xd260, 0xd950, 0x5554, 0x56af, 0x9ad0, 0x55d2,
    0x4ae0, 0xa5b6, 0xa4d0, 0xd250, 0xd295, 0xb54f, 0xd6a0, 0xada2, 0x95b0, 0x4977,
    0x497f, 0xa4b0, 0xb4b5, 0x6a50, 0x6d40, 0xab54, 0x2b6f, 0x9570, 0x52f2, 0x4970,
    0x6566, 0xd4a0, 0xea50, 0x6a95, 0x5adf, 0x2b60, 0x86e3, 0x92ef, 0xc8d7, 0xc95f,
    0xd4a0, 0xd8a6, 0xb55f, 0x56a0, 0xa5b4, 0x25df, 0x92d0, 0xd2b2, 0xa950, 0xb557,
    0x6ca0, 0xb550, 0x5355, 0x4daf, 0xa5b0, 0x4573, 0x52bf, 0xa9a8, 0xe950, 0x6aa0,
    0xaea6, 0xab50, 0x4b60, 0xaae4, 0xa570, 0x5260, 0xf263, 0xd950, 0x5b57, 0x56a0,
    0x96d0, 0x4dd5, 0x4ad0, 0xa4d0, 0xd4d4, 0xd250, 0xd558, 0xb540, 0xb6a0, 0x95a6,
    0x95bf, 0x49b0, 0xa974, 0xa4b0, 0xb27a, 0x6a50, 0x6d40, 0xaf46, 0xab60, 0x9570,
    0x4af5, 0x4970, 0x64b0, 0x74a3, 0xea50, 0x6b58, 0x5ac0, 0xab60, 0x96d5, 0x92e0,
    0xc960, 0xd954, 0xd4a0, 0xda50, 0x7552, 0x56a0, 0xabb7, 0x25d0, 0x92d0, 0xcab5,
    0xa950, 0xb4a0, 0xbaa4, 0xad50, 0x55d9, 0x4ba0, 0xa5b0, 0x5176, 0x52bf, 0xa930,
    0x7954, 0x6aa0, 0xad50, 0x5b52, 0x4b60, 0xa6e6, 0xa4e0, 0xd260, 0xea65, 0xd530,
    0x5aa0, 0x76a3, 0x96d0, 0x4afb, 0x4ad0, 0xa4d0, 0xd0b6, 0xd25f, 0xd520, 0xdd45,
    0xb5a0, 0x56d0, 0x55b2, 0x49b0, 0xa577, 0xa4b0, 0xaa50, 0xb255, 0x6d2f, 0xada0,
    0x4b63, 0x937f, 0x49f8, 0x4970, 0x64b0, 0x68a6, 0xea5f, 0x6b20, 0xa6c4, 0xaaef,
    0x92e0, 0xd2e3, 0xc960, 0xd557, 0xd4a0, 0xda50, 0x5d55, 0x56a0, 0xa6d0, 0x55d4,
    0x52d0, 0xa9b8, 0xa950, 0xb4a0, 0xb6a6, 0xad50, 0x55a0, 0xaba4, 0xa5b0, 0x52b0,
    0xb273, 0x6930, 0x7337, 0x6aa0, 0xad50, 0x4b55, 0x4b6f, 0xa570, 0x54e4, 0xd260,
    0xe968, 0xd520, 0xdaa0, 0x6aa6, 0x56df, 0x4ae0, 0xa9d4, 0xa4d0, 0xd150, 0xf252,
    0xd520,
};

LunarCalendar::LunarCalendar()
{
    UErrorCode status = U_ZERO_ERROR;
    calendar_ = new icu::GregorianCalendar(status);
    if (U_FAILURE(status)) {
        HILOG_ERROR_I18N("LunarCalendar: create GregorianCalendar failed");
        if (calendar_ != nullptr) {
            delete calendar_;
        }
        calendar_ = nullptr;
    }
}

LunarCalendar::~LunarCalendar()
{
    if (calendar_ != nullptr) {
        delete calendar_;
    }
    calendar_ = nullptr;
}

bool LunarCalendar::SetGregorianDate(int32_t year, int32_t month, int32_t day)
{
    ConvertDate(year, month, day);
    isGregorianLeapYear = false;
    isValidDate = VerifyDate(year, month, day);
    if (!isValidDate) {
        return false;
    }
    solorYear = year;
    solorMonth = month;
    solorDay = day;
    CalcDaysFromBaseDate();
    SolorDateToLunarDate();
    return true;
}

void LunarCalendar::ConvertDate(int32_t& year, int32_t& month, int32_t& day)
{
    if (calendar_ == nullptr) {
        return;
    }
    calendar_->set(year, month - 1, day);
    UErrorCode status = U_ZERO_ERROR;
    int32_t tempYear = calendar_->get(UCAL_YEAR, status);
    if (U_FAILURE(status)) {
        HILOG_ERROR_I18N("ConvertDate: get year failed");
        return;
    }
    int32_t tempMonth = calendar_->get(UCAL_MONTH, status) + 1;
    if (U_FAILURE(status)) {
        HILOG_ERROR_I18N("ConvertDate: get month failed");
        return;
    }
    int32_t tempDay = calendar_->get(UCAL_DATE, status);
    if (U_FAILURE(status)) {
        HILOG_ERROR_I18N("ConvertDate: get day failed");
        return;
    }
    year = tempYear;
    month = tempMonth;
    day = tempDay;
}

void LunarCalendar::CalcDaysFromBaseDate()
{
    daysCounts = DAYS_OF_YEAR * (solorYear - START_YEAR);
    daysCounts += accDaysOfMonth[solorMonth];
    if (isGregorianLeapYear && solorMonth > MONTH_FEB) {
        daysCounts++;
    }
    daysCounts--;
    daysCounts += solorDay;
    daysCounts += (solorYear - START_YEAR) / FREQ_LEAP_YEAR;
    if (isGregorianLeapYear) {
        daysCounts--;
    }
    if (solorYear >= VALID_END_YEAR) {
        daysCounts--;
    }
}

void LunarCalendar::SolorDateToLunarDate()
{
    int32_t daysInPerLunarYear = 0;
    int32_t daysInPerLunarMonth = 0;
    int32_t leapMonth = 0xf;
    int tempDaysCounts = daysCounts;
    tempDaysCounts -= DAYS_FROM_SOLAR_TO_LUNAR;
    int32_t i = 0;
    for (i = START_YEAR; (tempDaysCounts > 0) && (i < END_YEAR); i++) {
        daysInPerLunarYear = GetDaysPerLunarYear(i);
        tempDaysCounts -= daysInPerLunarYear;
    }
    if (tempDaysCounts < 0) {
        tempDaysCounts += daysInPerLunarYear;
        --i;
    }
    lunarYear = i;

    leapMonth = lunarDateInfo[lunarYear - START_YEAR] & 0xf;
    leapMonth = (leapMonth == 0xf) ? 0 : leapMonth;
    isLeapMonth = false;
    for (i = 1; i <= VALID_END_MONTH && tempDaysCounts > 0; i++) {
        if (leapMonth > 0 && (leapMonth + 1) == i && !isLeapMonth) {
            --i;
            isLeapMonth = true;
            daysInPerLunarMonth = ((lunarDateInfo[lunarYear - START_YEAR + 1] & 0xf) == 0xf) ? DAYS_IN_BIG_MONTH :
                DAYS_IN_SMALL_MONTH;
        } else {
            daysInPerLunarMonth = ((lunarDateInfo[lunarYear - START_YEAR] &
                (0x8000 >> (i - 1))) == (0x8000 >> (i - 1))) ? DAYS_IN_BIG_MONTH : DAYS_IN_SMALL_MONTH;
        }
        if (isLeapMonth && (leapMonth + 1) == i) {
            isLeapMonth = false;
        }
        tempDaysCounts -= daysInPerLunarMonth;
    }
    AdjustLeapMonth(i, tempDaysCounts, leapMonth);
    if (tempDaysCounts < 0) {
        tempDaysCounts += daysInPerLunarMonth;
        --i;
    }
    lunarMonth = i;
    lunarDay = tempDaysCounts + 1;
}

void LunarCalendar::AdjustLeapMonth(int32_t& i, int32_t tempDaysCounts, int32_t leapMonth)
{
    if (tempDaysCounts == 0 && leapMonth > 0 && i == leapMonth + 1) {
        if (isLeapMonth) {
            isLeapMonth = false;
        } else {
            isLeapMonth = true;
            --i;
        }
    }
}

int32_t LunarCalendar::GetDaysPerLunarYear(int32_t lunarYear)
{
    int32_t daysPerLunarYear = 0;
    if ((lunarYear < START_YEAR) || (lunarYear > END_YEAR)) {
        return 0;
    }
    daysPerLunarYear += BASE_DAYS_PER_LUNAR_YEAR;
    for (uint32_t i = 0x8000; i > 0x8; i = i >> 1) {
        daysPerLunarYear += ((lunarDateInfo[lunarYear - START_YEAR] & i) == i) ? 1 : 0;
    }
    if (((lunarDateInfo[lunarYear - START_YEAR] & 0xf) != 0) &&
        ((lunarDateInfo[lunarYear - START_YEAR] & 0xf) != 0xf)) {
        daysPerLunarYear += ((lunarDateInfo[lunarYear - START_YEAR + 1] & 0xf) == 0xf) ? DAYS_IN_BIG_MONTH :
            DAYS_IN_SMALL_MONTH;
    }
    return daysPerLunarYear;
}

bool LunarCalendar::VerifyDate(int32_t year, int32_t month, int32_t day)
{
    if ((year < VALID_START_YEAR) || (year > VALID_END_YEAR)) {
        HILOG_ERROR_I18N("VerifyDate: %{public}d is an invalid year", year);
        return false;
    }

    if ((month < VALID_START_MONTH) || (month > VALID_END_MONTH)) {
        HILOG_ERROR_I18N("VerifyDate: %{public}d is an invalid month", month);
        return false;
    }

    int32_t validEndDay = daysOfMonth[month];
    isGregorianLeapYear = IsGregorianLeapYear(year);
    if (month == MONTH_FEB) {
        validEndDay = isGregorianLeapYear ? validEndDay + 1 : validEndDay;
    }

    if ((month < VALID_START_DAY) || (month > validEndDay)) {
        HILOG_ERROR_I18N("VerifyDate: %{public}d is an invalid day", day);
        return false;
    }
    return true;
}

bool LunarCalendar::IsGregorianLeapYear(int32_t year)
{
    if (year % YEAR_ERA == 0) {
        if (year % (YEAR_ERA * FREQ_LEAP_YEAR) == 0) {
            return true;
        }
        return false;
    }
    if (year % FREQ_LEAP_YEAR == 0) {
        return true;
    }
    return false;
}

int32_t LunarCalendar::GetLunarYear()
{
    if (!isValidDate) {
        HILOG_ERROR_I18N("GetLunarYear: invalid date");
        return -1;
    }
    return lunarYear;
}

int32_t LunarCalendar::GetLunarMonth()
{
    if (!isValidDate) {
        HILOG_ERROR_I18N("GetLunarYear: invalid date");
        return -1;
    }
    return lunarMonth;
}

int32_t LunarCalendar::GetLunarDay()
{
    if (!isValidDate) {
        HILOG_ERROR_I18N("GetLunarYear: invalid date");
        return -1;
    }
    return lunarDay;
}

bool LunarCalendar::IsLeapMonth()
{
    if (!isValidDate) {
        HILOG_ERROR_I18N("GetLunarYear: invalid date");
        return false;
    }
    return isLeapMonth;
}
} // namespace I18n
} // namespace Global
} // namespace OHOS