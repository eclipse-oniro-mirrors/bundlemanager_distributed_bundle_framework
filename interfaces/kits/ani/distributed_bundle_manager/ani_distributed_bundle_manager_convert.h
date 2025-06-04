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

#ifndef DISTRIBUTED_BUNDLE_FRAMEWORK_INTERFACE_KITS_ANI_ANI_DISTRIBUTED_BUNDLE_MANAGER_CONVERT_H
#define DISTRIBUTED_BUNDLE_FRAMEWORK_INTERFACE_KITS_ANI_ANI_DISTRIBUTED_BUNDLE_MANAGER_CONVERT_H

#include <ani.h>
#include <string>
#include <vector>

#include "element_name.h"
#include "remote_ability_info.h"

namespace OHOS {
namespace AppExecFwk {
class AniDistributedbundleManagerConvert {
public:
    static ani_object ConvertRemoteAbilityInfo(ani_env* env, const RemoteAbilityInfo& remoteAbilityInfo);
};
}
}
#endif // DISTRIBUTED_BUNDLE_FRAMEWORK_INTERFACE_KITS_ANI_ANI_DISTRIBUTED_BUNDLE_MANAGER_CONVERT_H