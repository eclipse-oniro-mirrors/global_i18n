/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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
#include "displaynames.h"
#include "i18n_hilog.h"
#include "locale_config.h"
#include "locale_helper.h"
#include "taboo_utils.h"
#include "utils.h"

namespace OHOS {
namespace Global {
namespace I18n {
const std::unordered_map<std::string, UDisplayContext> DisplayNames::ICU_STYLE_OPTIONS = {
    { "long", UDISPCTX_LENGTH_FULL },
    { "short", UDISPCTX_LENGTH_SHORT },
    { "narrow", UDISPCTX_LENGTH_SHORT }
};

const std::unordered_map<std::string, UDisplayContext> DisplayNames::ICU_LANGUAGE_DISPLAY_OPTIONS = {
    { "dialect", UDISPCTX_DIALECT_NAMES },
    { "standard", UDISPCTX_STANDARD_NAMES }
};

const std::unordered_map<std::string, UDateTimePatternField> DisplayNames::ICU_DATE_PATTERN_FIELD = {
    { "day", UDATPG_DAY_FIELD },
    { "dayPeriod", UDATPG_DAYPERIOD_FIELD },
    { "era", UDATPG_ERA_FIELD },
    { "hour", UDATPG_HOUR_FIELD },
    { "minute", UDATPG_MINUTE_FIELD },
    { "month", UDATPG_MONTH_FIELD },
    { "quarter", UDATPG_QUARTER_FIELD },
    { "second", UDATPG_SECOND_FIELD },
    { "timeZoneName", UDATPG_ZONE_FIELD },
    { "weekOfYear", UDATPG_WEEK_OF_YEAR_FIELD },
    { "weekday", UDATPG_WEEKDAY_FIELD },
    { "year", UDATPG_YEAR_FIELD },
};

const std::unordered_map<std::string, UDateTimePGDisplayWidth> DisplayNames::ICU_DATE_DISPLAY_WIDTH = {
    { "long", UDATPG_WIDE },
    { "short", UDATPG_ABBREVIATED },
    { "narrow", UDATPG_NARROW }
};

std::vector<std::string> DisplayNames::SupportedLocalesOf(const std::vector<std::string> &requestLocales,
                                                          const std::map<std::string, std::string> &configs,
                                                          I18nErrorCode &status)
{
    std::vector<std::string> undefined = {};
    auto requestedLocales = LocaleHelper::CanonicalizeLocaleList(requestLocales, status);
    if (status != I18nErrorCode::SUCCESS) {
        return undefined;
    }

    std::string localeMatcher = LocaleHelper::ParseOption(configs, "localeMatcher", "best fit", true, status);
    if (status != I18nErrorCode::SUCCESS) {
        return undefined;
    }
    std::set<std::string> availableLocales = LocaleInfo::GetValidLocales();
    return LocaleHelper::LookupSupportedLocales(availableLocales, requestedLocales);
}

DisplayNames::DisplayNames(const std::vector<std::string> &localeTags,
                           const std::map<std::string, std::string> &options)
{
    status = I18nErrorCode::SUCCESS;
    auto requestedLocales = LocaleHelper::CanonicalizeLocaleList(localeTags, status);
    if (status != I18nErrorCode::SUCCESS) {
        return;
    }

    ParseAllOptions(options);
    if (status != I18nErrorCode::SUCCESS) {
        return;
    }
    for (size_t i = 0; i < requestedLocales.size(); i++) {
        std::string curLocale = requestedLocales[i];
        UErrorCode icuStatus = U_ZERO_ERROR;
        locale = icu::Locale::forLanguageTag(icu::StringPiece(curLocale), icuStatus);
        if (U_FAILURE(icuStatus)) {
            continue;
        }
        if (LocaleInfo::allValidLocales.count(locale.getLanguage()) > 0) {
            InitDisplayNames(curLocale, options);
            if (status != I18nErrorCode::SUCCESS) {
                continue;
            }
            break;
        }
    }
    if (status != I18nErrorCode::SUCCESS || icuDisplaynames == nullptr) {
        InitDisplayNames("en-US", options);
    }
}

void DisplayNames::InitDisplayNames(const std::string &curLocale, const std::map<std::string, std::string> &options)
{
    status = I18nErrorCode::SUCCESS;
    localeInfo = std::make_unique<LocaleInfo>(curLocale, options);
    if (localeInfo == nullptr) {
        status = I18nErrorCode::FAILED;
        return;
    }
    if (!localeInfo->InitSuccess()) {
        status = I18nErrorCode::INVALID_PARAM;
        return;
    }
    locale = localeInfo->GetLocale();
    localeStr = localeInfo->GetBaseName();

    UDisplayContext icuStyle = UDISPCTX_LENGTH_FULL;
    auto styleIter = ICU_STYLE_OPTIONS.find(style);
    if (styleIter != ICU_STYLE_OPTIONS.end()) {
        icuStyle = styleIter->second;
    }

    UDisplayContext icuLanguageDisplay = UDISPCTX_DIALECT_NAMES;
    auto languageDisplayIter = ICU_LANGUAGE_DISPLAY_OPTIONS.find(languageDisplay);
    if (languageDisplayIter != ICU_LANGUAGE_DISPLAY_OPTIONS.end()) {
        icuLanguageDisplay = languageDisplayIter->second;
    }

    UDisplayContext displayContext[] = { icuStyle, icuLanguageDisplay, UDISPCTX_NO_SUBSTITUTE };
    // 3 indicates the array length
    icuDisplaynames.reset(icu::LocaleDisplayNames::createInstance(locale, displayContext, 3));
    if (icuDisplaynames == nullptr) {
        status = I18nErrorCode::FAILED;
        HILOG_ERROR_I18N("DisplayNames::InitDisplayNames: icu createInstance failed.");
        return;
    }
    createSuccess = true;
}

std::string DisplayNames::Display(const std::string &code)
{
    if (!createSuccess || icuDisplaynames == nullptr) {
        return "";
    }
    status = I18nErrorCode::SUCCESS;
    std::string displayName = "";
    if (type == "language") {
        displayName = DisplayLanguage(code);
    } else if (type == "region") {
        displayName = DisplayRegion(code);
    } else if (type == "script") {
        displayName = DisplayScript(code);
    } else if (type == "currency") {
        displayName = DisplayCurrency(code);
    } else if (type == "calendar") {
        displayName = DisplayCalendar(code);
    } else if (type == "dateTimeField") {
        displayName = DisplayDatetimefield(code);
    } else {
        HILOG_ERROR_I18N("DisplayNames::Display: this branch is unreachable");
    }

    if (displayName.empty() && status == I18nErrorCode::SUCCESS) {
        displayName = code;
    }
    return PseudoLocalizationProcessor(displayName);
}

void DisplayNames::ParseAllOptions(const std::map<std::string, std::string> &options)
{
    type = LocaleHelper::ParseOption(options, "type", "", false, status);
    localeMatcher = LocaleHelper::ParseOption(options, "localeMatcher", "best fit", true, status);
    style = LocaleHelper::ParseOption(options, "style", "long", true, status);
    fallback = LocaleHelper::ParseOption(options, "fallback", "code", true, status);
    languageDisplay = LocaleHelper::ParseOption(options, "languageDisplay", "dialect", true, status);
}

std::string DisplayNames::DisplayLanguage(const std::string &code)
{
    // Check the legality of the code
    UErrorCode icuStatus = U_ZERO_ERROR;
    icu::Locale codeLocale = icu::Locale(icu::Locale::forLanguageTag(code, icuStatus).getBaseName());
    if (U_FAILURE(icuStatus)) {
        HILOG_ERROR_I18N("DisplayNames::DisplayLanguage: unicode error, code %{public}s.", code.c_str());
        status = I18nErrorCode::INVALID_PARAM;
        return "";
    }
    std::string checked = codeLocale.toLanguageTag<std::string>(icuStatus);
    if (checked.size() == 0 || U_FAILURE(icuStatus) || !LocaleHelper::IsStructurallyValidLanguageTag(code)) {
        HILOG_ERROR_I18N("DisplayNames::DisplayLanguage: error code %{public}s.", code.c_str());
        status = I18nErrorCode::INVALID_PARAM;
        return "";
    }
    icu::UnicodeString displayname;
    icuDisplaynames->languageDisplayName(code.c_str(), displayname);
    std::string result = "";
    if (!displayname.isBogus()) {
        displayname.toUTF8String(result);
    }
    TabooUtils* tabooUtils = TabooUtils::GetInstance();
    if (tabooUtils == nullptr) {
        HILOG_ERROR_I18N("DisplayNames::DisplayLanguage: tabooUtils is nullptr.");
        return result;
    }
    return tabooUtils->ReplaceLanguageName(code, localeStr, result);
}

std::string DisplayNames::DisplayRegion(const std::string &code)
{
    if (!LocaleHelper::IsUnicodeRegionSubtag(code)) {
        HILOG_ERROR_I18N("DisplayNames::DisplayRegion: error code %{public}s.", code.c_str());
        status = I18nErrorCode::INVALID_PARAM;
        return "";
    }
    icu::UnicodeString displayname;
    icuDisplaynames->regionDisplayName(code.c_str(), displayname);
    std::string result = "";
    if (!displayname.isBogus()) {
        displayname.toUTF8String(result);
    }
    TabooUtils* tabooUtils = TabooUtils::GetInstance();
    if (tabooUtils == nullptr) {
        HILOG_ERROR_I18N("DisplayNames::DisplayRegion: tabooUtils is nullptr.");
        return result;
    }
    return tabooUtils->ReplaceCountryName(code, localeStr, result);
}

std::string DisplayNames::DisplayScript(const std::string &code)
{
    if (!LocaleHelper::IsUnicodeScriptSubtag(code)) {
        HILOG_ERROR_I18N("DisplayNames::DisplayScript: error code %{public}s.", code.c_str());
        status = I18nErrorCode::INVALID_PARAM;
        return "";
    }
    icu::UnicodeString displayname;
    icuDisplaynames->scriptDisplayName(code.c_str(), displayname);
    std::string result = "";
    if (!displayname.isBogus()) {
        displayname.toUTF8String(result);
    }
    return result;
}

std::string DisplayNames::DisplayCurrency(const std::string &code)
{
    if (!LocaleHelper::IsWellFormedCurrencyCode(code)) {
        HILOG_ERROR_I18N("DisplayNames::DisplayCurrency: error code %{public}s.", code.c_str());
        status = I18nErrorCode::INVALID_PARAM;
        return "";
    }
    icu::UnicodeString displayname;
    icuDisplaynames->keyValueDisplayName("currency", code.c_str(), displayname);
    std::string result = "";
    if (!displayname.isBogus()) {
        displayname.toUTF8String(result);
    }
    return result;
}

std::string DisplayNames::DisplayCalendar(const std::string &code)
{
    if (!LocaleHelper::IsWellFormedCalendarCode(code)) {
        HILOG_ERROR_I18N("DisplayNames::DisplayCalendar: error code %{public}s.", code.c_str());
        status = I18nErrorCode::INVALID_PARAM;
        return "";
    }
    std::string calendarStrCode = std::strcmp(code.c_str(), "gregory") == 0
                                        ? "gregorian"
                                        : std::strcmp(code.c_str(), "ethioaa") == 0
                                            ? "ethiopic-amete-alem"
                                            : code;
    icu::UnicodeString displayname;
    icuDisplaynames->keyValueDisplayName("calendar", calendarStrCode.c_str(), displayname);
    std::string result = "";
    if (!displayname.isBogus()) {
        displayname.toUTF8String(result);
    }
    return result;
}

std::string DisplayNames::DisplayDatetimefield(const std::string &code)
{
    auto fieldIter = ICU_DATE_PATTERN_FIELD.find(code);
    if (fieldIter == ICU_DATE_PATTERN_FIELD.end()) {
        HILOG_ERROR_I18N("DisplayNames::DisplayDatetimefield: error code %{public}s.", code.c_str());
        status = I18nErrorCode::INVALID_PARAM;
        return "";
    }
    UDateTimePatternField field = fieldIter->second;
    UDateTimePGDisplayWidth width = UDATPG_WIDE;
    auto icuDateWidthIter = ICU_DATE_DISPLAY_WIDTH.find(style);
    if (icuDateWidthIter != ICU_DATE_DISPLAY_WIDTH.end()) {
        width = icuDateWidthIter->second;
    }

    UErrorCode icuStatus = U_ZERO_ERROR;
    icu::Locale locales = icuDisplaynames->getLocale();
    std::unique_ptr<icu::DateTimePatternGenerator> generator(
        icu::DateTimePatternGenerator::createInstance(locales, icuStatus));
    if (U_FAILURE(icuStatus)) {
        HILOG_ERROR_I18N("DisplayNames::DisplayDatetimefield: create DateTimePatternGenerator fail");
        return "";
    }
    icu::UnicodeString displayname = generator->getFieldDisplayName(field, width);
    std::string result = "";
    displayname.toUTF8String(result);
    return result;
}

std::map<std::string, std::string> DisplayNames::ResolvedOptions()
{
    std::map<std::string, std::string> options = {
        {"locale", localeStr},
        {"style", style},
        {"type", type},
        {"fallback", fallback},
        {"languageDisplay", languageDisplay}
    };
    return options;
}

I18nErrorCode DisplayNames::GetError() const
{
    return status;
}

std::string DisplayNames::GetErrorMessage() const
{
    if (status == I18nErrorCode::SUCCESS) {
        return "";
    }
    if (type == "language") {
        return "not match the language id";
    } else if (type == "region") {
        return "invalid region";
    } else if (type == "script") {
        return "invalid script";
    } else if (type == "currency") {
        return "not a wellformed currency code";
    } else if (type == "calendar") {
        return "invalid calendar";
    } else if (type == "dateTimeField") {
        return "invalid datetimefield";
    }
    HILOG_ERROR_I18N("DisplayNames::GetErrorMessage: this branch is unreachable");
    return "";
}
} // namespace I18n
} // namespace Global
} // namespace OHOS