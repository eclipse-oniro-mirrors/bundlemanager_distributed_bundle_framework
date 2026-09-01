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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_DBMS_INCLUDE_DISTRIBUTED_BUNDLE_MGR_CLIENT_H
#define FOUNDATION_APPEXECFWK_SERVICES_DBMS_INCLUDE_DISTRIBUTED_BUNDLE_MGR_CLIENT_H

#include "iremote_object.h"
#include "device_manager_callback.h"
#include "distributed_bms_interface.h"
#include "singleton.h"

namespace OHOS {
namespace AppExecFwk {
class DistributedBundleMgrClient : public DelayedSingleton<DistributedBundleMgrClient> {
public:
    /**
     * @brief get remote ability info
     * @param elementName Indicates the elementName.
     * @param remoteAbilityInfo Indicates the remote ability info.
     * @return Returns result code when get ability info.
     */
    int32_t GetRemoteAbilityInfo(const ElementName &elementName, RemoteAbilityInfo &remoteAbilityInfo);

    /**
     * @brief get remote ability info
     * @param elementName Indicates the elementName.
     * @param localeInfo Indicates the localeInfo.
     * @param remoteAbilityInfo Indicates the remote ability info.
     * @return Returns result code when get remote ability info.
     */
    int32_t GetRemoteAbilityInfo(const ElementName &elementName, const std::string &localeInfo,
        RemoteAbilityInfo &remoteAbilityInfo);

    /**
     * @brief get remote ability infos
     * @param elementNames Indicates the elementNames.
     * @param remoteAbilityInfos Indicates the remote ability infos.
     * @return Returns result code when get remote ability infos.
     */
    int32_t GetRemoteAbilityInfos(const std::vector<ElementName> &elementNames,
        std::vector<RemoteAbilityInfo> &remoteAbilityInfos);

    /**
     * @brief get remote ability infos
     * @param elementNames Indicates the elementNames.
     * @param localeInfo Indicates the localeInfo.
     * @param remoteAbilityInfos Indicates the remote ability infos.
     * @return Returns result code when get remote ability infos.
     */
    int32_t GetRemoteAbilityInfos(const std::vector<ElementName> &elementNames, const std::string &localeInfo,
        std::vector<RemoteAbilityInfo> &remoteAbilityInfos);

    bool GetDistributedBundleInfo(const std::string &networkId, const std::string &bundleName,
        DistributedBundleInfo &distributedBundleInfo);

    /**
     * @brief get distributedBundleName based on a given accessTokenId and networkId.
     * @param networkId Indicates the networkId of remote device.
     * @param accessTokenId AccessTokenId of the application
     * @param bundleNames distributed bundle name.
     * @return Returns ERR_OK on success, others on failure when get distributed bundle name.
     */
    int32_t GetDistributedBundleName(const std::string &networkId, uint32_t accessTokenId, std::string &bundleName);

    /**
     * @brief get remote bundle version code
     * @param deviceId Indicates the deviceId of remote device.
     * @param bundleName Indicates the bundleName.
     * @param versionCode Indicates the version code.
     * @return Returns ERR_OK on success, others on failure when get remote bundle version code.
     */
    int32_t GetRemoteBundleVersionCode(const std::string &deviceId, const std::string &bundleName,
        uint32_t &versionCode);

    int32_t GetRemoteMetadata(const std::string &networkId, const std::string &bundleName,
        std::vector<ModuleMetadata> &metadataInfos);

    void ResetDistributedBundleMgrProxy();
private:
    sptr<IDistributedBms> dProxy_;
    std::mutex dProxyMutex_;
    std::shared_ptr<DistributedHardware::DmInitCallback> initCallback_;
    std::mutex isInitMutex_;
    bool isInit_ = false;
    sptr<IRemoteObject::DeathRecipient> recipient_;
    std::mutex getProxyMutex_;

    sptr<IDistributedBms> GetDistributedBundleMgrProxy();
    bool LoadDistributedBundleMgrService();
    bool InitDeviceManager();

class DeviceInitCallBack : public DistributedHardware::DmInitCallback {
    void OnRemoteDied() override;
};
};
} // namespace AppExecFwk
} // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_SERVICES_DBMS_INCLUDE_DISTRIBUTED_BUNDLE_MGR_CLIENT_H
