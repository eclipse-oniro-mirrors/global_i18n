/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef OHOS_GLOBAL_I18N_ENTITY_RECOGNIZER_ADDON_H
#define OHOS_GLOBAL_I18N_ENTITY_RECOGNIZER_ADDON_H

#include "entity_recognizer.h"
#include "locale_config.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"

namespace OHOS {
namespace Global {
namespace I18n {
class EntityRecognizerAddon {
public:
    EntityRecognizerAddon();
    ~EntityRecognizerAddon();
    static void Destructor(napi_env env, void *nativeObject, void *hint);
    static napi_value InitEntityRecognizer(napi_env env, napi_value exports);

private:
    static napi_value constructor(napi_env env, napi_callback_info info);
    static napi_value FindEntityInfo(napi_env env, napi_callback_info info);
    static napi_value CreateEntityInfoItem(napi_env env, const int begin, const int end, const std::string& type);
    static napi_value GetEntityInfoItem(napi_env env, std::vector<std::vector<int>>& entityInfo);
    std::unique_ptr<EntityRecognizer> entityRecognizer_ = nullptr;
    static const size_t ENTITY_INFO_LEN;
};
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif