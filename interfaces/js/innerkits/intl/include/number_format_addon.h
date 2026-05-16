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

#ifndef OHOS_GLOBAL_I18N_NUMBER_FORMAT_ADDON_H
#define OHOS_GLOBAL_I18N_NUMBER_FORMAT_ADDON_H

#include <memory>

#include "napi/native_api.h"
#include "napi/native_node_api.h"

#include "number_format.h"

namespace OHOS {
namespace Global {
namespace I18n {
class NumberFormatAddon {
public:
    NumberFormatAddon();
    ~NumberFormatAddon();
    static void Destructor(napi_env env, void *nativeObject, void *hint);
    static napi_value InitNumberFormat(napi_env env, napi_value exports);
    static napi_status UnwrapNumberFormat(napi_env env, napi_value value, NumberFormatAddon** numberFormat);

    std::shared_ptr<NumberFormat> CopyNumberFormat();

private:
    static napi_value NumberFormatConstructor(napi_env env, napi_callback_info info);
    static napi_value FormatNumber(napi_env env, napi_callback_info info);
    static napi_value GetNumberResolvedOptions(napi_env env, napi_callback_info info);
    static napi_value FormatRangeNumber(napi_env env, napi_callback_info info);

    bool InitNumberFormatContext(napi_env env, napi_callback_info info, std::vector<std::string> localeTags,
        std::map<std::string, std::string> &map);

    static constexpr napi_type_tag TYPE_TAG = { 0x4783eef06c554489, 0xa3da7a277e1cff92 };

    std::shared_ptr<NumberFormat> numberfmt_ = nullptr;
};
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif