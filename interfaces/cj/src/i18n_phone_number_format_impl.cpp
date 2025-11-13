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

#include "i18n_phone_number_format_impl.h"

namespace OHOS {
namespace Global {
namespace I18n {
extern "C"
{
    FfiI18nPhoneNumberFormat::FfiI18nPhoneNumberFormat
    (std::unique_ptr<PhoneNumberFormat> phoneNumberFormat)
    {
        phoneNumberFormat_ = std::move(phoneNumberFormat);
    }

    char* FfiI18nPhoneNumberFormat::format(std::string number)
    {
        return MallocCString(phoneNumberFormat_->format(number));
    }

    bool  FfiI18nPhoneNumberFormat::isValidNumber(std::string number)
    {
        return phoneNumberFormat_->isValidPhoneNumber(number);
    }

    char* FfiI18nPhoneNumberFormat::getLocationName(std::string number, std::string locale)
    {
        return MallocCString(phoneNumberFormat_->getLocationName(number, locale));
    }
}
}  // namespace I18n
}  // namespace Global
}  // namespace OHOS