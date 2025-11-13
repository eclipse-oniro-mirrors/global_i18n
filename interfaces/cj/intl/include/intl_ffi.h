/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef INTL_FFI_H
#define INTL_FFI_H

#include "number_format.h"
#include "relative_time_format.h"
#include "plural_rules.h"
#include "collator.h"
#include "date_time_format.h"
#include "locale_info.h"
#include <cstdint>
#include <string>
#include "cj_common_ffi.h"
#include "ffi_remote_data.h"
#include "hilog/log.h"
#include "i18n_hilog.h"

#define FFI_EXPORT __attribute__((visibility("default")))

extern "C" {
struct CNumberOptions {
    char *locale;
    char *currency;
    char *currencySign;
    char *currencyDisplay;
    char *unit;
    char *unitDisplay;
    char *unitUsage;
    char *signDispaly;
    char *compactDisplay;
    char *notation;
    char *localeMather;
    char *style;
    char *numberingSystem;
    bool useGrouping;
    int64_t minimumIntegerDigits;
    int64_t minimumFractionDigits;
    int64_t maximumFractionDigits;
    int64_t minimumSignificantDigits;
    int64_t maximumSignificantDigits;
};

struct CRelativeTimeFormatInputOptions {
    char *localeMatcher;
    char *numeric;
    char *style;
};

struct CRelativeTimeFormatResolveOptions {
    char *localeMatcher;
    char *numeric;
    char *style;
    char *numberingSystem;
};

struct CPluralRulesOptions {
    char *localeMatcher;
    char *type;
    int32_t minimumIntegerDigits;
    int32_t minimumFractionDigits;
    int32_t maximumFractionDigits;
    int32_t minimumSignificantDigits;
    int32_t maximumSignificantDigits;
};

struct CCollatorOptions {
    char *localeMatcher;
    char *usage;
    char *sensitivity;
    bool ignorePunctuation;
    char *collation;
    bool numeric;
    char *caseFirst;
};

struct CLocaleOptions {
    char *calendar;
    char *collation;
    char *hourCycle;
    char *numberingSystem;
    bool numeric;
    char *caseFirst;
};

struct CDateTimeOptions {
    char *locale;
    char *dateStyle;
    char *timeStyle;
    char *hourCycle;
    char *timeZone;
    char *numberingSystem;
    bool hour12;
    char *weekday;
    char *era;
    char *year;
    char *month;
    char *day;
    char *hour;
    char *minute;
    char *second;
    char *timeZoneName;
    char *dayPeriod;
    char *localeMatcher;
    char *formatMatcher;
};

struct CArrArrString {
    CArrString *head;
    int64_t size;
};

// NumberFormat
FFI_EXPORT int64_t FfiOHOSNumberFormatImplConstructor(int32_t *errCode);
FFI_EXPORT int64_t FfiOHOSNumberFormatImplConstructorwithLocale(
    char *locale, CNumberOptions, int64_t flag, int32_t *errCode);
FFI_EXPORT int64_t FfiOHOSNumberFormatImplConstructorwithArrayLocale(
    CArrString locale, CNumberOptions options, int64_t flag, int32_t *errCode);
FFI_EXPORT char *FfiOHOSNumberFormatImplFormat(int64_t id, double number);
FFI_EXPORT CNumberOptions FfiOHOSNumberFormatImplGetNumberResolvedOptions(int64_t id);

// RelativeTimeFormat
FFI_EXPORT int64_t FfiOHOSRelativeTimeFormatImplConstructor(int32_t *errCode);
FFI_EXPORT int64_t FfiOHOSRelativeTimeFormatImplConstructorwithLocale(
    char *locale, CRelativeTimeFormatInputOptions options, int64_t flag, int32_t *errCode);
FFI_EXPORT int64_t FfiOHOSRelativeTimeFormatImplConstructorwithArrayLocale(
    CArrString locale, CRelativeTimeFormatInputOptions options, int64_t flag, int32_t *errCode);
FFI_EXPORT char *FfiOHOSRelativeTimeFormatImplFormat(int64_t id, double number, char *unit);
FFI_EXPORT CArrArrString FfiOHOSRelativeTimeFormatImplFormatToParts(int64_t id, double number, char *unit);
FFI_EXPORT CRelativeTimeFormatResolveOptions FfiOHOSRelativeTimeFormatImplResolvedOptions(int64_t id);

//  PluralRules
FFI_EXPORT int64_t FfiOHOSPluralRulesImplConstructor(int32_t *errCode);
FFI_EXPORT int64_t FfiOHOSPluralRulesImplConstructorwithLocale(
    char *locale, CPluralRulesOptions options, int64_t flag, int32_t *errCode);
FFI_EXPORT int64_t FfiOHOSPluralRulesImplConstructorwithArrayLocale(
    CArrString locale, CPluralRulesOptions options, int64_t flag, int32_t *errCode);
FFI_EXPORT char *FfiOHOSPluralRulesImplSelect(int64_t id, double n);

// Collator
FFI_EXPORT int64_t FfiOHOSCollatorImplConstructor(int32_t *errCode);
FFI_EXPORT int64_t FfiOHOSCollatorImplConstructorwithLocale(
    char *locale, CCollatorOptions options, int64_t flag, int32_t *errCode);
FFI_EXPORT int64_t FfiOHOSCollatorImplConstructorwithArrayLocale(
    CArrString locale, CCollatorOptions options, int64_t flag, int32_t *errCode);
FFI_EXPORT int32_t FfiOHOSCollatorImplCompare(int64_t id, char *first, char *second);
FFI_EXPORT CCollatorOptions FfiOHOSCollatorOptionsImplResolvedOptions(int64_t id);

// DateTimeFormat
FFI_EXPORT int64_t FfiOHOSDateTimeFormatImplConstructor(int32_t *errCode);
FFI_EXPORT int64_t FfiOHOSDateTimeFormatImplConstructorwithLocale(
    char *locale, CDateTimeOptions options, int64_t flag, int32_t *errCode);
FFI_EXPORT int64_t FfiOHOSDateTimeFormatImplConstructorwithArrayLocale(
    CArrString locale, CDateTimeOptions options, int64_t flag, int32_t *errCode);
FFI_EXPORT CDateTimeOptions FfiOHOSDateTimeFormatOptionsImplResolvedOptions(int64_t id);
FFI_EXPORT char *FfiOHOSDateTimeFormatOptionsImplFormat(int64_t id, int64_t date);
FFI_EXPORT char *FfiOHOSDateTimeFormatOptionsImplFormatRange(int64_t id, int64_t startDate, int64_t endDate);

// Locale
FFI_EXPORT int64_t FfiOHOSLocaleImplConstructor(int32_t *errCode);
FFI_EXPORT int64_t FfiOHOSLocaleImplConstructorwithLocale(
    char *locale, CLocaleOptions options, int64_t flag, int32_t *errCode);
FFI_EXPORT char *FfiOHOSLocaleImplToString(int64_t id);
FFI_EXPORT char *FfiOHOSLocaleImplMinimize(int64_t id);
FFI_EXPORT char *FfiOHOSLocaleImplMaximize(int64_t id);

FFI_EXPORT char *FfiOHOSLocaleImplGetLanguage(int64_t id);
FFI_EXPORT char *FfiOHOSLocaleImplGetScript(int64_t id);
FFI_EXPORT char *FfiOHOSLocaleImplGetRegion(int64_t id);
FFI_EXPORT char *FfiOHOSLocaleImplGetBaseName(int64_t id);
FFI_EXPORT char *FfiOHOSLocaleImplGetCaseFirst(int64_t id);
FFI_EXPORT char *FfiOHOSLocaleImplGetCalendar(int64_t id);
FFI_EXPORT char *FfiOHOSLocaleImplGetCollation(int64_t id);
FFI_EXPORT char *FfiOHOSLocaleImplGetHourCycle(int64_t id);
FFI_EXPORT char *FfiOHOSLocaleImplGetNumberingSystem(int64_t id);
FFI_EXPORT char *FfiOHOSLocaleImplGetNumeric(int64_t id);
}

#endif