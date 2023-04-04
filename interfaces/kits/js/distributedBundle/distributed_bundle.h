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

#ifndef BUNDLE_MANAGER_FRAMEWORK_DISTRIBUTEBUNDLEMGR_INTERFACES_KITS_JS_DISTRIBUTE_BUNDLE_H
#define BUNDLE_MANAGER_FRAMEWORK_DISTRIBUTEBUNDLEMGR_INTERFACES_KITS_JS_DISTRIBUTE_BUNDLE_H

#include <vector>
#include <string>

#include "base_cb_info.h"
#include "element_name.h"
#include "remote_ability_info.h"

namespace OHOS {
namespace AppExecFwk {
struct GetRemoteAbilityInfoCallbackInfo : public BaseCallbackInfo {
    explicit GetRemoteAbilityInfoCallbackInfo(napi_env napiEnv) : BaseCallbackInfo(napiEnv) {}
    std::vector<ElementName> elementNames;
    std::vector<RemoteAbilityInfo> remoteAbilityInfos;
    std::string locale = "";
    bool isArray = false;
};

napi_value GetRemoteAbilityInfo(napi_env env, napi_callback_info info);
}  // namespace AppExecFwk
}  // namespace OHOS
#endif // BUNDLE_MANAGER_FRAMEWORK_DISTRIBUTEBUNDLEMGR_INTERFACES_KITS_JS_DISTRIBUTE_BUNDLE_H