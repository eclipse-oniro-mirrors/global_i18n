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
#ifndef OHOS_GLOBAL_I18N_SIMPLE_NUMBER_FORMAT_H
#define OHOS_GLOBAL_I18N_SIMPLE_NUMBER_FORMAT_H

#include "i18n_types.h"
#include "locale_info.h"
#include "unicode/numberformatter.h"

namespace OHOS {
namespace Global {
namespace I18n {
class SimpleNumberFormat {
public:
    SimpleNumberFormat(const std::string& skeleton, std::shared_ptr<LocaleInfo> localeInfo, I18nErrorCode& errCode);
    SimpleNumberFormat(const std::string& skeleton, const std::string& localeTag, I18nErrorCode& errCode);
    ~SimpleNumberFormat();
    std::string Format(double number);
    icu::number::FormattedNumber FormatToFormattedNumber(double number);

private:
    static bool Init();
    void InitSimpleNumberFormat(const std::string& skeleton, const std::string& locale, I18nErrorCode& errCode);
    static bool icuInitialized;
    icu::number::LocalizedNumberFormatter simpleNumberFormat;
    bool initSuccess = false;
};
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif