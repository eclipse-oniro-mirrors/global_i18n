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

#include "accesstoken_kit.h"
#include "hilog/log.h"
#include "ipc_skeleton.h"
#include "tokenid_kit.h"
#include "i18n_service_ability_stub.h"

namespace OHOS {
namespace Global {
namespace I18n {
static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, 0xD001E00, "I18nServiceAbilityStub" };
using namespace OHOS::HiviewDFX;

I18nServiceAbilityStub::I18nServiceAbilityStub()
{
    InitInnerFuncMap();
    HiLog::Info(LABEL, "I18nServiceAbilityStub object init success.");
}

I18nServiceAbilityStub::~I18nServiceAbilityStub()
{
    HiLog::Info(LABEL, "I18nServiceAbilityStub object release.");
}

int32_t I18nServiceAbilityStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    HiLog::Info(LABEL, "I18nServiceAbilityStub::OnRemoteRequest process request for code=%{public}u", code);
    // check whether request's descriptor is consistent with sa's descriptor
    std::u16string descriptor = GetDescriptor();
    std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (descriptor != remoteDescriptor) {
        HiLog::Error(LABEL, "I18nServiceAbilityStub client and server's descriptor are inconsistent.");
        reply.WriteInt32(I18nErrorCode::INCONSISTENT_DESCRIPTOR);
        return 0;
    }
    // map request code to corresponding process function.
    int32_t ret;
    bool hasMappedInnerFunc = false;
    auto funcIter = innerFuncMap.find(code);
    if (funcIter != innerFuncMap.end()) {
        auto func = funcIter->second;
        if (func != nullptr) {
            hasMappedInnerFunc = true;
            ret = (this->*func)(data, reply);
        }
    }
    if (!hasMappedInnerFunc) {
        HiLog::Info(LABEL, "I18nServiceAbilityStub::OnRemoteRequest invalid request code=%{public}u", code);
        ret = IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
    // trigger release i18n service after one request finished.
    UnloadI18nServiceAbility();
    return ret;
}

void I18nServiceAbilityStub::InitInnerFuncMap()
{
    innerFuncMap[static_cast<uint32_t>(ILocaleConfigAbilityCode::SET_SYSTEM_LANGUAGE)] =
        &I18nServiceAbilityStub::SetSystemLanguageInner;
    innerFuncMap[static_cast<uint32_t>(ILocaleConfigAbilityCode::SET_SYSTEM_REGION)] =
        &I18nServiceAbilityStub::SetSystemRegionInner;
    innerFuncMap[static_cast<uint32_t>(ILocaleConfigAbilityCode::SET_SYSTEM_LOCALE)] =
        &I18nServiceAbilityStub::SetSystemLocaleInner;
    innerFuncMap[static_cast<uint32_t>(ILocaleConfigAbilityCode::SET_24_HOUR_CLOCK)] =
        &I18nServiceAbilityStub::Set24HourClockInner;
    innerFuncMap[static_cast<uint32_t>(ILocaleConfigAbilityCode::SET_USING_LOCAL_DIGIT)] =
        &I18nServiceAbilityStub::SetUsingLocalDigitInner;
    innerFuncMap[static_cast<uint32_t>(ILocaleConfigAbilityCode::ADD_PREFERRED_LANGUAGE)] =
        &I18nServiceAbilityStub::AddPreferredLanguageInner;
    innerFuncMap[static_cast<uint32_t>(ILocaleConfigAbilityCode::REMOVE_PREFERRED_LANGUAGE)] =
        &I18nServiceAbilityStub::RemovePreferredLanguageInner;
}

/**
 * check whether request process has correct tokenType and permission.
 */
I18nErrorCode I18nServiceAbilityStub::CheckPermission()
{
    uint64_t callerFullToken = IPCSkeleton::GetCallingFullTokenID();
    uint32_t callerToken = IPCSkeleton::GetCallingTokenID();
    bool isSystemApp = Security::AccessToken::TokenIdKit::IsSystemAppByFullTokenID(callerFullToken);
    Security::AccessToken::ATokenTypeEnum tokenType =
        Security::AccessToken::AccessTokenKit::GetTokenTypeFlag(callerToken);
    bool isShell = tokenType == Security::AccessToken::ATokenTypeEnum::TOKEN_SHELL;
    bool isNative = tokenType == Security::AccessToken::ATokenTypeEnum::TOKEN_NATIVE;
    if (!isSystemApp && !isShell && !isNative) {
        HiLog::Error(LABEL, "I18nServiceAbilityStub caller process is not System app, Shell or Native.");
        return I18nErrorCode::NO_PERMISSION;
    }
    int result = Security::AccessToken::AccessTokenKit::VerifyAccessToken(callerToken,
        "ohos.permission.UPDATE_CONFIGURATION");
    if (result != Security::AccessToken::PermissionState::PERMISSION_GRANTED) {
        HiLog::Error(LABEL, "I18nServiceAbilityStub caller process doesn't have UPDATE_CONFIGURATION permission.");
        return I18nErrorCode::NO_PERMISSION;
    }
    return I18nErrorCode::SUCCESS;
}

int32_t I18nServiceAbilityStub::SetSystemLanguageInner(MessageParcel &data, MessageParcel &reply)
{
    I18nErrorCode err = CheckPermission();
    if (err != I18nErrorCode::SUCCESS) {
        reply.WriteInt32(static_cast<int32_t>(err));
        return 0;
    }
    std::string language = data.ReadString();
    err = SetSystemLanguage(language);
    if (err != I18nErrorCode::SUCCESS) {
        HiLog::Error(LABEL, "I18nServiceAbilityStub::SetSystemLanguageInner failed with errorCode=%{public}d",
            static_cast<int32_t>(err));
    }
    reply.WriteInt32(static_cast<int32_t>(err));
    return 0;
}

int32_t I18nServiceAbilityStub::SetSystemRegionInner(MessageParcel &data, MessageParcel &reply)
{
    I18nErrorCode err = CheckPermission();
    if (err != I18nErrorCode::SUCCESS) {
        reply.WriteInt32(static_cast<int32_t>(err));
        return 0;
    }
    std::string region = data.ReadString();
    err = SetSystemRegion(region);
    if (err != I18nErrorCode::SUCCESS) {
        HiLog::Error(LABEL, "I18nServiceAbilityStub::SetSystemRegionInner failed with errorCode=%{public}d",
            static_cast<int32_t>(err));
    }
    reply.WriteInt32(static_cast<int32_t>(err));
    return 0;
}

int32_t I18nServiceAbilityStub::SetSystemLocaleInner(MessageParcel &data, MessageParcel &reply)
{
    I18nErrorCode err = CheckPermission();
    if (err != I18nErrorCode::SUCCESS) {
        reply.WriteInt32(static_cast<int32_t>(err));
        return 0;
    }
    std::string locale = data.ReadString();
    err = SetSystemLocale(locale);
    if (err != I18nErrorCode::SUCCESS) {
        HiLog::Error(LABEL, "I18nServiceAbilityStub::SetSystemLocaleInner failed with errorCode=%{public}d",
            static_cast<int32_t>(err));
    }
    reply.WriteInt32(static_cast<int32_t>(err));
    return 0;
}

int32_t I18nServiceAbilityStub::Set24HourClockInner(MessageParcel &data, MessageParcel &reply)
{
    I18nErrorCode err = CheckPermission();
    if (err != I18nErrorCode::SUCCESS) {
        reply.WriteInt32(static_cast<int32_t>(err));
        return 0;
    }
    std::string flag = data.ReadString();
    err = Set24HourClock(flag);
    if (err != I18nErrorCode::SUCCESS) {
        HiLog::Error(LABEL, "I18nServiceAbilityStub::Set24HourClockInner failed with errorCode=%{public}d",
            static_cast<int32_t>(err));
    }
    reply.WriteInt32(static_cast<int32_t>(err));
    return 0;
}

int32_t I18nServiceAbilityStub::SetUsingLocalDigitInner(MessageParcel &data, MessageParcel &reply)
{
    I18nErrorCode err = CheckPermission();
    if (err != I18nErrorCode::SUCCESS) {
        reply.WriteInt32(static_cast<int32_t>(err));
        return 0;
    }
    bool flag = data.ReadBool();
    err = SetUsingLocalDigit(flag);
    if (err != I18nErrorCode::SUCCESS) {
        HiLog::Error(LABEL, "I18nServiceAbilityStub::SetUsingLocalDigitInner failed with errorCode=%{public}d",
            static_cast<int32_t>(err));
    }
    reply.WriteInt32(static_cast<int32_t>(err));
    return 0;
}

int32_t I18nServiceAbilityStub::AddPreferredLanguageInner(MessageParcel &data, MessageParcel &reply)
{
    I18nErrorCode err = CheckPermission();
    if (err != I18nErrorCode::SUCCESS) {
        reply.WriteInt32(static_cast<int32_t>(err));
        return 0;
    }
    std::string language = data.ReadString();
    int32_t index = data.ReadInt32();
    err = AddPreferredLanguage(language, index);
    if (err != I18nErrorCode::SUCCESS) {
        HiLog::Error(LABEL, "I18nServiceAbilityStub::AddPreferredLanguageInner failed with errorCode=%{public}d",
            static_cast<int32_t>(err));
    }
    reply.WriteInt32(static_cast<int32_t>(err));
    return 0;
}

int32_t I18nServiceAbilityStub::RemovePreferredLanguageInner(MessageParcel &data, MessageParcel &reply)
{
    I18nErrorCode err = CheckPermission();
    if (err != I18nErrorCode::SUCCESS) {
        reply.WriteInt32(static_cast<int32_t>(err));
        return 0;
    }
    int32_t index = data.ReadInt32();
    err = RemovePreferredLanguage(index);
    if (err != I18nErrorCode::SUCCESS) {
        HiLog::Error(LABEL, "I18nServiceAbilityStub::RemovePreferredLanguageInner failed with errorCode=%{public}d",
            static_cast<int32_t>(err));
    }
    reply.WriteInt32(static_cast<int32_t>(err));
    return 0;
}
} // namespace I18n
} // namespace Global
} // namespace OHOS