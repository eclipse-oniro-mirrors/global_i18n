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

namespace intl {
    export interface DateTimeOptions {
        locale?: string;
        dateStyle?: string;
        timeStyle?: string;
        hourCycle?: string;
        timeZone?: string;
        numberingSystem?: string;
        hour12?: boolean;
        weekday?: string;
        era?: string;
        year?: string;
        month?: string;
        day?: string;
        hour?: string;
        minute?: string;
        second?: string;
        timeZoneName?: string;
        dayPeriod?: string;
        localeMatcher?: string;
        formatMatcher?: string;
    }

    export class DateTimeOptionsInner implements DateTimeOptions {
        locale?: string;
        dateStyle?: string;
        timeStyle?: string;
        hourCycle?: string;
        timeZone?: string;
        numberingSystem?: string;
        hour12?: boolean;
        weekday?: string;
        era?: string;
        year?: string;
        month?: string;
        day?: string;
        hour?: string;
        minute?: string;
        second?: string;
        timeZoneName?: string;
        dayPeriod?: string;
        localeMatcher?: string;
        formatMatcher?: string;
    }

    export interface NumberOptions {
        locale?: string
        currency?: string
        currencySign?: string
        currencyDisplay?: string
        unit?: string
        unitDisplay?: string
        unitUsage?: string
        signDisplay?: string
        compactDisplay?: string
        notation?: string
        localeMatcher?: string
        style?: string
        numberingSystem?: string
        useGrouping?: boolean;
        minimumIntegerDigits?: number;
        minimumFractionDigits?: number;
        maximumFractionDigits?: number;
        minimumSignificantDigits?: number;
        maximumSignificantDigits?: number;
        roundingPriority?: string
        roundingIncrement?: number;
        roundingMode?: string
    }

    export class NumberOptionsInner implements NumberOptions {
        locale?: string;
        currency?: string;
        currencySign?: string;
        currencyDisplay?: string;
        unit?: string;
        unitDisplay?: string;
        unitUsage?: string;
        signDisplay?: string;
        compactDisplay?: string;
        notation?: string;
        localeMatcher?: string;
        style?: string;
        numberingSystem?: string;
        useGrouping?: boolean;
        minimumIntegerDigits?: number;
        minimumFractionDigits?: number;
        maximumFractionDigits?: number;
        minimumSignificantDigits?: number;
        maximumSignificantDigits?: number;
        roundingPriority?: string;
        roundingIncrement?: number;
        roundingMode?: string;
    }

    export interface CollatorOptions {
        localeMatcher?: string;
        usage?: string;
        sensitivity?: string;
        ignorePunctuation?: boolean;
        collation?: string;
        numeric?: boolean;
        caseFirst?: string;
    }

    export class CollatorOptionsInner implements CollatorOptions {
        localeMatcher?: string;
        usage?: string;
        sensitivity?: string;
        ignorePunctuation?: boolean;
        collation?: string;
        numeric?: boolean;
        caseFirst?: string;
    }

    export class DateTimeFormat {
        static { loadLibrary("intl_ani.z"); }

        private nativeDateTimeFormat: long;

        constructor(context: long) {
            if (this.nativeDateTimeFormat == 0) {
                this.nativeDateTimeFormat = context;
            }
        }

        constructor() {
            this.nativeDateTimeFormat = DateTimeFormat.create().nativeDateTimeFormat;
        }

        constructor(locale: string | Array<string>, options?: DateTimeOptions) {
            this.nativeDateTimeFormat = DateTimeFormat.create(locale, options).nativeDateTimeFormat;
        }

        native static create(): DateTimeFormat;
        native static create(locale: string | Array<string>, options?: DateTimeOptions): DateTimeFormat;
        native format(date: Date): string;
        native formatRange(startDate: Date, endDate: Date): string;
        native resolvedOptions(): DateTimeOptions;
    }

    export class NumberFormat {
        static { loadLibrary("intl_ani.z"); }

        private nativeNumberFormat: long;

        constructor(context: long) {
            if (this.nativeNumberFormat == 0) {
                this.nativeNumberFormat = context;
            }
        }

        constructor() {
            this.nativeNumberFormat = NumberFormat.create().nativeNumberFormat;
        }

        constructor(locale: string | Array<string>, options?: NumberOptions) {
            this.nativeNumberFormat = NumberFormat.create(locale, options).nativeNumberFormat;
        }

        native static create(): NumberFormat;
        native static create(locale: string | Array<string>, options?: NumberOptions): NumberFormat;
        native format(number: number): string;
        native formatRange(startRange: number, endRange: number): string;
        native resolvedOptions(): NumberOptions;
    }

    export class Collator {
        static { loadLibrary("intl_ani.z"); }

        private nativeCollator: long;

        constructor(context: long) {
            if (this.nativeCollator == 0) {
                this.nativeCollator = context;
            }
        }

        constructor() {
            this.nativeCollator = Collator.create().nativeCollator;
        }

        constructor(locale: string | Array<string>, options?: CollatorOptions) {
            this.nativeCollator = Collator.create(locale, options).nativeCollator;
        }

        native static create(): Collator;
        native static create(locale: string | Array<string>, options?: CollatorOptions): Collator;
        native compare(first: string, second: string): number;
        native resolvedOptions(): CollatorOptions;
    }
}
