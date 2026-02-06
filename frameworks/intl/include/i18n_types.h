/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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

#ifndef OHOS_GLOBAL_I18N_I18N_TYPES_H
#define OHOS_GLOBAL_I18N_I18N_TYPES_H

#include <string>

namespace OHOS {
namespace Global {
namespace I18n {
enum class I18nErrorCode {
    SUCCESS = 0,
    FAILED = 1,
    NO_PERMISSION = 2,
    INCONSISTENT_DESCRIPTOR = 3,
    INVALID_LANGUAGE_TAG = 4,
    INVALID_REGION_TAG = 5,
    INVALID_LOCALE_TAG = 6,
    REMOVE_PREFERRED_LANGUAGE_FAILED = 7,
    ADD_PREFERRED_LANGUAGE_NON_EXIST_FAILED = 8,
    ADD_PREFERRED_LANGUAGE_EXIST_FAILED = 9,
    UPDATE_LOCAL_DIGIT_FAILED = 10,
    UPDATE_24_HOUR_CLOCK_FAILED = 11,
    UPDATE_SYSTEM_LANGUAGE_FAILED = 12,
    UPDATE_SYSTEM_LOCALE_FAILED = 13,
    UPDATE_SYSTEM_PREFERRED_LANGUAGE_FAILED = 14,
    PUBLISH_COMMON_EVENT_FAILED = 15,
    LOAD_SA_FAILED = 16,
    INVALID_24_HOUR_CLOCK_TAG = 17,
    NOT_SYSTEM_APP = 18,
    INVALID_TEMPERATURE_TYPE = 19,
    UPDATE_TEMPERATURE_TYPE_FAILED = 20,
    INVALID_WEEK_DAY = 21,
    UPDATE_WEEK_DAY_FAILED = 22,
    INVALID_NUMBER_FORMAT_SKELETON = 23,
    INVALID_DATE_TIME_FORMAT_SKELETON = 24,
    INVALID_DATE_TIME_FORMAT_PATTERN = 25,
    INVALID_PARAM = 26,
    MISSING_PARAM = 27
};

enum I18nNormalizerMode {
    NFC = 1,
    NFD,
    NFKC,
    NFKD
};

enum TemperatureType {
    CELSIUS = 1,
    FAHRENHEIT = 2,
    KELVIN = 3,
};

enum WeekDay {
    MON = 1,
    TUE = 2,
    WED = 3,
    THU = 4,
    FRI = 5,
    SAT = 6,
    SUN = 7,
};

enum class LocaleType {
    INVALID = 0,
    BUILTINS_LOCALE = 1,
    LOCALE_INFO = 2,
};

enum class ErrorType {
    NO_ERROR,
    TYPE_ERROR,
    RANGE_ERROR
};

struct ErrorMessage {
    ErrorType type = ErrorType::NO_ERROR;
    std::string message = "";
};
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif