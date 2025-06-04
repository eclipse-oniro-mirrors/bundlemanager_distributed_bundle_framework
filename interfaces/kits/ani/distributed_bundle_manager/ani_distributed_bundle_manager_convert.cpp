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

#include "ani_distributed_bundle_manager_convert.h"
#include "common_fun_ani.h"

namespace OHOS {
namespace AppExecFwk {
namespace {
    constexpr const char* CLASSNAME_REMOTEABILITYINFO = "LbundleManager/RemoteAbilityInfoInner/RemoteAbilityInfoInner;";
    constexpr const char* PROPERTYNAME_ELEMENTNAME = "elementName";
    constexpr const char* PROPERTYNAME_ICON = "icon";
    constexpr const char* PROPERTYNAME_LABEL = "label";
}

ani_object AniDistributedbundleManagerConvert::ConvertRemoteAbilityInfo(ani_env* env,
    const RemoteAbilityInfo& remoteAbilityInfo)
{
    RETURN_NULL_IF_NULL(env);

    ani_class cls = CommonFunAni::CreateClassByName(env, CLASSNAME_REMOTEABILITYINFO);
    RETURN_NULL_IF_NULL(cls);

    ani_object object = CommonFunAni::CreateNewObjectByClass(env, cls);
    RETURN_NULL_IF_NULL(object);

    ani_string string = nullptr;

    // elementName: ElementName
    ani_object aElementNameObject = CommonFunAni::ConvertElementName(env, remoteAbilityInfo.elementName);
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
} // AppExecFwk
} // OHOS