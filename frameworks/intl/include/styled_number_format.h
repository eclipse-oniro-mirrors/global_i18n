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

#ifndef OHOS_GLOBAL_I18N_STYLED_NUMBER_FORMAT_H
#define OHOS_GLOBAL_I18N_STYLED_NUMBER_FORMAT_H

#include <vector>

#include "number_format.h"
#include "simple_number_format.h"
#include "unicode/numberformatter.h"

namespace OHOS {
namespace Global {
namespace I18n {
class StyledNumberFormat {
public:
    struct NumberPart {
        std::string part_name;
        size_t start;
        size_t length;
    };

    StyledNumberFormat(bool isNumberFormat, std::shared_ptr<NumberFormat> numberFormat,
        std::shared_ptr<SimpleNumberFormat> simpleNumberFormat);
    std::string Format(double number);
    std::vector<StyledNumberFormat::NumberPart> ParseToParts(double number);

private:
    std::vector<StyledNumberFormat::NumberPart> ParseToParts(icu::number::FormattedNumber &formattedValue);
    std::vector<StyledNumberFormat::NumberPart> ParseToParts(icu::FormattedRelativeDateTime &formattedValue);
    void HandlePart(const std::string& part, std::unordered_map<std::string, size_t> &index,
        std::vector<StyledNumberFormat::NumberPart> &result);

    bool isNumberFormat;
    std::shared_ptr<NumberFormat> numberFormat;
    std::shared_ptr<SimpleNumberFormat> simpleNumberFormat;
};
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif