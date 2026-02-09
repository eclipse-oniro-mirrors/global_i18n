/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef OHOS_GLOBAL_I18N_STYLED_DATE_TIME_FORMAT_H
#define OHOS_GLOBAL_I18N_STYLED_DATE_TIME_FORMAT_H

#include <vector>
#include <unordered_set>
#include "date_time_format_part.h"
#include "intl_date_time_format.h"
#include "simple_date_time_format.h"
#include "unicode/smpdtfmt.h"

namespace OHOS {
namespace Global {
namespace I18n {
class StyledDateTimeFormat {
public:
    enum class InitStatus {
        FAILURE = 0,
        INIT_SIMPLE,
        INIT_INTL
    };

    StyledDateTimeFormat(std::shared_ptr<SimpleDateTimeFormat> simpleFormatter,
        std::shared_ptr<IntlDateTimeFormat> intlFormatter);

    std::pair<std::string, std::vector<DateTimeFormatPart>> Format(double date);

    static std::unordered_set<std::string> GetSupportedTypes();

private:
    std::shared_ptr<SimpleDateTimeFormat> simpleFormatter;
    std::shared_ptr<IntlDateTimeFormat> intlFormatter;
    InitStatus status = InitStatus::FAILURE;
};
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif