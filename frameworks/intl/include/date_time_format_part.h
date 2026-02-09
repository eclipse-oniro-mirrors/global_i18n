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
#ifndef GLOBAL_I18N_DATE_TIME_FORMAT_PART_H
#define GLOBAL_I18N_DATE_TIME_FORMAT_PART_H

#include <memory>
#include <string>
#include <vector>
#include "unicode/smpdtfmt.h"

namespace OHOS {
namespace Global {
namespace I18n {
class DateTimeFormatPart {
public:
    DateTimeFormatPart(int32_t fieldId, size_t start, size_t length);
    ~DateTimeFormatPart();

    void SetStart(size_t value);
    std::string GetPartName() const;
    size_t GetStart() const;
    size_t GetLength() const;

    static std::string ConvertFieldIdToDateType(int32_t fieldId);
    static std::pair<icu::UnicodeString, std::vector<DateTimeFormatPart>> ParseToParts(
        std::shared_ptr<icu::SimpleDateFormat> icuSimpleDateFormat, double milliseconds);

private:
    std::string partName;
    size_t start;
    size_t length;
};
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif
