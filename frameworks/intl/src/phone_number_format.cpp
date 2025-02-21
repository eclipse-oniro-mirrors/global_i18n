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
#include "phone_number_format.h"
#include <cctype>
#include <dlfcn.h>
#include <regex>
#include "unicode/localebuilder.h"
#include "locale_config.h"
#include "unicode/locid.h"
#include "i18n_hilog.h"
#include "map"
#include "new"
#include "set"
#include "securec.h"
#include "string"
#include "taboo_utils.h"
#include "utility"
#include "utils.h"

namespace OHOS {
namespace Global {
namespace I18n {
const int RECV_CHAR_LEN = 128;
using i18n::phonenumbers::PhoneNumberUtil;
void* PhoneNumberFormat::dynamicHandler = nullptr;
std::mutex PhoneNumberFormat::phoneMutex;
std::mutex PhoneNumberFormat::AS_YOU_TYPE_FORMAT_MUTEX;
size_t PhoneNumberFormat::MAX_NUMBER_LENGTH = 30;
std::map<char, char> PhoneNumberFormat::VALID_PHONE_NUMBER_CHARS {
    { '+', '+' },
    { ' ', ' ' },
    { '*', '*' },
    { '-', '-' },
    { '#', '#' },
    { '(', '(' },
    { ')', ')' },
    { ';', ';' },
    { ',', ',' },
    { 'A', '2' },
    { 'B', '2' },
    { 'C', '2' },
    { 'D', '3' },
    { 'E', '3' },
    { 'F', '3' },
    { 'G', '4' },
    { 'H', '4' },
    { 'I', '4' },
    { 'J', '5' },
    { 'K', '5' },
    { 'L', '5' },
    { 'M', '6' },
    { 'N', '6' },
    { 'O', '6' },
    { 'P', '7' },
    { 'Q', '7' },
    { 'R', '7' },
    { 'S', '7' },
    { 'T', '8' },
    { 'U', '8' },
    { 'V', '8' },
    { 'W', '9' },
    { 'X', '9' },
    { 'Y', '9' },
    { 'Z', '9' },
    { 'a', '2' },
    { 'b', '2' },
    { 'c', '2' },
    { 'd', '3' },
    { 'e', '3' },
    { 'f', '3' },
    { 'g', '4' },
    { 'h', '4' },
    { 'i', '4' },
    { 'j', '5' },
    { 'k', '5' },
    { 'l', '5' },
    { 'm', '6' },
    { 'n', '6' },
    { 'o', '6' },
    { 'p', '7' },
    { 'q', '7' },
    { 'r', '7' },
    { 's', '7' },
    { 't', '8' },
    { 'u', '8' },
    { 'v', '8' },
    { 'w', '9' },
    { 'x', '9' },
    { 'y', '9' },
    { 'z', '9' },
};

PhoneNumberFormat::PhoneNumberFormat(const std::string &countryTag,
                                     const std::map<std::string, std::string> &options)
{
    util = PhoneNumberUtil::GetInstance();
    if (LocaleConfig::IsValidRegion(countryTag)) {
        country = countryTag;
    } else {
        icu::Locale locale = icu::Locale::createFromName(countryTag.c_str());
        country = locale.getCountry();
    }
    std::string type = "";
    auto search = options.find("type");
    if (search != options.end()) {
        type = search->second;
    }

    std::map<std::string, PhoneNumberUtil::PhoneNumberFormat> type2PhoneNumberFormat = {
        {"E164", PhoneNumberUtil::PhoneNumberFormat::E164},
        {"RFC3966", PhoneNumberUtil::PhoneNumberFormat::RFC3966},
        {"INTERNATIONAL", PhoneNumberUtil::PhoneNumberFormat::INTERNATIONAL},
        {"NATIONAL", PhoneNumberUtil::PhoneNumberFormat::NATIONAL}
    };

    std::set<std::string> validType = {"E164", "RFC3966", "INTERNATIONAL", "NATIONAL"};
    if (validType.find(type) != validType.end()) {
        withOptions = true;
        phoneNumberFormat = type2PhoneNumberFormat[type];
    } else {
        phoneNumberFormat = PhoneNumberUtil::PhoneNumberFormat::NATIONAL;
    }
    if (type.compare("TYPING") == 0) {
        formatter = std::unique_ptr<AsYouTypeFormatter>(util->GetAsYouTypeFormatter(country));
    }
}

PhoneNumberFormat::~PhoneNumberFormat()
{
}

void PhoneNumberFormat::CloseDynamicHandler()
{
    std::lock_guard<std::mutex> phoneLock(phoneMutex);
    if (dynamicHandler != nullptr) {
        dlclose(dynamicHandler);
    }
    dynamicHandler = nullptr;
}

std::unique_ptr<PhoneNumberFormat> PhoneNumberFormat::CreateInstance(const std::string &countryTag,
    const std::map<std::string, std::string> &options)
{
    std::unique_ptr<PhoneNumberFormat> phoneNumberFormat = std::make_unique<PhoneNumberFormat>(countryTag, options);
    if (phoneNumberFormat->GetPhoneNumberUtil() == nullptr) {
        return nullptr;
    }
    return phoneNumberFormat;
}

PhoneNumberUtil* PhoneNumberFormat::GetPhoneNumberUtil()
{
    return util;
}

bool PhoneNumberFormat::isValidPhoneNumber(const std::string &number) const
{
    i18n::phonenumbers::PhoneNumber phoneNumber;
    PhoneNumberUtil::ErrorType type = util->Parse(number, country, &phoneNumber);
    if (type != PhoneNumberUtil::ErrorType::NO_PARSING_ERROR) {
        return false;
    }
    return util->IsValidNumber(phoneNumber);
}

std::string PhoneNumberFormat::format(const std::string &number)
{
    std::string formatted_number;
    if (formatter != nullptr) {
        formatted_number = GetAsYouTypeFormatResult(number);
        return PseudoLocalizationProcessor(formatted_number);
    }
    i18n::phonenumbers::PhoneNumber phoneNumber;
    PhoneNumberUtil::ErrorType type = util->ParseAndKeepRawInput(number, country, &phoneNumber);
    if (type != PhoneNumberUtil::ErrorType::NO_PARSING_ERROR) {
        return PseudoLocalizationProcessor("");
    }
    const std::string prefix = "106";
    if (number.compare(0, prefix.length(), prefix) == 0) {
        util->FormatInOriginalFormat(phoneNumber, country, &formatted_number);
    } else {
        util->Format(phoneNumber, phoneNumberFormat, &formatted_number);
    }
    return PseudoLocalizationProcessor(formatted_number);
}

std::string PhoneNumberFormat::GetAsYouTypeFormatResult(const std::string &number)
{
    std::lock_guard<std::mutex> formatLock(AS_YOU_TYPE_FORMAT_MUTEX);
    if (formatter == nullptr || number.length() > MAX_NUMBER_LENGTH) {
        return number;
    }
    std::regex pattern("[^\\d]");
    std::string phoneNumber = std::regex_replace(number, pattern, "");
    if (lastFormatNumber.length() > 0 && phoneNumber.length() == lastFormatNumber.length() + 1) {
        if (phoneNumber.compare(0, lastFormatNumber.length(), lastFormatNumber) != 0) {
            return FormatAllInputNumber(number, phoneNumber);
        }
        char lastChar = *(phoneNumber.rbegin());
        std::string result;
        this->lastFormatNumber = phoneNumber;
        return formatter->InputDigit(lastChar, &result);
    }

    return FormatAllInputNumber(number, phoneNumber);
}

std::string PhoneNumberFormat::FormatAllInputNumber(const std::string &originalNumber,
                                                    std::string &replacedNumber)
{
    formatter->Clear();
    this->lastFormatNumber = replacedNumber;
    std::string result;
    std::string formattedNumber;
    for (size_t i = 0; i < originalNumber.length(); i++) {
        char c = originalNumber.at(i);
        auto iter = VALID_PHONE_NUMBER_CHARS.find(c);
        if (isdigit(c)) {
            formattedNumber = formatter->InputDigit(c, &result);
        } else if (iter != VALID_PHONE_NUMBER_CHARS.end()) {
            char replacedChar = VALID_PHONE_NUMBER_CHARS[c];
            formattedNumber = formatter->InputDigit(replacedChar, &result);
        }
    }
    return formattedNumber;
}

std::string PhoneNumberFormat::getLocationName(
    const std::string &number, const std::string &locale)
{
    // Combine country and locale parameters
    UErrorCode status = U_ZERO_ERROR;
    icu::Locale displayLocale = icu::Locale::createFromName(locale.c_str());
    displayLocale.addLikelySubtags(status);
    icu::LocaleBuilder builder = icu::LocaleBuilder().setRegion(country);
    builder.setLanguage(displayLocale.getLanguage());
    builder.setScript(displayLocale.getScript());
    icu::Locale phoneLocale = builder.build(status);

    i18n::phonenumbers::PhoneNumber phoneNumber;
    PhoneNumberUtil::ErrorType type = util->Parse(number, phoneLocale.getCountry(), &phoneNumber);
    if (type != PhoneNumberUtil::ErrorType::NO_PARSING_ERROR) {
        return "";
    }
    std::string regionCode;
    util->GetRegionCodeForNumber(phoneNumber, &regionCode);

    std::string locName = getPhoneLocationName(number, phoneLocale.getName(), locale, regionCode);

    icu::LocaleBuilder regionbuilder = icu::LocaleBuilder().setRegion(regionCode);
    icu::Locale regionLocale = builder.build(status);
    icu::UnicodeString displayRegion;
    regionLocale.getDisplayCountry(displayLocale, displayRegion);
    std::string displayCountryName;
    displayRegion.toUTF8String(displayCountryName);

    // Check if region name is a country name
    if (locName.compare(displayCountryName) == 0) {
        if (getBlockedRegionName(regionCode, displayLocale.getLanguage())) {
            return "";
        }

        TabooUtils taboo;
        return taboo.ReplaceCountryName(regionCode, locale, displayCountryName);
    }

    // Process the city name
    std::string formatted_number;
    util->Format(phoneNumber, PhoneNumberUtil::PhoneNumberFormat::E164, &formatted_number);
    return getCityName(locale, formatted_number.substr(1), locName);
}

std::string PhoneNumberFormat::getPhoneLocationName(
    const std::string& number, const std::string& phoneLocale,
    const std::string& displayLocale, const std::string& regionCode)
{
    OpenHandler();
    std::lock_guard<std::mutex> phoneLock(phoneMutex);
    std::string locName;
    if (dynamicHandler) {
        HILOG_ERROR_I18N("LocationNameFunc Init");
        ExposeLocationName locationNameFunc = reinterpret_cast<ExposeLocationName>(
            dlsym(dynamicHandler, "exposeLocationName"));
        if (locationNameFunc) {
            // The function uses the same locale for phone and display.
            char recvArr[RECV_CHAR_LEN];
            const char* numberArr = number.c_str();
            const char* phoneLocaleArr = phoneLocale.c_str();
            locationNameFunc(numberArr, phoneLocaleArr, recvArr, RECV_CHAR_LEN);
            locName = recvArr;
        }
    }
    return locName;
}

void PhoneNumberFormat::OpenHandler()
{
    if (dynamicHandler == nullptr) {
        HILOG_INFO_I18N("DynamicHandler init.");
        std::lock_guard<std::mutex> phoneLock(phoneMutex);
        if (dynamicHandler == nullptr) {
            HILOG_INFO_I18N("DynamicHandler lock init.");
#ifndef SUPPORT_ASAN
            const char* geocodingSO = "libgeocoding.z.so";
#else
            const char* geocodingSO = "system/asan/lib64/platformsdk/libgeocoding.z.so";
#endif
            HILOG_INFO_I18N("DynamicHandler lock init.");
            dynamicHandler = dlopen(geocodingSO, RTLD_NOW);
        }
    }
}

bool PhoneNumberFormat::getBlockedRegionName(
    const std::string& regionCode, const std::string& language)
{
    void *i18nUtilHandle = dlopen("libi18n_util.z.so", RTLD_NOW);
    if (i18nUtilHandle == nullptr) {
        return false;
    }
    GetBlockedRegionName getBlockedRegionName = (GetBlockedRegionName)dlsym(i18nUtilHandle, "GetBlockedRegionName");
    bool ret = false;
    if (getBlockedRegionName) {
        ret = getBlockedRegionName(language.c_str(), regionCode.c_str());
    }
    dlclose(i18nUtilHandle);
    i18nUtilHandle = nullptr;
    return ret;
}

std::string PhoneNumberFormat::getCityName(
    const std::string& language, const std::string& phonenumber,
    const std::string& locationName)
{
    char recvArr[RECV_CHAR_LEN];
    int err = strcpy_s(recvArr, RECV_CHAR_LEN, locationName.c_str());
    if (err != 0) {
        return locationName;
    }
    void *i18nUtilHandle = dlopen("libi18n_util.z.so", RTLD_NOW);
    if (i18nUtilHandle == nullptr) {
        return locationName;
    }
    GetBlockedPhoneLocationName getBlockedPhoneLocationName =
        (GetBlockedPhoneLocationName)dlsym(i18nUtilHandle, "GetBlockedPhoneLocationName");
    if (getBlockedPhoneLocationName) {
        getBlockedPhoneLocationName(language.c_str(), phonenumber.c_str(), recvArr);
    }
    if (locationName.compare(recvArr) != 0) {
        dlclose(i18nUtilHandle);
        i18nUtilHandle = nullptr;
        return "";
    }

    GetReplacedPhoneLocationName getReplacedPhoneLocationName =
        (GetReplacedPhoneLocationName)dlsym(i18nUtilHandle, "GetReplacedPhoneLocationName");
    if (getReplacedPhoneLocationName) {
        getReplacedPhoneLocationName(language.c_str(), phonenumber.c_str(), recvArr);
    }
    dlclose(i18nUtilHandle);
    i18nUtilHandle = nullptr;
    std::string ret(recvArr);
    return ret;
}
} // namespace I18n
} // namespace Global
} // namespace OHOS
