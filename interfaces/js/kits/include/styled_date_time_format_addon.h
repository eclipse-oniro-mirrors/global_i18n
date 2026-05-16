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

#ifndef OHOS_GLOBAL_I18N_STYLED_DATE_TIME_FORMAT_ADDON_H
#define OHOS_GLOBAL_I18N_STYLED_DATE_TIME_FORMAT_ADDON_H

#include <memory>
#include <unordered_map>

#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "simple_date_time_format_addon.h"
#include "intl_date_time_format_addon.h"
#include "styled_date_time_format.h"

namespace OHOS {
namespace Global {
namespace I18n {
class StyledDateTimeFormatAddon {
public:
    StyledDateTimeFormatAddon();
    ~StyledDateTimeFormatAddon();

    static void Destructor(napi_env env, void *nativeObject, void *hint);
    static napi_value InitStyledDateTimeFormat(napi_env env, napi_value exports);

private:
    bool InitStyledFormatContext(napi_env env, napi_value options,
        IntlDateTimeFormatAddon* intlFormat, SimpleDateTimeFormatAddon* simpleFormat);
    void ParseOptions(napi_env env, napi_value options);
    void ParseOption(napi_env env, napi_value options, const std::string& optionName);

    static napi_value Format(napi_env env, napi_callback_info info);
    static napi_value constructor(napi_env env, napi_callback_info info);
    static void ParseDateTimeFormat(napi_env env, napi_value value,
        IntlDateTimeFormatAddon** intlFormat, SimpleDateTimeFormatAddon** simpleFormat);
    static napi_value CreateStyleOptions(napi_env env, const std::vector<DateTimeFormatPart>& parts,
        const std::unordered_map<std::string, napi_ref>& styledOptions);
    static napi_value CreateStyleOption(napi_env env, const DateTimeFormatPart& part, napi_ref textStyleRef);
    static void SetTextStyle(napi_env env, napi_value &option, const std::string& name,
        napi_value &property);

    static constexpr napi_type_tag TYPE_TAG = { 0x474eba8414634a91, 0x9675dcb6d57817b0 };

    napi_env env_ = nullptr;
    std::unordered_map<std::string, napi_ref> styledFormatOptions_ {};
    std::unique_ptr<StyledDateTimeFormat> styledDateTimeFormat_ = nullptr;
};
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif