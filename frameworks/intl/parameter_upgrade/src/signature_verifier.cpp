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

#include <climits>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <filesystem>
#include <fstream>
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <openssl/sha.h>
#include <sstream>
#include <unistd.h>
#include "i18n_hilog.h"
#include "securec.h"
#include "utils.h"
#include "signature_verifier.h"

namespace OHOS {
namespace Global {
namespace I18n {
const int32_t SignatureVerifier::BASE64_ENCODE_PACKET_LEN = 3;
const int32_t SignatureVerifier::BASE64_ENCODE_LEN_OF_EACH_GROUP_DATA = 4;
const size_t SignatureVerifier::HASH_BUFFER_SIZE = 4096;
const std::string SignatureVerifier::PREFIX_NAME = "Name: ";
const size_t SignatureVerifier::MIN_SIZE = 2;

// verify certificate file
bool SignatureVerifier::VerifyCertFile(const std::string& certPath, const std::string& verifyPath,
    const std::string& pubkeyPath, const std::string& manifestPath)
{
    if (!VerifyFileSign(pubkeyPath, certPath, verifyPath)) {
        HILOG_ERROR_I18N("SignatureVerifier::VerifyCertFile: Verify file sign failed.");
        return false;
    }
    std::ifstream file(verifyPath);
    if (!file.is_open()) {
        HILOG_ERROR_I18N("SignatureVerifier::VerifyCertFile: Verify file open failed.");
        return false;
    }
    std::string line;
    std::getline(file, line);
    file.close();
    std::vector<std::string> strs;
    Split(line, ":", strs);
    if (strs.size() < MIN_SIZE) {
        HILOG_ERROR_I18N("SignatureVerifier::VerifyCertFile: Parse digest failed.");
        return false;
    }
    std::string sha256Digest = strs[1];
    sha256Digest = trim(sha256Digest);
    std::string manifestDigest = CalcFileSha256Digest(manifestPath);
    if (sha256Digest.compare(manifestDigest) != 0) {
        HILOG_ERROR_I18N("SignatureVerifier::VerifyCertFile: Manifest digest verify failed.");
        return false;
    }
    return true;
}

// verify cert file sign
bool SignatureVerifier::VerifyFileSign(const std::string& pubkeyPath, const std::string& signPath,
    const std::string& digestPath)
{
    std::string validPubkeyPath = GetAbsoluteFilePath(pubkeyPath);
    if (validPubkeyPath.empty()) {
        HILOG_ERROR_I18N("SignatureVerifier::VerifyFileSign: Check pubkey file failed.");
        return false;
    }
    std::string validSignPath = GetAbsoluteFilePath(signPath);
    if (validSignPath.empty()) {
        HILOG_ERROR_I18N("SignatureVerifier::VerifyFileSign: Check sign file failed.");
        return false;
    }
    std::string validDigestPath = GetAbsoluteFilePath(digestPath);
    if (validDigestPath.empty()) {
        HILOG_ERROR_I18N("SignatureVerifier::VerifyFileSign: Check digest file failed.");
        return false;
    }

    std::string signStr = GetFileStream(validSignPath);
    std::string digestStr = GetFileStream(validDigestPath);
    if (signStr.empty() || digestStr.empty()) {
        HILOG_ERROR_I18N("SignatureVerifier::VerifyFileSign: sign (%{public}s), digest (%{public}s).",
            signStr.c_str(), digestStr.c_str());
        return false;
    }
    RSA* pubkey = RSA_new();
    if (pubkey == nullptr) {
        HILOG_ERROR_I18N("SignatureVerifier::VerifyFileSign: Create RSA failed.");
        return false;
    }
    BIO* bio = BIO_new_file(validPubkeyPath.c_str(), "r");
    if (bio == nullptr) {
        RSA_free(pubkey);
        HILOG_ERROR_I18N("SignatureVerifier::VerifyFileSign: Create BIO failed.");
        return false;
    }
    if (PEM_read_bio_RSA_PUBKEY(bio, &pubkey, nullptr, nullptr) == nullptr) {
        RSA_free(pubkey);
        BIO_free(bio);
        HILOG_ERROR_I18N("SignatureVerifier::VerifyFileSign: PEM read bio RSA pubkey failed.");
        return false;
    }
    bool verify = VerifyRsa(pubkey, digestStr, signStr);
    RSA_free(pubkey);
    BIO_free(bio);
    return verify;
}

bool SignatureVerifier::VerifyUpdateFile(const std::string& filePath, const std::string& manifestPath,
    std::vector<std::string>& filesList)
{
    char* result = realpath(manifestPath.c_str(), nullptr);
    if (result == nullptr) {
        HILOG_ERROR_I18N("SignatureVerifier::VerifyUpdateFile: realpath error: %{public}s.", strerror(errno));
        return false;
    }
    std::ifstream file(result);
    free(result);
    result = nullptr;
    if (!file.is_open()) {
        HILOG_ERROR_I18N("SignatureVerifier::VerifyUpdateFile: Open manifest failed.");
        return false;
    }
    std::string line;
    while (std::getline(file, line)) {
        if (line.find(PREFIX_NAME) == std::string::npos) {
            continue;
        }
        std::string nextLine;
        std::getline(file, nextLine);
        std::vector<std::string> strs;
        Split(nextLine, ":", strs);
        if (strs.size() < MIN_SIZE) {
            HILOG_ERROR_I18N("SignatureVerifier::VerifyUpdateFile: Parse digest failed.");
            return false;
        }
        std::string sha256Digest = strs[1];
        sha256Digest = trim(sha256Digest);
        if (sha256Digest.empty()) {
            HILOG_ERROR_I18N("SignatureVerifier::VerifyUpdateFile: Digest is empty.");
            return false;
        }
        std::string fileName = line.substr(PREFIX_NAME.size());
        filesList.push_back(fileName);
        std::string absFilePath = filePath + trim(fileName);
        absFilePath = GetAbsoluteFilePath(absFilePath);
        if (absFilePath.empty()) {
            HILOG_ERROR_I18N("VerifyUpdateFile: File %{public}s is not exist.", fileName.c_str());
            return false;
        }
        std::string fileDigest = CalcFileSha256Digest(absFilePath);
        if (fileDigest.compare(sha256Digest) != 0) {
            HILOG_ERROR_I18N("VerifyUpdateFile: verify %{public}s failed.", fileDigest.c_str());
            return false;
        }
    }
    return true;
}

bool SignatureVerifier::VerifyRsa(RSA* pubkey, const std::string& digest, const std::string& sign)
{
    EVP_PKEY* evpKey = EVP_PKEY_new();
    if (evpKey == nullptr) {
        HILOG_ERROR_I18N("SignatureVerifier::VerifyRsa: Create EVP_PKEY failed.");
        return false;
    }
    if (EVP_PKEY_set1_RSA(evpKey, pubkey) != 1) {
        EVP_PKEY_free(evpKey);
        HILOG_ERROR_I18N("SignatureVerifier::VerifyRsa: EVP_PKEY set pubkey failed.");
        return false;
    }
    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    if (ctx == nullptr) {
        EVP_PKEY_free(evpKey);
        HILOG_ERROR_I18N("SignatureVerifier::VerifyRsa: Create EVP_MD_CTX failed.");
        return false;
    }
    EVP_MD_CTX_init(ctx);
    if (EVP_VerifyInit_ex(ctx, EVP_sha256(), nullptr) != 1) {
        EVP_PKEY_free(evpKey);
        EVP_MD_CTX_free(ctx);
        HILOG_ERROR_I18N("SignatureVerifier::VerifyRsa: Verify init failed.");
        return false;
    }
    if (EVP_VerifyUpdate(ctx, digest.c_str(), digest.size()) != 1) {
        EVP_PKEY_free(evpKey);
        EVP_MD_CTX_free(ctx);
        HILOG_ERROR_I18N("SignatureVerifier::VerifyRsa: Verify update failed.");
        return false;
    }
    char* signArr = const_cast<char*>(sign.c_str());
    if (EVP_VerifyFinal(ctx, reinterpret_cast<unsigned char *>(signArr), sign.size(), evpKey) != 1) {
        EVP_PKEY_free(evpKey);
        EVP_MD_CTX_free(ctx);
        HILOG_ERROR_I18N("SignatureVerifier::VerifyRsa: Verify final failed.");
        return false;
    }
    EVP_PKEY_free(evpKey);
    EVP_MD_CTX_free(ctx);
    return true;
}

std::string SignatureVerifier::CalcFileSha256Digest(const std::string& path)
{
    std::string dist;
    unsigned char res[SHA256_DIGEST_LENGTH] = {0};
    if (!CalcFileShaOriginal(path, res)) {
        HILOG_ERROR_I18N("SignatureVerifier::CalcFileSha256Digest: Calc file sha original failed.");
        return dist;
    }
    CalcBase64(res, SHA256_DIGEST_LENGTH, dist);
    return dist;
}

void SignatureVerifier::CalcBase64(uint8_t* input, uint32_t inputLen, std::string& encodedStr)
{
    size_t base64Len = static_cast<size_t>(ceil(static_cast<long double>(inputLen) / BASE64_ENCODE_PACKET_LEN) *
        BASE64_ENCODE_LEN_OF_EACH_GROUP_DATA + 1);
    std::unique_ptr<unsigned char[]> base64Str = std::make_unique<unsigned char[]>(base64Len);
    int encodeLen = EVP_EncodeBlock(reinterpret_cast<uint8_t*>(base64Str.get()), input, inputLen);
    size_t outLen = static_cast<size_t>(encodeLen);
    encodedStr = std::string(reinterpret_cast<char*>(base64Str.get()), outLen);
}

bool SignatureVerifier::CalcFileShaOriginal(const std::string& filePath, unsigned char* hash)
{
    if (filePath.empty() || hash == nullptr || !IsLegalPath(filePath)) {
        HILOG_ERROR_I18N("SignatureVerifier::CalcFileShaOriginal: Invalid input parameter.");
        return false;
    }
    char* resolvedPath = realpath(filePath.c_str(), nullptr);
    if (resolvedPath == nullptr) {
        HILOG_ERROR_I18N("SignatureVerifier::CalcFileShaOriginal: Input parameter filePath invalid.");
        return false;
    }
    FILE* fp = fopen(resolvedPath, "rb");
    free(resolvedPath);
    if (fp == nullptr) {
        HILOG_ERROR_I18N("SignatureVerifier::CalcFileShaOriginal: Close file failed.");
        return false;
    }
    size_t n;
    char buffer[HASH_BUFFER_SIZE] = {0};
    SHA256_CTX ctx;
    SHA256_Init(&ctx);
    while ((n = fread(buffer, 1, sizeof(buffer), fp))) {
        SHA256_Update(&ctx, reinterpret_cast<unsigned char*>(buffer), n);
    }
    SHA256_Final(hash, &ctx);
    if (fclose(fp) == -1) {
        HILOG_ERROR_I18N("SignatureVerifier::CalcFileShaOriginal: Close file failed.");
        return false;
    }
    return true;
}

// load file content
std::string SignatureVerifier::GetFileStream(const std::string& filePath)
{
    if (filePath.length() > PATH_MAX) {
        HILOG_ERROR_I18N("SignatureVerifier::GetFileStream: File path length is too large.");
        return "";
    }
    char* result = realpath(filePath.c_str(), nullptr);
    if (result == nullptr) {
        HILOG_ERROR_I18N("SignatureVerifier::GetFileStream: Get real path failed.");
        return "";
    }
    std::ifstream file(result, std::ios::in | std::ios::binary);
    free(result);
    result = nullptr;
    if (!file.is_open()) {
        HILOG_ERROR_I18N("SignatureVerifier::GetFileStream: Open file failed.");
        return "";
    }
    std::stringstream inFile;
    inFile << file.rdbuf();
    return inFile.str();
}
} // namespace I18n
} // namespace Global
} // namespace OHOS