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

#ifndef INTERFACES_CJ_INCLUDE_I18N_HOLIDAY_IMPL_H_
#define INTERFACES_CJ_INCLUDE_I18N_HOLIDAY_IMPL_H_

#include <string>
#include <vector>
#include "holiday_manager.h"
#include "ffi_remote_data.h"
#include "i18n_struct.h"

namespace OHOS {
namespace Global {
namespace I18n {
class CHolidayManager : public OHOS::FFI::FFIData {
    DECL_TYPE(CHolidayManager, FFIData)
public:
    explicit CHolidayManager(char* icsPath);
    ~CHolidayManager() = default;
    std::vector<HolidayInfoItem> getHolidayInfoItemArray(int32_t year);
    std::vector<HolidayInfoItem> getHolidayInfoItemArray();
    bool isHoliday();
    bool isHoliday(int32_t year, int32_t month, int32_t day);
private:
   std::unique_ptr<HolidayManager> holidayManager_;
};

}  // namespace I18n
}  // namespace Global
}  // namespace OHOS

#endif  // INTERFACES_CJ_INCLUDE_I18N_HOLIDAY_IMPL_H_
