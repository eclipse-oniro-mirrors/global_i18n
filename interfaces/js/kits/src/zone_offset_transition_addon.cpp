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
#include "zone_offset_transition.h"
#include "error_util.h"
#include "i18n_hilog.h"
#include "zone_offset_transition_addon.h"
#include "js_lifecycle_managers.h"
#include "js_utils.h"
#include "variable_convertor.h"

namespace OHOS {
namespace Global {
namespace I18n {
static thread_local ThreadReference* g_zoneOffsetTransConstructor = nullptr;

ZoneOffsetTransitionAddon::ZoneOffsetTransitionAddon() {}

ZoneOffsetTransitionAddon::~ZoneOffsetTransitionAddon() {}

void ZoneOffsetTransitionAddon::Destructor(napi_env env, void *nativeObject, void *hint)
{
    if (!nativeObject) {
        return;
    }
    delete reinterpret_cast<ZoneOffsetTransitionAddon *>(nativeObject);
    nativeObject = nullptr;
}

napi_value ZoneOffsetTransitionAddon::InitZoneOffsetTransition(napi_env env, napi_value exports)
{
    HandleScope scope(env, "InitZoneOffsetTransition");
    if (!scope.IsOpen()) {
        return nullptr;
    }

    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("getMilliseconds", GetMilliseconds),
        DECLARE_NAPI_FUNCTION("getOffsetAfter", GetOffsetAfter),
        DECLARE_NAPI_FUNCTION("getOffsetBefore", GetOffsetBefore),
    };
    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, "ZoneOffsetTransition", NAPI_AUTO_LENGTH,
        I18nZoneOffsetTransitionConstructor, nullptr,
        sizeof(properties) / sizeof(napi_property_descriptor), properties, &constructor);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("InitZoneOffsetTransition: Failed to define class ZoneOffsetTransition at Init");
        return nullptr;
    }
    status = napi_set_named_property(env, exports, "ZoneOffsetTransition", constructor);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("InitZoneOffsetTransition: Set property ZoneOffsetTransition failed.");
        return nullptr;
    }
    g_zoneOffsetTransConstructor = ThreadReference::CreateInstance(env, constructor, 1);
    if (!g_zoneOffsetTransConstructor) {
        HILOG_ERROR_I18N("InitZoneOffsetTransition: Failed to create reference g_timezoneConstructor at init");
        return nullptr;
    }
    return exports;
}

napi_value ZoneOffsetTransitionAddon::I18nZoneOffsetTransitionConstructor(napi_env env, napi_callback_info info)
{
    size_t argc = 3;
    napi_value argv[3] = { nullptr };
    napi_value thisVar = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("ZoneOffsetTransitionAddon: Get parameter info failed");
        return nullptr;
    }
    if (argc < CONSTRUCTOR_ARGS_COUNT) {
        HILOG_ERROR_I18N("ZoneOffsetTransitionAddon: Constructor args count less then required");
        return nullptr;
    }
    int32_t code = 0;
    double milliSecond = VariableConvertor::GetDouble(env, argv[0], code);
    if (code != 0) {
        return nullptr;
    }
    int32_t offsetBefore = VariableConvertor::GetInt(env, argv[1], code);
    if (code != 0) {
        return nullptr;
    }
    int32_t offsetAfter = VariableConvertor::GetInt(env, argv[2], code);
    if (code != 0) {
        return nullptr;
    }
    ZoneOffsetTransitionAddon* obj = new (std::nothrow) ZoneOffsetTransitionAddon();
    if (obj == nullptr) {
        HILOG_ERROR_I18N("ZoneOffsetTransitionAddon: Create ZoneOffsetTransitionAddon object failed");
        return nullptr;
    }
    status = napi_wrap_s(env, thisVar, reinterpret_cast<void *>(obj),
        ZoneOffsetTransitionAddon::Destructor, nullptr, &TYPE_TAG, nullptr);
    if (status != napi_ok) {
        delete obj;
        HILOG_ERROR_I18N("ZoneOffsetTransitionAddon: Wrap ZoneOffsetTransition failed");
        return nullptr;
    }
    obj->zoneOffsetTransition_ = std::make_unique<ZoneOffsetTransition>(milliSecond, offsetBefore, offsetAfter);
    if (!obj->zoneOffsetTransition_) {
        delete obj;
        HILOG_ERROR_I18N("ZoneOffsetTransitionAddon: Create ZoneOffsetTransition failed");
        return nullptr;
    }
    return thisVar;
}

napi_value ZoneOffsetTransitionAddon::GetZoneOffsetTransObject(napi_env env, ZoneOffsetTransition* zoneOffsetTrans)
{
    if (g_zoneOffsetTransConstructor == nullptr) {
        HILOG_ERROR_I18N("GetZoneOffsetTransObject: g_zoneOffsetTransConstructor is nullptr");
        return nullptr;
    }
    size_t argc = 3;
    napi_value argv[3] = { nullptr };
    napi_value milliseconds = nullptr;
    napi_status status = napi_create_double(env, zoneOffsetTrans->GetMilliseconds(), &milliseconds);
    if (status != napi_ok || milliseconds == nullptr) {
        HILOG_ERROR_I18N("ErrorUtil::NapiThrow: create napi double failed");
        return nullptr;
    }
    argv[0] = milliseconds;
    napi_value offsetBefore = nullptr;
    status = napi_create_int32(env, zoneOffsetTrans->GetOffsetBefore(), &offsetBefore);
    if (status != napi_ok || offsetBefore == nullptr) {
        HILOG_ERROR_I18N("ErrorUtil::NapiThrow: create int32 failed");
        return nullptr;
    }
    argv[1] = offsetBefore;
    napi_value offsetAfter = nullptr;
    status = napi_create_int32(env, zoneOffsetTrans->GetOffsetAfter(), &offsetAfter);
    if (status != napi_ok || offsetAfter == nullptr) {
        HILOG_ERROR_I18N("ErrorUtil::NapiThrow: create int32 failed");
        return nullptr;
    }
    argv[CONSTRUCTOR_ARGS_COUNT - 1] = offsetAfter;
    napi_value constructor = nullptr;
    if (!g_zoneOffsetTransConstructor->GetReferenceValue(env, &constructor)) {
        HILOG_ERROR_I18N("Failed to create reference of normalizer Constructor");
        return nullptr;
    }
    napi_value result = nullptr;
    status = napi_new_instance(env, constructor, argc, argv, &result);
    if (status != napi_ok || result == nullptr) {
        HILOG_ERROR_I18N("create normalizer instance failed");
        return nullptr;
    }
    return result;
}

napi_value ZoneOffsetTransitionAddon::GetMilliseconds(napi_env env, napi_callback_info info)
{
    size_t argc = 0;
    napi_value *argv = nullptr;
    napi_value thisVar = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("GetMilliseconds: Get parameter info failed");
        return nullptr;
    }
    ZoneOffsetTransitionAddon *obj = nullptr;
    status = napi_unwrap_s(env, thisVar, &TYPE_TAG, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->zoneOffsetTransition_) {
        HILOG_ERROR_I18N("GetMilliseconds: Get ZoneOffsetTransition object failed");
        return nullptr;
    }
    double time = obj->zoneOffsetTransition_->GetMilliseconds();
    napi_value result = nullptr;
    status = napi_create_double(env, time, &result);
    if (status != napi_ok || result == nullptr) {
        HILOG_ERROR_I18N("GetMilliseconds: create double js variable failed.");
        return nullptr;
    }
    return result;
}

napi_value ZoneOffsetTransitionAddon::GetOffsetAfter(napi_env env, napi_callback_info info)
{
    size_t argc = 0;
    napi_value *argv = nullptr;
    napi_value thisVar = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("GetOffsetAfter: Get parameter info failed");
        return nullptr;
    }
    ZoneOffsetTransitionAddon *obj = nullptr;
    status = napi_unwrap_s(env, thisVar, &TYPE_TAG, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->zoneOffsetTransition_) {
        HILOG_ERROR_I18N("GetOffsetAfter: Get ZoneOffsetTransition object failed");
        return nullptr;
    }
    int32_t offsetAfter = obj->zoneOffsetTransition_->GetOffsetAfter();
    return VariableConvertor::CreateNumber(env, offsetAfter);
}

napi_value ZoneOffsetTransitionAddon::GetOffsetBefore(napi_env env, napi_callback_info info)
{
    size_t argc = 0;
    napi_value *argv = nullptr;
    napi_value thisVar = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("GetOffsetBefore: Get parameter info failed");
        return nullptr;
    }
    ZoneOffsetTransitionAddon *obj = nullptr;
    status = napi_unwrap_s(env, thisVar, &TYPE_TAG, reinterpret_cast<void **>(&obj));
    if (status != napi_ok || !obj || !obj->zoneOffsetTransition_) {
        HILOG_ERROR_I18N("GetOffsetBefore: Get ZoneOffsetTransition object failed");
        return nullptr;
    }
    int32_t offsetBefore = obj->zoneOffsetTransition_->GetOffsetBefore();
    return VariableConvertor::CreateNumber(env, offsetBefore);
}
} // namespace I18n
} // namespace Global
} // namespace OHOS