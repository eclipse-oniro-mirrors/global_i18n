/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef OHOS_GLOBAL_I18N_I18N_BREAK_ITERATOR_MOCK_H
#define OHOS_GLOBAL_I18N_I18N_BREAK_ITERATOR_MOCK_H

#include <gmock/gmock.h>
#include "i18n_break_iterator.h"

namespace OHOS {
namespace Global {
namespace I18n {
class I18nBreakIteratorMock : public I18nBreakIterator {
public:
    I18nBreakIteratorMock(std::string localeTag) : I18nBreakIterator(localeTag) {
    };
    ~I18nBreakIteratorMock() override = default;
    MOCK_METHOD0(GetBreakIterator, icu::BreakIterator*());
};
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif