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
#include <fstream>
#include <sys/mount.h>
#include "i18n_hilog.h"
#include "signature_verifier.h"
#include "utils.h"
#include "upgrade_utils.h"

namespace OHOS {
namespace Global {
namespace I18n {
const std::string UpgradeUtils::CERT_FILE = "CERT.ENC";
const std::string UpgradeUtils::VERIFY_FILE = "CERT.SF";
const std::string UpgradeUtils::MANIFEST_FILE = "MANIFEST.MF";
const size_t UpgradeUtils::VERSION_SIZE = 4;

bool UpgradeUtils::CheckIfUpdateNecessary(const std::string& updateVersionPath, const std::string& localVersionPath)
{
    std::string versionUpdate = LoadFileVersion(updateVersionPath);
    std::string versionLocal = LoadFileVersion(localVersionPath);
    HILOG_INFO_I18N("UpgradeUtils::CheckIfUpdateNecessary: versionUpdate(%{public}s) versionLocal(%{public}s).",
        versionUpdate.c_str(), versionLocal.c_str());

    if (versionLocal.empty() || versionUpdate.empty()) {
        return false;
    }
    return CompareVersion(versionLocal, versionUpdate);
}

bool UpgradeUtils::CheckFileIntegrity(const std::string& cfgPath, const std::string& pubkeyPath,
    std::vector<std::string>& filesList)
{
    std::string certPath = cfgPath + CERT_FILE;
    std::string verifyPath = cfgPath + VERIFY_FILE;
    std::string manifestPath = cfgPath + MANIFEST_FILE;
    if (!SignatureVerifier::VerifyCertFile(certPath, verifyPath, pubkeyPath, manifestPath)) {
        HILOG_ERROR_I18N("UpgradeUtils::CheckFileIntegrity: VerifyCertFile error.");
        return false;
    }
    return SignatureVerifier::VerifyUpdateFile(cfgPath, manifestPath, filesList);
}

bool UpgradeUtils::CopyDataFile(const std::string& srcPath, const std::string& dstPath,
    const std::vector<std::string>& filesList)
{
    if (!IsDirExist(dstPath.c_str())) {
        HILOG_ERROR_I18N("UpgradeUtils::CopyDataFile: dstPath is not exist.");
        return false;
    }

    std::filesystem::path dstPathDir(dstPath);
    ClearDir(dstPathDir);

    bool copySuccess = true;
    for (const auto& file : filesList) {
        std::filesystem::path srcFile = std::filesystem::path(srcPath) / file;
        std::filesystem::path dstFile = std::filesystem::path(dstPath) / file;
        if (!CopySingleFile(srcFile, dstFile)) {
            HILOG_ERROR_I18N("UpgradeUtils::CopyDataFile: Copy failed for %{public}s.", file.c_str());
            copySuccess = false;
            break;
        }
    }

    if (!copySuccess) {
        HILOG_ERROR_I18N("UpgradeUtils::CopyDataFile: CopyDataFile error, clearing the safe directory...");
        ClearDir(dstPathDir);
    }

    return copySuccess;
}

bool UpgradeUtils::FileDirMount(const std::string& srcDirPath, const std::string& dstDirPath)
{
    if (!IsDirExist(srcDirPath.c_str()) || !IsDirExist(dstDirPath.c_str())) {
        HILOG_ERROR_I18N("UpgradeUtils::FileDirMount: srcDirPath or dstDirPath not exist.");
        return false;
    }

    std::string cotaOpkeyVersionDir = srcDirPath;
    std::string custOpkeyVersionDir = dstDirPath;

    if (mount(cotaOpkeyVersionDir.c_str(), custOpkeyVersionDir.c_str(), nullptr, MS_BIND, nullptr) != 0) {
        HILOG_ERROR_I18N("UpgradeUtils::FileDirMount: Fail to mount, errno %{public}s.", strerror(errno));
        return false;
    }

    if (mount(nullptr, custOpkeyVersionDir.c_str(), nullptr, MS_REMOUNT | MS_BIND | MS_RDONLY, nullptr) != 0) {
        HILOG_ERROR_I18N("UpgradeUtils::FileDirMount: Fail to mount read only, errno %{public}s.", strerror(errno));
        if (umount(custOpkeyVersionDir.c_str()) != 0) {
            HILOG_ERROR_I18N("UpgradeUtils::FileDirMount: Error during unmount, errno %{public}s.", strerror(errno));
        }
        return false;
    }

    HILOG_INFO_I18N("UpgradeUtils::FileDirMount: Succeeded in mounting the file path.");
    return true;
}

std::string UpgradeUtils::LoadFileVersion(const std::string& versionPath)
{
    std::string version;
    char* result = realpath(versionPath.c_str(), nullptr);
    if (result == nullptr) {
        HILOG_ERROR_I18N("UpgradeUtils::LoadFileVersion: realpath error: %{public}s.", strerror(errno));
        return version;
    }
    std::ifstream file(result);
    free(result);
    result = nullptr;
    if (!file.is_open()) {
        HILOG_ERROR_I18N("UpgradeUtils::LoadFileVersion: open version file failed.");
        return version;
    }
    std::string line;
    std::vector<std::string> strs;
    while (std::getline(file, line)) {
        Split(line, "=", strs);
        if (strs.size() < SignatureVerifier::MIN_SIZE) {
            continue;
        }
        if (strs[0] == "version") {
            version = trim(strs[1]);
            break;
        }
    }
    return version;
}

// compare version
bool UpgradeUtils::CompareVersion(const std::string& preVersion, const std::string& curVersion)
{
    std::vector<std::string> preVersionStr;
    std::vector<std::string> curVersionStr;
    Split(preVersion, ".", preVersionStr);
    Split(curVersion, ".", curVersionStr);
    if (curVersionStr.size() != VERSION_SIZE || preVersionStr.size() != VERSION_SIZE) {
        HILOG_ERROR_I18N("UpgradeUtils::CompareVersion: Parse version failed.");
        return false;
    }
    for (size_t i = 0; i < VERSION_SIZE; i++) {
        int32_t status = 0;
        int32_t preVersionNum = ConvertString2Int(preVersionStr.at(i), status);
        if (status != 0) {
            HILOG_ERROR_I18N("UpgradeUtils::CompareVersion: preVersionStr convert to int failed.");
            return false;
        }
        int32_t curVersionNum = ConvertString2Int(curVersionStr.at(i), status);
        if (status != 0) {
            HILOG_ERROR_I18N("UpgradeUtils::CompareVersion: curVersionStr convert to int failed.");
            return false;
        }
        if (preVersionNum < curVersionNum) {
            return true;
        } else if (preVersionNum > curVersionNum) {
            return false;
        }
    }
    return false;
}

bool UpgradeUtils::CopySingleFile(const std::filesystem::path& srcFile, const std::filesystem::path& dstFile)
{
    if (!FileExist(srcFile.c_str())) {
        HILOG_ERROR_I18N("UpgradeUtils::copySingleFile: Source file does not exist.");
        return false;
    }

    if (!EnsureDirectoryExists(dstFile.parent_path())) {
        HILOG_ERROR_I18N("UpgradeUtils::copySingleFile: Target file dir not exist.");
        return false;
    }
    if (!FileCopy(srcFile.c_str(), dstFile.c_str())) {
        HILOG_ERROR_I18N("UpgradeUtils::copySingleFile: Failed to copy file.");
        return false;
    }
    return true;
}

bool UpgradeUtils::EnsureDirectoryExists(const std::filesystem::path& dirPath)
{
    std::error_code errCode;
    if (!std::filesystem::exists(dirPath)) {
        std::filesystem::create_directories(dirPath, errCode);
        if (errCode) {
            HILOG_ERROR_I18N("UpgradeUtils::EnsureDirectoryExists: failed to create directory, Error %{public}s",
                errCode.message().c_str());
            return false;
        }
    }
    return true;
}
}
}
}