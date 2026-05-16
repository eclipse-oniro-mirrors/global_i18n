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

#ifndef OHOS_GLOBAL_I18N_SYMBOL_NUMBER_FORMAT_ADDON_H
#define OHOS_GLOBAL_I18N_SYMBOL_NUMBER_FORMAT_ADDON_H

#include <memory>

#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "symbol_number_format.h"

namespace OHOS {
namespace Global {
namespace I18n {
union NumberData {
    NumberData()
    {
        valueNumber = 0;
        valueBigint = "";
    }

    NumberData(NumberData& data)
    {
        this->valueNumber = data.valueNumber;
        this->valueBigint = data.valueBigint;
    }

    ~NumberData() {}

    double valueNumber;
    std::string valueBigint;
};

enum class NumberDataType {
    INVALID_DATA = 0,
    NUMBER_TYPE = 1,
    BIGINT_TYPE = 2
};

class SymbolNumberFormatAddon {
public:
    SymbolNumberFormatAddon();
    ~SymbolNumberFormatAddon();
    static void Destructor(napi_env env, void *nativeObject, void *hint);
    static napi_value InitSymbolNumberFormat(napi_env env, napi_value exports);
    std::shared_ptr<SymbolNumberFormat> GetNumberFormat();
    static bool IsSymbolNumberFormat(napi_env env, napi_value argv);
    static napi_status UnwrapNumberFormat(napi_env env, napi_value value,
        SymbolNumberFormatAddon** numberFormat);

private:
    static napi_value Constructor(napi_env env, napi_callback_info info);
    bool InitSymbolFormatContext(napi_env env, const std::vector<std::string>& localeTags,
        std::map<std::string, std::string>& options);
    static void ParseOptions(napi_env env, napi_value optionsValue,
        std::map<std::string, std::string>& options);
    static napi_value Format(napi_env env, napi_callback_info info);
    static napi_value FormatToParts(napi_env env, napi_callback_info info);
    static napi_value FormatRange(napi_env env, napi_callback_info info);
    static napi_value FormatRangeToParts(napi_env env, napi_callback_info info);
    static napi_value ToString(napi_env env, napi_callback_info info);
    static napi_value ResolvedOptions(napi_env env, napi_callback_info info);
    static napi_value Parse(napi_env env, napi_callback_info info);
    static bool GetParseParams(napi_env env, napi_callback_info info,
        std::string& text, bool& lenientMode, napi_value& thisVar);
    static bool UnwrapParseObject(napi_env env, napi_value thisVar,
        SymbolNumberFormatAddon*& obj);
    static bool HandleParseResult(napi_env env, I18nErrorCode errCode);
    static NumberData ParseNumberData(napi_env env, napi_value value, NumberDataType& type);
    static napi_value CreateResolvedOptions(napi_env env,
        const std::map<std::string, std::string>& options);
    static constexpr napi_type_tag TYPE_TAG = { 0x093cae8d387f4376, 0xb9c588152f2d1165 };

    std::shared_ptr<SymbolNumberFormat> symbolNumberFormat_ = nullptr;
};
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif
