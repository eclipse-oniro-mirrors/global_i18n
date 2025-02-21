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
#ifndef I18N_ADDON_H
#define I18N_ADDON_H

#include <string>
#include <unordered_map>
#include "napi/native_api.h"
#include "i18n_break_iterator.h"
#include "i18n_normalizer.h"
#include "index_util.h"
#include "napi/native_node_api.h"
#include "locale_config.h"
#include "locale_info.h"
#include "locale_matcher.h"
#include "measure_data.h"
#include "number_format.h"
#include "phone_number_format.h"
#include "preferred_language.h"

namespace OHOS {
namespace Global {
namespace I18n {
void GetOptionMap(napi_env env, napi_value argv, std::map<std::string, std::string> &map);
void GetOptionValue(napi_env env, napi_value options, const std::string &optionName,
    std::string &value);

class I18nAddon {
public:
    static napi_value Init(napi_env env, napi_value exports);
    static void Destructor(napi_env env, void *nativeObject, void *finalize_hint);
    I18nAddon();
    virtual ~I18nAddon();
    static napi_value IsRTL(napi_env env, napi_callback_info info);
    static napi_value IsRTLWihtError(napi_env env, napi_callback_info info);
    static napi_value InitPhoneNumberFormat(napi_env env, napi_value exports);
    static napi_value UnitConvert(napi_env env, napi_callback_info info);
    static napi_value InitI18nBreakIterator(napi_env env, napi_value exports);
    static napi_value InitI18nIndexUtil(napi_env env, napi_value exports);
    static napi_value GetDateOrder(napi_env env, napi_callback_info info);
    static napi_value GetTimePeriodName(napi_env env, napi_callback_info info);
    static napi_value GetBestMatchLocale(napi_env env, napi_callback_info info);
    static napi_value GetThreeLetterLanguage(napi_env env, napi_callback_info info);
    static napi_value GetThreeLetterRegion(napi_env env, napi_callback_info info);
    static napi_value InitI18nTransliterator(napi_env env, napi_value exports);
    static napi_value InitIndexUtil(napi_env env, napi_value exports);
    static napi_value InitI18nUtil(napi_env env, napi_value exports);
    static napi_value GetTransliteratorInstance(napi_env env, napi_callback_info info);
    static napi_value GetAvailableIDs(napi_env env, napi_callback_info info);
    static napi_value InitUtil(napi_env env, napi_value exports);
    static napi_value System(napi_env env, napi_value exports);

private:
    static napi_value PhoneNumberFormatConstructor(napi_env env, napi_callback_info info);
    static napi_value IsValidPhoneNumber(napi_env env, napi_callback_info info);
    static napi_value FormatPhoneNumber(napi_env env, napi_callback_info info);
    static napi_value GetLocationName(napi_env env, napi_callback_info info);
    bool InitPhoneNumberFormatContext(napi_env env, napi_callback_info info, const std::string &country,
                                      const std::map<std::string, std::string> &options);
    static napi_value I18nBreakIteratorConstructor(napi_env env, napi_callback_info info);
    static napi_value InitBreakIterator(napi_env env, napi_value exports);
    static napi_value GetLineInstance(napi_env env, napi_callback_info info);
    static napi_value Current(napi_env env, napi_callback_info info);
    static napi_value First(napi_env env, napi_callback_info info);
    static napi_value Last(napi_env env, napi_callback_info info);
    static napi_value Previous(napi_env env, napi_callback_info info);
    static napi_value Next(napi_env env, napi_callback_info info);
    static napi_value SetText(napi_env env, napi_callback_info info);
    static napi_value GetText(napi_env env, napi_callback_info info);
    static napi_value Following(napi_env env, napi_callback_info info);
    static napi_value IsBoundary(napi_env env, napi_callback_info info);

    static napi_value GetIndexUtil(napi_env env, napi_callback_info info);
    static napi_value I18nIndexUtilConstructor(napi_env env, napi_callback_info info);
    static napi_value GetIndexList(napi_env env, napi_callback_info info);
    static napi_value AddLocale(napi_env env, napi_callback_info info);
    static napi_value GetIndex(napi_env env, napi_callback_info info);
    bool InitIndexUtilContext(napi_env env, napi_callback_info info, const std::string &localeTag);

    static napi_value Transform(napi_env env, napi_callback_info info);
    bool InitTransliteratorContext(napi_env env, napi_callback_info info, const std::string &idTag);
    static napi_value InitTransliterator(napi_env env, napi_value exports);
    static napi_value I18nTransliteratorConstructor(napi_env env, napi_callback_info info);
    static std::string ModifyOrder(std::string &pattern);
    static void ProcessNormal(char ch, int *order, size_t orderSize, int *lengths, size_t lengsSize);

    static int GetParamOfGetTimePeriodName(napi_env env, napi_callback_info info, std::string &tag, int32_t &hour);

    static napi_value ObjectConstructor(napi_env env, napi_callback_info info);

    napi_env env_;
    std::unique_ptr<PhoneNumberFormat> phonenumberfmt_ = nullptr;
    std::unique_ptr<icu::Transliterator> transliterator_ = nullptr;
    std::unique_ptr<I18nBreakIterator> brkiter_ = nullptr;
    std::unique_ptr<IndexUtil> indexUtil_ = nullptr;
};
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif
