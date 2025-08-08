/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include <common_event_data.h>
#include <common_event_manager.h>
#include <common_event_support.h>
#include "common_event_subscriber.h"
#include "i18n_hilog.h"
#include "multi_users.h"

#include "i18n_service_event.h"

namespace OHOS {
namespace Global {
namespace I18n {
const int I18nServiceEvent::RETRY = 3;
const int32_t I18nServiceEvent::OS_ACCOUNT_UID = 3058;
const std::string I18nServiceEvent::OS_ACCOUNT_PERMISSION = "ohos.permission.MANAGE_LOCAL_ACCOUNTS";

I18nServiceEvent::I18nServiceEvent()
{
    HILOG_INFO_I18N("I18nServiceEvent: start.");
}

I18nServiceEvent::~I18nServiceEvent()
{
    if (subscriber) {
        bool status = EventFwk::CommonEventManager::UnSubscribeCommonEvent(subscriber);
        if (!status) {
            HILOG_ERROR_I18N("UnSubscriberEvent: UnSubscriberEvent failed");
        }
        subscriber = nullptr;
    }
}

void I18nServiceEvent::SubscriberEvent()
{
    if (subscriber) {
        return;
    }
    EventFwk::MatchingSkills matchingSkills;
    matchingSkills.AddEvent(EventFwk::CommonEventSupport::COMMON_EVENT_USER_SWITCHED);
    matchingSkills.AddEvent(EventFwk::CommonEventSupport::COMMON_EVENT_USER_ADDED);
    matchingSkills.AddEvent(EventFwk::CommonEventSupport::COMMON_EVENT_USER_REMOVED);
    EventFwk::CommonEventSubscribeInfo subscribeInfo(matchingSkills);
    subscribeInfo.SetPublisherUid(OS_ACCOUNT_UID);
    subscribeInfo.SetPermission(OS_ACCOUNT_PERMISSION);
    subscriber = std::make_shared<I18nServiceSubscriber>(subscribeInfo, *this);

    int32_t retry = RETRY;
    do {
        bool status = EventFwk::CommonEventManager::SubscribeCommonEvent(subscriber);
        if (status) {
            HILOG_INFO_I18N("SubscriberEvent: SubscriberEvent success.");
            return;
        }
        HILOG_ERROR_I18N("SubscriberEvent: SubscriberEvent failed %{public}d.", retry);
        retry--;
        sleep(1);
    } while (retry);
    HILOG_INFO_I18N("SubscriberEvent: SubscriberEvent failed");
}

void I18nServiceEvent::CheckStartReason(const SystemAbilityOnDemandReason& startReason)
{
    std::string action = startReason.GetName();
    std::string localId = std::to_string(startReason.GetExtraData().GetCode());
    HILOG_INFO_I18N("CheckStartReason: start reason is %{public}s", action.c_str());
    HandleMultiUser(action, localId);
}

void I18nServiceEvent::OnReceiveEvent(const EventFwk::CommonEventData& data)
{
    std::string action = data.GetWant().GetAction();
    std::string localId = std::to_string(data.GetCode());
    HILOG_INFO_I18N("OnReceiveEvent: handle event %{public}s", action.c_str());
    HandleMultiUser(action, localId);
}

void I18nServiceEvent::HandleMultiUser(const std::string& action, const std::string& localId)
{
#ifdef SUPPORT_MULTI_USER
    if (action == EventFwk::CommonEventSupport::COMMON_EVENT_USER_SWITCHED) {
        MultiUsers::SwitchUser(localId);
    } else if (action == EventFwk::CommonEventSupport::COMMON_EVENT_USER_ADDED) {
        MultiUsers::AddUser(localId);
    } else if (action == EventFwk::CommonEventSupport::COMMON_EVENT_USER_REMOVED) {
        MultiUsers::RemoveUser(localId);
    }
#endif
}
} // namespace I18n
} // namespace Global
} // namespace OHOS