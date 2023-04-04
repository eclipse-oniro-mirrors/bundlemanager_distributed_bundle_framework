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
#include <cstdio>
#include <cmath>
#include <unistd.h>


#include "image_compress.h"

#include "app_log_wrapper.h"
#include "securec.h"
#include "image_source.h"
#include "image_packer.h"
#include "media_errors.h"

namespace OHOS {
namespace AppExecFwk {
namespace {
    constexpr size_t FORMAT_LENGTH = 8;
    const std::string JPEG_FORMAT = "image/jpeg";
    const std::string PNG_FORMAT = "image/png";
    const std::string WEBP_FORMAT = "image/webp";
    const std::string BUNDLE_PATH = "/data/app/el1/bundle";
    constexpr int32_t QUALITY = 20;
    constexpr int32_t MUNBER_ONE = 1;
    constexpr int64_t BUFFER_SIZE = 2 * 1024 * 1024;
    constexpr int32_t FILE_MAX_SIZE = 10240;
    constexpr int32_t FILE_COMPRESS_SIZE = 4096;
    constexpr int32_t WEBP_COMPRESS_SIZE = 128;
    constexpr uint8_t JPEG_DATA_ZERO = 0xFF;
    constexpr uint8_t JPEG_DATA_ONE = 0xD8;
    constexpr uint8_t JPEG_DATA_TWO = 0xFF;
    constexpr uint8_t PNG_DATA_ZERO = 0x89;
    constexpr uint8_t PNG_DATA_ONE = 0x50;
    constexpr uint8_t PNG_DATA_TWO = 0x4E;
    constexpr uint8_t PNG_DATA_THREE = 0x47;
    constexpr int32_t INDEX_ZERO = 0;
    constexpr int32_t INDEX_ONE = 1;
    constexpr int32_t INDEX_TWO = 2;
    constexpr int32_t INDEX_THREE = 3;
}
bool ImageCompress::IsPathValid(const std::string &srcPath)
{
    if (srcPath.find(BUNDLE_PATH) != std::string::npos) {
        return access(srcPath.c_str(), R_OK) == 0;
    } else {
        return false;
    }
}

bool ImageCompress::IsImageNeedCompressBySize(size_t fileSize)
{
    return fileSize > FILE_MAX_SIZE;
}

double ImageCompress::CalculateRatio(size_t fileSize, const std::string &imageType)
{
    if (imageType == WEBP_FORMAT) {
        return sqrt(static_cast<double>(WEBP_COMPRESS_SIZE) / fileSize);
    }
    return sqrt(static_cast<double>(FILE_COMPRESS_SIZE) / fileSize);
}

ImageType ImageCompress::GetImageType(const std::unique_ptr<uint8_t[]> &fileData, size_t fileLength)
{
    if (fileLength < FORMAT_LENGTH) {
        return ImageType::WORNG_TYPE;
    }
    const uint8_t* data = fileData.get();
    if (data[INDEX_ZERO] == JPEG_DATA_ZERO && data[INDEX_ONE] == JPEG_DATA_ONE
        && data[INDEX_TWO] == JPEG_DATA_TWO) {
        return ImageType::JPEG;
    } else if (data[INDEX_ZERO] == PNG_DATA_ZERO && data[INDEX_ONE] == PNG_DATA_ONE &&
        data[INDEX_TWO] == PNG_DATA_TWO && data[INDEX_THREE] == PNG_DATA_THREE) {
        return ImageType::PNG;
    } else {
        return ImageType::WORNG_TYPE;
    }
}

bool ImageCompress::GetImageTypeString(const std::unique_ptr<uint8_t[]> &fileData,
    size_t fileLength, std::string &imageType)
{
    ImageType type = GetImageType(fileData, fileLength);
    if (type == ImageType::WORNG_TYPE) {
        APP_LOGE("input wrong type image!");
        return false;
    }
    imageType = type == ImageType::JPEG ? JPEG_FORMAT : PNG_FORMAT;
    return true;
}

bool ImageCompress::GetImageFileInfo(const std::string &srcFile,
    std::unique_ptr<uint8_t[]> &fileContent, int64_t &fileLength)
{
    if (!IsPathValid(srcFile)) {
        APP_LOGE("%{public}s is unavailable", srcFile.c_str());
        return false;
    }
    FILE* file = fopen(srcFile.c_str(), "rb");
    if (!file) {
        APP_LOGE("ImageCompress: GetImageTypeByFile %{public}s is unavailable", srcFile.c_str());
        return false;
    }
    if (fseek(file, 0L, SEEK_END) != 0) {
        fclose(file);
        return false;
    }
    fileLength = ftell(file);
    rewind(file);
    fileContent = std::make_unique<uint8_t[]>(fileLength);
    if (!fread(fileContent.get(), sizeof(uint8_t), fileLength, file)) {
        APP_LOGE("read file failed!");
        fclose(file);
        return false;
    }
    if (fclose(file) != 0) {
        APP_LOGE("close file failed!");
        return false;
    }
    return true;
}

bool ImageCompress::CompressImageByContent(const std::unique_ptr<uint8_t[]> &fileData, size_t fileSize,
    std::unique_ptr<uint8_t[]> &compressedData, int64_t &compressedSize, std::string &imageType)
{
    ImageType type = GetImageType(fileData, fileSize);
    if (type == ImageType::WORNG_TYPE) {
        APP_LOGE("input wrong image!");
        return false;
    }
    imageType = type == ImageType::JPEG ? JPEG_FORMAT : WEBP_FORMAT;
    uint32_t errorCode = 0;
    Media::SourceOptions options;
    std::unique_ptr<Media::ImageSource> imageSourcePtr =
        Media::ImageSource::CreateImageSource(fileData.get(), fileSize, options, errorCode);
    // do compress
    Media::DecodeOptions decodeOptions;
    uint32_t pixMapError = 0;
    std::unique_ptr<Media::PixelMap> pixMap = imageSourcePtr->CreatePixelMap(decodeOptions, pixMapError);
    if (pixMap == nullptr || pixMapError != Media::SUCCESS) {
        APP_LOGE("CreatePixelMap failed!");
        return false;
    }
    double ratio = CalculateRatio(fileSize, imageType);
    APP_LOGE("ratio is %{public}f", ratio);
    pixMap->scale(ratio, ratio);
    Media::ImagePacker imagePacker;
    Media::PackOption packOption;
    packOption.format = imageType;
    packOption.quality = QUALITY;
    packOption.numberHint = MUNBER_ONE;
    uint8_t *resultBuffer = reinterpret_cast<uint8_t *>(malloc(BUFFER_SIZE));
    if (resultBuffer == nullptr) {
        APP_LOGE("image packer malloc buffer failed.");
        return 0;
    }
    imagePacker.StartPacking(resultBuffer, BUFFER_SIZE, packOption);
    imagePacker.AddImage(*pixMap);
    imagePacker.FinalizePacking(compressedSize);
    compressedData = std::make_unique<uint8_t[]>(compressedSize);
    APP_LOGD("compressedSize is %{public}d", static_cast<int32_t>(compressedSize));
    uint8_t *result = compressedData.get();
    if (memcpy_s(result, compressedSize, resultBuffer, compressedSize) != EOK) {
        free(resultBuffer);
        APP_LOGE("memcpy_s to compressedData failed!");
        return false;
    }
    free(resultBuffer);
    return true;
}
}
}