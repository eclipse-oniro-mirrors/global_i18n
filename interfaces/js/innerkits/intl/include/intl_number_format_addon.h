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

#ifndef OHOS_GLOBAL_I18N_INTL_NUMBER_FORMAT_ADDON_H
#define OHOS_GLOBAL_I18N_INTL_NUMBER_FORMAT_ADDON_H

#include <memory>

#include "napi/native_api.h"
#include "napi/native_node_api.h"

#include "number_format.h"

namespace OHOS {
namespace Global {
namespace I18n {
class IntlNumberFormatAddon {
public:
    IntlNumberFormatAddon();
    ~IntlNumberFormatAddon();
    static void Destructor(napi_env env, void *nativeObject, void *hint);
    static napi_value InitIntlNumberFormat(napi_env env, napi_value exports);
    static napi_status UnwrapIntlNumberFormat(napi_env env, napi_value value, IntlNumberFormatAddon** numberFormat);

    std::shared_ptr<NumberFormat> GetNumberFormat();

private:
    static napi_value NumberFormatConstructor(napi_env env, napi_callback_info info);
    bool HandleArguments(napi_env env, napi_value argVal, size_t argc,
        std::vector<std::string> &localeTags);
    static napi_value FormatNumber(napi_env env, napi_callback_info info);
    static napi_value GetNumberResolvedOptions(napi_env env, napi_callback_info info);
    static napi_value FormatRangeNumber(napi_env env, napi_callback_info info);
    static napi_value FormatRangeToParts(napi_env env, napi_callback_info info);
    static napi_value FormatToParts(napi_env env, napi_callback_info info);
    static napi_value ToString(napi_env env, napi_callback_info info);
    static napi_value SupportedLocalesOf(napi_env env, napi_callback_info info);
    static napi_value GetFormatToPartsInner(napi_env env, napi_value param, napi_valuetype &valueType,
        napi_value &thisVar);
    static napi_value FormatWithDiffParamType(napi_env env, napi_value param, napi_valuetype &valueType,
        napi_value &thisVar);
    static napi_value SetNumberFormatParts(napi_env env, napi_status &status,
        const std::vector<std::vector<std::string>> &numberParts);
    static napi_value CreateNumberFormatWithoutNew(napi_env env, size_t argc, napi_value *argv);
    static double GetFormatParam(napi_env env, napi_value param, int32_t &code);

    bool InitNumberFormatContext(napi_env env, const std::vector<std::string> &localeTags,
        std::map<std::string, std::string> &map);

    static constexpr napi_type_tag TYPE_TAG = { 0xd0435ea30bd94ac2, 0xbba013c582b30986 };

    std::shared_ptr<NumberFormat> numberfmt_ = nullptr;
};
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif