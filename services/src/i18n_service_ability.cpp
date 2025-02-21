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
#include "i18n_service_ability_load_manager.h"
#include "locale_config.h"
#include "preferred_language.h"
#include "system_ability_definition.h"
#include "i18n_service_ability.h"

namespace OHOS {
namespace Global {
namespace I18n {
static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, 0xD001E00, "I18nServiceAbility" };
using namespace OHOS::HiviewDFX;

REGISTER_SYSTEM_ABILITY_BY_ID(I18nServiceAbility, I18N_SA_ID, false);
static const std::string UNLOAD_TASK = "i18n_service_unload";
static const uint32_t DELAY_MILLISECONDS_FOR_UNLOAD_SA = 10000;

I18nServiceAbility::I18nServiceAbility(int32_t saId, bool runOnCreate) : SystemAbility(saId, runOnCreate)
{
    HiLog::Info(LABEL, "I18nServiceAbility object init success.");
}

I18nServiceAbility::~I18nServiceAbility()
{
    HiLog::Info(LABEL, "I18nServiceAbility object release.");
}

I18nErrorCode I18nServiceAbility::SetSystemLanguage(const std::string &language)
{
    return LocaleConfig::SetSystemLanguage(language);
}

I18nErrorCode I18nServiceAbility::SetSystemRegion(const std::string &region)
{
    return LocaleConfig::SetSystemRegion(region);
}

I18nErrorCode I18nServiceAbility::SetSystemLocale(const std::string &locale)
{
    return LocaleConfig::SetSystemLocale(locale);
}

I18nErrorCode I18nServiceAbility::Set24HourClock(const std::string &flag)
{
    return LocaleConfig::Set24HourClock(flag);
}

I18nErrorCode I18nServiceAbility::SetUsingLocalDigit(bool flag)
{
    return LocaleConfig::SetUsingLocalDigit(flag);
}

I18nErrorCode I18nServiceAbility::AddPreferredLanguage(const std::string &language, int32_t index)
{
    return PreferredLanguage::AddPreferredLanguage(language, index);
}

I18nErrorCode I18nServiceAbility::RemovePreferredLanguage(int32_t index)
{
    return PreferredLanguage::RemovePreferredLanguage(index);
}

void I18nServiceAbility::UnloadI18nServiceAbility()
{
    HiLog::Info(LABEL, "I18nServiceAbility::UnloadI18nServiceAbility ready to unload after 10 seconds.");
    if (handler != nullptr) {
        handler->RemoveTask(UNLOAD_TASK);
    }
    auto task = [this]() {
        auto i18nSaLoadManager = DelayedSingleton<I18nServiceAbilityLoadManager>::GetInstance();
        if (i18nSaLoadManager != nullptr) {
            HiLog::Info(LABEL, "I18nServiceAbility::UnloadI18nServiceAbility start to unload i18n sa.");
            i18nSaLoadManager->UnloadI18nService(I18N_SA_ID);
        }
    };
    if (handler != nullptr) {
        handler->PostTask(task, UNLOAD_TASK, DELAY_MILLISECONDS_FOR_UNLOAD_SA);
    }
}

void I18nServiceAbility::OnStart()
{
    HiLog::Info(LABEL, "I18nServiceAbility start.");
    bool status = Publish(this);
    if (status) {
        HiLog::Info(LABEL, "I18nServiceAbility Publish success.");
    } else {
        HiLog::Info(LABEL, "I18nServiceAbility Publish failed.");
    }
    handler = std::make_shared<AppExecFwk::EventHandler>(AppExecFwk::EventRunner::Create(true));
    UnloadI18nServiceAbility();
}

void I18nServiceAbility::OnStop()
{
    HiLog::Info(LABEL, "I18nServiceAbility Stop.");
}
} // namespace I18n
} // namespace Global
} // namespace OHOS