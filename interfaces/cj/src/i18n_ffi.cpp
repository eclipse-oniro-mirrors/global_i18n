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
#include "i18n_ffi.h"
#include "i18n_struct.h"
#include "locale_config.h"
 
namespace OHOS {
namespace Global {
namespace I18n {
extern "C"
{
char* MallocCString(const std::string& origin)
{
    if (origin.empty()) {
        return nullptr;
    }
    auto length = origin.length() + 1;
    char* res = static_cast<char*>(malloc(sizeof(char) * length));
    if (res == nullptr) {
        return nullptr;
    }
    return std::char_traits<char>::copy(res, origin.c_str(), length);
}

CArrStr VectorStringToCArr(const std::vector<std::string>& vectorString)
{
    CArrStr strArray;
    strArray.length = static_cast<int64_t>(vectorString.size());
    strArray.data = static_cast<char**>(malloc(strArray.length * sizeof(char*)));
    if (strArray.data == nullptr) {
        return CArrStr{0};
    }
    for (int64_t i = 0; i < strArray.length; i++) {
        strArray.data[i] = MallocCString(vectorString[i]);
    }
    return strArray;
}

bool FfiOHOSIsRTL(char* locale)
{
    bool isRTL = LocaleConfig::IsRTL(std::string(locale));
    return isRTL;
}
}
}  // namespace I18n
}  // namespace Global
}  // namespace OHOS