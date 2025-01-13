/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#include "date_time_matched.h"
#include "i18n_hilog.h"

namespace OHOS {
namespace Global {
namespace I18n {
DateTimeMatched::DateTimeMatched(std::string& locale)
{
    dateRuleInit = new DateRuleInit(locale);
    if (dateRuleInit == nullptr) {
        HILOG_ERROR_I18N("DateRuleInit construct failed.");
    }
}

DateTimeMatched::~DateTimeMatched()
{
    if (dateRuleInit != nullptr) {
        delete dateRuleInit;
    }
}

std::vector<int> DateTimeMatched::GetMatchedDateTime(icu::UnicodeString& message)
{
    icu::UnicodeString messageStr = message;
    std::vector<int> result {0};
    if (this->dateRuleInit == nullptr) {
        HILOG_ERROR_I18N("GetMatchedDateTime failed because this->dateRuleInit is nullptr.");
        return result;
    }
    std::vector<MatchedDateTimeInfo> matches = this->dateRuleInit->Detect(messageStr);
    size_t length = matches.size();
    if (length > 0) {
        size_t posDateTime = 2;
        size_t posBegin = 1;
        size_t posEnd = 2;
        result.resize(posDateTime * length + 1);
        result[0] = static_cast<int>(length);
        for (size_t i = 0; i < length; i++) {
            result[i * posDateTime + posBegin] = matches[i].GetBegin();
            result[i * posDateTime + posEnd] = matches[i].GetEnd();
        }
    }
    return result;
}
} // namespace I18n
} // namespace Global
} // namespace OHOS