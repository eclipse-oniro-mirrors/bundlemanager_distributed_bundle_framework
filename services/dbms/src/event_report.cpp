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

#include "event_report.h"

#include <unordered_map>

#include "app_log_wrapper.h"
#ifdef HISYSEVENT_ENABLE
#include "hisysevent.h"
#endif

namespace OHOS {
namespace AppExecFwk {
namespace {
const std::string GET_REMOTE_ABILITY_INFO = "GET_REMOTE_ABILITY_INFO";
const std::string GET_REMOTE_ABILITY_INFOS = "GET_REMOTE_ABILITY_INFOS";

const std::string DEVICE_ID = "DEVICE_ID";
const std::string BUNDLE_NAME = "BUNDLE_NAME";
const std::string LOCALE_INFO = "LOCALE_INFO";
const std::string ABILITY_NAME = "ABILITY_NAME";
const std::string RESULT_CODE = "RESULT_CODE";

const std::unordered_map<DBMSEventType, std::string> DBMS_EVENT_STR_MAP = {
    { DBMSEventType::GET_REMOTE_ABILITY_INFO, GET_REMOTE_ABILITY_INFO },
    { DBMSEventType::GET_REMOTE_ABILITY_INFOS, GET_REMOTE_ABILITY_INFOS },
};
}

void EventReport::SendSystemEvent(
    DBMSEventType dbmsEventType, const DBMSEventInfo& eventInfo)
{
#ifndef HISYSEVENT_ENABLE
    APP_LOGD("Hisysevent is disabled");
#else
    auto iter = DBMS_EVENT_STR_MAP.find(dbmsEventType);
    if (iter == DBMS_EVENT_STR_MAP.end()) {
        return;
    }

    auto eventName = iter->second;
    HiSysEventWrite(
        OHOS::HiviewDFX::HiSysEvent::Domain::BUNDLE_MANAGER,
        eventName,
        OHOS::HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        DEVICE_ID, eventInfo.deviceID,
        BUNDLE_NAME, eventInfo.bundleName,
        LOCALE_INFO, eventInfo.localeInfo,
        ABILITY_NAME, eventInfo.abilityName,
        RESULT_CODE, eventInfo.resultCode);
#endif
}
}  // namespace AppExecFwk
}  // namespace OHOS