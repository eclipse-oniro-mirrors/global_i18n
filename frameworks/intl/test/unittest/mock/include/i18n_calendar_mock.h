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
#ifndef OHOS_GLOBAL_I18N_I18N_CALENDAR_MOCK_H
#define OHOS_GLOBAL_I18N_I18N_CALENDAR_MOCK_H

#include <gmock/gmock.h>
#include "i18n_calendar.h"

namespace OHOS {
namespace Global {
namespace I18n {
class I18nCalendarMock : public I18nCalendar {
public:
    I18nCalendarMock(std::string localeTag) : I18nCalendar(localeTag) {
    };
    ~I18nCalendarMock() override = default;
    MOCK_CONST_METHOD0(GetIcuCalendar, icu::Calendar*());
};
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif