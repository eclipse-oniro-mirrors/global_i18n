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

#ifndef I18N_SYMBOL_NUMBER_FORMAT_ADDON_H
#define I18N_SYMBOL_NUMBER_FORMAT_ADDON_H

#include "ani.h"
#include "symbol_number_format.h"

namespace OHOS {
namespace Global {
namespace I18n {
class I18nSymbolNumberFormatAddon {
public:
    static I18nSymbolNumberFormatAddon* UnwrapAddon(ani_env *env, ani_object object);
    static ani_status BindContextSymbolNumberFormat(ani_env *env);
    static bool IsSymbolNumberFormat(ani_env *env, ani_object numberFormat);
    std::shared_ptr<SymbolNumberFormat> GetNumberFormat();

private:
    bool InitContext(ani_env *env, ani_object locale, ani_object options);
    static ani_object Create(ani_env *env, [[maybe_unused]] ani_object object, ani_object locale, ani_object options);
    static ani_string FormatDoubleImpl(ani_env *env, ani_object object, ani_double value);
    static ani_string FormatLongImpl(ani_env *env, ani_object object, ani_long value);
    static ani_string FormatDecStrImpl(ani_env *env, ani_object object, ani_string value);
    static ani_object FormatToPartsDoubleImpl(ani_env *env, ani_object object, ani_double value);
    static ani_object FormatToPartsDecStrImpl(ani_env *env, ani_object object, ani_string value);
    static ani_string FormatRangeDoubleDoubleImpl(ani_env *env, ani_object object, ani_double start, ani_double end);
    static ani_string FormatRangeDoubleDecStrImpl(ani_env *env, ani_object object, ani_double start, ani_string end);
    static ani_string FormatRangeDecStrDoubleImpl(ani_env *env, ani_object object, ani_string start, ani_double end);
    static ani_string FormatRangeDecStrDecStrImpl(ani_env *env, ani_object object, ani_string start, ani_string end);
    static ani_object FormatToRangePartsDoubleDoubleImpl(ani_env *env, ani_object object,
        ani_double start, ani_double end);
    static ani_object FormatToRangePartsDoubleDecStrImpl(ani_env *env, ani_object object,
        ani_double start, ani_string end);
    static ani_object FormatToRangePartsDecStrDoubleImpl(ani_env *env, ani_object object,
        ani_string start, ani_double end);
    static ani_object FormatToRangePartsDecStrDecStrImpl(ani_env *env, ani_object object,
        ani_string start, ani_string end);
    static ani_object ResolvedOptions(ani_env *env, ani_object object);
    static ani_double Parse(ani_env *env, ani_object object, ani_string text, ani_boolean lenientMode);
    static ani_string ToString(ani_env *env, ani_object object);
    static std::map<std::string, std::string> ParseOptions(ani_env *env, ani_object options);
    static ani_object CreateNumberFormatPartArray(ani_env *env, const NumberFormatParts &parts,
        bool isRange);
    static ani_object CreateNumberFormatPart(ani_env *env, const std::unordered_map<std::string, std::string> &part);
    static ani_object CreateNumberRangeFormatPart(ani_env *env,
        const std::unordered_map<std::string, std::string> &part);
    static ani_object CreateResolvedOptions(ani_env *env, const std::map<std::string, std::string> &options);

    std::shared_ptr<SymbolNumberFormat> symbolNumberFormat_ = nullptr;
};
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif
