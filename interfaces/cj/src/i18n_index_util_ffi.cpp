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
#include <cstdlib>
#include <chrono>
#include <string>
#include <map>
#include "i18n_struct.h"
#include "i18n_hilog.h"
#include "i18n_index_util_ffi.h"
#include "i18n_index_util_impl.h"
#include "index_util.h"
#include <utility>
 
namespace OHOS {
namespace Global {
namespace I18n {
using namespace OHOS::FFI;
extern "C"
{
    int64_t FfiI18nIndexUtilConstructor(char* locale)
    {
        std::unique_ptr<IndexUtil> indexUtil_ = std::make_unique<IndexUtil>(std::string(locale));
        if (indexUtil_ == nullptr) {
            HILOG_ERROR_I18N("Create IndexUtil fail");
            return -1;
        }
        auto ffiIndexUtilInstance = FFIData::Create<FfiI18nIndexUtil>(std::move(indexUtil_));
        if (ffiIndexUtilInstance == nullptr) {
            HILOG_ERROR_I18N("Create FfiI18nIndexUtil fail");
            return -1;
        }
        return ffiIndexUtilInstance -> GetID();
    }

    void FfiI18nIndexUtilAddLocale(int64_t remoteDataID, char* locale)
    {
        auto ffiIndexUtilInstance = FFIData::GetData<FfiI18nIndexUtil>(remoteDataID);
        if (!ffiIndexUtilInstance) {
            HILOG_ERROR_I18N("The FfiI18nIndexUtil instance is nullptr");
            return;
        }
        ffiIndexUtilInstance->addLocale(std::string(locale));
    }

    char* FfiI18nIndexUtilGetIndex(int64_t remoteDataID, char* text)
    {
        auto ffiIndexUtilInstance = FFIData::GetData<FfiI18nIndexUtil>(remoteDataID);
        if (!ffiIndexUtilInstance) {
            HILOG_ERROR_I18N("The FfiI18nIndexUtil instance is nullptr");
            return nullptr;
        }
        return ffiIndexUtilInstance->getIndex(std::string(text));
    }

    CArrStr FfiI18nIndexUtilGetIndexList(int64_t remoteDataID)
    {
        auto ffiIndexUtilInstance = FFIData::GetData<FfiI18nIndexUtil>(remoteDataID);
        if (!ffiIndexUtilInstance) {
            HILOG_ERROR_I18N("The FfiI18nIndexUtil instance is nullptr");
            return { nullptr, 0 };
        }
        return ffiIndexUtilInstance->getIndexList();
    }
}
}  // namespace I18n
}  // namespace Global
}  // namespace OHOS