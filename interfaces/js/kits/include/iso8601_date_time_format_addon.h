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

#ifndef OHOS_GLOBAL_I18N_ISO8601_DATE_TIME_FORMAT_ADDON_H
#define OHOS_GLOBAL_I18N_ISO8601_DATE_TIME_FORMAT_ADDON_H

#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "iso8601_date_time_format.h"

namespace OHOS {
namespace Global {
namespace I18n {
class ISO8601DateTimeFormatAddon {
public:
    ISO8601DateTimeFormatAddon();
    ~ISO8601DateTimeFormatAddon();
    std::shared_ptr<ISO8601DateTimeFormat> GetDateTimeFormat();
    static void Destructor(napi_env env, void *nativeObject, void *hint);
    static napi_value InitISO8601DateTimeFormat(napi_env env, napi_value exports);
    static napi_value ISO8601DateTimeFormatConstructor(napi_env env, napi_callback_info info);
    static napi_status UnwrapDateTimeFormat(napi_env env, napi_value value,
        ISO8601DateTimeFormatAddon** dateTimeFormat);

private:
    static napi_value Format(napi_env env, napi_callback_info info);

    static constexpr napi_type_tag TYPE_TAG = { 0xb1587ea41cbc40ae, 0xb38af333561e5f66 };

    std::shared_ptr<ISO8601DateTimeFormat> iso8601DateTimeFormat_ = nullptr;
};
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif