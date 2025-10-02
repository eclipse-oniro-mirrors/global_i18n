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
#ifndef INTERFACES_CJ_INCLUDE_I18N_UTIL_FFI_H_
#define INTERFACES_CJ_INCLUDE_I18N_UTIL_FFI_H_

#include <cstdint>
#include "i18n_struct.h"

#define FFI_EXPORT __attribute__((visibility("default")))

extern "C" {
    FFI_EXPORT char* FfiI18nUtilUnitConvert(UnitInfo fromUnit, UnitInfo toUnit,
        double value, char* locale, char* style);
    FFI_EXPORT char* FfiI18nUtilGetDateOrder(char* locale);
    FFI_EXPORT char* FfiI18nUtilGetTimePeriodName(int32_t hour, char* locale, int32_t* errcode);
    FFI_EXPORT char* FfiI18nUtilGetThreeLetterLanguage(char* locale, int32_t* errcode);
    FFI_EXPORT char* FfiI18nUtilGetThreeLetterRegion(char* locale, int32_t* errcode);
    FFI_EXPORT char* FfiI18nUtilGetBestMatchLocale(char* locale, CArrStr localeList, int32_t* errcode);
}

#endif  // INTERFACES_CJ_INCLUDE_I18N_UTIL_FFI_H_
