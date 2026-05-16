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
#ifndef INTL_IMPL_H
#define INTL_IMPL_H

#include <string>
#include <vector>
#include "intl_ffi.h"
#include "ffi_remote_data.h"
#include "preferences_observer.h"

namespace OHOS {
namespace Global {
namespace I18n {
namespace Intl {
std::vector<std::string> ArrayStringToVectorString(CArrString arrString);
char *IMallocCString(const std::string &origin);
class NumberFormatImpl : public OHOS::FFI::FFIData {
    DECL_TYPE(NumberFormatImpl, OHOS::FFI::FFIData)
public:
    explicit NumberFormatImpl(int32_t *errCode);
    NumberFormatImpl(char *locale, CNumberOptions options, int64_t flag, int32_t *errCode);
    NumberFormatImpl(CArrString locale, CNumberOptions options, int64_t flag, int32_t *errCode);
    char *Format(double number);
    CNumberOptions ResolveOptions();

private:
    std::unique_ptr<OHOS::Global::I18n::NumberFormat> numberFmt_ = nullptr;
};

class RelativeTimeFormatImpl : public OHOS::FFI::FFIData {
    DECL_TYPE(RelativeTimeFormatImpl, OHOS::FFI::FFIData);

public:
    explicit RelativeTimeFormatImpl(int32_t *errCode);
    RelativeTimeFormatImpl(char *locale, CRelativeTimeFormatInputOptions options, int64_t flag, int32_t *errCode);
    RelativeTimeFormatImpl(
        CArrString locale, CRelativeTimeFormatInputOptions options, int64_t flag, int32_t *errCode);
    char *Format(double number, char *unit);
    CArrArrString FormatToParts(double number, char *unit);
    CRelativeTimeFormatResolveOptions ResolveOptions();

private:
    std::unique_ptr<OHOS::Global::I18n::RelativeTimeFormat> relativeTimeFmt_ = nullptr;
};

class PluralRulesImpl : public OHOS::FFI::FFIData {
    DECL_TYPE(PluralRulesImpl, OHOS::FFI::FFIData);

public:
    explicit PluralRulesImpl(int32_t *errCode);
    PluralRulesImpl(char *locale, CPluralRulesOptions options, int64_t flag, int32_t *errCode);
    PluralRulesImpl(CArrString locale, CPluralRulesOptions options, int64_t flag, int32_t *errCode);
    char *Select(double n);

private:
    std::unique_ptr<OHOS::Global::I18n::PluralRules> pluralRules_ = nullptr;
};

class CollatorImpl : public OHOS::FFI::FFIData {
    DECL_TYPE(CollatorImpl, OHOS::FFI::FFIData);

public:
    explicit CollatorImpl(int32_t *errCode);
    CollatorImpl(char *locale, CCollatorOptions options, int64_t flag, int32_t *errCode);
    CollatorImpl(CArrString locale, CCollatorOptions options, int64_t flag, int32_t *errCode);
    CCollatorOptions ResolveOptions();
    int32_t Compare(char *str1, char *str2);

private:
    std::unique_ptr<OHOS::Global::I18n::Collator> collator_ = nullptr;
};

class DateTimeFormatImpl : public OHOS::FFI::FFIData {
    DECL_TYPE(DateTimeFormatImpl, OHOS::FFI::FFIData);

public:
    explicit DateTimeFormatImpl(int32_t *errCode);
    DateTimeFormatImpl(char *locale, CDateTimeOptions options, int64_t flag, int32_t *errCode);
    DateTimeFormatImpl(CArrString locale, CDateTimeOptions options, int64_t flag, int32_t *errCode);
    CDateTimeOptions ResolveOptions();
    char *Format(int64_t date);
    char *FormatRange(int64_t startDate, int64_t endDate);
    std::map<std::string, std::string> MapInsert(CDateTimeOptions options);

private:
    std::unique_ptr<OHOS::Global::I18n::DateTimeFormat> dateFmt_ = nullptr;
};

class LocaleImpl : public OHOS::FFI::FFIData {
    DECL_TYPE(LocaleImpl, OHOS::FFI::FFIData);

public:
    explicit LocaleImpl(int32_t *errCode);
    LocaleImpl(char *locale, CLocaleOptions options, int64_t flag, int32_t *errCode);
    char *ToString();
    char *Maximize();
    char *Minimize();

    char *GetLanguage();
    char *GetRegion();
    char *GetScript();
    char *GetBaseName();
    char *GetCaseFirst();
    char *GetCalendar();
    char *GetCollation();
    char *GetHourCycle();
    char *GetNumberingSystem();
    char *GetNumeric();

private:
    std::unique_ptr<OHOS::Global::I18n::LocaleInfo> locale_ = nullptr;
};

}  // namespace Intl
}  // namespace I18n
}  // namespace Global
}  // namespace OHOS
#endif