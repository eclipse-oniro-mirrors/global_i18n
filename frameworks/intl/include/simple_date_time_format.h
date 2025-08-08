/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#ifndef OHOS_GLOBAL_I18N_SIMPLE_DATE_TIME_FORMAT_H
#define OHOS_GLOBAL_I18N_SIMPLE_DATE_TIME_FORMAT_H

#include <vector>
#include "i18n_types.h"
#include "locale_info.h"
#include "unicode/locid.h"
#include "unicode/smpdtfmt.h"

namespace OHOS {
namespace Global {
namespace I18n {
class SimpleDateTimeFormat {
public:
    SimpleDateTimeFormat(const std::string& skeletonOrPattern, std::shared_ptr<LocaleInfo> localeInfo,
        bool isBestPattern, I18nErrorCode& errCode);
    std::string Format(int64_t milliseconds);

private:
    I18nErrorCode ParsePattern(const std::string& pattern);
    I18nErrorCode ParseSkeleton(const std::string& skeleton, const icu::Locale& locale);
    I18nErrorCode InitFormatters(const icu::Locale& locale, bool isBestPattern);
    static bool IsValidPattern(const std::string& pattern);
    static bool Init();
    static bool icuInitialized;
    bool initSuccess = false;
    std::vector<std::shared_ptr<icu::SimpleDateFormat>> formatters;
    std::vector<std::string> patterns;
    std::vector<bool> isPatterns;
    static const char SEPARATOR;
    static const std::string VALID_PATTERN;
};
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif