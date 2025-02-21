/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <dirent.h>
#include <mutex>
#include <stdexcept>
#include <string>
#include <sys/stat.h>
#include <vector>
#include "accesstoken_kit.h"
#include "i18n_hilog.h"
#include "ipc_skeleton.h"
#include "locale_config.h"
#include "parameter.h"
#include "tokenid_kit.h"
#include "unicode/localebuilder.h"
#include "utils.h"

namespace OHOS {
namespace Global {
namespace I18n {
using namespace std;
static const std::string PSEUDO_LOCALE_TAG = "en-XA";
static const std::string PSEUDO_START_TAG = "{";
static const std::string PSEUDO_END_TAG = "}";
static const char *TZDATA_PATH = "/system/etc/zoneinfo/tzdata";
static const char *DISTRO_TZDATA_PATH = "/system/etc/tzdata_distro/hos/tzdata";
static std::mutex validLocaleMutex;

void Split(const string &src, const string &sep, vector<string> &dest)
{
    if (src == "") {
        return;
    }
    string::size_type begin = 0;
    string::size_type end = src.find(sep);
    while (end != string::npos) {
        dest.push_back(src.substr(begin, end - begin));
        begin = end + sep.size();
        end = src.find(sep, begin);
    }
    if (begin != src.size()) {
        dest.push_back(src.substr(begin));
    }
}

void Merge(const std::vector<std::string>& src, const std::string& sep, std::string& dest)
{
    if (src.size() == 0) {
        dest = "";
        return;
    }
    dest = src[0];
    for (size_t i = 1; i < src.size(); ++i) {
        dest += sep;
        dest += src[i];
    }
}

std::string ReadSystemParameter(const char *paramKey, const int paramLength)
{
    char param[paramLength];
    int status = GetParameter(paramKey, "", param, paramLength);
    if (status > 0) {
        return param;
    }
    return "";
}

int32_t ConvertString2Int(const string &numberStr, int32_t& status)
{
    if (!numberStr.empty() && std::all_of(numberStr.begin(), numberStr.end(), ::isdigit)) {
        try {
            return std::stoi(numberStr);
        } catch (const std::invalid_argument &except) {
            status = -1;
            return -1;
        } catch (const std::out_of_range &except) {
            status = -1;
            return -1;
        } catch (...) {
            status = -1;
            HILOG_ERROR_I18N("ConvertString2Int: unknow error. numberStr: %{public}s.", numberStr.c_str());
            return -1;
        }
    } else {
        status = -1;
        return -1;
    }
}

bool IsValidLocaleTag(icu::Locale &locale)
{
    static std::unordered_set<std::string> allValidLocalesLanguageTag;
    GetAllValidLocalesTag(allValidLocalesLanguageTag);
    std::string languageTag = locale.getLanguage() == nullptr ? "" : locale.getLanguage();
    if (allValidLocalesLanguageTag.find(languageTag) == allValidLocalesLanguageTag.end()) {
        HILOG_ERROR_I18N("GetTimePeriodName does not support this languageTag: %{public}s", languageTag.c_str());
        return false;
    }
    return true;
}

void GetAllValidLocalesTag(std::unordered_set<std::string>& allValidLocalesLanguageTag)
{
    static bool init = false;
    if (init) {
        return;
    }
    std::lock_guard<std::mutex> matchLocaleLock(validLocaleMutex);
    if (init) {
        return;
    }
    int32_t validCount = 1;
    const icu::Locale *validLocales = icu::Locale::getAvailableLocales(validCount);
    for (int i = 0; i < validCount; i++) {
        const char* language = validLocales[i].getLanguage();
        if (language != nullptr) {
            allValidLocalesLanguageTag.insert(language);
        }
    }
    init = true;
}

bool CheckTzDataFilePath(const std::string &filePath)
{
    char *realpathRes = nullptr;
    realpathRes = realpath(filePath.c_str(), nullptr);
    if (realpathRes == nullptr) {
        return false;
    }
    std::ifstream file(filePath.c_str());
    if (!file.good()) {
        file.close();
        free(realpathRes);
        return false;
    }
    file.close();
    free(realpathRes);
    realpathRes = nullptr;
    return true;
}

std::string StrReplaceAll(const std::string& str,
    const std::string& target, const std::string& replace)
{
    std::string::size_type pos = 0;
    std::string result = str;
    if (replace.empty() || target.compare(replace) == 0) {
        return result;
    }
    while ((pos = result.find(target)) != std::string::npos) {
        result.replace(pos, target.length(), replace);
    }
    return result;
}

std::string GetISO3Language(const string& language)
{
    UErrorCode icuStatus = U_ZERO_ERROR;
    icu::Locale locale = icu::Locale::forLanguageTag(language.data(), icuStatus);
    if (U_FAILURE(icuStatus) || !IsValidLocaleTag(locale)) {
        return "";
    }
    return locale.getISO3Language();
}

std::string GetISO3Country(const string& country)
{
    UErrorCode icuStatus = U_ZERO_ERROR;
    icu::Locale locale;
    if (LocaleConfig::IsValidRegion(country)) {
        locale = icu::LocaleBuilder().setLanguage("zh").setRegion(country).build(icuStatus);
    } else if (LocaleConfig::IsValidTag(country)) {
        locale = icu::Locale::forLanguageTag(country.data(), icuStatus);
    } else {
        return "";
    }
    if (U_FAILURE(icuStatus) || !IsValidLocaleTag(locale)) {
        return "";
    }
    return locale.getISO3Country();
}

bool FileExist(const std::string& path)
{
    bool status = false;
    try {
        status = std::filesystem::exists(path.c_str());
    } catch (const std::filesystem::filesystem_error &except) {
        HILOG_ERROR_I18N("utils: FileExist failed because filesystem_error, error message: %{public}s.",
            except.code().message().c_str());
        return false;
    } catch (const std::__h::__fs::filesystem::filesystem_error &except) {
        HILOG_ERROR_I18N("utils: FileExist failed because filesystem_error, error message: %{public}s.",
            except.code().message().c_str());
        return false;
    } catch (const std::bad_alloc &except) {
        HILOG_ERROR_I18N("utils: FileExist failed because bad_alloc, error message: %{public}s.",
            except.what());
        return false;
    }
    return status;
}

bool FileCopy(const std::string& srcPath, const std::string& dstPath)
{
    try {
        std::filesystem::copy(srcPath.c_str(), dstPath.c_str());
        return true;
    } catch (const std::filesystem::filesystem_error &except) {
        HILOG_ERROR_I18N("utils: FileCopy failed because filesystem_error, error message: %{public}s.",
            except.code().message().c_str());
    } catch (const std::__h::__fs::filesystem::filesystem_error &except) {
        HILOG_ERROR_I18N("utils: FileCopy failed because filesystem_error, error message: %{public}s.",
            except.code().message().c_str());
    } catch (const std::bad_alloc &except) {
        HILOG_ERROR_I18N("utils: FileCopy failed because bad_alloc, error message: %{public}s.",
            except.what());
    }
    return false;
}

bool IsLegalPath(const std::string& path)
{
    if (path.find("./") != std::string::npos ||
        path.find("../") != std::string::npos) {
        return false;
    }
    return true;
}
 
bool IsDirExist(const char *path)
{
    if (!(path && *path)) {
        return false;
    }
    size_t length = strlen(path);
    if (length > PATH_MAX) {
        return false;
    }
    char resolvedPath[PATH_MAX];
    if (realpath(path, resolvedPath) == nullptr) {
        return false;
    }
    struct stat buf;
    return stat(resolvedPath, &buf) == 0 && S_ISDIR(buf.st_mode);
}
 
std::string trim(std::string &s)
{
    if (s.empty()) {
        return s;
    }
    s.erase(0, s.find_first_not_of(" "));
    s.erase(s.find_last_not_of(" ") + 1);
    return s;
}

bool GetPseudoLocalizationEnforce()
{
    std::string systemLocale = LocaleConfig::GetSystemLocale();
    if (systemLocale.compare(PSEUDO_LOCALE_TAG) == 0) {
        return true;
    }
    return false;
}

std::string PseudoLocalizationProcessor(const std::string &input)
{
    return PseudoLocalizationProcessor(input, GetPseudoLocalizationEnforce());
}

std::string PseudoLocalizationProcessor(const std::string &input, bool ifEnforce)
{
    if (ifEnforce) {
        return PSEUDO_START_TAG + input + PSEUDO_END_TAG;
    }
    return input;
}

bool CheckSystemPermission()
{
    uint64_t tokenId = IPCSkeleton::GetCallingFullTokenID();
    uint32_t callerToken = IPCSkeleton::GetCallingTokenID();
    bool isSystemApp = Security::AccessToken::TokenIdKit::IsSystemAppByFullTokenID(tokenId);
    Security::AccessToken::ATokenTypeEnum tokenType =
        Security::AccessToken::AccessTokenKit::GetTokenTypeFlag(callerToken);
    bool isShell = tokenType == Security::AccessToken::ATokenTypeEnum::TOKEN_SHELL;
    bool isNative = tokenType == Security::AccessToken::ATokenTypeEnum::TOKEN_NATIVE;
    if (!isSystemApp && !isShell && !isNative) {
        HILOG_ERROR_I18N("CheckSystemPermission failed, because current app is not system app.");
        return false;
    }
    return true;
}

size_t ConvertBytesToSizeT(const char *byteArray)
{
    size_t num0 = static_cast<size_t>(byteArray[0]) << BYTE_ARRAY_OFFSET_FIRST;
    size_t num1 = static_cast<size_t>(byteArray[1]) << BYTE_ARRAY_OFFSET_SECOND;
    size_t num2 = static_cast<size_t>(byteArray[2]) << BYTE_ARRAY_OFFSET_THIRD;
    return num0 + num1 + num2 + static_cast<size_t>(byteArray[BYTE_ARRAY_INDEX_THIRD]);
}

std::set<std::string> GetTimeZoneAvailableIDs(I18nErrorCode &errorCode)
{
    if (availableIDs.size() != 0) {
        return availableIDs;
    }
    struct stat s;
    const char *tzdataFilePath = stat(DISTRO_TZDATA_PATH, &s) == 0 ? DISTRO_TZDATA_PATH : TZDATA_PATH;
    std::unique_ptr<char[]> resolvedPath = std::make_unique<char[]>(PATH_MAX);
    if (realpath(tzdataFilePath, resolvedPath.get()) == nullptr) {
        HILOG_ERROR_I18N("GetTimeZoneAvailableIDs tzdata file path isn't exists.");
        return availableIDs;
    }
    std::ifstream tzdataFile(resolvedPath.get(), std::ios::in | std::ios::binary);
    if (!tzdataFile.is_open()) {
        HILOG_ERROR_I18N("Open tzdata failed");
        return availableIDs;
    }
    const size_t versionLength = 12;
    tzdataFile.ignore(versionLength);
    // offset means indexOffset or dataOffset.
    const size_t offsetSize = 4;
    // tempSize is the length of one index, include tz id length, data offset and tz file length.
    const size_t tempSize = 48;
    char *temp = new char[tempSize];
    const size_t offsetSizeTwice = 2;
    tzdataFile.read(temp, offsetSize * offsetSizeTwice);
    size_t indexOffset = ConvertBytesToSizeT(temp);
    size_t dataOffset = ConvertBytesToSizeT(temp + offsetSize);
    tzdataFile.ignore(offsetSize);
    while (indexOffset < dataOffset) {
        tzdataFile.read(temp, tempSize);
        indexOffset += tempSize;
        std::string tzid(temp);
        availableIDs.insert(tzid);
    }
    tzdataFile.close();
    delete[] temp;
    return availableIDs;
}
} // namespace I18n
} // namespace Global
} // namespace OHOS