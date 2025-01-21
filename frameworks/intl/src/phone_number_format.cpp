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

#include <dlfcn.h>

#include "localebuilder.h"
#include "locale_config.h"
#include "locid.h"
#include "hilog/log.h"
#include "map"
#include "new"
#include "set"
#include "securec.h"
#include "string"
#include "taboo_utils.h"
#include "utility"


namespace OHOS {
namespace Global {
namespace I18n {
const int RECV_CHAR_LEN = 128;
using i18n::phonenumbers::PhoneNumberUtil;
static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, 0xD001E00, "PhoneNumberFormat" };
using namespace OHOS::HiviewDFX;
void* PhoneNumberFormat::dynamicHandler = nullptr;
std::mutex PhoneNumberFormat::phoneMutex;

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
        phoneNumberFormat = type2PhoneNumberFormat[type];
    } else {
        phoneNumberFormat = PhoneNumberUtil::PhoneNumberFormat::NATIONAL;
    }
}

PhoneNumberFormat::~PhoneNumberFormat()
{
}

void PhoneNumberFormat::CloseDynamicHandler()
{
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

std::string PhoneNumberFormat::format(const std::string &number) const
{
    i18n::phonenumbers::PhoneNumber phoneNumber;
    PhoneNumberUtil::ErrorType type = util->Parse(number, country, &phoneNumber);
    if (type != PhoneNumberUtil::ErrorType::NO_PARSING_ERROR) {
        return "";
    }
    std::string formatted_number;
    util->Format(phoneNumber, phoneNumberFormat, &formatted_number);
    return formatted_number;
}

std::string PhoneNumberFormat::getLocationName(const std::string &number, const std::string &locale)
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

std::string PhoneNumberFormat::getPhoneLocationName(const std::string& number, const std::string& phoneLocale,
    const std::string& displayLocale, const std::string& regionCode)
{
    OpenHandler();
    if (dynamicHandler && !locationNameFunc) {
        HiLog::Error(LABEL, "LocationNameFunc Init");
        locationNameFunc = reinterpret_cast<ExposeLocationName>(dlsym(dynamicHandler, "exposeLocationName"));
    }
    std::string locName;
    if (locationNameFunc) {
        // The function uses the same locale for phone and display.
        char recvArr[RECV_CHAR_LEN];
        locationNameFunc(number.c_str(), phoneLocale.c_str(), recvArr, RECV_CHAR_LEN);
        locName = recvArr;
    }
    if (!locationNameFunc || locName.empty()) {
        locName = LocaleConfig::GetDisplayRegion(regionCode, displayLocale, false);
    }
    return locName;
}

void PhoneNumberFormat::OpenHandler()
{
    if (dynamicHandler == nullptr) {
        HiLog::Info(LABEL, "DynamicHandler init.");
        std::lock_guard<std::mutex> phoneLock(phoneMutex);
        if (dynamicHandler == nullptr) {
#ifndef SUPPORT_ASAN
            const char* geocodingSO = "libgeocoding.z.so";
#else
            const char* geocodingSO = "system/asan/lib64/libgeocoding.z.so";
#endif
            HiLog::Info(LABEL, "DynamicHandler lock init.");
            dynamicHandler = dlopen(geocodingSO, RTLD_NOW);
        }
    }
}

bool PhoneNumberFormat::getBlockedRegionName(const std::string& regionCode, const std::string& language)
{
    void *i18nUtilHandle = dlopen("libi18n_util.z.so", RTLD_NOW);
    if (i18nUtilHandle == nullptr) {
        return false;
    }
    GetBlockedRegionName getBlockedRegionName = (GetBlockedRegionName)dlsym(i18nUtilHandle, "GetBlockedRegionName");
    if (getBlockedRegionName) {
        return getBlockedRegionName(language.c_str(), regionCode.c_str());
    } else {
        return false;
    }
}

std::string PhoneNumberFormat::getCityName(const std::string& language, const std::string& phonenumber,
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
        return "";
    }

    GetReplacedPhoneLocationName getReplacedPhoneLocationName =
        (GetReplacedPhoneLocationName)dlsym(i18nUtilHandle, "GetReplacedPhoneLocationName");
    if (getReplacedPhoneLocationName) {
        getReplacedPhoneLocationName(language.c_str(), phonenumber.c_str(), recvArr);
    }
    std::string ret(recvArr);
    return ret;
}
} // namespace I18n
} // namespace Global
} // namespace OHOS
