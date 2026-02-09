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

#ifndef OHOS_GLOBAL_I18N_ADVANCED_MEASURE_FORMAT_ADDON_H
#define OHOS_GLOBAL_I18N_ADVANCED_MEASURE_FORMAT_ADDON_H

#include "advanced_measure_format.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"

namespace OHOS {
namespace Global {
namespace I18n {
class AdvancedMeasureFormatAddon {
public:
    AdvancedMeasureFormatAddon();
    ~AdvancedMeasureFormatAddon();
    static void Destructor(napi_env env, void *nativeObject, void *hint);
    static napi_value InitAdvancedMeasureFormat(napi_env env, napi_value exports);

private:
    bool InitContext(napi_env env, std::shared_ptr<NumberFormat> numberFormat, AdvancedMeasureFormatOptions options);

    static napi_value Constructor(napi_env env, napi_callback_info info);
    static AdvancedMeasureFormatOptions ParseOptions(napi_env env, napi_value object);
    static napi_value Format(napi_env env, napi_callback_info info);
    static std::string FormatNumber(napi_env env, napi_value value,
        std::shared_ptr<AdvancedMeasureFormat> advancedMeasureFormat);

    static constexpr napi_type_tag TYPE_TAG = { 0x64bcd3964dda4d64, 0x9bb210875629e6cd };

    std::shared_ptr<AdvancedMeasureFormat> advancedMeasureFormat_ = nullptr;
};
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif