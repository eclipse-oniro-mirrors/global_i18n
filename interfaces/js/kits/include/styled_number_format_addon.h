/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef OHOS_GLOBAL_I18N_STYLED_NUMBER_FORMAT_ADDON_H
#define OHOS_GLOBAL_I18N_STYLED_NUMBER_FORMAT_ADDON_H

#include <memory>
#include <unordered_map>

#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "simple_number_format.h"
#include "simple_number_format_addon.h"
#include "styled_number_format.h"
#include "number_format.h"
#include "number_format_addon.h"

namespace OHOS {
namespace Global {
namespace I18n {
class StyledNumberFormatAddon {
public:
    StyledNumberFormatAddon();
    ~StyledNumberFormatAddon();
    static void Destructor(napi_env env, void *nativeObject, void *hint);
    static napi_value InitStyledNumberFormat(napi_env env, napi_value exports);

private:
    static napi_value constructor(napi_env env, napi_callback_info info);
    static napi_value Format(napi_env env, napi_callback_info info);
    static napi_value CreateStyledString(napi_env env, const std::string &formattedNumber, napi_value styleOption);
    static napi_value CreateStyleOptions(napi_env env, const std::vector<StyledNumberFormat::NumberPart> &numberParts,
        const std::unordered_map<std::string, napi_ref> &styledNumberFormatOptions);
    static napi_value CreateStyleOption(napi_env env, StyledNumberFormat::NumberPart numberPart,
        napi_ref textStyleRef);
    static void GetNumberFmtOrSimpleNumberFmt(napi_env env, napi_value arg,
        NumberFormatAddon **numberformataddon, SimpleNumberFormatAddon **simpleNumberFormatAddon);
    static void SetNamedProperty(napi_env env, napi_value &option, const std::string& name, napi_value &property);

    bool InitStyledNumberFormatContent(napi_env env, NumberFormatAddon* numberFormatAddon,
        SimpleNumberFormatAddon* simpleNumberFormatAddon, napi_value options);
    void SetTextStyle(napi_env env, napi_value options, const std::string &optionName);

    napi_env env_ = nullptr;
    std::unordered_map<std::string, napi_ref> styledNumberFormatOptions_ {};
    std::unique_ptr<StyledNumberFormat> styledNumberFormat_ = nullptr;
};
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif