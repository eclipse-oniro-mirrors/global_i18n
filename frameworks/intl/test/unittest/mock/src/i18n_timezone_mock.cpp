/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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
#include "i18n_timezone_mock.h"

namespace OHOS {
namespace Global {
namespace I18n {
I18nTimeZoneMock::I18nTimeZoneMock(std::string &id, bool isZoneID)
    : I18nTimeZone(id, isZoneID)
{
}

std::string I18nTimeZoneMock::GetDisplayNameByTaboo(const std::string& localeStr, std::string result)
{
    std::string zhLocaleStr = "zh";
    size_t len = zhLocaleStr.size();
    if (!localeStr.compare(0, len, "zh") && !GetID().compare("Pacific/Enderbury")) {
        return "菲尼克斯群岛标准时间";
    }
    return result;
}
} // namespace I18n
} // namespace Global
} // namespace OHOS