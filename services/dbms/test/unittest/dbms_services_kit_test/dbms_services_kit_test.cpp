/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#define private public

#include <fstream>
#include <iostream>
#include <gtest/gtest.h>
#include <sstream>
#include <string>
#include <fcntl.h>
#include <future>

#include "accesstoken_kit.h"
#include "appexecfwk_errors.h"
#include "bundle_installer_proxy.h"
#include "bundle_mgr_proxy.h"
#include "dbms_device_manager.h"
#include "distributed_ability_info.h"
#include "distributed_bms.h"
#include "distributed_bms_interface.h"
#include "distributed_bms_proxy.h"
#include "distributed_bundle_info.h"
#include "distributed_module_info.h"
#include "element_name.h"
#include "event_report.h"
#ifdef DISTRIBUTED_BUNDLE_IMAGE_ENABLE
#include "image_compress.h"
#endif
#include "iservice_registry.h"
#include "json_util.h"
#include "nativetoken_kit.h"
#include "token_setproc.h"
#include "scope_guard.h"
#include "service_control.h"
#include "softbus_common.h"
#include "status_receiver_host.h"
#include "system_ability_definition.h"

using namespace testing::ext;
using namespace std::chrono_literals;
using namespace OHOS::AppExecFwk;

namespace OHOS {
namespace {
const std::string WRONG_BUNDLE_NAME = "wrong";
const std::string WRONG_ABILITY_NAME = "wrong";
const std::string BUNDLE_NAME = "com.query.test";
const std::string MODULE_NAME = "module01";
const std::string ABILITY_NAME = "MainAbility";
const std::string SYSTEM_HAP_FILE_PATH = "/data/test/resource/bms/install_bundle/distributed_system_module.hap";
const std::string DEVICE_ID = "1111";
const std::string INVALID_NAME = "invalid";
const std::string HAP_FILE_PATH =
    "/data/app/el1/bundle/public/com.example.test/entry.hap";
const std::string PATH_LOCATION = "/data/app/el1/bundle/public/com.query.test";
const std::string PATH_LOCATIONS = "/data/app/el1/bundle/new_create.txt";
const std::string DEVICE_ID_NORMAL = "deviceId";
const std::string LOCALE_INFO = "localeInfo";
const std::string EMPTY_STRING = "";
const int32_t USERID = 100;
const std::string MSG_SUCCESS = "[SUCCESS]";
const std::string OPERATION_FAILED = "Failure";
const std::string OPERATION_SUCCESS = "Success";
}  // namespace

class StatusReceiverImpl : public StatusReceiverHost {
public:
    StatusReceiverImpl();
    virtual ~StatusReceiverImpl();
    void OnStatusNotify(const int progress) override;
    void OnFinished(const int32_t resultCode, const std::string &resultMsg) override;
    std::string GetResultMsg() const;

private:
    mutable std::promise<std::string> resultMsgSignal_;
    int iProgress_ = 0;

    DISALLOW_COPY_AND_MOVE(StatusReceiverImpl);
};

StatusReceiverImpl::StatusReceiverImpl()
{
    APP_LOGI("create status receiver instance");
}

StatusReceiverImpl::~StatusReceiverImpl()
{
    APP_LOGI("destroy status receiver instance");
}

void StatusReceiverImpl::OnFinished(const int32_t resultCode, const std::string &resultMsg)
{
    APP_LOGD("OnFinished result is %{public}d, %{public}s", resultCode, resultMsg.c_str());
    resultMsgSignal_.set_value(resultMsg);
}
void StatusReceiverImpl::OnStatusNotify(const int progress)
{
    EXPECT_GT(progress, iProgress_);
    iProgress_ = progress;
    APP_LOGI("OnStatusNotify progress:%{public}d", progress);
}

std::string StatusReceiverImpl::GetResultMsg() const
{
    auto future = resultMsgSignal_.get_future();
    future.wait();
    std::string resultMsg = future.get();
    if (resultMsg == MSG_SUCCESS) {
        return OPERATION_SUCCESS;
    } else {
        return OPERATION_FAILED + resultMsg;
    }
}

class DbmsServicesKitTest : public testing::Test {
public:
    DbmsServicesKitTest();
    ~DbmsServicesKitTest();
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    std::shared_ptr<DistributedBms> GetDistributedBms();
    std::shared_ptr<DistributedBmsProxy> GetDistributedBmsProxy();
    std::shared_ptr<DistributedDataStorage> GetDistributedDataStorage();
    std::shared_ptr<EventReport> GetEventReport();
    sptr<DistributedBms> GetSptrDistributedBms();
    static sptr<BundleMgrProxy> GetBundleMgrProxy();
    static sptr<IBundleInstaller> GetInstallerProxy();
    bool InstallBundle(const std::string &bundlePath) const;
    bool UninstallBundle(const std::string &bundleName) const;
private:
    std::shared_ptr<DistributedBms> distributedBms_ = nullptr;
    std::shared_ptr<DistributedBmsProxy> distributedBmsProxy_ = nullptr;
    std::shared_ptr<DistributedDataStorage> distributedDataStorage_ = nullptr;
    std::shared_ptr<EventReport> eventReport_ = nullptr;
    sptr<DistributedBms> sptrDistributedBms_ = nullptr;
};

DbmsServicesKitTest::DbmsServicesKitTest()
{}

DbmsServicesKitTest::~DbmsServicesKitTest()
{}

void DbmsServicesKitTest::SetUpTestCase()
{}

void DbmsServicesKitTest::TearDownTestCase()
{}

void DbmsServicesKitTest::SetUp()
{
    const int32_t permsNum = 4;
    const int32_t PERMS_INDEX_TWO = 2;
    const int32_t permsIndexThree = 3;
    uint64_t tokenId;
    const char *perms[permsNum];
    perms[0] = OHOS_PERMISSION_DISTRIBUTED_SOFTBUS_CENTER;
    perms[1] = OHOS_PERMISSION_DISTRIBUTED_DATASYNC;
    perms[PERMS_INDEX_TWO] = "ohos.permission.ACCESS_SERVICE_DM";
    perms[permsIndexThree] = Constants::PERMISSION_GET_BUNDLE_INFO_PRIVILEGED;
    NativeTokenInfoParams infoInstance = {
        .dcapsNum = 0,
        .permsNum = permsNum,
        .aclsNum = 0,
        .dcaps = NULL,
        .perms = perms,
        .acls = NULL,
        .processName = "dsoftbus_service",
        .aplStr = "system_core",
    };
    tokenId = GetAccessTokenId(&infoInstance);
    SetSelfTokenID(tokenId);
    OHOS::Security::AccessToken::AccessTokenKit::ReloadNativeTokenInfo();
    std::string strExtra = std::to_string(402);
    auto extraArgv = strExtra.c_str();
    ServiceControlWithExtra("d-bms", START, &extraArgv, 1);
}

void DbmsServicesKitTest::TearDown()
{
    std::string strExtra = std::to_string(402);
    auto extraArgv = strExtra.c_str();
    ServiceControlWithExtra("d-bms", STOP, &extraArgv, 1);
}

sptr<BundleMgrProxy> DbmsServicesKitTest::GetBundleMgrProxy()
{
    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (!systemAbilityManager) {
        APP_LOGE("fail to get system ability mgr.");
        return nullptr;
    }

    sptr<IRemoteObject> remoteObject = systemAbilityManager->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    if (!remoteObject) {
        APP_LOGE("fail to get bundle manager proxy.");
        return nullptr;
    }

    APP_LOGI("get bundle manager proxy success.");
    return iface_cast<BundleMgrProxy>(remoteObject);
}

sptr<IBundleInstaller> DbmsServicesKitTest::GetInstallerProxy()
{
    sptr<BundleMgrProxy> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        APP_LOGE("bundle mgr proxy is nullptr.");
        return nullptr;
    }

    sptr<IBundleInstaller> installerProxy = bundleMgrProxy->GetBundleInstaller();
    if (!installerProxy) {
        APP_LOGE("fail to get bundle installer proxy.");
        return nullptr;
    }

    APP_LOGI("get bundle installer proxy success.");
    return installerProxy;
}

bool DbmsServicesKitTest::InstallBundle(const std::string &bundlePath) const
{
    setuid(Constants::FOUNDATION_UID);
    ScopeGuard uidGuard([&] { setuid(Constants::ROOT_UID); });
    sptr<IBundleInstaller> installerProxy = GetInstallerProxy();
    if (!installerProxy) {
        APP_LOGE("get bundle installer failed.");
        return false;
    }
    InstallParam installParam;
    installParam.installFlag = InstallFlag::REPLACE_EXISTING;
    installParam.userId = USERID;
    sptr<StatusReceiverImpl> statusReceiver = (new (std::nothrow) StatusReceiverImpl());
    EXPECT_NE(statusReceiver, nullptr);
    bool result = installerProxy->Install(bundlePath, installParam, statusReceiver);
    EXPECT_EQ(statusReceiver->GetResultMsg(), OPERATION_SUCCESS);
    return result;
}

bool DbmsServicesKitTest::UninstallBundle(const std::string &bundleName) const
{
    setuid(Constants::FOUNDATION_UID);
    ScopeGuard uidGuard([&] { setuid(Constants::ROOT_UID); });
    sptr<IBundleInstaller> installerProxy = GetInstallerProxy();
    if (!installerProxy) {
        APP_LOGE("get bundle installer failed.");
        return false;
    }
    InstallParam installParam;
    installParam.installFlag = InstallFlag::NORMAL;
    installParam.userId = USERID;
    sptr<StatusReceiverImpl> statusReceiver = (new (std::nothrow) StatusReceiverImpl());
    EXPECT_NE(statusReceiver, nullptr);
    bool result = installerProxy->Uninstall(bundleName, installParam, statusReceiver);
    EXPECT_EQ(statusReceiver->GetResultMsg(), OPERATION_SUCCESS);
    return result;
}

std::shared_ptr<DistributedBms> DbmsServicesKitTest::GetDistributedBms()
{
    if (distributedBms_ == nullptr) {
        distributedBms_ = std::make_unique<DistributedBms>();
    }
    return distributedBms_;
}

sptr<DistributedBms> DbmsServicesKitTest::GetSptrDistributedBms()
{
    if (sptrDistributedBms_ == nullptr) {
        sptrDistributedBms_ = new (std::nothrow) DistributedBms();
    }
    return sptrDistributedBms_;
}

std::shared_ptr<DistributedBmsProxy> DbmsServicesKitTest::GetDistributedBmsProxy()
{
    if (distributedBmsProxy_ == nullptr) {
        distributedBmsProxy_ = std::make_shared<DistributedBmsProxy>(nullptr);
    }
    return distributedBmsProxy_;
}

std::shared_ptr<DistributedDataStorage> DbmsServicesKitTest::GetDistributedDataStorage()
{
    if (distributedDataStorage_ == nullptr) {
        distributedDataStorage_ =
            std::make_unique<DistributedDataStorage>();
    }
    return distributedDataStorage_;
}

std::shared_ptr<EventReport> DbmsServicesKitTest::GetEventReport()
{
    if (eventReport_ == nullptr) {
        eventReport_ = std::make_shared<EventReport>();
    }
    return eventReport_;
}

/**
 * @tc.number: DbmsServicesKitTest
 * @tc.name: test GetRemoteAbilityInfo
 * @tc.require: issueI5MZ8V
 * @tc.desc: 1. system running normally
 *           2. test ElementName empty
 */
HWTEST_F(DbmsServicesKitTest, DbmsServicesKitTest_0001, Function | SmallTest | Level0)
{
    auto distributedBms = GetDistributedBms();
    EXPECT_NE(distributedBms, nullptr);
    if (distributedBms != nullptr) {
        ElementName name;
        RemoteAbilityInfo info;
        auto ret = distributedBms->GetRemoteAbilityInfo(name, info);
        EXPECT_EQ(ret, ERR_BUNDLE_MANAGER_DEVICE_ID_NOT_EXIST);
    }
}

/**
 * @tc.number: DbmsServicesKitTest
 * @tc.name: test GetRemoteAbilityInfo
 * @tc.require: issueI5MZ8V
 * @tc.desc: 1. system running normally
 *           2. test ElementName empty
 */
HWTEST_F(DbmsServicesKitTest, DbmsServicesKitTest_0002, Function | SmallTest | Level0)
{
    auto distributedBms = GetDistributedBms();
    EXPECT_NE(distributedBms, nullptr);
    if (distributedBms != nullptr) {
        ElementName name;
        RemoteAbilityInfo info;
        auto ret = distributedBms->GetRemoteAbilityInfo(name, "", info);
        EXPECT_EQ(ret, ERR_BUNDLE_MANAGER_DEVICE_ID_NOT_EXIST);
    }
}

/**
 * @tc.number: DbmsServicesKitTest
 * @tc.name: test GetRemoteAbilityInfo
 * @tc.require: issueI5MZ8V
 * @tc.desc: 1. system running normally
 *           2. test ElementName empty
 */
HWTEST_F(DbmsServicesKitTest, DbmsServicesKitTest_0003, Function | SmallTest | Level0)
{
    auto distributedBms = GetDistributedBms();
    EXPECT_NE(distributedBms, nullptr);
    if (distributedBms != nullptr) {
        ElementName name;
        RemoteAbilityInfo info;
        auto ret = distributedBms->GetRemoteAbilityInfo(name, "", info);
        EXPECT_EQ(ret, ERR_BUNDLE_MANAGER_DEVICE_ID_NOT_EXIST);
    }
}

/**
 * @tc.number: DbmsServicesKitTest
 * @tc.name: test GetRemoteAbilityInfos
 * @tc.require: issueI5MZ8V
 * @tc.desc: 1. system running normally
 *           2. test ElementName empty
 */
HWTEST_F(DbmsServicesKitTest, DbmsServicesKitTest_0004, Function | SmallTest | Level0)
{
    auto distributedBms = GetDistributedBms();
    EXPECT_NE(distributedBms, nullptr);
    if (distributedBms != nullptr) {
        std::vector<ElementName> name;
        std::vector<RemoteAbilityInfo> info;
        auto ret = distributedBms->GetRemoteAbilityInfos(name, info);
        EXPECT_EQ(ret, ERR_BUNDLE_MANAGER_PARAM_ERROR);
    }
}

/**
 * @tc.number: DbmsServicesKitTest
 * @tc.name: test GetRemoteAbilityInfos
 * @tc.require: issueI5MZ8V
 * @tc.desc: 1. system running normally
 *           2. test ElementName empty
 */
HWTEST_F(DbmsServicesKitTest, DbmsServicesKitTest_0005, Function | SmallTest | Level0)
{
    auto distributedBms = GetDistributedBms();
    EXPECT_NE(distributedBms, nullptr);
    if (distributedBms != nullptr) {
        std::vector<ElementName> name;
        std::vector<RemoteAbilityInfo> info;
        auto ret = distributedBms->GetRemoteAbilityInfos(name, "", info);
        EXPECT_EQ(ret, ERR_BUNDLE_MANAGER_PARAM_ERROR);
    }
}

/**
 * @tc.number: DbmsServicesKitTest
 * @tc.name: test GetAbilityInfo
 * @tc.require: issueI5MZ8V
 * @tc.desc: 1. system running normally
 *           2. test bundleName empty
 */
HWTEST_F(DbmsServicesKitTest, DbmsServicesKitTest_0006, Function | SmallTest | Level0)
{
    auto distributedBms = GetDistributedBms();
    EXPECT_NE(distributedBms, nullptr);
    if (distributedBms != nullptr) {
        ElementName name;
        name.SetBundleName(WRONG_BUNDLE_NAME);
        name.SetAbilityName(ABILITY_NAME);
        RemoteAbilityInfo info;
        auto ret = distributedBms->GetAbilityInfo(name, info);
        EXPECT_EQ(ret, ERR_BUNDLE_MANAGER_BUNDLE_NOT_EXIST);
    }
}

/**
 * @tc.number: DbmsServicesKitTest
 * @tc.name: test GetAbilityInfo
 * @tc.require: issueI5MZ8V
 * @tc.desc: 1. system running normally
 *           2. test abilityName empty
 */
HWTEST_F(DbmsServicesKitTest, DbmsServicesKitTest_0007, Function | SmallTest | Level0)
{
    auto res = InstallBundle(SYSTEM_HAP_FILE_PATH);
    EXPECT_TRUE(res);
    
    auto distributedBms = GetDistributedBms();
    EXPECT_NE(distributedBms, nullptr);
    if (distributedBms != nullptr) {
        ElementName name;
        name.SetBundleName(BUNDLE_NAME);
        name.SetAbilityName(WRONG_ABILITY_NAME);
        RemoteAbilityInfo info;
        auto ret = distributedBms->GetAbilityInfo(name, info);
        EXPECT_EQ(ret, ERR_BUNDLE_MANAGER_ABILITY_NOT_EXIST);
    }
    res = UninstallBundle(BUNDLE_NAME);
    EXPECT_TRUE(res);
}

/**
 * @tc.number: DbmsServicesKitTest
 * @tc.name: test GetAbilityInfo
 * @tc.require: issueI5MZ8V
 * @tc.desc: 1. system running normally
 *           2. test bundleName and abilityName both exist
 */
HWTEST_F(DbmsServicesKitTest, DbmsServicesKitTest_0008, Function | SmallTest | Level0)
{
    auto res = InstallBundle(SYSTEM_HAP_FILE_PATH);
    EXPECT_TRUE(res);
    auto distributedBms = GetDistributedBms();
    EXPECT_NE(distributedBms, nullptr);
    if (distributedBms != nullptr) {
        ElementName name;
        name.SetBundleName(BUNDLE_NAME);
        name.SetAbilityName(ABILITY_NAME);
        RemoteAbilityInfo info;
        auto ret = distributedBms->GetAbilityInfo(name, info);
#ifdef ON_64BIT_SYSTEM
        EXPECT_EQ(ret, ERR_BUNDLE_MANAGER_INTERNAL_ERROR);
#else
        EXPECT_EQ(ret, ERR_OK);
#endif
    }
    res = UninstallBundle(BUNDLE_NAME);
    EXPECT_TRUE(res);
}

/**
 * @tc.number: DbmsServicesKitTest
 * @tc.name: test GetAbilityInfo
 * @tc.require: issueI5MZ8V
 * @tc.desc: 1. system running normally
 *           2. test wrong abilityName
 */
HWTEST_F(DbmsServicesKitTest, DbmsServicesKitTest_0009, Function | SmallTest | Level0)
{
    auto res = InstallBundle(SYSTEM_HAP_FILE_PATH);
    EXPECT_TRUE(res);

    auto distributedBms = GetDistributedBms();
    EXPECT_NE(distributedBms, nullptr);
    if (distributedBms != nullptr) {
        ElementName name;
        name.SetBundleName(BUNDLE_NAME);
        name.SetModuleName(MODULE_NAME);
        name.SetAbilityName(WRONG_ABILITY_NAME);
        RemoteAbilityInfo info;
        auto ret = distributedBms->GetAbilityInfo(name, info);
        EXPECT_EQ(ret, ERR_BUNDLE_MANAGER_ABILITY_NOT_EXIST);
    }
    res = UninstallBundle(BUNDLE_NAME);
    EXPECT_TRUE(res);
}

/**
 * @tc.number: DbmsServicesKitTest
 * @tc.name: test GetAbilityInfo
 * @tc.require: issueI5MZ8V
 * @tc.desc: 1. system running normally
 *           2. test ElementName empty
 */
HWTEST_F(DbmsServicesKitTest, DbmsServicesKitTest_0010, Function | SmallTest | Level0)
{
    auto distributedBms = GetDistributedBms();
    EXPECT_NE(distributedBms, nullptr);
    if (distributedBms != nullptr) {
        std::vector<ElementName> name;
        std::vector<RemoteAbilityInfo> info;
        auto ret = distributedBms->GetAbilityInfos(name, info);
        EXPECT_EQ(ret, ERR_OK);
    }
}

/**
 * @tc.number: DbmsServicesKitTest
 * @tc.name: test GetAbilityInfo
 * @tc.require: issueI5MZ8V
 * @tc.desc: 1. system running normally
 *           2. test abilityName is empty
 */
HWTEST_F(DbmsServicesKitTest, DbmsServicesKitTest_0011, Function | SmallTest | Level0)
{
    auto res = InstallBundle(SYSTEM_HAP_FILE_PATH);
    EXPECT_TRUE(res);

    auto distributedBms = GetDistributedBms();
    EXPECT_NE(distributedBms, nullptr);
    if (distributedBms != nullptr) {
        std::vector<ElementName> names;
        ElementName name;
        name.SetBundleName(BUNDLE_NAME);
        name.SetModuleName(MODULE_NAME);
        name.SetAbilityName(WRONG_ABILITY_NAME);
        names.push_back(name);
        std::vector<RemoteAbilityInfo> infos;
        auto ret = distributedBms->GetAbilityInfos(names, infos);
        EXPECT_EQ(ret, ERR_BUNDLE_MANAGER_ABILITY_NOT_EXIST);
    }
    res = UninstallBundle(BUNDLE_NAME);
    EXPECT_TRUE(res);
}

/**
 * @tc.number: DbmsServicesKitTest
 * @tc.name: test GetAbilityInfo
 * @tc.require: issueI5MZ8V
 * @tc.desc: 1. system running normally
 *           2. test abilityName empty
 */
HWTEST_F(DbmsServicesKitTest, DbmsServicesKitTest_0012, Function | SmallTest | Level0)
{
    auto res = InstallBundle(SYSTEM_HAP_FILE_PATH);
    EXPECT_TRUE(res);

    auto distributedBms = GetDistributedBms();
    EXPECT_NE(distributedBms, nullptr);
    if (distributedBms != nullptr) {
        std::vector<ElementName> names;
        ElementName name;
        name.SetBundleName(BUNDLE_NAME);
        name.SetAbilityName(ABILITY_NAME);
        names.push_back(name);
        std::vector<RemoteAbilityInfo> infos;
        auto ret = distributedBms->GetAbilityInfos(names, infos);
#ifdef ON_64BIT_SYSTEM
        EXPECT_EQ(ret, ERR_BUNDLE_MANAGER_INTERNAL_ERROR);
#else
        EXPECT_EQ(ret, ERR_OK);
#endif
    }
    res = UninstallBundle(BUNDLE_NAME);
    EXPECT_TRUE(res);
}

/**
 * @tc.number: DbmsServicesKitTest
 * @tc.name: test GetAbilityInfo
 * @tc.require: issueI5MZ8V
 * @tc.desc: 1. system running normally
 *           2. test bundleName empty
 */
HWTEST_F(DbmsServicesKitTest, DbmsServicesKitTest_0013, Function | SmallTest | Level0)
{
    auto distributedBmsProxy = GetDistributedBmsProxy();
    EXPECT_NE(distributedBmsProxy, nullptr);
    if (distributedBmsProxy != nullptr) {
        ElementName name;
        RemoteAbilityInfo info;
        auto ret = distributedBmsProxy->GetRemoteAbilityInfo(name, "", info);
        EXPECT_EQ(ret, ERR_BUNDLE_MANAGER_BUNDLE_NOT_EXIST);
    }
}

/**
 * @tc.number: DbmsServicesKitTest
 * @tc.name: test GetAbilityInfo
 * @tc.require: issueI5MZ8V
 * @tc.desc: 1. system running normally
 *           2. test abilityName empty
 */
HWTEST_F(DbmsServicesKitTest, DbmsServicesKitTest_0014, Function | SmallTest | Level0)
{
    auto distributedBmsProxy = GetDistributedBmsProxy();
    EXPECT_NE(distributedBmsProxy, nullptr);
    if (distributedBmsProxy != nullptr) {
        ElementName name;
        name.SetBundleName(BUNDLE_NAME);
        RemoteAbilityInfo info;
        auto ret = distributedBmsProxy->GetRemoteAbilityInfo(name, "", info);
        EXPECT_EQ(ret, ERR_BUNDLE_MANAGER_ABILITY_NOT_EXIST);
    }
}

/**
 * @tc.number: DbmsServicesKitTest
 * @tc.name: test GetAbilityInfo
 * @tc.require: issueI5MZ8V
 * @tc.desc: 1. system running normally
 *           2. test deviceID empty
 */
HWTEST_F(DbmsServicesKitTest, DbmsServicesKitTest_0015, Function | SmallTest | Level0)
{
    auto distributedBmsProxy = GetDistributedBmsProxy();
    EXPECT_NE(distributedBmsProxy, nullptr);
    if (distributedBmsProxy != nullptr) {
        ElementName name;
        name.SetBundleName(BUNDLE_NAME);
        name.SetAbilityName(ABILITY_NAME);
        RemoteAbilityInfo info;
        auto ret = distributedBmsProxy->GetRemoteAbilityInfo(name, "", info);
        EXPECT_EQ(ret, ERR_BUNDLE_MANAGER_DEVICE_ID_NOT_EXIST);
    }
}

/**
 * @tc.number: DbmsServicesKitTest
 * @tc.name: test GetAbilityInfo
 * @tc.require: issueI5MZ8V
 * @tc.desc: 1. system running normally
 *           2. test ElementName not empty
 */
HWTEST_F(DbmsServicesKitTest, DbmsServicesKitTest_0016, Function | SmallTest | Level0)
{
    auto distributedBmsProxy = GetDistributedBmsProxy();
    EXPECT_NE(distributedBmsProxy, nullptr);
    if (distributedBmsProxy != nullptr) {
        ElementName name;
        name.SetBundleName(BUNDLE_NAME);
        name.SetAbilityName(ABILITY_NAME);
        name.SetDeviceID(DEVICE_ID);
        RemoteAbilityInfo info;
        auto ret = distributedBmsProxy->GetRemoteAbilityInfo(name, "", info);
        EXPECT_EQ(ret, ERR_APPEXECFWK_FAILED_GET_REMOTE_PROXY);
    }
}

/**
 * @tc.number: DbmsServicesKitTest
 * @tc.name: test GetAbilityInfo
 * @tc.require: issueI5MZ8V
 * @tc.desc: 1. system running normally
 *           2. test ElementNames empty
 */
HWTEST_F(DbmsServicesKitTest, DbmsServicesKitTest_0017, Function | SmallTest | Level0)
{
    auto distributedBmsProxy = GetDistributedBmsProxy();
    EXPECT_NE(distributedBmsProxy, nullptr);
    if (distributedBmsProxy != nullptr) {
        std::vector<ElementName> names;
        std::vector<RemoteAbilityInfo> infos;
        auto ret = distributedBmsProxy->GetRemoteAbilityInfos(names, infos);
        EXPECT_EQ(ret, ERR_APPEXECFWK_FAILED_GET_REMOTE_PROXY);
    }
}

/**
 * @tc.number: DbmsServicesKitTest
 * @tc.name: test GetAbilityInfo
 * @tc.require: issueI5MZ8V
 * @tc.desc: 1. system running normally
 *           2. test ElementNames not empty
 */
HWTEST_F(DbmsServicesKitTest, DbmsServicesKitTest_0018, Function | SmallTest | Level0)
{
    auto distributedBmsProxy = GetDistributedBmsProxy();
    EXPECT_NE(distributedBmsProxy, nullptr);
    if (distributedBmsProxy != nullptr) {
        std::vector<ElementName> names;
        ElementName name_1;
        name_1.SetBundleName(BUNDLE_NAME);
        name_1.SetAbilityName(ABILITY_NAME);
        name_1.SetDeviceID(DEVICE_ID);
        names.push_back(name_1);

        ElementName name_2;
        name_2.SetBundleName(BUNDLE_NAME);
        name_2.SetAbilityName(ABILITY_NAME);
        names.push_back(name_2);

        std::vector<RemoteAbilityInfo> infos;
        auto ret = distributedBmsProxy->GetRemoteAbilityInfos(names, infos);
        EXPECT_EQ(ret, ERR_BUNDLE_MANAGER_DEVICE_ID_NOT_EXIST);
    }
}

/**
 * @tc.number: DbmsServicesKitTest
 * @tc.name: test GetAbilityInfo
 * @tc.require: issueI5MZ8V
 * @tc.desc: 1. system running normally
 *           2. test GetAbilityInfo
 */
HWTEST_F(DbmsServicesKitTest, DbmsServicesKitTest_0019, Function | SmallTest | Level0)
{
    auto distributedBmsProxy = GetDistributedBmsProxy();
    EXPECT_NE(distributedBmsProxy, nullptr);
    if (distributedBmsProxy != nullptr) {
        ElementName name;
        name.SetBundleName(WRONG_BUNDLE_NAME);
        name.SetAbilityName(ABILITY_NAME);
        RemoteAbilityInfo info;
        auto ret = distributedBmsProxy->GetAbilityInfo(name, info);
        EXPECT_EQ(ret, ERR_APPEXECFWK_FAILED_GET_REMOTE_PROXY);
    }
}

/**
 * @tc.number: DbmsServicesKitTest
 * @tc.name: test GetAbilityInfo
 * @tc.require: issueI5MZ8V
 * @tc.desc: 1. system running normally
 *           2. test GetAbilityInfo
 */
HWTEST_F(DbmsServicesKitTest, DbmsServicesKitTest_0020, Function | SmallTest | Level0)
{
    auto distributedBmsProxy = GetDistributedBmsProxy();
    EXPECT_NE(distributedBmsProxy, nullptr);
    if (distributedBmsProxy != nullptr) {
        ElementName name;
        name.SetBundleName(WRONG_BUNDLE_NAME);
        name.SetAbilityName(ABILITY_NAME);
        RemoteAbilityInfo info;
        auto ret = distributedBmsProxy->GetAbilityInfo(name, "localeInfo", info);
        EXPECT_EQ(ret, ERR_APPEXECFWK_FAILED_GET_REMOTE_PROXY);
    }
}

/**
 * @tc.number: DbmsServicesKitTest
 * @tc.name: test GetAbilityInfo
 * @tc.require: issueI5MZ8V
 * @tc.desc: 1. system running normally
 *           2. test GetAbilityInfos
 */
HWTEST_F(DbmsServicesKitTest, DbmsServicesKitTest_0021, Function | SmallTest | Level0)
{
    auto distributedBmsProxy = GetDistributedBmsProxy();
    EXPECT_NE(distributedBmsProxy, nullptr);
    if (distributedBmsProxy != nullptr) {
        std::vector<ElementName> names;
        ElementName name;
        name.SetBundleName(BUNDLE_NAME);
        name.SetModuleName(MODULE_NAME);
        name.SetAbilityName(WRONG_ABILITY_NAME);
        names.push_back(name);
        std::vector<RemoteAbilityInfo> infos;
        auto ret = distributedBmsProxy->GetAbilityInfos(names, infos);
        EXPECT_EQ(ret, ERR_APPEXECFWK_FAILED_GET_REMOTE_PROXY);
    }
}

/**
 * @tc.number: DbmsServicesKitTest
 * @tc.name: test GetAbilityInfo
 * @tc.require: issueI5MZ8V
 * @tc.desc: 1. system running normally
 *           2. test GetAbilityInfos
 */
HWTEST_F(DbmsServicesKitTest, DbmsServicesKitTest_0022, Function | SmallTest | Level0)
{
    auto distributedBmsProxy = GetDistributedBmsProxy();
    EXPECT_NE(distributedBmsProxy, nullptr);
    if (distributedBmsProxy != nullptr) {
        std::vector<ElementName> names;
        ElementName name;
        name.SetBundleName(BUNDLE_NAME);
        name.SetModuleName(MODULE_NAME);
        name.SetAbilityName(WRONG_ABILITY_NAME);
        names.push_back(name);
        std::vector<RemoteAbilityInfo> infos;
        auto ret = distributedBmsProxy->GetAbilityInfos(names, "", infos);
        EXPECT_EQ(ret, ERR_APPEXECFWK_FAILED_GET_REMOTE_PROXY);
    }
}

/**
 * @tc.number: DbmsServicesKitTest
 * @tc.name: test GetAbilityInfo
 * @tc.require: issueI5MZ8V
 * @tc.desc: 1. system running normally
 *           2. test GetDistributedBundleInfo
 */
HWTEST_F(DbmsServicesKitTest, DbmsServicesKitTest_0023, Function | SmallTest | Level0)
{
    auto distributedBmsProxy = GetDistributedBmsProxy();
    EXPECT_NE(distributedBmsProxy, nullptr);
    if (distributedBmsProxy != nullptr) {
        DistributedBundleInfo distributedBundleInfo;
        auto ret = distributedBmsProxy->GetDistributedBundleInfo("", "", distributedBundleInfo);
        EXPECT_EQ(ret, false);
    }
}

#ifdef DISTRIBUTED_BUNDLE_IMAGE_ENABLE
/**
 * @tc.number: DbmsServicesKitTest
 * @tc.name: test IsPathValid
 * @tc.require: issueI5MZ8V
 * @tc.desc: 1. system running normally
 *           2. test path is not valid
 */
HWTEST_F(DbmsServicesKitTest, DbmsServicesKitTest_0031, Function | SmallTest | Level0)
{
    std::unique_ptr<ImageCompress> imageCompress = std::make_unique<ImageCompress>();
    EXPECT_NE(imageCompress, nullptr);
    if (imageCompress != nullptr) {
        bool res = imageCompress->IsPathValid(HAP_FILE_PATH);
        EXPECT_EQ(res, false);
    }
}

/**
 * @tc.number: DbmsServicesKitTest
 * @tc.name: test GetImageType
 * @tc.require: issueI5MZ8V
 * @tc.desc: 1. system running normally
 *           2. test get image type
 */
HWTEST_F(DbmsServicesKitTest, DbmsServicesKitTest_0032, Function | SmallTest | Level0)
{
    std::unique_ptr<ImageCompress> imageCompress = std::make_unique<ImageCompress>();
    EXPECT_NE(imageCompress, nullptr);
    if (imageCompress != nullptr) {
        std::unique_ptr<uint8_t[]> fileData;
        constexpr size_t fileLength = 7;
        ImageType res = imageCompress->GetImageType(fileData, fileLength);
        EXPECT_EQ(res, ImageType::WORNG_TYPE);
    }
}

/**
 * @tc.number: DbmsServicesKitTest
 * @tc.name: test GetImageTypeString
 * @tc.require: issueI5MZ8V
 * @tc.desc: 1. system running normally
 *           2. get image type failed
 */
HWTEST_F(DbmsServicesKitTest, DbmsServicesKitTest_0033, Function | SmallTest | Level0)
{
    std::unique_ptr<ImageCompress> imageCompress = std::make_unique<ImageCompress>();
    EXPECT_NE(imageCompress, nullptr);
    if (imageCompress != nullptr) {
        std::unique_ptr<uint8_t[]> fileData;
        constexpr size_t fileLength = 7;
        std::string imageType;
        bool res = imageCompress->GetImageTypeString(fileData, fileLength, imageType);
        EXPECT_FALSE(res);
    }
}

/**
 * @tc.number: DbmsServicesKitTest
 * @tc.name: test GetImageFileInfo
 * @tc.require: issueI5MZ8V
 * @tc.desc: 1. system running normally
 *           2. test get image file info failed
 */
HWTEST_F(DbmsServicesKitTest, DbmsServicesKitTest_0034, Function | SmallTest | Level0)
{
    std::unique_ptr<ImageCompress> imageCompress = std::make_unique<ImageCompress>();
    EXPECT_NE(imageCompress, nullptr);
    if (imageCompress != nullptr) {
        std::unique_ptr<uint8_t[]> fileContent;
        int64_t fileLength;
        bool res = imageCompress->
            GetImageFileInfo(
                HAP_FILE_PATH, fileContent, fileLength);
        EXPECT_EQ(res, false);
    }
}
#endif

#ifdef HISYSEVENT_ENABLE
/**
 * @tc.number: DbmsServicesKitTest_0035
 * @tc.name: SendSystemEvent
 * @tc.desc: Send System Event failed
 */
HWTEST_F(DbmsServicesKitTest, DbmsServicesKitTest_0035, Function | SmallTest | Level0)
{
    auto eventReport = GetEventReport();
    EXPECT_NE(eventReport, nullptr);
    if (eventReport != nullptr) {
        DBMSEventType dbmsEventType = DBMSEventType::UNKNOW;
        DBMSEventInfo eventInfo;
        eventReport->SendSystemEvent(dbmsEventType, eventInfo);
    }
}

/**
 * @tc.number: DbmsServicesKitTest_0036
 * @tc.name: SendSystemEvent
 * @tc.desc: Send System Event sucess
 */
HWTEST_F(DbmsServicesKitTest, DbmsServicesKitTest_0036, Function | SmallTest | Level0)
{
    auto eventReport = GetEventReport();
    EXPECT_NE(eventReport, nullptr);
    if (eventReport != nullptr) {
        DBMSEventType dbmsEventType = DBMSEventType::GET_REMOTE_ABILITY_INFO;
        DBMSEventInfo eventInfo;
        eventInfo.deviceID = "deviceID";
        eventInfo.bundleName = "bundleName";
        eventInfo.localeInfo = "localeInfo";
        eventInfo.abilityName = "abilityName";
        eventInfo.resultCode = 0;
        eventReport->SendSystemEvent(dbmsEventType, eventInfo);
    }
}
#endif

/**
 * @tc.number: DbmsServicesKitTest_0037
 * @tc.name: GetKvStore
 * @tc.desc: Get KvStore
 */
HWTEST_F(DbmsServicesKitTest, DbmsServicesKitTest_0037, Function | SmallTest | Level0)
{
    auto distributedDataStorage = GetDistributedDataStorage();
    EXPECT_NE(distributedDataStorage, nullptr);
    if (distributedDataStorage != nullptr) {
        auto ret = distributedDataStorage->GetKvStore();
        EXPECT_EQ(ret, DistributedKv::Status::SUCCESS);
    }
}

/**
 * @tc.number: DbmsServicesKitTest_0038
 * @tc.name: GetLocalUdid
 * @tc.desc: Get Local Udid
 */
HWTEST_F(DbmsServicesKitTest, DbmsServicesKitTest_0038, Function | SmallTest | Level0)
{
    auto distributedDataStorage = GetDistributedDataStorage();
    EXPECT_NE(distributedDataStorage, nullptr);
    if (distributedDataStorage != nullptr) {
        std::string udid = "udid";
        auto ret = distributedDataStorage->GetLocalUdid(udid);
        EXPECT_EQ(ret, true);
    }
}

/**
 * @tc.number: DbmsServicesKitTest_0039
 * @tc.name: GetUdidByNetworkId
 * @tc.desc: Get Udid By NetworkId
 */
HWTEST_F(DbmsServicesKitTest, DbmsServicesKitTest_0039, Function | SmallTest | Level0)
{
    auto distributedDataStorage = GetDistributedDataStorage();
    EXPECT_NE(distributedDataStorage, nullptr);
    if (distributedDataStorage != nullptr) {
        std::string networkId = "networkId";
        std::string udid = "udid";
        auto ret = distributedDataStorage->GetUdidByNetworkId(networkId, udid);
        EXPECT_EQ(ret, 0);
    }
}

/**
 * @tc.number: DbmsServicesKitTest_0040
 * @tc.name: ConvertToDistributedBundleInfo
 * @tc.desc: Convert To Distributed BundleInfo
 */
HWTEST_F(DbmsServicesKitTest, DbmsServicesKitTest_0040, Function | SmallTest | Level0)
{
    auto distributedDataStorage = GetDistributedDataStorage();
    EXPECT_NE(distributedDataStorage, nullptr);
    if (distributedDataStorage != nullptr) {
        BundleInfo bundleInfo;
        bundleInfo.name = "name";
        AbilityInfo abilityInfo;
        abilityInfo.moduleName = "moduleName";
        bundleInfo.abilityInfos.push_back(abilityInfo);
        auto ret = distributedDataStorage->ConvertToDistributedBundleInfo(bundleInfo);
        EXPECT_EQ(ret.bundleName, "name");
    }
}

/**
 * @tc.number: DbmsServicesKitTest_0041
 * @tc.name: ConvertToDistributedBundleInfo
 * @tc.desc: Convert To Distributed BundleInfo
 */
HWTEST_F(DbmsServicesKitTest, DbmsServicesKitTest_0041, Function | SmallTest | Level0)
{
    auto distributedDataStorage = GetDistributedDataStorage();
    EXPECT_NE(distributedDataStorage, nullptr);
    if (distributedDataStorage != nullptr) {
        BundleInfo bundleInfo;
        bundleInfo.name = "name";
        AbilityInfo abilityInfo1;
        abilityInfo1.moduleName = "moduleName";
        bundleInfo.abilityInfos.push_back(abilityInfo1);
        AbilityInfo abilityInfo2;
        abilityInfo2.moduleName = "moduleName";
        bundleInfo.abilityInfos.push_back(abilityInfo2);
        auto ret = distributedDataStorage->ConvertToDistributedBundleInfo(bundleInfo);
        EXPECT_EQ(ret.bundleName, "name");
    }
}

/**
 * @tc.number: DbmsServicesKitTest_0042
 * @tc.name: UpdateDistributedData
 * @tc.desc: Update Distributed Data
 */
HWTEST_F(DbmsServicesKitTest, DbmsServicesKitTest_0042, Function | SmallTest | Level0)
{
    auto distributedDataStorage = GetDistributedDataStorage();
    EXPECT_NE(distributedDataStorage, nullptr);
    if (distributedDataStorage != nullptr) {
        int32_t userId = 1;
        distributedDataStorage->UpdateDistributedData(userId);
    }
}

/**
 * @tc.number: DbmsServicesKitTest_0043
 * @tc.name: CheckKvStore
 * @tc.desc: Check KvStore
 */
HWTEST_F(DbmsServicesKitTest, DbmsServicesKitTest_0043, Function | SmallTest | Level0)
{
    auto distributedDataStorage = GetDistributedDataStorage();
    EXPECT_NE(distributedDataStorage, nullptr);
    if (distributedDataStorage != nullptr) {
        auto ret = distributedDataStorage->CheckKvStore();
        EXPECT_EQ(ret, true);
    }
}

/**
 * @tc.number: DbmsServicesKitTest_0044
 * @tc.name: DeviceAndNameToKey
 * @tc.desc: Device And Name To Key
 */
HWTEST_F(DbmsServicesKitTest, DbmsServicesKitTest_0044, Function | SmallTest | Level0)
{
    auto distributedDataStorage = GetDistributedDataStorage();
    EXPECT_NE(distributedDataStorage, nullptr);
    if (distributedDataStorage != nullptr) {
        std::string udid = "udid";
        std::string bundleName = "bundleName";
        auto ret = distributedDataStorage->DeviceAndNameToKey(udid, bundleName);
        EXPECT_EQ(ret, "udid_bundleName");
    }
}

/**
 * @tc.number: DbmsServicesKitTest_0045
 * @tc.name: GetStorageDistributeInfo
 * @tc.desc: Get Storage DistributeInfo
 */
HWTEST_F(DbmsServicesKitTest, DbmsServicesKitTest_0045, Function | SmallTest | Level0)
{
    auto distributedDataStorage = GetDistributedDataStorage();
    EXPECT_NE(distributedDataStorage, nullptr);
    if (distributedDataStorage != nullptr) {
        std::string networkId = "networkId";
        std::string bundleName = "bundleName";
        DistributedBundleInfo info;
        auto ret = distributedDataStorage->GetStorageDistributeInfo(networkId, bundleName, info);
        EXPECT_EQ(ret, false);
    }
}

/**
 * @tc.number: DbmsServicesKitTest_0046
 * @tc.name: DeleteStorageDistributeInfo
 * @tc.desc: Delete Storage DistributeInfo
 */
HWTEST_F(DbmsServicesKitTest, DbmsServicesKitTest_0046, Function | SmallTest | Level0)
{
    auto distributedDataStorage = GetDistributedDataStorage();
    EXPECT_NE(distributedDataStorage, nullptr);
    if (distributedDataStorage != nullptr) {
        std::string bundleName = "bundleName";
        int32_t userId = 1;
        distributedDataStorage->DeleteStorageDistributeInfo(bundleName, userId);
    }
}

/**
 * @tc.number: DbmsServicesKitTest_0047
 * @tc.name: InnerSaveStorageDistributeInfo
 * @tc.desc: Inner Save Storage DistributeInfo
 */
HWTEST_F(DbmsServicesKitTest, DbmsServicesKitTest_0047, Function | SmallTest | Level0)
{
    auto distributedDataStorage = GetDistributedDataStorage();
    EXPECT_NE(distributedDataStorage, nullptr);
    if (distributedDataStorage != nullptr) {
        DistributedBundleInfo distributedBundleInfo;
        distributedBundleInfo.bundleName = "bundleName";
        auto ret = distributedDataStorage->InnerSaveStorageDistributeInfo(distributedBundleInfo);
        EXPECT_EQ(ret, true);
    }
}

/**
 * @tc.number: DbmsServicesKitTest_0048
 * @tc.name: SaveStorageDistributeInfo
 * @tc.desc: Save Storage DistributeInfo
 */
HWTEST_F(DbmsServicesKitTest, DbmsServicesKitTest_0048, Function | SmallTest | Level0)
{
    auto distributedDataStorage = GetDistributedDataStorage();
    EXPECT_NE(distributedDataStorage, nullptr);
    if (distributedDataStorage != nullptr) {
        std::string bundleName = "bundleName";
        int32_t userId = 1;
        distributedDataStorage->SaveStorageDistributeInfo(bundleName, userId);
    }
}

/**
 * @tc.number: DbmsServicesKitTest_0049
 * @tc.name: OnStart
 * @tc.desc: On Start
 */
HWTEST_F(DbmsServicesKitTest, DbmsServicesKitTest_0049, Function | SmallTest | Level0)
{
    auto distributedBms = GetSptrDistributedBms();
    EXPECT_NE(distributedBms, nullptr);
    if (distributedBms != nullptr) {
        distributedBms->OnStart();
    }
}

/**
 * @tc.number: DbmsServicesKitTest_0050
 * @tc.name: OnStop
 * @tc.desc: On Stop
 */
HWTEST_F(DbmsServicesKitTest, DbmsServicesKitTest_0050, Function | SmallTest | Level0)
{
    auto distributedBms = GetSptrDistributedBms();
    EXPECT_NE(distributedBms, nullptr);
    if (distributedBms != nullptr) {
        distributedBms->OnStop();
    }
}

/**
 * @tc.number: DbmsServicesKitTest_0051
 * @tc.name: Init
 * @tc.desc: Init
 */
HWTEST_F(DbmsServicesKitTest, DbmsServicesKitTest_0051, Function | SmallTest | Level0)
{
    auto distributedBms = GetSptrDistributedBms();
    EXPECT_NE(distributedBms, nullptr);
    if (distributedBms != nullptr) {
        distributedBms->distributedSub_ = nullptr;
        distributedBms->Init();
        EXPECT_NE(distributedBms->distributedSub_, nullptr);
    }
}

/**
 * @tc.number: DbmsServicesKitTest_0052
 * @tc.name: GetRemoteAbilityInfo
 * @tc.desc: Get Remote AbilityInfo
 */
HWTEST_F(DbmsServicesKitTest, DbmsServicesKitTest_0052, Function | SmallTest | Level0)
{
    auto distributedBms = GetDistributedBms();
    EXPECT_NE(distributedBms, nullptr);
    if (distributedBms != nullptr) {
        OHOS::AppExecFwk::ElementName elementName;
        elementName.SetDeviceID("");
        std::string localeInfo = LOCALE_INFO;
        RemoteAbilityInfo remoteAbilityInfo;
        auto ret = distributedBms->GetRemoteAbilityInfo(elementName, localeInfo, remoteAbilityInfo);
        EXPECT_EQ(ret, ERR_BUNDLE_MANAGER_DEVICE_ID_NOT_EXIST);
    }
}

#ifdef DISTRIBUTED_BUNDLE_IMAGE_ENABLE
/**
 * @tc.number: DbmsServicesKitTest_0053
 * @tc.name: test IsPathValid
 * @tc.desc: 1. system running normally
 *           2. The test path is valid
 */
HWTEST_F(DbmsServicesKitTest, DbmsServicesKitTest_0053, Function | SmallTest | Level0)
{
    auto res = InstallBundle(SYSTEM_HAP_FILE_PATH);
    EXPECT_TRUE(res);

    std::unique_ptr<ImageCompress> imageCompress = std::make_unique<ImageCompress>();
    EXPECT_NE(imageCompress, nullptr);
    if (imageCompress != nullptr) {
        bool res = imageCompress->IsPathValid(PATH_LOCATION);
        EXPECT_EQ(res, true);
    }
    res = UninstallBundle(BUNDLE_NAME);
    EXPECT_TRUE(res);
}

/**
 * @tc.number: DbmsServicesKitTest_0054
 * @tc.name: test IsPathValid
 * @tc.desc: 1. system running normally
 *           2. Invalid test path (parameter is "")
 */
HWTEST_F(DbmsServicesKitTest, DbmsServicesKitTest_0054, Function | SmallTest | Level0)
{
    std::unique_ptr<ImageCompress> imageCompress = std::make_unique<ImageCompress>();
    EXPECT_NE(imageCompress, nullptr);
    if (imageCompress != nullptr) {
        bool res = imageCompress->IsPathValid("");
        EXPECT_EQ(res, false);
    }
}

/**
 * @tc.number: DbmsServicesKitTest_0055
 * @tc.name: test IsPathValid
 * @tc.desc: 1. system running normally
 *           2. Invalid test path (parameter is "null")
 */
HWTEST_F(DbmsServicesKitTest, DbmsServicesKitTest_0055, Function | SmallTest | Level0)
{
    std::unique_ptr<ImageCompress> imageCompress = std::make_unique<ImageCompress>();
    EXPECT_NE(imageCompress, nullptr);
    if (imageCompress != nullptr) {
        bool res = imageCompress->IsPathValid("null");
        EXPECT_EQ(res, false);
    }
}

/**
 * @tc.number: DbmsServicesKitTest_0056
 * @tc.name: test IsPathValid
 * @tc.desc: 1. system running normally
 *           2. Invalid test path (parameter is "/data/data11")
 */
HWTEST_F(DbmsServicesKitTest, DbmsServicesKitTest_0056, Function | SmallTest | Level0)
{
    std::unique_ptr<ImageCompress> imageCompress = std::make_unique<ImageCompress>();
    EXPECT_NE(imageCompress, nullptr);
    if (imageCompress != nullptr) {
        bool res = imageCompress->IsPathValid("/data/data11");
        EXPECT_EQ(res, false);
    }
}

/**
 * @tc.number: DbmsServicesKitTest_0057
 * @tc.name: test CalculateRatio
 * @tc.desc: 1. system running normally
 *           2. test CalculateRatio
 */
HWTEST_F(DbmsServicesKitTest, DbmsServicesKitTest_0057, Function | SmallTest | Level0)
{
    std::unique_ptr<ImageCompress> imageCompress = std::make_unique<ImageCompress>();
    EXPECT_NE(imageCompress, nullptr);
    if (imageCompress != nullptr) {
        constexpr size_t filesize = 2;
        double res = imageCompress->CalculateRatio(filesize, "image/webp");
        EXPECT_EQ(res, 8);
    }
}

/**
 * @tc.number: DbmsServicesKitTest_0058
 * @tc.name: test CalculateRatio
 * @tc.desc: 1. system running normally
 *           2. test CalculateRatio (Invalid imageType parameter is "image")
 */
HWTEST_F(DbmsServicesKitTest, DbmsServicesKitTest_0058, Function | SmallTest | Level0)
{
    std::unique_ptr<ImageCompress> imageCompress = std::make_unique<ImageCompress>();
    EXPECT_NE(imageCompress, nullptr);
    if (imageCompress != nullptr) {
        constexpr size_t filesize = 1;
        double res = imageCompress->CalculateRatio(filesize, "image");
        EXPECT_EQ(res, 64);
    }
}

/**
 * @tc.number: DbmsServicesKitTest_0059
 * @tc.name: test CalculateRatio
 * @tc.desc: 1. system running normally
 *           2. test CalculateRatio (Invalid imageType parameter is "/")
 */
HWTEST_F(DbmsServicesKitTest, DbmsServicesKitTest_0059, Function | SmallTest | Level0)
{
    std::unique_ptr<ImageCompress> imageCompress = std::make_unique<ImageCompress>();
    EXPECT_NE(imageCompress, nullptr);
    if (imageCompress != nullptr) {
        constexpr size_t filesize = 4;
        double res = imageCompress->CalculateRatio(filesize, "/");
        EXPECT_EQ(res, 32);
    }
}

/**
 * @tc.number: DbmsServicesKitTest_0060
 * @tc.name: test IsImageNeedCompressBySize
 * @tc.desc: 1. system running normally
 *           2. test IsImageNeedCompressBySize (parameters are 10241)
 */
HWTEST_F(DbmsServicesKitTest, DbmsServicesKitTest_0060, Function | SmallTest | Level0)
{
    std::unique_ptr<ImageCompress> imageCompress = std::make_unique<ImageCompress>();
    EXPECT_NE(imageCompress, nullptr);
    if (imageCompress != nullptr) {
        constexpr size_t fileSize = 10241;
        bool res = imageCompress->IsImageNeedCompressBySize(fileSize);
        EXPECT_EQ(res, true);
    }
}

/**
 * @tc.number: DbmsServicesKitTest_0061
 * @tc.name: test IsImageNeedCompressBySize
 * @tc.desc: 1. system running normally
 *           2. test IsImageNeedCompressBySize (parameters are 10239)
 */
HWTEST_F(DbmsServicesKitTest, DbmsServicesKitTest_0061, Function | SmallTest | Level0)
{
    std::unique_ptr<ImageCompress> imageCompress = std::make_unique<ImageCompress>();
    EXPECT_NE(imageCompress, nullptr);
    if (imageCompress != nullptr) {
        constexpr size_t fileSize = 10239;
        bool res = imageCompress->IsImageNeedCompressBySize(fileSize);
        EXPECT_EQ(res, false);
    }
}

/**
 * @tc.number: DbmsServicesKitTest_0062
 * @tc.name: test GetImageType
 * @tc.desc: 1. system running normally
 *           2. test Get Image Type (The fileData parameter is valid)
 */
HWTEST_F(DbmsServicesKitTest, DbmsServicesKitTest_0062, Function | SmallTest | Level0)
{
    std::unique_ptr<ImageCompress> imageCompress = std::make_unique<ImageCompress>();
    EXPECT_NE(imageCompress, nullptr);
    if (imageCompress != nullptr) {
        constexpr size_t fileLength = 64;
        std::unique_ptr<uint8_t[]> fileData = std::make_unique<uint8_t[]>(fileLength);
        uint8_t *data = fileData.get();
        data[0] = 0xFF;
        data[1] = 0xD8;
        data[2] = 0xFF;
        ImageType res = imageCompress->GetImageType(fileData, fileLength);
        EXPECT_EQ(res, ImageType::JPEG);
    }
}

/**
 * @tc.number: DbmsServicesKitTest_0063
 * @tc.name: test GetImageType
 * @tc.desc: 1. system running normally
 *           2. test Get Image Type (The fileData parameter is invalid)
 */
HWTEST_F(DbmsServicesKitTest, DbmsServicesKitTest_0063, Function | SmallTest | Level0)
{
    std::unique_ptr<ImageCompress> imageCompress = std::make_unique<ImageCompress>();
    EXPECT_NE(imageCompress, nullptr);
    if (imageCompress != nullptr) {
        constexpr size_t fileLength = 64;
        std::unique_ptr<uint8_t[]> fileData = std::make_unique<uint8_t[]>(fileLength);
        uint8_t *data = fileData.get();
        data[0] = 0xFF;
        data[1] = 0xD8;
        ImageType res = imageCompress->GetImageType(fileData, fileLength);
        EXPECT_EQ(res, ImageType::WORNG_TYPE);
    }
}

/**
 * @tc.number: DbmsServicesKitTest_0064
 * @tc.name: test GetImageType
 * @tc.desc: 1. system running normally
 *           2. test Get Image Type (The fileData parameter is valid)
 */
HWTEST_F(DbmsServicesKitTest, DbmsServicesKitTest_0064, Function | SmallTest | Level0)
{
    std::unique_ptr<ImageCompress> imageCompress = std::make_unique<ImageCompress>();
    EXPECT_NE(imageCompress, nullptr);
    if (imageCompress != nullptr) {
        constexpr size_t fileLength = 64;
        std::unique_ptr<uint8_t[]> fileData = std::make_unique<uint8_t[]>(fileLength);
        uint8_t *data = fileData.get();
        data[0] = 0x89;
        data[1] = 0x50;
        data[2] = 0x4E;
        data[3] = 0x47;
        ImageType res = imageCompress->GetImageType(fileData, fileLength);
        EXPECT_EQ(res, ImageType::PNG);
    }
}

/**
 * @tc.number: DbmsServicesKitTest_0065
 * @tc.name: test GetImageType
 * @tc.desc: 1. system running normally
 *           2. test Get Image Type (The fileData parameter is invalid)
 */
HWTEST_F(DbmsServicesKitTest, DbmsServicesKitTest_0065, Function | SmallTest | Level0)
{
    std::unique_ptr<ImageCompress> imageCompress = std::make_unique<ImageCompress>();
    EXPECT_NE(imageCompress, nullptr);
    if (imageCompress != nullptr) {
        constexpr size_t fileLength = 64;
        std::unique_ptr<uint8_t[]> fileData = std::make_unique<uint8_t[]>(fileLength);
        uint8_t *data = fileData.get();
        data[0] = 0x89;
        data[1] = 0x50;
        data[2] = 0x4E;
        ImageType res = imageCompress->GetImageType(fileData, fileLength);
        EXPECT_EQ(res, ImageType::WORNG_TYPE);
    }
}

/**
 * @tc.number: DbmsServicesKitTest_0066
 * @tc.name: test GetImageFileInfo
 * @tc.desc: 1. system running normally
 *           2. test Get Image File Info (The srcFile parameter is invalid)
 */
HWTEST_F(DbmsServicesKitTest, DbmsServicesKitTest_0066, Function | SmallTest | Level0)
{
    std::unique_ptr<ImageCompress> imageCompress = std::make_unique<ImageCompress>();
    EXPECT_NE(imageCompress, nullptr);
    if (imageCompress != nullptr) {
        std::unique_ptr<uint8_t[]> fileContent;
        int64_t fileLength;
        bool res = imageCompress->GetImageFileInfo(PATH_LOCATION, fileContent, fileLength);
        EXPECT_EQ(res, false);
    }
}

/**
 * @tc.number: DbmsServicesKitTest_0067
 * @tc.name: test GetImageFileInfo
 * @tc.desc: 1. system running normally
 *           2. test Get Image File Info (The srcFile parameter is valid)
 */
HWTEST_F(DbmsServicesKitTest, DbmsServicesKitTest_0067, Function | SmallTest | Level0)
{
    std::unique_ptr<ImageCompress> imageCompress = std::make_unique<ImageCompress>();
    EXPECT_NE(imageCompress, nullptr);
    std::fstream fs;
    fs.open(PATH_LOCATIONS, std::ios_base::app);
    fs.close();
    std::ofstream ofs;
    ofs.open(PATH_LOCATIONS, std::ios::out|std::ios::app);
    ofs << "test" << " ";
    ofs.close();
    if (imageCompress != nullptr) {
        int64_t fileLength = 64;
        std::unique_ptr<uint8_t[]> fileContent = std::make_unique<uint8_t[]>(fileLength);
        bool res = imageCompress->GetImageFileInfo(PATH_LOCATIONS, fileContent, fileLength);
        EXPECT_EQ(res, true);
    }
    std::string savePath = PATH_LOCATIONS;
    EXPECT_EQ(remove(savePath.c_str()), 0);
}

/**
 * @tc.number: DbmsServicesKitTest_0068
 * @tc.name: test GetImageFileInfo
 * @tc.desc: 1. system running normally
 *           2. test CompressImageByContent (The fileData parameter is invalid)
 */
HWTEST_F(DbmsServicesKitTest, DbmsServicesKitTest_0068, Function | SmallTest | Level0)
{
    std::unique_ptr<ImageCompress> imageCompress = std::make_unique<ImageCompress>();
    EXPECT_NE(imageCompress, nullptr);
    if (imageCompress != nullptr) {
        std::unique_ptr<uint8_t[]> fileData;
        std::unique_ptr<uint8_t[]> compressedData;
        constexpr size_t fileSize = 7;
        int64_t compressedSize = 2;
        std::string imageType = "image/jpeg";
        bool res = imageCompress->CompressImageByContent(fileData, fileSize, compressedData, compressedSize, imageType);
        EXPECT_EQ(res, false);
    }
}

/**
 * @tc.number: DbmsServicesKitTest_0069
 * @tc.name: test GetImageFileInfo
 * @tc.desc: 1. system running normally
 *           2. test CompressImageByContent (The fileData parameter is valid)
 */
HWTEST_F(DbmsServicesKitTest, DbmsServicesKitTest_0069, Function | SmallTest | Level0)
{
    std::unique_ptr<ImageCompress> imageCompress = std::make_unique<ImageCompress>();
    EXPECT_NE(imageCompress, nullptr);
    if (imageCompress != nullptr) {
        std::unique_ptr<uint8_t[]> fileData = std::make_unique<uint8_t[]>(100);
        uint8_t *data = fileData.get();
        data[0] = 0xFF;
        data[1] = 0xD8;
        data[2] = 0xFF;
        std::unique_ptr<uint8_t[]> compressedData = std::make_unique<uint8_t[]>(100);
        constexpr size_t fileSize = 8;
        int64_t compressedSize = 2;
        std::string imageType = "image/jpeg";
        bool res = imageCompress->CompressImageByContent(fileData, fileSize, compressedData, compressedSize, imageType);
        EXPECT_EQ(res, false);
    }
}
#endif

/**
 * @tc.number: DbmsServicesKitTest_0070
 * @tc.name: test CheckElementName
 * @tc.desc: 1. system running normally
 *           2. test CheckElementName
 */
HWTEST_F(DbmsServicesKitTest, DbmsServicesKitTest_0070, Function | SmallTest | Level0)
{
    auto distributedBmsProxy = GetDistributedBmsProxy();
    EXPECT_NE(distributedBmsProxy, nullptr);
    if (distributedBmsProxy != nullptr) {
        ElementName name;
        name.SetBundleName(BUNDLE_NAME);
        name.SetAbilityName(WRONG_ABILITY_NAME);
        name.SetDeviceID(DEVICE_ID);
        auto ret = distributedBmsProxy->CheckElementName(name);
        EXPECT_EQ(ret, ERR_OK);
    }
}

/**
 * @tc.number: DbmsServicesKitTest_0071
 * @tc.name: test GetAbilityInfo
 * @tc.desc: 1. system running normally
 *           2. test bundleName empty
 */
HWTEST_F(DbmsServicesKitTest, DbmsServicesKitTest_0071, Function | SmallTest | Level0)
{
    auto distributedBmsProxy = GetDistributedBmsProxy();
    EXPECT_NE(distributedBmsProxy, nullptr);
    if (distributedBmsProxy != nullptr) {
        ElementName name;
        RemoteAbilityInfo info;
        auto ret = distributedBmsProxy->GetRemoteAbilityInfo(name, info);
        EXPECT_EQ(ret, ERR_BUNDLE_MANAGER_BUNDLE_NOT_EXIST);
    }
}

/**
* @tc.number: DbmsServicesKitTest_0072
* @tc.name: GetRemoteAbilityInfo
* @tc.desc: Get Remote AbilityInfo
*/
HWTEST_F(DbmsServicesKitTest, DbmsServicesKitTest_0072, Function | SmallTest | Level0)
{
    auto distributedBms = GetDistributedBms();
    EXPECT_NE(distributedBms, nullptr);
    if (distributedBms != nullptr) {
        OHOS::AppExecFwk::ElementName elementName;
        std::string deviceId = DEVICE_ID_NORMAL;
        elementName.SetDeviceID(deviceId);
        std::string localeInfo = LOCALE_INFO;
        RemoteAbilityInfo remoteAbilityInfo;
        auto ret = distributedBms->GetRemoteAbilityInfo(elementName, localeInfo, remoteAbilityInfo);
        EXPECT_EQ(ret, ERR_BUNDLE_MANAGER_DEVICE_ID_NOT_EXIST);
    }
}

/**
* @tc.number: DbmsServicesKitTest_0073
* @tc.name: test GetRemoteAbilityInfos
* @tc.desc: Get Remote AbilityInfos
*/
HWTEST_F(DbmsServicesKitTest, DbmsServicesKitTest_0073, Function | SmallTest | Level0)
{
    auto distributedBms = GetDistributedBms();
    EXPECT_NE(distributedBms, nullptr);
    if (distributedBms != nullptr) {
        std::vector<ElementName> names;
        ElementName name;
        name.SetBundleName(BUNDLE_NAME);
        name.SetAbilityName(ABILITY_NAME);
        name.SetDeviceID(EMPTY_STRING);
        names.push_back(name);
        std::vector<RemoteAbilityInfo> info;
        auto ret = distributedBms->GetRemoteAbilityInfos(names, EMPTY_STRING, info);
        EXPECT_EQ(ret, ERR_BUNDLE_MANAGER_DEVICE_ID_NOT_EXIST);
    }
}

/**
* @tc.number: DbmsServicesKitTest_0074
* @tc.name: test GetRemoteAbilityInfos
* @tc.desc: Get Remote AbilityInfos
*/
HWTEST_F(DbmsServicesKitTest, DbmsServicesKitTest_0074, Function | SmallTest | Level0)
{
    auto distributedBms = GetDistributedBms();
    EXPECT_NE(distributedBms, nullptr);
    if (distributedBms != nullptr) {
        std::vector<ElementName> names;
        ElementName name;
        name.SetBundleName(BUNDLE_NAME);
        name.SetAbilityName(ABILITY_NAME);
        name.SetDeviceID(DEVICE_ID);
        names.push_back(name);
        std::vector<RemoteAbilityInfo> info;
        auto ret = distributedBms->GetRemoteAbilityInfos(names, EMPTY_STRING, info);
        EXPECT_EQ(ret, ERR_BUNDLE_MANAGER_DEVICE_ID_NOT_EXIST);
    }
}

/**
 * @tc.number: DbmsServicesKitTest
 * @tc.name: test GetDistributedBundleName
 * @tc.desc: 1. get distributed bundle name by networkId and accessTokenId
 */
HWTEST_F(DbmsServicesKitTest, DbmsServicesKitTest_0075, Function | SmallTest | Level0)
{
    auto distributedBmsProxy = GetDistributedBmsProxy();
    std::string bundleName;
    EXPECT_NE(distributedBmsProxy, nullptr);
    if (distributedBmsProxy != nullptr) {
        DistributedBundleInfo distributedBundleInfo;
        int32_t ret = distributedBmsProxy->GetDistributedBundleName("", 0, bundleName);
        EXPECT_EQ(ret, ERR_APPEXECFWK_FAILED_GET_REMOTE_PROXY);
    }
}

/**
 * @tc.number: InitDeviceManager_0100
 * @tc.name: test InitDeviceManager
 * @tc.desc: isInit_ is true, return true.
 */
HWTEST_F(DbmsServicesKitTest, InitDeviceManager_0100, Function | SmallTest | Level0)
{
    DbmsDeviceManager deviceManager;
    deviceManager.isInit_ = true;
    bool res = deviceManager.InitDeviceManager();
    EXPECT_TRUE(res);
}

/**
 * @tc.number: InitDeviceManager_0200
 * @tc.name: test InitDeviceManager
 * @tc.desc: isInit_ is false, return true.
 */
HWTEST_F(DbmsServicesKitTest, InitDeviceManager_0200, Function | SmallTest | Level0)
{
    DbmsDeviceManager deviceManager;
    deviceManager.isInit_ = false;
    bool res = deviceManager.InitDeviceManager();
    EXPECT_TRUE(res);
}

/**
 * @tc.number: GetUdidByNetworkId_0100
 * @tc.name: test GetUdidByNetworkId
 * @tc.desc: GetUdidByNetworkId is false
 */
HWTEST_F(DbmsServicesKitTest, GetUdidByNetworkId_0100, Function | SmallTest | Level0)
{
    DbmsDeviceManager deviceManager;
    std::string netWorkId = EMPTY_STRING;
    std::string uid = EMPTY_STRING;
    auto ret = deviceManager.GetUdidByNetworkId(netWorkId, uid);
    EXPECT_FALSE(ret == -1);
}

/**
 * @tc.number: VerifyCallingPermission_0100
 * @tc.name: Test VerifyCallingPermission
 * @tc.desc: Verify the VerifyCallingPermission return true.
 */
HWTEST_F(DbmsServicesKitTest, VerifyCallingPermission_0100, Function | MediumTest | Level1)
{
    auto distributedBms = GetDistributedBms();
    EXPECT_NE(distributedBms, nullptr);
    int res = distributedBms->VerifyCallingPermission(Constants::PERMISSION_GET_BUNDLE_INFO_PRIVILEGED);
    EXPECT_TRUE(res);
}

/**
 * @tc.number: VerifyCallingPermission_0200
 * @tc.name: Test VerifyCallingPermission
 * @tc.desc: Verify the VerifyCallingPermission return false.
 */
HWTEST_F(DbmsServicesKitTest, VerifyCallingPermission_0200, Function | MediumTest | Level1)
{
    auto distributedBms = GetDistributedBms();
    EXPECT_NE(distributedBms, nullptr);
    int res = distributedBms->VerifyCallingPermission("");
    EXPECT_FALSE(res);
}
} // OHOS