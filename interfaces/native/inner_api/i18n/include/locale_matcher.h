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

#ifndef OHOS_GLOBAL_I18N_LOCALE_MATCHER_H
#define OHOS_GLOBAL_I18N_LOCALE_MATCHER_H

#include "locale_info.h"

namespace OHOS {
namespace Global {
namespace I18n {
class LocaleMatcher {
public:
    /**
     * @brief Obtain the best matching locale from candidateLocales for the requestLocale.
     *
     * @param requestLocale Represents request matching locale.
     * @param candidateLocales Represents the list of locale to be matched.
     * @return result Represents the best matching locale.
     */
    static std::string GetBestMatchedLocale(const LocaleInfo* requestLocale,
        const std::vector<LocaleInfo*>& candidateLocales);

    /**
     * @brief compare current and other locale which is more matching with request.
     *
     * @param current Represents current resource locale.
     * @param other Represents other resource locale.
     * @param request Represents locale of the resource to be found.
     * @return int8_t Return 1 represents current locale is more matching, Returning -1 is the opposite.
     */
    static int8_t IsMoreSuitable(const LocaleInfo *current, const LocaleInfo *other, const LocaleInfo *request);

    static bool Match(const LocaleInfo *current, const LocaleInfo *other);

public:
    static uint64_t EN_GB_ENCODE;
    static uint64_t EN_QAAG_ENCODE;
    static uint64_t ZH_HANT_MO_ENCODE;
    static uint64_t ZH_HK_ENCODE;
    static uint32_t HANT_ENCODE;
    static constexpr uint64_t ROOT_LOCALE = 0x0;
    static constexpr uint16_t NULL_LANGUAGE = 0x0;
    static constexpr uint16_t NULL_REGION = 0x0;
    static constexpr uint16_t NULL_SCRIPT = 0x0;
    static constexpr uint64_t NULL_LOCALE = 0x0;
    static constexpr uint8_t TRACKPATH_ARRAY_SIZE = 5;

private:
    /**
     * @brief compare language, support new&old language code which include iw/hw, tl/fil, ji/yi, jw/jv, in/id.
     *
     * @param current locale to compare.
     * @param other locale to compare.
     * @return Return true represents two locale have same language, Return false is the opposite.
     */
    static bool CompareLanguage(const LocaleInfo *current, const LocaleInfo *other);
    static bool CompareRegionWhenLangIsNotEqual(const LocaleInfo *current, const LocaleInfo *other,
        const LocaleInfo *request);
    static int8_t CompareRegionWhenQaag(const LocaleInfo *current, const LocaleInfo *other, const LocaleInfo *request);
    static bool IsSimilarToUsEnglish(const LocaleInfo *localeInfo);

    /**
     * @brief find locale ancestors, including itself and ROOT_LOCALE.
     *
     * @param request
     * @param len
     * @param encodedLocale Represents the locale to be fallback.
     * @param result Represents the result of locale fallback.
     */
    static void FindTrackPath(const LocaleInfo *request, size_t len, uint64_t encodedLocale, uint64_t *result);

    /**
     * @brief Find region parent locale, if locale has not contain region, return ROOT_LOCALE.
     * Lookup child-parent locale tables, if tables has not contains, then clear the locale region and return it.
     *
     * @param encodedLocale locale encode
     * @return uint64_t  parent locale encode
     */
    static uint64_t SearchParentLocale(uint64_t encodedLocale, const LocaleInfo *request);
    static uint64_t AddScript(uint64_t encodedLocale, uint32_t encodedScript);
    static bool IsContainRegion(uint64_t encodedLocale);
    static uint64_t ClearRegion(uint64_t encodedLocale);

    /**
     * @brief compare current and target region, which is better for request.
     * @param current current locale
     * @param target target locale
     * @param request request locale
     * @return int8_t if current region is better than target region,return 1. if current region is equal target region,
     *         return 0. If target region is better than current region, return -1.
     */
    static int8_t CompareRegion(const LocaleInfo *current, const LocaleInfo *other, const LocaleInfo *request);
    static int8_t CompareWhenRegionIsNull(uint16_t currentEncodedRegion, uint16_t otherEncodedRegion,
        const LocaleInfo *current, const LocaleInfo *other, const LocaleInfo *request);

    /**
     * @brief find the default region of language and script.
     * first search language and script corresponding region.
     * if not found,search language corresponding region.
     *
     * @param language
     * @param script
     * @return uint16_t
     */
    static uint16_t FindDefaultRegionEncode(const char *language, const char *script);
    static int8_t AlphabeticallyCompare(const LocaleInfo *current, uint64_t currentEncodedLocale,
        const LocaleInfo *other, uint64_t otherEncodedLocale);

    /**
     * @brief find encodedLocale pos is in trackpath list.
     *
     * @param paths
     * @param len
     * @param encodedLocale
     * @return int8_t
     */
    static int8_t SearchTrackPathDistance(const uint64_t *paths, size_t len, uint64_t encodedLocale);
    static int8_t CompareDistance(uint64_t currentEncodedLocale, uint64_t otherEncodedLocale,
        const uint64_t *requestEncodedTrackPath, const LocaleInfo *request);

    /**
     * @brief find the  first locale which in target path,is also in request path.
     * return sum of the locale pos in target path and request path.
     *
     * @param requestPaths
     * @param targetPaths
     * @param len
     * @return size_t
     */
    static size_t ComputeTrackPathDistance(const uint64_t *requestPaths, const uint64_t *targetPaths, size_t len);
    static int8_t CompareDefaultRegion(const LocaleInfo *current, const LocaleInfo *other, const LocaleInfo *request);

    /**
     * @brief find locale is in typical locale.
     *
     * @param language
     * @param script
     * @param region
     * @return true
     * @return false
     */
    static bool IsDefaultLocale(const char *language, const char *script, const char *region);
    static int8_t CompareLanguageIgnoreOldNewCode(const LocaleInfo *current, const LocaleInfo *other,
        const LocaleInfo *request);
    static bool CompareScript(const LocaleInfo *current, const LocaleInfo *other);
    static uint32_t FindDefaultScriptEncode(const char *language, const char *region);
    static const constexpr uint8_t SCRIPT_OFFSET = 16;
};
} // namespace OHOS
} // namespace Global
} // namespace I18n
#endif // OHOS_GLOBAL_I18N_LOCALE_MATCHER_H