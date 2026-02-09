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


#include "i18n_entity_impl.h"
#include "unicode/utypes.h"
#include "unicode/locid.h"
#include "utils.h"
#include "entity_recognizer.h"
#include "i18n_ffi.h"
#include "i18n_hilog.h"

namespace OHOS {
namespace Global {
namespace I18n {

CEntity::CEntity(std::string locale)
{
    UErrorCode localeStatus = U_ZERO_ERROR;
    icu::Locale localeTag = icu::Locale::forLanguageTag(locale, localeStatus);
    if (!IsValidLocaleTag(localeTag)) {
        entityRecognizer_ = nullptr;
        return;
    }
    entityRecognizer_ = std::make_unique<EntityRecognizer>(localeTag);
    if (entityRecognizer_ == nullptr) {
        HILOG_ERROR_I18N("Create EntityRecognizer fail");
    }
}

static int32_t EntityInfoItems2C(const std::vector<EntityInfoItem>& items, CEntityInfoItemArr &arr)
{
    int32_t size = static_cast<int32_t>(items.size());
    if (size > 0) {
        arr.head = static_cast<CEntityInfoItem*>(malloc(sizeof(CEntityInfoItem) * size));
        if (arr.head == nullptr) {
            return -1;
        }
        arr.size = size;

        for (int32_t i = 0; i < size; ++i) {
            const EntityInfoItem& item = items[i];
            CEntityInfoItem cItem;
            cItem.type = MallocCString(item.type);
            cItem.begin = item.begin;
            cItem.end = item.end;

            if (cItem.type == nullptr) {
                return -1;
            }

            arr.head[i] = cItem;
        }
    } else {
        arr.head = nullptr;
        arr.size = 0;
    }
    return 0;
}

CEntityInfoItemArr CEntity::FindEntityInfo(std::string text)
{
    std::vector<std::vector<int>> entityInfo = entityRecognizer_->FindEntityInfo(text);
    std::vector<std::string> types = {"phone_number", "date"};
    std::vector<EntityInfoItem> items;
    CEntityInfoItemArr arr = { 0, nullptr };
    int index = 0;
    for (std::string::size_type t = 0; t < types.size(); t++) {
        for (int i = 0; i < entityInfo[t][0]; i++) {
            int begin = entityInfo[t][2 * i + 1];
            int end = entityInfo[t][2 * i + 2];
            std::string type = types[t];
            EntityInfoItem item = { type, begin, end };
            items.push_back(item);
            index++;
        }
    }
    EntityInfoItems2C(items, arr);
    return arr;
}

bool CEntity::InitSuccess()
{
    return entityRecognizer_ != nullptr;
}

}  // namespace I18n
}  // namespace Global
}  // namespace OHOS
