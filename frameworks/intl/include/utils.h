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

#include <filesystem>
#include <regex>
#include <string>
#include <vector>
#include <unordered_set>
#include <unicode/locid.h>
#include "set"
#include "i18n_types.h"

namespace OHOS {
namespace Global {
namespace I18n {
static std::set<std::string> availableIDs;

void Split(const std::string &src, const std::string &sep, std::vector<std::string> &dest);
void Split(const std::string& src, const std::string& sep, std::unordered_set<std::string>& dest);
void Merge(const std::vector<std::string>& src, const std::string& sep, std::string& dest);
std::string ReadSystemParameter(const char *paramKey, const int paramLength);
int32_t ConvertString2Int(const std::string &numberStr, int32_t& status);
bool IsValidLocaleTag(icu::Locale &locale);
void GetAllValidLocalesTag(std::unordered_set<std::string>& allValidLocalesLanguageTag);
bool CheckFilePath(const std::string &filePath);
std::string StrReplaceAll(const std::string& str,
    const std::string& target, const std::string& replace);
std::string GetISO3Language(const std::string& language);
std::string GetISO3Country(const std::string& country);
std::string trim(std::string &s);
bool FileExist(const std::string& path);
bool FileCopy(const std::string& srcPath, const std::string& dstPath);
bool IsLegalPath(const std::string& path);
bool IsDirExist(const char *path);
void ClearDir(const std::filesystem::path& dirPath);
bool GetPseudoLocalizationEnforce();
std::string PseudoLocalizationProcessor(const std::string &input);
std::string PseudoLocalizationProcessor(const std::string &input, bool ifEnforce);
bool CheckSystemPermission();
std::set<std::string> GetTimeZoneAvailableIDs(I18nErrorCode &errorCode);
bool RegexSearchNoExcept(const std::string& str, std::smatch& match, const std::regex& regex);
std::string LocaleEncode(const std::string& locale);
bool Eq(double a, double b);
bool Geq(double a, double b);
bool Leq(double a, double b);
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif