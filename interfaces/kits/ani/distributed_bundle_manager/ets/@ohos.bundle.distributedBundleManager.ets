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

/**
 * @file
 * @kit AbilityKit
 */

import { AsyncCallback, BusinessError } from '@ohos.base';
import { ElementName } from 'bundleManager.ElementName';
import { RemoteAbilityInfo as _RemoteAbilityInfo } from 'bundleManager.RemoteAbilityInfo';
import { RemoteAbilityInfoInner } from './bundleManager/RemoteAbilityInfoInner';

export default namespace distributedBundleManager {
  loadLibrary("ani_distributed_bundle_manager.z");

  export native function getRemoteAbilityInfoNative(elementNames: Array<ElementName>, locale: string): RemoteAbilityInfo;
  export native function getRemoteAbilityInfosNative(elementNames: Array<ElementName>, locale: string): Array<RemoteAbilityInfo>;

  function elementName2Array(elementName: ElementName): Array<ElementName> {
    let elementNames: Array<ElementName> = new Array<ElementName>();
    elementNames.push(elementName);
    return elementNames;
  }

  function getRemoteAbilityInfo(elementName: ElementName, callback: AsyncCallback<RemoteAbilityInfo>): void {
    let cb = (): RemoteAbilityInfo => {
      return distributedBundleManager.getRemoteAbilityInfoNative(elementName2Array(elementName), '');
    };
    let p1 = taskpool.execute(cb);
    p1.then((e: NullishType) => {
      let result: RemoteAbilityInfo = e as RemoteAbilityInfo;
      let br: BusinessError = new BusinessError();
      callback(br, result);
    }, (err: BusinessError): void => {
      let result: RemoteAbilityInfo = new RemoteAbilityInfoInner;
      callback(err, result);
    });
  }

  function getRemoteAbilityInfo(elementName: ElementName): Promise<RemoteAbilityInfo> {
    let p = new Promise<RemoteAbilityInfo>((resolve: (remoteAbilityInfo: RemoteAbilityInfo) => void, reject: (error: BusinessError) => void) => {
      let cb = (): RemoteAbilityInfo => {
        return distributedBundleManager.getRemoteAbilityInfoNative(elementName2Array(elementName), '');
      };
      let p1 = taskpool.execute(cb);
      p1.then((e: NullishType) => {
        let result: RemoteAbilityInfo = e as RemoteAbilityInfo;
        resolve(result);
      }, (err: BusinessError): void => {
        reject(err);
      });
    });
    return p;
  }

  function getRemoteAbilityInfo(elementNames: Array<ElementName>, callback: AsyncCallback<Array<RemoteAbilityInfo>>): void {
    let cb = (): (Array<RemoteAbilityInfo>) => {
      return distributedBundleManager.getRemoteAbilityInfosNative(elementNames, '');
    };
    let p1 = taskpool.execute(cb);
    p1.then((e: NullishType) => {
      let resultArray: Array<RemoteAbilityInfo> = e as Array<RemoteAbilityInfo>;
      let br: BusinessError = new BusinessError();
      callback(br, resultArray);
    }, (err: BusinessError): void => {
      let resultArray: Array<RemoteAbilityInfo> = new Array<RemoteAbilityInfo>;
      callback(err, resultArray);
    });
  }

  function getRemoteAbilityInfo(elementNames: Array<ElementName>): Promise<Array<RemoteAbilityInfo>> {
    let p = new Promise<Array<RemoteAbilityInfo>>((resolve: (arrRemoteAbilityInfo: Array<RemoteAbilityInfo>) => void, reject: (error: BusinessError) => void) => {
      let cb = (): (Array<RemoteAbilityInfo>) => {
        return distributedBundleManager.getRemoteAbilityInfosNative(elementNames, '');
      };
      let p1 = taskpool.execute(cb);
      p1.then((e: NullishType) => {
        let resultArray: Array<RemoteAbilityInfo> = e as Array<RemoteAbilityInfo>;
        resolve(resultArray);
      }, (err: BusinessError): void => {
        reject(err);
      });
    });
    return p;
  }

  function getRemoteAbilityInfo(elementName: ElementName, locale: string, callback: AsyncCallback<RemoteAbilityInfo>): void {
    let cb = (): RemoteAbilityInfo => {
      return distributedBundleManager.getRemoteAbilityInfoNative(elementName2Array(elementName), locale);
    };
    let p1 = taskpool.execute(cb);
    p1.then((e: NullishType) => {
      let result: RemoteAbilityInfo = e as RemoteAbilityInfo;
      let br: BusinessError = new BusinessError();
      callback(br, result);
    }, (err: BusinessError): void => {
      let result: RemoteAbilityInfo = new RemoteAbilityInfoInner;
      callback(err, result);
    });
  }

  function getRemoteAbilityInfo(elementName: ElementName, locale: string): Promise<RemoteAbilityInfo> {
    let p = new Promise<RemoteAbilityInfo>((resolve: (remoteAbilityInfo: RemoteAbilityInfo) => void, reject: (error: BusinessError) => void) => {
      let cb = (): RemoteAbilityInfo => {
        return distributedBundleManager.getRemoteAbilityInfoNative(elementName2Array(elementName), locale);
      };
      let p1 = taskpool.execute(cb);
      p1.then((e: NullishType) => {
        let result: RemoteAbilityInfo = e as RemoteAbilityInfo;
        resolve(result);
      }, (err: BusinessError): void => {
        reject(err);
      });
    });
    return p;
  }

  function getRemoteAbilityInfo(elementNames: Array<ElementName>, locale: string, callback: AsyncCallback<Array<RemoteAbilityInfo>>): void {
    let cb = (): (Array<RemoteAbilityInfo>) => {
      return distributedBundleManager.getRemoteAbilityInfosNative(elementNames, locale);
    };
    let p1 = taskpool.execute(cb);
    p1.then((e: NullishType) => {
      let resultArray: Array<RemoteAbilityInfo> = e as Array<RemoteAbilityInfo>;
      let br: BusinessError = new BusinessError();
      callback(br, resultArray);
    }, (err: BusinessError): void => {
      let resultArray: Array<RemoteAbilityInfo> = new Array<RemoteAbilityInfo>;
      callback(err, resultArray);
    });
  }

  function getRemoteAbilityInfo(elementNames: Array<ElementName>, locale: string): Promise<Array<RemoteAbilityInfo>> {
    let p = new Promise<Array<RemoteAbilityInfo>>((resolve: (arrRemoteAbilityInfo: Array<RemoteAbilityInfo>) => void, reject: (error: BusinessError) => void) => {
      let cb = (): (Array<RemoteAbilityInfo>) => {
        return distributedBundleManager.getRemoteAbilityInfosNative(elementNames, locale);
      };
      let p1 = taskpool.execute(cb);
      p1.then((e: NullishType) => {
        let resultArray: Array<RemoteAbilityInfo> = e as Array<RemoteAbilityInfo>;
        resolve(resultArray);
      }, (err: BusinessError): void => {
        reject(err);
      });
    });
    return p;
  }

  export type RemoteAbilityInfo = _RemoteAbilityInfo;
}
