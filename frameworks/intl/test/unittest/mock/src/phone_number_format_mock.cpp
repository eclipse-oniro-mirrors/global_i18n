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
#include "phone_number_format_mock.h"

namespace OHOS {
namespace Global {
namespace I18n {
PhoneNumberFormatMock::PhoneNumberFormatMock(const std::string &countryTag,
    const std::map<std::string, std::string> &options)
    : PhoneNumberFormat(countryTag, options)
{
}

bool PhoneNumberFormatMock::getBlockedRegionName(const std::string& regionCode, const std::string& language)
{
    if (!regionCode.compare("XK")) {
        return true;
    } else {
        return false;
    }
}

std::string PhoneNumberFormatMock::getCityName(const std::string& language, const std::string& phonenumber,
    const std::string& locationName)
{
    std::string zhLocaleStr = "zh";
    size_t len = zhLocaleStr.size();
    std::string phoneNumberPrefix = "861373193";
    size_t prefixLen = phoneNumberPrefix.size();
    if (!phonenumber.compare(0, prefixLen, "861373193") && !language.compare(0, len, "zh")) {
        return "安徽省宣城市2";
    }
    if (!phonenumber.compare(0, prefixLen, "861373163") && !language.compare(0, len, "zh")) {
        return "";
    }
    return locationName;
}
} // namespace I18n
} // namespace Global
} // namespace OHOS