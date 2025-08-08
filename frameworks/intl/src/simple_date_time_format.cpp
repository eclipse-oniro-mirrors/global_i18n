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
#include "unicode/datefmt.h"
#include "utils.h"
#include "simple_date_time_format.h"

namespace OHOS {
namespace Global {
namespace I18n {
const char SimpleDateTimeFormat::SEPARATOR = '\'';
const std::string SimpleDateTimeFormat::VALID_PATTERN = "aAbBcCdDeEFgGhHjJkKLmMOqQrsSuUvVwWxXyYzZ";
bool SimpleDateTimeFormat::icuInitialized = SimpleDateTimeFormat::Init();

SimpleDateTimeFormat::SimpleDateTimeFormat(const std::string& skeletonOrPattern, std::shared_ptr<LocaleInfo> localeInfo,
    bool isBestPattern, I18nErrorCode& errCode)
{
    if (skeletonOrPattern.empty()) {
        HILOG_ERROR_I18N("SimpleDateTimeFormat: skeleton or pattern is empty.");
        errCode = isBestPattern ? I18nErrorCode::INVALID_DATE_TIME_FORMAT_PATTERN :
            I18nErrorCode::INVALID_DATE_TIME_FORMAT_SKELETON;
        return;
    }

    std::string locale;
    if (localeInfo == nullptr) {
        locale = LocaleConfig::GetSystemLocale();
    } else {
        locale = localeInfo->GetBaseName();
    }
    if (locale.empty() || !LocaleConfig::IsValidTag(locale)) {
        HILOG_ERROR_I18N("SimpleDateTimeFormat: locale %{public}s is invalid.", locale.c_str());
        errCode = I18nErrorCode::INVALID_LOCALE_TAG;
        return;
    }
    UErrorCode status = U_ZERO_ERROR;
    icu::Locale icuLocale = icu::Locale::forLanguageTag(icu::StringPiece(locale), status);
    if (U_FAILURE(status)) {
        HILOG_ERROR_I18N("SimpleDateTimeFormat: create icu::Locale failed.");
        errCode = I18nErrorCode::FAILED;
        return;
    }

    errCode = isBestPattern ? ParsePattern(skeletonOrPattern) : ParseSkeleton(skeletonOrPattern, icuLocale);
    if (errCode != I18nErrorCode::SUCCESS) {
        HILOG_ERROR_I18N("SimpleDateTimeFormat: Parse pattern or skeleton failed.");
        return;
    }
    errCode = InitFormatters(icuLocale, isBestPattern);
    if (errCode != I18nErrorCode::SUCCESS) {
        HILOG_ERROR_I18N("SimpleDateTimeFormat: Init formatters failed.");
        return;
    }
    initSuccess = true;
}

std::string SimpleDateTimeFormat::Format(int64_t milliseconds)
{
    if (!initSuccess) {
        HILOG_ERROR_I18N("SimpleDateTimeFormat::Format: init failed.");
        return PseudoLocalizationProcessor("");
    }
    std::string systemTimeZone = LocaleConfig::GetSystemTimezone();
    auto timeZone = std::unique_ptr<icu::TimeZone>(icu::TimeZone::createTimeZone(systemTimeZone.c_str()));
    UErrorCode status = U_ZERO_ERROR;
    UDate date = static_cast<UDate>(milliseconds);
    icu::UnicodeString formatResult;
    for (size_t pos = 0; pos < formatters.size(); pos++) {
        std::string pattern = patterns[pos];
        if (!isPatterns[pos]) {
            formatResult.append(pattern.c_str());
            continue;
        }
        auto formatter = formatters[pos];
        if (formatter == nullptr) {
            HILOG_ERROR_I18N("SimpleDateTimeFormat::Format: formatter is nullptr.");
            return PseudoLocalizationProcessor("");
        }
        if (timeZone != nullptr) {
            formatter->setTimeZone(*timeZone);
        }
        formatter->format(date, formatResult, status);
        if (U_FAILURE(status)) {
            HILOG_ERROR_I18N("SimpleDateTimeFormat::Format: simpleDateFormat format failed.");
            return PseudoLocalizationProcessor("");
        }
    }
    std::string result;
    formatResult.toUTF8String(result);
    return PseudoLocalizationProcessor(result);
}

I18nErrorCode SimpleDateTimeFormat::ParsePattern(const std::string& pattern)
{
    bool isPattern = true;
    std::string content;
    int32_t numberOfSeparator = 0;
    size_t pos = 0;
    size_t len = pattern.length();
    while (pos < len) {
        char c = pattern[pos];
        // ' represents the separator
        if (c == SEPARATOR) {
            pos++;
            // '' represents the character '
            if (pos < len && pattern[pos] == SEPARATOR) {
                content.push_back(SEPARATOR);
                pos++;
                continue;
            }
            numberOfSeparator++;
            if (!content.empty()) {
                patterns.push_back(content);
                isPatterns.push_back(isPattern);
                content.clear();
            }
            isPattern = !isPattern;
        } else {
            content.push_back(c);
            pos++;
        }
    }
    if (!content.empty()) {
        patterns.push_back(content);
        isPatterns.push_back(isPattern);
    }
    int32_t validNumber = 2;
    if (numberOfSeparator % validNumber == 1) {
        HILOG_ERROR_I18N("SimpleDateTimeFormat::ParsePattern: Number of separator is invalid.");
        return I18nErrorCode::INVALID_DATE_TIME_FORMAT_PATTERN;
    }
    return I18nErrorCode::SUCCESS;
}

I18nErrorCode SimpleDateTimeFormat::ParseSkeleton(const std::string& skeleton, const icu::Locale& locale)
{
    if (!IsValidPattern(skeleton)) {
        return I18nErrorCode::INVALID_DATE_TIME_FORMAT_SKELETON;
    }
    UErrorCode status = U_ZERO_ERROR;
    icu::UnicodeString bestPattern = icu::DateFormat::getBestPattern(locale, skeleton.c_str(), status);
    if (U_FAILURE(status)) {
        HILOG_ERROR_I18N("ParseSkeleton: Get best pattern failed.");
        return I18nErrorCode::FAILED;
    }
    std::string pattern;
    bestPattern.toUTF8String(pattern);
    patterns.push_back(pattern);
    isPatterns.push_back(true);
    return I18nErrorCode::SUCCESS;
}

I18nErrorCode SimpleDateTimeFormat::InitFormatters(const icu::Locale& locale, bool isBestPattern)
{
    if (patterns.size() != isPatterns.size()) {
        return I18nErrorCode::FAILED;
    }
    size_t sizeOfPatterns = patterns.size();
    formatters.assign(sizeOfPatterns, nullptr);
    UErrorCode status = U_ZERO_ERROR;
    for (size_t pos = 0; pos < sizeOfPatterns; pos++) {
        if (!isPatterns[pos]) {
            continue;
        }
        std::string pattern = patterns[pos];
        if (isBestPattern && !IsValidPattern(pattern)) {
            return I18nErrorCode::INVALID_DATE_TIME_FORMAT_PATTERN;
        }
        formatters[pos] = std::make_shared<icu::SimpleDateFormat>(pattern.c_str(), locale, status);
        if (U_FAILURE(status)) {
            HILOG_ERROR_I18N("InitFormatters: Pattern %{public}s create SimpleDateFormat failed.",
                pattern.c_str());
            return I18nErrorCode::FAILED;
        }
    }
    return I18nErrorCode::SUCCESS;
}

bool SimpleDateTimeFormat::IsValidPattern(const std::string& pattern)
{
    if (pattern.empty()) {
        HILOG_ERROR_I18N("SimpleDateTimeFormat::IsValidPattern: Pattern is empty.");
        return false;
    }
    for (const char c : pattern) {
        if (VALID_PATTERN.find(c) == std::string::npos) {
            HILOG_ERROR_I18N("SimpleDateTimeFormat::IsValidPattern: %{public}s is an invalid pattern.",
                pattern.c_str());
            return false;
        }
    }
    return true;
}

bool SimpleDateTimeFormat::Init()
{
    SetHwIcuDirectory();
    return true;
}
} // namespace I18n
} // namespace Global
} // namespace OHOS
