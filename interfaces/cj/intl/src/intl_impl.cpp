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
#include "intl_impl.h"

#include <vector>
#include <map>
#include "cj_lambda.h"

using namespace OHOS::FFI;

namespace OHOS::Global::I18n::Intl {
std::vector<std::string> ArrayStringToVectorString(CArrString arrString)
{
    std::vector<std::string> res;
    for (int i = 0; i < arrString.size; i++) {
        std::string value = arrString.head[i];
        res.push_back(value);
    }
    return res;
}

char *IMallocCString(const std::string &origin)
{
    if (origin.empty()) {
        return nullptr;
    }
    auto length = origin.length() + 1;
    char *res = static_cast<char *>(malloc(sizeof(char) * length));
    if (res == nullptr) {
        return nullptr;
    }
    return std::char_traits<char>::copy(res, origin.c_str(), length);
}

NumberFormatImpl::NumberFormatImpl(int32_t *errCode)
{
    std::vector<std::string> locale;
    std::map<std::string, std::string> map = {};
    numberFmt_ = std::make_unique<NumberFormat>(locale, map);

    if (numberFmt_ == nullptr) {
        *errCode = -1;
        HILOG_ERROR_I18N("The numberFormat is nullptr");
        return;
    }
}

NumberFormatImpl::NumberFormatImpl(char *locale, CNumberOptions options, int64_t flag, int32_t *errCode)
{
    std::vector<std::string> locale_list;
    std::string locale_str = locale;
    locale_list.push_back(locale_str);
    std::map<std::string, std::string> map = {};
    if (flag == 0) {
        if (std::strlen(options.locale) != 0) {
            map.insert(std::make_pair("locale", options.locale));
        }
        if (std::strlen(options.currency) != 0) {
            map.insert(std::make_pair("currency", options.currency));
        }
        map.insert(std::make_pair("currencySign", options.currencySign));
        map.insert(std::make_pair("currencyDisplay", options.currencyDisplay));
        if (std::strlen(options.unit) != 0) {
            map.insert(std::make_pair("unit", options.unit));
        }
        map.insert(std::make_pair("unitDisplay", options.unitDisplay));
        map.insert(std::make_pair("unitUsage", options.unitUsage));
        map.insert(std::make_pair("signDispaly", options.signDispaly));
        map.insert(std::make_pair("compactDisplay", options.compactDisplay));
        map.insert(std::make_pair("notation", options.notation));
        map.insert(std::make_pair("localeMather", options.localeMather));
        map.insert(std::make_pair("style", options.style));
        if (std::strlen(options.numberingSystem) != 0) {
            map.insert(std::make_pair("numberingSystem", options.numberingSystem));
        }
        std::string value = std::to_string(options.useGrouping) == "1" ? "true" : "false";
        map.insert(std::make_pair("useGrouping", value));
        map.insert(std::make_pair("minimumIntegerDigits", std::to_string(options.minimumIntegerDigits)));
        map.insert(std::make_pair("minimumFractionDigits", std::to_string(options.minimumFractionDigits)));
        map.insert(std::make_pair("maximumFractionDigits", std::to_string(options.maximumFractionDigits)));
        map.insert(std::make_pair("minimumSignificantDigits", std::to_string(options.minimumSignificantDigits)));
        map.insert(std::make_pair("maximumSignificantDigits", std::to_string(options.maximumSignificantDigits)));
        if (options.minimumFractionDigits != -1 && options.maximumFractionDigits != -1 &&
            options.minimumFractionDigits > options.maximumFractionDigits) {
            HILOG_ERROR_I18N(
                "GetNumberOptionValues: Invalid parameter value: minimumFractionDigits > maximumFractionDigits");
        }
    }
    numberFmt_ = std::make_unique<NumberFormat>(locale_list, map);

    if (numberFmt_ == nullptr) {
        *errCode = -1;
        HILOG_ERROR_I18N("The numberFormat is nullptr");
        return;
    }
}

NumberFormatImpl::NumberFormatImpl(CArrString locale, CNumberOptions options, int64_t flag, int32_t *errCode)
{
    std::vector<std::string> locale_list = ArrayStringToVectorString(locale);
    std::map<std::string, std::string> map = {};
    if (flag == 0) {
        if (std::strlen(options.locale) != 0) {
            map.insert(std::make_pair("locale", options.locale));
        }
        if (std::strlen(options.currency) != 0) {
            map.insert(std::make_pair("currency", options.currency));
        }
        map.insert(std::make_pair("currencySign", options.currencySign));
        map.insert(std::make_pair("currencyDisplay", options.currencyDisplay));
        if (std::strlen(options.unit) != 0) {
            map.insert(std::make_pair("unit", options.unit));
        }
        map.insert(std::make_pair("unitDisplay", options.unitDisplay));
        map.insert(std::make_pair("unitUsage", options.unitUsage));
        map.insert(std::make_pair("signDispaly", options.signDispaly));
        map.insert(std::make_pair("compactDisplay", options.compactDisplay));
        map.insert(std::make_pair("notation", options.notation));
        map.insert(std::make_pair("localeMather", options.localeMather));
        map.insert(std::make_pair("style", options.style));
        if (std::strlen(options.numberingSystem) != 0) {
            map.insert(std::make_pair("numberingSystem", options.numberingSystem));
        }
        std::string value = std::to_string(options.useGrouping) == "1" ? "true" : "false";
        map.insert(std::make_pair("useGrouping", value));
        map.insert(std::make_pair("minimumIntegerDigits", std::to_string(options.minimumIntegerDigits)));
        map.insert(std::make_pair("minimumFractionDigits", std::to_string(options.minimumFractionDigits)));
        map.insert(std::make_pair("maximumFractionDigits", std::to_string(options.maximumFractionDigits)));
        map.insert(std::make_pair("minimumSignificantDigits", std::to_string(options.minimumSignificantDigits)));
        map.insert(std::make_pair("maximumSignificantDigits", std::to_string(options.maximumSignificantDigits)));

        if (options.minimumFractionDigits != -1 && options.maximumFractionDigits != -1 &&
            options.minimumFractionDigits > options.maximumFractionDigits) {
            HILOG_ERROR_I18N(
                "GetNumberOptionValues: Invalid parameter value: minimumFractionDigits > maximumFractionDigits");
        }
    }
    numberFmt_ = std::make_unique<NumberFormat>(locale_list, map);

    if (numberFmt_ == nullptr) {
        *errCode = -1;
        HILOG_ERROR_I18N("The numberFormat is nullptr");
        return;
    }
}

char *NumberFormatImpl::Format(double number)
{
    std::string value = numberFmt_->Format(number);
    return IMallocCString(value);
}

CNumberOptions NumberFormatImpl::ResolveOptions()
{
    std::map<std::string, std::string> options = {};
    numberFmt_->GetResolvedOptions(options);
    struct CNumberOptions numberOptions;
    numberOptions.locale = IMallocCString(options["locale"]);
    numberOptions.currency = IMallocCString(options["currency"]);
    numberOptions.currencySign = IMallocCString(options["currencySign"]);
    numberOptions.unit = IMallocCString(options["unit"]);
    numberOptions.unitDisplay = IMallocCString(options["unitDisplay"]);
    numberOptions.unitUsage = IMallocCString(options["unitUsage"]);
    numberOptions.signDispaly = IMallocCString(options["signDispaly"]);
    numberOptions.compactDisplay = IMallocCString(options["compactDisplay"]);
    numberOptions.notation = IMallocCString(options["notation"]);
    numberOptions.localeMather = IMallocCString(options["localeMather"]);
    numberOptions.style = IMallocCString(options["style"]);
    numberOptions.numberingSystem = IMallocCString(options["numberingSystem"]);
    numberOptions.useGrouping = (options["useGrouping"] == "true");
    numberOptions.minimumIntegerDigits = atoi(options["minimumIntegerDigits"].c_str());
    numberOptions.minimumFractionDigits = atoi(options["minimumFractionDigits"].c_str());
    numberOptions.maximumFractionDigits = atoi(options["maximumFractionDigits"].c_str());
    numberOptions.minimumSignificantDigits = atoi(options["minimumSignificantDigits"].c_str());
    numberOptions.maximumSignificantDigits = atoi(options["maximumSignificantDigits"].c_str());

    return numberOptions;
}

RelativeTimeFormatImpl::RelativeTimeFormatImpl(int32_t *errCode)
{
    std::vector<std::string> locale;
    std::map<std::string, std::string> options = {};
    relativeTimeFmt_ = std::make_unique<RelativeTimeFormat>(locale, options);

    if (relativeTimeFmt_ == nullptr) {
        *errCode = -1;
        HILOG_ERROR_I18N("The RelativeTimeFormat is nullptr");
        return;
    }
}

RelativeTimeFormatImpl::RelativeTimeFormatImpl(
    char *locale, CRelativeTimeFormatInputOptions options, int64_t flag, int32_t *errCode)
{
    std::vector<std::string> locale_list;
    std::string locale_str = locale;
    locale_list.push_back(locale_str);
    std::map<std::string, std::string> map = {};
    if (flag == 0) {
        map.insert(std::make_pair("localeMatcher", options.localeMatcher));
        map.insert(std::make_pair("numeric", options.numeric));
        map.insert(std::make_pair("style", options.style));
    }
    relativeTimeFmt_ = std::make_unique<RelativeTimeFormat>(locale_list, map);
    if (relativeTimeFmt_ == nullptr) {
        *errCode = -1;
        HILOG_ERROR_I18N("The RelativeTimeFormat is nullptr");
        return;
    }
}

RelativeTimeFormatImpl::RelativeTimeFormatImpl(
    CArrString locale, CRelativeTimeFormatInputOptions options, int64_t flag, int32_t *errCode)
{
    std::vector<std::string> locale_list = ArrayStringToVectorString(locale);

    std::map<std::string, std::string> map = {};
    if (flag == 0) {
        map.insert(std::make_pair("localeMatcher", options.localeMatcher));
        map.insert(std::make_pair("numeric", options.numeric));
        map.insert(std::make_pair("style", options.style));
    }
    relativeTimeFmt_ = std::make_unique<RelativeTimeFormat>(locale_list, map);

    if (relativeTimeFmt_ == nullptr) {
        *errCode = -1;
        HILOG_ERROR_I18N("The RelativeTimeFormat is nullptr");
        return;
    }
}

char *RelativeTimeFormatImpl::Format(double number, char *unit)
{
    std::string unitStr = unit;
    std::string value = relativeTimeFmt_->Format(number, unitStr);
    char *res = IMallocCString(value);
    return res;
}

void FreeCArrArrString(CArrString *arr, size_t index)
{
    for (size_t i = 0; i < index; i++) {
        for (int64_t j = 0; j < arr[i].size; j++) {
            free(arr[i].head[j]);
        }
        free(arr[i].head);
    }
}

CArrArrString RelativeTimeFormatImpl::FormatToParts(double number, char *unit)
{
    std::string unitStr = unit;
    std::vector<std::vector<std::string>> timeVectorStr;

    CArrArrString ret = {.head = nullptr, .size = 0};
    relativeTimeFmt_->FormatToParts(number, unitStr, timeVectorStr);
    CArrString *res = static_cast<CArrString *>(malloc(sizeof(CArrString) * timeVectorStr.size()));
    if (res == nullptr) {
        HILOG_ERROR_I18N("The CArrString is nullptr");
        return ret;
    }

    for (size_t i = 0; i < timeVectorStr.size(); i++) {
        std::vector<std::string> timeVector = timeVectorStr[i];
        CArrString retime = {.head = nullptr, .size = 0};
        char **time = static_cast<char **>(malloc(sizeof(char *) * timeVector.size()));
        if (time == nullptr) {
            HILOG_ERROR_I18N("The char** is nullptr");
            FreeCArrArrString(res, i);
            free(res);
            return ret;
        }
        for (size_t j = 0; j < timeVector.size(); j++) {
            std::string value = timeVector[j];
            time[j] = IMallocCString(value);
        }
        retime.head = time;
        retime.size = static_cast<int64_t>(timeVector.size());
        res[i] = retime;
    }
    ret.head = res;
    ret.size = static_cast<int64_t>(timeVectorStr.size());
    return ret;
}

CRelativeTimeFormatResolveOptions RelativeTimeFormatImpl::ResolveOptions()
{
    std::map<std::string, std::string> options = {};
    relativeTimeFmt_->GetResolvedOptions(options);
    struct CRelativeTimeFormatResolveOptions relativeTimeFormatResolveOptions;
    relativeTimeFormatResolveOptions.localeMatcher = IMallocCString(options["localeMatcher"]);
    relativeTimeFormatResolveOptions.numeric = IMallocCString(options["numeric"]);
    relativeTimeFormatResolveOptions.style = IMallocCString(options["style"]);
    relativeTimeFormatResolveOptions.numberingSystem = IMallocCString(options["numberingSystem"]);
    return relativeTimeFormatResolveOptions;
}

PluralRulesImpl::PluralRulesImpl(int32_t *errCode)
{
    std::vector<std::string> locale;
    std::map<std::string, std::string> options = {};
    pluralRules_ = std::make_unique<PluralRules>(locale, options);

    if (pluralRules_ == nullptr) {
        *errCode = -1;
        HILOG_ERROR_I18N("The PluralRules is nullptr");
        return;
    }
}

PluralRulesImpl::PluralRulesImpl(char *locale, CPluralRulesOptions options, int64_t flag, int32_t *errCode)
{
    std::vector<std::string> locale_list;
    std::string locale_str = locale;
    locale_list.push_back(locale_str);
    std::map<std::string, std::string> map = {};
    if (flag == 0) {
        map.insert(std::make_pair("localeMatcher", options.localeMatcher));
        map.insert(std::make_pair("type", options.type));
        map.insert(std::make_pair("minimumIntegerDigits", std::to_string(options.minimumIntegerDigits)));
        map.insert(std::make_pair("minimumFractionDigits", std::to_string(options.minimumFractionDigits)));
        map.insert(std::make_pair("maximumFractionDigits", std::to_string(options.maximumFractionDigits)));
        map.insert(std::make_pair("minimumSignificantDigits", std::to_string(options.minimumSignificantDigits)));
        map.insert(std::make_pair("maximumSignificantDigits", std::to_string(options.maximumSignificantDigits)));
    }
    pluralRules_ = std::make_unique<PluralRules>(locale_list, map);

    if (pluralRules_ == nullptr) {
        *errCode = -1;
        HILOG_ERROR_I18N("The PluralRules is nullptr");
        return;
    }
}

PluralRulesImpl::PluralRulesImpl(CArrString locale, CPluralRulesOptions options, int64_t flag, int32_t *errCode)
{
    std::vector<std::string> locale_list = ArrayStringToVectorString(locale);

    std::map<std::string, std::string> map = {};
    if (flag == 0) {
        map.insert(std::make_pair("localeMatcher", options.localeMatcher));
        map.insert(std::make_pair("type", options.type));
        map.insert(std::make_pair("minimumIntegerDigits", std::to_string(options.minimumIntegerDigits)));
        map.insert(std::make_pair("minimumFractionDigits", std::to_string(options.minimumFractionDigits)));
        map.insert(std::make_pair("maximumFractionDigits", std::to_string(options.maximumFractionDigits)));
        map.insert(std::make_pair("minimumSignificantDigits", std::to_string(options.minimumSignificantDigits)));
        map.insert(std::make_pair("maximumSignificantDigits", std::to_string(options.maximumSignificantDigits)));
    }
    pluralRules_ = std::make_unique<PluralRules>(locale_list, map);

    if (pluralRules_ == nullptr) {
        *errCode = -1;
        HILOG_ERROR_I18N("The PluralRules is nullptr");
        return;
    }
}

char *PluralRulesImpl::Select(double n)
{
    std::string value = pluralRules_->Select(n);
    char *res = IMallocCString(value);
    return res;
}

CollatorImpl::CollatorImpl(int32_t *errCode)
{
    std::vector<std::string> locale;
    std::map<std::string, std::string> options = {};
    collator_ = std::make_unique<Collator>(locale, options);

    if (collator_ == nullptr) {
        *errCode = -1;
        HILOG_ERROR_I18N("The Collator is nullptr");
        return;
    }
}

CollatorImpl::CollatorImpl(char *locale, CCollatorOptions options, int64_t flag, int32_t *errCode)
{
    std::vector<std::string> locale_list;
    std::string locale_str = locale;
    locale_list.push_back(locale_str);
    std::map<std::string, std::string> map = {};
    if (flag == 0) {
        map.insert(std::make_pair("localeMatcher", options.localeMatcher));
        map.insert(std::make_pair("usage", options.usage));
        map.insert(std::make_pair("sensitivity", options.sensitivity));
        std::string ignore = std::to_string(options.ignorePunctuation) == "1" ? "true" : "false";
        map.insert(std::make_pair("ignorePunctuation", ignore));
        map.insert(std::make_pair("collation", options.collation));
        std::string num = std::to_string(options.numeric) == "1" ? "true" : "false";
        map.insert(std::make_pair("numeric", num));
        map.insert(std::make_pair("caseFirst", options.caseFirst));
    }
    collator_ = std::make_unique<Collator>(locale_list, map);

    if (collator_ == nullptr) {
        *errCode = -1;
        HILOG_ERROR_I18N("The Collator is nullptr");
        return;
    }
}

CollatorImpl::CollatorImpl(CArrString locale, CCollatorOptions options, int64_t flag, int32_t *errCode)
{
    std::vector<std::string> locale_list = ArrayStringToVectorString(locale);

    std::map<std::string, std::string> map = {};
    if (flag == 0) {
        map.insert(std::make_pair("localeMatcher", options.localeMatcher));
        map.insert(std::make_pair("usage", options.usage));
        map.insert(std::make_pair("sensitivity", options.sensitivity));
        std::string ignore = std::to_string(options.ignorePunctuation) == "1" ? "true" : "false";
        map.insert(std::make_pair("ignorePunctuation", ignore));
        map.insert(std::make_pair("collation", options.collation));
        std::string num = std::to_string(options.numeric) == "1" ? "true" : "false";
        map.insert(std::make_pair("numeric", num));
        map.insert(std::make_pair("caseFirst", options.caseFirst));
    }
    collator_ = std::make_unique<Collator>(locale_list, map);

    if (collator_ == nullptr) {
        *errCode = -1;
        HILOG_ERROR_I18N("The Collator is nullptr");
        return;
    }
}

CCollatorOptions CollatorImpl::ResolveOptions()
{
    std::map<std::string, std::string> options = {};
    collator_->ResolvedOptions(options);
    struct CCollatorOptions collatorResolveOptions;
    collatorResolveOptions.localeMatcher = IMallocCString(options["localeMatcher"]);
    collatorResolveOptions.usage = IMallocCString(options["usage"]);
    collatorResolveOptions.sensitivity = IMallocCString(options["sensitivity"]);
    collatorResolveOptions.ignorePunctuation = (options["ignorePunctuation"] == "true");
    collatorResolveOptions.collation = IMallocCString(options["collation"]);
    collatorResolveOptions.numeric = (options["numeric"] == "true");
    collatorResolveOptions.caseFirst = IMallocCString(options["caseFirst"]);

    return collatorResolveOptions;
}

int32_t CollatorImpl::Compare(char *str1, char *str2)
{
    std::string str1_str = str1;
    std::string str2_str = str2;
    int32_t result = collator_->Compare(str1_str, str2_str);
    return result;
}

DateTimeFormatImpl::DateTimeFormatImpl(int32_t *errCode)
{
    std::vector<std::string> locale;
    std::map<std::string, std::string> options = {};
    dateFmt_ = std::make_unique<DateTimeFormat>(locale, options);

    if (dateFmt_ == nullptr) {
        *errCode = -1;
        HILOG_ERROR_I18N("The DateTimeFormat is nullptr");
        return;
    }
}

DateTimeFormatImpl::DateTimeFormatImpl(char *locale, CDateTimeOptions options, int64_t flag, int32_t *errCode)
{
    std::vector<std::string> locale_list;
    std::string locale_str = locale;
    locale_list.push_back(locale_str);
    std::map<std::string, std::string> map = {};
    if (flag == 0) {
        if (std::strlen(options.locale) != 0) {
            map.insert(std::make_pair("locale", options.locale));
        }
        if (std::strlen(options.dateStyle) != 0) {
            map.insert(std::make_pair("dateStyle", options.dateStyle));
        }
        if (std::strlen(options.timeStyle) != 0) {
            map.insert(std::make_pair("timeStyle", options.timeStyle));
        }
        if (std::strlen(options.hourCycle) != 0) {
            map.insert(std::make_pair("hourCycle", options.hourCycle));
        }
        if (std::strlen(options.timeZone) != 0) {
            map.insert(std::make_pair("timeZone", options.timeZone));
        }
        if (std::strlen(options.numberingSystem) != 0) {
            map.insert(std::make_pair("numberingSystem", options.numberingSystem));
        }
        std::string h12 = std::to_string(options.hour12) == "1" ? "true" : "false";
        map.insert(std::make_pair("hour12", h12));
        if (std::strlen(options.weekday) != 0) {
            map.insert(std::make_pair("weekday", options.weekday));
        }
        std::map<std::string, std::string> map1 = MapInsert(options);
        map.insert(map1.begin(), map1.end());
    }
    dateFmt_ = std::make_unique<DateTimeFormat>(locale_list, map);

    if (dateFmt_ == nullptr) {
        *errCode = -1;
        HILOG_ERROR_I18N("The DateTimeFormat is nullptr");
        return;
    }
}

std::map<std::string, std::string> DateTimeFormatImpl::MapInsert(CDateTimeOptions options)
{
    std::map<std::string, std::string> map = {};
    if (std::strlen(options.era) != 0) {
        map.insert(std::make_pair("era", options.era));
    }
    if (std::strlen(options.year) != 0) {
        map.insert(std::make_pair("year", options.year));
    }
    if (std::strlen(options.month) != 0) {
        map.insert(std::make_pair("month", options.month));
    }
    if (std::strlen(options.day) != 0) {
        map.insert(std::make_pair("day", options.day));
    }
    if (std::strlen(options.hour) != 0) {
        map.insert(std::make_pair("hour", options.hour));
    }
    if (std::strlen(options.minute) != 0) {
        map.insert(std::make_pair("minute", options.minute));
    }
    if (std::strlen(options.second) != 0) {
        map.insert(std::make_pair("second", options.second));
    }
    if (std::strlen(options.timeZoneName) != 0) {
        map.insert(std::make_pair("timeZoneName", options.timeZoneName));
    }
    if (std::strlen(options.dayPeriod) != 0) {
        map.insert(std::make_pair("dayPeriod", options.dayPeriod));
    }
    if (std::strlen(options.localeMatcher) != 0) {
        map.insert(std::make_pair("localeMatcher", options.localeMatcher));
    }
    if (std::strlen(options.formatMatcher) != 0) {
        map.insert(std::make_pair("formatMatcher", options.formatMatcher));
    }
    return map;
}

DateTimeFormatImpl::DateTimeFormatImpl(CArrString locale, CDateTimeOptions options, int64_t flag, int32_t *errCode)
{
    std::vector<std::string> locale_list = ArrayStringToVectorString(locale);

    std::map<std::string, std::string> map = {};
    if (flag == 0) {
        if (std::strlen(options.locale) != 0) {
            map.insert(std::make_pair("locale", options.locale));
        }
        if (std::strlen(options.dateStyle) != 0) {
            map.insert(std::make_pair("dateStyle", options.dateStyle));
        }
        if (std::strlen(options.timeStyle) != 0) {
            map.insert(std::make_pair("timeStyle", options.timeStyle));
        }
        if (std::strlen(options.hourCycle) != 0) {
            map.insert(std::make_pair("hourCycle", options.hourCycle));
        }
        if (std::strlen(options.timeZone) != 0) {
            map.insert(std::make_pair("timeZone", options.timeZone));
        }
        if (std::strlen(options.numberingSystem) != 0) {
            map.insert(std::make_pair("numberingSystem", options.numberingSystem));
        }
        std::string h12 = std::to_string(options.hour12) == "1" ? "true" : "false";
        map.insert(std::make_pair("hour12", h12));
        if (std::strlen(options.weekday) != 0) {
            map.insert(std::make_pair("weekday", options.weekday));
        }
        std::map<std::string, std::string> map1 = MapInsert(options);
        map.insert(map1.begin(), map1.end());
    }
    dateFmt_ = std::make_unique<DateTimeFormat>(locale_list, map);

    if (dateFmt_ == nullptr) {
        *errCode = -1;
        HILOG_ERROR_I18N("The DateTimeFormat is nullptr");
        return;
    }
}

CDateTimeOptions DateTimeFormatImpl::ResolveOptions()
{
    std::map<std::string, std::string> options = {};
    dateFmt_->GetResolvedOptions(options);
    struct CDateTimeOptions dateTimeOptions;
    dateTimeOptions.locale = IMallocCString(options["locale"]);
    dateTimeOptions.dateStyle = IMallocCString(options["dateStyle"]);
    dateTimeOptions.timeStyle = IMallocCString(options["timeStyle"]);
    dateTimeOptions.hourCycle = IMallocCString(options["hourCycle"]);
    dateTimeOptions.timeZone = IMallocCString(options["timeZone"]);
    dateTimeOptions.numberingSystem = IMallocCString(options["numberingSystem"]);
    bool h = (options["hour"] == "true");
    dateTimeOptions.hour12 = h;
    dateTimeOptions.weekday = IMallocCString(options["weekday"]);
    dateTimeOptions.era = IMallocCString(options["era"]);
    dateTimeOptions.year = IMallocCString(options["year"]);
    dateTimeOptions.month = IMallocCString(options["month"]);
    dateTimeOptions.day = IMallocCString(options["day"]);
    dateTimeOptions.hour = IMallocCString(options["hour"]);
    dateTimeOptions.minute = IMallocCString(options["minute"]);
    dateTimeOptions.second = IMallocCString(options["second"]);
    dateTimeOptions.timeZoneName = IMallocCString(options["timeZoneName"]);
    dateTimeOptions.localeMatcher = IMallocCString(options["localeMatcher"]);
    dateTimeOptions.formatMatcher = IMallocCString(options["formatMatcher"]);
    return dateTimeOptions;
}

char *DateTimeFormatImpl::Format(int64_t date)
{
    std::string date_str = dateFmt_->Format(date);
    char *res = IMallocCString(date_str);
    return res;
}

char *DateTimeFormatImpl::FormatRange(int64_t startDate, int64_t endDate)
{
    std::string date_str = dateFmt_->FormatRange(startDate, endDate);
    char *res = IMallocCString(date_str);
    return res;
}

LocaleImpl::LocaleImpl(int32_t *errCode)
{
    std::string locale_str = "";
    std::map<std::string, std::string> options = {};
    locale_ = std::make_unique<LocaleInfo>(locale_str, options);
    if (locale_ == nullptr) {
        *errCode = -1;
        HILOG_ERROR_I18N("The Locale is nullptr");
        return;
    }
}

LocaleImpl::LocaleImpl(char *locale, CLocaleOptions options, int64_t flag, int32_t *errCode)
{
    std::string locale_str = locale;

    std::map<std::string, std::string> map = {};
    if (flag == 0) {
        if (std::strlen(options.calendar) != 0) {
            map.insert(std::make_pair("calendar", options.calendar));
        }
        if (std::strlen(options.collation) != 0) {
            map.insert(std::make_pair("collation", options.collation));
        }
        if (std::strlen(options.hourCycle) != 0) {
            map.insert(std::make_pair("hourCycle", options.hourCycle));
        }
        if (std::strlen(options.numberingSystem) != 0) {
            map.insert(std::make_pair("numberingSystem", options.numberingSystem));
        }
        std::string num = std::to_string(options.numeric) == "1" ? "true" : "false";
        map.insert(std::make_pair("numeric", num));
        if (std::strlen(options.caseFirst) != 0) {
            map.insert(std::make_pair("caseFirst", options.caseFirst));
        }
    }
    locale_ = std::make_unique<LocaleInfo>(locale_str, map);
    if (locale_ == nullptr) {
        *errCode = -1;
        HILOG_ERROR_I18N("The Locale is nullptr");
        return;
    }
}

char *LocaleImpl::ToString()
{
    if (locale_ == nullptr) {
        return nullptr;
    }
    std::string value = locale_->ToString();
    char *res = IMallocCString(value);
    return res;
}

char *LocaleImpl::Maximize()
{
    std::string value = locale_->Maximize();
    char *res = IMallocCString(value);
    return res;
}
char *LocaleImpl::Minimize()
{
    std::string value = locale_->Minimize();
    char *res = IMallocCString(value);
    return res;
}

char *LocaleImpl::GetLanguage()
{
    std::string value = locale_->GetLanguage();
    char *res = IMallocCString(value);
    return res;
}
char *LocaleImpl::GetRegion()
{
    std::string value = locale_->GetRegion();
    char *res = IMallocCString(value);
    return res;
}
char *LocaleImpl::GetScript()
{
    std::string value = locale_->GetScript();
    char *res = IMallocCString(value);
    return res;
}
char *LocaleImpl::GetBaseName()
{
    std::string value = locale_->GetBaseName();
    char *res = IMallocCString(value);
    return res;
}
char *LocaleImpl::GetCaseFirst()
{
    std::string value = locale_->GetCaseFirst();
    char *res = IMallocCString(value);
    return res;
}
char *LocaleImpl::GetCalendar()
{
    std::string value = locale_->GetCalendar();
    char *res = IMallocCString(value);
    return res;
}
char *LocaleImpl::GetCollation()
{
    std::string value = locale_->GetCollation();
    char *res = IMallocCString(value);
    return res;
}
char *LocaleImpl::GetHourCycle()
{
    std::string value = locale_->GetHourCycle();
    char *res = IMallocCString(value);
    return res;
}
char *LocaleImpl::GetNumberingSystem()
{
    std::string value = locale_->GetNumberingSystem();
    char *res = IMallocCString(value);
    return res;
}
char *LocaleImpl::GetNumeric()
{
    std::string value = locale_->GetNumeric();
    char *res = IMallocCString(value);
    return res;
}

}  // namespace OHOS::Global::I18n::Intl