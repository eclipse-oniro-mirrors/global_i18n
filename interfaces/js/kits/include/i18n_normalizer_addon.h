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

#ifndef OHOS_GLOBAL_I18N_NORMALIZER_H
#define OHOS_GLOBAL_I18N_NORMALIZER_H

#include "i18n_normalizer.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"

namespace OHOS {
namespace Global {
namespace I18n {
class I18nNormalizerAddon {
public:
    I18nNormalizerAddon();
    ~I18nNormalizerAddon();
    static void Destructor(napi_env env, void *nativeObject, void *hint);
    static napi_value InitI18nNormalizer(napi_env env, napi_value exports);
    static napi_value CreateI18NNormalizerModeEnum(napi_env env, napi_status &initStatus);

private:
    static napi_value I18nNormalizerConstructor(napi_env env, napi_callback_info info);
    // process js function call
    static napi_value Normalize(napi_env env, napi_callback_info info);
    static napi_value InitNormalizer(napi_env env, napi_value exports);
    static napi_value GetI18nNormalizerInstance(napi_env env, napi_callback_info info);
    static napi_status SetEnumValue(napi_env env, napi_value enumObj, const char* enumName, int32_t enumVal);
    
    static const int32_t NORMALIZER_MODE_NFC = 1;
    static const int32_t NORMALIZER_MODE_NFD = 2;
    static const int32_t NORMALIZER_MODE_NFKC = 3;
    static const int32_t NORMALIZER_MODE_NFKD = 4;
    static const char *NORMALIZER_MODE_NFC_NAME;
    static const char *NORMALIZER_MODE_NFD_NAME;
    static const char *NORMALIZER_MODE_NFKC_NAME;
    static const char *NORMALIZER_MODE_NFKD_NAME;

    std::unique_ptr<I18nNormalizer> normalizer_ = nullptr;
};
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif