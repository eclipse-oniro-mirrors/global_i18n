/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "styled_date_time_format.h"

#include "i18n_hilog.h"

namespace OHOS {
namespace Global {
namespace I18n {
const std::unordered_set<std::string> SUPPORT_TYPES_OF_TEXT_STYLE = {
    "year",
    "month",
    "day",
    "hour",
    "minute",
    "second",
    "weekday",
    "era",
    "timeZoneName",
    "dayPeriod"
};

StyledDateTimeFormat::StyledDateTimeFormat(std::shared_ptr<SimpleDateTimeFormat> simpleFormatter,
    std::shared_ptr<IntlDateTimeFormat> intlFormatter) : simpleFormatter(simpleFormatter), intlFormatter(intlFormatter)
{
    if (simpleFormatter != nullptr) {
        status = InitStatus::INIT_SIMPLE;
    }
    if (intlFormatter != nullptr) {
        status = InitStatus::INIT_INTL;
    }
}

std::pair<std::string, std::vector<DateTimeFormatPart>> StyledDateTimeFormat::Format(double milliseconds)
{
    if (status == InitStatus::FAILURE) {
        return {};
    }
    std::pair<std::string, std::vector<DateTimeFormatPart>> result;
    if (status == InitStatus::INIT_SIMPLE) {
        result = simpleFormatter->FormatToParts(milliseconds);
    } else {
        result = intlFormatter->FormatToParts(milliseconds);
    }
    return result;
}

std::unordered_set<std::string> StyledDateTimeFormat::GetSupportedTypes()
{
    return SUPPORT_TYPES_OF_TEXT_STYLE;
}
} // namespace I18n
} // namespace Global
} // namespace OHOS