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

#include "i18n_timezone_impl.h"
#include "i18n_ffi.h"
namespace OHOS {
namespace Global {
namespace I18n {
    FfiI18nTimeZone::FfiI18nTimeZone(std::unique_ptr<I18nTimeZone> timezone)
    {
        timezone_ = std::move(timezone);
    }

    char* FfiI18nTimeZone::getID()
    {
        return MallocCString(timezone_->GetID());
    }

    int32_t FfiI18nTimeZone::getRawOffset()
    {
        return timezone_->GetRawOffset();
    }

    int32_t FfiI18nTimeZone::getOffset(double date)
    {
        return timezone_->GetOffset(date);
    }

    char* FfiI18nTimeZone::getDisplayName()
    {
        return MallocCString(timezone_->GetDisplayName());
    }

    char* FfiI18nTimeZone::getDisplayName(std::string locale)
    {
        return MallocCString(timezone_->GetDisplayName(locale));
    }

    char* FfiI18nTimeZone::getDisplayName(std::string locale, bool isDST)
    {
        return MallocCString(timezone_->GetDisplayName(locale, isDST));
    }

    char* FfiI18nTimeZone::getDisplayName(bool isDST)
    {
        return MallocCString(timezone_->GetDisplayName(isDST));
    }
}  // namespace I18n
}  // namespace Global
}  // namespace OHOS
