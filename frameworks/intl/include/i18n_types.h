/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

namespace OHOS {
namespace Global {
namespace I18n {
enum I18nErrorCode {
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
    INVALID_24_HOUR_CLOCK_TAG = 17
};

enum I18nNormalizerMode {
    NFC = 1,
    NFD,
    NFKC,
    NFKD
};
}
}
}
#endif