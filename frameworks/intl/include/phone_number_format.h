/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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
#ifndef OHOS_GLOBAL_I18N_PHONE_NUMBER_FORMAT_H
#define OHOS_GLOBAL_I18N_PHONE_NUMBER_FORMAT_H

#include <map>
#include <set>
#include <string>
#include <mutex>
#include "memory"
#include "phonenumbers/phonenumberutil.h"
#include "phonenumbers/asyoutypeformatter.h"

namespace OHOS {
namespace Global {
namespace I18n {
using i18n::phonenumbers::PhoneNumberUtil;
using i18n::phonenumbers::AsYouTypeFormatter;
using ExposeLocationName = int (*)(const char*, const char*, char*, const int, bool);

class PhoneNumberFormat {
public:
    PhoneNumberFormat(const std::string &countryTag, const std::map<std::string, std::string> &options);
    virtual ~PhoneNumberFormat();
    bool isValidPhoneNumber(const std::string &number) const;
    std::string format(const std::string &number);
    static std::unique_ptr<PhoneNumberFormat> CreateInstance(const std::string &countryTag,
                                                             const std::map<std::string, std::string> &options);
    std::string getLocationName(const std::string &number, const std::string &locale);
    std::string getPhoneLocationName(const std::string& number, const std::string& phoneLocale,
        const std::string& displayLocale);
    virtual bool getBlockedRegionName(const std::string& regionCode, const std::string& language);
    virtual std::string getCityName(const std::string& language, const std::string& phonenumber,
        const std::string& locationName);
    static void CloseDynamicHandler();

private:
    PhoneNumberUtil* GetPhoneNumberUtil();
    void OpenHandler();
    std::string GetAsYouTypeFormatResult(const std::string &number);
    std::string FormatAllInputNumber(const std::string &originalNumber, std::string &replacedNumber);
    bool IsNumberTooLong(i18n::phonenumbers::PhoneNumber phoneNumber);
    std::string GetPossibleRegionCode(const i18n::phonenumbers::PhoneNumber &phoneNumber);
    PhoneNumberUtil *util;
    std::unique_ptr<AsYouTypeFormatter> formatter = nullptr;
    std::string country;
    std::string formatType;
    PhoneNumberUtil::PhoneNumberFormat phoneNumberFormat;
    static void* dynamicHandler;
    static std::mutex phoneMutex;
    static std::mutex AS_YOU_TYPE_FORMAT_MUTEX;
    static size_t MAX_NUMBER_LENGTH;
    std::string lastFormatNumber;
    bool withOptions = false;
    static std::map<char, char> VALID_PHONE_NUMBER_CHARS;
};
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif
