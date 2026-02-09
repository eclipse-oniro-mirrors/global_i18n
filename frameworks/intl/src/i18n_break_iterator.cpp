/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#include "i18n_break_iterator.h"

#include "unicode/brkiter.h"
#include "string"
#include "unicode/utypes.h"

namespace OHOS {
namespace Global {
namespace I18n {
I18nBreakIterator::I18nBreakIterator(std::string localeTag)
{
    UErrorCode status = U_ZERO_ERROR;
    iter = icu::BreakIterator::createLineInstance(localeTag.c_str(), status);
    if (!U_SUCCESS(status)) {
        if (iter != nullptr) {
            delete iter;
        }
        iter = nullptr;
    }
}

I18nBreakIterator::~I18nBreakIterator()
{
    if (iter != nullptr) {
        delete iter;
    }
}

int32_t I18nBreakIterator::Current()
{
    icu::BreakIterator* breakIter = GetBreakIterator();
    if (breakIter != nullptr) {
        return breakIter->current();
    }
    return OFF_BOUND;
}

int32_t I18nBreakIterator::First()
{
    icu::BreakIterator* breakIter = GetBreakIterator();
    if (breakIter != nullptr) {
        return breakIter->first();
    }
    return OFF_BOUND;
}

int32_t I18nBreakIterator::Last()
{
    icu::BreakIterator* breakIter = GetBreakIterator();
    if (breakIter != nullptr) {
        return breakIter->last();
    }
    return OFF_BOUND;
}

int32_t I18nBreakIterator::Previous()
{
    icu::BreakIterator* breakIter = GetBreakIterator();
    if (breakIter != nullptr) {
        return breakIter->previous();
    }
    return OFF_BOUND;
}

int32_t I18nBreakIterator::Next(int32_t number)
{
    icu::BreakIterator* breakIter = GetBreakIterator();
    if (breakIter != nullptr) {
        return breakIter->next(number);
    }
    return OFF_BOUND;
}

int32_t I18nBreakIterator::Next()
{
    icu::BreakIterator* breakIter = GetBreakIterator();
    if (breakIter != nullptr) {
        return breakIter->next();
    }
    return OFF_BOUND;
}

int32_t I18nBreakIterator::Following(int32_t offset)
{
    icu::BreakIterator* breakIter = GetBreakIterator();
    if (breakIter != nullptr) {
        return breakIter->following(offset);
    }
    return OFF_BOUND;
}

void I18nBreakIterator::SetText(const char *text)
{
    icu::BreakIterator* breakIter = GetBreakIterator();
    if (breakIter != nullptr) {
        ftext = text;
        breakIter->setText(ftext);
    }
}

void I18nBreakIterator::GetText(std::string &str)
{
    icu::BreakIterator* breakIter = GetBreakIterator();
    if (breakIter != nullptr) {
        ftext.toUTF8String(str);
    }
}

bool I18nBreakIterator::IsBoundary(int32_t offset)
{
    icu::BreakIterator* breakIter = GetBreakIterator();
    if (breakIter != nullptr) {
        return breakIter->isBoundary(offset);
    }
    return false;
}

icu::BreakIterator* I18nBreakIterator::GetBreakIterator()
{
    return this->iter;
}
} // namespace I18n
} // namespace Global
} // namespace OHOS