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

#include "i18n_unicode_ffi.h"
#include <cstdlib>
#include "character.h"
 
namespace OHOS {
namespace Global {
namespace I18n {
extern "C"
{
    char* FfiI18nUnicodeGetType(const char* text)
    {
        return MallocCString(GetType(text));
    }

    bool FfiI18nUnicodeIsUpperCase(const char* text)
    {
        return IsUpperCase(text);
    }

    bool FfiI18nUnicodeIsLowerCase(const char* text)
    {
        return IsLowerCase(text);
    }

    bool FfiI18nUnicodeIsLetter(const char* text)
    {
        return IsLetter(text);
    }

    bool FfiI18nUnicodeIsIdeograph(const char* text)
    {
        return IsIdeoGraphic(text);
    }

    bool FfiI18nUnicodeIsRTL(const char* text)
    {
        return IsRTLCharacter(text);
    }

    bool FfiI18nUnicodeIsWhitespace(const char* text)
    {
        return IsWhiteSpace(text);
    }

    bool FfiI18nUnicodeIsSpaceChar(const char* text)
    {
        return IsSpaceChar(text);
    }

    bool FfiI18nUnicodeIsDigit(const char* text)
    {
        return IsDigit(text);
    }
}
}  // namespace I18n
}  // namespace Global
}  // namespace OHOS