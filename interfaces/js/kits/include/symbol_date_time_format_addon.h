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

#include <memory>

#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "symbol_date_time_format.h"

namespace OHOS {
namespace Global {
namespace I18n {
class SymbolDateTimeFormatAddon {
public:
    SymbolDateTimeFormatAddon();
    ~SymbolDateTimeFormatAddon();
    static void Destructor(napi_env env, void *nativeObject, void *hint);
    static napi_value InitSymbolDateTimeFormat(napi_env env, napi_value exports);
    std::shared_ptr<SymbolDateTimeFormat> GetDateTimeFormat();
    static bool IsSymbolDateTimeFormat(napi_env env, napi_value argv);
    static napi_status UnwrapDateTimeFormat(napi_env env, napi_value value,
        SymbolDateTimeFormatAddon** dateTimeFormat);

private:
    static napi_value Constructor(napi_env env, napi_callback_info info);
    bool InitSymbolFormatContext(napi_env env, const std::vector<std::string>& localeTags,
        const std::unordered_map<std::string, std::string>& options);
    static void ParseOptions(napi_env env, napi_value optionsValue,
        std::unordered_map<std::string, std::string>& options);
    static napi_value Format(napi_env env, napi_callback_info info);
    static napi_value FormatToParts(napi_env env, napi_callback_info info);
    static napi_value FormatRange(napi_env env, napi_callback_info info);
    static napi_value FormatRangeToParts(napi_env env, napi_callback_info info);
    static napi_value ToString(napi_env env, napi_callback_info info);
    static napi_value ResolvedOptions(napi_env env, napi_callback_info info);
    static napi_value Parse(napi_env env, napi_callback_info info);

    static bool GetParseParams(napi_env env, napi_callback_info info, std::string& text,
        bool& lenientMode, napi_value& thisVar);
    static bool UnwrapParseObject(napi_env env, napi_value thisVar, SymbolDateTimeFormatAddon*& obj);
    static bool HandleParseResult(napi_env env, I18nErrorCode errCode);

    static void ParseAmPmSymbol(napi_env env, napi_value optionsValue,
        std::unordered_map<std::string, std::string>& options);
    static napi_value ResolvedAmPmSymbol(napi_env env, std::unordered_map<std::string, std::string>& options);
    static constexpr napi_type_tag TYPE_TAG = { 0x858e7bb669bd4ee7, 0xa3bd7019de6dcb16 };

    std::shared_ptr<SymbolDateTimeFormat> symbolDateTimeFormat_ = nullptr;
};
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif