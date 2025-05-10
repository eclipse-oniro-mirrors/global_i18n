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
#include <map>
#include <vector>
#include "i18n_hilog.h"
#include "i18n_struct.h"
#include "i18n_phone_number_format_ffi.h"
#include "i18n_phone_number_format_impl.h"
#include <utility>
 
namespace OHOS {
namespace Global {
namespace I18n {
using namespace OHOS::FFI;
using namespace OHOS::HiviewDFX;
extern "C"
{
    int64_t FfiI18nPhoneNumberFormatConstructor(char* country, char* formatType)
    {
        std::map<std::string, std::string> options;
        if (formatType != nullptr) {
            options.insert(std::make_pair("type", std::string(formatType)));
        }
        std::unique_ptr<PhoneNumberFormat> phoneNumberFormatInstance
            = PhoneNumberFormat::CreateInstance(country, options);
        if (phoneNumberFormatInstance == nullptr) {
            HILOG_ERROR_I18N("Create PhoneNumberFormat fail");
            return -1;
        }
        auto ffiI18nPhoneNumberFormatInstance
            = FFIData::Create<FfiI18nPhoneNumberFormat>(std::move(phoneNumberFormatInstance));
        if (ffiI18nPhoneNumberFormatInstance == nullptr) {
            HILOG_ERROR_I18N("Create FfiI18nPhoneNumberFormat fail");
            return -1;
        }
        return ffiI18nPhoneNumberFormatInstance->GetID();
    }

    char* FfiI18nPhoneNumberFormatFormat(int64_t remoteDataID, char* number)
    {
        auto format = FFIData::GetData<FfiI18nPhoneNumberFormat>(remoteDataID);
        if (!format) {
            HILOG_ERROR_I18N("The FfiI18nPhoneNumberFormat instance is nullptr");
            return nullptr;
        }
        return format->format(std::string(number));
    }
    bool FfiI18nPhoneNumberFormatIsValidNumber(int64_t remoteDataID, char* number)
    {
        auto format = FFIData::GetData<FfiI18nPhoneNumberFormat>(remoteDataID);
        if (!format) {
            HILOG_ERROR_I18N("The FfiI18nPhoneNumberFormat instance is nullptr");
            return false;
        }
        return format->isValidNumber(std::string(number));
    }
    char* FfiI18nPhoneNumberFormatGetLocationName(int64_t remoteDataID, char* number, char* locale)
    {
        auto format = FFIData::GetData<FfiI18nPhoneNumberFormat>(remoteDataID);
        if (!format) {
            HILOG_ERROR_I18N("The FfiI18nPhoneNumberFormat instance is nullptr");
            return nullptr;
        }
        return format->getLocationName(std::string(number), std::string(locale));
    }
}
}  // namespace I18n
}  // namespace Global
}  // namespace OHOS