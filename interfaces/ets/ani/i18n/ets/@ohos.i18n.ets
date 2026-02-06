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

import intl from '@ohos.intl';

namespace i18n {
    export enum WeekDay {
        MON = 1,
        TUE = 2,
        WED = 3,
        THU = 4,
        FRI = 5,
        SAT = 6,
        SUN = 7
    }

    export enum TemperatureType {
        CELSIUS = 1,
        FAHRENHEIT = 2,
        KELVIN = 3
    }

    export enum NormalizerMode {
        NFC = 1,
        NFD = 2,
        NFKC = 3,
        NFKD = 4
    }

    export enum SuggestionType {
        SUGGESTION_TYPE_NONE = 0,
        SUGGESTION_TYPE_RELATED = 1,
        SUGGESTION_TYPE_SIM = 2,
    }

    export interface UnitInfo {
        unit: string;
        measureSystem: string;
    }

    export class UnitInfoInner implements UnitInfo {
        unit: string;
        measureSystem: string;
    }

    export interface PhoneNumberFormatOptions {
        type?: string;
    }

    export class PhoneNumberFormatOptionsInner implements PhoneNumberFormatOptions {
        type?: string;
    }

    export interface SortOptions {
        locale?: string;
        isUseLocalName?: boolean;
        isSuggestedFirst?: boolean;
    }

    export class SortOptionsInner implements SortOptions {
        locale?: string;
        isUseLocalName?: boolean;
        isSuggestedFirst?: boolean;
    }

    export interface LocaleItem {
        id: string;
        suggestionType: SuggestionType;
        displayName: string;
        localName?: string;
    }

    export class LocaleItemInner implements LocaleItem {
        id: string;
        suggestionType: SuggestionType;
        displayName: string;
        localName?: string;
    }

    export interface TimeZoneCityItem {
        zoneId: string;
        cityId: string;
        cityDisplayName: string;
        offset: number;
        zoneDisplayName: string;
        rawOffset?: number;
    }

    export class TimeZoneCityItemInner implements TimeZoneCityItem {
        zoneId: string;
        cityId: string;
        cityDisplayName: string;
        offset: number;
        zoneDisplayName: string;
        rawOffset?: number;
    }

    export interface HolidayInfoItem {
        baseName: string;
        year: number;
        month: number;
        day: number;
        localNames?: Array<HolidayLocalName>;
    }

    export class HolidayInfoItemInner implements HolidayInfoItem {
        baseName: string;
        year: number;
        month: number;
        day: number;
        localNames?: Array<HolidayLocalName>;
    }

    export interface HolidayLocalName {
        language: string;
        name: string;
    }

    export class HolidayLocalNameInner implements HolidayLocalName {
        language: string;
        name: string;
    }

    export interface EntityInfoItem {
        begin: number;
        end: number;
        type: string;
    }

    export class EntityInfoItemInner implements EntityInfoItem {
        begin: number;
        end: number;
        type: string;
    }

    export class System {
        static { loadLibrary("i18n_ani.z"); }

        native static getDisplayCountry(country: string, locale: string, sentenceCase?: boolean): string;
        native static getDisplayLanguage(language: string, locale: string, sentenceCase?: boolean): string;
        native static getSystemLanguages(): Array<string>;
        native static getSystemCountries(language: string): Array<string>;
        native static isSuggested(language: string, region?: string): boolean;
        native static getSystemLanguage(): string;
        native static setSystemLanguage(language: string): void;
        native static getSystemRegion(): string;
        native static setSystemRegion(region: string): void;
        native static getSystemLocale(): string;
        native static setSystemLocale(locale: string): void;
        native static is24HourClock(): boolean;
        native static set24HourClock(option: boolean): void;
        native static addPreferredLanguage(language: string, index?: number): void;
        native static removePreferredLanguage(index: number): void;
        native static getPreferredLanguageList(): Array<string>;
        native static getFirstPreferredLanguage(): string;
        native static setAppPreferredLanguage(language: string): void;
        native static getAppPreferredLanguage(): string;
        native static setUsingLocalDigit(flag: boolean): void;
        native static getUsingLocalDigit(): boolean;
        native static getSimplifiedLanguage(language?: string): string;
        native static setTemperatureType(type: TemperatureType): void;
        native static getTemperatureType(): TemperatureType;
        native static getTemperatureName(type: TemperatureType): string;
        native static setFirstDayOfWeek(type: WeekDay): void;
        native static getFirstDayOfWeek(): WeekDay;

        public static getSystemLocaleInstance(): Intl.Locale {
            let systemLocaleStr = System.getSystemLocale();
            let systemLocale = new Intl.Locale(systemLocaleStr);
            return systemLocale;
        }
    }

    export class I18NUtil {
        static { loadLibrary("i18n_ani.z"); }

        native static unitConvert(fromUnit: UnitInfo, toUnit: UnitInfo,
            value: number, locale: string, style?: string): string;
        native static getDateOrder(locale: string): string;
        native static getTimePeriodName(hour: number, locale?: string): string;
        native static getBestMatchLocale(locale: string, localeList: string[]): string;
        native static getThreeLetterLanguage(locale: string): string;
        native static getThreeLetterRegion(locale: string): string;
    }

    export class PhoneNumberFormat {
        static { loadLibrary("i18n_ani.z"); }

        private nativePhoneNumberFormat: long = 0;

        constructor(context: long) {
            if (this.nativePhoneNumberFormat == 0) {
                this.nativePhoneNumberFormat = context;
            }
        }

        constructor(country: string, options?: PhoneNumberFormatOptions) {
            this.nativePhoneNumberFormat = PhoneNumberFormat.create(country, options).nativePhoneNumberFormat;
        }

        native static create(country: string, options?: PhoneNumberFormatOptions): PhoneNumberFormat;
        native isValidNumber(number: string): boolean;
        native format(number: string): string;
        native getLocationName(number: string, locale: string): string;
    }

    export class Calendar {
        static { loadLibrary("i18n_ani.z"); }

        private nativeCalendar: long = 0;

        constructor(context: long) {
            if (this.nativeCalendar == 0) {
                this.nativeCalendar = context;
            }
        }

        native setTime(date: Date): void;
        native setTime(time: number): void;
        native set(year: number, month: number, date: number, hour?: number, minute?: number, second?: number): void;
        native setTimeZone(timezone: string): void;
        native getTimeZone(): string;
        native getFirstDayOfWeek(): number;
        native setFirstDayOfWeek(value: number): void;
        native getMinimalDaysInFirstWeek(): number;
        native setMinimalDaysInFirstWeek(value: number): void;
        native get(field: string): number;
        native getDisplayName(locale: string): string;
        native isWeekend(date?: Date): boolean;
        native add(field: string, amount: number): void;
        native getTimeInMillis(): number;
        native compareDays(date: Date): number;
    }

    export class BreakIterator {
        static { loadLibrary("i18n_ani.z"); }

        private nativeBreakIterator: long = 0;

        constructor(context: long) {
            if (this.nativeBreakIterator == 0) {
                this.nativeBreakIterator = context;
            }
        }

        native current(): number;
        native first(): number;
        native last(): number;
        native next(index?: number): number;
        native previous(): number;
        native setLineBreakText(text: string): void;
        native following(offset: number): number;
        native getLineBreakText(): string;
        native isBoundary(offset: number): boolean;
    }

    export class IndexUtil {
        static { loadLibrary("i18n_ani.z"); }

        private nativeIndexUtil: long = 0;

        constructor(context: long) {
            if (this.nativeIndexUtil == 0) {
                this.nativeIndexUtil = context;
            }
        }

        native getIndexList(): Array<string>;
        native addLocale(locale: string): void;
        native getIndex(text: string): string;
    }

    export class Unicode {
        static { loadLibrary("i18n_ani.z"); }

        native static isDigit(ch: string): boolean;
        native static isSpaceChar(ch: string): boolean;
        native static isWhitespace(ch: string): boolean;
        native static isRTL(ch: string): boolean;
        native static isIdeograph(ch: string): boolean;
        native static isLetter(ch: string): boolean;
        native static isLowerCase(ch: string): boolean;
        native static isUpperCase(ch: string): boolean;
        native static getType(ch: string): string;
    }

    export class TimeZone {
        static { loadLibrary("i18n_ani.z"); }

        private nativeTimeZone: long = 0;

        constructor(context: long) {
            if (this.nativeTimeZone == 0) {
                this.nativeTimeZone = context;
            }
        }

        native getID(): string;
        native getDisplayName(locale?: string, isDST?: boolean): string;
        native getRawOffset(): number;
        native getOffset(date?: number): number;
        native static getAvailableIDs(): Array<string>;
        native static getAvailableZoneCityIDs(): Array<string>;
        native static getCityDisplayName(cityID: string, locale: string): string;
        native static getTimezoneFromCity(cityID: string): TimeZone;
        native static getTimezonesByLocation(longitude: number, latitude: number): Array<TimeZone>;
    }

    export class Transliterator {
        static { loadLibrary("i18n_ani.z"); }

        private nativeTransliterator: long = 0;

        constructor(context: long) {
            if (this.nativeTransliterator == 0) {
                this.nativeTransliterator = context;
            }
        }

        native static getAvailableIDs(): string[];
        native static getInstance(id: string): Transliterator;
        native transform(text: string): string;
    }

    export class Normalizer {
        static { loadLibrary("i18n_ani.z"); }

        private nativeNormalizer: long = 0;

        constructor(context: long) {
            if (this.nativeNormalizer == 0) {
                this.nativeNormalizer = context;
            }
        }

        native static getInstance(mode: NormalizerMode): Normalizer;
        native normalize(text: string): string;
    }

    export class SystemLocaleManager {
        static { loadLibrary("i18n_ani.z"); }

        private nativeSysLocaleMgr: long = 0;

        constructor(context: long) {
            if (this.nativeSysLocaleMgr == 0) {
                this.nativeSysLocaleMgr = context;
            }
        }

        constructor() {
            this.nativeSysLocaleMgr = SystemLocaleManager.create().nativeSysLocaleMgr;
        }

        native static create(): SystemLocaleManager;
        native getLanguageInfoArray(languages: Array<string>, options?: SortOptions): Array<LocaleItem>;
        native getRegionInfoArray(regions: Array<string>, options?: SortOptions): Array<LocaleItem>;
        native static getTimeZoneCityItemArray(): Array<TimeZoneCityItem>;
    }

    export class HolidayManager {
        static { loadLibrary("i18n_ani.z"); }

        private nativeHolidayManager: long = 0;

        constructor(context: long) {
            if (this.nativeHolidayManager == 0) {
                this.nativeHolidayManager = context;
            }
        }

        constructor(icsPath: String) {
            this.nativeHolidayManager = HolidayManager.create(icsPath).nativeHolidayManager;
        }

        native static create(icsPath: String): HolidayManager;
        native isHoliday(date?: Date): boolean;
        native getHolidayInfoItemArray(year?: number): Array<HolidayInfoItem>;
    }

    export class EntityRecognizer {
        static { loadLibrary("i18n_ani.z"); }

        private nativeEntityRecognizer: long = 0;

        constructor(context: long) {
            if (this.nativeEntityRecognizer == 0) {
                this.nativeEntityRecognizer = context;
            }
        }

        constructor(locale?: string) {
            this.nativeEntityRecognizer = EntityRecognizer.create(locale).nativeEntityRecognizer;
        }

        native static create(locale?: string): EntityRecognizer;
        native findEntityInfo(text: string): Array<EntityInfoItem>;
    }

    export class SimpleDateTimeFormat {
        static { loadLibrary("i18n_ani.z"); }

        private nativeSimpleDateTimeFormat: long = 0;

        constructor(context: long) {
            if (this.nativeSimpleDateTimeFormat == 0) {
                this.nativeSimpleDateTimeFormat = context;
            }
        }

        native format(date: Date): string;
    }

    export class SimpleNumberFormat {
        static { loadLibrary("i18n_ani.z"); }

        private nativeSimpleNumberFormat: long = 0;

        constructor(context: long) {
            if (this.nativeSimpleNumberFormat == 0) {
                this.nativeSimpleNumberFormat = context;
            }
        }

        native format(value: number): string;
    }

    loadLibrary("i18n_ani.z");
    export native function getCalendar(locale: string, type?: string): Calendar;
    export native function isRTL(locale: string): boolean;
    export native function getLineInstance(locale: string): BreakIterator;
    export native function getInstance(locale?: string): IndexUtil;
    export native function getTimeZone(zoneID?: string): TimeZone;
}
