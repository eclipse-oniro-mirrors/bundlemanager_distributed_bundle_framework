/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_D_BUNDLEMGR_INCLUDE_DISTRIBUTED_BMS_H
#define FOUNDATION_APPEXECFWK_SERVICES_D_BUNDLEMGR_INCLUDE_DISTRIBUTED_BMS_H

#include <memory>

#include "bundle_info.h"
#include "bundle_mgr_interface.h"
#include "dbms_device_manager.h"
#include "distributed_bms_host.h"
#include "distributed_monitor.h"
#include "if_system_ability_manager.h"
#include "iremote_object.h"
#include "resource_manager.h"
#include "singleton.h"
#include "system_ability.h"
namespace OHOS {
namespace AppExecFwk {
class DistributedBms : public SystemAbility, public DistributedBmsHost {
    DECLARE_DELAYED_SINGLETON(DistributedBms);
    DECLARE_SYSTEM_ABILITY(DistributedBms);

public:
    OHOS::sptr<OHOS::AppExecFwk::IBundleMgr> GetBundleMgr();

    OHOS::sptr<DbmsDeviceManager> GetDeviceManager();
    /**
     * @brief get remote ability info
     * @param elementName Indicates the elementName.
     * @param remoteAbilityInfo Indicates the remote ability info.
     * @return Returns true when get remote ability info success; returns false otherwise.
     */
    int32_t GetRemoteAbilityInfo(
        const OHOS::AppExecFwk::ElementName &elementName, RemoteAbilityInfo &remoteAbilityInfo) override;

    /**
     * @brief get remote ability info
     * @param elementName Indicates the elementName.
     * @param localeInfo Indicates the localeInfo.
     * @param remoteAbilityInfo Indicates the remote ability info.
     * @return Returns true when get remote ability info success; returns false otherwise.
     */
    int32_t GetRemoteAbilityInfo(const OHOS::AppExecFwk::ElementName &elementName, const std::string &localeInfo,
        RemoteAbilityInfo &remoteAbilityInfo) override;
    /**
     * @brief get remote ability infos
     * @param elementNames Indicates the elementNames.
     * @param remoteAbilityInfos Indicates the remote ability infos.
     * @return Returns true when get remote ability info success; returns false otherwise.
     */
    int32_t GetRemoteAbilityInfos(
        const std::vector<ElementName> &elementNames, std::vector<RemoteAbilityInfo> &remoteAbilityInfos) override;

    /**
     * @brief get remote ability infos
     * @param elementNames Indicates the elementNames.
     * @param localeInfo Indicates the localeInfo.
     * @param remoteAbilityInfos Indicates the remote ability infos.
     * @return Returns true when get remote ability info success; returns false otherwise.
     */
    int32_t GetRemoteAbilityInfos(const std::vector<ElementName> &elementNames, const std::string &localeInfo,
        std::vector<RemoteAbilityInfo> &remoteAbilityInfos) override;

    /**
     * @brief get ability info
     * @param elementName Indicates the elementName.
     * @param remoteAbilityInfo Indicates the remote ability info.
     * @return Returns true when get remote ability info success; returns false otherwise.
     */
    int32_t GetAbilityInfo(
        const OHOS::AppExecFwk::ElementName &elementName, RemoteAbilityInfo &remoteAbilityInfo) override;

    /**
     * @brief get ability info
     * @param elementName Indicates the elementName.
     * @param localeInfo Indicates the localeInfo.
     * @param remoteAbilityInfo Indicates the remote ability info.
     * @return Returns true when get remote ability info success; returns false otherwise.
     */
    int32_t GetAbilityInfo(const OHOS::AppExecFwk::ElementName &elementName, const std::string &localeInfo,
        RemoteAbilityInfo &remoteAbilityInfo) override;
    /**
     * @brief get ability infos
     * @param elementNames Indicates the elementNames.
     * @param remoteAbilityInfos Indicates the remote ability infos.
     * @return Returns true when get remote ability info success; returns false otherwise.
     */
    int32_t GetAbilityInfos(
        const std::vector<ElementName> &elementNames, std::vector<RemoteAbilityInfo> &remoteAbilityInfos) override;

    /**
     * @brief get ability infos
     * @param elementNames Indicates the elementNames.
     * @param localeInfo Indicates the localeInfo.
     * @param remoteAbilityInfos Indicates the remote ability infos.
     * @return Returns true when get remote ability info success; returns false otherwise.
     */
    int32_t GetAbilityInfos(const std::vector<ElementName> &elementNames, const std::string &localeInfo,
        std::vector<RemoteAbilityInfo> &remoteAbilityInfos) override;

    bool GetDistributedBundleInfo(const std::string &networkId, const std::string &bundleName,
        DistributedBundleInfo &distributedBundleInfo) override;

    /**
     * @brief get distributedBundleName based on a given accessTokenId and networkId.
     * @param networkId Indicates the networkId of remote device.
     * @param accessTokenId AccessTokenId of the application
     * @param bundleNames distributed bundle name.
     * @return Returns ERR_OK on success, others on failure when get distributed bundle name.
     */
    int32_t GetDistributedBundleName(const std::string &networkId,  uint32_t accessTokenId,
        std::string &bundleName) override;

    int32_t GetUdidByNetworkId(const std::string &networkId, std::string &udid);

    /**
     * @brief Start the bundle manager service.
     * @return
     */
    virtual void OnStart() override;
    /**
     * @brief Stop the bundle manager service.
     * @return
     */
    virtual void OnStop() override;

private:
    OHOS::sptr<OHOS::AppExecFwk::IBundleMgr> bundleMgr_;
    std::shared_ptr<DbmsDeviceManager> dbmsDeviceManager_;
    std::shared_ptr<DistributedMonitor> distributedSub_;
    std::mutex bundleMgrMutex_;

    void Init();
    void InitDeviceManager();
    bool GetMediaBase64(std::unique_ptr<uint8_t[]> &data, int64_t fileLength,
        std::string &imageType, std::string &value);
    std::unique_ptr<unsigned char[]> LoadResourceFile(std::string &path, int &len);
    std::unique_ptr<char[]> EncodeBase64(std::unique_ptr<uint8_t[]> &data, int srcLen);
    int32_t GetAbilityIconByContent(
        const AbilityInfo &abilityInfo, int32_t userId, RemoteAbilityInfo &remoteAbilityInfo);
    int32_t Base64WithoutCompress(std::unique_ptr<uint8_t[]> &imageContent, size_t imageContentSize,
        RemoteAbilityInfo &remoteAbilityInfo);
    bool VerifySystemApp();
    bool VerifyTokenNative(Security::AccessToken::AccessTokenID callerToken);
    bool VerifyTokenShell(Security::AccessToken::AccessTokenID callerToken);
    bool VerifyCallingPermission(const std::string &permissionName);
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_SERVICES_D_BUNDLEMGR_INCLUDE_DISTRIBUTED_BMS_H