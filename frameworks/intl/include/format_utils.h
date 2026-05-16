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
#ifndef OHOS_GLOBAL_I18N_FORMAT_UTILS_H
#define OHOS_GLOBAL_I18N_FORMAT_UTILS_H

#include <string>
#include <unordered_map>
#include "unicode/unum.h"

namespace OHOS {
namespace Global {
namespace I18n {
class FormatUtils {
public:
    static std::string GetNumberFieldType(const std::string &napiType,
        const int32_t fieldId, const double number);

private:
    static const std::unordered_map<UNumberFormatFields, std::string> NUMBER_FORMAT_FIELD_MAP;
};
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif