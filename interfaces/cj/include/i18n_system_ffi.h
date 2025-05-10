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
#ifndef I18N_SYSTEM_FFI_H
#define I18N_SYSTEM_FFI_H

#include <cstdint>
#include "ffi_remote_data.h"
#include "i18n_struct.h"

#define FFI_EXPORT __attribute__((visibility("default")))

extern "C" {
    FFI_EXPORT char* FfiI18nSystemGetAppPreferredLanguage();
    FFI_EXPORT char* FfiI18nSystemGetSystemLanguage();
    FFI_EXPORT char* FfiI18nSystemGetSystemRegion();
    FFI_EXPORT bool FfiI18nSystemIsSuggested(char* language, char* region, int32_t parameterStatus);
    FFI_EXPORT CArrStr FfiI18nSystemGetSystemCountries(char* language);
    FFI_EXPORT char* FfiI18nSystemGetDisplayCountry(char* country, char* locale, bool sentenceCase, int32_t* errcode);
    FFI_EXPORT bool FfiI18nSystemGetUsingLocalDigit();
    FFI_EXPORT int32_t FfiI18nSystemSetAppPreferredLanguage(char* language);
    FFI_EXPORT char* FfiI18nSystemGetFirstPreferredLanguage();
    FFI_EXPORT CArrStr FfiI18nSystemGetPreferredLanguageList();
    FFI_EXPORT bool FfiI18nSystemIs24HourClock();
    FFI_EXPORT CArrStr FfiI18nSystemGetSystemLanguages();
    FFI_EXPORT char* FfiI18nSystemGetDisplayLanguage(char* language, char* locale, bool sentenceCase, int32_t* errCode);
    FFI_EXPORT char* FfiI18nSystemGetSystemLocale();
}

#endif