/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except", "in compliance with the License.
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
#ifndef OHOS_GLOBAL_I18N_TIMEZONE_H
#define OHOS_GLOBAL_I18N_TIMEZONE_H

#include <cstdint>
#include <map>
#include <mutex>
#include <png.h>
#include <set>
#include <string>
#include "i18n_types.h"
#include "memory"
#include "unicode/timezone.h"
#include <vector>
#include <tuple>

namespace OHOS {
namespace Global {
namespace I18n {
using GetReplacedTimezoneDisplayName = void (*)(const char*, const char*, char*);

class I18nTimeZone {
public:
    I18nTimeZone(std::string &id, bool isZoneID);
    virtual ~I18nTimeZone();
    int32_t GetOffset(double date);
    int32_t GetRawOffset();
    std::string GetID();
    std::string GetDisplayName();
    std::string GetDisplayName(bool isDST);
    std::string GetDisplayName(std::string localeStr);
    std::string GetDisplayName(std::string localeStr, bool isDST);
    virtual std::string GetDisplayNameByTaboo(const std::string& localeStr, std::string result);
    static std::unique_ptr<I18nTimeZone> CreateInstance(std::string &id, bool isZoneID);
    static std::set<std::string> GetAvailableIDs(I18nErrorCode &errorCode);
    static std::set<std::string> GetAvailableZoneCityIDs();
    static std::string GetCityDisplayName(std::string &cityID, std::string &locale);
    static std::vector<std::string> GetTimezoneIdByLocation(const double x, const double y);
    static std::map<std::string, std::string> FindCityDisplayNameMap(std::string &locale);
    static std::string GetLocaleBaseName(std::string &locale);
    static std::string GetTimezoneIdByCityId(const std::string &cityId);

private:
    static const char *TIMEZONE_KEY;
    static const char *DEFAULT_TIMEZONE;
    static const char *CITY_TIMEZONE_DATA_PATH;
    static const char *DEVICE_CITY_TIMEZONE_DATA_PATH;
    static const char *DISTRO_DEVICE_CITY_TIMEZONE_DATA_PATH;
    static const char *TZ_PIXEL_PATH;
    static const char *DISTRO_TZ_PIXEL_PATH;
    static const char *DEFAULT_LOCALE;
    static const char *CITY_DISPLAYNAME_PATH;
    static const char *BASE_DEVICE_CITY_DISPLAYNAME_PATH;
    static const char *DISTRO_DEVICE_CITY_DISPLAYNAME_PATH;
    static const char *DISTRO_ROOT_DISPLAYNAME_PATH;
    static const char *SUPPORTED_LOCALES_PATH;
    static const char *TIMEZONE_ROOT_TAG;
    static const char *TIMEZONE_SECOND_ROOT_TAG;
    static const char *CITY_DISPLAYNAME_ROOT_TAG;
    static const char *CITY_DISPLAYNAME_SECOND_ROOT_TAG;
    static const uint32_t ELEMENT_NUM = 2;
    static const uint32_t TZ_X_PLUS = 180;
    static const uint32_t TZ_HALF_OF_SIZE = 2;
    static const uint32_t TZ_MAX_PIXEL_VALUE = 255;
    static constexpr double NUM_PRECISION = 0.1;
    static std::set<std::string> supportedLocales;
    static std::set<std::string> availableZoneCityIDs;
    static std::map<std::string, std::string> city2TimeZoneID;
    static std::map<int, std::string> categoryNum2TimezoneWN;
    static std::map<int, std::string> categoryNum2TimezoneEN;
    static std::map<int, std::string> categoryNum2TimezoneWS;
    static std::map<int, std::string> categoryNum2TimezoneES;
    static std::map<std::string, std::string> BEST_MATCH_LOCALE;
    static constexpr int SYS_PARAM_LEN = 128;
    icu::TimeZone *timezone = nullptr;
    static std::mutex cityTimeZoneMutex;
    static std::mutex matchLocaleMutex;

    static bool ReadTimeZoneData(const char *xmlPath);
    static std::string ComputeLocale(std::string &locale);
    icu::TimeZone* GetTimeZone();
    static std::string FindCityDisplayNameFromXml(std::string &cityID, std::string &locale);
    static std::string GetCityDisplayNameXmlPath(const std::string &locale);
    static bool GetSupportedLocales();
    static std::string GetFallBack(std::string &localeStr);
    static void GetTimezoneIDFromZoneInfo(std::set<std::string> &availableIDs, std::string &parentPath,
        std::string &parentName);
    static std::vector<int> GetColorData(const uint16_t x, const uint16_t y,
                                         uint16_t actualHeight, std::string preferredPath);
    static int InitPngptr(png_structp &png_ptr, png_infop &info_ptr, FILE **fp, std::string preferredPath);
    static std::vector<std::string> FindTzData();
    static bool ParamExceedScope(const int x, const int y, int fixedX, int fixedY);
    static std::string GetPreferredPath(const double x, const std::vector<std::string> &filePaths);
    static void SetVersionPathMap(std::string verison, std::string path,
            std::map<std::string, std::vector<std::string>> *pathMap);
    static void GetTzDataWidth(std::vector<std::string> filePaths, uint32_t *width, uint32_t *height);
    static void CloseFile(FILE *fp);
    static bool ValidateDataIntegrity(const std::vector<std::string> &pathList);
    static bool CheckLatitudeAndLongitude(const double x, const double y);
    static std::map<int, std::string> GetTimeZoneCategoryMap(const double x, const double y);
    static void SetCity2TimeZoneID(const char* key, const char* value);
    static void SetBestMatchLocale(const std::string& key, const std::string& value);
    static const char *GetDeviceCityDisplayNamePath();
};
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif