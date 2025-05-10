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
#ifndef OHOS_GLOBAL_I18N_PHONE_NUMBER_FORMAT_MOCK_H
#define OHOS_GLOBAL_I18N_PHONE_NUMBER_FORMAT_MOCK_H

#include "phone_number_format.h"

namespace OHOS {
namespace Global {
namespace I18n {
class PhoneNumberFormatMock : public PhoneNumberFormat {
public:
    PhoneNumberFormatMock(const std::string &countryTag, const std::map<std::string, std::string> &options);
    virtual bool getBlockedRegionName(const std::string& regionCode, const std::string& language) override;
    virtual std::string getCityName(const std::string& language, const std::string& phonenumber,
        const std::string& locationName) override;
};
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif