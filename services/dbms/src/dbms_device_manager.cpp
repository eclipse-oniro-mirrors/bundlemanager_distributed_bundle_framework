/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#include "dbms_device_manager.h"

#include "app_log_wrapper.h"
#include "bundle_constants.h"
#include "device_manager.h"
#include "service_control.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace AppExecFwk {
namespace {
    const std::string DISTRIBUTED_BUNDLE_NAME = "distributed_bundle_framework";
    const std::string SERVICES_NAME = "d-bms";
}

DbmsDeviceManager::DbmsDeviceManager()
{
    APP_LOGI("DbmsDeviceManager instance is created");
}

bool DbmsDeviceManager::InitDeviceManager()
{
    std::lock_guard<std::mutex> lock(isInitMutex_);
    if (isInit_) {
        APP_LOGI("device manager already init");
        return true;
    }

    initCallback_ = std::make_shared<DeviceInitCallBack>();
    int32_t ret =
        DistributedHardware::DeviceManager::GetInstance().InitDeviceManager(DISTRIBUTED_BUNDLE_NAME, initCallback_);
    if (ret != 0) {
        APP_LOGE("init device manager failed, ret:%{public}d", ret);
        return false;
    }
    isInit_ = true;
    APP_LOGI("register device manager success");
    return true;
}

void DbmsDeviceManager::DeviceInitCallBack::OnRemoteDied()
{
    APP_LOGI("DeviceInitCallBack OnRemoteDied");
}

int32_t DbmsDeviceManager::GetUdidByNetworkId(const std::string &netWorkId, std::string &udid)
{
    APP_LOGI("GetUdidByNetworkId");
    if (!InitDeviceManager()) {
        return -1;
    }
    return DistributedHardware::DeviceManager::GetInstance().GetUdidByNetworkId(
        DISTRIBUTED_BUNDLE_NAME, netWorkId, udid);
}
}
}