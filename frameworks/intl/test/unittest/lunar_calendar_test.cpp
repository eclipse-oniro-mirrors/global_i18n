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
#include "lunar_calendar_test.h"

#include "i18n_calendar.h"
#include "lunar_calendar.h"

using namespace OHOS::Global::I18n;
using testing::ext::TestSize;
using namespace std;

namespace OHOS {
namespace Global {
namespace I18n {
static const int32_t YEAR_ERA = 100;
static const int32_t FREQ_LEAP_YEAR = 4;

void LunarCalendarTest::SetUpTestCase(void)
{}

void LunarCalendarTest::TearDownTestCase(void)
{}

void LunarCalendarTest::SetUp(void)
{}

void LunarCalendarTest::TearDown(void)
{}


bool IsGregorianLeapYear(int32_t year)
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

/**
 * @tc.name: LunarCalendarTest0001
 * @tc.desc: Test Intl Chinese Calendar
 * @tc.type: FUNC
 */
HWTEST_F(LunarCalendarTest, LunarCalendarTest0001, TestSize.Level1)
{
    int32_t monthDays[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    LunarCalendar lunarCalendar;
    I18nCalendar i18nCalendar("zh-CN", CalendarType::CHINESE);
    I18nCalendar gregoryI18nCalendar("zh-CN", CalendarType::GREGORY);
    for (int32_t year = 1901; year < 2101; year++) {
        for (int32_t month = 1; month < 13; month++) {
            int32_t monthDay = monthDays[month - 1];
            monthDay += IsGregorianLeapYear(year) && month == 2 ? 1 : 0;
            for (int32_t date = 1; date <= monthDay; date++) {
                lunarCalendar.SetGregorianDate(year, month, date);
                gregoryI18nCalendar.Set(year, month - 1, date);

                // Test millsecond to lunar date
                i18nCalendar.SetTime(gregoryI18nCalendar.GetTimeInMillis());
                int32_t lunarEra = i18nCalendar.Get(UCAL_ERA);
                int32_t lunarYear = i18nCalendar.Get(UCAL_YEAR);
                int32_t lunarMonth = i18nCalendar.Get(UCAL_MONTH);
                int32_t lunarDate = i18nCalendar.Get(UCAL_DATE);
                int32_t lunarDoy = i18nCalendar.Get(UCAL_DAY_OF_YEAR);
                int32_t lunarDow = i18nCalendar.Get(UCAL_DAY_OF_WEEK);
                int32_t lunarDowIm = i18nCalendar.Get(UCAL_DAY_OF_WEEK_IN_MONTH);
                int32_t lunarYow = i18nCalendar.Get(UCAL_YEAR_WOY);
                int32_t lunarDowLocal = i18nCalendar.Get(UCAL_DOW_LOCAL);
                int32_t lunarexTendYear = i18nCalendar.Get(UCAL_EXTENDED_YEAR);
                int32_t lunarexJulianDay = i18nCalendar.Get(UCAL_JULIAN_DAY);
                int32_t lunarIsLeapMonth = i18nCalendar.Get(UCAL_IS_LEAP_MONTH);
                EXPECT_EQ(lunarMonth, lunarCalendar.GetLunarMonth() - 1);
                EXPECT_EQ(lunarDate, lunarCalendar.GetLunarDay());
                EXPECT_EQ(lunarIsLeapMonth, lunarCalendar.IsLeapMonth());

                // Test set year month day to lunar date
                i18nCalendar.Add(UCAL_YEAR, 1);
                i18nCalendar.Set(lunarYear, lunarMonth, lunarDate);
                EXPECT_EQ(lunarEra, i18nCalendar.Get(UCAL_ERA));
                EXPECT_EQ(lunarYear, i18nCalendar.Get(UCAL_YEAR));
                EXPECT_EQ(lunarMonth, i18nCalendar.Get(UCAL_MONTH));
                EXPECT_EQ(lunarDate, i18nCalendar.Get(UCAL_DATE));
                EXPECT_EQ(lunarDoy, i18nCalendar.Get(UCAL_DAY_OF_YEAR));
                EXPECT_EQ(lunarDow, i18nCalendar.Get(UCAL_DAY_OF_WEEK));
                EXPECT_EQ(lunarDowIm, i18nCalendar.Get(UCAL_DAY_OF_WEEK_IN_MONTH));
                EXPECT_EQ(lunarYow, i18nCalendar.Get(UCAL_YEAR_WOY));
                EXPECT_EQ(lunarDowLocal, i18nCalendar.Get(UCAL_DOW_LOCAL));
                EXPECT_EQ(lunarexTendYear, i18nCalendar.Get(UCAL_EXTENDED_YEAR));
                EXPECT_EQ(lunarexJulianDay, i18nCalendar.Get(UCAL_JULIAN_DAY));
                EXPECT_EQ(lunarIsLeapMonth, i18nCalendar.Get(UCAL_IS_LEAP_MONTH));
            }
        }
    }
}
} // namespace I18n
} // namespace Global
} // namespace OHOS