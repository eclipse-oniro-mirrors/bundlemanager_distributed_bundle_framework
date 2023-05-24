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
#include "mock_distributed_bms_interface.h"

namespace OHOS {
namespace AppExecFwk {
MockDistributedBmsHost::~MockDistributedBmsHost()
{}

int32_t MockDistributedBmsHost::GetRemoteAbilityInfo(
    const OHOS::AppExecFwk::ElementName &elementName, RemoteAbilityInfo &remoteAbilityInfo)
{
    return 0;
}

int32_t MockDistributedBmsHost::GetRemoteAbilityInfo(const OHOS::AppExecFwk::ElementName &elementName,
    const std::string &localeInfo, RemoteAbilityInfo &remoteAbilityInfo)
{
    return 0;
}

int32_t MockDistributedBmsHost::GetRemoteAbilityInfos(
    const std::vector<ElementName> &elementNames, std::vector<RemoteAbilityInfo> &remoteAbilityInfos)
{
    return 0;
}

int32_t MockDistributedBmsHost::GetRemoteAbilityInfos(const std::vector<ElementName> &elementNames,
    const std::string &localeInfo, std::vector<RemoteAbilityInfo> &remoteAbilityInfos)
{
    return 0;
}

int32_t MockDistributedBmsHost::GetAbilityInfo(
    const OHOS::AppExecFwk::ElementName &elementName, RemoteAbilityInfo &remoteAbilityInfo)
{
    return 0;
}

int32_t MockDistributedBmsHost::GetAbilityInfo(const OHOS::AppExecFwk::ElementName &elementName,
    const std::string &localeInfo, RemoteAbilityInfo &remoteAbilityInfo)
{
    return 0;
}

int32_t MockDistributedBmsHost::GetAbilityInfos(
    const std::vector<ElementName> &elementNames, std::vector<RemoteAbilityInfo> &remoteAbilityInfos)
{
    return 0;
}

int32_t MockDistributedBmsHost::GetAbilityInfos(const std::vector<ElementName> &elementNames,
    const std::string &localeInfo, std::vector<RemoteAbilityInfo> &remoteAbilityInfos)
{
    return 0;
}

bool MockDistributedBmsHost::GetDistributedBundleInfo(
    const std::string &networkId, const std::string &bundleName, DistributedBundleInfo &distributedBundleInfo)
{
    return true;
}

int32_t MockDistributedBmsHost::GetDistributedBundleName(const std::string &networkId, uint32_t accessTokenId,
    std::string &bundleName)
{
    return 0;
}
} // namespace AppExecFwk
} // namespace OHOS