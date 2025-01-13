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
#include <fcntl.h>
#include <filesystem>
#include <fstream>
#include <openssl/pem.h>
#include <openssl/sha.h>
#include <sstream>
#include <unistd.h>
#include "i18n_hilog.h"
#include "signature_verifier.h"
#include "utils.h"

namespace OHOS {
namespace Global {
namespace I18n {
namespace {
    const int32_t BASE64_ENCODE_PACKET_LEN = 3;
    const int32_t BASE64_ENCODE_LEN_OF_EACH_GROUP_DATA = 4;
}

const int SignatureVerifier::HASH_BUFFER_SIZE = 4096;
const int SignatureVerifier::MIN_SIZE = 2;
const int SignatureVerifier::VERSION_SIZE = 4;


std::string SignatureVerifier::LoadFileVersion(const std::string& versionPath)
{
    std::string version;
    if (!FileExist(versionPath.c_str())) {
        return version;
    }
    std::ifstream file(versionPath);
    std::string line;
    std::vector<std::string> strs;
    while (std::getline(file, line)) {
        Split(line, "=", strs);
        if (strs.size() < MIN_SIZE) {
            continue;
        }
        if (strs[0] == "version") {
            version = trim(strs[1]);
            break;
        }
    }
    file.close();
    return version;
}

// compare version
int SignatureVerifier::CompareVersion(std::string& preVersion, std::string& curVersion)
{
    std::vector<std::string> preVersionstr;
    std::vector<std::string> curVersionstr;
    Split(preVersion, ".", preVersionstr);
    Split(curVersion, ".", curVersionstr);
    if (curVersionstr.size() != VERSION_SIZE || preVersionstr.size() != VERSION_SIZE) {
        return -1;
    }
    for (int i = 0; i < VERSION_SIZE; i++) {
        if (atoi(preVersionstr.at(i).c_str()) < atoi(curVersionstr.at(i).c_str())) {
            return 1;
        } else if (atoi(preVersionstr.at(i).c_str()) > atoi(curVersionstr.at(i).c_str())) {
            return -1;
        }
    }
    return 0;
}

// verify certificate file
bool SignatureVerifier::VerifyCertFile(const std::string& certPath, const std::string& verifyPath,
    const std::string& pubkeyPath, const std::string& manifestPath)
{
    if (!VerifyFileSign(pubkeyPath, certPath, verifyPath)) {
        return false;
    }
    std::ifstream file(verifyPath);
    if (!file.good()) {
        return false;
    }
    std::string line;
    std::string sha256Digest;
    std::getline(file, line);
    file.close();
    std::vector<std::string> strs;
    Split(line, ":", strs);
    if (strs.size() < MIN_SIZE) {
        return false;
    }
    sha256Digest = strs[1];
    sha256Digest = trim(sha256Digest);
    // std::string manifestPath = CFG_PATH + MANIFEST_FILE;
    std::string manifestDigest = CalcFileSha256Digest(manifestPath);
    if (sha256Digest == manifestDigest) {
        return true;
    }
    return false;
}

 
// verify param file digest
bool SignatureVerifier::VerifyParamFile(const std::string& fileName, const std::string& filePath,
    const std::string& manifestPath)
{
    std::ifstream file(manifestPath);
    if (!file.good()) {
        return false;
    }
    std::string absFilePath = filePath + fileName;
    if (!CheckTzDataFilePath(absFilePath)) {
        return false;
    }
    std::string sha256Digest;
    std::string line;
    while (std::getline(file, line)) {
        if (line.find("Name: " + fileName) != std::string::npos) {
            std::string nextLine;
            std::getline(file, nextLine);
            std::vector<std::string> strs;
            Split(nextLine, ":", strs);
            if (strs.size() < MIN_SIZE) {
                return false;
            }
            sha256Digest = strs[1];
            sha256Digest = trim(sha256Digest);
            break;
        }
    }
    if (sha256Digest.empty()) {
        return false;
    }
    std::string fileDigest = CalcFileSha256Digest(absFilePath);
    if (fileDigest == sha256Digest) {
        return true;
    }
    return false;
}
 
 
// verify cert file sign
bool SignatureVerifier::VerifyFileSign(const std::string& pubkeyPath, const std::string& signPath,
    const std::string& digestPath)
{
    if (!FileExist(pubkeyPath.c_str())) {
        return false;
    }
 
    if (!FileExist(signPath.c_str())) {
        return false;
    }
 
    if (!FileExist(digestPath.c_str())) {
        return false;
    }
    std::string signStr = GetFileStream(signPath);
    std::string digestStr = GetFileStream(digestPath);
    RSA* pubkey = RSA_new();
    bool verify = false;
    if (pubkey != nullptr && !signStr.empty() && !digestStr.empty()) {
        BIO* bio = BIO_new_file(pubkeyPath.c_str(), "r");
        if (PEM_read_bio_RSA_PUBKEY(bio, &pubkey, nullptr, nullptr) == nullptr) {
            BIO_free(bio);
            return false;
        }
        verify = VerifyRsa(pubkey, digestStr, signStr);
        BIO_free(bio);
    }
    RSA_free(pubkey);
    return verify;
}
 
bool SignatureVerifier::VerifyRsa(RSA* pubkey, const std::string& digest, const std::string& sign)
{
    EVP_PKEY* evpKey = EVP_PKEY_new();
    if (evpKey == nullptr) {
        return false;
    }
    if (EVP_PKEY_set1_RSA(evpKey, pubkey) != 1) {
        return false;
    }
    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    EVP_MD_CTX_init(ctx);
    if (ctx == nullptr) {
        EVP_PKEY_free(evpKey);
        return false;
    }
    if (EVP_VerifyInit_ex(ctx, EVP_sha256(), nullptr) != 1) {
        EVP_PKEY_free(evpKey);
        EVP_MD_CTX_free(ctx);
        return false;
    }
    if (EVP_VerifyUpdate(ctx, digest.c_str(), digest.size()) != 1) {
        EVP_PKEY_free(evpKey);
        EVP_MD_CTX_free(ctx);
        return false;
    }
    char* signArr = const_cast<char*>(sign.c_str());
    if (EVP_VerifyFinal(ctx, reinterpret_cast<unsigned char *>(signArr), sign.size(), evpKey) != 1) {
        EVP_PKEY_free(evpKey);
        EVP_MD_CTX_free(ctx);
        return false;
    }
    EVP_PKEY_free(evpKey);
    EVP_MD_CTX_free(ctx);
    return true;
}
 
std::string SignatureVerifier::CalcFileSha256Digest(const std::string& path)
{
    unsigned char res[SHA256_DIGEST_LENGTH] = {0};
    CalcFileShaOriginal(path, res);
    std::string dist;
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
 
int SignatureVerifier::CalcFileShaOriginal(const std::string& filePath, unsigned char* hash)
{
    if (filePath.empty() || hash == nullptr || !IsLegalPath(filePath)) {
        return -1;
    }
    FILE* fp = fopen(filePath.c_str(), "rb");
    if (fp == nullptr) {
        return -1;
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
        return -1;
    }
    return 0;
}
 
// load file content
std::string SignatureVerifier::GetFileStream(const std::string& filePath)
{
    if (filePath.length() > PATH_MAX) {
        return "";
    }
    char* resolvedPath = new char[PATH_MAX + 1];
    if (realpath(filePath.c_str(), resolvedPath) == nullptr) {
        delete[] resolvedPath;
        return "";
    }
    const std::string newFilePath = resolvedPath;
    std::ifstream file(newFilePath, std::ios::in | std::ios::binary);
    if (!file.good()) {
        delete[] resolvedPath;
        return "";
    }
    std::stringstream inFile;
    inFile << file.rdbuf();
    delete[] resolvedPath;
    return inFile.str();
}

}
}
}