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
#ifndef OHOS_GLOBAL_I18N_SIGNATURE_VERIFIER_H
#define OHOS_GLOBAL_I18N_SIGNATURE_VERIFIER_H

#include <string>
#include <vector>

struct rsa_st;
typedef struct rsa_st RSA;

namespace OHOS {
namespace Global {
namespace I18n {
class SignatureVerifier {
public:
    static bool VerifyCertFile(const std::string& certPath, const std::string& verifyPath,
        const std::string& pubkeyPath, const std::string& manifestPath);
    static bool VerifyUpdateFile(const std::string& filePath, const std::string& manifestPath,
        std::vector<std::string>& filesList);
    static const size_t MIN_SIZE;

private:
    static bool VerifyRsa(RSA* pubkey, const std::string& digest, const std::string& sign);
    static std::string CalcFileSha256Digest(const std::string& path);
    static void CalcBase64(uint8_t* input, uint32_t inputLen, std::string& encodedStr);
    static bool CalcFileShaOriginal(const std::string& filePath, unsigned char* hash);
    static bool VerifyFileSign(const std::string& pubkeyPath, const std::string& signPath,
        const std::string& digestPath);
    static std::string GetFileStream(const std::string& filePath);
    static const int32_t BASE64_ENCODE_PACKET_LEN;
    static const int32_t BASE64_ENCODE_LEN_OF_EACH_GROUP_DATA;
    static const size_t HASH_BUFFER_SIZE;
    static const std::string PREFIX_NAME;
};
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif