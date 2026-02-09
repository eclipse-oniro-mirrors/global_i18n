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
#include "i18n_hilog.h"
#include "locale_config.h"
#include "locale_helper.h"
#include "ohos/init_data.h"
#include "utils.h"
#include "parameter.h"
#include "relative_time_format.h"
#include "format_utils.h"

namespace OHOS {
namespace Global {
namespace I18n {
const char* RelativeTimeFormat::DEVICE_TYPE_NAME = "const.product.devicetype";

std::unordered_map<std::string, UDateRelativeDateTimeFormatterStyle> RelativeTimeFormat::relativeFormatStyle = {
    { "long", UDAT_STYLE_LONG },
    { "short", UDAT_STYLE_SHORT },
    { "narrow", UDAT_STYLE_NARROW }
};

std::unordered_map<std::string, std::string> RelativeTimeFormat::defaultFormatStyle = {
    { "wearable", "narrow" },
    { "liteWearable", "narrow" },
    { "watch", "narrow" }
};

std::unordered_map<std::string, URelativeDateTimeUnit> RelativeTimeFormat::relativeUnits = {
    { "second", UDAT_REL_UNIT_SECOND },
    { "seconds", UDAT_REL_UNIT_SECOND },
    { "minute", UDAT_REL_UNIT_MINUTE },
    { "minutes", UDAT_REL_UNIT_MINUTE },
    { "hour", UDAT_REL_UNIT_HOUR },
    { "hours", UDAT_REL_UNIT_HOUR },
    { "day", UDAT_REL_UNIT_DAY },
    { "days", UDAT_REL_UNIT_DAY },
    { "week", UDAT_REL_UNIT_WEEK },
    { "weeks", UDAT_REL_UNIT_WEEK },
    { "month", UDAT_REL_UNIT_MONTH },
    { "months", UDAT_REL_UNIT_MONTH },
    { "quarter", UDAT_REL_UNIT_QUARTER },
    { "quarters", UDAT_REL_UNIT_QUARTER },
    { "year", UDAT_REL_UNIT_YEAR },
    { "years", UDAT_REL_UNIT_YEAR },
};

std::unordered_map<std::string, std::string> RelativeTimeFormat::pluralUnitMap = {
    { "seconds", "second" },
    { "minutes", "minute" },
    { "hours", "hour" },
    { "days", "day" },
    { "weeks", "week" },
    { "months", "month" },
    { "quarters", "quarter" },
    { "years", "year" },
};

RelativeTimeFormat::RelativeTimeFormat(const std::vector<std::string> &localeTags,
    std::map<std::string, std::string> &configs, bool fromArkTs)
{
    createFromArkTs = fromArkTs;
    I18nErrorCode errorCode = I18nErrorCode::SUCCESS;
    std::vector<std::string> resolvedLocales =
        LocaleHelper::SupportedLocalesOf(localeTags, configs, errorCode);
    if (errorCode != I18nErrorCode::SUCCESS) {
        HILOG_ERROR_I18N("RelativeTimeFormat: Get Supported Locales failed");
        return;
    }
    SetRelativeTimeFormat(resolvedLocales, configs, fromArkTs);
}

std::vector<std::string> RelativeTimeFormat::CanonicalizeLocales(
    const std::vector<std::string>& localeTags, ErrorMessage& errorMsg)
{
    if (localeTags.size() == 0) {
        return {"en-US"};
    }
    if (localeTags.size() == 1 && localeTags[0] == "default") {
        return {"en-US"};
    }
    I18nErrorCode status = I18nErrorCode::SUCCESS;
    std::vector<std::string> resolvedLocales =
        LocaleHelper::CanonicalizeLocaleList(localeTags, status);
    if (status != I18nErrorCode::SUCCESS) {
        errorMsg.type = ErrorType::RANGE_ERROR;
        errorMsg.message = "invalid locale";
        HILOG_ERROR_I18N("RelativeTimeFormat::CanonicalizeLocales CanonicalizeLocaleList failed.");
        return {};
    }
    return resolvedLocales;
}

RelativeTimeFormat::RelativeTimeFormat(const std::vector<std::string> &localeTags,
    std::map<std::string, std::string> &configs)
{
    SetDefaultStyle();
    SetRelativeTimeFormat(localeTags, configs, false);
}

RelativeTimeFormat::~RelativeTimeFormat()
{
}

void RelativeTimeFormat::SetRelativeTimeFormat(const std::vector<std::string> &localeTags,
    std::map<std::string, std::string> &configs, bool fromArkTs)
{
    UErrorCode status = U_ZERO_ERROR;
    ParseConfigs(configs);
    for (size_t i = 0; i < localeTags.size(); i++) {
        std::string curLocale = LocaleConfig::RemoveCustExtParam(localeTags[i]);
        locale = icu::Locale::forLanguageTag(icu::StringPiece(curLocale), status);
        if (U_FAILURE(status)) {
            status = U_ZERO_ERROR;
            continue;
        }
        localeBaseName = fromArkTs ? localeTags[i] : locale.getBaseName();
        std::replace(localeBaseName.begin(), localeBaseName.end(), '_', '-');
        relativeTimeFormat = std::make_unique<icu::RelativeDateTimeFormatter>(locale, nullptr, style,
            UDISPCTX_CAPITALIZATION_NONE, status);
        if (U_FAILURE(status) || !relativeTimeFormat) {
            status = U_ZERO_ERROR;
            continue;
        }
        createSuccess = true;
        std::string value = LocaleConfig::QueryExtParam(curLocale, "nu", "-u-");
        if (!value.empty()) {
            numberingSystem = value;
            intlNumberingSystem = value;
        }
        break;
    }
    CreateRelativeTimeFormatWithDefaultLocale(fromArkTs);
}

void RelativeTimeFormat::CreateRelativeTimeFormatWithDefaultLocale(bool fromArkTs)
{
    if (!createSuccess) {
        UErrorCode status = U_ZERO_ERROR;
        std::string systemLocale = fromArkTs ?
            LocaleHelper::DefaultLocale() : LocaleConfig::GetEffectiveLocale();
        std::string curLocale = LocaleConfig::RemoveCustExtParam(systemLocale);
        locale = icu::Locale::forLanguageTag(icu::StringPiece(curLocale), status);
        if (U_FAILURE(status)) {
            status = U_ZERO_ERROR;
            return;
        }
        localeBaseName = locale.getBaseName();
        std::replace(localeBaseName.begin(), localeBaseName.end(), '_', '-');
        relativeTimeFormat = std::make_unique<icu::RelativeDateTimeFormatter>(locale, nullptr, style,
            UDISPCTX_CAPITALIZATION_NONE, status);
        if (U_SUCCESS(status) && relativeTimeFormat) {
            createSuccess = true;
            std::string value = LocaleConfig::QueryExtParam(curLocale, "nu", "-u-");
            if (!value.empty()) {
                numberingSystem = value;
                intlNumberingSystem = value;
            }
        }
    }
}

void RelativeTimeFormat::ParseConfigs(std::map<std::string, std::string> &configs)
{
    if (configs.count("style") > 0) {
        styleString = configs["style"];
    }
    if (relativeFormatStyle.count(styleString) > 0) {
        style = relativeFormatStyle[styleString];
    }
    if (configs.count("numeric") > 0) {
        numeric = configs["numeric"];
    }
    if (configs.count("localeMatcher") > 0) {
        localeMatcher = configs["localeMatcher"];
    }
}

std::string RelativeTimeFormat::Format(double number, const std::string &unit)
{
    icu::FormattedRelativeDateTime formattedTime = FormatToFormattedValue(number, unit);
    UErrorCode status = U_ZERO_ERROR;
    std::string result;
    icu::UnicodeString formatResult = formattedTime.toString(status);
    if (U_SUCCESS(status)) {
        formatResult.toUTF8String(result);
    }
    return PseudoLocalizationProcessor(result);
}

std::string RelativeTimeFormat::GetSingularUnit(const std::string &unit)
{
    std::string singularUnit(unit);
    if (createFromArkTs && pluralUnitMap.find(unit) != pluralUnitMap.end()) {
        singularUnit = pluralUnitMap[unit];
    }
    return singularUnit;
}

void RelativeTimeFormat::InsertInfo(std::vector<std::vector<std::string>> &timeVector,
    const std::string &unit, bool isInteger, const std::string &value)
{
    std::vector<std::string> info;
    std::string singularUnit = GetSingularUnit(unit);
    if (isInteger) {
        info.push_back("integer");
        info.push_back(value);
        info.push_back(singularUnit);
    } else {
        info.push_back("literal");
        info.push_back(value);
    }
    timeVector.push_back(info);
}

void RelativeTimeFormat::ProcessIntegerField(const std::map<size_t, size_t> &indexMap,
    std::vector<std::vector<std::string>> &timeVector, size_t &startIndex, const std::string &unit,
    const std::string &result)
{
    for (auto iter = indexMap.begin(); iter != indexMap.end(); iter++) {
        if (iter->first > startIndex) {
            InsertInfo(timeVector, unit, true, result.substr(startIndex, iter->first - startIndex));
            InsertInfo(timeVector, unit, true, result.substr(iter->first, iter->second - iter->first));
            startIndex = iter->second;
        }
    }
}

void RelativeTimeFormat::FormatToParts(double number, const std::string &unit,
    std::vector<std::vector<std::string>> &timeVector)
{
    icu::FormattedRelativeDateTime fmtRelativeTime = FormatToFormattedValue(number, unit);
    UErrorCode status = U_ZERO_ERROR;
    icu::UnicodeString fmtRelativeTimeStr = fmtRelativeTime.toString(status);
    if (U_FAILURE(status)) {
        HILOG_ERROR_I18N("RelativeTimeFormat::FormatToParts: Get formated string failed.");
        return;
    }
    std::string result;
    fmtRelativeTimeStr.toUTF8String(result);
    if (createFromArkTs) {
        IntlFormatToParts(fmtRelativeTime, result, timeVector, number, unit);
        return;
    }
    icu::ConstrainedFieldPosition constrainedPos;
    constrainedPos.constrainCategory(UFIELD_CATEGORY_NUMBER);
    size_t prevIndex = 0;
    size_t length = result.length();
    std::map<size_t, size_t> indexMap;
    while (fmtRelativeTime.nextPosition(constrainedPos, status)) {
        if (U_FAILURE(status)) {
            HILOG_ERROR_I18N("RelativeTimeFormat::FormatToParts: Get next position failed.");
            return;
        }
        size_t startIndex = (size_t)constrainedPos.getStart();
        if (constrainedPos.getCategory() == UFIELD_CATEGORY_NUMBER) {
            if (constrainedPos.getField() == UNUM_GROUPING_SEPARATOR_FIELD) {
                indexMap.insert(std::make_pair(startIndex, (size_t)constrainedPos.getLimit()));
                continue;
            }
            if (startIndex > prevIndex) {
                InsertInfo(timeVector, unit, false, result.substr(prevIndex, startIndex - prevIndex));
            }
            if (constrainedPos.getField() == UNUM_INTEGER_FIELD) {
                ProcessIntegerField(indexMap, timeVector, startIndex, unit, result);
            }
            InsertInfo(timeVector, unit, true, result.substr(startIndex,
                (size_t)constrainedPos.getLimit() - startIndex));
            prevIndex = (size_t)constrainedPos.getLimit();
        }
    }
    if (prevIndex < length) {
        InsertInfo(timeVector, unit, false, result.substr(prevIndex, length - prevIndex));
    }
}

void RelativeTimeFormat::IntlFormatToParts(icu::FormattedValue &fmtRelativeTime, const std::string& result,
    std::vector<std::vector<std::string>> &timeVector, double number, const std::string &unit)
{
    UErrorCode status = U_ZERO_ERROR;
    icu::ConstrainedFieldPosition constrainedPos;
    constrainedPos.constrainCategory(UFIELD_CATEGORY_NUMBER);
    int32_t prevIndex = 0;
    size_t length = result.length();
    std::map<int32_t, int32_t> indexMap;
    while (fmtRelativeTime.nextPosition(constrainedPos, status)) {
        if (U_FAILURE(status)) {
            HILOG_ERROR_I18N("RelativeTimeFormat::FormatToParts: Get next position failed.");
            return;
        }
        int32_t fieldId = constrainedPos.getField();
        int32_t startIndex = constrainedPos.getStart();
        int32_t limit = constrainedPos.getLimit();
        if (constrainedPos.getField() == UNUM_GROUPING_SEPARATOR_FIELD) {
            indexMap.insert(std::make_pair(startIndex, limit));
            continue;
        }
        if (startIndex > prevIndex) {
            std::string val = result.substr(prevIndex, startIndex - prevIndex);
            timeVector.push_back(FormatPart("literal", val, ""));
        }
        for (auto iter = indexMap.begin(); iter != indexMap.end(); iter++) {
            if (iter->first > startIndex) {
                std::string typeString = FormatUtils::GetNumberFieldType("number", fieldId, number);
                std::string valStr = result.substr(startIndex, iter->first - startIndex);
                timeVector.push_back(FormatPart(typeString, valStr, unit));
                std::string value = result.substr(iter->first, iter->second - iter->first);
                timeVector.push_back(FormatPart("group", value, unit));
                startIndex = iter->second;
            }
        }
        std::string typeString = FormatUtils::GetNumberFieldType("number", fieldId, number);
        std::string value = result.substr(startIndex, limit - startIndex);
        timeVector.push_back(FormatPart(typeString, value, unit));
        prevIndex = limit;
    }
    if (length > static_cast<size_t>(prevIndex)) {
        std::string value = result.substr(prevIndex, length - static_cast<size_t>(prevIndex));
        timeVector.push_back(FormatPart("literal", value, ""));
    }
}

std::vector<std::string> RelativeTimeFormat::FormatPart(const std::string& type,
    const std::string& value, const std::string& unit)
{
    std::vector<std::string> info;
    info.push_back(type);
    info.push_back(value);
    if (!unit.empty()) {
        std::string singularUnit = GetSingularUnit(unit);
        info.push_back(singularUnit);
    }
    return info;
}

icu::FormattedRelativeDateTime RelativeTimeFormat::FormatToFormattedValue(double number, const std::string &unit)
{
    if (!createSuccess || relativeTimeFormat == nullptr || !relativeUnits.count(unit)) {
        return {};
    }
    UErrorCode status = U_ZERO_ERROR;
    icu::FormattedRelativeDateTime fmtRelativeTime;
    if (numeric.compare("always") == 0) {
        fmtRelativeTime = relativeTimeFormat->formatNumericToValue(number, relativeUnits[unit], status);
    } else {
        fmtRelativeTime = relativeTimeFormat->formatToValue(number, relativeUnits[unit], status);
    }

    if (U_FAILURE(status)) {
        return {};
    }
    return fmtRelativeTime;
}

void RelativeTimeFormat::GetResolvedOptions(std::map<std::string, std::string> &map)
{
    map.insert(std::make_pair("locale", localeBaseName));
    if (!styleString.empty()) {
        map.insert(std::make_pair("style", styleString));
    }
    if (!numeric.empty()) {
        map.insert(std::make_pair("numeric", numeric));
    }
    if (createFromArkTs) {
        std::string numSystem = intlNumberingSystem.empty() ? GetNumberSystemFromLocale() : intlNumberingSystem;
        map.insert(std::make_pair("numberingSystem", numSystem));
    } else if (!numberingSystem.empty()) {
        map.insert(std::make_pair("numberingSystem", numberingSystem));
    }
}

std::string RelativeTimeFormat::GetNumberSystemFromLocale()
{
    UErrorCode status = U_ZERO_ERROR;
    icu::Locale inLocale = icu::Locale::forLanguageTag(localeBaseName.data(), status);
    if (U_FAILURE(status)) {
        HILOG_ERROR_I18N("GetNumberSystemFromLocale: Create locale for %{public}s failed", localeBaseName.c_str());
        return "";
    }
    std::unique_ptr<icu::NumberingSystem> numSystem =
        std::unique_ptr<icu::NumberingSystem>(icu::NumberingSystem::createInstance(inLocale, status));
    if (U_FAILURE(status) || numSystem == nullptr) {
        HILOG_ERROR_I18N("GetNumberSystemFromLocale: Create icu::NumberingSystem failed");
        return "";
    }
    const char* name = numSystem->getName();
    if (name == nullptr) {
        HILOG_ERROR_I18N("GetNumberSystemFromLocale: icu::NumberingSystem name is nullptr");
        return "";
    }
    std::string numberSystem(name);
    return numberSystem;
}

std::vector<std::string> RelativeTimeFormat::SupportedLocalesOf(
    const std::vector<std::string> &requestLocales,
    const std::map<std::string, std::string> &configs, I18nErrorCode &status)
{
    return LocaleHelper::SupportedLocalesOf(requestLocales, configs, status);
}

void RelativeTimeFormat::SetDefaultStyle()
{
    char value[CONFIG_LEN];
    int code = GetParameter(DEVICE_TYPE_NAME, "", value, CONFIG_LEN);
    if (code > 0) {
        std::string deviceType = value;
        auto iter = defaultFormatStyle.find(deviceType);
        styleString = iter != defaultFormatStyle.end() ? defaultFormatStyle[deviceType] : styleString;
    }
}
} // namespace I18n
} // namespace Global
} // namespace OHOS
