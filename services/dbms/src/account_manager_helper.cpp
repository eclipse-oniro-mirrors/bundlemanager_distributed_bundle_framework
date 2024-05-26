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

#include "account_manager_helper.h"

#include <vector>

#include "app_log_wrapper.h"
#include "bundle_constants.h"

#ifdef ACCOUNT_ENABLE
#include "os_account_manager.h"
#endif

namespace OHOS {
namespace AppExecFwk {
int32_t AccountManagerHelper::GetCurrentActiveUserId()
{
#ifdef ACCOUNT_ENABLE
    std::int32_t localId;
    int32_t ret = AccountSA::OsAccountManager::GetForegroundOsAccountLocalId(localId);
    if (ret != 0) {
        APP_LOGE("GetForegroundOsAccountLocalId failed ret:%{public}d", ret);
        return Constants::INVALID_USERID;
    }
    return localId;
#else
    APP_LOGI("ACCOUNT_ENABLE is false");
    return Constants::INVALID_USERID;
#endif
}
}  // namespace AppExecFwk
}  // namespace OHOS
