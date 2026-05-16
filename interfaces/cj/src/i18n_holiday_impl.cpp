/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "i18n_holiday_impl.h"
#include <string>
#include <vector>
#include "i18n_hilog.h"

namespace OHOS {
namespace Global {
namespace I18n {
CHolidayManager::CHolidayManager(char* icsPath)
{
    holidayManager_ = std::make_unique<HolidayManager>(icsPath);
    if (holidayManager_ == nullptr) {
        HILOG_ERROR_I18N("Create HolidayManager fail");
    }
}

std::vector<HolidayInfoItem> CHolidayManager::getHolidayInfoItemArray(int32_t year)
{
    std::vector<HolidayInfoItem> itemList = holidayManager_->GetHolidayInfoItemArray(year);
    return itemList;
}

std::vector<HolidayInfoItem> CHolidayManager::getHolidayInfoItemArray()
{
    std::vector<HolidayInfoItem> itemList = holidayManager_->GetHolidayInfoItemArray();
    return itemList;
}

bool CHolidayManager::isHoliday()
{
    return holidayManager_->IsHoliday();
}

bool CHolidayManager::isHoliday(int32_t year, int32_t month, int32_t day)
{
    return holidayManager_->IsHoliday(year, month, day);
}
}  // namespace I18n
}  // namespace Global
}  // namespace OHOS