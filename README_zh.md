# **分布式包管理服务(DBMS)**

## 简介

分布式包管理服务负责管理跨设备的组件调度和任务管理，实现跨设备RPC的能力，可以按需获取跨设备指定语言的资源。

## 目录

```
foundation/bundlemanager/distributed_bundle_framework
├── interfaces
│   ├── inner_api                 # 内部接口存放目录
│   └── kits/js                   # JS应用接口
│       ├── distributebundlemgr        
│       └── distributedBundle          
├── services/dbms                 # dbms服务框架代码
└── services/dbms/test	          # 测试目录
```

## 说明
### 获取远程设备AbilityInfo信息
getRemoteAbilityInfo获取由elementName指定的远程设备上的应用的AbilityInfo信息(callback形式)

* **参数：**

  | 参数名      | 类型                                                         | 必填 | 说明                                                         |
  | ----------- | ------------------------------------------------------------ | ---- | ------------------------------------------------------------ |
  | elementName | [ElementName](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/reference/apis/js-apis-bundleManager-elementName.md) | 是   | ElementName信息。                                            |
  | callback    | AsyncCallback<[RemoteAbilityInfo](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/reference/apis/js-apis-bundleManager-remoteAbilityInfo.md)> | 是   | 回调函数，调用成功返回err为null，data为RemoteAbilityInfo对象；调用失败err为错误对象, data为undefined。 |

* **错误码：**

  以下错误码的详细介绍请参见[ohos.bundle错误码](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/reference/errorcodes/errorcode-bundle.md)。

  | 错误码ID | 错误信息                                 |
  | -------- | ---------------------------------------- |
  | 17700001 | The specified bundle name is not found.  |
  | 17700003 | The specified ability name is not found. |
  | 17700007 | The specified device ID is not found.    |
  | 17700027 | The distributed service is not running.  |

* **示例：**

```ts
  try {
      distributedBundle.getRemoteAbilityInfo(
          {
              deviceId: '1',
              bundleName: 'com.example.application',
              abilityName: 'EntryAbility'
          }, (err, data) => {
            if (err) {
              console.log(`Operation failed: error code is ${err.code}  and error message is ${err.message}`);
            } else {
              console.info('Operation succeed:' + JSON.stringify(data));
            }
          });
  } catch (err) {
      console.log(`Operation failed: error code is ${err.code}  and error message is ${err.message}`);
  }
  ```

* **指南：**

  更多开发指导可参考[**示例文档**](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/reference/apis/js-apis-distributedBundleManager.md)

## 相关仓

[bundlemanager_bundle_framework](https://gitee.com/openharmony/bundlemanager_bundle_framework)
