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
#include "app_log_wrapper.h"
#include "bundle_errors.h"
#include "business_error_ani.h"
#include "common_fun_ani.h"
#include "distributed_helper.h"
#include "napi_constants.h"

namespace OHOS {
namespace AppExecFwk {
namespace {
constexpr const char* NS_NAME_DISTRIBUTEDMANAGER = "@ohos.bundle.distributedBundleManager.distributedBundleManager";
} // namespace

static ani_object GetRemoteAbilityInfoInner(ani_env *env,
    ani_object aniElementNames, ani_string aniLocale, bool isArray)
{
    std::vector<ElementName> elementNames;
    if (!CommonFunAni::ParseAniArray(env, aniElementNames, elementNames, CommonFunAni::ParseElementName)) {
        APP_LOGE("ParseAniArray ElementNames failed");
        BusinessErrorAni::ThrowCommonError(env, ERROR_PARAM_CHECK_ERROR, PARAMETER_ELEMENT_NAME, TYPE_OBJECT);
        return nullptr;
    }

    std::string locale;
    if (!CommonFunAni::ParseString(env, aniLocale, locale)) {
        APP_LOGE("parse locale failed");
        BusinessErrorAni::ThrowCommonError(env, ERROR_PARAM_CHECK_ERROR, PARAMETER_LOCALE, TYPE_STRING);
        return nullptr;
    }

    if (elementNames.size() > GET_REMOTE_ABILITY_INFO_MAX_SIZE) {
        BusinessErrorAni::ThrowError(env, ERROR_PARAM_CHECK_ERROR,
            "BusinessError 401: The number of ElementNames is greater than 10");
        return nullptr;
    }

    std::vector<RemoteAbilityInfo> remoteAbilityInfos;
    int32_t ret = DistributedHelper::InnerGetRemoteAbilityInfo(elementNames, locale, isArray, remoteAbilityInfos);
    if (ret != ERR_OK) {
        APP_LOGE("InnerGetRemoteAbilityInfo failed ret: %{public}d", ret);
        BusinessErrorAni::ThrowCommonError(env, ret,
            RESOURCE_NAME_GET_REMOTE_ABILITY_INFO, Constants::PERMISSION_GET_BUNDLE_INFO_PRIVILEGED);
        return nullptr;
    }

    if (isArray) {
        ani_object remoteAbilityInfosObject = CommonFunAni::ConvertAniArray(env, remoteAbilityInfos,
            AniDistributedbundleManagerCommon::ConvertRemoteAbilityInfo);
        if (remoteAbilityInfosObject == nullptr) {
            APP_LOGE("nullptr remoteAbilityInfosObject");
            return nullptr;
        }
        return remoteAbilityInfosObject;
    } else {
        return AniDistributedbundleManagerCommon::ConvertRemoteAbilityInfo(env, remoteAbilityInfos[0]);
    }
}

static ani_object AniGetRemoteAbilityInfo(ani_env *env, ani_object aniElementNames, ani_string aniLocale)
{
    APP_LOGD("ani GetRemoteAbilityInfo called");
    return GetRemoteAbilityInfoInner(env, aniElementNames, aniLocale, false);
}

static ani_object AniGetRemoteAbilityInfos(ani_env *env, ani_object aniElementNames, ani_string aniLocale)
{
    APP_LOGD("ani GetRemoteAbilityInfos called");
    return GetRemoteAbilityInfoInner(env, aniElementNames, aniLocale, true);
}

static ani_long AniGetRemoteBundleVersionCode(ani_env *env, ani_string aniDeviceId, ani_string aniBundleName)
{
    APP_LOGD("ani GetRemoteBundleVersionCode called");
    
    if (aniDeviceId == nullptr) {
        APP_LOGE("aniDeviceId is null");
        BusinessErrorAni::ThrowCommonError(env, ERROR_PARAM_CHECK_ERROR, PARAMETER_DEVICE_ID, TYPE_STRING);
        return 0;
    }
    
    if (aniBundleName == nullptr) {
        APP_LOGE("aniBundleName is null");
        BusinessErrorAni::ThrowCommonError(env, ERROR_PARAM_CHECK_ERROR, PARAMETER_BUNDLE_NAME, TYPE_STRING);
        return 0;
    }
    
    std::string deviceId;
    if (!CommonFunAni::ParseString(env, aniDeviceId, deviceId)) {
        APP_LOGE("parse deviceId failed");
        BusinessErrorAni::ThrowCommonError(env, ERROR_PARAM_CHECK_ERROR, PARAMETER_DEVICE_ID, TYPE_STRING);
        return 0;
    }
    
    std::string bundleName;
    if (!CommonFunAni::ParseString(env, aniBundleName, bundleName)) {
        APP_LOGE("parse bundleName failed");
        BusinessErrorAni::ThrowCommonError(env, ERROR_PARAM_CHECK_ERROR, PARAMETER_BUNDLE_NAME, TYPE_STRING);
        return 0;
    }
    
    if (deviceId.empty()) {
        APP_LOGE("deviceId is empty");
        BusinessErrorAni::ThrowCommonError(env, ERROR_PARAM_CHECK_ERROR, PARAMETER_DEVICE_ID, TYPE_STRING);
        return 0;
    }
    
    if (bundleName.empty()) {
        APP_LOGE("bundleName is empty");
        BusinessErrorAni::ThrowCommonError(env, ERROR_PARAM_CHECK_ERROR, PARAMETER_BUNDLE_NAME, TYPE_STRING);
        return 0;
    }
    
    uint32_t versionCode = 0;
    int32_t ret = DistributedHelper::InnerGetRemoteBundleVersionCode(deviceId, bundleName, versionCode);
    if (ret != ERR_OK) {
        APP_LOGE("InnerGetRemoteBundleVersionCode failed ret: %{public}d", ret);
        BusinessErrorAni::ThrowCommonError(env, ret,
            RESOURCE_NAME_GET_REMOTE_BUNDLE_VERSION_CODE, Constants::PERMISSION_GET_BUNDLE_INFO_PRIVILEGED);
        return 0;
    }

    return static_cast<ani_long>(versionCode);
}

extern "C" {
ANI_EXPORT ani_status ANI_Constructor(ani_vm* vm, uint32_t* result)
{
    APP_LOGI("ANI_Constructor distributedBundleManager called");
    ani_env* env;
    ani_status status = vm->GetEnv(ANI_VERSION_1, &env);
    RETURN_ANI_STATUS_IF_NOT_OK(status, "Unsupported ANI_VERSION_1");

    arkts::ani_signature::Namespace naName = arkts::ani_signature::Builder::BuildNamespace(NS_NAME_DISTRIBUTEDMANAGER);
    ani_namespace kitNs = nullptr;
    status = env->FindNamespace(naName.Descriptor().c_str(), &kitNs);
    if (status != ANI_OK) {
        APP_LOGE("FindNamespace: %{public}s fail with %{public}d", NS_NAME_DISTRIBUTEDMANAGER, status);
        return status;
    }

    std::array methods = {
        ani_native_function { "getRemoteAbilityInfoNative", nullptr, reinterpret_cast<void*>(AniGetRemoteAbilityInfo) },
        ani_native_function { "getRemoteAbilityInfosNative", nullptr,
            reinterpret_cast<void*>(AniGetRemoteAbilityInfos) },
        ani_native_function { "getRemoteBundleVersionCodeNative", nullptr,
            reinterpret_cast<void*>(AniGetRemoteBundleVersionCode) }
    };

    status = env->Namespace_BindNativeFunctions(kitNs, methods.data(), methods.size());
    if (status != ANI_OK) {
        APP_LOGE("Namespace_BindNativeFunctions: %{public}s fail with %{public}d", NS_NAME_DISTRIBUTEDMANAGER, status);
        return status;
    }

    *result = ANI_VERSION_1;

    APP_LOGI("ANI_Constructor finished");

    return ANI_OK;
}
}
} // AppExecFwk
} // OHOS