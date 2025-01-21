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
#ifndef OHOS_GLOBAL_I18N_VERIFY_SIGN_TOOL_H
#define OHOS_GLOBAL_I18N_VERIFY_SIGN_TOOL_H

#include <openssl/rsa.h>
#include <string>
#include <vector>

namespace OHOS {
namespace Global {
namespace I18n {
enum VerifyStatus {
    VERIFY_FAILED = 0,
    VERIFY_START = 1,
    VERIFY_SUCCESS = 2
};

class SignatureVerifier {
public:
    static bool VerifyCertFile(const std::string& certPath,
        const std::string& verifyPath, const std::string& pubkeyPath, const std::string& manifestPath);
    static bool VerifyParamFile(const std::string& filePath,
        const std::string& manifestPath, const std::string& absFilePath);
    static std::string LoadFileVersion(const std::string& versionPath);
    static int CompareVersion(std::string& preVersion, std::string& curVersion);

private:
    static bool VerifyRsa(RSA* pubkey, const std::string& digest, const std::string& sign);
    static std::string CalcFileSha256Digest(const std::string& path);
    static void CalcBase64(uint8_t* input, uint32_t inputLen, std::string& encodedStr);
    static int CalcFileShaOriginal(const std::string& filePath, unsigned char* hash);
    static bool VerifyFileSign(const std::string& pubkeyPath, const std::string& signPath,
        const std::string& digestPath);
    static std::string GetFileStream(const std::string& filePath);
    static const int HASH_BUFFER_SIZE;
    static const int MIN_SIZE;
    static const int VERSION_SIZE;
};

} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif