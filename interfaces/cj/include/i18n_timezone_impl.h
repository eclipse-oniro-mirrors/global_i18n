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

#ifndef INTERFACES_CJ_INCLUDE_I18N_TIMEZONE_IMPL_H
#define INTERFACES_CJ_INCLUDE_I18N_TIMEZONE_IMPL_H

#include <string>
#include "ffi_remote_data.h"
#include "i18n_struct.h"
#include "i18n_timezone.h"

namespace OHOS {
namespace Global {
namespace I18n {
class FfiI18nTimeZone : public OHOS::FFI::FFIData {
    DECL_TYPE(FfiI18nTimeZone, FFIData)
public:
    explicit FfiI18nTimeZone(std::unique_ptr<OHOS::Global::I18n::I18nTimeZone> timezone_);
    ~FfiI18nTimeZone() = default;
    char* getID();
    int32_t getRawOffset();
    int32_t getOffset(double date);
    char* getDisplayName();
    char* getDisplayName(std::string locale);
    char* getDisplayName(std::string locale, bool isDST);
    char* getDisplayName(bool isDST);
private:
    std::unique_ptr<OHOS::Global::I18n::I18nTimeZone> timezone_ = nullptr;
    friend class OHOS::FFI::RuntimeType;
    friend class OHOS::FFI::TypeBase;
};
}  // namespace I18n
}  // namespace Global
}  // namespace OHOS

#endif  // INTERFACES_CJ_INCLUDE_I18N_CALENDAR_IMPL_H_
