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
#ifndef GLOBAL_I18N_API_VERSION_H
#define GLOBAL_I18N_API_VERSION_H

#include <mutex>
#include <string>
#include <unordered_map>

namespace OHOS {
namespace Global {
namespace I18n {
enum ApiCompareResult {
    INVALID_PARAM = -2,
    SYS_ERROR = -1,
    LESS_THAN = 0,
    GREATER_THAN = 1
};

class ApiVersion {
public:
    static ApiCompareResult CheckApiTargetVersion(const std::string &apiName);

private:
    static std::unordered_map<std::string, int32_t> versionMap;
    static const int32_t API_VERSION_MOD;
    static std::mutex initMutex;

    static int32_t GetApiTargetVersion();
    static int32_t InitSystemApiVersion();
};
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif
