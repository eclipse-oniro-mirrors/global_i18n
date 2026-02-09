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

import { TextStyle, StyledString, StyleOptions, StyledStringKey } from 'arkui.component.styledString';

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

    export enum UnitUsage {
        AREA_LAND_AGRICULT = 1,
        AREA_LAND_COMMERCL = 2,
        AREA_LAND_RESIDNTL = 3,
        LENGTH_PERSON = 4,
        LENGTH_PERSON_SMALL = 5,
        LENGTH_RAINFALL = 6,
        LENGTH_ROAD = 7,
        LENGTH_ROAD_SMALL = 8,
        LENGTH_SNOWFALL = 9,
        LENGTH_VEHICLE = 10,
        LENGTH_VISIBLTY = 11,
        LENGTH_VISIBLTY_SMALL = 12,
        LENGTH_PERSON_INFORMAL = 13,
        LENGTH_PERSON_SMALL_INFORMAL = 14,
        LENGTH_ROAD_INFORMAL = 15,
        SPEED_ROAD_TRAVEL = 16,
        SPEED_WIND = 17,
        TEMPERATURE_PERSON = 18,
        TEMPERATURE_WEATHER = 19,
        VOLUME_VEHICLE_FUEL = 20,
        ELAPSED_TIME_SECOND = 21,
        SIZE_FILE_BYTE = 22,
        SIZE_SHORTFILE_BYTE = 23
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
        offset: int;
        zoneDisplayName: string;
        rawOffset?: int;
    }

    export class TimeZoneCityItemInner implements TimeZoneCityItem {
        zoneId: string;
        cityId: string;
        cityDisplayName: string;
        offset: int;
        zoneDisplayName: string;
        rawOffset?: int;
    }

    export interface HolidayInfoItem {
        baseName: string;
        year: int;
        month: int;
        day: int;
        localNames?: Array<HolidayLocalName>;
    }

    export class HolidayInfoItemInner implements HolidayInfoItem {
        baseName: string;
        year: int;
        month: int;
        day: int;
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
        begin: int;
        end: int;
        type: string;
    }

    export class EntityInfoItemInner implements EntityInfoItem {
        begin: int;
        end: int;
        type: string;
    }

    export interface StyledNumberFormatOptions {
        integer?: TextStyle;
        decimal?: TextStyle;
        fraction?: TextStyle;
        unit?: TextStyle;
    }

    export interface StyledDateTimeFormatOptions {
        year?: TextStyle;
        month?: TextStyle;
        day?: TextStyle;
        hour?: TextStyle;
        minute?: TextStyle;
        second?: TextStyle;
        weekday?: TextStyle;
        era?: TextStyle;
        timeZoneName?: TextStyle;
        dayPeriod?: TextStyle;
    }

    class StyledPartInner {
        constructor(start: int, length: int, type: string) {
            this.start = start;
            this.length = length;
            this.type = type;
        }

        constructor() {}

        start: int;
        length: int;
        type: string;
    }

    class StyledFormatResult {
        styledParts: Array<StyledPartInner>;
        content: string;
    }

    export interface AdvancedMeasureFormatOptions {
        unitUsage?: UnitUsage;
    }

    export class System {
        static { loadLibraryWithPermissionCheck("i18n_ani.z", "@ohos.i18n"); }

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
        native static addPreferredLanguage(language: string, index?: int): void;
        native static removePreferredLanguage(index: int): void;
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
        native static getSystemCollations(): Map<string, string>;
        native static getUsingCollation(): string;
        native static setSystemCollation(identifier: string): void;
        native static getSystemMeasurements(): Map<string, string>;
        native static getUsingMeasurement(): string;
        native static setSystemMeasurement(identifier: string): void;
        native static getUsingNumberingSystem(): string;
        native static setSystemNumberingSystem(identifier: string): void;
        native static getSystemNumberingSystems(): Map<string, string>;
        native static getSystemNumberPatterns(): Map<string, string>;
        native static setSystemNumberPattern(pattern: string): void;
        native static getUsingNumberPattern(): string;
        native static getSystemNumericalDatePatterns(): Map<string, string>;
        native static setSystemNumericalDatePattern(identifier: string): void;
        native static getUsingNumericalDatePattern(): string;
        native static getSystemLocaleInstance(): Intl.Locale;
    }

    export class I18NUtil {
        static { loadLibraryWithPermissionCheck("i18n_ani.z", "@ohos.i18n"); }

        native static unitConvert(fromUnit: UnitInfo, toUnit: UnitInfo,
            value: double, locale: string, style?: string): string;
        native static getDateOrder(locale: string): string;
        native static getTimePeriodName(hour: int, locale?: string): string;
        native static getBestMatchLocale(locale: string, localeList: string[]): string;
        native static getThreeLetterLanguage(locale: string): string;
        native static getThreeLetterRegion(locale: string): string;
        native static getUnicodeWrappedFilePath(path: string, delimiter?: string, locale?: Intl.Locale): string;
    }

    export class PhoneNumberFormat {
        static { loadLibraryWithPermissionCheck("i18n_ani.z", "@ohos.i18n"); }

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
        native isValidNumber(phoneNumber: string): boolean;
        native format(phoneNumber: string): string;
        native getLocationName(phoneNumber: string, locale: string): string;
    }

    export class Calendar {
        static { loadLibraryWithPermissionCheck("i18n_ani.z", "@ohos.i18n"); }

        private nativeCalendar: long = 0;

        constructor(context: long) {
            if (this.nativeCalendar == 0) {
                this.nativeCalendar = context;
            }
        }

        native setTime(date: Date): void;
        native setTime(time: double): void;
        native set(year: int, month: int, date: int, hour?: int, minute?: int, second?: int): void;
        native setTimeZone(timezone: string): void;
        native getTimeZone(): string;
        native getFirstDayOfWeek(): int;
        native setFirstDayOfWeek(value: int): void;
        native getMinimalDaysInFirstWeek(): int;
        native setMinimalDaysInFirstWeek(value: int): void;
        native get(field: string): int;
        native getDisplayName(locale: string): string;
        native isWeekend(date?: Date): boolean;
        native add(field: string, amount: int): void;
        native getTimeInMillis(): long;
        native compareDays(date: Date): int;
    }

    export class BreakIterator {
        static { loadLibraryWithPermissionCheck("i18n_ani.z", "@ohos.i18n"); }

        private nativeBreakIterator: long = 0;

        constructor(context: long) {
            if (this.nativeBreakIterator == 0) {
                this.nativeBreakIterator = context;
            }
        }

        native current(): int;
        native first(): int;
        native last(): int;
        native next(index?: int): int;
        native previous(): int;
        native setLineBreakText(text: string): void;
        native following(offset: int): int;
        native getLineBreakText(): string;
        native isBoundary(offset: int): boolean;
    }

    export class IndexUtil {
        static { loadLibraryWithPermissionCheck("i18n_ani.z", "@ohos.i18n"); }

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
        static { loadLibraryWithPermissionCheck("i18n_ani.z", "@ohos.i18n"); }

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

    export class ZoneOffsetTransition {
        static { loadLibraryWithPermissionCheck("i18n_ani.z", "@ohos.i18n"); }

        private nativeZoneOffsetTransition: long = 0;

        constructor(context: long) {
            if (this.nativeZoneOffsetTransition == 0) {
                this.nativeZoneOffsetTransition = context;
            }
        }

        native getMilliseconds(): double;
        native getOffsetAfter(): int;
        native getOffsetBefore(): int;
    }

    export class ZoneRules {
        static { loadLibraryWithPermissionCheck("i18n_ani.z", "@ohos.i18n"); }

        private nativeZoneRules: long = 0;

        constructor(context: long) {
            if (this.nativeZoneRules == 0) {
                this.nativeZoneRules = context;
            }
        }

        native nextTransition(date?: double): ZoneOffsetTransition;
    }

    export class TimeZone {
        static { loadLibraryWithPermissionCheck("i18n_ani.z", "@ohos.i18n"); }

        private nativeTimeZone: long = 0;

        constructor(context: long) {
            if (this.nativeTimeZone == 0) {
                this.nativeTimeZone = context;
            }
        }

        native getID(): string;
        native getDisplayName(locale?: string, isDST?: boolean): string;
        native getRawOffset(): int;
        native getOffset(date?: double): int;
        native getZoneRules(): ZoneRules;
        native static getAvailableIDs(): Array<string>;
        native static getAvailableZoneCityIDs(): Array<string>;
        native static getCityDisplayName(cityID: string, locale: string): string;
        native static getTimezoneFromCity(cityID: string): TimeZone;
        native static getTimezonesByLocation(longitude: double, latitude: double): Array<TimeZone>;
    }

    export class Transliterator {
        static { loadLibraryWithPermissionCheck("i18n_ani.z", "@ohos.i18n"); }

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
        static { loadLibraryWithPermissionCheck("i18n_ani.z", "@ohos.i18n"); }

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
        static { loadLibraryWithPermissionCheck("i18n_ani.z", "@ohos.i18n"); }

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
        static { loadLibraryWithPermissionCheck("i18n_ani.z", "@ohos.i18n"); }

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
        native getHolidayInfoItemArray(year?: int): Array<HolidayInfoItem>;
    }

    export class EntityRecognizer {
        static { loadLibraryWithPermissionCheck("i18n_ani.z", "@ohos.i18n"); }

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
        static { loadLibraryWithPermissionCheck("i18n_ani.z", "@ohos.i18n"); }

        private nativeSimpleDateTimeFormat: long = 0;

        constructor(context: long) {
            if (this.nativeSimpleDateTimeFormat == 0) {
                this.nativeSimpleDateTimeFormat = context;
            }
        }

        native format(date: Date): string;
    }

    export class SimpleNumberFormat {
        static { loadLibraryWithPermissionCheck("i18n_ani.z", "@ohos.i18n"); }

        private nativeSimpleNumberFormat: long = 0;

        constructor(context: long) {
            if (this.nativeSimpleNumberFormat == 0) {
                this.nativeSimpleNumberFormat = context;
            }
        }

        native format(value: double): string;
    }

    export class StyledNumberFormat {
        static { loadLibraryWithPermissionCheck("i18n_ani.z", "@ohos.i18n"); }

        private nativeStyledNumberFormat: long = 0;
        private styledOptions: StyledNumberFormatOptions = {};

        constructor(context: long) {
            if (this.nativeStyledNumberFormat == 0) {
                this.nativeStyledNumberFormat = context;
            }
        }

        constructor(numberFormat: Intl.NumberFormat | SimpleNumberFormat, options?: StyledNumberFormatOptions) {
            this.nativeStyledNumberFormat = StyledNumberFormat.create(numberFormat).nativeStyledNumberFormat;
            if (options != undefined) {
                this.styledOptions = options;
            }
        }

        native static create(numberFormat: Intl.NumberFormat | SimpleNumberFormat): StyledNumberFormat;
        native getFormattedNumber(value: double): string;
        native getStyle(value: double): Array<StyledPartInner>;
        format(value: double): StyledString {
            let str = this.getFormattedNumber(value);
            let style = this.getStyle(value);
            let option: Array<StyleOptions> = new Array<StyleOptions>();
            for (let i = 0; i < style.length; i++) {
                if (style[i].type == 'integer' && this.styledOptions.integer != undefined) {
                    let styleOption: StyleOptions = {
                        start: style[i].start,
                        length: style[i].length,
                        styledKey: StyledStringKey.FONT,
                        styledValue: this.styledOptions.integer as TextStyle
                    };
                    option.push(styleOption);
                }
                if (style[i].type == 'decimal' && this.styledOptions.decimal != undefined) {
                    let styleOption: StyleOptions = {
                        start: style[i].start,
                        length: style[i].length,
                        styledKey: StyledStringKey.FONT,
                        styledValue: this.styledOptions.decimal as TextStyle
                    };
                    option.push(styleOption);
                }
                if (style[i].type == 'fraction' && this.styledOptions.fraction != undefined) {
                    let styleOption: StyleOptions = {
                        start: style[i].start,
                        length: style[i].length,
                        styledKey: StyledStringKey.FONT,
                        styledValue: this.styledOptions.fraction as TextStyle
                    };
                    option.push(styleOption);
                }
                if (style[i].type == 'unit' && this.styledOptions.unit != undefined) {
                    let styleOption: StyleOptions = {
                        start: style[i].start,
                        length: style[i].length,
                        styledKey: StyledStringKey.FONT,
                        styledValue: this.styledOptions.unit as TextStyle
                    };
                    option.push(styleOption);
                }
            }
            return new StyledString(str, option);
        }
    }

    export class StyledDateTimeFormat {
        static { loadLibraryWithPermissionCheck("i18n_ani.z", "@ohos.i18n"); }

        private nativeStyledDateTimeFormat: long = 0;
        private styledOptionsMap = new Map<string, TextStyle | undefined>();
        private dateTimeFormat: Intl.DateTimeFormat;
        private isIntlDateTimeFormat: boolean = false;

        constructor(context: long) {
            if (this.nativeStyledDateTimeFormat == 0) {
                this.nativeStyledDateTimeFormat = context;
            }
        }

        constructor(dateTimeFormat: Intl.DateTimeFormat | SimpleDateTimeFormat,
            options?: StyledDateTimeFormatOptions) {
            this.isIntlDateTimeFormat = dateTimeFormat instanceof Intl.DateTimeFormat;
            if (this.isIntlDateTimeFormat) {
                this.dateTimeFormat = dateTimeFormat as Intl.DateTimeFormat;
            } else {
                this.nativeStyledDateTimeFormat =
                    StyledDateTimeFormat.create(dateTimeFormat).nativeStyledDateTimeFormat;
            }

            if (options != undefined) {
                this.styledOptionsMap.set('year', options.year);
                this.styledOptionsMap.set('month', options.month);
                this.styledOptionsMap.set('day', options.day);
                this.styledOptionsMap.set('hour', options.hour);
                this.styledOptionsMap.set('minute', options.minute);
                this.styledOptionsMap.set('second', options.second);
                this.styledOptionsMap.set('weekday', options.weekday);
                this.styledOptionsMap.set('era', options.era);
                this.styledOptionsMap.set('timeZoneName', options.timeZoneName);
                this.styledOptionsMap.set('dayPeriod', options.dayPeriod);
            }
        }

        native static create(dateTimeFormat: Intl.DateTimeFormat | SimpleDateTimeFormat): StyledDateTimeFormat;
        native formatToPats(date: Date): StyledFormatResult;

        format(date: Date): StyledString {
            let formatResult = "";
            let formatParts: Array<StyledPartInner>;
            if (this.isIntlDateTimeFormat) {
                formatParts = new Array<StyledPartInner>();
                let parts = this.dateTimeFormat.formatToParts(date);
                for (let i = 0; i < parts.length; i++) {
                    let styledPartInner = new StyledPartInner(formatResult.length, parts[i].value.length,
                        parts[i].type);
                    formatParts.push(styledPartInner);
                    formatResult += parts[i].value;
                }
            } else {
                let StyledFormatResult = this.formatToPats(date);
                formatResult = StyledFormatResult.content;
                formatParts = StyledFormatResult.styledParts;
            }

            let option: Array<StyleOptions> = new Array<StyleOptions>();
            for (let i = 0; i < formatParts.length; i++) {
                let styled = this.styledOptionsMap.get(formatParts[i].type);
                if (styled != undefined) {
                    let styleOption: StyleOptions = {
                        start: formatParts[i].start,
                        length: formatParts[i].length,
                        styledKey: StyledStringKey.FONT,
                        styledValue: styled as TextStyle
                    };
                    option.push(styleOption);
                }
            }
            return new StyledString(formatResult, option);
        }
    }

    export class AdvancedMeasureFormat {
        static { loadLibraryWithPermissionCheck("i18n_ani.z", "@ohos.i18n"); }

        private nativeAdvancedMeasureFormat: long = 0;

        constructor(context: long) {
            if (this.nativeAdvancedMeasureFormat == 0) {
                this.nativeAdvancedMeasureFormat = context;
            }
        }

        constructor(numberFormat: Intl.NumberFormat, options: AdvancedMeasureFormatOptions) {
            this.nativeAdvancedMeasureFormat =
                AdvancedMeasureFormat.create(numberFormat, options).nativeAdvancedMeasureFormat;
        }

        native static create(numberFormat: Intl.NumberFormat,
            options: AdvancedMeasureFormatOptions): AdvancedMeasureFormat;
        native formatNumber(value: double): string;

        format(num: double): string {
            return this.formatNumber(num);
        }
    }

    loadLibraryWithPermissionCheck("i18n_ani.z", "@ohos.i18n");
    export native function getCalendar(locale: string, type?: string): Calendar;
    export native function isRTL(locale: string): boolean;
    export native function getLineInstance(locale: string): BreakIterator;
    export native function getInstance(locale?: string): IndexUtil;
    export native function getTimeZone(zoneID?: string): TimeZone;
    export native function getSimpleDateTimeFormatByPattern(pattern: string,
        locale?: Intl.Locale): SimpleDateTimeFormat;
    export native function getSimpleDateTimeFormatBySkeleton(skeleton: string,
        locale?: Intl.Locale): SimpleDateTimeFormat;
    export native function getSimpleNumberFormatBySkeleton(skeleton: string, locale?: Intl.Locale): SimpleNumberFormat;
}
