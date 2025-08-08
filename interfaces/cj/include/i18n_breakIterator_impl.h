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

#ifndef INTERFACES_CJ_INCLUDE_I18N_BREAKITERATOR_IMPL_H_
#define INTERFACES_CJ_INCLUDE_I18N_BREAKITERATOR_IMPL_H_

#include <string>
#include <vector>
#include "i18n_break_iterator.h"
#include "ffi_remote_data.h"

namespace OHOS {
namespace Global {
namespace I18n {
class CBreakIterator : public OHOS::FFI::FFIData {
    DECL_TYPE(CBreakIterator, FFIData)
public:
    explicit CBreakIterator(std::string localeTag);
    ~CBreakIterator() = default;
    void SetLineBreakText(const char* text);
    std::string GetLineBreakText();
    int32_t current();
    int32_t first();
    int32_t last();
    int32_t next(int32_t number);
    int32_t previous();
    int32_t following(int32_t offset);
    bool IsBoundary(int32_t offset);
private:
    std::unique_ptr<I18nBreakIterator> brkiter_ = nullptr;
};

}  // namespace I18n
}  // namespace Global
}  // namespace OHOS

#endif  // INTERFACES_CJ_INCLUDE_I18N_BREAKITERATOR_IMPL_H_
