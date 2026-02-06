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
#include "intl_ffi.h"
#include "intl_impl.h"
#include <string>

using namespace OHOS::FFI;

namespace OHOS {
namespace Global {
namespace I18n {
namespace Intl {
extern "C" {
char *FfiOHOSNumberFormatImplFormat(int64_t id, double number)
{
    auto instance = FFIData::GetData<NumberFormatImpl>(id);
    if (!instance) {
        HILOG_ERROR_I18N("The NumberFormatImpl instance is nullptr");
        return nullptr;
    }
    return instance->Format(number);
}

int64_t FfiOHOSNumberFormatImplConstructor(int32_t *errCode)
{
    auto nativeCJNumberFormat = FFIData::Create<NumberFormatImpl>(errCode);
    if (!nativeCJNumberFormat) {
        return 0;
    }
    return nativeCJNumberFormat->GetID();
}

int64_t FfiOHOSNumberFormatImplConstructorwithLocale(
    char *locale, CNumberOptions options, int64_t flag, int32_t *errCode)
{
    auto nativeCJNumberFormat = FFIData::Create<NumberFormatImpl>(locale, options, flag, errCode);
    if (!nativeCJNumberFormat) {
        return 0;
    }
    return nativeCJNumberFormat->GetID();
}

int64_t FfiOHOSNumberFormatImplConstructorwithArrayLocale(
    CArrString locale, CNumberOptions options, int64_t flag, int32_t *errCode)
{
    auto nativeCJNumberFormat = FFIData::Create<NumberFormatImpl>(locale, options, flag, errCode);
    if (!nativeCJNumberFormat) {
        return 0;
    }
    return nativeCJNumberFormat->GetID();
}

CNumberOptions FfiOHOSNumberFormatImplGetNumberResolvedOptions(int64_t id)
{
    auto instance = FFIData::GetData<NumberFormatImpl>(id);
    struct CNumberOptions numberOptions = { nullptr };
    if (!instance) {
        HILOG_ERROR_I18N("The NumberFormatImpl instance is nullptr");
        return numberOptions;
    }
    return instance->ResolveOptions();
}

int64_t FfiOHOSRelativeTimeFormatImplConstructor(int32_t *errCode)
{
    auto nativeCJRelativeTimeFormat = FFIData::Create<RelativeTimeFormatImpl>(errCode);
    if (!nativeCJRelativeTimeFormat) {
        return 0;
    }
    return nativeCJRelativeTimeFormat->GetID();
}

int64_t FfiOHOSRelativeTimeFormatImplConstructorwithLocale(
    char *locale, CRelativeTimeFormatInputOptions options, int64_t flag, int32_t *errCode)
{
    auto nativeCJRelativeTimeFormat = FFIData::Create<RelativeTimeFormatImpl>(locale, options, flag, errCode);
    if (!nativeCJRelativeTimeFormat) {
        return 0;
    }
    return nativeCJRelativeTimeFormat->GetID();
}

int64_t FfiOHOSRelativeTimeFormatImplConstructorwithArrayLocale(
    CArrString locale, CRelativeTimeFormatInputOptions options, int64_t flag, int32_t *errCode)
{
    auto nativeCJRelativeTimeFormat = FFIData::Create<RelativeTimeFormatImpl>(locale, options, flag, errCode);
    if (!nativeCJRelativeTimeFormat) {
        return 0;
    }
    return nativeCJRelativeTimeFormat->GetID();
}

char *FfiOHOSRelativeTimeFormatImplFormat(int64_t id, double number, char *unit)
{
    auto instance = FFIData::GetData<RelativeTimeFormatImpl>(id);
    if (!instance) {
        HILOG_ERROR_I18N("The RelativeTimeFormatImpl instance is nullptr");
        return nullptr;
    }
    return instance->Format(number, unit);
}

CArrArrString FfiOHOSRelativeTimeFormatImplFormatToParts(int64_t id, double number, char *unit)
{
    auto instance = FFIData::GetData<RelativeTimeFormatImpl>(id);
    if (!instance) {
        CArrArrString ret = { nullptr };
        HILOG_ERROR_I18N("The RelativeTimeFormatImpl instance is nullptr");
        return ret;
    }
    return instance->FormatToParts(number, unit);
}

CRelativeTimeFormatResolveOptions FfiOHOSRelativeTimeFormatImplResolvedOptions(int64_t id)
{
    auto instance = FFIData::GetData<RelativeTimeFormatImpl>(id);
    if (!instance) {
        struct CRelativeTimeFormatResolveOptions relativeTimeFormatResolveOptions = { nullptr };
        HILOG_ERROR_I18N("The RelativeTimeFormatImpl instance is nullptr");
        return relativeTimeFormatResolveOptions;
    }
    return instance->ResolveOptions();
}

int64_t FfiOHOSPluralRulesImplConstructor(int32_t *errCode)
{
    auto nativeCJPluralRules = FFIData::Create<PluralRulesImpl>(errCode);
    if (!nativeCJPluralRules) {
        return 0;
    }
    return nativeCJPluralRules->GetID();
}

int64_t FfiOHOSPluralRulesImplConstructorwithLocale(
    char *locale, CPluralRulesOptions options, int64_t flag, int32_t *errCode)
{
    auto nativeCJPluralRules = FFIData::Create<PluralRulesImpl>(locale, options, flag, errCode);
    if (!nativeCJPluralRules) {
        return 0;
    }
    return nativeCJPluralRules->GetID();
}

int64_t FfiOHOSPluralRulesImplConstructorwithArrayLocale(
    CArrString locale, CPluralRulesOptions options, int64_t flag, int32_t *errCode)
{
    auto nativeCJPluralRules = FFIData::Create<PluralRulesImpl>(locale, options, flag, errCode);
    if (!nativeCJPluralRules) {
        return 0;
    }
    return nativeCJPluralRules->GetID();
}

char *FfiOHOSPluralRulesImplSelect(int64_t id, double n)
{
    auto instance = FFIData::GetData<PluralRulesImpl>(id);
    if (!instance) {
        HILOG_ERROR_I18N("The PluralRulesImpl instance is nullptr");
        return nullptr;
    }
    return instance->Select(n);
}

int64_t FfiOHOSCollatorImplConstructor(int32_t *errCode)
{
    auto nativeCJCollator = FFIData::Create<CollatorImpl>(errCode);
    if (!nativeCJCollator) {
        return 0;
    }
    return nativeCJCollator->GetID();
}

int64_t FfiOHOSCollatorImplConstructorwithLocale(char *locale, CCollatorOptions options, int64_t flag, int32_t *errCode)
{
    auto nativeCJCollator = FFIData::Create<CollatorImpl>(locale, options, flag, errCode);
    if (!nativeCJCollator) {
        return 0;
    }
    return nativeCJCollator->GetID();
}

int64_t FfiOHOSCollatorImplConstructorwithArrayLocale(
    CArrString locale, CCollatorOptions options, int64_t flag, int32_t *errCode)
{
    auto nativeCJCollator = FFIData::Create<CollatorImpl>(locale, options, flag, errCode);
    if (!nativeCJCollator) {
        return 0;
    }
    return nativeCJCollator->GetID();
}

CCollatorOptions FfiOHOSCollatorOptionsImplResolvedOptions(int64_t id)
{
    auto instance = FFIData::GetData<CollatorImpl>(id);
    if (!instance) {
        struct CCollatorOptions collatorResolveOptions = { nullptr };
        HILOG_ERROR_I18N("The CollatorImpl instance is nullptr");
        return collatorResolveOptions;
    }
    return instance->ResolveOptions();
}

int32_t FfiOHOSCollatorImplCompare(int64_t id, char *str1, char *str2)
{
    auto instance = FFIData::GetData<CollatorImpl>(id);
    if (!instance) {
        HILOG_ERROR_I18N("The CollatorImpl instance is nullptr");
        return 0;
    }
    return instance->Compare(str1, str2);
}

int64_t FfiOHOSDateTimeFormatImplConstructor(int32_t *errCode)
{
    auto nativeCJDateTimeFormat = FFIData::Create<DateTimeFormatImpl>(errCode);
    if (!nativeCJDateTimeFormat) {
        return 0;
    }
    return nativeCJDateTimeFormat->GetID();
}

int64_t FfiOHOSDateTimeFormatImplConstructorwithLocale(
    char *locale, CDateTimeOptions options, int64_t flag, int32_t *errCode)
{
    auto nativeCJDateTimeFormat = FFIData::Create<DateTimeFormatImpl>(locale, options, flag, errCode);
    if (!nativeCJDateTimeFormat) {
        return 0;
    }
    return nativeCJDateTimeFormat->GetID();
}

int64_t FfiOHOSDateTimeFormatImplConstructorwithArrayLocale(
    CArrString locale, CDateTimeOptions options, int64_t flag, int32_t *errCode)
{
    auto nativeCJDateTimeFormat = FFIData::Create<DateTimeFormatImpl>(locale, options, flag, errCode);
    if (!nativeCJDateTimeFormat) {
        return 0;
    }
    return nativeCJDateTimeFormat->GetID();
}

CDateTimeOptions FfiOHOSDateTimeFormatOptionsImplResolvedOptions(int64_t id)
{
    auto instance = FFIData::GetData<DateTimeFormatImpl>(id);
    if (!instance) {
        struct CDateTimeOptions dateTimeOptions = { nullptr };
        HILOG_ERROR_I18N("The DateTimeFormatImpl instance is nullptr");
        return dateTimeOptions;
    }
    return instance->ResolveOptions();
}

char *FfiOHOSDateTimeFormatOptionsImplFormat(int64_t id, int64_t date)
{
    auto instance = FFIData::GetData<DateTimeFormatImpl>(id);
    if (!instance) {
        HILOG_ERROR_I18N("The DateTimeFormatImpl instance is nullptr");
        return nullptr;
    }
    return instance->Format(date);
}

char *FfiOHOSDateTimeFormatOptionsImplFormatRange(int64_t id, int64_t startDate, int64_t endDate)
{
    auto instance = FFIData::GetData<DateTimeFormatImpl>(id);
    if (!instance) {
        HILOG_ERROR_I18N("The DateTimeFormatImpl instance is nullptr");
        return nullptr;
    }
    return instance->FormatRange(startDate, endDate);
}

int64_t FfiOHOSLocaleImplConstructor(int32_t *errCode)
{
    auto nativeCJLocale = FFIData::Create<LocaleImpl>(errCode);
    if (!nativeCJLocale) {
        return 0;
    }

    return nativeCJLocale->GetID();
}

int64_t FfiOHOSLocaleImplConstructorwithLocale(char *locale, CLocaleOptions options, int64_t flag, int32_t *errCode)
{
    auto nativeCJLocale = FFIData::Create<LocaleImpl>(locale, options, flag, errCode);
    if (!nativeCJLocale) {
        return 0;
    }
    return nativeCJLocale->GetID();
}

char *FfiOHOSLocaleImplToString(int64_t id)
{
    auto instance = FFIData::GetData<LocaleImpl>(id);
    if (!instance) {
        HILOG_ERROR_I18N("The LocaleImpl instance is nullptr");
        return nullptr;
    }
    return instance->ToString();
}

char *FfiOHOSLocaleImplMaximize(int64_t id)
{
    auto instance = FFIData::GetData<LocaleImpl>(id);
    if (!instance) {
        HILOG_ERROR_I18N("The LocaleImpl instance is nullptr");
        return nullptr;
    }
    return instance->Maximize();
}

char *FfiOHOSLocaleImplMinimize(int64_t id)
{
    auto instance = FFIData::GetData<LocaleImpl>(id);
    if (!instance) {
        HILOG_ERROR_I18N("The LocaleImpl instance is nullptr");
        return nullptr;
    }
    return instance->Minimize();
}

char *FfiOHOSLocaleImplGetLanguage(int64_t id)
{
    auto instance = FFIData::GetData<LocaleImpl>(id);
    if (!instance) {
        HILOG_ERROR_I18N("The LocaleImpl instance is nullptr");
        return nullptr;
    }
    return instance->GetLanguage();
}

char *FfiOHOSLocaleImplGetRegion(int64_t id)
{
    auto instance = FFIData::GetData<LocaleImpl>(id);
    if (!instance) {
        HILOG_ERROR_I18N("The LocaleImpl instance is nullptr");
        return nullptr;
    }
    return instance->GetRegion();
}

char *FfiOHOSLocaleImplGetScript(int64_t id)
{
    auto instance = FFIData::GetData<LocaleImpl>(id);
    if (!instance) {
        HILOG_ERROR_I18N("The LocaleImpl instance is nullptr");
        return nullptr;
    }
    return instance->GetScript();
}

char *FfiOHOSLocaleImplGetBaseName(int64_t id)
{
    auto instance = FFIData::GetData<LocaleImpl>(id);
    if (!instance) {
        HILOG_ERROR_I18N("The LocaleImpl instance is nullptr");
        return nullptr;
    }
    return instance->GetBaseName();
}

char *FfiOHOSLocaleImplGetCaseFirst(int64_t id)
{
    auto instance = FFIData::GetData<LocaleImpl>(id);
    if (!instance) {
        HILOG_ERROR_I18N("The LocaleImpl instance is nullptr");
        return nullptr;
    }
    return instance->GetCaseFirst();
}

char *FfiOHOSLocaleImplGetCalendar(int64_t id)
{
    auto instance = FFIData::GetData<LocaleImpl>(id);
    if (!instance) {
        HILOG_ERROR_I18N("The LocaleImpl instance is nullptr");
        return nullptr;
    }
    return instance->GetCalendar();
}

char *FfiOHOSLocaleImplGetCollation(int64_t id)
{
    auto instance = FFIData::GetData<LocaleImpl>(id);
    if (!instance) {
        HILOG_ERROR_I18N("The LocaleImpl instance is nullptr");
        return nullptr;
    }
    return instance->GetCollation();
}

char *FfiOHOSLocaleImplGetHourCycle(int64_t id)
{
    auto instance = FFIData::GetData<LocaleImpl>(id);
    if (!instance) {
        HILOG_ERROR_I18N("The LocaleImpl instance is nullptr");
        return nullptr;
    }
    return instance->GetHourCycle();
}

char *FfiOHOSLocaleImplGetNumberingSystem(int64_t id)
{
    auto instance = FFIData::GetData<LocaleImpl>(id);
    if (!instance) {
        HILOG_ERROR_I18N("The LocaleImpl instance is nullptr");
        return nullptr;
    }
    return instance->GetNumberingSystem();
}

char *FfiOHOSLocaleImplGetNumeric(int64_t id)
{
    auto instance = FFIData::GetData<LocaleImpl>(id);
    if (!instance) {
        HILOG_ERROR_I18N("The LocaleImpl instance is nullptr");
        return nullptr;
    }
    return instance->GetNumeric();
}
}
}  // namespace Intl
}  // namespace I18n
}  // namespace Global
}  // namespace OHOS