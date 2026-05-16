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

#include "zone_rules_addon.h"

#include "i18n_hilog.h"
#include "variable_converter.h"
#include "zone_offset_transition_addon.h"

using namespace OHOS;
using namespace Global;
using namespace I18n;

ZoneRulesAddon* ZoneRulesAddon::UnwrapAddon(ani_env *env, ani_object object)
{
    ani_long ptr;
    if (ANI_OK != env->Object_GetFieldByName_Long(object, "nativeZoneRules", &ptr)) {
        HILOG_ERROR_I18N("Get Long 'nativeZoneRules' failed");
        return nullptr;
    }
    return reinterpret_cast<ZoneRulesAddon*>(ptr);
}

ani_object ZoneRulesAddon::GetZoneRulesObject(ani_env *env, ani_object object, std::string tzId)
{
    std::unique_ptr<ZoneRulesAddon> obj = std::make_unique<ZoneRulesAddon>();
    obj->zoneRules_ = std::make_unique<ZoneRules>(tzId);
    static const char* className = "@ohos.i18n.i18n.ZoneRules";
    ani_object zoneRulesObject = VariableConverter::CreateAniObject(env, className, obj.get());
    obj.release();
    return zoneRulesObject;
}

ani_object ZoneRulesAddon::NextTransition(ani_env *env, ani_object object, ani_object date)
{
    ZoneRulesAddon *obj = UnwrapAddon(env, object);
    if (!obj || !obj->zoneRules_) {
        HILOG_ERROR_I18N("ZoneRulesAddon::NextTransition Get ZoneRulesAddon object failed");
        return nullptr;
    }
    ani_boolean isUndefined;
    if (ANI_OK != env->Reference_IsUndefined(date, &isUndefined)) {
        HILOG_ERROR_I18N("NextTransition: Reference IsUndefined failed");
        return nullptr;
    }
    std::unique_ptr<ZoneOffsetTransition> zoneOffsetTrans;
    if (!isUndefined) {
        double paramDate = 0;
        if (ANI_OK != env->Object_CallMethodByName_Double(date, "toDouble", ":d", &paramDate)) {
            HILOG_ERROR_I18N("NextTransition: Unbox Double failed");
            return nullptr;
        }
        zoneOffsetTrans = obj->zoneRules_->NextTransition(paramDate);
    } else {
        zoneOffsetTrans = obj->zoneRules_->NextTransition();
    }
    return ZoneOffsetTransitionAddon::GetZoneOffsetTransObject(env, zoneOffsetTrans.get());
}

ani_status ZoneRulesAddon::BindContextZoneRules(ani_env *env)
{
    static const char *className = "@ohos.i18n.i18n.ZoneRules";
    ani_class cls;
    if (ANI_OK != env->FindClass(className, &cls)) {
        HILOG_ERROR_I18N("Find class '%{public}s' failed", className);
        return ANI_ERROR;
    }

    std::array methods = {
        ani_native_function { "nextTransition", nullptr, reinterpret_cast<void *>(ZoneRulesAddon::NextTransition) },
    };
    if (ANI_OK != env->Class_BindNativeMethods(cls, methods.data(), methods.size())) {
        HILOG_ERROR_I18N("Cannot bind native methods to '%{public}s'", className);
        return ANI_ERROR;
    }
    return ANI_OK;
}
