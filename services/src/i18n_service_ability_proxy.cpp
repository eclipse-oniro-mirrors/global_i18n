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

#include "hilog/log.h"
#include "message_parcel.h"
#include "i18n_service_ability_proxy.h"

namespace OHOS {
namespace Global {
namespace I18n {
static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, 0xD001E00, "I18nServiceAbilityProxy" };
using namespace OHOS::HiviewDFX;
static const std::u16string DESCRIPTOR = u"OHOS.global.II18nServiceAbility";

I18nServiceAbilityProxy::I18nServiceAbilityProxy(const sptr<IRemoteObject> &impl)
    : IRemoteProxy<II18nServiceAbility>(impl)
{
}

I18nErrorCode I18nServiceAbilityProxy::SetSystemLanguage(const std::string &language)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(DESCRIPTOR);
    data.WriteString(language);
    Remote()->SendRequest(static_cast<uint32_t>(ILocaleConfigAbilityCode::SET_SYSTEM_LANGUAGE), data,
        reply, option);
    return ProcessReply(reply.ReadInt32());
}

I18nErrorCode I18nServiceAbilityProxy::SetSystemRegion(const std::string &region)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(DESCRIPTOR);
    data.WriteString(region);
    Remote()->SendRequest(static_cast<uint32_t>(ILocaleConfigAbilityCode::SET_SYSTEM_REGION), data,
        reply, option);
    return ProcessReply(reply.ReadInt32());
}

I18nErrorCode I18nServiceAbilityProxy::SetSystemLocale(const std::string &locale)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(DESCRIPTOR);
    data.WriteString(locale);
    Remote()->SendRequest(static_cast<uint32_t>(ILocaleConfigAbilityCode::SET_SYSTEM_LOCALE), data,
        reply, option);
    return ProcessReply(reply.ReadInt32());
}

I18nErrorCode I18nServiceAbilityProxy::Set24HourClock(const std::string &flag)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(DESCRIPTOR);
    data.WriteString(flag);
    Remote()->SendRequest(static_cast<uint32_t>(ILocaleConfigAbilityCode::SET_24_HOUR_CLOCK), data,
        reply, option);
    return ProcessReply(reply.ReadInt32());
}

I18nErrorCode I18nServiceAbilityProxy::SetUsingLocalDigit(bool flag)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(DESCRIPTOR);
    data.WriteBool(flag);
    Remote()->SendRequest(static_cast<uint32_t>(ILocaleConfigAbilityCode::SET_USING_LOCAL_DIGIT), data,
        reply, option);
    return ProcessReply(reply.ReadInt32());
}

I18nErrorCode I18nServiceAbilityProxy::AddPreferredLanguage(const std::string &language, int32_t index)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(DESCRIPTOR);
    data.WriteString(language);
    data.WriteInt32(index);
    Remote()->SendRequest(static_cast<uint32_t>(ILocaleConfigAbilityCode::ADD_PREFERRED_LANGUAGE), data,
        reply, option);
    return ProcessReply(reply.ReadInt32());
}

I18nErrorCode I18nServiceAbilityProxy::RemovePreferredLanguage(int32_t index)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(DESCRIPTOR);
    data.WriteInt32(index);
    Remote()->SendRequest(static_cast<uint32_t>(ILocaleConfigAbilityCode::REMOVE_PREFERRED_LANGUAGE),
        data, reply, option);
    return ProcessReply(reply.ReadInt32());
}

I18nErrorCode I18nServiceAbilityProxy::ProcessReply(int32_t reply)
{
    I18nErrorCode err = static_cast<I18nErrorCode>(reply);
    if (err != I18nErrorCode::SUCCESS) {
        HiLog::Error(LABEL,
            "I18nServiceAbilityProxy::ProcessReply failed with errorcode=%{public}d", reply);
    }
    return err;
}
} // namespace I18n
} // namespace Global
} // namespace OHOS