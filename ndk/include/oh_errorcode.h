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

/**
 * @addtogroup i18n
 * @{
 *
 * @brief The error code in i18n.
 * @since 21
 */

/**
 * @file i18n_errorcode.h
 *
 * @brief The error code in i18n.
 *
 * @library libohi18n.so
 * @kit LocalizationKit
 * @syscap SystemCapability.Global.I18n
 * @since 21
 */

#ifndef GLOBAL_OH_ERRORCODE_H
#define GLOBAL_OH_ERRORCODE_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief error codes of i18n
 * @since 22
 */
typedef enum I18n_ErrorCode {
    /** @error Success*/
    SUCCESS = 0,

    /** @error Invalid input parameter */
    ERROR_INVALID_PARAMETER = 8900001,

    /** @error Unexpected error, such as memory error. */
    UNEXPECTED_ERROR = 8900050,
} I18n_ErrorCode;

#ifdef __cplusplus
};
#endif
#endif // GLOBAL_OH_ERRORCODE_H

/** @} */