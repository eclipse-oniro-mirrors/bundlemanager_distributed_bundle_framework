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

#include "distributed_bms_proxy.h"

#include "app_log_wrapper.h"
#include "appexecfwk_errors.h"
#include "parcel_macro.h"

namespace OHOS {
namespace AppExecFwk {
DistributedBmsProxy::DistributedBmsProxy(const sptr<IRemoteObject> &object) : IRemoteProxy<IDistributedBms>(object)
{
    APP_LOGI("DistributedBmsProxy instance is created");
}

DistributedBmsProxy::~DistributedBmsProxy()
{
    APP_LOGI("DistributedBmsProxy instance is destroyed");
}

int32_t DistributedBmsProxy::GetRemoteAbilityInfo(
    const OHOS::AppExecFwk::ElementName &elementName, RemoteAbilityInfo &remoteAbilityInfo)
{
    APP_LOGD("DistributedBmsProxy GetRemoteAbilityInfo");
    return GetRemoteAbilityInfo(elementName, "", remoteAbilityInfo);
}

int32_t DistributedBmsProxy::GetRemoteAbilityInfo(const OHOS::AppExecFwk::ElementName &elementName,
                                                  const std::string &localeInfo,
                                                  RemoteAbilityInfo &remoteAbilityInfo)
{
    APP_LOGD("DistributedBmsProxy GetRemoteAbilityInfoWithLocale");
    int32_t checkRet = CheckElementName(elementName);
    if (checkRet != ERR_OK) {
        APP_LOGE("DistributedBmsProxy GetRemoteAbilityInfoWithLocale check elementName failed");
        return checkRet;
    }
    MessageParcel data;
    MessageParcel reply;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        APP_LOGE("fail to GetRemoteAbilityInfo due to write InterfaceToken fail");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    if (!data.WriteParcelable(&elementName)) {
        APP_LOGE("DistributedBmsProxy GetRemoteAbilityInfo write elementName error");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    if (!data.WriteString(localeInfo)) {
        APP_LOGE("DistributedBmsProxy GetRemoteAbilityInfo write localeInfo error");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }

    int32_t result = GetParcelableInfo<RemoteAbilityInfo>(
        DistributedInterfaceCode::GET_REMOTE_ABILITY_INFO_WITH_LOCALE, data, remoteAbilityInfo);
    if (result != OHOS::NO_ERROR) {
        APP_LOGE("fail to query ability info mutiparam from server, result:%{public}d", result);
    }
    return result;
}

int32_t DistributedBmsProxy::GetRemoteAbilityInfos(
    const std::vector<ElementName> &elementNames, std::vector<RemoteAbilityInfo> &remoteAbilityInfos)
{
    APP_LOGD("DistributedBmsProxy GetRemoteAbilityInfos");
    return GetRemoteAbilityInfos(elementNames, "", remoteAbilityInfos);
}

int32_t DistributedBmsProxy::GetRemoteAbilityInfos(const std::vector<ElementName> &elementNames,
                                                   const std::string &localeInfo,
                                                   std::vector<RemoteAbilityInfo> &remoteAbilityInfos)
{
    APP_LOGD("DistributedBmsProxy GetRemoteAbilityInfosWithLocale");
    for (const auto &elementName : elementNames) {
        int32_t checkRet = CheckElementName(elementName);
        if (checkRet != ERR_OK) {
            APP_LOGE("DistributedBmsProxy GetRemoteAbilityInfosWithLocale check elementName failed");
            return checkRet;
        }
    }
    MessageParcel data;
    MessageParcel reply;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        APP_LOGE("fail to GetRemoteAbilityInfos due to write InterfaceToken fail");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    if (!WriteParcelableVector(elementNames, data)) {
        APP_LOGE("DistributedBmsProxy GetRemoteAbilityInfos write elementName error");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    if (!data.WriteString(localeInfo)) {
        APP_LOGE("DistributedBmsProxy GetRemoteAbilityInfos write localeInfo error");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }

    int32_t result = GetParcelableInfos<RemoteAbilityInfo>(
        DistributedInterfaceCode::GET_REMOTE_ABILITY_INFOS_WITH_LOCALE, data, remoteAbilityInfos);
    if (result != OHOS::NO_ERROR) {
        APP_LOGE("fail to query remote ability infos mutiparam from server");
    }
    return result;
}

int32_t DistributedBmsProxy::GetAbilityInfo(
    const OHOS::AppExecFwk::ElementName &elementName, RemoteAbilityInfo &remoteAbilityInfo)
{
    APP_LOGD("DistributedBmsProxy GetAbilityInfo");
    return GetAbilityInfo(elementName, "", remoteAbilityInfo);
}

int32_t DistributedBmsProxy::GetAbilityInfo(const OHOS::AppExecFwk::ElementName &elementName,
                                            const std::string &localeInfo,
                                            RemoteAbilityInfo &remoteAbilityInfo)
{
    APP_LOGD("DistributedBmsProxy GetAbilityInfoWithLocale");
    MessageParcel data;
    MessageParcel reply;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        APP_LOGE("fail to GetAbilityInfo due to write InterfaceToken fail");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    if (!data.WriteParcelable(&elementName)) {
        APP_LOGE("DistributedBmsProxy GetAbilityInfo write elementName error");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    if (!data.WriteString(localeInfo)) {
        APP_LOGE("DistributedBmsProxy GetRemoteAbilityInfos write localeInfo error");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    int32_t result = GetParcelableInfo<RemoteAbilityInfo>(
        DistributedInterfaceCode::GET_ABILITY_INFO_WITH_LOCALE, data, remoteAbilityInfo);
    if (result == OHOS::IPC_STUB_UNKNOW_TRANS_ERR) {
        return GetParcelableInfo<RemoteAbilityInfo>(
            DistributedInterfaceCode::GET_ABILITY_INFO, data, remoteAbilityInfo);
    }
    return result;
}

int32_t DistributedBmsProxy::GetAbilityInfos(
    const std::vector<ElementName> &elementNames, std::vector<RemoteAbilityInfo> &remoteAbilityInfos)
{
    APP_LOGD("DistributedBmsProxy GetAbilityInfos");
    return GetAbilityInfos(elementNames, "", remoteAbilityInfos);
}

int32_t DistributedBmsProxy::GetAbilityInfos(const std::vector<ElementName> &elementNames,
                                             const std::string &localeInfo,
                                             std::vector<RemoteAbilityInfo> &remoteAbilityInfos)
{
    APP_LOGD("DistributedBmsProxy GetAbilityInfos");
    MessageParcel data;
    MessageParcel reply;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        APP_LOGE("fail to GetAbilityInfos due to write InterfaceToken fail");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    if (!WriteParcelableVector(elementNames, data)) {
        APP_LOGE("DistributedBmsProxy GetAbilityInfos write elementName error");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    if (!data.WriteString(localeInfo)) {
        APP_LOGE("DistributedBmsProxy GetRemoteAbilityInfos write localeInfo error");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    int32_t result = GetParcelableInfos<RemoteAbilityInfo>(
        DistributedInterfaceCode::GET_ABILITY_INFOS_WITH_LOCALE, data, remoteAbilityInfos);
    if (result == OHOS::IPC_STUB_UNKNOW_TRANS_ERR) {
        return GetParcelableInfos<RemoteAbilityInfo>(
            DistributedInterfaceCode::GET_ABILITY_INFOS, data, remoteAbilityInfos);
    }
    return result;
}

bool DistributedBmsProxy::GetDistributedBundleInfo(const std::string &networkId, const std::string &bundleName,
    DistributedBundleInfo &distributedBundleInfo)
{
    APP_LOGD("DistributedBmsProxy GetDistributedBundleInfo");
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        APP_LOGE("fail to GetDistributedBundleInfo due to write InterfaceToken fail");
        return false;
    }
    if (!data.WriteString(networkId)) {
        APP_LOGE("DistributedBmsProxy GetDistributedBundleInfo write networkId error");
        return false;
    }
    if (!data.WriteString(bundleName)) {
        APP_LOGE("DistributedBmsProxy GetDistributedBundleInfo write bundleName error");
        return false;
    }
    int32_t result = GetParcelableInfo<DistributedBundleInfo>(
        DistributedInterfaceCode::GET_DISTRIBUTED_BUNDLE_INFO, data, distributedBundleInfo);
    if (result == OHOS::NO_ERROR) {
        return true;
    }
    return false;
}

int32_t DistributedBmsProxy::GetDistributedBundleName(const std::string &networkId, uint32_t accessTokenId,
    std::string &bundleName)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        APP_LOGE("fail to get distributed bundleInfo list due to write InterfaceToken fail");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    if (!data.WriteString(networkId)) {
        APP_LOGE("distributed mms proxy get distributed bundleInfo list write networkId error");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    if (!data.WriteUint32(accessTokenId)) {
        APP_LOGE("distributed mms proxy get distributed bundleInfo list write accessTokenId error");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    MessageParcel reply;
    int32_t result = SendRequest(DistributedInterfaceCode::GET_DISTRIBUTED_BUNDLE_NAME, data, reply);
    if (result == OHOS::NO_ERROR) {
        bundleName = reply.ReadString();
    }
    return result;
}

template<typename T>
bool DistributedBmsProxy::WriteParcelableVector(const std::vector<T> &parcelableVector, Parcel &data)
{
    if (!data.WriteInt32(parcelableVector.size())) {
        APP_LOGE("write ParcelableVector failed");
        return false;
    }

    for (auto &parcelable : parcelableVector) {
        if (!data.WriteParcelable(&parcelable)) {
            APP_LOGE("write ParcelableVector failed");
            return false;
        }
    }
    return true;
}

template<typename T>
int32_t DistributedBmsProxy::GetParcelableInfo(DistributedInterfaceCode code, MessageParcel &data, T &parcelableInfo)
{
    MessageParcel reply;
    int32_t result = SendRequest(code, data, reply);
    if (result != OHOS::NO_ERROR) {
        APP_LOGE("reply result false");
        return result;
    }

    if (!reply.ReadBool()) {
        APP_LOGE("reply result false");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }

    std::unique_ptr<T> info(reply.ReadParcelable<T>());
    if (!info) {
        APP_LOGE("readParcelableInfo failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    parcelableInfo = *info;
    APP_LOGD("get parcelable info success");
    return OHOS::NO_ERROR;
}

template<typename T>
int32_t DistributedBmsProxy::GetParcelableInfos(
    DistributedInterfaceCode code, MessageParcel &data, std::vector<T> &parcelableInfos)
{
    MessageParcel reply;
    int32_t result = SendRequest(code, data, reply);
    if (result != OHOS::NO_ERROR) {
        APP_LOGE("reply result false");
        return result;
    }

    if (!reply.ReadBool()) {
        APP_LOGE("reply result false");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }

    int32_t infoSize = reply.ReadInt32();
    CONTAINER_SECURITY_VERIFY(reply, infoSize, &parcelableInfos);
    for (int32_t i = 0; i < infoSize; i++) {
        std::unique_ptr<T> info(reply.ReadParcelable<T>());
        if (!info) {
            APP_LOGE("Read Parcelable infos failed");
            return ERR_APPEXECFWK_PARCEL_ERROR;
        }
        parcelableInfos.emplace_back(*info);
    }
    APP_LOGD("get parcelable infos success");
    return OHOS::NO_ERROR;
}

int32_t DistributedBmsProxy::SendRequest(DistributedInterfaceCode code, MessageParcel &data, MessageParcel &reply)
{
    APP_LOGD("DistributedBmsProxy SendRequest");
    sptr<IRemoteObject> remote = Remote();
    MessageOption option(MessageOption::TF_SYNC);
    if (remote == nullptr) {
        APP_LOGE("fail to send %{public}d cmd to service due to remote object is null", code);
        return ERR_APPEXECFWK_FAILED_GET_REMOTE_PROXY;
    }
    int32_t result = remote->SendRequest(static_cast<uint32_t>(code), data, reply, option);
    if (result != OHOS::NO_ERROR) {
        APP_LOGE("fail to send %{public}d cmd to service due to transact error:%{public}d", code, result);
    }
    return result;
}

int32_t DistributedBmsProxy::CheckElementName(const ElementName &elementName)
{
    if (elementName.GetBundleName().empty()) {
        APP_LOGE("fail to GetRemoteAbilityInfo due to bundleName empty");
        return ERR_BUNDLE_MANAGER_BUNDLE_NOT_EXIST;
    }
    if (elementName.GetAbilityName().empty()) {
        APP_LOGE("fail to GetRemoteAbilityInfo due to abilityName empty");
        return ERR_BUNDLE_MANAGER_ABILITY_NOT_EXIST;
    }
    if (elementName.GetDeviceID().empty()) {
        APP_LOGE("fail to GetRemoteAbilityInfo due to devicedID empty");
        return ERR_BUNDLE_MANAGER_DEVICE_ID_NOT_EXIST;
    }
    return ERR_OK;
}
}  // namespace AppExecFwk
}  // namespace OHOS