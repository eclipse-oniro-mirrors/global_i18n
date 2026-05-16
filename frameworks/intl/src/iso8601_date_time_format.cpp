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

#include "iso8601_date_time_format.h"
#include "i18n_hilog.h"
#include "locale_config.h"
#include "ohos/init_data.h"
#include "unicode/calendar.h"
#include "unicode/timezone.h"

namespace OHOS {
namespace Global {
namespace I18n {
bool ISO8601DateTimeFormat::icuInitialized = ISO8601DateTimeFormat::Init();

bool ISO8601DateTimeFormat::Init()
{
    SetHwIcuDirectory();
    return true;
}

ISO8601DateTimeFormat::ISO8601DateTimeFormat(DateFormat dateFormat, TimePrecision timePrecision,
    SeparatorStyle separatorStyle, const std::string& timeZone, bool displayTimeZone,
    I18nErrorCode& errCode)
    : dateFormat_(dateFormat), timePrecision_(timePrecision), separatorStyle_(separatorStyle),
      timeZone_(timeZone), displayTimeZone_(displayTimeZone)
{
    if (!icuInitialized) {
        HILOG_ERROR_I18N("ISO8601DateTimeFormat: ICU not initialized.");
        errCode = I18nErrorCode::INITIALIZATION_ERROR;
        return;
    }
    InitFormatters(errCode);
}

void ISO8601DateTimeFormat::InitFormatters(I18nErrorCode& errCode)
{
    std::string pattern = BuildPattern();
    UErrorCode status = U_ZERO_ERROR;
    icu::UnicodeString unicodePattern = icu::UnicodeString::fromUTF8(pattern);
    formatter_ = std::make_unique<icu::SimpleDateFormat>(unicodePattern, status);

    if (U_FAILURE(status)) {
        HILOG_ERROR_I18N("ISO8601DateTimeFormat::InitFormatters: Failed to create SimpleDateFormat.");
        errCode = I18nErrorCode::INITIALIZATION_ERROR;
        return;
    }

    // Set timezone if specified
    if (!timeZone_.empty()) {
        std::unique_ptr<icu::TimeZone> tz(icu::TimeZone::createTimeZone(timeZone_.c_str()));
        if (tz != nullptr) {
            formatter_->setTimeZone(*tz);
        }
    } else {
        // Use UTC timezone
        std::unique_ptr<icu::TimeZone> tz(icu::TimeZone::createTimeZone("UTC"));
        if (tz != nullptr) {
            formatter_->setTimeZone(*tz);
        }
    }

    errCode = I18nErrorCode::SUCCESS;
}

std::string ISO8601DateTimeFormat::BuildPattern() const
{
    std::string pattern;
    pattern += GetDateFormatPattern();

    if (timePrecision_ != TimePrecision::DATE_ONLY) {
        pattern += GetTimePrecisionPattern();
    }

    if (displayTimeZone_ && timePrecision_ != TimePrecision::DATE_ONLY) {
        pattern += GetTimeZonePattern();
    }

    return pattern;
}

std::string ISO8601DateTimeFormat::GetDateFormatPattern() const
{
    switch (dateFormat_) {
        case DateFormat::CALENDAR:
            if (separatorStyle_ == SeparatorStyle::EXTENDED) {
                return "yyyy-MM-dd";
            } else {
                return "yyyyMMdd";
            }
        case DateFormat::ORDINAL:
            if (separatorStyle_ == SeparatorStyle::EXTENDED) {
                return "yyyy-DDD";
            } else {
                return "yyyyDDD";
            }
        case DateFormat::WEEK:
            if (separatorStyle_ == SeparatorStyle::EXTENDED) {
                return "YYYY-'W'ww-e";
            } else {
                return "YYYY'W'wwe";
            }
        default:
            if (separatorStyle_ == SeparatorStyle::EXTENDED) {
                return "yyyy-MM-dd";
            } else {
                return "yyyyMMdd";
            }
    }
}

std::string ISO8601DateTimeFormat::GetTimePrecisionPattern() const
{
    switch (timePrecision_) {
        case TimePrecision::HOURS:
            if (separatorStyle_ == SeparatorStyle::EXTENDED) {
                return "'T'HH";
            } else {
                return "'T'HH";
            }
        case TimePrecision::MINUTES:
            if (separatorStyle_ == SeparatorStyle::EXTENDED) {
                return "'T'HH:mm";
            } else {
                return "'T'HHmm";
            }
        case TimePrecision::SECONDS:
            if (separatorStyle_ == SeparatorStyle::EXTENDED) {
                return "'T'HH:mm:ss";
            } else {
                return "'T'HHmmss";
            }
        case TimePrecision::MILLISECONDS:
            if (separatorStyle_ == SeparatorStyle::EXTENDED) {
                return "'T'HH:mm:ss.SSS";
            } else {
                return "'T'HHmmss.SSS";
            }
        default:
            return "";
    }
}

std::string ISO8601DateTimeFormat::GetTimeZonePattern() const
{
    if (separatorStyle_ == SeparatorStyle::EXTENDED) {
        return "XXX";
    } else {
        return "XX";
    }
}

std::string ISO8601DateTimeFormat::Format(int64_t milliseconds)
{
    if (formatter_ == nullptr) {
        HILOG_ERROR_I18N("ISO8601DateTimeFormat::Format: formatter is nullptr.");
        return "";
    }

    UDate date = static_cast<UDate>(milliseconds);
    icu::UnicodeString result;
    formatter_->format(date, result);

    std::string resultStr;
    result.toUTF8String(resultStr);
    return resultStr;
}
} // namespace I18n
} // namespace Global
} // namespace OHOS