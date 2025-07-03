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

#include "distributed_helper.h"

#include "app_log_wrapper.h"
#include "bundle_errors.h"
#include "business_error.h"
#include "common_func.h"
#include "distributed_bms_interface.h"
#include "distributed_bms_proxy.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace AppExecFwk {

static OHOS::sptr<OHOS::AppExecFwk::IDistributedBms> GetDistributedBundleMgr()
{
    APP_LOGD("GetDistributedBundleMgr start");
    auto samgr = OHOS::SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (samgr == nullptr) {
        APP_LOGE("GetDistributedBundleMgr samgr is nullptr");
        return nullptr;
    }
    auto remoteObject = samgr->GetSystemAbility(OHOS::DISTRIBUTED_BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    if (remoteObject == nullptr) {
        APP_LOGE("GetDistributedBundleMgr remoteObject is nullptr");
        return nullptr;
    }
    auto distributeBundleMgr = OHOS::iface_cast<IDistributedBms>(remoteObject);
    if (distributeBundleMgr == nullptr) {
        APP_LOGE("GetDistributedBundleMgr distributeBundleMgr is nullptr");
        return nullptr;
    }
    return distributeBundleMgr;
}

int32_t DistributedHelper::InnerGetRemoteAbilityInfo(const std::vector<ElementName> &elementNames,
    const std::string &locale, bool isArray, std::vector<RemoteAbilityInfo> &remoteAbilityInfos)
{
    if (elementNames.size() == 0) {
        APP_LOGE("InnerGetRemoteAbilityInfos elementNames is empty");
        return ERROR_PARAM_CHECK_ERROR;
    }
    auto iDistBundleMgr = GetDistributedBundleMgr();
    if (iDistBundleMgr == nullptr) {
        APP_LOGE("can not get iDistBundleMgr");
        return ERROR_DISTRIBUTED_SERVICE_NOT_RUNNING;
    }
    int32_t result;
    if (isArray) {
        result = iDistBundleMgr->GetRemoteAbilityInfos(elementNames, locale, remoteAbilityInfos);
    } else {
        RemoteAbilityInfo remoteAbilityInfo;
        result = iDistBundleMgr->GetRemoteAbilityInfo(elementNames[0], locale, remoteAbilityInfo);
        remoteAbilityInfos.push_back(remoteAbilityInfo);
    }
    if (result != 0) {
        APP_LOGE("InnerGetRemoteAbilityInfo failed");
    }
    return CommonFunc::ConvertErrCode(result);
}
} // AppExecFwk
} // OHOS