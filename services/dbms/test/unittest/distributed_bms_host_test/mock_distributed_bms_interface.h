/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#ifndef FOUNDATION_APPEXECFWK_MOCK_DISTRIBUTED_BMS_HOST_H
#define FOUNDATION_APPEXECFWK_MOCK_DISTRIBUTED_BMS_HOST_H

#include <gmock/gmock.h>

#define private public
#include "distributed_bms_host.h"
#undef private

namespace OHOS {
namespace AppExecFwk {
class MockDistributedBmsHost : public DistributedBmsHost {
public:
    MockDistributedBmsHost() = default;
    ~MockDistributedBmsHost() override;
    int32_t GetRemoteAbilityInfo(const OHOS::AppExecFwk::ElementName &elementName,
        RemoteAbilityInfo &remoteAbilityInfo) override;
    int32_t GetRemoteAbilityInfo(const OHOS::AppExecFwk::ElementName &elementName,
        const std::string &localeInfo, RemoteAbilityInfo &remoteAbilityInfo) override;
    int32_t GetRemoteAbilityInfos(
        const std::vector<ElementName> &elementNames, std::vector<RemoteAbilityInfo> &remoteAbilityInfos) override;
    int32_t GetRemoteAbilityInfos(const std::vector<ElementName> &elementNames, const std::string &localeInfo,
        std::vector<RemoteAbilityInfo> &remoteAbilityInfos) override;
    int32_t GetAbilityInfo(
        const OHOS::AppExecFwk::ElementName &elementName, RemoteAbilityInfo &remoteAbilityInfo) override;
    int32_t GetAbilityInfo(const OHOS::AppExecFwk::ElementName &elementName, const std::string &localeInfo,
        RemoteAbilityInfo &remoteAbilityInfo) override;
    int32_t GetAbilityInfos(
        const std::vector<ElementName> &elementNames, std::vector<RemoteAbilityInfo> &remoteAbilityInfos) override;
    int32_t GetAbilityInfos(const std::vector<ElementName> &elementNames, const std::string &localeInfo,
        std::vector<RemoteAbilityInfo> &remoteAbilityInfos) override;
    bool GetDistributedBundleInfo(const std::string &networkId, const std::string &bundleName,
        DistributedBundleInfo &distributedBundleInfo) override;
    int32_t GetDistributedBundleName(const std::string &networkId,  uint32_t accessTokenId,
        std::string &bundleName) override;
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_MOCK_DISTRIBUTED_BMS_HOST_H