/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef BUNDLE_MANAGER_FRAMEWORK_DISTRIBUTEBUNDLEMGR_INTERFACES_KITS_JS_DISTRIBUTED_HELPER_H
#define BUNDLE_MANAGER_FRAMEWORK_DISTRIBUTEBUNDLEMGR_INTERFACES_KITS_JS_DISTRIBUTED_HELPER_H

#include "element_name.h"
#include "remote_ability_info.h"

namespace OHOS {
namespace AppExecFwk {
namespace {
constexpr int32_t GET_REMOTE_ABILITY_INFO_MAX_SIZE = 10;
constexpr const char* RESOURCE_NAME_GET_REMOTE_ABILITY_INFO = "GetRemoteAbilityInfo";
constexpr const char* PARAMETER_ELEMENT_NAME = "elementName";
constexpr const char* PARAMETER_LOCALE = "locale";
}
class DistributedHelper {
public:
    static int32_t InnerGetRemoteAbilityInfo(const std::vector<ElementName> &elementNames, const std::string &locale,
        bool isArray, std::vector<RemoteAbilityInfo> &remoteAbilityInfos);
};
}
}
#endif // BUNDLE_MANAGER_FRAMEWORK_DISTRIBUTEBUNDLEMGR_INTERFACES_KITS_JS_DISTRIBUTED_HELPER_H