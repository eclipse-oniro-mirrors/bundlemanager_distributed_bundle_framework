/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#ifndef FOUNDATION_APPEXECFWK_MOCK_BUNDLEMGR_INCLUDE_SCOPE_GUARD
#define FOUNDATION_APPEXECFWK_MOCK_BUNDLEMGR_INCLUDE_SCOPE_GUARD

#include <string>

namespace OHOS {
namespace AppExecFwk {
class MockScopeGuard final {
public:
    using Function = std::function<void()>;
    explicit MockScopeGuard(Function fn) : fn_(fn), dismissed_(false)
    {}

    ~MockScopeGuard()
    {
        if (!dismissed_) {
            fn_();
        }
    }

    void Dismiss()
    {
        dismissed_ = true;
    }

private:
    Function fn_;
    bool dismissed_;
};
}
}
#endif // FOUNDATION_APPEXECFWK_MOCK_BUNDLEMGR_INCLUDE_SCOPE_GUARD