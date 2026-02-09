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
#ifndef OHOS_GLOBAL_I18N_UPGRADE_UTILS_H
#define OHOS_GLOBAL_I18N_UPGRADE_UTILS_H

#include <filesystem>
#include <string>
#include <vector>

namespace OHOS {
namespace Global {
namespace I18n {
class UpgradeUtils {
public:
    static bool CheckIfUpdateNecessary(const std::string& updateVersionPath, const std::string& localVersionPath);
    static bool CheckFileIntegrity(const std::string& cfgPath, const std::string& pubkeyPath,
        std::vector<std::string>& filesList);
    static bool CopyDataFile(const std::string& srcPath, const std::string& dstPath,
        const std::vector<std::string>& filesList);
    static bool FileDirMount(const std::string& srcDirPath, const std::string& dstDirPath);

private:
    static std::string LoadFileVersion(const std::string& versionPath);
    static bool CompareVersion(const std::string& preVersion, const std::string& curVersion);
    static bool CopySingleFile(const std::filesystem::path& srcFile, const std::filesystem::path& dstFile);
    static bool EnsureDirectoryExists(const std::filesystem::path& dirPath);
    static const std::string CERT_FILE;
    static const std::string VERIFY_FILE;
    static const std::string MANIFEST_FILE;
    static const size_t VERSION_SIZE;
};
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif