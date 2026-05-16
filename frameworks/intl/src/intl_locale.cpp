/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "intl_locale.h"

#include "i18n_hilog.h"
#include "locale_helper.h"
#include "ohos/init_data.h"

namespace OHOS {
namespace Global {
namespace I18n {
const std::string APPLY_OPTION_ERROR_MESSAGE = "apply option to tag failed";
const std::string OUT_OF_RANGE_ERROR_MESSAGE = "Value out of range for locale options property";
const std::string INSERT_OR_BUILD_ERROR_MESSAGE = "insert or build failed";
const std::string INVALID_LOCALE_ERROR_MESSAGE = "invalid locale";
const std::string IntlLocale::languageTag = "language";
const std::string IntlLocale::baseNameTag = "baseName";
const std::string IntlLocale::regionTag = "region";
const std::string IntlLocale::scriptTag = "script";
const std::string IntlLocale::calendarTag = "calendar";
const std::string IntlLocale::collationTag = "collation";
const std::string IntlLocale::hourCycleTag = "hourCycle";
const std::string IntlLocale::numberingSystemTag = "numberingSystem";
const std::string IntlLocale::numericTag = "numeric";
const std::string IntlLocale::caseFirstTag = "caseFirst";
const std::string calendarExtTag = "ca";
const std::string collationExtTag = "co";
const std::string hourCycleExtTag = "hc";
const std::string numberingSystemExtTag = "nu";
const std::string numericExtTag = "kn";
const std::string caseFirstExtTag = "kf";
const std::unordered_set<std::string> HOUR_CYCLE = { "h11", "h12", "h23", "h24" };
const std::unordered_set<std::string> CASE_FIRST = { "upper", "lower", "false" };
constexpr uint8_t INTL_INDEX_FOUR = 4;

IntlLocale::IntlLocale(const std::string& localeTag, const std::unordered_map<std::string, std::string>& configs,
    std::string& errMessage)
{
    if (!CheckLocaleParam(localeTag, configs)) {
        errMessage = APPLY_OPTION_ERROR_MESSAGE;
        HILOG_ERROR_I18N("IntlLocale::IntlLocale: ApplyOptionsToTag failed.");
        return;
    }
    icu::LocaleBuilder builder;
    if (!SetLocaleParam(localeTag, configs, &builder)) {
        errMessage = APPLY_OPTION_ERROR_MESSAGE;
        HILOG_ERROR_I18N("IntlLocale::IntlLocale: BuildOptionsTags failed.");
        return;
    }
    if (!CheckConfigsExtParam(configs)) {
        errMessage = OUT_OF_RANGE_ERROR_MESSAGE;
        HILOG_ERROR_I18N("IntlLocale::IntlLocale: CheckOption failed.");
        return;
    }
    if (!SetConfigsExtParam(configs, &builder)) {
        errMessage = INSERT_OR_BUILD_ERROR_MESSAGE;
        HILOG_ERROR_I18N("IntlLocale::IntlLocale: InsertOptions failed.");
        return;
    }
    initSuccess = true;
}

IntlLocale::~IntlLocale()
{
}

bool IntlLocale::CheckLocaleParam(const std::string& localeTag,
    const std::unordered_map<std::string, std::string>& configs)
{
    if (localeTag.empty()) {
        HILOG_ERROR_I18N("IntlLocale::CheckLocaleParam: localeTag is empty.");
        return false;
    }
    if (!LocaleHelper::IsStructurallyValidLanguageTag(localeTag)) {
        HILOG_ERROR_I18N("IntlLocale::CheckLocaleParam: Verify structurally valid languageTag failed.");
        return false;
    }
    auto languageIterator = configs.find(languageTag);
    if (languageIterator != configs.end()) {
        std::string language = languageIterator->second;
        if (language.empty() || LocaleHelper::IsAlpha(language, INTL_INDEX_FOUR, INTL_INDEX_FOUR)) {
            HILOG_ERROR_I18N("IntlLocale::CheckLocaleParam: language is %{public}s.", language.c_str());
            return false;
        }
    }
    auto scriptIterator = configs.find(scriptTag);
    if (scriptIterator != configs.end()) {
        std::string script = scriptIterator->second;
        if (script.empty()) {
            HILOG_ERROR_I18N("IntlLocale::CheckLocaleParam: script is empty.");
            return false;
        }
    }
    auto regionIterator = configs.find(regionTag);
    if (regionIterator != configs.end()) {
        std::string region = regionIterator->second;
        if (region.empty()) {
            HILOG_ERROR_I18N("IntlLocale::CheckLocaleParam: region is empty.");
            return false;
        }
    }
    return true;
}

bool IntlLocale::SetLocaleParam(const std::string& localeTag,
    const std::unordered_map<std::string, std::string>& configs, icu::LocaleBuilder* builder)
{
    if (builder == nullptr) {
        HILOG_ERROR_I18N("IntlLocale::SetLocaleParam: builder is nullptr.");
        return false;
    }
    builder->setLanguageTag(localeTag);
    UErrorCode status = U_ZERO_ERROR;
    icu::Locale locale = builder->build(status);
    if (U_FAILURE(status)) {
        HILOG_ERROR_I18N("IntlLocale::SetLocaleParam: Build locale failed, localeTag is %{public}s.",
            localeTag.c_str());
        return false;
    }
    locale.canonicalize(status);
    if (U_FAILURE(status)) {
        HILOG_ERROR_I18N("IntlLocale::SetLocaleParam: locale canonicalize failed.");
        return false;
    }
    builder->setLocale(locale);

    auto languageIterator = configs.find(languageTag);
    if (languageIterator != configs.end()) {
        std::string language = languageIterator->second;
        builder->setLanguage(language);
    }

    auto scriptIterator = configs.find(scriptTag);
    if (scriptIterator != configs.end()) {
        std::string script = scriptIterator->second;
        builder->setScript(script);
    }

    auto regionIterator = configs.find(regionTag);
    if (regionIterator != configs.end()) {
        std::string region = regionIterator->second;
        builder->setRegion(region);
    }

    builder->build(status);
    if (U_FAILURE(status)) {
        HILOG_ERROR_I18N("IntlLocale::SetLocaleParam: Build locale failed.");
        return false;
    }
    return true;
}

bool IntlLocale::CheckConfigsExtParam(const std::unordered_map<std::string, std::string>& configs)
{
    auto hourCycleIterator = configs.find(hourCycleTag);
    if (hourCycleIterator != configs.end()) {
        std::string hourCycle = hourCycleIterator->second;
        if (HOUR_CYCLE.find(hourCycle) == HOUR_CYCLE.end()) {
            HILOG_ERROR_I18N("IntlLocale::CheckConfigsExtParam: Check hourCycle failed, hourCycle is %{public}s.",
                hourCycle.c_str());
            return false;
        }
    }

    auto caseFirstIterator = configs.find(caseFirstTag);
    if (caseFirstIterator != configs.end()) {
        std::string caseFirst = caseFirstIterator->second;
        if (CASE_FIRST.find(caseFirst) == CASE_FIRST.end()) {
            HILOG_ERROR_I18N("IntlLocale::CheckConfigsExtParam: Check caseFirst failed, caseFirst is %{public}s.",
                caseFirst.c_str());
            return false;
        }
    }

    return true;
}

bool IntlLocale::SetConfigsExtParam(const std::unordered_map<std::string, std::string>& configs,
    icu::LocaleBuilder* builder)
{
    if (!IntlLocale::SetExtParam(configs, calendarTag, calendarExtTag, builder)) {
        return false;
    }

    if (!IntlLocale::SetExtParam(configs, collationTag, collationExtTag, builder)) {
        return false;
    }

    if (!IntlLocale::SetExtParam(configs, hourCycleTag, hourCycleExtTag, builder)) {
        return false;
    }

    if (!IntlLocale::SetExtParam(configs, caseFirstTag, caseFirstExtTag, builder)) {
        return false;
    }

    if (!IntlLocale::SetExtParam(configs, numericTag, numericExtTag, builder)) {
        return false;
    }

    if (!IntlLocale::SetExtParam(configs, numberingSystemTag, numberingSystemExtTag, builder)) {
        return false;
    }

    UErrorCode status = U_ZERO_ERROR;
    icuLocale = builder->build(status);
    if (U_FAILURE(status)) {
        HILOG_ERROR_I18N("IntlLocale::IntlLocale: Build icuLocale failed.");
        return false;
    }
    icuLocale.canonicalize(status);
    if (U_FAILURE(status)) {
        HILOG_ERROR_I18N("IntlLocale::IntlLocale: icuLocale canonicalize failed.");
        return false;
    }
    return true;
}

bool IntlLocale::SetExtParam(const std::unordered_map<std::string, std::string>& configs, const std::string paramTag,
    const std::string paramExtTag, icu::LocaleBuilder* builder)
{
    auto paramIterator = configs.find(paramTag);
    if (paramIterator != configs.end()) {
        std::string param = paramIterator->second;
        if (!uloc_toLegacyType(uloc_toLegacyKey(paramExtTag.c_str()), param.c_str())) {
            HILOG_ERROR_I18N("IntlLocale::SetExtParam: Set %{public}s failed, calendar is %{public}s.",
                paramTag.c_str(), param.c_str());
            return false;
        }
        builder->setUnicodeLocaleKeyword(paramExtTag.c_str(), param.c_str());
    }
    return true;
}

std::string IntlLocale::GetLanguage()
{
    if (!initSuccess) {
        return "";
    }
    return icuLocale.getLanguage();
}

std::string IntlLocale::GetScript()
{
    if (!initSuccess) {
        return "";
    }
    return icuLocale.getScript();
}

std::string IntlLocale::GetRegion()
{
    if (!initSuccess) {
        return "";
    }
    return icuLocale.getCountry();
}

std::string IntlLocale::GetBaseName()
{
    if (!initSuccess) {
        return "";
    }
    std::string result = icuLocale.getBaseName();
    std::replace(result.begin(), result.end(), '_', '-');
    return result;
}

std::string IntlLocale::GetCalendar()
{
    if (!initSuccess) {
        return "";
    }
    UErrorCode status = U_ZERO_ERROR;
    std::string calendar = icuLocale.getUnicodeKeywordValue<std::string>(calendarExtTag, status);
    if (U_FAILURE(status)) {
        return "";
    }
    return calendar;
}

std::string IntlLocale::GetCollation()
{
    if (!initSuccess) {
        return "";
    }
    UErrorCode status = U_ZERO_ERROR;
    std::string collation = icuLocale.getUnicodeKeywordValue<std::string>(collationExtTag, status);
    if (U_FAILURE(status)) {
        return "";
    }
    return collation;
}

std::string IntlLocale::GetHourCycle()
{
    if (!initSuccess) {
        return "";
    }
    UErrorCode status = U_ZERO_ERROR;
    std::string hourCycle = icuLocale.getUnicodeKeywordValue<std::string>(hourCycleExtTag, status);
    if (U_FAILURE(status)) {
        return "";
    }
    return hourCycle;
}

std::string IntlLocale::GetNumberingSystem()
{
    if (!initSuccess) {
        return "";
    }
    UErrorCode status = U_ZERO_ERROR;
    std::string numberingSystem = icuLocale.getUnicodeKeywordValue<std::string>(numberingSystemExtTag, status);
    if (U_FAILURE(status)) {
        return "";
    }
    return numberingSystem;
}

std::string IntlLocale::GetNumeric()
{
    if (!initSuccess) {
        return "";
    }
    UErrorCode status = U_ZERO_ERROR;
    std::string numeric = icuLocale.getUnicodeKeywordValue<std::string>(numericExtTag, status);
    if (U_FAILURE(status)) {
        return "";
    }
    return numeric;
}

std::string IntlLocale::GetCaseFirst()
{
    if (!initSuccess) {
        return "";
    }
    UErrorCode status = U_ZERO_ERROR;
    std::string caseFirst = icuLocale.getUnicodeKeywordValue<std::string>(caseFirstExtTag, status);
    if (U_FAILURE(status)) {
        return "";
    }
    return caseFirst;
}

std::string IntlLocale::ToString(std::string& errMessage)
{
    if (!initSuccess) {
        return "";
    }
    UErrorCode status = U_ZERO_ERROR;
    std::string result = icuLocale.toLanguageTag<std::string>(status);
    if (U_FAILURE(status)) {
        errMessage = INVALID_LOCALE_ERROR_MESSAGE;
        HILOG_ERROR_I18N("IntlLocale::ToString: Get languageTag from icuLocale failed.");
        return "";
    }
    return result;
}

std::string IntlLocale::Maximize()
{
    if (!initSuccess) {
        return "";
    }
    UErrorCode status = U_ZERO_ERROR;
    icu::Locale curLocale = icuLocale;
    curLocale.addLikelySubtags(status);
    if (U_FAILURE(status)) {
        HILOG_ERROR_I18N("IntlLocale::Maximize: Add likely subtags failed.");
        return "";
    }
    std::string result = curLocale.toLanguageTag<std::string>(status);
    if (U_FAILURE(status)) {
        HILOG_ERROR_I18N("IntlLocale::Maximize: Get languageTag from curLocale failed.");
        return "";
    }
    return result;
}

std::string IntlLocale::Minimize()
{
    if (!initSuccess) {
        return "";
    }
    UErrorCode status = U_ZERO_ERROR;
    icu::Locale curLocale = icuLocale;
    curLocale.minimizeSubtags(status);
    if (U_FAILURE(status)) {
        HILOG_ERROR_I18N("IntlLocale::Minimize: Minimize subtags failed.");
        return "";
    }
    std::string result = curLocale.toLanguageTag<std::string>(status);
    if (U_FAILURE(status)) {
        HILOG_ERROR_I18N("IntlLocale::Minimize: Get languageTag from curLocale failed.");
        return "";
    }
    return result;
}

bool IntlLocale::icuInitialized = IntlLocale::Init();

bool IntlLocale::Init()
{
    SetHwIcuDirectory();
    return true;
}
} // namespace I18n
} // namespace Global
} // namespace OHOS
