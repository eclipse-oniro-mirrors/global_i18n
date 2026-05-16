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
#include "advanced_measure_format.h"

#include "i18n_hilog.h"

namespace OHOS {
namespace Global {
namespace I18n {
const std::unordered_map<UnitUsage, std::string> UNIT_USAGE_CONVERT = {
    { UnitUsage::AREA_LAND_AGRICULT, "area-land-agricult" },
    { UnitUsage::AREA_LAND_COMMERCL, "area-land-commercl" },
    { UnitUsage::AREA_LAND_RESIDNTL, "area-land-residntl" },
    { UnitUsage::LENGTH_PERSON, "length-person" },
    { UnitUsage::LENGTH_PERSON_SMALL, "length-person-small" },
    { UnitUsage::LENGTH_RAINFALL, "length-rainfall" },
    { UnitUsage::LENGTH_ROAD, "length-road" },
    { UnitUsage::LENGTH_ROAD_SMALL, "length-road-small" },
    { UnitUsage::LENGTH_SNOWFALL, "length-snowfall" },
    { UnitUsage::LENGTH_VEHICLE, "length-vehicle" },
    { UnitUsage::LENGTH_VISIBLTY, "length-visiblty" },
    { UnitUsage::LENGTH_VISIBLTY_SMALL, "length-visiblty-small" },
    { UnitUsage::LENGTH_PERSON_INFORMAL, "length-person-informal" },
    { UnitUsage::LENGTH_PERSON_SMALL_INFORMAL, "length-person-small-informal"},
    { UnitUsage::LENGTH_ROAD_INFORMAL, "length-road-informal" },
    { UnitUsage::SPEED_ROAD_TRAVEL, "speed-road-travel" },
    { UnitUsage::SPEED_WIND, "speed-wind" },
    { UnitUsage::TEMPERATURE_PERSON, "temperature-person" },
    { UnitUsage::TEMPERATURE_WEATHER, "temperature-weather" },
    { UnitUsage::VOLUME_VEHICLE_FUEL, "volume-vehicle-fuel" },
    { UnitUsage::ELAPSED_TIME_SECOND, "elapsed-time-second" },
    { UnitUsage::SIZE_FILE_BYTE, "size-file-byte" },
    { UnitUsage::SIZE_SHORTFILE_BYTE, "size-shortfile-byte" },
};

AdvancedMeasureFormat::AdvancedMeasureFormat(std::shared_ptr<NumberFormat> numberFormat,
    const AdvancedMeasureFormatOptions& options)
{
    numberFormat_ = numberFormat;
    options_ = options;
    InitContext();
}

AdvancedMeasureFormat::~AdvancedMeasureFormat()
{
}

std::string AdvancedMeasureFormat::FormatNumber(double value)
{
    if (numberFormat_ == nullptr) {
        HILOG_ERROR_I18N("AdvancedMeasureFormat::FormatNumber: numberFormat_ is nullptr.");
        return "";
    }
    return numberFormat_->Format(value);
}

void AdvancedMeasureFormat::InitContext()
{
    if (numberFormat_ == nullptr) {
        HILOG_ERROR_I18N("AdvancedMeasureFormat::InitContext: numberFormat_ is nullptr.");
        return;
    }
    InitUnitUsage();
}

void AdvancedMeasureFormat::InitUnitUsage()
{
    auto iter = UNIT_USAGE_CONVERT.find(options_.unitUsage);
    if (iter == UNIT_USAGE_CONVERT.end()) {
        return;
    }
    numberFormat_->SetUnitUsage(iter->second);
}
} // namespace I18n
} // namespace Global
} // namespace OHOS
