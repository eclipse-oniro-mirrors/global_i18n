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

#ifndef INTERFACES_CJ_INCLUDE_I18N_TRANSLITERATOR_IMPL_H
#define INTERFACES_CJ_INCLUDE_I18N_TRANSLITERATOR_IMPL_H

#include <string>
#include "ffi_remote_data.h"
#include "unicode/translit.h"

namespace OHOS {
namespace Global {
namespace I18n {
class FfiI18nTransliterator : public OHOS::FFI::FFIData {
    DECL_TYPE(FfiI18nTimeZone, FFIData)
public:
    explicit FfiI18nTransliterator(std::unique_ptr<icu::Transliterator> transliterator_);
    ~FfiI18nTransliterator() = default;
    char* transform(char* text);
private:
    std::unique_ptr<icu::Transliterator> transliterator_ = nullptr;
    friend class OHOS::FFI::RuntimeType;
    friend class OHOS::FFI::TypeBase;
};
}  // namespace I18n
}  // namespace Global
}  // namespace OHOS

#endif  // INTERFACES_CJ_INCLUDE_I18N_CALENDAR_IMPL_H_
