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
#ifndef GLOBAL_I18N_ADVANCED_MEASURE_FORMAT_H
#define GLOBAL_I18N_ADVANCED_MEASURE_FORMAT_H

#include <memory>
#include <string>

#include "number_format.h"

namespace OHOS {
namespace Global {
namespace I18n {
enum class UnitUsage {
    AREA_LAND_AGRICULT = 1,
    AREA_LAND_COMMERCL = 2,
    AREA_LAND_RESIDNTL = 3,
    LENGTH_PERSON = 4,
    LENGTH_PERSON_SMALL = 5,
    LENGTH_RAINFALL = 6,
    LENGTH_ROAD = 7,
    LENGTH_ROAD_SMALL = 8,
    LENGTH_SNOWFALL = 9,
    LENGTH_VEHICLE = 10,
    LENGTH_VISIBLTY = 11,
    LENGTH_VISIBLTY_SMALL = 12,
    LENGTH_PERSON_INFORMAL = 13,
    LENGTH_PERSON_SMALL_INFORMAL = 14,
    LENGTH_ROAD_INFORMAL = 15,
    SPEED_ROAD_TRAVEL = 16,
    SPEED_WIND = 17,
    TEMPERATURE_PERSON = 18,
    TEMPERATURE_WEATHER = 19,
    VOLUME_VEHICLE_FUEL = 20,
    ELAPSED_TIME_SECOND = 21,
    SIZE_FILE_BYTE = 22,
    SIZE_SHORTFILE_BYTE = 23
};

struct AdvancedMeasureFormatOptions {
    UnitUsage unitUsage;
};

class AdvancedMeasureFormat {
public:
    AdvancedMeasureFormat(std::shared_ptr<NumberFormat> numberFormat, const AdvancedMeasureFormatOptions& options);
    ~AdvancedMeasureFormat();

    std::string FormatNumber(double value);

private:
    void InitContext();
    void InitUnitUsage();

    AdvancedMeasureFormatOptions options_;
    std::shared_ptr<NumberFormat> numberFormat_ = nullptr;
};
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif