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

#include "zone_offset_transition_addon.h"

#include "i18n_hilog.h"
#include "variable_converter.h"

using namespace OHOS;
using namespace Global;
using namespace I18n;

ZoneOffsetTransitionAddon* ZoneOffsetTransitionAddon::UnwrapAddon(ani_env *env, ani_object object)
{
    ani_long ptr;
    if (ANI_OK != env->Object_GetFieldByName_Long(object, "nativeZoneOffsetTransition", &ptr)) {
        HILOG_ERROR_I18N("Get Long 'nativeZoneOffsetTransition' failed");
        return nullptr;
    }
    return reinterpret_cast<ZoneOffsetTransitionAddon*>(ptr);
}

ani_object ZoneOffsetTransitionAddon::GetZoneOffsetTransObject(ani_env *env,
    ZoneOffsetTransition* zoneOffsetTrans)
{
    if (zoneOffsetTrans == nullptr) {
        HILOG_ERROR_I18N("GetZoneOffsetTransObject: zoneOffsetTrans is nullptr");
        return nullptr;
    }
    double milliseconds = zoneOffsetTrans->GetMilliseconds();
    int32_t offsetBefore = zoneOffsetTrans->GetOffsetBefore();
    int32_t offsetAfter = zoneOffsetTrans->GetOffsetAfter();
    static const char* className = "@ohos.i18n.i18n.ZoneOffsetTransition";
    std::unique_ptr<ZoneOffsetTransitionAddon> transObj = std::make_unique<ZoneOffsetTransitionAddon>();
    transObj->zoneOffsetTransition_ =
        std::make_unique<ZoneOffsetTransition>(milliseconds, offsetBefore, offsetAfter);
    ani_object zoneTransObject = VariableConverter::CreateAniObject(env, className, transObj.get());
    transObj.release();
    return zoneTransObject;
}

ani_double ZoneOffsetTransitionAddon::GetMilliseconds(ani_env *env, ani_object object)
{
    ZoneOffsetTransitionAddon *obj = UnwrapAddon(env, object);
    if (!obj || !obj->zoneOffsetTransition_) {
        HILOG_ERROR_I18N("ZoneRulesAddon::NextTransition Get ZoneRulesAddon object failed");
        return 0;
    }
    double milliseconds = obj->zoneOffsetTransition_->GetMilliseconds();
    return milliseconds;
}

ani_int ZoneOffsetTransitionAddon::GetOffsetAfter(ani_env *env, ani_object object)
{
    ZoneOffsetTransitionAddon *obj = UnwrapAddon(env, object);
    if (!obj || !obj->zoneOffsetTransition_) {
        HILOG_ERROR_I18N("ZoneRulesAddon::NextTransition Get ZoneRulesAddon object failed");
        return -1;
    }
    int32_t offsetAfter = obj->zoneOffsetTransition_->GetOffsetAfter();
    return offsetAfter;
}

ani_int ZoneOffsetTransitionAddon::GetOffsetBefore(ani_env *env, ani_object object)
{
    ZoneOffsetTransitionAddon *obj = UnwrapAddon(env, object);
    if (!obj || !obj->zoneOffsetTransition_) {
        HILOG_ERROR_I18N("ZoneRulesAddon::NextTransition Get ZoneRulesAddon object failed");
        return -1;
    }
    int32_t offsetBefore = obj->zoneOffsetTransition_->GetOffsetBefore();
    return offsetBefore;
}

ani_status ZoneOffsetTransitionAddon::BindContextZoneOffsetTransition(ani_env *env)
{
    static const char *className = "@ohos.i18n.i18n.ZoneOffsetTransition";
    ani_class cls;
    if (ANI_OK != env->FindClass(className, &cls)) {
        HILOG_ERROR_I18N("Find class '%{public}s' failed", className);
        return ANI_ERROR;
    }

    std::array methods = {
        ani_native_function { "getMilliseconds", nullptr,
            reinterpret_cast<void *>(ZoneOffsetTransitionAddon::GetMilliseconds) },
        ani_native_function { "getOffsetAfter", nullptr,
            reinterpret_cast<void *>(ZoneOffsetTransitionAddon::GetOffsetAfter) },
        ani_native_function { "getOffsetBefore", nullptr,
            reinterpret_cast<void *>(ZoneOffsetTransitionAddon::GetOffsetBefore) },
    };
    if (ANI_OK != env->Class_BindNativeMethods(cls, methods.data(), methods.size())) {
        HILOG_ERROR_I18N("Cannot bind native methods to '%{public}s'", className);
        return ANI_ERROR;
    }
    return ANI_OK;
}
