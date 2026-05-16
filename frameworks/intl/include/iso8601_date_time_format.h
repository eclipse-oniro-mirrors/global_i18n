/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
#ifndef OHOS_GLOBAL_I18N_ISO8601_DATE_TIME_FORMAT_H
#define OHOS_GLOBAL_I18N_ISO8601_DATE_TIME_FORMAT_H

#include <string>
#include "i18n_types.h"
#include "unicode/smpdtfmt.h"

namespace OHOS {
namespace Global {
namespace I18n {
class ISO8601DateTimeFormat {
public:
    enum class DateFormat {
        CALENDAR,
        ORDINAL,
        WEEK
    };

    enum class TimePrecision {
        DATE_ONLY,
        HOURS,
        MINUTES,
        SECONDS,
        MILLISECONDS
    };

    enum class SeparatorStyle {
        EXTENDED,
        BASIC
    };

    ISO8601DateTimeFormat(DateFormat dateFormat, TimePrecision timePrecision,
        SeparatorStyle separatorStyle, const std::string& timeZone, bool displayTimeZone,
        I18nErrorCode& errCode);
    ~ISO8601DateTimeFormat() = default;
    std::string Format(int64_t milliseconds);

private:
    void InitFormatters(I18nErrorCode& errCode);
    std::string BuildPattern() const;
    std::string GetDateFormatPattern() const;
    std::string GetTimePrecisionPattern() const;
    std::string GetTimeZonePattern() const;

    DateFormat dateFormat_;
    TimePrecision timePrecision_;
    SeparatorStyle separatorStyle_;
    std::string timeZone_;
    bool displayTimeZone_;
    std::unique_ptr<icu::SimpleDateFormat> formatter_;
    static bool icuInitialized;
    static bool Init();
};
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif