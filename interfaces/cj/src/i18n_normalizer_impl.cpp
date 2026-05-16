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

#include "i18n_ffi.h"
#include "i18n_hilog.h"
#include "i18n_normalizer_impl.h"

namespace OHOS {
namespace Global {
namespace I18n {
extern "C"
{
    FfiI18nNormalizer::FfiI18nNormalizer(std::unique_ptr<I18nNormalizer> normalizer)
    {
        normalizer_ = std::move(normalizer);
    }

    char* FfiI18nNormalizer::normalize(char* text)
    {
        I18nErrorCode errorCode = I18nErrorCode::SUCCESS;
        std::string normalizedText = normalizer_->Normalize(text, std::strlen(text), errorCode);
        if (errorCode != I18nErrorCode::SUCCESS) {
            HILOG_ERROR_I18N("FfiI18nNormalizer normalize fail, error code: %d", errorCode);
            return nullptr;
        }
        return MallocCString(normalizedText);
    }
}
}  // namespace I18n
}  // namespace Global
}  // namespace OHOS