/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include <cstddef>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <unistd.h>
#include <fuzzer/FuzzedDataProvider.h>
#include "i18n_service_ability_client.h"
#include "i18n_types.h"
#include "accesstoken_kit.h"
#include "token_setproc.h"
#include "nativetoken_kit.h"
#include "serviceclient_fuzzer.h"
#include <chrono>
#include <thread>

namespace OHOS {
    static constexpr int32_t UID = 3013;
    static constexpr size_t MOD = 2;
    static constexpr size_t DELAY_SECONDS = 3;
    const static std::vector<Global::I18n::TemperatureType> TEMPERATURE_TYPE_VECT = {
        Global::I18n::TemperatureType::CELSIUS,
        Global::I18n::TemperatureType::FAHRENHEIT,
        Global::I18n::TemperatureType::KELVIN
    };
    const static std::vector<Global::I18n::WeekDay> WEEK_DAY_VECT = {
        Global::I18n::WeekDay::MON,
        Global::I18n::WeekDay::TUE,
        Global::I18n::WeekDay::WED,
        Global::I18n::WeekDay::THU,
        Global::I18n::WeekDay::FRI,
        Global::I18n::WeekDay::SAT,
        Global::I18n::WeekDay::SUN
    };

    void AddChangeNativeTokenTypeAndPermission()
    {
        const char* permissions[] = {
            "ohos.permission.UPDATE_CONFIGURATION"
        };
        NativeTokenInfoParams infoInstance = {
            .dcapsNum = 0,
            .permsNum = sizeof(permissions) / sizeof(permissions[0]),
            .aclsNum = 0,
            .dcaps = nullptr,
            .perms = permissions,
            .acls = nullptr,
            .aplStr = "system_basic",
        };
        infoInstance.processName = "ServiceClientFuzzTest";
        uint64_t tokenId = GetAccessTokenId(&infoInstance);
        printf("Get tokenid is %s \r\n", std::to_string(tokenId).c_str());
        int32_t setSelfErrorCode = SetSelfTokenID(tokenId);
        if (setSelfErrorCode != 0) {
            printf("SetSelfTokenID() Failed, return code is %d \r\n", setSelfErrorCode);
        }
        seteuid(0);
        int32_t ret = OHOS::Security::AccessToken::AccessTokenKit::ReloadNativeTokenInfo();
        printf("ReloadNativeTokenInfo result is %d \r\n", ret);
        setuid(UID);
    }

    void RemoveNativeTokenTypeAndPermission()
    {
        SetSelfTokenID(GetSelfTokenID());
        seteuid(0);
        OHOS::Security::AccessToken::AccessTokenKit::ReloadNativeTokenInfo();
        seteuid(UID);
    }

    void DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
    {
        using namespace Global::I18n;
        FuzzedDataProvider provider(data, size);
        std::string input = provider.ConsumeRandomLengthString();
        size_t newSize = provider.ConsumeIntegral<size_t>();
        std::this_thread::sleep_for(std::chrono::seconds(DELAY_SECONDS));
        I18nServiceAbilityClient::SetSystemLanguage(input);

        std::this_thread::sleep_for(std::chrono::seconds(DELAY_SECONDS));
        I18nServiceAbilityClient::SetSystemRegion(input);
        std::this_thread::sleep_for(std::chrono::seconds(DELAY_SECONDS));
        I18nServiceAbilityClient::SetSystemLocale(input);
        std::this_thread::sleep_for(std::chrono::seconds(DELAY_SECONDS));
        I18nServiceAbilityClient::Set24HourClock(input);
        std::this_thread::sleep_for(std::chrono::seconds(DELAY_SECONDS));
        I18nServiceAbilityClient::SetSystemCollation(input);
        std::this_thread::sleep_for(std::chrono::seconds(DELAY_SECONDS));
        I18nServiceAbilityClient::SetSystemNumberingSystem(input);
        std::this_thread::sleep_for(std::chrono::seconds(DELAY_SECONDS));
        I18nServiceAbilityClient::SetSystemNumberPattern(input);
        std::this_thread::sleep_for(std::chrono::seconds(DELAY_SECONDS));
        I18nServiceAbilityClient::SetSystemMeasurement(input);
        std::this_thread::sleep_for(std::chrono::seconds(DELAY_SECONDS));
        I18nServiceAbilityClient::SetSystemNumericalDatePattern(input);
        std::this_thread::sleep_for(std::chrono::seconds(DELAY_SECONDS));
        I18nServiceAbilityClient::SetUsingLocalDigit(newSize % MOD == 0);
        int32_t intValue = static_cast<int32_t>(newSize);
        std::this_thread::sleep_for(std::chrono::seconds(DELAY_SECONDS));
        I18nServiceAbilityClient::AddPreferredLanguage(input, intValue);
        std::this_thread::sleep_for(std::chrono::seconds(DELAY_SECONDS));
        I18nServiceAbilityClient::RemovePreferredLanguage(intValue);
        size_t temperatureType = provider.ConsumeIntegralInRange<size_t>(0, 2);
        std::this_thread::sleep_for(std::chrono::seconds(DELAY_SECONDS));
        I18nServiceAbilityClient::SetTemperatureType(TEMPERATURE_TYPE_VECT[temperatureType]);
        size_t weekDay = provider.ConsumeIntegralInRange<size_t>(0, 6);
        std::this_thread::sleep_for(std::chrono::seconds(DELAY_SECONDS));
        I18nServiceAbilityClient::SetFirstDayOfWeek(WEEK_DAY_VECT[weekDay]);

        I18nServiceAbilityClient::GetLanguageFromUserId(intValue);
        std::unordered_map<std::string, std::string> mapValue;
        I18nServiceAbilityClient::GetSystemCollations(mapValue);
        I18nServiceAbilityClient::GetUsingCollation(input);
        I18nServiceAbilityClient::GetSystemNumberingSystems(mapValue);
        I18nServiceAbilityClient::GetUsingNumberingSystem(input);
        I18nServiceAbilityClient::GetSystemNumberPatterns(mapValue);
        I18nServiceAbilityClient::GetUsingNumberPattern(input);
        I18nServiceAbilityClient::GetSystemMeasurements(mapValue);
        I18nServiceAbilityClient::GetUsingMeasurement(input);
        I18nServiceAbilityClient::GetSystemNumericalDatePatterns(mapValue);
        I18nServiceAbilityClient::GetUsingNumericalDatePattern(input);
    }
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    OHOS::AddChangeNativeTokenTypeAndPermission();
    OHOS::DoSomethingInterestingWithMyAPI(data, size);
    OHOS::RemoveNativeTokenTypeAndPermission();
    return 0;
}