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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_DBMS_INCLUDE_IMAGE_COMPRESS_H
#define FOUNDATION_APPEXECFWK_SERVICES_DBMS_INCLUDE_IMAGE_COMPRESS_H

#include <stdio.h>
#include <cstdlib>
#include <string>
#include <cmath>

namespace OHOS {
namespace AppExecFwk {
    enum class ImageType {
        JPEG = 1,
        PNG = 2,
        WORNG_TYPE = 3,
    };
class ImageCompress {
public:
    ImageCompress() = default;
    bool IsPathValid(const std::string &srcPath);
    bool IsImageNeedCompressBySize(size_t fileSize);
    double CalculateRatio(size_t fileSize, const std::string &imageType);
    ImageType GetImageType(const std::unique_ptr<uint8_t[]> &fileData, size_t fileLength);
    bool GetImageTypeString(const std::unique_ptr<uint8_t[]> &fileData, size_t fileLength, std::string &imageType);
    bool GetImageFileInfo(const std::string &srcFile, std::unique_ptr<uint8_t[]> &fileContent, int64_t &fileLength);
    bool CompressImageByContent(const std::unique_ptr<uint8_t[]> &fileData, size_t fileSize,
        std::unique_ptr<uint8_t[]> &compressedData, int64_t &compressedSize, std::string &imageType);
};
}
}

#endif // FOUNDATION_APPEXECFWK_SERVICES_DBMS_INCLUDE_IMAGE_COMPRESS_H