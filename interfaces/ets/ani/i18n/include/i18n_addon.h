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

#ifndef I18N_ADDON_H
#define I18N_ADDON_H

#include "ani.h"
#include "i18n_break_iterator.h"
#include "index_util.h"
#include "phone_number_format.h"
#include "unicode/translit.h"

namespace OHOS {
namespace Global {
namespace I18n {
class I18nPhoneNumberFormatAddon {
public:
    static I18nPhoneNumberFormatAddon* UnwrapAddon(ani_env *env, ani_object object);
    static ani_object Create(ani_env *env, [[maybe_unused]] ani_object object, ani_string country, ani_object options);
    static ani_boolean IsValidNumber(ani_env *env, ani_object object, ani_string phoneNumber);
    static ani_string Format(ani_env *env, ani_object object, ani_string phoneNumber);
    static ani_string GetLocationName(ani_env *env, ani_object object, ani_string phoneNumber, ani_string locale);
    static ani_status BindContextPhoneNumberFormat(ani_env *env);
private:
    std::unique_ptr<PhoneNumberFormat> phonenumberfmt_ = nullptr;
};

class I18nBreakIteratorAddon {
public:
    static I18nBreakIteratorAddon* UnwrapAddon(ani_env *env, ani_object object);
    static ani_object GetLineInstance(ani_env *env, ani_string locale);
    static ani_int Current(ani_env *env, ani_object object);
    static ani_int First(ani_env *env, ani_object object);
    static ani_int Last(ani_env *env, ani_object object);
    static ani_int Next(ani_env *env, ani_object object, ani_object index);
    static ani_int Previous(ani_env *env, ani_object object);
    static void SetLineBreakText(ani_env *env, ani_object object, ani_string text);
    static ani_int Following(ani_env *env, ani_object object, ani_int offset);
    static ani_string GetLineBreakText(ani_env *env, ani_object object);
    static ani_boolean IsBoundary(ani_env *env, ani_object object, ani_int offset);
    static ani_status BindContextBreakIterator(ani_env *env);
private:
    std::unique_ptr<I18nBreakIterator> brkiter_ = nullptr;
};

class I18nIndexUtilAddon {
public:
    static I18nIndexUtilAddon* UnwrapAddon(ani_env *env, ani_object object);
    static ani_object GetInstance(ani_env *env, ani_string string);
    static ani_object GetIndexList(ani_env *env, ani_object object);
    static void AddLocale(ani_env *env, ani_object object, ani_string locale);
    static ani_object GetIndex(ani_env *env, ani_object object, ani_string text);
    static ani_status BindContextIndexUtil(ani_env *env);
private:
    std::unique_ptr<IndexUtil> indexUtil_ = nullptr;
};

class I18nTransliteratorAddon {
public:
    static I18nTransliteratorAddon* UnwrapAddon(ani_env *env, ani_object object);
    static ani_object GetAvailableIDs(ani_env *env, [[maybe_unused]] ani_object object);
    static ani_object GetInstance(ani_env *env, [[maybe_unused]] ani_object object, ani_string id);
    static ani_string Transform(ani_env *env, ani_object object, ani_string text);
    static ani_status BindContextTransliterator(ani_env *env);
private:
    std::unique_ptr<icu::Transliterator> transliterator_ = nullptr;
};

class I18nAddon {
public:
    static ani_boolean IsRTL(ani_env *env, ani_string locale);
    static ani_string UnitConvert(ani_env *env, [[maybe_unused]] ani_object object, ani_object fromUnit,
        ani_object toUnit, ani_double value, ani_string locale, ani_string style);
    static ani_string GetDateOrder(ani_env *env, [[maybe_unused]] ani_object object, ani_string locale);
    static ani_string GetTimePeriodName(ani_env *env, [[maybe_unused]] ani_object object,
        ani_int hour, ani_string locale);
    static ani_string GetBestMatchLocale(ani_env *env, [[maybe_unused]] ani_object object,
        ani_string locale, ani_object localeList);
    static ani_string GetThreeLetterLanguage(ani_env *env, [[maybe_unused]] ani_object object, ani_string locale);
    static ani_string GetThreeLetterRegion(ani_env *env, [[maybe_unused]] ani_object object, ani_string locale);
    static ani_string GetUnicodeWrappedFilePath(ani_env *env, [[maybe_unused]] ani_object object,
        ani_string path, ani_string delimiter, ani_object locale);
    static ani_status BindContextI18NUtil(ani_env *env);
    static ani_status BindContextNamespace(ani_env *env);
};
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif
