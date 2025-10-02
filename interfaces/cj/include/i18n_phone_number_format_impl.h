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

#ifndef INTERFACES_CJ_INCLUDE_I18N_PHONE_NUMBER_FORMAT_IMPL_H
#define INTERFACES_CJ_INCLUDE_I18N_PHONE_NUMBER_FORMAT_IMPL_H

#include <string>
#include "ffi_remote_data.h"
#include "i18n_ffi.h"
#include "phone_number_format.h"

namespace OHOS {
namespace Global {
namespace I18n {
class FfiI18nPhoneNumberFormat : public OHOS::FFI::FFIData {
    DECL_TYPE(FfiI18nPhoneNumberFormat, FFIData)
public:
    explicit FfiI18nPhoneNumberFormat(std::unique_ptr<OHOS::Global::I18n::PhoneNumberFormat> phoneNumberFormat);
    ~FfiI18nPhoneNumberFormat() = default;
    char* format(std::string number);
    bool isValidNumber(std::string number);
    char* getLocationName(std::string number, std::string locale);
private:
    std::unique_ptr<OHOS::Global::I18n::PhoneNumberFormat> phoneNumberFormat_ = nullptr;
    friend class OHOS::FFI::RuntimeType;
    friend class OHOS::FFI::TypeBase;
};
}  // namespace I18n
}  // namespace Global
}  // namespace OHOS

#endif  // INTERFACES_CJ_INCLUDE_I18N_CALENDAR_IMPL_H_
