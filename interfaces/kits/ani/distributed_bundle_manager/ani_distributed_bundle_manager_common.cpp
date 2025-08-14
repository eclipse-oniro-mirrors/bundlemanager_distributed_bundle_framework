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

#include <ani_signature_builder.h>

#include "ani_distributed_bundle_manager_common.h"
#include "common_fun_ani.h"

namespace OHOS {
namespace AppExecFwk {
namespace AniDistributedbundleManagerCommon {
namespace {
constexpr const char* CLASSNAME_ELEMENT_NAME = "bundleManager.ElementNameInner.ElementNameInner";
constexpr const char* CLASSNAME_REMOTE_ABILITY_INFO = "bundleManager.RemoteAbilityInfoInner.RemoteAbilityInfoInner";
}

ani_object ConvertDistributedBundleElementName(ani_env* env, const ElementName& elementName)
{
    RETURN_NULL_IF_NULL(env);

    ani_ref undefinedRef = nullptr;
    ani_status status = env->GetUndefined(&undefinedRef);
    if (status != ANI_OK) {
        APP_LOGE("GetUndefined failed %{public}d", status);
        return nullptr;
    }

    // deviceId?: string
    ani_ref deviceId = undefinedRef;
    ani_string deviceIdString = nullptr;
    if (CommonFunAni::StringToAniStr(env, elementName.GetDeviceID(), deviceIdString)) {
        deviceId = deviceIdString;
    }

    // bundleName: string
    ani_string bundleName = nullptr;
    RETURN_NULL_IF_FALSE(CommonFunAni::StringToAniStr(env, elementName.GetBundleName(), bundleName));

    // moduleName?: string
    ani_ref moduleName = undefinedRef;
    ani_string moduleNameString = nullptr;
    if (CommonFunAni::StringToAniStr(env, elementName.GetModuleName(), moduleNameString)) {
        moduleName = moduleNameString;
    }

    // abilityName: string
    ani_string abilityName = nullptr;
    RETURN_NULL_IF_FALSE(CommonFunAni::StringToAniStr(env, elementName.GetAbilityName(), abilityName));

    ani_value args[] = {
        { .r = bundleName },
        { .r = abilityName },
        { .r = deviceId },
        { .r = moduleName },
        { .r = undefinedRef },
        { .r = undefinedRef },
    };
    static const std::string ctorSig =
        arkts::ani_signature::SignatureBuilder()
            .AddClass(CommonFunAniNS::CLASSNAME_STRING) // bundleName: string
            .AddClass(CommonFunAniNS::CLASSNAME_STRING) // abilityName: string
            .AddClass(CommonFunAniNS::CLASSNAME_STRING) // deviceId?: string
            .AddClass(CommonFunAniNS::CLASSNAME_STRING) // moduleName?: string
            .AddClass(CommonFunAniNS::CLASSNAME_STRING) // uri?: string
            .AddClass(CommonFunAniNS::CLASSNAME_STRING) // shortName?: string
            .BuildSignatureDescriptor();
    return CommonFunAni::CreateNewObjectByClassV2(env, CLASSNAME_ELEMENT_NAME, ctorSig, args);
}

ani_object ConvertRemoteAbilityInfo(ani_env* env, const RemoteAbilityInfo& remoteAbilityInfo)
{
    RETURN_NULL_IF_NULL(env);

    // elementName: ElementName
    ani_object elementName = ConvertDistributedBundleElementName(env, remoteAbilityInfo.elementName);
    RETURN_NULL_IF_NULL(elementName);

    // label: string
    ani_string label = nullptr;
    RETURN_NULL_IF_FALSE(CommonFunAni::StringToAniStr(env, remoteAbilityInfo.label, label));

    // icon: string
    ani_string icon = nullptr;
    RETURN_NULL_IF_FALSE(CommonFunAni::StringToAniStr(env, remoteAbilityInfo.icon, icon));

    ani_value args[] = {
        { .r = elementName },
        { .r = label },
        { .r = icon },
    };
    static const std::string ctorSig =
        arkts::ani_signature::SignatureBuilder()
            .AddClass(CLASSNAME_ELEMENT_NAME)           // elementName: ElementName
            .AddClass(CommonFunAniNS::CLASSNAME_STRING) // label: string
            .AddClass(CommonFunAniNS::CLASSNAME_STRING) // icon: string
            .BuildSignatureDescriptor();
    return CommonFunAni::CreateNewObjectByClassV2(env, CLASSNAME_REMOTE_ABILITY_INFO, ctorSig, args);
}
} // AniDistributedbundleManagerCommon
} // AppExecFwk
} // OHOS