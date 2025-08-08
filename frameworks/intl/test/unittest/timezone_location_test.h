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

#ifndef OHOS_GLOBAL_I18N_TIMEZONE_LOCATION_TEST
#define OHOS_GLOBAL_I18N_TIMEZONE_LOCATION_TEST
#include <tuple>
#include <vector>

namespace OHOS {
namespace Global {
namespace I18n {
class TimeZoneLocationTest : public testing::Test {
public:
    static std::vector<std::tuple<double, double, std::vector<std::string>>> timeZoneTestcases;
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

int TimeZoneLocationTest0001(void);
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif