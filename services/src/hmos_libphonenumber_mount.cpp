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
#include <filesystem>
#include <string>
#include <sys/mount.h>
#include "i18n_hilog.h"
#include "signature_verifier.h"
#include "utils.h"


namespace OHOS {
namespace Global {
namespace I18n {
namespace {
const std::string CUST_GLOBAL_CARRIER_DIR = "/system/etc/LIBPHONENUMBER/mount_dir/";
const std::string VERSION_FILE = "version.txt";
const std::string CERT_FILE = "CERT.ENC";
const std::string VERIFY_FILE = "CERT.SF";
const std::string MANIFEST_FILE = "MANIFEST.MF";
const std::string SUB_TYPE = "generic";
const std::string CFG_PATH =
    "/data/service/el1/public/update/param_service/install/system/etc/LIBPHONENUMBER/generic/";
const std::string LOCALE_PATH = "/system/etc/LIBPHONENUMBER/generic/";
const std::string SAFE_PATH = "/data/service/el1/public/i18n/libphonenumber/";
const std::string PUBKEY_NAME = "hota_i18n_upgrade_v1.pem";
const std::vector<std::string> DATA_FILES = {"GeocodingInfo", "MetadataInfo"};
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
    } else {
        HILOG_INFO_I18N("Mount: success to mount cotaDir to custDir: opkey(%{public}s)", cotaOpkeyVersionDir.c_str());
    }
    return true;
}

void CopyDataFile()
{
    if (!IsDirExist(SAFE_PATH.c_str())) {
        HILOG_INFO_I18N("CopyDataFile: SAFE_PATH not exist");
        return;
    }

    for (size_t i = 0; i < DATA_FILES.size(); i++) {
        std::string srcPath = CFG_PATH + DATA_FILES[i];
        std::string dstPath = SAFE_PATH + DATA_FILES[i];
        if (!FileExist(srcPath)) {
            HILOG_INFO_I18N("CopyDataFile: %{public}s not exist", DATA_FILES[i].c_str());
            continue;
        }
        if (!FileCopy(srcPath, dstPath)) {
            HILOG_INFO_I18N("CopyDataFile: copy %{public}s failed", DATA_FILES[i].c_str());
        }
    }
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
    std::string pubkeyPath = LOCALE_PATH + PUBKEY_NAME;
    std::string manifestPath = CFG_PATH + MANIFEST_FILE;
    if (!SignatureVerifier::VerifyCertFile(certPath, verifyPath, pubkeyPath, manifestPath)) {
        HILOG_ERROR_I18N("CheckFileIntegrity: VerifyCertFile error");
        return false;
    }

    for (size_t i = 0; i < DATA_FILES.size(); i++) {
        HILOG_ERROR_I18N("CheckFileIntegrity: file to verify (%{public}s)", DATA_FILES[i].c_str());
        std::string filePath = CFG_PATH + DATA_FILES[i];
        if (FileExist(filePath.c_str()) &&
            !SignatureVerifier::VerifyParamFile(DATA_FILES[i], CFG_PATH, manifestPath)) {
            HILOG_ERROR_I18N("CheckFileIntegrity: VerifyParamFile error");
            return false;
        }
    }

    return true;
}

void UpdateLibphonenumber()
{
    if (!CheckIfUpdateNecessary()) {
        HILOG_INFO_I18N("UpdateLibphonenumber: CheckIfUpdateNecessary error, no need to update");
        return;
    }
    if (!CheckFileIntegrity()) {
        HILOG_INFO_I18N("UpdateLibphonenumber: CheckFileIntegrity error, no need to update");
        return;
    }
    
    CopyDataFile();

    if (!Mount()) {
        HILOG_INFO_I18N("UpdateLibphonenumber: mount error");
        return;
    }

    HILOG_INFO_I18N("UpdateLibphonenumber: UpdateLibphonenumber");
}

}
}
}

int main(int argc, char *argv[])
{
    HILOG_INFO_I18N("hmos_libphonenumber_mount: UpdateLibphonenumber start");
    OHOS::Global::I18n::UpdateLibphonenumber();
    return 0;
}