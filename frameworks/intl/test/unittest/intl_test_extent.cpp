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

#include <gtest/gtest.h>
#include <map>
#include <vector>

#include "character.h"
#include "collator.h"
#include "date_time_sequence.h"
#include "date_time_filter.h"
#include "date_time_format.h"
#include "date_time_rule.h"
#include "holiday_manager.h"
#include "i18n_break_iterator.h"
#include "i18n_calendar.h"
#include "i18n_timezone.h"
#include "i18n_types.h"
#include "index_util.h"
#include "locale_compare.h"
#include "locale_config.h"
#include "locale_info.h"
#include "locale_matcher.h"
#include "measure_data.h"
#include "number_format.h"
#include "parameter.h"
#include "phone_number_format.h"
#include "plural_rules.h"
#include "preferred_language.h"
#include "regex_rule.h"
#include "relative_time_format.h"
#include "rules_engine.h"
#include "system_locale_manager.h"
#include "taboo_utils.h"
#include "taboo.h"
#include "token_setproc.h"
#include "utils.h"
#include "intl_test.h"
#include "generate_ics_file.h"
#include "signature_verifier.h"
#include <unistd.h>

using namespace OHOS::Global::I18n;
using testing::ext::TestSize;
using namespace std;

namespace OHOS {
namespace Global {
namespace I18n {
/**
 * @tc.name: IntlFuncTest0047
 * @tc.desc: Test Intl LocaleConfig GetBlockedLanguages
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest, IntlFuncTest0047, TestSize.Level1)
{
    std::unordered_set<std::string> languageSet = LocaleConfig::GetBlockedLanguages();
    EXPECT_TRUE(languageSet.size() == 0);
}

/**
 * @tc.name: IntlFuncTest0048
 * @tc.desc: Test Intl LocaleConfig GetBlockedRegions
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest, IntlFuncTest0048, TestSize.Level1)
{
    std::unordered_set<std::string> regionSet = LocaleConfig::GetBlockedRegions();
    EXPECT_TRUE(regionSet.size() == 0);
}

/**
 * @tc.name: IntlFuncTest0049
 * @tc.desc: Test Intl LocaleConfig GetLanguageBlockedRegions
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest, IntlFuncTest0049, TestSize.Level1)
{
    std::unordered_set<std::string> blockedRegionSet = LocaleConfig::GetLanguageBlockedRegions();
    EXPECT_EQ(blockedRegionSet.size(), 0);
    I18nErrorCode status = LocaleConfig::SetSystemLanguage("zh-Hans");
    EXPECT_EQ(status, I18nErrorCode::SUCCESS);
    std::string systemLanguage = LocaleConfig::GetSystemLanguage();
    EXPECT_EQ(systemLanguage, "zh-Hans");

    status = LocaleConfig::SetSystemRegion("CN");
    EXPECT_EQ(status, I18nErrorCode::SUCCESS);
    std::string systemRegion = LocaleConfig::GetSystemRegion();
    EXPECT_EQ(systemRegion, "CN");
    std::string systemLocale = LocaleConfig::GetSystemLocale();
    EXPECT_EQ(systemLocale, "zh-Hans-CN");

    const std::string locale = "zh-CN";
    bool isRTL = LocaleConfig::IsRTL(locale);
    EXPECT_TRUE(!isRTL);
    std::string validLocale = LocaleConfig::GetValidLocale(locale);
    EXPECT_EQ(validLocale, "zh-CN");

    status = LocaleConfig::Set24HourClock("false");
    EXPECT_EQ(status, I18nErrorCode::SUCCESS);
    bool is24HourClock = LocaleConfig::Is24HourClock();
    EXPECT_TRUE(!is24HourClock);

    status = LocaleConfig::SetUsingLocalDigit(true);
    EXPECT_EQ(status, I18nErrorCode::UPDATE_LOCAL_DIGIT_FAILED);
    bool usingLocalDigit = LocaleConfig::GetUsingLocalDigit();
    EXPECT_FALSE(usingLocalDigit);
}

/**
 * @tc.name: IntlFuncTest0050
 * @tc.desc: Test Intl PreferredLanguage AddPreferredLanguageExist
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest, IntlFuncTest0050, TestSize.Level1)
{
    // init test environment
    I18nErrorCode status = LocaleConfig::SetSystemLanguage("zh-Hans");
    EXPECT_EQ(status, I18nErrorCode::SUCCESS);
    std::vector<std::string> preferredLanguageList = PreferredLanguage::GetPreferredLanguageList();
    for (auto i = preferredLanguageList.size() - 1; i > 0; --i) {
        status = PreferredLanguage::RemovePreferredLanguage(i);
        EXPECT_EQ(status, I18nErrorCode::SUCCESS);
    }
    // execute test
    const std::string language = "zh-CN";
    const std::string languageDe = "de-DE";
    const std::string fakeLanguage = "1**1";
    status = PreferredLanguage::AddPreferredLanguage(language, 0);
    EXPECT_EQ(status, I18nErrorCode::SUCCESS);
    status = PreferredLanguage::AddPreferredLanguage(languageDe, 1);
    EXPECT_EQ(status, I18nErrorCode::SUCCESS);
    status = PreferredLanguage::AddPreferredLanguage(language, 3);
    EXPECT_EQ(status, I18nErrorCode::SUCCESS);
    PreferredLanguage::AddPreferredLanguage(fakeLanguage, -1);
    PreferredLanguage::AddPreferredLanguage(language, -1);
    std::vector<std::string> list = PreferredLanguage::GetPreferredLanguageList();
    EXPECT_EQ(list.size(), 2);
    std::string firstPreferredLanguage = PreferredLanguage::GetFirstPreferredLanguage();
    EXPECT_EQ(firstPreferredLanguage, "zh-Hans");
    std::string preferredLocale = PreferredLanguage::GetPreferredLocale();
    EXPECT_EQ(preferredLocale, "zh-CN");
    status = PreferredLanguage::RemovePreferredLanguage(0);
    EXPECT_EQ(status, I18nErrorCode::SUCCESS);
    // restore environment
    status = LocaleConfig::SetSystemLanguage("zh-Hans");
    EXPECT_EQ(status, I18nErrorCode::SUCCESS);
    preferredLanguageList = PreferredLanguage::GetPreferredLanguageList();
    for (auto i = preferredLanguageList.size() - 1; i > 0; --i) {
        status = PreferredLanguage::RemovePreferredLanguage(i);
        EXPECT_EQ(status, I18nErrorCode::SUCCESS);
    }
}

/**
 * @tc.name: IntlFuncTest0051
 * @tc.desc: Test Intl I18nCalendar
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest, IntlFuncTest0051, TestSize.Level1)
{
    I18nCalendar *calendar = new I18nCalendar("zh-Hans-CN");
    I18nCalendar *calendar2 = new I18nCalendar("zh-Hans-CN", CalendarType::CHINESE);
    calendar->SetTime(1684742124645);
    calendar->Set(1989, 5, 23);
    calendar->SetTimeZone("Asia/Shanghai");
    std::string tzId = calendar->GetTimeZone();
    EXPECT_EQ(tzId, "Asia/Shanghai");
    int32_t minimalDaysInFirstWeek = calendar->GetMinimalDaysInFirstWeek();
    EXPECT_EQ(minimalDaysInFirstWeek, 1);
    int32_t firstDayOfWeek = calendar->GetFirstDayOfWeek();
    EXPECT_EQ(firstDayOfWeek, 1);
    calendar2->Set(2023, 5, 28);
    bool isWeekend = calendar2->IsWeekend();
    EXPECT_TRUE(isWeekend);
    I18nCalendar *calendarFake = new I18nCalendar("123");
    I18nCalendar *calendarInvalid = new I18nCalendar("123", CalendarType::CHINESE);
    delete calendarFake;
    delete calendarInvalid;
    delete calendar;
    delete calendar2;
}

/**
 * @tc.name: IntlFuncTest0052
 * @tc.desc: Test Intl IndexUtil
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest, IntlFuncTest0052, TestSize.Level1)
{
    IndexUtil *indexUtil = new IndexUtil("zh-CN");
    std::vector<std::string> indexList = indexUtil->GetIndexList();
    EXPECT_EQ(indexList.size(), 28);
    indexUtil->AddLocale("en-US");
    std::string indexStr = indexUtil->GetIndex("A");
    EXPECT_EQ(indexStr, "A");
    IndexUtil *indexUtil2 = new IndexUtil("");
    IndexUtil *idxUtil = new IndexUtil("@@#");
    idxUtil->GetIndexList();
    idxUtil->GetIndex("**");
    delete indexUtil;
    delete indexUtil2;
    delete idxUtil;

    IndexUtil indexUtil3("");
    indexUtil3.AddLocale("en-US");
    indexStr = indexUtil3.GetIndex("A");
    EXPECT_EQ(indexStr, "A");
}

/**
 * @tc.name: IntlFuncTest0053
 * @tc.desc: Test Intl PhoneNumberFormat
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest, IntlFuncTest0053, TestSize.Level1)
{
    map<string, string> options = {
        { "type", "NATIONAL" }
    };
    std::unique_ptr<PhoneNumberFormat> phoneNumberFormat =
        std::make_unique<PhoneNumberFormat>("zh-CN", options);
    std::string location = phoneNumberFormat->getLocationName("13228901234", "en-US");
    EXPECT_EQ(location, "Lhasa, Xizang");
    std::string location2 = phoneNumberFormat->getLocationName("15156712345", "zh-CN");
    EXPECT_EQ(location2, "安徽省亳州市");
    std::string location3 = phoneNumberFormat->getLocationName("17673241234", "zh-CN");
    EXPECT_EQ(location3, "湖南省株洲市");
    bool flag = phoneNumberFormat->isValidPhoneNumber("+8618622350085");
    EXPECT_TRUE(flag);
    std::string number2 = "+8618622350085";
    std::string formatResult = phoneNumberFormat->format(number2);
    EXPECT_EQ(formatResult, "186 2235 0085");
    std::string location4 = phoneNumberFormat->getLocationName("fake-number", "zh-CN");
    EXPECT_EQ(location4, "");
}

/**
 * @tc.name: IntlFuncTest0054
 * @tc.desc: Test Intl TabooUtils
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest, IntlFuncTest0054, TestSize.Level1)
{
    TabooUtils *tabooUtils = new TabooUtils();
    std::string res1 = tabooUtils->ReplaceCountryName("CN", "en", "China");
    EXPECT_EQ(res1, "China");
    std::string res2 = tabooUtils->ReplaceLanguageName("zh", "en", "chinese");
    EXPECT_EQ(res2, "chinese");
    std::string res3 = tabooUtils->ReplaceCountryName("TW", "zh-Hans", "中国台湾");
    EXPECT_EQ(res3, "中国台湾");
    delete tabooUtils;
    Taboo* taboo = new Taboo();
    delete taboo;
}

/**
 * @tc.name: IntlFuncTest0055
 * @tc.desc: Test Intl LocaleCompare
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest, IntlFuncTest0055, TestSize.Level1)
{
    int32_t result = LocaleCompare::Compare("zh-CN", "zh-Hans-CN");
    EXPECT_EQ(result, 9);
    I18nBreakIterator *i18nBreakIterator = new I18nBreakIterator("zh-Hans-CN");
    bool isBoundary = i18nBreakIterator->IsBoundary(6);
    EXPECT_TRUE(!isBoundary);
    int32_t current = i18nBreakIterator->Current();
    EXPECT_EQ(current, 0);
    int32_t first = i18nBreakIterator->First();
    EXPECT_EQ(first, 0);
    int32_t last = i18nBreakIterator->Last();
    EXPECT_EQ(last, 0);
    int32_t previous = i18nBreakIterator->Previous();
    EXPECT_EQ(previous, -1);
    int32_t next6 = i18nBreakIterator->Next(6);
    EXPECT_EQ(next6, -1);
    int32_t resultLatn = LocaleCompare::Compare("en-Latn-US", "en-Qaag-US");
    EXPECT_EQ(resultLatn, 9);
    int32_t resultTl = LocaleCompare::Compare("tl-PH", "fil-PH");
    EXPECT_EQ(resultTl, 9);
    int32_t resultFil = LocaleCompare::Compare("fil-PH", "tl-PH");
    EXPECT_EQ(resultFil, 9);
    int32_t resultQaag = LocaleCompare::Compare("en-Qaag-US", "en-Latn-US");
    EXPECT_EQ(resultQaag, 9);
    int32_t resultHashMapZh = LocaleCompare::Compare("zh-MO", "zh-Hant-HK");
    EXPECT_EQ(resultHashMapZh, 8);
    int32_t resultZh = LocaleCompare::Compare("zh-Hant-MO", "zh-Hant-HK");
    EXPECT_EQ(resultZh, 8);
    int32_t resultHashMapEn = LocaleCompare::Compare("en-WS", "en-001");
    EXPECT_EQ(resultHashMapEn, 8);
    int32_t resultHashEn = LocaleCompare::Compare("en-Latn-WS", "en-001");
    EXPECT_EQ(resultHashEn, 8);
    int32_t resultHashQaagEn = LocaleCompare::Compare("en-Qaag-WS", "en-001");
    EXPECT_EQ(resultHashQaagEn, 8);
    I18nBreakIterator *breakIterator = new I18nBreakIterator("2--**");
    breakIterator->Current();
    breakIterator->First();
    breakIterator->Last();
    breakIterator->Previous();
    breakIterator->Next(6);
    breakIterator->Next();
    breakIterator->Following(0);
    breakIterator->IsBoundary(6);
    delete breakIterator;
    delete i18nBreakIterator;
}

/**
 * @tc.name: IntlFuncTest0056
 * @tc.desc: Test Intl SystemLocaleManager
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest, IntlFuncTest0056, TestSize.Level1)
{
    SystemLocaleManager *systemLocaleManager = new SystemLocaleManager();
    std::vector<std::string> languages = {"en", "de", "es", "fr"};
    SortOptions sortOptions = {"en-US", true, true};
    I18nErrorCode status;
    std::vector<LocaleItem> languageInfos = systemLocaleManager->GetLanguageInfoArray(languages, sortOptions, status);
    EXPECT_EQ(languageInfos.size(), 4);
    const std::vector<std::string> countries = {"US", "GB", "DE", "CA"};
    std::vector<LocaleItem> countryInfos = systemLocaleManager->GetCountryInfoArray(countries, sortOptions, status);
    EXPECT_EQ(countryInfos.size(), 4);
    std::vector<TimeZoneCityItem> timezoneCityItemList = SystemLocaleManager::GetTimezoneCityInfoArray(status);
    EXPECT_TRUE(timezoneCityItemList.size() > 0);
    delete systemLocaleManager;
}

/**
 * @tc.name: IntlFuncTest0057
 * @tc.desc: Test Intl Utils
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest, IntlFuncTest0057, TestSize.Level1)
{
    bool isDigit = IsDigit("55");
    EXPECT_TRUE(isDigit);
    bool isSpaceChar = IsSpaceChar(" ");
    EXPECT_TRUE(isSpaceChar);
    bool isWhiteSpace = IsWhiteSpace("  ");
    EXPECT_TRUE(isWhiteSpace);
    bool isRTLCharacter = IsRTLCharacter("^");
    EXPECT_TRUE(!isRTLCharacter);
    isRTLCharacter = IsRTLCharacter("\u0645");
    EXPECT_TRUE(isRTLCharacter);
    bool isIdeoGraphic = IsIdeoGraphic("&&*");
    EXPECT_TRUE(!isIdeoGraphic);
    bool isLetter = IsLetter("cccUt");
    EXPECT_TRUE(isLetter);
    bool isLowerCase = IsLowerCase("abc");
    EXPECT_TRUE(isLowerCase);
    bool isUpperCase = IsUpperCase("AbC");
    EXPECT_TRUE(isUpperCase);
    std::string getType = GetType("$$%");
    EXPECT_EQ(getType, "U_CURRENCY_SYMBOL");
}

/**
 * @tc.name: IntlFuncTest0058
 * @tc.desc: Test Intl MeasureData
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest, IntlFuncTest0058, TestSize.Level1)
{
    std::string timezoneId = "Asia/Shanghai";
    I18nTimeZone *i18nTimeZone = new I18nTimeZone(timezoneId, true);
    int32_t offset = i18nTimeZone->GetOffset(1684742124645);
    EXPECT_EQ(offset, 28800000);
    int32_t rawOffset = i18nTimeZone->GetRawOffset();
    EXPECT_EQ(rawOffset, 28800000);
    std::string tzId = i18nTimeZone->GetID();
    EXPECT_EQ(tzId, "Asia/Shanghai");
    std::string displayName = i18nTimeZone->GetDisplayName();
    EXPECT_EQ(displayName, "中国标准时间");
    std::string displayName2 = i18nTimeZone->GetDisplayName(true);
    EXPECT_EQ(displayName2, "中国标准时间");
    std::string zhCn = "zh-CN";
    std::string displayNameCn = i18nTimeZone->GetDisplayName(zhCn);
    EXPECT_EQ(displayNameCn, "中国标准时间");
    std::string displayName4 = i18nTimeZone->GetDisplayName("zh-CN", true);
    EXPECT_EQ(displayName4, "中国标准时间");
    std::string cityId = "Shanghai";
    std::string localeId = "en-US";
    std::string cityDisplayName = I18nTimeZone::GetCityDisplayName(cityId, localeId);
    EXPECT_EQ(cityDisplayName, "Shanghai (China)");
    std::unique_ptr<I18nTimeZone> timezone = I18nTimeZone::CreateInstance(timezoneId, true);
    I18nErrorCode errorCode = I18nErrorCode::SUCCESS;
    std::set<std::string> set0 = I18nTimeZone::GetAvailableIDs(errorCode);
    EXPECT_EQ(set0.size(), 442);
    std::set<std::string> set1 = I18nTimeZone::GetAvailableZoneCityIDs();
    EXPECT_TRUE(set1.size() > 0);
    std::string empty = "";
    std::string fakeCityId = "fake cityId";
    I18nTimeZone *i18nTimeZoneEmpty = new I18nTimeZone(empty, true);
    I18nTimeZone *i18nTimeZoneFake = new I18nTimeZone(fakeCityId, false);
    I18nTimeZone *i18nTimeZoneCityId = new I18nTimeZone(cityId, false);
    delete i18nTimeZoneEmpty;
    delete i18nTimeZoneFake;
    delete i18nTimeZoneCityId;
    delete i18nTimeZone;
    uint32_t mask = GetMask("CN");
    EXPECT_EQ(mask, 2462);
}

/**
 * @tc.name: IntlFuncTest0059
 * @tc.desc: Test Intl NumberFormat
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest, IntlFuncTest0059, TestSize.Level1)
{
    std::vector<std::string> localeTags = { "$$@@#" };
    std::map<std::string, std::string> configs = {
        {"style", "unit"}, {"unit", "fake unit"}
    };
    NumberFormat formatter(localeTags, configs);
    std::string res = formatter.Format(12);
    EXPECT_EQ(res, "12");
    localeTags = { "zh-Hans-u-nu-latn" };
    NumberFormat* format = new (std::nothrow) NumberFormat(localeTags, configs);
    EXPECT_EQ(res, format->Format(12));
    delete format;
}

/**
 * @tc.name: IntlFuncTest0060
 * @tc.desc: Test Intl HolidayManager
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest, IntlFuncTest0060, TestSize.Level1)
{
    IcsFileWriter icsFileWriter;
    std::string path = icsFileWriter.GenerateFile();
    HolidayManager *holiday_manager = new HolidayManager(path.c_str());
    std::map<std::string, std::vector<HolidayInfoItem>> holidayDataMap;
    std::vector<HolidayInfoItem> infoList;
    std::vector<HolidayLocalName> localNameList1;
    localNameList1.push_back({"tr", "Kurban Bayrami Tatili"});
    std::vector<HolidayLocalName> localNameList2;
    localNameList2.push_back({"tr", "Kurban Bayrami 2. Günü"});
    HolidayInfoItem item1 = {"Sacrifice Feast Holiday", 2022, 6, 25, localNameList1};
    HolidayInfoItem item2 = {"The Second Day of Sacrifice Feast", 2022, 6, 25, localNameList2};
    infoList.push_back(item1);
    infoList.push_back(item2);
    holidayDataMap.insert({"20220625", infoList});
    holiday_manager->SetHolidayData(holidayDataMap);
    std::vector<HolidayInfoItem> list = holiday_manager->GetHolidayInfoItemArray(2022);
    EXPECT_EQ(2, list.size());
    list = holiday_manager->GetHolidayInfoItemArray();
    EXPECT_EQ(0, list.size());
    bool flag = holiday_manager->IsHoliday(2022, 6, 25);
    EXPECT_TRUE(flag);
    flag = holiday_manager->IsHoliday();
    EXPECT_TRUE(!flag);
    delete holiday_manager;
}

/**
 * @tc.name: IntlFuncTest0061
 * @tc.desc: Test Intl NumberFormat.format
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest, IntlFuncTest0061, TestSize.Level1)
{
    string locale = "en-CN";
    string expects = "123K";
    vector<string> locales;
    locales.push_back(locale);
    string style = "decimal";
    map<string, string> options = { { "style", style },
                                    { "notation", "compact" } };
    NumberFormat *numFmt = new (std::nothrow) NumberFormat(locales, options);
    if (!numFmt) {
        EXPECT_TRUE(false);
        return;
    }
    string out = numFmt->Format(123456.789);
    EXPECT_EQ(out, expects);
    EXPECT_EQ(numFmt->GetStyle(), style);
    delete numFmt;
}

/**
 * @tc.name: IntlFuncTest0062
 * @tc.desc: Test Intl utils.cpp
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest, IntlFuncTest0062, TestSize.Level1)
{
    string emptyStr = "";
    string sep = ";";
    vector<string> dest;
    std::unordered_set<std::string> allValidLocalesLanguageTag;
    Split(emptyStr, sep, dest);
    int32_t status = 0;
    string numberStr = "12345678901234567890123456789012345678901234567890987654321";
    ConvertString2Int(numberStr, status);
    numberStr = "@#";
    ConvertString2Int(numberStr, status);
    icu::Locale locale("$$$$5%%%");
    bool isValid = IsValidLocaleTag(locale);
    EXPECT_TRUE(!isValid);
    GetAllValidLocalesTag(allValidLocalesLanguageTag);
    EXPECT_TRUE(allValidLocalesLanguageTag.size() == 0);
    const std::string str = "zh_Hans_CN";
    const std::string target = "-";
    const std::string replace = "";
    StrReplaceAll(str, target, replace);

    std::string localeRule = "zh-Hans";
    DateTimeRule* dateTimeRule = new DateTimeRule(localeRule);
    DateTimeFilter* dateTimeFilter = new DateTimeFilter(localeRule, dateTimeRule);
    delete dateTimeFilter;
    delete dateTimeRule;
}

/**
 * @tc.name: IntlFuncTest0063
 * @tc.desc: Test Intl RegexRule
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest, IntlFuncTest0063, TestSize.Level1)
{
    using namespace i18n::phonenumbers;
    string regexStr = "[a-z]1?";
    icu::UnicodeString regex(regexStr.c_str());
    std::string isValidType = "PrexxxxSuf";
    std::string handleType = "EndWithMobile";
    std::string insensitive = "False";
    std::string type = "xxx";
    RegexRule* regexRule = new RegexRule(regex, isValidType, handleType, insensitive, type);
    handleType = "fake";
    isValidType = "Code";
    RegexRule* regexRule2 = new RegexRule(regex, isValidType, handleType, insensitive, type);
    icu::UnicodeString message(type.c_str());

    i18n::phonenumbers::PhoneNumber phoneNumber;
    PhoneNumberUtil* phoneNumberUtil = i18n::phonenumbers::PhoneNumberUtil::GetInstance();
    std::string input = "[010111111111111;ext=0782";
    std::string country = "CN";
    size_t size = input.length();
    phoneNumberUtil->ParseAndKeepRawInput(input, country, &phoneNumber);
    PhoneNumberMatch* possibleNumber = new PhoneNumberMatch(size, input, phoneNumber);
    PhoneNumberMatch* phoneNumberMatch = regexRule2->IsValid(possibleNumber, message);
    EXPECT_TRUE(phoneNumberMatch != nullptr);
    delete phoneNumberMatch;
    delete regexRule;
    delete regexRule2;
}

/**
 * @tc.name: IntlFuncTest0064
 * @tc.desc: Test number format default parameter
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest, IntlFuncTest0064, TestSize.Level1)
{
    int bufferLen = 10;
    char value[bufferLen];
    vector<string> locales;
    locales.push_back("en-GB");
    int code = GetParameter("const.product.devicetype", "", value, bufferLen);
    std::string deviceType;
    if (code > 0) {
        deviceType = value;
    }
    map<string, string> unitOptions = {
        { "style", "unit" },
        { "unit", "hectare" }
    };
    NumberFormat *unitFormatter = new NumberFormat(locales, unitOptions);
    string unitRes = unitFormatter->Format(123);
    map<string, string> currencyOptions = {
        { "style", "currency" },
        { "currency", "USD" }
    };
    NumberFormat *currencyFormatter = new NumberFormat(locales, currencyOptions);
    string currencyRes = currencyFormatter->Format(123);
    if (deviceType == "wearable" || deviceType == "liteWearable" || deviceType == "watch") {
        EXPECT_EQ(currencyRes, "$123.00");
        EXPECT_EQ(unitRes, "123ha");
    } else if (deviceType == "tablet" || deviceType == "2in1" || deviceType == "tv" || deviceType == "pc") {
        EXPECT_EQ(currencyRes, "US$123.00");
        EXPECT_EQ(unitRes, "123 hectares");
    } else {
        EXPECT_EQ(currencyRes, "US$123.00");
        EXPECT_EQ(unitRes, "123 ha");
    }
    delete unitFormatter;
    delete currencyFormatter;
}

/**
 * @tc.name: IntlFuncTest0065
 * @tc.desc: Test relative time format default parameter
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest, IntlFuncTest0065, TestSize.Level1)
{
    int bufferLen = 10;
    char value[bufferLen];
    vector<string> locales;
    locales.push_back("fr-FR");
    int code = GetParameter("const.product.devicetype", "", value, bufferLen);
    std::string deviceType;
    if (code > 0) {
        deviceType = value;
    }
    map<string, string> options;
    RelativeTimeFormat *formatter = new RelativeTimeFormat(locales, options);

    double number = -1;
    string unit = "day";
    string res = formatter->Format(number, unit);
    if (deviceType == "wearable" || deviceType == "liteWearable" || deviceType == "watch") {
        EXPECT_EQ(res, "-1 j");
    } else if (deviceType == "tablet" || deviceType == "2in1" || deviceType == "tv" || deviceType == "pc") {
        EXPECT_EQ(res, "il y a 1 jour");
    } else {
        EXPECT_EQ(res, "il y a 1 jour");
    }
    delete formatter;
}

/**
 * @tc.name: IntlFuncTest0066
 * @tc.desc: Test datetime format default parameter
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest, IntlFuncTest0066, TestSize.Level1)
{
    int bufferLen = 10;
    char value[bufferLen];
    vector<string> locales;
    locales.push_back("en-GB");
    int code = GetParameter("const.product.devicetype", "", value, bufferLen);
    std::string deviceType;
    if (code > 0) {
        deviceType = value;
    }
    int64_t milliseconds = 987654321;

    map<string, string> defaultOptions;
    std::unique_ptr<DateTimeFormat> defaultFormatter = DateTimeFormat::CreateInstance(locales, defaultOptions);
    string defaultRes = defaultFormatter->Format(milliseconds);

    map<string, string> dateOptions = {
        { "dateStyle", "auto" }
    };
    std::unique_ptr<DateTimeFormat> dateFormatter = DateTimeFormat::CreateInstance(locales, dateOptions);
    string dateRes = dateFormatter->Format(milliseconds);

    map<string, string> timeOptions = {
        { "timeStyle", "auto" }
    };
    std::unique_ptr<DateTimeFormat> timeFormatter = DateTimeFormat::CreateInstance(locales, timeOptions);
    string timeRes = timeFormatter->Format(milliseconds);

    if (deviceType == "wearable" || deviceType == "liteWearable" || deviceType == "watch") {
        EXPECT_EQ(defaultRes, "12/01/1970");
        EXPECT_EQ(dateRes, "12/01/1970");
        EXPECT_EQ(timeRes, "06:20");
    } else if (deviceType == "tablet" || deviceType == "2in1" || deviceType == "tv" || deviceType == "pc") {
        EXPECT_EQ(defaultRes, "12 Jan 1970");
        EXPECT_EQ(dateRes, "12 Jan 1970");
        EXPECT_EQ(timeRes, "06:20:54");
    } else {
        EXPECT_EQ(defaultRes, "12/01/1970");
        EXPECT_EQ(dateRes, "12/01/1970");
        EXPECT_EQ(timeRes, "06:20");
    }
}

/**
 * @tc.name: IntlFuncTest0067
 * @tc.desc: Test datetime format default parameter
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest, IntlFuncTest0067, TestSize.Level1)
{
    int bufferLen = 10;
    char value[bufferLen];
    vector<string> locales;
    locales.push_back("en-GB");
    int code = GetParameter("const.product.devicetype", "", value, bufferLen);
    std::string deviceType;
    if (code > 0) {
        deviceType = value;
    }
    int64_t milliseconds = 987654321;

    map<string, string> monthOptions = {
        { "month", "auto" }
    };
    std::unique_ptr<DateTimeFormat> monthFormatter = DateTimeFormat::CreateInstance(locales, monthOptions);
    string monthRes = monthFormatter->Format(milliseconds);

    map<string, string> weekdayOptions = {
        { "weekday", "auto" }
    };
    std::unique_ptr<DateTimeFormat> weekdayFormatter = DateTimeFormat::CreateInstance(locales, weekdayOptions);
    string weekdayRes = weekdayFormatter->Format(milliseconds);

    map<string, string> eraOptions = {
        { "year", "2-digit" },
        { "era", "auto" }
    };
    std::unique_ptr<DateTimeFormat> eraFormatter = DateTimeFormat::CreateInstance(locales, eraOptions);
    string eraRes = eraFormatter->Format(milliseconds);

    if (deviceType == "wearable" || deviceType == "liteWearable" || deviceType == "watch") {
        EXPECT_EQ(monthRes, "Jan");
        EXPECT_EQ(weekdayRes, "Mon");
        EXPECT_EQ(eraRes, "70 A");
    } else if (deviceType == "tablet" || deviceType == "2in1" || deviceType == "tv" || deviceType == "pc") {
        EXPECT_EQ(monthRes, "January");
        EXPECT_EQ(weekdayRes, "Monday");
        EXPECT_EQ(eraRes, "70 Anno Domini");
    } else {
        EXPECT_EQ(monthRes, "Jan");
        EXPECT_EQ(weekdayRes, "Mon");
        EXPECT_EQ(eraRes, "70 AD");
    }
}

/**
 * @tc.name: IntlFuncTest0068
 * @tc.desc: Test datetime format default parameter
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest, IntlFuncTest0068, TestSize.Level1)
{
    int bufferLen = 10;
    char value[bufferLen];
    vector<string> locales;
    locales.push_back("fr-FR");
    int code = GetParameter("const.product.devicetype", "", value, bufferLen);
    std::string deviceType;
    if (code > 0) {
        deviceType = value;
    }
    int64_t milliseconds = 987654321;

    map<string, string> dayPeriodOptions = {
        { "hour", "numeric" },
        { "hourCycle", "h12" },
        { "dayPeriod", "auto" },
        { "timeZone", "UTC" }
    };
    std::unique_ptr<DateTimeFormat> dayPeriodFormatter = DateTimeFormat::CreateInstance(locales, dayPeriodOptions);
    string dayPeriodRes = dayPeriodFormatter->Format(milliseconds);

    map<string, string> timeZoneNameOptions = {
        { "hour", "2-digit" },
        { "timeZoneName", "auto" }
    };
    std::unique_ptr<DateTimeFormat> timeZoneNameFormatter
        = DateTimeFormat::CreateInstance(locales, timeZoneNameOptions);
    string timeZoneNameRes = timeZoneNameFormatter->Format(milliseconds);

    if (deviceType == "wearable" || deviceType == "liteWearable" || deviceType == "watch") {
        EXPECT_EQ(dayPeriodRes, "10 mat.");
        EXPECT_EQ(timeZoneNameRes, "6\xE2\x80\xAFPM UTC+8");
    } else if (deviceType == "tablet" || deviceType == "2in1" || deviceType == "tv" || deviceType == "pc") {
        EXPECT_EQ(dayPeriodRes, "10\xE2\x80\xAF" "du matin");
        EXPECT_EQ(timeZoneNameRes, "6\xE2\x80\xAFPM heure normale de la Chine");
    } else {
        EXPECT_EQ(dayPeriodRes, "10\xE2\x80\xAFmatin");
        EXPECT_EQ(timeZoneNameRes, "6\xE2\x80\xAFPM UTC+8");
    }
}

/**
 * @tc.name: IntlFuncTest0069
 * @tc.desc: Test number format unitUsage
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest, IntlFuncTest0069, TestSize.Level1)
{
    std::string locale = "en-GB";
    std::vector<std::string> locales;
    locales.push_back(locale);
    map<string, string> options = { { "style", "unit" },
                                    { "unit", "day" },
                                    { "unitUsage", "elapsed-time-second"} };
    NumberFormat *numFmt = new (std::nothrow) NumberFormat(locales, options);
    if (!numFmt) {
        EXPECT_TRUE(false);
        return;
    }
    std::string nowRes = numFmt->Format(0.00001);
    std::string secondRes = numFmt->Format(0.00004);
    std::string dayRes = numFmt->Format(1.5);
    std::string monthRes = numFmt->Format(-62.5);

    EXPECT_EQ(nowRes, "now");
    EXPECT_EQ(secondRes, "3 seconds ago");
    EXPECT_EQ(dayRes, "yesterday");
    EXPECT_EQ(monthRes, "2 months ago");
    delete numFmt;
}

/**
 * @tc.name: IntlFuncTest0070
 * @tc.desc: Test number format unitUsage
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest, IntlFuncTest0070, TestSize.Level1)
{
    std::string locale = "en-GB";
    std::vector<std::string> locales;
    locales.push_back(locale);
    map<string, string> options = { { "style", "unit" },
                                    { "unit", "megabyte" },
                                    { "unitUsage", "size-file-byte"} };
    NumberFormat *numFmt = new (std::nothrow) NumberFormat(locales, options);
    if (!numFmt) {
        EXPECT_TRUE(false);
        return;
    }
    std::string byteRes = numFmt->Format(0.00000812);
    std::string kbRes = numFmt->Format(0.125);
    std::string mbRes = numFmt->Format(3.5);
    std::string gbRes = numFmt->Format(23122);

    EXPECT_EQ(byteRes, "8 byte");
    EXPECT_EQ(kbRes, "125 kB");
    EXPECT_EQ(mbRes, "3.50 MB");
    EXPECT_EQ(gbRes, "23.12 GB");
    delete numFmt;
}

/**
 * @tc.name: IntlFuncTest0071
 * @tc.desc: Test number format unitUsage
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest, IntlFuncTest0071, TestSize.Level1)
{
    std::string locale = "en-GB";
    std::vector<std::string> locales;
    locales.push_back(locale);
    map<string, string> options = { { "style", "unit" },
                                    { "unit", "megabyte" },
                                    { "unitUsage", "size-shortfile-byte"} };
    NumberFormat *numFmt = new (std::nothrow) NumberFormat(locales, options);
    if (!numFmt) {
        EXPECT_TRUE(false);
        return;
    }
    std::string byteRes = numFmt->Format(0.00000812);
    std::string kbRes = numFmt->Format(0.125);
    std::string mbRes = numFmt->Format(3.5);
    std::string gbRes = numFmt->Format(23122);

    EXPECT_EQ(byteRes, "8 byte");
    EXPECT_EQ(kbRes, "125 kB");
    EXPECT_EQ(mbRes, "3.5 MB");
    EXPECT_EQ(gbRes, "23 GB");
    delete numFmt;
}

/**
 * @tc.name: IntlFuncTest0072
 * @tc.desc: Test date order
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest, IntlFuncTest0072, TestSize.Level1)
{
    std::string locale = "en-GB";
    std::string result = DateTimeSequence::GetDateOrder(locale);
    EXPECT_EQ(result, "d-LLL-y");
    locale = "zh-Hans";
    result = DateTimeSequence::GetDateOrder(locale);
    EXPECT_EQ(result, "y-L-d");
    locale = "bo-CN";
    result = DateTimeSequence::GetDateOrder(locale);
    EXPECT_EQ(result, "y-LLL-d");
    locale = "zh-Hant";
    result = DateTimeSequence::GetDateOrder(locale);
    EXPECT_EQ(result, "y-L-d");
    locale = "ug";
    result = DateTimeSequence::GetDateOrder(locale);
    EXPECT_EQ(result, "L-d-y");
}

/**
 * @tc.name: IntlFuncTest0073
 * @tc.desc: Test PhoneNumberFormat.format()
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest, IntlFuncTest0073, TestSize.Level1)
{
    map<string, string> options = {
        { "type", "TYPING" }
    };
    std::unique_ptr<PhoneNumberFormat> phoneNumFmt =
        std::make_unique<PhoneNumberFormat>("zh-CN", options);
    std::string number = "186223500";
    std::string formated = "";
    for (char c : number) {
        formated = formated + c;
        formated = phoneNumFmt->format(formated);
    }
    EXPECT_EQ(formated, "186 2235 00");
    number = "186223500102933884747757758585885858854774";
    phoneNumFmt->format(number);
    number = "(020) 6355";
    phoneNumFmt->format(number);
    number = "123";
    phoneNumFmt->format(number);
    number = "2134";
    phoneNumFmt->format(number);

    formated = "";
    number = "075576453";
    for (char c : number) {
        formated = formated + c;
        formated = phoneNumFmt->format(formated);
    }
    EXPECT_EQ(formated, "0755 7645 3");

    std::unique_ptr<PhoneNumberFormat> phoneNumFmt2 =
        std::make_unique<PhoneNumberFormat>("AD", options);
    formated = "";
    number = "7123945";
    for (char c : number) {
        if (c == '4') {
            formated = formated.substr(0, formated.length() - 1);
            formated = phoneNumFmt->format(formated);
        }
        formated = formated + c;
        formated = phoneNumFmt->format(formated);
    }
    EXPECT_EQ(formated, "712 345");
    PhoneNumberFormat::CloseDynamicHandler();
}

/**
 * @tc.name: IntlFuncTest0074
 * @tc.desc: Test LocaleMatcher
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest, IntlFuncTest0074, TestSize.Level1)
{
    const LocaleInfo* current = nullptr;
    const LocaleInfo* other = new LocaleInfo("fil");
    const LocaleInfo* other2 = nullptr;
    LocaleMatcher::IsMoreSuitable(current, other, nullptr);
    LocaleMatcher::IsMoreSuitable(current, other2, nullptr);
    const LocaleInfo* request = new LocaleInfo("en-Qaag-GB");
    const LocaleInfo* currentHE = new LocaleInfo("he");
    const LocaleInfo* otherIW = new LocaleInfo("iw");
    LocaleMatcher::IsMoreSuitable(currentHE, otherIW, request);
    const LocaleInfo* currentTL = new LocaleInfo("tl");
    LocaleMatcher::IsMoreSuitable(currentTL, other, request);
    LocaleMatcher::IsMoreSuitable(other, currentTL, request);
    const LocaleInfo* currentJI = new LocaleInfo("ji");
    const LocaleInfo* otherYI = new LocaleInfo("yi");
    LocaleMatcher::IsMoreSuitable(currentJI, otherYI, request);
    LocaleMatcher::IsMoreSuitable(otherYI, currentJI, request);
    const LocaleInfo* currentJW = new LocaleInfo("jw");
    const LocaleInfo* otherJV = new LocaleInfo("jv");
    LocaleMatcher::IsMoreSuitable(currentJW, otherJV, request);
    LocaleMatcher::IsMoreSuitable(otherJV, currentJW, request);
    const LocaleInfo* currentIN = new LocaleInfo("in-PH");
    const LocaleInfo* otherID = new LocaleInfo("id-MY");
    LocaleMatcher::IsMoreSuitable(currentIN, otherID, request);
    LocaleMatcher::IsMoreSuitable(otherID, currentIN, request);
    LocaleMatcher::IsMoreSuitable(nullptr, currentIN, request);
    LocaleMatcher::IsMoreSuitable(currentIN, nullptr, request);
    const LocaleInfo* enLatn = new LocaleInfo("en-Latn-US");
    LocaleMatcher::Match(request, enLatn);
    const LocaleInfo* otherIN = new LocaleInfo("in-MY");
    LocaleMatcher::Match(currentIN, otherIN);
    delete other;
    delete request;
    delete currentHE;
    delete otherIW;
    delete currentTL;
    delete currentJI;
    delete otherYI;
    delete currentJW;
    delete otherJV;
    delete currentIN;
    delete otherID;
    delete enLatn;
    delete otherIN;
}

/**
 * @tc.name: IntlFuncTest0075
 * @tc.desc: Test RulesEngine
 * @tc.type: FUNC
 */
HWTEST_F(IntlTest, IntlFuncTest0075, TestSize.Level1)
{
    DateTimeRule* dateTimeRule = nullptr;
    std::unordered_map<std::string, std::string> rulesMap = {};
    std::unordered_map<std::string, std::string> subRules = {};
    std::unordered_map<std::string, std::string> param = {};
    std::unordered_map<std::string, std::string> paramBackup = {};
    RulesSet rulesSet(rulesMap, subRules, param, paramBackup);
    RulesEngine rulesEngine(dateTimeRule, rulesSet);
    std::string locale = "&&%";
    DateTimeSequence::GetDateOrder(locale);
    const std::string oldWriteVersion = "1.10.23.100";
    const std::string newWriteVersion = "1.10.24.100";
    IcsFileWriter icsFileWriter;
    std::string oldFilePath = icsFileWriter.WriteVersionFile(oldWriteVersion, "old");
    std::string newFilePath = icsFileWriter.WriteVersionFile(newWriteVersion, "new");
    std::string oldVersion = SignatureVerifier::LoadFileVersion(oldFilePath);
    std::string newVersion = SignatureVerifier::LoadFileVersion(newFilePath);
    int res = SignatureVerifier::CompareVersion(oldVersion, newVersion);
    EXPECT_EQ(res, 1);
    FileExist(oldFilePath);
    trim(locale);
    const std::string destPath = "/data/log/copy.txt";
    FileCopy(oldFilePath, destPath);
    IsLegalPath(destPath);
    const std::string relativePath = "../log/copy.txt";
    IsLegalPath(relativePath);
    const char* dirPath = "/data/log";
    IsDirExist(dirPath);
}
} // namespace I18n
} // namespace Global
} // namespace OHOS