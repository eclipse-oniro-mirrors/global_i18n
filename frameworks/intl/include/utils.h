/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#ifndef OHOS_GLOBAL_I18N_UTILS_H
#define OHOS_GLOBAL_I18N_UTILS_H

#include <string>
#include <vector>
#include <unordered_set>
#include <unicode/locid.h>
#include "set"
#include "i18n_types.h"

namespace OHOS {
namespace Global {
namespace I18n {
static const uint32_t BYTE_ARRAY_OFFSET_FIRST = 24;
static const uint32_t BYTE_ARRAY_OFFSET_SECOND = 16;
static const uint32_t BYTE_ARRAY_OFFSET_THIRD = 8;
static const uint32_t BYTE_ARRAY_INDEX_THIRD = 3;
static std::set<std::string> availableIDs;

void Split(const std::string &src, const std::string &sep, std::vector<std::string> &dest);
void Merge(const std::vector<std::string>& src, const std::string& sep, std::string& dest);
std::string ReadSystemParameter(const char *paramKey, const int paramLength);
int32_t ConvertString2Int(const std::string &numberStr, int32_t& status);
bool IsValidLocaleTag(icu::Locale &locale);
void GetAllValidLocalesTag(std::unordered_set<std::string>& allValidLocalesLanguageTag);
bool CheckTzDataFilePath(const std::string &filePath);
std::string StrReplaceAll(const std::string& str,
    const std::string& target, const std::string& replace);
std::string GetISO3Language(const std::string& language);
std::string GetISO3Country(const std::string& country);
std::string trim(std::string &s);
bool FileExist(const std::string& path);
bool FileCopy(const std::string& srcPath, const std::string& dstPath);
bool IsLegalPath(const std::string& path);
void Split(const std::string &src, const std::string &sep, std::vector<std::string> &dest);
bool IsDirExist(const char *path);
bool GetPseudoLocalizationEnforce();
std::string PseudoLocalizationProcessor(const std::string &input);
std::string PseudoLocalizationProcessor(const std::string &input, bool ifEnforce);
bool CheckSystemPermission();
size_t ConvertBytesToSizeT(const char *byteArray);
std::set<std::string> GetTimeZoneAvailableIDs(I18nErrorCode &errorCode);
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif