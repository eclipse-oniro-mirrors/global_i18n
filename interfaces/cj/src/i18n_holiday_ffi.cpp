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

#include "i18n_holiday_ffi.h"
#include "i18n_struct.h"
#include "ffi_remote_data.h"
#include "i18n_ffi.h"
#include "i18n_holiday_impl.h"
#include "i18n_hilog.h"

namespace OHOS {
namespace Global {
namespace I18n {
using namespace OHOS::FFI;
#define HOLIDAY_OPT_SUCCESS 0
#define HOLIDAY_OPT_FAILED (-1)
extern "C" {
int64_t FfiOHOSHolidayManagerImplConstructor(char* icsPath)
{
    auto instance = FFIData::Create<CHolidayManager>(icsPath);
    if (instance == nullptr) {
        HILOG_ERROR_I18N("Create CHolidayManager fail");
        return -1;
    }
    return instance->GetID();
}

bool FfiOHOSHolidayManagerIsHoliday(int64_t id, CDate date)
{
    auto instance = FFIData::GetData<CHolidayManager>(id);
    if (!instance) {
        HILOG_ERROR_I18N("The CHolidayManager instance is nullptr");
        return false;
    }
    if (date.isNull) {
        return instance->isHoliday();
    } else {
        return instance->isHoliday(date.year, date.month, date.day);
    }
}

static int32_t HolidayLocalNames2Cj(const std::vector<HolidayLocalName>& localNames, HolidayLocalNameArr &arr)
{
    int64_t size = static_cast<int64_t>(localNames.size());
    if (size > 0) {
        arr.head = static_cast<CHolidayLocalName *>(malloc(sizeof(CHolidayLocalName) * size));
        if (arr.head == nullptr) {
            return HOLIDAY_OPT_FAILED;
        }
        arr.size = size;

        uint32_t idx = 0;
        for (const auto& each : localNames) {
            CHolidayLocalName info;
            info.language = MallocCString(each.language);
            info.name = MallocCString(each.name);
            arr.head[idx] = info;
            idx++;
        }
    } else {
        arr.head = nullptr;
        arr.size = 0;
    }

    return HOLIDAY_OPT_SUCCESS;
}

static void FreeHolidayLocalNameArr(HolidayLocalNameArr &arr)
{
    for (int64_t index = 0; index < arr.size; index++) {
        free(arr.head[index].language);
        free(arr.head[index].name);
    }
}

static int32_t HolidayInfo2Cj(const std::vector<HolidayInfoItem>& items, HolidayInfoItemArr &arr)
{
    int64_t size = static_cast<int64_t>(items.size());
    if (size <= 0) {
        arr.head = nullptr;
        arr.size = 0;
        return HOLIDAY_OPT_SUCCESS;
    }
    arr.head = static_cast<CHolidayInfoItem*>(malloc(sizeof(CHolidayInfoItem) * size));
    if (arr.head == nullptr) {
        return HOLIDAY_OPT_FAILED;
    }
    uint32_t idx = 0;
    for (const auto& each : items) {
        CHolidayInfoItem info;
        info.baseName = MallocCString(each.baseName);
        info.day = each.day;
        info.month = each.month;
        info.year = each.year;

        if (HolidayLocalNames2Cj(each.localNames, info.localNames) != HOLIDAY_OPT_SUCCESS) {
            for (uint32_t index = 0; index < idx; index++) {
                free(arr.head[index].baseName);
                FreeHolidayLocalNameArr(arr.head[index].localNames);
                free(arr.head[index].localNames.head);
            }
            free(info.baseName);
            free(arr.head);
            return HOLIDAY_OPT_FAILED;
        }

        arr.head[idx] = info;
        idx++;
    }
    arr.size = idx;
    return HOLIDAY_OPT_SUCCESS;
}

HolidayInfoItemArr FfiOHOSHolidayManagerGetHolidayInfoItemArray(int64_t id, int32_t year)
{
    auto instance = FFIData::GetData<CHolidayManager>(id);
    std::vector<HolidayInfoItem> result;
    HolidayInfoItemArr infos = { 0, nullptr };
    if (year == -1) {
        result = instance->getHolidayInfoItemArray();
    } else {
        result = instance->getHolidayInfoItemArray(year);
    }

    HolidayInfo2Cj(result, infos);
    return infos;
}
}
}  // namespace I18n
}  // namespace Global
}  // namespace OHOS
