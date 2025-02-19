/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except", "in compliance with the License.
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

#include <cstddef>
#include <cstdint>
#include "rules_engine.h"
#include "rulesengine_fuzzer.h"

namespace OHOS {
    bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
    {
        using namespace Global::I18n;
        if (size < 1) {
            return false;
        }
        std::string input(reinterpret_cast<const char*>(data), size);
        std::unordered_map<std::string, std::string> rulesMap;
        std::unordered_map<std::string, std::string> subRules;
        std::unordered_map<std::string, std::string> param;
        std::unordered_map<std::string, std::string> paramBackup;
        rulesMap[input] = input;
        subRules[input] = input;
        param[input] = input;
        paramBackup[input] = input;
        RulesSet ruleSet = {rulesMap, subRules, param, paramBackup};

        DateTimeRule* dateTimeRule = new DateTimeRule(input);
        RulesEngine* rulesEngine = new RulesEngine(dateTimeRule, ruleSet);
        icu::UnicodeString message(input.c_str());
        rulesEngine->Match(message);
        RulesEngine* rulesEngine2 = new RulesEngine();
        delete dateTimeRule;
        delete rulesEngine;
        delete rulesEngine2;
        return true;
    }
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}

