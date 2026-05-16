/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
#include "timezone_location.h"

#include "i18n_hilog.h"

#include <cstdio>
#include <unistd.h>
#include <unordered_map>
#include "utils.h"
#include <chrono>
#include "cJSON.h"

namespace OHOS {
namespace Global {
namespace I18n {
const char *TimeZoneLocation::TIMEZONE_LOCATION_PATH = "/system/usr/ohos_timezone/timezone_location.dat";
const std::vector<OceanZone> TimeZoneLocation::OCEAN_ZONES {
    {"Etc/GMT-12", 172.5, 180 },
    {"Etc/GMT-11", 157.5, 172.5 },
    {"Etc/GMT-10", 142.5, 157.5 },
    {"Etc/GMT-9", 127.5, 142.5 },
    {"Etc/GMT-8", 112.5, 127.5 },
    {"Etc/GMT-7", 97.5, 112.5 },
    {"Etc/GMT-6", 82.5, 97.5 },
    {"Etc/GMT-5", 67.5, 82.5 },
    {"Etc/GMT-4", 52.5, 67.5 },
    {"Etc/GMT-3", 37.5, 52.5 },
    {"Etc/GMT-2", 22.5, 37.5 },
    {"Etc/GMT-1", 7.5, 22.5 },
    {"Etc/GMT", -7.5, 7.5 },
    {"Etc/GMT+1", -22.5, -7.5 },
    {"Etc/GMT+2", -37.5, -22.5 },
    {"Etc/GMT+3", -52.5, -37.5 },
    {"Etc/GMT+4", -67.5, -52.5 },
    {"Etc/GMT+5", -82.5, -67.5 },
    {"Etc/GMT+6", -97.5, -82.5 },
    {"Etc/GMT+7", -112.5, -97.5 },
    {"Etc/GMT+8", -127.5, -112.5 },
    {"Etc/GMT+9", -142.5, -127.5 },
    {"Etc/GMT+10", -157.5, -142.5 },
    {"Etc/GMT+11", -172.5, -157.5 },
    {"Etc/GMT+12", -180, -172.5 },
};

TimeZoneLocation::TimeZoneLocation()
{
}

TimeZoneLocation::~TimeZoneLocation()
{
}

std::vector<Point> TimeZoneLocation::GetPointVect(int32_t* points, size_t size)
{
    std::vector<Point> result;
    size_t i = 0;
    while (i < size - 1) {
        double lon = static_cast<double>(points[i]) / 1000000.0;
        double lat = static_cast<double>(points[i + 1]) / 1000000.0;
        result.push_back({lon, lat});
        i += LOOP_STEP;
    }
    return result;
}

bool TimeZoneLocation::IsPointOnSegment(const Point& p, const Point& before, const Point& after)
{
    const double eps = 1e-9;
    double cross = (after.x - before.x) * (p.y - before.y) - (after.y - before.y) * (p.x - before.x);
    if (std::fabs(cross) > eps) {
        return false;
    }
    double minX = std::min(before.x, after.x) - eps;
    double maxX = std::max(before.x, after.x) + eps;
    double minY = std::min(before.y, after.y) - eps;
    double maxY = std::max(before.y, after.y) + eps;
    return Geq(p.x, minX) && Leq(p.x, maxX) && Geq(p.y, minY) && Leq(p.y, maxY);
}

bool TimeZoneLocation::IsPointInPolygon(const Point& p, const std::vector<Point>& polygon)
{
    int32_t polygonSize = static_cast<int32_t>(polygon.size());
    if (polygonSize < MIN_POLYGON_POINTS_COUNT) {
        return false;
    }
    for (size_t i = 0; i < polygon.size(); ++i) {
        if (IsPointOnSegment(p, polygon[i], polygon[(i + 1) % polygonSize])) {
            return true;
        }
    }
    bool inside = false;
    size_t j = polygon.size() - 1;
    for (size_t i = 0; i < polygon.size(); ++i) {
        double x0 = polygon[i].x;
        double y0 = polygon[i].y;
        double x1 = polygon[j].x;
        double y1 = polygon[j].y;
        if ((y0 > p.y) != (y1 > p.y) &&
            p.x < (x1 - x0) * (p.y - y0) / (y1 - y0) + x0) {
            inside = !inside;
        }
        j = i;
    }
    
    return inside;
}

std::vector<std::string> TimeZoneLocation::GetTimezoneAtSea(double lon)
{
    std::vector<std::string> vect;
    if (Eq(lon, MIN_LONGITUDE) || Eq(lon, MAX_LONGITUDE)) {
        vect.push_back("Etc/GMT+12");
        return vect;
    }
    for (size_t i = 0; i < OCEAN_ZONES.size(); i++) {
        OceanZone zone = OCEAN_ZONES[i];
        if (Leq(zone.left, lon) && Geq(zone.right, lon)) {
            vect.push_back(zone.tzid);
        } else if (zone.right < lon) {
            break;
        }
    }
    return vect;
}

bool TimeZoneLocation::IsPointInMultiPolygon(const std::vector<int32_t>& pointsInPolygon, double lon,
    double lat, FILE* file)
{
    bool flag = false;
    for (size_t k = 0; k < pointsInPolygon.size(); ++k) {
        if (flag) {
            if (fseek(file, pointsInPolygon[k] * POINT_BYTE_SIZE, SEEK_CUR) != 0) {
                HILOG_ERROR_I18N("IsPointInMultiPolygon: seek failed");
                return false;
            }
            continue;
        }
        int32_t pointLen = pointsInPolygon[k] * POINT_VALUE_SIZE;
        int32_t* points = static_cast<int32_t*>(malloc(sizeof(int32_t) * pointLen));
        if (points == nullptr) {
            HILOG_ERROR_I18N("IsPointInMultiPolygon: malloc memory failed");
            return false;
        }
        if (fread(points, sizeof(int32_t), pointLen, file) != static_cast<size_t>(pointLen)) {
            HILOG_ERROR_I18N("IsPointInMultiPolygon: read point Length failed");
            free(points);
            points = nullptr;
            return false;
        }
        std::vector<Point> pointList = GetPointVect(points, pointLen);
        const Point point = {lon, lat};
        if (IsPointInPolygon(point, pointList)) {
            flag = true;
        }
        free(points);
        points = nullptr;
    }
    return flag;
}

std::string TimeZoneLocation::NextSquare(QueryData& quadData, double lon, double lat)
{
    if (Geq(lat, quadData.midLat) && Geq(lon, quadData.midLon)) {
        quadData.bottom = quadData.midLat;
        quadData.left = quadData.midLon;
        return "a";
    } else if (Geq(lat, quadData.midLat) && Leq(lon, quadData.midLon)) {
        quadData.bottom = quadData.midLat;
        quadData.right = quadData.midLon;
        return "b";
    } else if (lat < quadData.midLat && lon < quadData.midLon) {
        quadData.top = quadData.midLat;
        quadData.right = quadData.midLon;
        return "c";
    } else {
        quadData.top = quadData.midLat;
        quadData.left = quadData.midLon;
        return "d";
    }
}

bool TimeZoneLocation::MatchNextKey(char* str, int len, int& curPos,
    std::string prefix, std::string nextKey)
{
    std::string key = prefix;
    size_t length = key.size();
    while (curPos < len) {
        curPos += 1;
        if (curPos >= len) {
            break;
        }
        char c = str[curPos];
        if (c == 'a' || c == 'b' || c == 'c' || c == 'd') {
            key += c;
            length += 1;
        } else if (c == '}' || c == ']') {
            length -= 1;
            key = key.substr(0, length);
        }
        if (key == prefix + nextKey) {
            return true;
        }
    }
    return false;
}

std::tuple<ParseJsonStrResult, std::string> TimeZoneLocation::GetJsonObjectFromPosition(
    const char* lookupStr, int pos, int len)
{
    if (pos + JSONITEM_BEGIN_OFFSET >= len) {
        return std::make_tuple(ParseJsonStrResult::INVALID, "");
    }
    if (lookupStr[pos + JSONITEM_BEGIN_OFFSET] == '[') {
        std::string jsonArrayStr = "";
        for (int i = pos + JSONITEM_BEGIN_OFFSET; i < len; i++) {
            jsonArrayStr += lookupStr[i];
            if (lookupStr[i] == ']') {
                break;
            }
        }
        return std::make_tuple(ParseJsonStrResult::ARRAY, jsonArrayStr);
    }
    if (lookupStr[pos + KEY_POS_P_OFFSET] == 'p' && lookupStr[pos + KEY_POS_O_OFFSET] == 'o') {
        std::string jsonObjectStr = "";
        for (int i = pos + JSONITEM_BEGIN_OFFSET; i < len; i++) {
            jsonObjectStr += lookupStr[i];
            if (lookupStr[i] == '}') {
                break;
            }
        }
        return std::make_tuple(ParseJsonStrResult::OBJECT, jsonObjectStr);
    }
    return std::make_tuple(ParseJsonStrResult::INVALID, "");
}

std::vector<std::string> TimeZoneLocation::GetTimezoneIdList(double lon, double lat,
    int32_t tzIndexCount, FILE* datFile,
    const std::vector<std::string>& tzidNames)
{
    std::vector<std::string> tzidVect;
    for (int i = 0; i < tzIndexCount; ++i) {
        int32_t tzIndex;
        if (fread(&tzIndex, sizeof(int32_t), 1, datFile) != 1) {
            HILOG_ERROR_I18N("GetTimezoneIdList: read tzid Index failed");
            return tzidVect;
        }
        if (tzIndex > static_cast<int32_t>(tzidNames.size() - 1)) {
            HILOG_ERROR_I18N("GetTimezoneIdList: tzIndex:%{public}d out of range", tzIndex);
            return tzidVect;
        }
        int32_t polygonLen;
        if (fread(&polygonLen, sizeof(int32_t), 1, datFile) != 1) {
            HILOG_ERROR_I18N("GetTimezoneIdList: read polygon length failed");
            return tzidVect;
        }
        std::vector<int32_t> pointsInPolygon;
        for (int j = 0; j < polygonLen; j++) {
            int32_t pointCount;
            if (fread(&pointCount, sizeof(int32_t), 1, datFile) != 1) {
                HILOG_ERROR_I18N("GetTimezoneIdList: read point count failed");
                return tzidVect;
            }
            pointsInPolygon.push_back(pointCount);
        }
        if (IsPointInMultiPolygon(pointsInPolygon, lon, lat, datFile)) {
            tzidVect.push_back(tzidNames[static_cast<size_t>(tzIndex)]);
        }
    }
    return tzidVect;
}

std::vector<std::string> TimeZoneLocation::ReadPolygonData(double lon, double lat,
    FILE* datFile, const std::string& jsonObjectStr,
    const std::vector<std::string>& tzidNames)
{
    std::vector<std::string> tzidList;
    if (jsonObjectStr.empty()) {
        HILOG_ERROR_I18N("ReadPolygonData: %{public}s is empty or invalid", jsonObjectStr.c_str());
        return tzidList;
    }
    cJSON *jsonObject = cJSON_Parse(jsonObjectStr.c_str());
    if (jsonObject == nullptr) {
        HILOG_ERROR_I18N("ReadPolygonData: parse json string failed");
        return tzidList;
    }
    cJSON *posPtr = cJSON_GetObjectItemCaseSensitive(jsonObject, "pos");
    if (!cJSON_IsNumber(posPtr)) {
        HILOG_ERROR_I18N("ReadPolygonData: pos is not number");
        cJSON_Delete(jsonObject);
        return tzidList;
    }
    int32_t pos = posPtr->valueint;
    cJSON_Delete(jsonObject);
    if (fseek(datFile, pos, SEEK_CUR) != 0) {
        HILOG_ERROR_I18N("ReadPolygonData: seek to position:%{public}d failed", pos);
        return tzidList;
    }
    int32_t tzIndexCount;
    if (fread(&tzIndexCount, sizeof(int32_t), 1, datFile) != 1) {
        HILOG_ERROR_I18N("ReadPolygonData: read tz Index Length failed");
        return tzidList;
    }
    tzidList = GetTimezoneIdList(lon, lat, tzIndexCount, datFile, tzidNames);
    if (tzidList.empty()) {
        tzidList = GetTimezoneAtSea(lon);
    }
    return tzidList;
}

void TimeZoneLocation::CloseFile(FILE* datFile)
{
    if (datFile == nullptr) {
        HILOG_ERROR_I18N("CloseFile: datFile is nullptr");
        return;
    }
    if (fclose(datFile) != 0) {
        HILOG_ERROR_I18N("CloseFile: close datFile failed");
    }
}

bool TimeZoneLocation::GetAllTimezoneId(std::vector<std::string>& tzidNames, FILE* datFile,
    int32_t indexCount)
{
    for (int k = 0; k < indexCount; k++) {
        char timezoneStr[TIMEZONE_ID_LENGTH];
        if (fread(timezoneStr,  sizeof(char), TIMEZONE_ID_LENGTH, datFile) != TIMEZONE_ID_LENGTH) {
            HILOG_ERROR_I18N("GetAllTimezoneId: read tzid length less then %{public}zu", TIMEZONE_ID_LENGTH);
            return false;
        }
        tzidNames.push_back(std::string(timezoneStr));
    }
    return true;
}

void TimeZoneLocation::ReadDatFile(FILE* datFile, double lon, double lat,
    const std::vector<std::string>& tzidNames, std::vector<std::string>& tzidList)
{
    int32_t lookupSize;
    if (fread(&lookupSize,  sizeof(int32_t), 1, datFile) != 1) {
        HILOG_ERROR_I18N("ReadDatFile: read lookupSize failed");
        return;
    }
    if (lookupSize < 0 || lookupSize > MAX_LOOKUP_STR_SIZE) {
        HILOG_ERROR_I18N("ReadDatFile: lookupSize exceed it's scope");
        return;
    }
    std::vector<char> charVect(lookupSize);
    char* lookupJsonStr = charVect.data();
    size_t readLookupSize = fread(lookupJsonStr, sizeof(char), lookupSize, datFile);
    if (readLookupSize != static_cast<size_t>(lookupSize)) {
        HILOG_ERROR_I18N("ReadDatFile: readLookupSize:%zu not equals lookupSize:%d",
            readLookupSize, lookupSize);
        return;
    }
    QueryData quadData{89.9999, -89.9999, -179.9999, 179.9999, 0, 0};
    int curPos = 0;
    std::string quadPos = "";
    while (curPos < lookupSize) {
        std::string nextKey = NextSquare(quadData, lon, lat);
        if (!MatchNextKey(lookupJsonStr, lookupSize, curPos, quadPos, nextKey)) {
            tzidList = GetTimezoneAtSea(lon);
            break;
        }
        quadPos += nextKey;
        std::tuple<ParseJsonStrResult, std::string> parseResult =
            GetJsonObjectFromPosition(lookupJsonStr, curPos, lookupSize);
        if (std::get<0>(parseResult) == ParseJsonStrResult::OBJECT) {
            std::string jsonObjectStr = std::get<1>(parseResult);
            tzidList = ReadPolygonData(lon, lat, datFile, jsonObjectStr, tzidNames);
            break;
        } else if (std::get<0>(parseResult) == ParseJsonStrResult::ARRAY) {
            std::string jsonArrayStr = std::get<1>(parseResult);
            ReadJsonArray(jsonArrayStr, tzidNames, tzidList);
            break;
        }
        quadData.midLat = (quadData.top + quadData.bottom) / DIVISOR_FOR_MEDIAN;
        quadData.midLon = (quadData.left + quadData.right) / DIVISOR_FOR_MEDIAN;
    }
}

void TimeZoneLocation::ReadJsonArray(const std::string& jsonArrayStr,
    const std::vector<std::string>& tzidNames, std::vector<std::string>& tzidList)
{
    if (jsonArrayStr.empty()) {
        HILOG_ERROR_I18N("ReadJsonArray: JsonArray string: %{public}s is empty or invalid",
            jsonArrayStr.c_str());
        return;
    }
    cJSON *jsonArray = cJSON_Parse(jsonArrayStr.c_str());
    if (jsonArray == nullptr) {
        HILOG_ERROR_I18N("ReadJsonArray: parse json array string failed");
        return;
    }
    if (!cJSON_IsArray(jsonArray)) {
        HILOG_ERROR_I18N("ReadJsonArray: %{public}s is not json array string", jsonArrayStr.c_str());
        cJSON_Delete(jsonArray);
        return;
    }
    int arraySize = cJSON_GetArraySize(jsonArray);
    for (int i = 0; i < arraySize; i++) {
        cJSON *item = cJSON_GetArrayItem(jsonArray, i);
        if (item == nullptr || !cJSON_IsNumber(item)) {
            HILOG_ERROR_I18N("ReadJsonArray: jsonarray item:%{public}d invalid", i);
            cJSON_Delete(jsonArray);
            return;
        }
        int32_t tzidIndex = item->valueint;
        if (tzidIndex < 0 || tzidIndex > static_cast<int32_t>(tzidNames.size() - 1)) {
            HILOG_ERROR_I18N("ReadJsonArray: tzidIndex: %{public}d out of range", tzidIndex);
            cJSON_Delete(jsonArray);
            return;
        }
        std::string tzid = tzidNames[static_cast<size_t>(tzidIndex)];
        tzidList.push_back(tzid);
    }
    cJSON_Delete(jsonArray);
}

std::vector<std::string> TimeZoneLocation::GetTimezoneByLocation(
    const double lon, const double lat)
{
    std::vector<std::string> tzidList;
    FILE* datFile = fopen(TIMEZONE_LOCATION_PATH, "rb");
    if (datFile == nullptr) {
        HILOG_ERROR_I18N("GetTimezoneByLocation: open dat file failed");
        return tzidList;
    }
    int32_t indexCount;
    if (fread(&indexCount,  sizeof(int32_t), 1, datFile) != 1) {
        HILOG_ERROR_I18N("GetTimezoneByLocation: read indexCount failed");
        CloseFile(datFile);
        return tzidList;
    }
    std::vector<std::string> tzidNames;
    if (!GetAllTimezoneId(tzidNames, datFile, indexCount)) {
        CloseFile(datFile);
        return tzidList;
    }
    ReadDatFile(datFile, lon, lat, tzidNames, tzidList);
    CloseFile(datFile);
    return tzidList;
}
} // namespace I18n
} // namespace Global
} // namespace OHOS
