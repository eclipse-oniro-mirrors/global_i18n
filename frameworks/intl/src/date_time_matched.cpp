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
#include "hilog/log.h"

namespace OHOS {
namespace Global {
namespace I18n {
static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, 0xD001E00, "DateTimeMatched" };
using namespace OHOS::HiviewDFX;
DateTimeMatched::DateTimeMatched(std::string& locale)
{
    dateRuleInit = new DateRuleInit(locale);
    if (dateRuleInit == nullptr) {
        HiLog::Error(LABEL, "DateRuleInit construct failed.");
    }
}

DateTimeMatched::~DateTimeMatched()
{
    delete dateRuleInit;
}

std::vector<int> DateTimeMatched::GetMatchedDateTime(icu::UnicodeString& message)
{
    icu::UnicodeString messageStr = message;
    std::vector<int> result {0};
    if (this->dateRuleInit == nullptr) {
        HiLog::Error(LABEL, "GetMatchedDateTime failed because this->dateRuleInit is nullptr.");
        return result;
    }
    std::vector<MatchedDateTimeInfo> matches = this->dateRuleInit->Detect(messageStr);
    int length = matches.size();
    if (length > 0) {
        int posDateTime = 2;
        int posBegin = 1;
        int posEnd = 2;
        result.resize(posDateTime * length + 1);
        result[0] = length;
        for (int i = 0; i < length; i++) {
            result[i * posDateTime + posBegin] = matches[i].GetBegin();
            result[i * posDateTime + posEnd] = matches[i].GetEnd();
        }
    }
    return result;
}
} // namespace I18n
} // namespace Global
} // namespace OHOS