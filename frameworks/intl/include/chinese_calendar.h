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
#ifndef OHOS_GLOBAL_I18N_CHINESE_CALENDAR_H
#define OHOS_GLOBAL_I18N_CHINESE_CALENDAR_H

#include <cstdint>
#include <string>

#include "i18n_calendar.h"
#include "i18n_types.h"
#include "unicode/calendar.h"
#include "unicode/ucal.h"
#include "unicode/utypes.h"

namespace OHOS {
namespace Global {
namespace I18n {

struct ChineseCalendarTime {
    int32_t gregorianYear = 0;
    int32_t cyclicalYear = 0;
    int32_t month = 0;
    int32_t date = 0;
    int32_t hour = 0;
    int32_t minute = 0;
    int32_t second = 0;
    bool isLeapMonth = false;
    static const std::unordered_map<std::string, int32_t ChineseCalendarTime::*> MEMBER_NAME_MAP;
};

class ChineseCalendar : public I18nCalendar {
public:
    explicit ChineseCalendar(const std::string& localeTag);
    ~ChineseCalendar();
    void SetChineseCalendarTime(const ChineseCalendarTime& time, I18nErrorCode& errCode);
    static bool CheckLeapMonth(int32_t gregorianYear, int32_t cyclicalYear, int32_t month, I18nErrorCode& errCode);

private:
    static int32_t ParseEra(int32_t gregorianYear, int32_t cyclicalYear, I18nErrorCode& errCode);
    static bool IsValidYear(int32_t gregorianYear, int32_t cyclicalYear);
};
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif