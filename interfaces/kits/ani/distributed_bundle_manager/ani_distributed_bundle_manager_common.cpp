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

#include "ani_distributed_bundle_manager_common.h"
#include "common_fun_ani.h"

namespace OHOS {
namespace AppExecFwk {
namespace AniDistributedbundleManagerCommon {
namespace {
constexpr const char* CLASSNAME_ELEMENTNAME = "bundleManager.ElementNameInner.ElementNameInner";
constexpr const char* CLASSNAME_REMOTEABILITYINFO = "bundleManager.RemoteAbilityInfoInner.RemoteAbilityInfoInner";
constexpr const char* PROPERTYNAME_DEVICEID = "deviceId";
constexpr const char* PROPERTYNAME_BUNDLENAME = "bundleName";
constexpr const char* PROPERTYNAME_MODULENAME = "moduleName";
constexpr const char* PROPERTYNAME_ABILITYNAME = "abilityName";
constexpr const char* PROPERTYNAME_ELEMENTNAME = "elementName";
constexpr const char* PROPERTYNAME_ICON = "icon";
constexpr const char* PROPERTYNAME_LABEL = "label";
}

ani_object ConvertDistributedBundleElementName(ani_env* env, const ElementName& elementName)
{
    RETURN_NULL_IF_NULL(env);

    ani_class cls = CommonFunAni::CreateClassByName(env, CLASSNAME_ELEMENTNAME);
    RETURN_NULL_IF_NULL(cls);

    ani_object object = CommonFunAni::CreateNewObjectByClass(env, CLASSNAME_ELEMENTNAME, cls);
    RETURN_NULL_IF_NULL(object);

    ani_string string = nullptr;

    // deviceId?: string
    if (CommonFunAni::StringToAniStr(env, elementName.GetDeviceID(), string)) {
        RETURN_NULL_IF_FALSE(CommonFunAni::CallSetterOptional(env, cls, object, PROPERTYNAME_DEVICEID, string));
    }

    // bundleName: string
    RETURN_NULL_IF_FALSE(CommonFunAni::StringToAniStr(env, elementName.GetBundleName(), string));
    RETURN_NULL_IF_FALSE(CommonFunAni::CallSetter(env, cls, object, PROPERTYNAME_BUNDLENAME, string));

    // moduleName?: string
    if (CommonFunAni::StringToAniStr(env, elementName.GetModuleName(), string)) {
        RETURN_NULL_IF_FALSE(CommonFunAni::CallSetterOptional(env, cls, object, PROPERTYNAME_MODULENAME, string));
    }

    // abilityName: string
    RETURN_NULL_IF_FALSE(CommonFunAni::StringToAniStr(env, elementName.GetAbilityName(), string));
    RETURN_NULL_IF_FALSE(CommonFunAni::CallSetter(env, cls, object, PROPERTYNAME_ABILITYNAME, string));

    return object;
}

ani_object ConvertRemoteAbilityInfo(ani_env* env, const RemoteAbilityInfo& remoteAbilityInfo)
{
    RETURN_NULL_IF_NULL(env);

    ani_class cls = CommonFunAni::CreateClassByName(env, CLASSNAME_REMOTEABILITYINFO);
    RETURN_NULL_IF_NULL(cls);

    ani_object object = CommonFunAni::CreateNewObjectByClass(env, CLASSNAME_REMOTEABILITYINFO, cls);
    RETURN_NULL_IF_NULL(object);

    ani_string string = nullptr;

    // elementName: ElementName
    ani_object aElementNameObject = ConvertDistributedBundleElementName(env, remoteAbilityInfo.elementName);
    RETURN_NULL_IF_NULL(aElementNameObject);
    RETURN_NULL_IF_FALSE(CommonFunAni::CallSetter(env, cls, object, PROPERTYNAME_ELEMENTNAME, aElementNameObject));

    // label: string
    RETURN_NULL_IF_FALSE(CommonFunAni::StringToAniStr(env, remoteAbilityInfo.label, string));
    RETURN_NULL_IF_FALSE(CommonFunAni::CallSetter(env, cls, object, PROPERTYNAME_LABEL, string));

    // icon: string
    RETURN_NULL_IF_FALSE(CommonFunAni::StringToAniStr(env, remoteAbilityInfo.icon, string));
    RETURN_NULL_IF_FALSE(CommonFunAni::CallSetter(env, cls, object, PROPERTYNAME_ICON, string));

    return object;
}
} // AniDistributedbundleManagerCommon
} // AppExecFwk
} // OHOS