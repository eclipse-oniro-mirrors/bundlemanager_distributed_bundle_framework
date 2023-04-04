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
#include "distributed_bundle.h"

#include "app_log_wrapper.h"
#include "appexecfwk_errors.h"
#include "bundle_constants.h"
#include "bundle_errors.h"
#include "business_error.h"
#include "common_func.h"
#include "distributed_bms_interface.h"
#include "distributed_bms_proxy.h"
#include "if_system_ability_manager.h"
#include "ipc_skeleton.h"
#include "iservice_registry.h"
#include "napi_arg.h"
#include "napi_constants.h"
#include "napi/native_api.h"
#include "napi/native_common.h"
#include "napi/native_node_api.h"
#include "securec.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace AppExecFwk {
namespace {
constexpr int32_t GET_REMOTE_ABILITY_INFO_MAX_SIZE = 10;
const std::string RESOURCE_NAME_GET_REMOTE_ABILITY_INFO = "GetRemoteAbilityInfo";
const std::string PARAMETER_ELEMENT_NAME = "elementName";
const std::string PARAMETER_LOCALE = "locale";
}

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

static void ConvertElementName(napi_env env, napi_value objElementName, const ElementName &elementName)
{
    napi_value nDeviceId;
    NAPI_CALL_RETURN_VOID(
        env, napi_create_string_utf8(env, elementName.GetDeviceID().c_str(), NAPI_AUTO_LENGTH, &nDeviceId));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objElementName, "deviceId", nDeviceId));

    napi_value nBundleName;
    NAPI_CALL_RETURN_VOID(
        env, napi_create_string_utf8(env, elementName.GetBundleName().c_str(), NAPI_AUTO_LENGTH, &nBundleName));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objElementName, "bundleName", nBundleName));

    napi_value nModuleName;
    NAPI_CALL_RETURN_VOID(
        env, napi_create_string_utf8(env, elementName.GetModuleName().c_str(), NAPI_AUTO_LENGTH, &nModuleName));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objElementName, "moduleName", nModuleName));

    napi_value nAbilityName;
    NAPI_CALL_RETURN_VOID(
        env, napi_create_string_utf8(env, elementName.GetAbilityName().c_str(), NAPI_AUTO_LENGTH, &nAbilityName));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objElementName, "abilityName", nAbilityName));
}

static void ConvertRemoteAbilityInfo(
    napi_env env, const RemoteAbilityInfo &remoteAbilityInfo, napi_value objRemoteAbilityInfo)
{
    napi_value objElementName;
    NAPI_CALL_RETURN_VOID(env, napi_create_object(env, &objElementName));
    ConvertElementName(env, objElementName, remoteAbilityInfo.elementName);
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objRemoteAbilityInfo, "elementName", objElementName));

    napi_value nLabel;
    NAPI_CALL_RETURN_VOID(
        env, napi_create_string_utf8(env, remoteAbilityInfo.label.c_str(), NAPI_AUTO_LENGTH, &nLabel));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objRemoteAbilityInfo, "label", nLabel));

    napi_value nIcon;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, remoteAbilityInfo.icon.c_str(), NAPI_AUTO_LENGTH, &nIcon));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objRemoteAbilityInfo, "icon", nIcon));
}

static void ConvertRemoteAbilityInfos(
    napi_env env, const std::vector<RemoteAbilityInfo> &remoteAbilityInfos, napi_value objRemoteAbilityInfos)
{
    if (remoteAbilityInfos.size() == 0) {
        APP_LOGE("ConvertRemoteAbilityInfos remoteAbilityInfos is empty");
        return;
    }
    size_t index = 0;
    for (const auto &remoteAbilityInfo : remoteAbilityInfos) {
        APP_LOGD("remoteAbilityInfo bundleName:%{public}s, abilityName:%{public}s, label:%{public}s",
                 remoteAbilityInfo.elementName.GetBundleName().c_str(),
                 remoteAbilityInfo.elementName.GetAbilityName().c_str(),
                 remoteAbilityInfo.label.c_str());
        napi_value objRemoteAbilityInfo = nullptr;
        NAPI_CALL_RETURN_VOID(env, napi_create_object(env, &objRemoteAbilityInfo));
        ConvertRemoteAbilityInfo(env, remoteAbilityInfo, objRemoteAbilityInfo);
        NAPI_CALL_RETURN_VOID(env, napi_set_element(env, objRemoteAbilityInfos, index, objRemoteAbilityInfo));
        index++;
    }
}

static bool ParseElementName(napi_env env, napi_value args, OHOS::AppExecFwk::ElementName &elementName)
{
    APP_LOGD("begin to parse ElementName");
    napi_valuetype valueType;
    napi_status status = napi_typeof(env, args, &valueType);
    if ((status != napi_ok)|| (valueType != napi_object)) {
        APP_LOGE("args not object type");
        return false;
    }
    std::string deviceId;
    if (!CommonFunc::ParseStringPropertyFromObject(env, args, "deviceId", true, deviceId)) {
        APP_LOGE("begin to parse ElementName deviceId failed");
        return false;
    }
    elementName.SetDeviceID(deviceId);

    std::string bundleName;
    if (!CommonFunc::ParseStringPropertyFromObject(env, args, "bundleName", true, bundleName)) {
        APP_LOGE("begin to parse ElementName bundleName failed");
        return false;
    }
    elementName.SetBundleName(bundleName);

    std::string abilityName;
    if (!CommonFunc::ParseStringPropertyFromObject(env, args, "abilityName", true, abilityName)) {
        APP_LOGE("begin to parse ElementName abilityName failed");
        return false;
    }
    elementName.SetAbilityName(abilityName);

    std::string moduleName;
    if (!CommonFunc::ParseStringPropertyFromObject(env, args, "moduleName", false, moduleName)) {
        APP_LOGE("begin to parse ElementName moduleName failed");
        return false;
    }
    elementName.SetModuleName(moduleName);
    APP_LOGD("parse ElementName end");
    return true;
}

static bool ParseElementNames(napi_env env, napi_value args, bool &isArray, std::vector<ElementName> &elementNames)
{
    APP_LOGD("begin to parse ElementNames");
    NAPI_CALL_BASE(env, napi_is_array(env, args, &isArray), false);
    if (!isArray) {
        APP_LOGD("parseElementNames args not array");
        ElementName elementName;
        if (ParseElementName(env, args, elementName)) {
            elementNames.push_back(elementName);
            return true;
        }
        return false;
    }
    uint32_t arrayLength = 0;
    NAPI_CALL_BASE(env, napi_get_array_length(env, args, &arrayLength), false);
    APP_LOGD("arrayLength:%{public}d", arrayLength);
    if (arrayLength == 0) {
        APP_LOGE("error: ElementNames is empty");
        return false;
    }
    for (uint32_t i = 0; i < arrayLength; i++) {
        napi_value value = nullptr;
        NAPI_CALL_BASE(env, napi_get_element(env, args, i, &value), false);
        napi_valuetype valueType = napi_undefined;
        NAPI_CALL_BASE(env, napi_typeof(env, value, &valueType), false);
        if (valueType != napi_object) {
            APP_LOGE("array inside not object type");
            elementNames.clear();
            return false;
        }
        ElementName elementName;
        if (!ParseElementName(env, value, elementName)) {
            APP_LOGE("elementNames parse elementName failed");
            return false;
        }
        elementNames.push_back(elementName);
    }
    return true;
}

int32_t InnerGetRemoteAbilityInfo(const std::vector<ElementName> &elementNames, const std::string &locale,
    bool isArray, std::vector<RemoteAbilityInfo> &remoteAbilityInfos)
{
    if (elementNames.size() == 0) {
        APP_LOGE("InnerGetRemoteAbilityInfos elementNames is empty");
        return ERROR_BUNDLE_SERVICE_EXCEPTION;
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

void GetRemoteAbilityInfoExec(napi_env env, void *data)
{
    GetRemoteAbilityInfoCallbackInfo *asyncCallbackInfo = reinterpret_cast<GetRemoteAbilityInfoCallbackInfo*>(data);
    if (asyncCallbackInfo == nullptr) {
        APP_LOGE("asyncCallbackInfo is null");
        return;
    }
    asyncCallbackInfo->err = InnerGetRemoteAbilityInfo(asyncCallbackInfo->elementNames,
        asyncCallbackInfo->locale, asyncCallbackInfo->isArray, asyncCallbackInfo->remoteAbilityInfos);
}

void GetRemoteAbilityInfoComplete(napi_env env, napi_status status, void *data)
{
    GetRemoteAbilityInfoCallbackInfo *asyncCallbackInfo = reinterpret_cast<GetRemoteAbilityInfoCallbackInfo*>(data);
    if (asyncCallbackInfo == nullptr) {
        APP_LOGE("asyncCallbackInfo is null in %{public}s", __func__);
        return;
    }
    std::unique_ptr<GetRemoteAbilityInfoCallbackInfo> callbackPtr {asyncCallbackInfo};
    napi_value result[ARGS_SIZE_TWO] = {0};
    if ((asyncCallbackInfo->err == SUCCESS) && !asyncCallbackInfo->remoteAbilityInfos.empty()) {
        NAPI_CALL_RETURN_VOID(env, napi_get_null(env, &result[0]));
        if (callbackPtr->isArray) {
            NAPI_CALL_RETURN_VOID(env, napi_create_array(env, &result[ARGS_SIZE_ONE]));
            ConvertRemoteAbilityInfos(env, asyncCallbackInfo->remoteAbilityInfos, result[ARGS_SIZE_ONE]);
        } else {
            NAPI_CALL_RETURN_VOID(env, napi_create_object(env, &result[ARGS_SIZE_ONE]));
            ConvertRemoteAbilityInfo(env, asyncCallbackInfo->remoteAbilityInfos[0], result[ARGS_SIZE_ONE]);
        }
    } else {
        result[0] = BusinessError::CreateCommonError(env, asyncCallbackInfo->err,
            RESOURCE_NAME_GET_REMOTE_ABILITY_INFO, Constants::PERMISSION_GET_BUNDLE_INFO_PRIVILEGED);
    }
    if (asyncCallbackInfo->deferred) {
        if (asyncCallbackInfo->err == SUCCESS) {
            NAPI_CALL_RETURN_VOID(env, napi_resolve_deferred(env, asyncCallbackInfo->deferred, result[ARGS_SIZE_ONE]));
        } else {
            NAPI_CALL_RETURN_VOID(env, napi_reject_deferred(env, asyncCallbackInfo->deferred, result[0]));
        }
    } else {
        napi_value callback = nullptr;
        napi_value placeHolder = nullptr;
        NAPI_CALL_RETURN_VOID(env, napi_get_reference_value(env, asyncCallbackInfo->callback, &callback));
        NAPI_CALL_RETURN_VOID(env, napi_call_function(env, nullptr, callback,
            sizeof(result) / sizeof(result[0]), result, &placeHolder));
    }
}

napi_value GetRemoteAbilityInfo(napi_env env, napi_callback_info info)
{
    APP_LOGD("begin to GetRemoteAbilityInfo");
    NapiArg args(env, info);
    GetRemoteAbilityInfoCallbackInfo *asyncCallbackInfo =
        new (std::nothrow) GetRemoteAbilityInfoCallbackInfo(env);
    if (asyncCallbackInfo == nullptr) {
        return nullptr;
    }
    std::unique_ptr<GetRemoteAbilityInfoCallbackInfo> callbackPtr {asyncCallbackInfo};
    if (!args.Init(ARGS_SIZE_ONE, ARGS_SIZE_THREE)) {
        APP_LOGE("param count invalid.");
        BusinessError::ThrowTooFewParametersError(env, ERROR_PARAM_CHECK_ERROR);
        return nullptr;
    }
    for (size_t i = 0; i < args.GetMaxArgc(); ++i) {
        napi_valuetype valueType = napi_undefined;
        napi_typeof(env, args[i], &valueType);
        if ((i == ARGS_POS_ZERO) && (!ParseElementNames(env, args[i], asyncCallbackInfo->isArray,
            asyncCallbackInfo->elementNames))) {
            BusinessError::ThrowParameterTypeError(env, ERROR_PARAM_CHECK_ERROR,
                PARAMETER_ELEMENT_NAME, TYPE_OBJECT);
            return nullptr;
        } else if (((i == ARGS_POS_ONE) && (valueType == napi_function)) ||
                   ((i == ARGS_POS_TWO) && (valueType == napi_function))) {
            NAPI_CALL(env, napi_create_reference(env, args[i], NAPI_RETURN_ONE, &asyncCallbackInfo->callback));
            break;
        } else if ((i == ARGS_POS_ONE) && !CommonFunc::ParseString(env, args[i], asyncCallbackInfo->locale)) {
            BusinessError::ThrowParameterTypeError(env, ERROR_PARAM_CHECK_ERROR, PARAMETER_LOCALE, TYPE_STRING);
            return nullptr;
        }
    }
    if (asyncCallbackInfo->elementNames.size() > GET_REMOTE_ABILITY_INFO_MAX_SIZE) {
        BusinessError::ThrowError(env, ERROR_PARAM_CHECK_ERROR,
            "BusinessError 401: The number of ElementNames is greater than 10");
        return nullptr;
    }
    auto promise = CommonFunc::AsyncCallNativeMethod<GetRemoteAbilityInfoCallbackInfo>(env, asyncCallbackInfo,
        RESOURCE_NAME_GET_REMOTE_ABILITY_INFO, GetRemoteAbilityInfoExec, GetRemoteAbilityInfoComplete);
    callbackPtr.release();
    APP_LOGD("GetRemoteAbilityInfo end");
    return promise;
}
}  // namespace AppExecFwk
}  // namespace OHOS
