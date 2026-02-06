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
#include "i18n_calendar.h"

#include "buddhcal.h"
#include "chnsecal.h"
#include "coptccal.h"
#include "ethpccal.h"
#include "hebrwcal.h"
#include "indiancal.h"
#include "islamcal.h"
#include "japancal.h"
#include "locale_config.h"
#include "unicode/locdspnm.h"
#include "persncal.h"
#include "string"
#include "ureslocs.h"
#include "ulocimp.h"
#include "unicode/umachine.h"
#include "unicode/gregocal.h"
#include "unicode/timezone.h"
#include "unicode/unistr.h"
#include "unicode/urename.h"
#include "ustr_imp.h"
#include "unicode/ustring.h"
#include "utils.h"

namespace OHOS {
namespace Global {
namespace I18n {
static std::unordered_map<WeekDay, UCalendarDaysOfWeek> WEEK_DAY_TO_ICU {
    { WeekDay::MON, UCalendarDaysOfWeek::UCAL_MONDAY },
    { WeekDay::TUE, UCalendarDaysOfWeek::UCAL_TUESDAY },
    { WeekDay::WED, UCalendarDaysOfWeek::UCAL_WEDNESDAY },
    { WeekDay::THU, UCalendarDaysOfWeek::UCAL_THURSDAY },
    { WeekDay::FRI, UCalendarDaysOfWeek::UCAL_FRIDAY },
    { WeekDay::SAT, UCalendarDaysOfWeek::UCAL_SATURDAY },
    { WeekDay::SUN, UCalendarDaysOfWeek::UCAL_SUNDAY }
};

static std::unordered_map<std::string, UCalendarDaysOfWeek> WEEK_DAY_STR_TO_ICU {
    { "mon", UCalendarDaysOfWeek::UCAL_MONDAY },
    { "tue", UCalendarDaysOfWeek::UCAL_TUESDAY },
    { "wed", UCalendarDaysOfWeek::UCAL_WEDNESDAY },
    { "thu", UCalendarDaysOfWeek::UCAL_THURSDAY },
    { "fri", UCalendarDaysOfWeek::UCAL_FRIDAY },
    { "sat", UCalendarDaysOfWeek::UCAL_SATURDAY },
    { "sun", UCalendarDaysOfWeek::UCAL_SUNDAY }
};

I18nCalendar::I18nCalendar(std::string localeTag)
{
    UErrorCode status = U_ZERO_ERROR;
    icu::Locale tempLocale = icu::Locale::forLanguageTag(localeTag, status);
    if (U_FAILURE(status)) {
        calendar_ = new icu::GregorianCalendar(status);
        if (!U_SUCCESS(status)) {
            if (calendar_ != nullptr) {
                delete calendar_;
            }
            calendar_ = nullptr;
        }
        return;
    }
    calendar_ = icu::Calendar::createInstance(tempLocale, status);
    if (U_FAILURE(status)) {
        if (calendar_ != nullptr) {
            delete calendar_;
        }
        calendar_ = nullptr;
    }
    InitFirstDayOfWeek(localeTag);
}

I18nCalendar::I18nCalendar(std::string localeTag, CalendarType type)
{
    UErrorCode status = U_ZERO_ERROR;
    icu::Locale tempLocale = icu::Locale::forLanguageTag(localeTag, status);
    if (U_FAILURE(status)) {
        calendar_ = new icu::GregorianCalendar(status);
        if (!U_SUCCESS(status)) {
            if (calendar_ != nullptr) {
                delete calendar_;
            }
            calendar_ = nullptr;
        }
        return;
    }
    InitCalendar(tempLocale, type);
    InitFirstDayOfWeek(localeTag);
}

void I18nCalendar::InitCalendar(const icu::Locale &locale, CalendarType type)
{
    UErrorCode status = U_ZERO_ERROR;
    switch (type) {
        case BUDDHIST: {
            calendar_ = new icu::BuddhistCalendar(locale, status);
            break;
        }
        case CHINESE: {
            calendar_ = new icu::ChineseCalendar(locale, status);
            break;
        }
        case COPTIC: {
            calendar_ = new icu::CopticCalendar(locale, status);
            break;
        }
        case ETHIOPIC: {
            calendar_ = new icu::EthiopicCalendar(locale, status);
            break;
        }
        case HEBREW: {
            calendar_ = new icu::HebrewCalendar(locale, status);
            break;
        }
        case INDIAN: {
            calendar_ = new icu::IndianCalendar(locale, status);
            break;
        }
        case ISLAMIC_CIVIL: {
            calendar_ = new icu::IslamicCalendar(locale, status, icu::IslamicCalendar::ECalculationType::CIVIL);
            break;
        }
        default: {
            InitCalendar2(locale, type, status);
        }
    }
    if (!U_SUCCESS(status)) {
        if (calendar_ != nullptr) {
            delete calendar_;
        }
        calendar_ = nullptr;
    }
}

void I18nCalendar::InitCalendar2(const icu::Locale &locale, CalendarType type, UErrorCode &status)
{
    switch (type) {
        case ISLAMIC_TBLA: {
            calendar_ = new icu::IslamicCalendar(locale, status, icu::IslamicCalendar::ECalculationType::TBLA);
            break;
        }
        case ISLAMIC_UMALQURA: {
            calendar_ = new icu::IslamicCalendar(locale, status, icu::IslamicCalendar::ECalculationType::UMALQURA);
            break;
        }
        case JAPANESE: {
            calendar_ = new icu::JapaneseCalendar(locale, status);
            break;
        }
        case PERSIAN: {
            calendar_ = new icu::PersianCalendar(locale, status);
            break;
        }
        case GREGORY: {
            calendar_ = new icu::GregorianCalendar(locale, status);
            break;
        }
        default: {
            calendar_ = icu::Calendar::createInstance(locale, status);
        }
    }
}

I18nCalendar::~I18nCalendar()
{
    if (calendar_ != nullptr) {
        delete calendar_;
    }
}

void I18nCalendar::SetTime(double value)
{
    icu::Calendar* icuCalendar = GetIcuCalendar();
    if (icuCalendar != nullptr) {
        UErrorCode status = U_ZERO_ERROR;
        icuCalendar->setTime(value, status);
        return;
    }
}

void I18nCalendar::SetTimeZone(std::string id)
{
    icu::UnicodeString zone = icu::UnicodeString::fromUTF8(id);
    icu::TimeZone *timezone = icu::TimeZone::createTimeZone(zone);
    if (timezone != nullptr) {
        icu::Calendar* icuCalendar = GetIcuCalendar();
        if (icuCalendar != nullptr) {
            icuCalendar->setTimeZone(*timezone);
        }
        delete(timezone);
    }
}

std::string I18nCalendar::GetTimeZone(void)
{
    std::string ret;
    icu::Calendar* icuCalendar = GetIcuCalendar();
    if (icuCalendar) {
        icu::UnicodeString unistr;
        icuCalendar->getTimeZone().getID(unistr);
        unistr.toUTF8String<std::string>(ret);
    }
    return ret;
}

void I18nCalendar::Set(int32_t year, int32_t month, int32_t date)
{
    icu::Calendar* icuCalendar = GetIcuCalendar();
    if (icuCalendar != nullptr) {
        icuCalendar->set(year, month, date);
        return;
    }
}

void I18nCalendar::Set(UCalendarDateFields field, int32_t value)
{
    icu::Calendar* icuCalendar = GetIcuCalendar();
    if (icuCalendar != nullptr) {
        icuCalendar->set(field, value);
        return;
    }
}

int32_t I18nCalendar::Get(UCalendarDateFields field) const
{
    icu::Calendar* icuCalendar = GetIcuCalendar();
    if (icuCalendar != nullptr) {
        UErrorCode status = U_ZERO_ERROR;
        return icuCalendar->get(field, status);
    }
    return 0;
}

void I18nCalendar::Add(UCalendarDateFields field, int32_t amount)
{
    icu::Calendar* icuCalendar = GetIcuCalendar();
    if (icuCalendar != nullptr) {
        UErrorCode status = U_ZERO_ERROR;
        icuCalendar->add(field, amount, status);
    }
}

void I18nCalendar::SetMinimalDaysInFirstWeek(int32_t value)
{
    icu::Calendar* icuCalendar = GetIcuCalendar();
    if (icuCalendar != nullptr) {
        icuCalendar->setMinimalDaysInFirstWeek((uint8_t)value);
        return;
    }
}

void I18nCalendar::SetFirstDayOfWeek(int32_t value)
{
    if (value < UCalendarDaysOfWeek::UCAL_SUNDAY || value > UCAL_SATURDAY) {
        return;
    }
    icu::Calendar* icuCalendar = GetIcuCalendar();
    if (icuCalendar != nullptr) {
        icuCalendar->setFirstDayOfWeek(UCalendarDaysOfWeek(value));
        return;
    }
}

UDate I18nCalendar::GetTimeInMillis(void)
{
    icu::Calendar* icuCalendar = GetIcuCalendar();
    if (icuCalendar != nullptr) {
        UErrorCode status = U_ZERO_ERROR;
        return icuCalendar->getTime(status);
    }
    return 0;
}

int32_t I18nCalendar::GetMinimalDaysInFirstWeek(void)
{
    icu::Calendar* icuCalendar = GetIcuCalendar();
    if (icuCalendar != nullptr) {
        return icuCalendar->getMinimalDaysInFirstWeek();
    }
    return 1;
}

int32_t I18nCalendar::GetFirstDayOfWeek(void)
{
    icu::Calendar* icuCalendar = GetIcuCalendar();
    if (icuCalendar != nullptr) {
        return static_cast<int>(icuCalendar->getFirstDayOfWeek());
    }
    return UCAL_SUNDAY;
}

bool I18nCalendar::IsWeekend(int64_t date, UErrorCode &status)
{
    icu::Calendar* icuCalendar = GetIcuCalendar();
    if (icuCalendar != nullptr) {
        return icuCalendar->isWeekend(date, status);
    }
    return false;
}

bool I18nCalendar::IsWeekend(void)
{
    icu::Calendar* icuCalendar = GetIcuCalendar();
    if (icuCalendar != nullptr) {
        return icuCalendar->isWeekend();
    }
    return false;
}

std::string I18nCalendar::GetDisplayName(std::string &displayLocaleTag)
{
    icu::Calendar* icuCalendar = GetIcuCalendar();
    if (icuCalendar == nullptr) {
        return PseudoLocalizationProcessor("");
    }
    const char *type = icuCalendar->getType();
    if (type == nullptr) {
        return PseudoLocalizationProcessor("");
    }
    UErrorCode status = U_ZERO_ERROR;
    icu::Locale displayLocale = icu::Locale::forLanguageTag(displayLocaleTag, status);
    if (U_FAILURE(status)) {
        return PseudoLocalizationProcessor("");
    }
    icu::LocaleDisplayNames *dspName = icu::LocaleDisplayNames::createInstance(displayLocale);
    icu::UnicodeString unistr;
    if (dspName != nullptr) {
        dspName->keyValueDisplayName("calendar", type, unistr);
        delete dspName;
    }
    std::string ret;
    unistr.toUTF8String<std::string>(ret);
    return PseudoLocalizationProcessor(ret);
}

int32_t I18nCalendar::CompareDays(UDate date)
{
    icu::Calendar* icuCalendar = GetIcuCalendar();
    if (icuCalendar != nullptr) {
        UErrorCode status = U_ZERO_ERROR;
        UDate nowMs = icuCalendar->getTime(status);
        double ret = (date - nowMs) / (24 * 60 * 60 * 1000); // Convert 24 hours into milliseconds
        return ret > 0 ? std::ceil(ret) : std::floor(ret);
    }
    return 0;
}

icu::Calendar* I18nCalendar::GetIcuCalendar() const
{
    return this->calendar_;
}

void I18nCalendar::InitFirstDayOfWeek(const std::string& localeTag)
{
    icu::Calendar* icuCalendar = GetIcuCalendar();
    if (icuCalendar == nullptr) {
        return;
    }
    std::string param = LocaleConfig::QueryExtParam(localeTag, "fw", "-u-");
    if (param.empty()) {
        return;
    }
    if (WEEK_DAY_STR_TO_ICU.find(param) != WEEK_DAY_STR_TO_ICU.end()) {
        icuCalendar->setFirstDayOfWeek(WEEK_DAY_STR_TO_ICU[param]);
    }
}
} // namespace I18n
} // namespace Global
} // namespace OHOS