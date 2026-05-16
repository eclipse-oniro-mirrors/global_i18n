/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#include "replace_intl_module.h"
#include <dlfcn.h>
#include "i18n_hilog.h"

namespace OHOS {
namespace Global {
namespace I18n {
using RegisterIntl = bool (*)(napi_env);

void ReplaceIntlModule(napi_env env)
{
    void *intlUtilHandle = dlopen("libintl_napi.z.so", RTLD_NOW);
    if (intlUtilHandle == nullptr) {
        HILOG_ERROR_I18N("ReplaceIntlModule: load libintl_napi.z.so failed");
        return;
    }
    RegisterIntl registerIntl = reinterpret_cast<RegisterIntl>(dlsym(intlUtilHandle, "RegisterIntl"));

    bool ret = false;
    if (registerIntl) {
        ret = registerIntl(env);
    } else {
        HILOG_ERROR_I18N("ReplaceIntlModule: registerIntl is null");
    }
    if (!ret) {
        HILOG_ERROR_I18N("ReplaceIntlModule: Register intl failed");
    }
}
} // namespace I18n
} // namespace Global
} // namespace OHOS