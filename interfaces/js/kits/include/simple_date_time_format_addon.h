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

#ifndef OHOS_GLOBAL_I18N_SIMPLE_DATE_TIME_FORMAT_ADDON_H
#define OHOS_GLOBAL_I18N_SIMPLE_DATE_TIME_FORMAT_ADDON_H

#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "simple_date_time_format.h"

namespace OHOS {
namespace Global {
namespace I18n {
class SimpleDateTimeFormatAddon {
public:
    SimpleDateTimeFormatAddon();
    ~SimpleDateTimeFormatAddon();
    static void Destructor(napi_env env, void *nativeObject, void *hint);
    static napi_value InitSimpleDateTimeFormat(napi_env env, napi_value exports);
    static napi_value GetSimpleDateTimeFormatBySkeleton(napi_env env, napi_callback_info info);
    static napi_value GetSimpleDateTimeFormatByPattern(napi_env env, napi_callback_info info);

private:
    static napi_value Format(napi_env env, napi_callback_info info);
    static std::vector<napi_value> GenerateParameter(napi_env env, napi_callback_info info, bool isBestPattern,
        size_t& num);
    static bool GetDateTime(napi_env env, napi_value date, int64_t& milliseconds);
    static napi_value SimpleDateTimeFormatConstructor(napi_env env, napi_callback_info info);
    static std::unique_ptr<SimpleDateTimeFormat> InitSimpleDateTimeFormatContext(napi_env env, napi_value locale,
        const std::string& skeleton, bool isBestPattern, I18nErrorCode& errCode);
    std::unique_ptr<SimpleDateTimeFormat> simpleDateTimeFormat_ = nullptr;
};
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif