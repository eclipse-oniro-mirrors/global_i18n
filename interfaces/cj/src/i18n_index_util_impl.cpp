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

#include "i18n_ffi.h"
#include "i18n_index_util_impl.h"

namespace OHOS {
namespace Global {
namespace I18n {
extern "C"
{
    FfiI18nIndexUtil::FfiI18nIndexUtil(std::unique_ptr<IndexUtil> indexUtil)
    {
        indexUtil_ = std::move(indexUtil);
    }

    void FfiI18nIndexUtil::addLocale(std::string locale)
    {
        indexUtil_->AddLocale(locale);
    }

    char* FfiI18nIndexUtil::getIndex(std::string text)
    {
        return MallocCString(indexUtil_->GetIndex(text));
    }

    CArrStr FfiI18nIndexUtil::getIndexList()
    {
        return VectorStringToCArr(indexUtil_->GetIndexList());
}
}
}  // namespace I18n
}  // namespace Global
}  // namespace OHOS