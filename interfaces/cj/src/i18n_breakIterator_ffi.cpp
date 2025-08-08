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

#include "i18n_breakIterator_ffi.h"
#include "i18n_breakIterator_impl.h"
#include "i18n_ffi.h"
#include "i18n_hilog.h"

namespace OHOS {
namespace Global {
namespace I18n {
using namespace OHOS::FFI;
constexpr int32_t OFF_BOUND = -1;
extern "C" {
void FfiOHOSBreakIteratorSetLineBreakText(int64_t id, const char* text)
{
    auto instance = FFIData::GetData<CBreakIterator>(id);
    if (!instance) {
        HILOG_ERROR_I18N("The CBreakIterator instance is nullptr");
        return;
    }
    instance->SetLineBreakText(text);
}

char* FfiOHOSBreakIteratorGetLineBreakText(int64_t id)
{
    auto instance = FFIData::GetData<CBreakIterator>(id);
    if (!instance) {
        HILOG_ERROR_I18N("The CBreakIterator instance is nullptr");
        return nullptr;
    }
    return MallocCString(instance->GetLineBreakText());
}

int32_t FfiOHOSBreakIteratorCurrent(int64_t id)
{
    auto instance = FFIData::GetData<CBreakIterator>(id);
    if (!instance) {
        HILOG_ERROR_I18N("The CBreakIterator instance is nullptr");
        return OFF_BOUND;
    }
    return instance->current();
}

int32_t FfiOHOSBreakIteratorFirst(int64_t id)
{
    auto instance = FFIData::GetData<CBreakIterator>(id);
    if (!instance) {
        HILOG_ERROR_I18N("The CBreakIterator instance is nullptr");
        return OFF_BOUND;
    }
    return instance->first();
}

int32_t FfiOHOSBreakIteratorFollowing(int64_t id, int32_t offset)
{
    auto instance = FFIData::GetData<CBreakIterator>(id);
    if (!instance) {
        HILOG_ERROR_I18N("The CBreakIterator instance is nullptr");
        return OFF_BOUND;
    }
    return instance->following(offset);
}

int32_t FfiOHOSBreakIteratorLast(int64_t id)
{
    auto instance = FFIData::GetData<CBreakIterator>(id);
    if (!instance) {
        HILOG_ERROR_I18N("The CBreakIterator instance is nullptr");
        return OFF_BOUND;
    }
    return instance->last();
}

int32_t FfiOHOSBreakIteratorNext(int64_t id, int32_t number)
{
    auto instance = FFIData::GetData<CBreakIterator>(id);
    if (!instance) {
        HILOG_ERROR_I18N("The CBreakIterator instance is nullptr");
        return OFF_BOUND;
    }
    return instance->next(number);
}

int32_t FfiOHOSBreakIteratorPrevious(int64_t id)
{
    auto instance = FFIData::GetData<CBreakIterator>(id);
    if (!instance) {
        HILOG_ERROR_I18N("The CBreakIterator instance is nullptr");
        return OFF_BOUND;
    }
    return instance->previous();
}

bool FfiOHOSBreakIteratorIsBoundary(int64_t id, int32_t offset)
{
    auto instance = FFIData::GetData<CBreakIterator>(id);
    if (!instance) {
        HILOG_ERROR_I18N("The CBreakIterator instance is nullptr");
        return false;
    }
    return instance->IsBoundary(offset);
}

int64_t FfiOHOSGetLineInstance(const char* locale)
{
    std::string localeStr(locale);
    auto instance = FFIData::Create<CBreakIterator>(localeStr);
    if (instance == nullptr) {
        HILOG_ERROR_I18N("Create CBreakIterator fail");
        return -1;
    }
    return instance->GetID();
}
}
}  // namespace I18n
}  // namespace Global
}  // namespace OHOS
