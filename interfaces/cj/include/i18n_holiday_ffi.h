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
#ifndef INTERFACES_CJ_INCLUDE_I18N_HOLIDAY_FFI_H_
#define INTERFACES_CJ_INCLUDE_I18N_HOLIDAY_FFI_H_

#include <string>
#include "i18n_calendar_ffi.h"

#define FFI_EXPORT __attribute__((visibility("default")))

extern "C" {
    FFI_EXPORT int64_t FfiOHOSHolidayManagerImplConstructor(char* icsPath);
    FFI_EXPORT bool FfiOHOSHolidayManagerIsHoliday(int64_t id, CDate date);
    FFI_EXPORT HolidayInfoItemArr FfiOHOSHolidayManagerGetHolidayInfoItemArray(
        int64_t id, int32_t year);
}

#endif  // INTERFACES_CJ_INCLUDE_I18N_HOLIDAY_FFI_H_
