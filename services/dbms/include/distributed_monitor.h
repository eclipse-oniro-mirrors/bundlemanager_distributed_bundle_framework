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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_DBMS_INCLUDE_DISTRIBUTED_MONITOR_H
#define FOUNDATION_APPEXECFWK_SERVICES_DBMS_INCLUDE_DISTRIBUTED_MONITOR_H

#include "common_event_manager.h"
#include "common_event_support.h"
#include "common_event_subscriber.h"
#include "common_event_subscribe_info.h"
#include "distributed_data_storage.h"

namespace OHOS {
namespace AppExecFwk {
class DistributedMonitor : public EventFwk::CommonEventSubscriber {
public:
    DistributedMonitor(const EventFwk::CommonEventSubscribeInfo& sp):CommonEventSubscriber(sp) {}
    ~DistributedMonitor() {}

    void OnReceiveEvent(const EventFwk::CommonEventData &eventData)
    {
        auto want = eventData.GetWant();
        std::string action = want.GetAction();
        APP_LOGI("OnReceiveEvent action:%{public}s", action.c_str());
        if (action == EventFwk::CommonEventSupport::COMMON_EVENT_USER_SWITCHED) {
            int32_t userId = eventData.GetCode();
            APP_LOGI("OnReceiveEvent switched userId:%{public}d", userId);
            DistributedDataStorage::GetInstance()->UpdateDistributedData(userId);
            return;
        }
        int32_t userId = want.GetIntParam(Constants::USER_ID, Constants::INVALID_USERID);
        std::string bundleName = want.GetElement().GetBundleName();
        if (action == EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_ADDED ||
            action == EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_CHANGED) {
            DistributedDataStorage::GetInstance()->SaveStorageDistributeInfo(bundleName, userId);
        } else if (action == EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_REMOVED) {
            DistributedDataStorage::GetInstance()->DeleteStorageDistributeInfo(bundleName, userId);
        } else {
            APP_LOGW("OnReceiveEvent undefined action");
        }
    }
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_SERVICES_DBMS_INCLUDE_DISTRIBUTED_MONITOR_H