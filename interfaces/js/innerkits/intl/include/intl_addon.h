/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef INTL_ADDON_H
#define INTL_ADDON_H

#include <string>

#include "napi/native_api.h"
#include "napi/native_node_api.h"

#include "collator.h"
#include "date_time_format.h"
#include "plural_rules.h"
#include "relative_time_format.h"

namespace OHOS {
namespace Global {
namespace I18n {
void GetDateOptionValues(napi_env env, napi_value options, std::map<std::string, std::string> &map);
void GetCollatorOptionValue(napi_env env, napi_value options, std::map<std::string, std::string> &map);
void GetPluralRulesOptionValues(napi_env env, napi_value options, std::map<std::string, std::string> &map);
void GetRelativeTimeOptionValues(napi_env env, napi_value options, std::map<std::string, std::string> &map);
bool GetStringParameter(napi_env env, napi_value value, std::vector<char> &buf);

class IntlAddon {
public:
    static napi_value Init(napi_env env, napi_value exports);

    static napi_value InitDateTimeFormat(napi_env env, napi_value exports);
    static napi_value InitCollator(napi_env env, napi_value exports);
    static napi_value InitRelativeTimeFormat(napi_env env, napi_value exports);
    static napi_value InitPluralRules(napi_env env, napi_value exports);
    static void Destructor(napi_env env, void *nativeObject, void *finalize_hint);

    IntlAddon();
    virtual ~IntlAddon();

private:
    static napi_value DateTimeFormatConstructor(napi_env env, napi_callback_info info);
    static napi_value RelativeTimeFormatConstructor(napi_env env, napi_callback_info info);

    static napi_value FormatDateTime(napi_env env, napi_callback_info info);
    static napi_value FormatDateTimeRange(napi_env env, napi_callback_info info);
    static napi_value GetDateTimeResolvedOptions(napi_env env, napi_callback_info info);

    static napi_value FormatRelativeTime(napi_env env, napi_callback_info info);
    static napi_value FormatToParts(napi_env env, napi_callback_info info);
    static void FillInArrayElement(napi_env env, napi_value &result, napi_status &status,
        const std::vector<std::vector<std::string>> &timeVector);
    static napi_value GetRelativeTimeResolvedOptions(napi_env env, napi_callback_info info);

    static napi_value CollatorConstructor(napi_env env, napi_callback_info info);
    static napi_value CompareString(napi_env env, napi_callback_info info);
    static napi_value GetCollatorResolvedOptions(napi_env env, napi_callback_info info);

    static napi_value PluralRulesConstructor(napi_env env, napi_callback_info info);
    static napi_value Select(napi_env env, napi_callback_info info);

    static int64_t GetMilliseconds(napi_env env, napi_value *argv, int index);
    bool InitDateTimeFormatContext(napi_env env, napi_callback_info info, std::vector<std::string> localeTags,
        std::map<std::string, std::string> &map);
    bool InitCollatorContext(napi_env env, napi_callback_info info, std::vector<std::string> localeTags,
        std::map<std::string, std::string> &map);
    bool InitRelativeTimeFormatContext(napi_env env, napi_callback_info info, std::vector<std::string> localeTags,
        std::map<std::string, std::string> &map);
    bool InitPluralRulesContext(napi_env env, napi_callback_info info, std::vector<std::string> localeTags,
        std::map<std::string, std::string> &map);

    static constexpr napi_type_tag TYPE_TAG = { 0x5dcb8d6b32f04178, 0xb6e54d595c49281b };

    napi_env env_;
    std::unique_ptr<DateTimeFormat> datefmt_ = nullptr;
    std::unique_ptr<RelativeTimeFormat> relativetimefmt_ = nullptr;
    std::unique_ptr<Collator> collator_ = nullptr;
    std::unique_ptr<PluralRules> pluralrules_ = nullptr;
};
} // namespace I18n
} // namespace Global
} // namespace OHOS

extern "C" bool RegisterIntl(napi_env env);
extern "C" bool BindingIntlModules(napi_env env, napi_value &intlModule);
#endif