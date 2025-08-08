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
#ifndef I18N_TIMEZONE_FFI_H
#define I18N_TIMEZONE_FFI_H

#include <cstdint>
#include "ffi_remote_data.h"
#include "i18n_struct.h"

#define FFI_EXPORT __attribute__((visibility("default")))

extern "C" {
    FFI_EXPORT int64_t FfiI18nTimezoneConstructor(char* id, bool isZoneId);
    FFI_EXPORT CArrStr FfiI18nTimezoneGetTimezonesByLocation(double longitude, double latitude);
    FFI_EXPORT char* FfiI18nTimezoneGetCityDisplayName(char* cityID, char* locale);
    FFI_EXPORT CArrStr FfiI18nTimezoneGetAvailableZoneCityIDs();
    FFI_EXPORT CArrStr FfiI18nTimezoneGetAvailableIDs();
    FFI_EXPORT char* FfiI18nTimezoneGetDisplayName(int64_t remoteDataID,
        char* locale, bool isDST, int32_t parameterStatus);
    FFI_EXPORT int32_t FfiI18nTimezoneGetOffset(int64_t remoteDataID, double date, int32_t parameterStatus);
    FFI_EXPORT int32_t FfiI18nTimezoneGetRawOffset(int64_t remoteDataID);
    FFI_EXPORT char* FfiI18nTimezoneGetID(int64_t remoteDataID);
}

#endif