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
#ifndef INTERFACES_CJ_INCLUDE_I18N_BREAKITERATOR_FFI_H_
#define INTERFACES_CJ_INCLUDE_I18N_BREAKITERATOR_FFI_H_

#include <cstdint>

#define FFI_EXPORT __attribute__((visibility("default")))

extern "C" {
    FFI_EXPORT void FfiOHOSBreakIteratorSetLineBreakText(int64_t id, const char* text);
    FFI_EXPORT char* FfiOHOSBreakIteratorGetLineBreakText(int64_t id);
    FFI_EXPORT int32_t FfiOHOSCalendarBreakIteratorCurrent(int64_t id);
    FFI_EXPORT int32_t FfiOHOSCalendarBreakIteratorFirst(int64_t id);
    FFI_EXPORT int32_t FfiOHOSCalendarBreakIteratorLast(int64_t id);
    FFI_EXPORT int32_t FfiOHOSCalendarBreakIteratorNext(int64_t id, int32_t index);
    FFI_EXPORT int32_t FfiOHOSCalendarBreakIteratorPrevious(int64_t id);
    FFI_EXPORT int32_t FfiOHOSCalendarBreakIteratorFollowing(int64_t id, int32_t offset);
    FFI_EXPORT bool FfiOHOSCalendarBreakIteratorIsBoundary(int64_t id);
    FFI_EXPORT int64_t FfiOHOSGetLineInstance(const char* locale);
}

#endif  // INTERFACES_CJ_INCLUDE_I18N_BREAKITERATOR_FFI_H_
