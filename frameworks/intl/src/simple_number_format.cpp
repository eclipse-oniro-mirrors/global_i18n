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

#include "i18n_hilog.h"
#include "locale_config.h"
#include "ohos/init_data.h"
#include "unicode/locid.h"
#include "utils.h"
#include "simple_number_format.h"

namespace OHOS {
namespace Global {
namespace I18n {
bool SimpleNumberFormat::icuInitialized = SimpleNumberFormat::Init();

SimpleNumberFormat::SimpleNumberFormat(const std::string& skeleton, std::shared_ptr<LocaleInfo> localeInfo,
    I18nErrorCode& errCode)
{
    if (skeleton.empty()) {
        HILOG_ERROR_I18N("SimpleNumberFormat: Skeleton is empty.");
        errCode = I18nErrorCode::INVALID_NUMBER_FORMAT_SKELETON;
        return;
    }
    std::string locale;
    if (localeInfo == nullptr) {
        locale = LocaleConfig::GetSystemLocale();
    } else {
        locale = localeInfo->GetBaseName();
    }
    if (locale.empty() || !LocaleConfig::IsValidTag(locale)) {
        HILOG_ERROR_I18N("SimpleNumberFormat: Locale %{public}s is invalid.", locale.c_str());
        errCode = I18nErrorCode::INVALID_LOCALE_TAG;
        return;
    }
    UErrorCode status = U_ZERO_ERROR;
    icu::Locale icuLocale = icu::Locale::forLanguageTag(icu::StringPiece(locale), status);
    if (U_FAILURE(status)) {
        HILOG_ERROR_I18N("SimpleNumberFormat: Create icu::Locale failed.");
        errCode = I18nErrorCode::FAILED;
        return;
    }

    icu::number::UnlocalizedNumberFormatter unlocalizedNumberFormat =
        icu::number::NumberFormatter::forSkeleton(skeleton.c_str(), status);
    if (U_FAILURE(status)) {
        HILOG_ERROR_I18N("SimpleNumberFormat: Create UnlocalizedNumberFormatter failed.");
        if (status == U_NUMBER_SKELETON_SYNTAX_ERROR) {
            HILOG_ERROR_I18N("SimpleNumberFormat: Invalid skeleton %{public}s.", skeleton.c_str());
            errCode = I18nErrorCode::INVALID_NUMBER_FORMAT_SKELETON;
            return;
        }
        errCode = I18nErrorCode::FAILED;
        return;
    }
    simpleNumberFormat = unlocalizedNumberFormat.locale(icuLocale);
    initSuccess = true;
}

SimpleNumberFormat::~SimpleNumberFormat() {}

std::string SimpleNumberFormat::Format(double number)
{
    icu::number::FormattedNumber formattedNumber = FormatToFormattedNumber(number);
    UErrorCode status = U_ZERO_ERROR;
    icu::UnicodeString formatResult = formattedNumber.toString(status);
    if (U_FAILURE(status)) {
        HILOG_ERROR_I18N("SimpleNumberFormat::Format: FormattedNumber convert to string failed.");
        return PseudoLocalizationProcessor("");
    }
    std::string result;
    formatResult.toUTF8String(result);
    return PseudoLocalizationProcessor(result);
}

bool SimpleNumberFormat::Init()
{
    SetHwIcuDirectory();
    return true;
}

icu::number::FormattedNumber SimpleNumberFormat::FormatToFormattedNumber(double number)
{
    if (!initSuccess) {
        HILOG_ERROR_I18N("SimpleNumberFormat::FormatToFormattedNumber: Init failed.");
        return {};
    }
    UErrorCode status = U_ZERO_ERROR;
    icu::number::FormattedNumber formattedNumber = simpleNumberFormat.formatDouble(number, status);
    if (U_FAILURE(status)) {
        HILOG_ERROR_I18N("SimpleNumberFormat::FormatToFormattedNumber: Format double failed.");
        return {};
    }
    return formattedNumber;
}
} // namespace I18n
} // namespace Global
} // namespace OHOS
