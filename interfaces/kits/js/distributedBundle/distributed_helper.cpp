/*
 * Copyright (c) 2025-2026 Huawei Device Co., Ltd.
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

#include "distributed_helper.h"

#include "app_log_wrapper.h"
#include "bundle_errors.h"
#include "business_error.h"
#include "common_func.h"
#include "distributed_bms_interface.h"
#include "distributed_bms_proxy.h"
#include "distributed_bundle_mgr_client.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace AppExecFwk {
int32_t DistributedHelper::InnerGetRemoteAbilityInfo(const std::vector<ElementName> &elementNames,
    const std::string &locale, bool isArray, std::vector<RemoteAbilityInfo> &remoteAbilityInfos)
{
    if (elementNames.size() == 0) {
        APP_LOGE("InnerGetRemoteAbilityInfo elementNames is empty");
        return ERROR_PARAM_CHECK_ERROR;
    }
    int32_t result;
    if (isArray) {
        result = DistributedBundleMgrClient::GetInstance()->GetRemoteAbilityInfos(
            elementNames, locale, remoteAbilityInfos);
    } else {
        RemoteAbilityInfo remoteAbilityInfo;
        result = DistributedBundleMgrClient::GetInstance()->GetRemoteAbilityInfo(
            elementNames[0], locale, remoteAbilityInfo);
        remoteAbilityInfos.push_back(remoteAbilityInfo);
    }
    if (result != 0) {
        APP_LOGE("InnerGetRemoteAbilityInfo failed");
    }
    return CommonFunc::ConvertErrCode(result);
}

int32_t DistributedHelper::InnerGetRemoteBundleVersionCode(const std::string &deviceId, const std::string &bundleName,
    uint32_t &versionCode)
{
    if (deviceId.empty()) {
        APP_LOGE("InnerGetRemoteBundleVersionCode deviceId is empty");
        return ERROR_PARAM_CHECK_ERROR;
    }
    if (bundleName.empty()) {
        APP_LOGE("InnerGetRemoteBundleVersionCode bundleName is empty");
        return ERROR_PARAM_CHECK_ERROR;
    }
    int32_t result = DistributedBundleMgrClient::GetInstance()->GetRemoteBundleVersionCode(
        deviceId, bundleName, versionCode);
    if (result != 0) {
        APP_LOGE("InnerGetRemoteBundleVersionCode failed");
    }
    return CommonFunc::ConvertErrCode(result);
}

int32_t DistributedHelper::InnerGetRemoteMetadata(const std::string &deviceId,
    const std::string &bundleName, std::vector<ModuleMetadata> &metadataInfos)
{
    if (deviceId.empty()) {
        APP_LOGE("InnerGetRemoteMetadata deviceId is empty");
        return ERROR_PARAM_CHECK_ERROR;
    }
    if (bundleName.empty()) {
        APP_LOGE("InnerGetRemoteMetadata bundleName is empty");
        return ERROR_PARAM_CHECK_ERROR;
    }
    int32_t result = DistributedBundleMgrClient::GetInstance()->GetRemoteMetadata(
        deviceId, bundleName, metadataInfos);
    if (result != 0) {
        APP_LOGE("InnerGetRemoteMetadata failed");
    }
    return CommonFunc::ConvertErrCode(result);
}
} // AppExecFwk
} // OHOS