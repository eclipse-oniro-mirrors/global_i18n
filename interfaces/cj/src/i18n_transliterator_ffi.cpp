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

#include <cstdlib>
#include <string>
#include "i18n_hilog.h"
#include "i18n_ffi.h"
#include "i18n_transliterator_ffi.h"
#include "i18n_transliterator_impl.h"
#include <utility>

namespace OHOS {
namespace Global {
namespace I18n {
using namespace OHOS::FFI;
using namespace OHOS::HiviewDFX;
extern "C"
{
    int64_t FfiI18nTransliteratorConstructor(char* id)
    {
        UErrorCode status = U_ZERO_ERROR;
        icu::UnicodeString unistr = icu::UnicodeString::fromUTF8(icu::StringPiece(id));
        icu::Transliterator *trans
            = icu::Transliterator::createInstance(unistr, UTransDirection::UTRANS_FORWARD, status);
        if (U_FAILURE(status) || (trans == nullptr)) {
            HILOG_ERROR_I18N("Create icuTransliterator fail");
            return -1;
        }
        auto ffiTransliteratorInstance
            = FFIData::Create<FfiI18nTransliterator>(std::unique_ptr<icu::Transliterator>(trans));
        if (ffiTransliteratorInstance == nullptr) {
            HILOG_ERROR_I18N("Create FfiI18nTransliterator fail");
            return -1;
        }
        return ffiTransliteratorInstance->GetID();
    }

    CArrStr FfiI18nTransliteratorGetAvailableIDs()
    {
        UErrorCode icuStatus = U_ZERO_ERROR;
        icu::StringEnumeration* idStrEnum = icu::Transliterator::getAvailableIDs(icuStatus);
        if (U_FAILURE(icuStatus)) {
            HILOG_ERROR_I18N("Failed to get available ids");
            if (idStrEnum) {
                delete idStrEnum;
            }
            return { nullptr, 0 };
        }
        std::vector<std::string> idList;
        icu::UnicodeString temp = nullptr;
        while ((temp = idStrEnum->next(nullptr, icuStatus)) != nullptr) {
            std::string id;
            temp.toUTF8String(id);
            idList.push_back(id);
        }
        delete idStrEnum;
        return VectorStringToCArr(idList);
    }

    char* FfiI18nTransliteratorTransform(int64_t remoteDataId, char* text)
    {
        auto transLiterator_ = FFIData::GetData<FfiI18nTransliterator>(remoteDataId);
        if (!transLiterator_) {
            HILOG_ERROR_I18N("The FfiI18nTransliterator instance is nullptr");
            return nullptr;
        }
        return transLiterator_->transform(text);
    }
}
}  // namespace I18n
}  // namespace Global
}  // namespace OHOS