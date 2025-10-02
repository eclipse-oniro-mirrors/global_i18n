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
#ifndef I18N_UNICODE_FFI_H
#define I18N_UNICODE_FFI_H

#include <cstdint>
#include "ffi_remote_data.h"
#include "i18n_ffi.h"
#include "i18n_struct.h"

extern "C" {
    FFI_EXPORT char* FfiI18nUnicodeGetType(char* text);
    FFI_EXPORT bool FfiI18nUnicodeIsUpperCase(char* text);
    FFI_EXPORT bool FfiI18nUnicodeIsLowerCase(char* text);
    FFI_EXPORT bool FfiI18nUnicodeIsLetter(char* text);
    FFI_EXPORT bool FfiI18nUnicodeIsIdeograph(char* text);
    FFI_EXPORT bool FfiI18nUnicodeIsRTL(char* text);
    FFI_EXPORT bool FfiI18nUnicodeIsWhitespace(char* text);
    FFI_EXPORT bool FfiI18nUnicodeIsSpaceChar(char* text);
    FFI_EXPORT bool FfiI18nUnicodeIsDigit(char* text);
}

#endif