/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#ifndef OHOS_GLOBAL_I18N_SYMBOL_DATE_TIME_FORMAT_ADDON_H
#define OHOS_GLOBAL_I18N_SYMBOL_DATE_TIME_FORMAT_ADDON_H

#include <string>
#include <unordered_map>
#include "ani.h"
#include "symbol_date_time_format.h"

namespace OHOS {
namespace Global {
namespace I18n {
class I18nSymbolDateTimeFormatAddon {
public:
    static ani_status BindContextSymbolDateTimeFormat(ani_env *env);
    static I18nSymbolDateTimeFormatAddon* UnwrapAddon(ani_env *env, ani_object object);
    static bool IsSymbolDateTimeFormat(ani_env *env, ani_object dateTimeFormat);

    std::shared_ptr<SymbolDateTimeFormat> GetDateTimeFormat();

private:
    bool InitContext(ani_env *env, ani_object locale, ani_object options);

    static ani_object Create(ani_env *env, [[maybe_unused]] ani_object object, ani_object locale, ani_object options);
    static std::unordered_map<std::string, std::string> ParseOptions(ani_env *env, ani_object options);
    static ani_string FormatImpl(ani_env *env, ani_object object, ani_object date);
    static ani_object FormatToPartsImpl(ani_env *env, ani_object object, ani_object date);
    static ani_string FormatRangeImpl(ani_env *env, ani_object object, ani_double startDate, ani_double endDate);
    static ani_object FormatRangeToPartsImpl(ani_env *env, ani_object object, ani_double startDate,
        ani_double endDate);
    static ani_string ToString(ani_env *env, [[maybe_unused]] ani_object object);
    static ani_object ResolvedOptions(ani_env *env, ani_object object);
    static ani_long Parse(ani_env *env, ani_object object, ani_string text, ani_boolean lenientMode);
    static void ParseAmPmSymbol(ani_env *env, ani_object options,
        std::unordered_map<std::string, std::string>& result);
    static ani_array CreateDateTimeFormatPartArray(ani_env *env,
        const std::vector<std::unordered_map<std::string, std::string>> &parts);
    static ani_object CreateDateTimeFormatPart(ani_env *env, const std::unordered_map<std::string, std::string> &part);
    static ani_array CreateDateTimeRangeFormatPartArray(ani_env *env,
        const std::vector<std::unordered_map<std::string, std::string>> &parts);
    static ani_object CreateDateTimeRangeFormatPart(ani_env *env,
        const std::unordered_map<std::string, std::string> &part);
    static ani_object CreateResolvedOptions(ani_env *env, std::unordered_map<std::string, std::string> &options);
    static void ResolvedAmPmSymbol(ani_env *env, ani_object resolvedOptions,
        std::unordered_map<std::string, std::string> &options);
    std::shared_ptr<SymbolDateTimeFormat> symbolDateTimeFormat_ = nullptr;
};
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif
