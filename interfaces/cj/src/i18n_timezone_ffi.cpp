/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <cstdint>
#include <cstdlib>
#include <chrono>
#include <string>
#include <vector>
#include "i18n_hilog.h"
#include "i18n_ffi.h"
#include "i18n_struct.h"
#include "i18n_timezone_ffi.h"
#include "i18n_timezone_impl.h"
#include <utility>
 
namespace OHOS {
namespace Global {
namespace I18n {
using namespace OHOS::FFI;
using namespace OHOS::HiviewDFX;
extern "C"
{
    int64_t FfiI18nTimezoneConstructor(const char* id, bool isZoneId)
    {
        std::string zoneId(id);
        std::unique_ptr<I18nTimeZone> timezoneInstance = I18nTimeZone::CreateInstance(zoneId, isZoneId);
        if (timezoneInstance == nullptr) {
            HILOG_ERROR_I18N("Create I18nTimeZone fail");
            return -1;
        }
        auto ffiTimezoneInstance = FFIData::Create<FfiI18nTimeZone>(std::move(timezoneInstance));
        if (ffiTimezoneInstance == nullptr) {
            HILOG_ERROR_I18N("Create FfiI18nTimeZone fail");
            return -1;
        }
        return ffiTimezoneInstance->GetID();
    }

    CArrStr FfiI18nTimezoneGetTimezonesByLocation(double longitude, double latitude)
    {
        return VectorStringToCArr(I18nTimeZone::GetTimezoneIdByLocation(longitude, latitude));
    }

    char* FfiI18nTimezoneGetCityDisplayName(const char* cityID, const char* locale)
    {
        std::string zoneCityID(cityID);
        std::string zoneLocale(locale);
        return MallocCString(I18nTimeZone::GetCityDisplayName(zoneCityID, zoneLocale));
    }

    CArrStr FfiI18nTimezoneGetAvailableZoneCityIDs()
    {
        std::unordered_set<std::string> cityIDSet = I18nTimeZone::GetAvailableZoneCityIDs();
        std::vector<std::string> cityIDVec(cityIDSet.begin(), cityIDSet.end());
        return VectorStringToCArr(cityIDVec);
    }

    CArrStr FfiI18nTimezoneGetAvailableIDs()
    {
        I18nErrorCode errorCode = I18nErrorCode::SUCCESS;
        std::set<std::string> timezoneIDSet = I18nTimeZone::GetAvailableIDs(errorCode);
        if (errorCode != I18nErrorCode::SUCCESS) {
            return { nullptr, 0 };
        }
        std::vector<std::string> timezoneIDVec(timezoneIDSet.begin(), timezoneIDSet.end());
        return VectorStringToCArr(timezoneIDVec);
    }

    char* FfiI18nTimezoneGetDisplayName(int64_t remoteDataID, const char* locale, bool isDST, int32_t parameterStatus)
    {
        auto timezone = FFIData::GetData<FfiI18nTimeZone>(remoteDataID);
        if (!timezone) {
            HILOG_ERROR_I18N("The FfiI18nTimeZone instance is nullptr");
            return nullptr;
        }
        char* result;
        if (parameterStatus == 0) {
            result = timezone->getDisplayName();
        } else if (parameterStatus == 1) {  // 1 represents one string parameter.
            result = timezone->getDisplayName(std::string(locale));
        } else if (parameterStatus == 2) {  // 2 represents one boolean parameter.
            result = timezone->getDisplayName(isDST);
        } else {
            result = timezone->getDisplayName(std::string(locale), isDST);
        }
        return result;
    }

    int32_t FfiI18nTimezoneGetOffset(int64_t remoteDataID, double date, int32_t parameterStatus)
    {
        auto timezone = FFIData::GetData<FfiI18nTimeZone>(remoteDataID);
        if (!timezone) {
            HILOG_ERROR_I18N("The FfiI18nTimeZone instance is nullptr");
            return 0;
        }
        double cDate = 0;
        if (parameterStatus == 0) { // 0 reprensents date is null, use system time instead
            auto time = std::chrono::system_clock::now();
            auto sinceEpoch = time.time_since_epoch();
            auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(sinceEpoch);
            cDate = (double)millis.count();
        } else {
            cDate = date;
        }
        return timezone->getOffset(cDate);
    }

    int32_t FfiI18nTimezoneGetRawOffset(int64_t remoteDataID)
    {
        auto timezone = FFIData::GetData<FfiI18nTimeZone>(remoteDataID);
        if (!timezone) {
            HILOG_ERROR_I18N("The FfiI18nTimeZone instance is nullptr");
            return 0;
        }
        return timezone->getRawOffset();
    }

    char* FfiI18nTimezoneGetID(int64_t remoteDataID)
    {
        auto timezone = FFIData::GetData<FfiI18nTimeZone>(remoteDataID);
        if (!timezone) {
            HILOG_ERROR_I18N("The FfiI18nTimeZone instance is nullptr");
            return nullptr;
        }
        return timezone->getID();
    }
}
}  // namespace I18n
}  // namespace Global
}  // namespace OHOS