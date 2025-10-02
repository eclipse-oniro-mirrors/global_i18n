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
#ifndef OHOS_GLOBAL_I18N_DISPLAYNAMES_ADDON_H
#define OHOS_GLOBAL_I18N_DISPLAYNAMES_ADDON_H

#include "napi/native_api.h"
#include "napi/native_node_api.h"

#include "displaynames.h"

namespace OHOS {
namespace Global {
namespace I18n {
class DisplayNamesAddon {
public:
    static napi_value InitDisplayNames(napi_env env, napi_value exports);
    static void Destructor(napi_env env, void *nativeObject, void *finalize_hint);

    DisplayNamesAddon();
    virtual ~DisplayNamesAddon();

private:
    static napi_value DisplayNamesConstructor(napi_env env, napi_callback_info info);
    static napi_value SupportedLocalesOf(napi_env env, napi_callback_info info);
    static napi_value Of(napi_env env, napi_callback_info info);
    static napi_value ResolvedOptions(napi_env env, napi_callback_info info);
    static void GetOptionValues(napi_env env, napi_value options, std::map<std::string, std::string> &map);

    bool InitDisplayNamesContext(napi_env env, napi_callback_info info, const std::vector<std::string> &localeTags,
        const std::map<std::string, std::string> &map);

    napi_env env_;
    std::unique_ptr<DisplayNames> displaynames_ = nullptr;
};
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif