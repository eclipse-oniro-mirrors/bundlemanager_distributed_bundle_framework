/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "distributed_bundle_mgr_client.h"

#include <chrono>
#include <thread>
#include "app_log_wrapper.h"
#include "device_manager.h"
#include "distributed_bundle_mgr_death_recipient.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace AppExecFwk {
namespace {
    constexpr int16_t LOAD_SA_TIMEOUT_MS = 4 * 1000;
    constexpr int32_t ERROR_DISTRIBUTED_SERVICE_NOT_RUNNING = 17700027;
    constexpr const char* PKG_NAME = "distributed_bundle_framework";
}
bool DistributedBundleMgrClient::InitDeviceManager()
{
    std::lock_guard<std::mutex> lock(isInitMutex_);
    if (isInit_) {
        APP_LOGI_NOFUNC("device manager already init");
        return true;
    }

    initCallback_ = std::make_shared<DeviceInitCallBack>();
    auto ret = DistributedHardware::DeviceManager::GetInstance().InitDeviceManager(PKG_NAME, initCallback_);
    if (ret != ERR_OK) {
        APP_LOGE_NOFUNC("init device manager failed, ret:%{public}d", ret);
        return false;
    }
    isInit_ = true;
    APP_LOGI_NOFUNC("register device manager success");
    return true;
}

void DistributedBundleMgrClient::DeviceInitCallBack::OnRemoteDied()
{
    APP_LOGI_NOFUNC("DeviceInitCallBack OnRemoteDied");
}

int32_t DistributedBundleMgrClient::GetRemoteAbilityInfo(const ElementName &elementName,
    RemoteAbilityInfo &remoteAbilityInfo)
{
    auto proxy = GetDistributedBundleMgrProxy();
    if (proxy == nullptr) {
        APP_LOGE_NOFUNC("GetDistributedBundleMgrProxy failed");
        return ERROR_DISTRIBUTED_SERVICE_NOT_RUNNING;
    }
    return proxy->GetRemoteAbilityInfo(elementName, remoteAbilityInfo);
}

int32_t DistributedBundleMgrClient::GetRemoteAbilityInfo(const ElementName &elementName,
    const std::string &localeInfo, RemoteAbilityInfo &remoteAbilityInfo)
{
    auto proxy = GetDistributedBundleMgrProxy();
    if (proxy == nullptr) {
        APP_LOGE_NOFUNC("GetDistributedBundleMgrProxy failed");
        return ERROR_DISTRIBUTED_SERVICE_NOT_RUNNING;
    }
    return proxy->GetRemoteAbilityInfo(elementName, localeInfo, remoteAbilityInfo);
}

int32_t DistributedBundleMgrClient::GetRemoteAbilityInfos(const std::vector<ElementName> &elementNames,
    std::vector<RemoteAbilityInfo> &remoteAbilityInfos)
{
    auto proxy = GetDistributedBundleMgrProxy();
    if (proxy == nullptr) {
        APP_LOGE_NOFUNC("GetDistributedBundleMgrProxy failed");
        return ERROR_DISTRIBUTED_SERVICE_NOT_RUNNING;
    }
    return proxy->GetRemoteAbilityInfos(elementNames, remoteAbilityInfos);
}

int32_t DistributedBundleMgrClient::GetRemoteAbilityInfos(const std::vector<ElementName> &elementNames,
    const std::string &localeInfo, std::vector<RemoteAbilityInfo> &remoteAbilityInfos)
{
    auto proxy = GetDistributedBundleMgrProxy();
    if (proxy == nullptr) {
        APP_LOGE_NOFUNC("GetDistributedBundleMgrProxy failed");
        return ERROR_DISTRIBUTED_SERVICE_NOT_RUNNING;
    }
    return proxy->GetRemoteAbilityInfos(elementNames, localeInfo, remoteAbilityInfos);
}

bool DistributedBundleMgrClient::GetDistributedBundleInfo(const std::string &networkId, const std::string &bundleName,
    DistributedBundleInfo &distributedBundleInfo)
{
    auto proxy = GetDistributedBundleMgrProxy();
    if (proxy == nullptr) {
        APP_LOGE_NOFUNC("GetDistributedBundleMgrProxy failed");
        return false;
    }
    return proxy->GetDistributedBundleInfo(networkId, bundleName, distributedBundleInfo);
}

int32_t DistributedBundleMgrClient::GetDistributedBundleName(const std::string &networkId, uint32_t accessTokenId,
    std::string &bundleName)
{
    auto proxy = GetDistributedBundleMgrProxy();
    if (proxy == nullptr) {
        APP_LOGE_NOFUNC("GetDistributedBundleMgrProxy failed");
        return ERROR_DISTRIBUTED_SERVICE_NOT_RUNNING;
    }
    return proxy->GetDistributedBundleName(networkId, accessTokenId, bundleName);
}


int32_t DistributedBundleMgrClient::GetRemoteBundleVersionCode(const std::string &deviceId,
    const std::string &bundleName, uint32_t &versionCode)
{
    auto proxy = GetDistributedBundleMgrProxy();
    if (proxy == nullptr) {
        APP_LOGE_NOFUNC("GetDistributedBundleMgrProxy failed");
        return ERROR_DISTRIBUTED_SERVICE_NOT_RUNNING;
    }
    return proxy->GetRemoteBundleVersionCode(deviceId, bundleName, versionCode);
}

int32_t DistributedBundleMgrClient::GetRemoteMetadata(const std::string &networkId,
    const std::string &bundleName, std::vector<ModuleMetadata> &metadataInfos)
{
    auto proxy = GetDistributedBundleMgrProxy();
    if (proxy == nullptr) {
        APP_LOGE_NOFUNC("GetDistributedBundleMgrProxy failed");
        return ERROR_DISTRIBUTED_SERVICE_NOT_RUNNING;
    }
    return proxy->GetRemoteMetadata(networkId, bundleName, metadataInfos);
}

void DistributedBundleMgrClient::ResetDistributedBundleMgrProxy()
{
    std::lock_guard<std::mutex> lock(dProxyMutex_);
    if ((dProxy_ != nullptr) && (dProxy_->AsObject() != nullptr)) {
        dProxy_->AsObject()->RemoveDeathRecipient(recipient_);
    }
    dProxy_ = nullptr;
}

sptr<IDistributedBms> DistributedBundleMgrClient::GetDistributedBundleMgrProxy()
{
    std::lock_guard<std::mutex> lockProxy(getProxyMutex_);
    {
        std::lock_guard<std::mutex> lock(dProxyMutex_);
        if (dProxy_ != nullptr) {
            APP_LOGD("distributedBundleMgr ready");
            return dProxy_;
        }
    }

    if (!LoadDistributedBundleMgrService()) {
        APP_LOGE_NOFUNC("LoadDistributedBundleMgrService failed");
        return nullptr;
    }
    std::lock_guard<std::mutex> lock(dProxyMutex_);
    if ((dProxy_ == nullptr) || (dProxy_->AsObject() == nullptr)) {
        APP_LOGE_NOFUNC("the distributed bundle mgr proxy or remote object is nullptr");
        return nullptr;
    }

    recipient_ = new (std::nothrow) DistributedBundleMgrDeathRecipient(this);
    if (recipient_ == nullptr) {
        APP_LOGE_NOFUNC("the death recipient is nullptr");
        return nullptr;
    }
    dProxy_->AsObject()->AddDeathRecipient(recipient_);
    return dProxy_;
}

bool DistributedBundleMgrClient::LoadDistributedBundleMgrService()
{
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (samgr == nullptr) {
        APP_LOGE_NOFUNC("GetSystemAbilityManager failed");
        return false;
    }
    {
        std::unique_lock<std::mutex> lock(dProxyMutex_);
        sptr<IRemoteObject> remoteObject = samgr->CheckSystemAbility(DISTRIBUTED_BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
        if (remoteObject != nullptr) {
            dProxy_ = iface_cast<IDistributedBms>(remoteObject);
            return true;
        }
    }
    if (!InitDeviceManager()) {
        return false;
    }
    std::vector<DistributedHardware::DmDeviceInfo> dmDeviceInfos;
    auto ret = DistributedHardware::DeviceManager::GetInstance().GetTrustedDeviceList(PKG_NAME, "", dmDeviceInfos);
    if (ret != ERR_OK) {
        APP_LOGE_NOFUNC("GetTrustedDeviceList failed %{public}d", ret);
        return false;
    }
    if (dmDeviceInfos.empty()) {
        APP_LOGE_NOFUNC("device is not networked");
        return false;
    }
    std::unique_lock<std::mutex> lock(dProxyMutex_);
    sptr<IRemoteObject> remoteObject =
        samgr->LoadSystemAbility(DISTRIBUTED_BUNDLE_MGR_SERVICE_SYS_ABILITY_ID, LOAD_SA_TIMEOUT_MS);
    if (remoteObject != nullptr) {
        dProxy_ = iface_cast<IDistributedBms>(remoteObject);
        return true;
    }
    return false;
}
}
}
