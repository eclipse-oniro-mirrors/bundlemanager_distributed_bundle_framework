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

export default namespace distributedBundleManager {
  loadLibraryWithPermissionCheck("ani_distributed_bundle_manager.z", "@ohos.bundle.distributedBundleManager");

  const ERROR_PARAM_CHECK_ERROR: int = 401;
  const GET_REMOTE_ABILITY_INFO_MAX_SIZE: int = 10;
  const ELEMENT_NAMES_SIZE_ERROR: string = "BusinessError 401: The number of ElementNames is greater than 10";
  const ELEMENT_NAME_ERROR: string = "BusinessError 401: Parameter error. The type of elementName must be object.";

  function createBusinessError(code: int, message: string) {
    let err = new BusinessError();
    err.code = code;
    err.name = 'Error';
    err.message = message;
    return err;
  }

  function checkElementName(elementName: ElementName) {
    if (elementName.deviceId === undefined) {
      throw createBusinessError(ERROR_PARAM_CHECK_ERROR, ELEMENT_NAME_ERROR);
    }
  }

  function checkElementNames(elementNames: Array<ElementName>) {
    if (elementNames.length === 0) {
      throw createBusinessError(ERROR_PARAM_CHECK_ERROR, ELEMENT_NAME_ERROR);
    }
    for (const elementName of elementNames) {
      if (elementName.deviceId === undefined) {
        throw createBusinessError(ERROR_PARAM_CHECK_ERROR, ELEMENT_NAME_ERROR);
      }
    }
    if (elementNames.length > GET_REMOTE_ABILITY_INFO_MAX_SIZE) {
      throw createBusinessError(ERROR_PARAM_CHECK_ERROR, ELEMENT_NAMES_SIZE_ERROR);
    }
  }

  native function getRemoteAbilityInfoNative(elementNames: Array<ElementName>, locale: string): RemoteAbilityInfo;
  native function getRemoteAbilityInfosNative(elementNames: Array<ElementName>, locale: string): Array<RemoteAbilityInfo>;

  function elementName2Array(elementName: ElementName): Array<ElementName> {
    let elementNames: Array<ElementName> = new Array<ElementName>();
    elementNames.push(elementName);
    return elementNames;
  }

  function getRemoteAbilityInfo(elementName: ElementName, callback: AsyncCallback<RemoteAbilityInfo>): void {
    checkElementName(elementName);
    let cb = (): RemoteAbilityInfo => {
      return getRemoteAbilityInfoNative(elementName2Array(elementName), '');
    };
    let p1 = taskpool.execute(cb);
    p1.then((e: NullishType) => {
      let result: RemoteAbilityInfo = e as RemoteAbilityInfo;
      callback(null, result);
    }, (err: Error): void => {
      callback(err as BusinessError, undefined);
    });
  }

  function getRemoteAbilityInfo(elementName: ElementName): Promise<RemoteAbilityInfo> {
    checkElementName(elementName);
    let p = new Promise<RemoteAbilityInfo>((resolve: (remoteAbilityInfo: RemoteAbilityInfo) => void, reject: (error: BusinessError) => void) => {
      let cb = (): RemoteAbilityInfo => {
        return getRemoteAbilityInfoNative(elementName2Array(elementName), '');
      };
      let p1 = taskpool.execute(cb);
      p1.then((e: NullishType) => {
        let result: RemoteAbilityInfo = e as RemoteAbilityInfo;
        resolve(result);
      }, (err: Error): void => {
        reject(err as BusinessError);
      });
    });
    return p;
  }

  function getRemoteAbilityInfo(elementNames: Array<ElementName>, callback: AsyncCallback<Array<RemoteAbilityInfo>>): void {
    checkElementNames(elementNames);
    let cb = (): (Array<RemoteAbilityInfo>) => {
      return getRemoteAbilityInfosNative(elementNames, '');
    };
    let p1 = taskpool.execute(cb);
    p1.then((e: NullishType) => {
      let resultArray: Array<RemoteAbilityInfo> = e as Array<RemoteAbilityInfo>;
      callback(null, resultArray);
    }, (err: Error): void => {
      callback(err as BusinessError, undefined);
    });
  }

  function getRemoteAbilityInfo(elementNames: Array<ElementName>): Promise<Array<RemoteAbilityInfo>> {
    checkElementNames(elementNames);
    let p = new Promise<Array<RemoteAbilityInfo>>((resolve: (arrRemoteAbilityInfo: Array<RemoteAbilityInfo>) => void, reject: (error: BusinessError) => void) => {
      let cb = (): (Array<RemoteAbilityInfo>) => {
        return getRemoteAbilityInfosNative(elementNames, '');
      };
      let p1 = taskpool.execute(cb);
      p1.then((e: NullishType) => {
        let resultArray: Array<RemoteAbilityInfo> = e as Array<RemoteAbilityInfo>;
        resolve(resultArray);
      }, (err: Error): void => {
        reject(err as BusinessError);
      });
    });
    return p;
  }

  function getRemoteAbilityInfo(elementName: ElementName, locale: string, callback: AsyncCallback<RemoteAbilityInfo>): void {
    checkElementName(elementName);
    let cb = (): RemoteAbilityInfo => {
      return getRemoteAbilityInfoNative(elementName2Array(elementName), locale);
    };
    let p1 = taskpool.execute(cb);
    p1.then((e: NullishType) => {
      let result: RemoteAbilityInfo = e as RemoteAbilityInfo;
      callback(null, result);
    }, (err: Error): void => {
      callback(err as BusinessError, undefined);
    });
  }

  function getRemoteAbilityInfo(elementName: ElementName, locale: string): Promise<RemoteAbilityInfo> {
    checkElementName(elementName);
    let p = new Promise<RemoteAbilityInfo>((resolve: (remoteAbilityInfo: RemoteAbilityInfo) => void, reject: (error: BusinessError) => void) => {
      let cb = (): RemoteAbilityInfo => {
        return getRemoteAbilityInfoNative(elementName2Array(elementName), locale);
      };
      let p1 = taskpool.execute(cb);
      p1.then((e: NullishType) => {
        let result: RemoteAbilityInfo = e as RemoteAbilityInfo;
        resolve(result);
      }, (err: Error): void => {
        reject(err as BusinessError);
      });
    });
    return p;
  }

  function getRemoteAbilityInfo(elementNames: Array<ElementName>, locale: string, callback: AsyncCallback<Array<RemoteAbilityInfo>>): void {
    checkElementNames(elementNames);
    let cb = (): (Array<RemoteAbilityInfo>) => {
      return getRemoteAbilityInfosNative(elementNames, locale);
    };
    let p1 = taskpool.execute(cb);
    p1.then((e: NullishType) => {
      let resultArray: Array<RemoteAbilityInfo> = e as Array<RemoteAbilityInfo>;
      callback(null, resultArray);
    }, (err: Error): void => {
      callback(err as BusinessError, undefined);
    });
  }

  function getRemoteAbilityInfo(elementNames: Array<ElementName>, locale: string): Promise<Array<RemoteAbilityInfo>> {
    checkElementNames(elementNames);
    let p = new Promise<Array<RemoteAbilityInfo>>((resolve: (arrRemoteAbilityInfo: Array<RemoteAbilityInfo>) => void, reject: (error: BusinessError) => void) => {
      let cb = (): (Array<RemoteAbilityInfo>) => {
        return getRemoteAbilityInfosNative(elementNames, locale);
      };
      let p1 = taskpool.execute(cb);
      p1.then((e: NullishType) => {
        let resultArray: Array<RemoteAbilityInfo> = e as Array<RemoteAbilityInfo>;
        resolve(resultArray);
      }, (err: Error): void => {
        reject(err as BusinessError);
      });
    });
    return p;
  }

  export type RemoteAbilityInfo = _RemoteAbilityInfo;
}
