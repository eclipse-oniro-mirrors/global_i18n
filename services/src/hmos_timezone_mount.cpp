/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except", "in compliance with the License.
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
#include <climits>
#include <filesystem>
#include <fstream>
#include <string>
#include <sys/mount.h>
#include "i18n_hilog.h"
#include "parameter.h"
#include "signature_verifier.h"
#include "utils.h"


namespace OHOS {
namespace Global {
namespace I18n {

const std::string CUST_GLOBAL_CARRIER_DIR = "/system/etc/tzdata_distro/";
const std::string VERSION_FILE = "version.txt";
const std::string CERT_FILE = "CERT.ENC";
const std::string VERIFY_FILE = "CERT.SF";
const std::string MANIFEST_FILE = "MANIFEST.MF";
const std::string SUB_TYPE = "generic";
const std::string CFG_PATH =
    "/data/service/el1/public/update/param_service/install/system/etc/TIMEZONE/generic/current/";
const std::string LOCALE_PATH = "/system/etc/TIMEZONE/generic/current/";
const std::string SAFE_PATH = "/data/service/el1/public/i18n/timezone/";
const std::string PUBKEY_PATH = "/system/etc/LIBPHONENUMBER/generic/";
const std::string PUBKEY_NAME = "hota_i18n_upgrade_v1.pem";
const std::string COTA_PARAM_TIMEZONE_KEY = "persist.global.tz_override";
const int FILE_NAME_INDEX = 6;

std::vector<std::string> g_dataFiles = {};

bool Init()
{
    std::string manifestPath = CFG_PATH + MANIFEST_FILE;
    std::unique_ptr<char[]> resolvedPath = std::make_unique<char[]>(PATH_MAX);
    if (realpath(manifestPath.c_str(), resolvedPath.get()) == nullptr) {
        HILOG_ERROR_I18N("MANIFEST_FILE file path isn't exists.");
        return false;
    }
    std::ifstream file(resolvedPath.get());
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            if (line.find("Name: ") == 0) {
                g_dataFiles.push_back(line.substr(FILE_NAME_INDEX));
            }
        }
        file.close();
        return true;
    } else {
        return false;
    }
}

bool Mount()
{
    if (!IsDirExist(CUST_GLOBAL_CARRIER_DIR.c_str()) ||
        !IsDirExist(SAFE_PATH.c_str())) {
        HILOG_ERROR_I18N("Mount: CUST_GLOBAL_CARRIER_DIR or CFG_PATH not exist");
        return false;
    }

    std::string cotaOpkeyVersionDir = SAFE_PATH;
    std::string custOpkeyVersionDir = CUST_GLOBAL_CARRIER_DIR;

    if (mount(cotaOpkeyVersionDir.c_str(), custOpkeyVersionDir.c_str(), nullptr, MS_BIND, nullptr) != 0) {
        HILOG_ERROR_I18N("Mount: fail to mount: opkey(%{public}s) errno(%{public}s)",
            cotaOpkeyVersionDir.c_str(), strerror(errno));
        return false;
    }

    if (mount(nullptr, custOpkeyVersionDir.c_str(), nullptr, MS_REMOUNT | MS_BIND | MS_RDONLY, nullptr) != 0) {
        HILOG_ERROR_I18N("Mount: fail to mount read only: opkey(%{public}s) errno(%{public}s)",
            cotaOpkeyVersionDir.c_str(), strerror(errno));
        if (umount(custOpkeyVersionDir.c_str()) != 0) {
            HILOG_ERROR_I18N("Error during unmount: %{public}s\n", strerror(errno));
        }
        return false;
    }

    HILOG_INFO_I18N("Mount: success ro bind cotaDir to custDir: opkey(%{public}s)", cotaOpkeyVersionDir.c_str());
    return true;
}

void ensureDirectoryExists(const std::filesystem::path& dir_path)
{
    std::error_code ec;
    if (!std::filesystem::exists(dir_path)) {
        HILOG_ERROR_I18N("Directory does not exist: %{public}s", dir_path.c_str());
        std::filesystem::create_directories(dir_path, ec);
        if (ec) {
            HILOG_ERROR_I18N("Failed to create directory: %{public}s, Error: %{public}s", dir_path.c_str(),
                ec.message().c_str());
        }
    }
}

void clearPath(const std::filesystem::path& dir_path)
{
    std::error_code ec;
    for (const auto& entry : std::filesystem::directory_iterator(dir_path)) {
        std::filesystem::remove_all(entry.path(), ec);
        if (ec) {
            HILOG_ERROR_I18N("clearPath: Error removing file: %{public}s, Error: %{public}s", entry.path().c_str(),
                ec.message().c_str());
        }
    }
}

bool copySingleFile(const std::filesystem::path& src_path, const std::filesystem::path& dst_path)
{
    if (!FileExist(src_path.string())) {
        HILOG_ERROR_I18N("copySingleFile: Source file does not exist: %{public}s", src_path.c_str());
        return false;
    }

    ensureDirectoryExists(dst_path.parent_path());
    HILOG_INFO_I18N("copySingleFile: copy file: %{public}s,  %{public}s", src_path.c_str(),  dst_path.c_str());
    if (!FileCopy(src_path.string(), dst_path.string())) {
        HILOG_ERROR_I18N("copySingleFile: Failed to copy file: %{public}s", src_path.c_str());
        return false;
    }

    return true;
}

bool CopyDataFile()
{
    HILOG_INFO_I18N("TimeZone CopyDataFile start");
    if (!IsDirExist(SAFE_PATH.c_str())) {
        HILOG_ERROR_I18N("TimeZone CopyDataFile: SAFE_PATH does not exist");
        return false;
    }

    std::filesystem::path safe_dir(SAFE_PATH);
    clearPath(safe_dir);

    bool copySuccess = true;
    for (const auto& file : g_dataFiles) {
        std::filesystem::path src_path = std::filesystem::path(CFG_PATH) / file;
        std::filesystem::path dst_path = std::filesystem::path(SAFE_PATH) / file;
        if (!copySingleFile(src_path, dst_path)) {
            HILOG_ERROR_I18N("CopyDataFile: Copy failed for %{public}s", file.c_str());
            copySuccess = false;
            break;
        }
    }

    if (!copySuccess) {
        HILOG_ERROR_I18N("CopyDataFile error, clearing the safe directory...");
        clearPath(safe_dir);
    }

    return copySuccess;
}

bool CheckIfUpdateNecessary()
{
    std::string versionUpdate = SignatureVerifier::LoadFileVersion(CFG_PATH + VERSION_FILE);
    std::string versionLocale = SignatureVerifier::LoadFileVersion(LOCALE_PATH + VERSION_FILE);
    HILOG_INFO_I18N("CheckIfUpdateNecessary: Verify: versionUpdate(%{public}s) versionLocale(%{public}s)",
        versionUpdate.c_str(), versionLocale.c_str());

    if (versionLocale.length() == 0 || versionUpdate.length() == 0) {
        return false;
    }
    if (SignatureVerifier::CompareVersion(versionLocale, versionUpdate) <= 0) {
        return false;
    }
    return true;
}

bool CheckFileIntegrity()
{
    std::string certPath = CFG_PATH + CERT_FILE;
    std::string verifyPath = CFG_PATH + VERIFY_FILE;
    std::string pubkeyPath = PUBKEY_PATH + PUBKEY_NAME;
    std::string manifestPath = CFG_PATH + MANIFEST_FILE;
    if (!SignatureVerifier::VerifyCertFile(certPath, verifyPath, pubkeyPath, manifestPath)) {
        HILOG_ERROR_I18N("CheckFileIntegrity: VerifyCertFile error");
        return false;
    }

    for (unsigned long i = 0; i < g_dataFiles.size(); i++) {
        HILOG_ERROR_I18N("CheckFileIntegrity: file to verify (%{public}s)", g_dataFiles[i].c_str());
        std::string filePath = CFG_PATH + g_dataFiles[i];
        if (!FileExist(filePath.c_str())) {
            HILOG_ERROR_I18N("CheckFileIntegrity: file not exist (%{public}s)", g_dataFiles[i].c_str());
            return false;
        }
        if (!SignatureVerifier::VerifyParamFile(g_dataFiles[i], CFG_PATH, manifestPath)) {
            HILOG_ERROR_I18N("CheckFileIntegrity: VerifyParamFile error");
            return false;
        }
    }

    return true;
}

void UpdateTimeZone()
{
    if (!Init()) {
        SetParameter(COTA_PARAM_TIMEZONE_KEY.c_str(), "false");
        HILOG_INFO_I18N("UpdateTimeZone: init error");
        return;
    }
    if (!Mount()) {
        SetParameter(COTA_PARAM_TIMEZONE_KEY.c_str(), "false");
        HILOG_INFO_I18N("UpdateTimeZone: mount error");
        return;
    }
    if (!CheckIfUpdateNecessary()) {
        SetParameter(COTA_PARAM_TIMEZONE_KEY.c_str(), "false");
        HILOG_INFO_I18N("UpdateTimeZone: CheckIfUpdateNecessary error, no need to update");
        return;
    }
    if (!CheckFileIntegrity()) {
        SetParameter(COTA_PARAM_TIMEZONE_KEY.c_str(), "false");
        HILOG_INFO_I18N("UpdateTimeZone: CheckFileIntegrity error, no need to update");
        return;
    }
    if (!CopyDataFile()) {
        SetParameter(COTA_PARAM_TIMEZONE_KEY.c_str(), "false");
        HILOG_INFO_I18N("UpdateTimeZone: CopyDataFile error");
        return;
    }

    SetParameter(COTA_PARAM_TIMEZONE_KEY.c_str(), "true");
    HILOG_INFO_I18N("UpdateTimeZone: UpdateTimeZone");
}

}
}
}

int main(int argc, char *argv[])
{
    HILOG_INFO_I18N("hmos_timezone_mount: UpdateTimeZone start");
    OHOS::Global::I18n::UpdateTimeZone();
    return 0;
}