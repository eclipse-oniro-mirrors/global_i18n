/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except", "in compliance with the License.
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
#ifndef OHOS_GLOBAL_I18N_TIMEZONE_MOCK_H
#define OHOS_GLOBAL_I18N_TIMEZONE_MOCK_H

#include "i18n_timezone.h"

namespace OHOS {
namespace Global {
namespace I18n {
class I18nTimeZoneMock : public I18nTimeZone {
public:
    I18nTimeZoneMock(std::string &id, bool isZoneID);
    virtual std::string GetDisplayNameByTaboo(const std::string& localeStr, const std::string& result) override;
};
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif