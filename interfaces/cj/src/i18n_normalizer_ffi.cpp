/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <cstdint>
#include <cstdlib>
#include <chrono>
#include <string>
#include "i18n_hilog.h"
#include "i18n_normalizer_ffi.h"
#include "i18n_normalizer_impl.h"
 
namespace OHOS {
namespace Global {
namespace I18n {
using namespace OHOS::FFI;
extern "C"
{
    int64_t FfiI18nNormalizerConstructor(int32_t mode)
    {
        I18nNormalizerMode normalizerMode = I18nNormalizerMode(mode);
        I18nErrorCode errorCode = I18nErrorCode::SUCCESS;
        std::unique_ptr<I18nNormalizer> normalizer_ = std::make_unique<I18nNormalizer>(normalizerMode, errorCode);
        if (errorCode != I18nErrorCode::SUCCESS || !normalizer_) {
            HILOG_ERROR_I18N("Create I18nNormalizer fail, error code: %d", errorCode);
            return -1;
        }
        auto ffiNormalizerInstance = FFIData::Create<FfiI18nNormalizer>(std::move(normalizer_));
        if (ffiNormalizerInstance == nullptr) {
            HILOG_ERROR_I18N("Create FfiI18nNormalizer fail");
            return -1;
        }
        return ffiNormalizerInstance->GetID();
    }

    char* FfiI18nNormalizerNormalize(int64_t remoteDataID, char* text)
    {
        auto normalizer = FFIData::GetData<FfiI18nNormalizer>(remoteDataID);
        if (!normalizer) {
            HILOG_ERROR_I18N("The FfiI18nNormalizer instance is nullptr");
            return nullptr;
        }
        return normalizer->normalize(text);
    }
}
}  // namespace I18n
}  // namespace Global
}  // namespace OHOS