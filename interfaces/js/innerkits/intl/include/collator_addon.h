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
#ifndef COLLATOR_ADDON_H
#define COLLATOR_ADDON_H

#include "napi/native_api.h"
#include "napi/native_node_api.h"

#include "collator.h"

namespace OHOS {
namespace Global {
namespace I18n {
class CollatorAddon {
public:
    static napi_value InitCollator(napi_env env, napi_value exports);
    static void Destructor(napi_env env, void *nativeObject, void *finalize_hint);

    CollatorAddon();
    virtual ~CollatorAddon();

private:
    static napi_value CreateCollatorWithoutNew(napi_env env, size_t argc, napi_value *argv);
    static napi_value CollatorConstructor(napi_env env, napi_callback_info info);
    static napi_value SupportedLocalesOf(napi_env env, napi_callback_info info);
    static napi_value CompareString(napi_env env, napi_callback_info info);
    static napi_value ToString(napi_env env, napi_callback_info info);
    static napi_value GetCollatorResolvedOptions(napi_env env, napi_callback_info info);

    bool InitCollatorContext(napi_env env, napi_callback_info info, std::vector<std::string> localeTags,
        std::map<std::string, std::string> &map);

    static constexpr napi_type_tag TYPE_TAG = { 0xf8f8839acc0b4034, 0xbeb94fff32cd557a };

    napi_env env_;
    std::unique_ptr<Collator> collator_ = nullptr;
};
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif