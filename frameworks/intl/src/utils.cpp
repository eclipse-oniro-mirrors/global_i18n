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
#include "utils.h"
#include <algorithm>
#include <bitset>
#include <cerrno>
#include <climits>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <mutex>
#include <sstream>
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

namespace OHOS {
namespace Global {
namespace I18n {
using namespace std;
static const std::string PSEUDO_LOCALE_TAG = "en-XA";
static const std::string PSEUDO_START_TAG = "{";
static const std::string PSEUDO_END_TAG = "}";
static const char CHAR_A = 'A';
static const int32_t CHAR2INT_SIZE = 2;
constexpr const char *TIMEZONE_LIST_CONFIG_PATH = "/system/etc/zoneinfo/timezone_list.cfg";
constexpr const char *DISTRO_TIMEZONE_LIST_CONFIG = "/system/etc/tzdata_distro/timezone_list.cfg";
static std::mutex validLocaleMutex;

void Split(const string &src, const string &sep, vector<string> &dest)
{
    if (src.empty()) {
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

void Split(const std::string& src, const std::string& sep, std::unordered_set<std::string>& dest)
{
    dest.clear();
    if (src.empty()) {
        return;
    }
    std::string::size_type begin = 0;
    std::string::size_type end = src.find(sep);
    while (end != string::npos) {
        dest.insert(src.substr(begin, end - begin));
        begin = end + sep.size();
        end = src.find(sep, begin);
    }
    if (begin != src.size()) {
        dest.insert(src.substr(begin));
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

int32_t ConvertString2Int(const string &numberStr, int32_t& status, int base)
{
    if (!numberStr.empty() && std::all_of(numberStr.begin(), numberStr.end(), ::isdigit)) {
        try {
            return std::stoi(numberStr, nullptr, base);
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

double ConvertString2Double(const string &numberStr, int32_t& status)
{
    try {
        return std::stod(numberStr);
    } catch (const std::invalid_argument &except) {
        status = -1;
        return -1;
    } catch (const std::out_of_range &except) {
        status = -1;
        return -1;
    } catch (...) {
        status = -1;
        HILOG_ERROR_I18N("ConvertString2Double: unknow error. numberStr: %{public}s.", numberStr.c_str());
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
    if (validLocales == nullptr) {
        return;
    }
    for (int i = 0; i < validCount; i++) {
        const char* language = validLocales[i].getLanguage();
        if (language != nullptr) {
            allValidLocalesLanguageTag.insert(language);
        }
    }
    init = true;
}

std::string GetAbsoluteFilePath(const std::string &filePath)
{
    if (filePath.empty()) {
        HILOG_ERROR_I18N("utils::GetAbsoluteFilePath: input param filePath is empty");
        return "";
    }
    char* result = realpath(filePath.c_str(), nullptr);
    if (result == nullptr) {
        HILOG_ERROR_I18N("utils::GetAbsoluteFilePath realpath error, error message: %{public}s.", strerror(errno));
        return "";
    }
    std::ifstream file(result);
    if (!file.good()) {
        free(result);
        HILOG_ERROR_I18N("utils::GetAbsoluteFilePath file open is not good.");
        return "";
    }
    std::string absolutePath(result);
    free(result);
    result = nullptr;
    return absolutePath;
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

void ClearDir(const std::filesystem::path& dirPath)
{
    std::error_code errCode;
    for (const auto& entry : std::filesystem::directory_iterator(dirPath)) {
        std::filesystem::remove_all(entry.path(), errCode);
        if (errCode) {
            HILOG_ERROR_I18N("ClearDir: Error removing file: %{public}s, Error: %{public}s", entry.path().c_str(),
                errCode.message().c_str());
        }
    }
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

std::set<std::string> GetTimeZoneAvailableIDs(I18nErrorCode &errorCode)
{
    if (availableIDs.size() != 0) {
        return availableIDs;
    }
    struct stat s;
    const char *tzIdConfigPath = stat(DISTRO_TIMEZONE_LIST_CONFIG, &s) == 0 ?
        DISTRO_TIMEZONE_LIST_CONFIG : TIMEZONE_LIST_CONFIG_PATH;
    std::string absolutePath = GetAbsoluteFilePath(tzIdConfigPath);
    if (absolutePath.empty()) {
        HILOG_ERROR_I18N("GetTimeZoneAvailableIDs: check file %{public}s failed.", tzIdConfigPath);
        return availableIDs;
    }
    std::unique_ptr<char[]> resolvedPath = std::make_unique<char[]>(PATH_MAX + 1);
    if (realpath(tzIdConfigPath, resolvedPath.get()) == nullptr) {
        HILOG_ERROR_I18N("Get realpath failed, errno: %{public}d.", errno);
        return availableIDs;
    }
    std::ifstream file(resolvedPath.get());
    if (!file.good()) {
        HILOG_ERROR_I18N("Open timezone list config file failed.");
        return availableIDs;
    }
    std::string line;
    while (std::getline(file, line)) {
        if (line.length() == 0) {
            break;
        }
        line.resize(line.find_last_not_of("\r\n") + 1);
        availableIDs.insert(line);
    }
    file.close();
    return availableIDs;
}

bool RegexSearchNoExcept(const std::string& str, std::smatch& match, const std::regex& regex)
{
    try {
        return std::regex_search(str, match, regex);
    } catch (const std::regex_error &except) {
        HILOG_ERROR_I18N("RegexSearchNoExcept: regex_error caught %{public}s.", except.what());
        return false;
    }
}

std::string LocaleEncode(const std::string& locale)
{
    std::stringstream ss;
    for (auto& c : locale) {
        int32_t number = (c == '-') ? 0 : static_cast<int32_t>(c) - static_cast<int32_t>(CHAR_A) + 1;
        ss << std::setw(CHAR2INT_SIZE) << std::setfill('0') << number;
    }
    return ss.str();
}

bool Eq(double a, double b)
{
    return fabs(a - b) < 1e-6;
}

bool Geq(double a, double b)
{
    return a > b || Eq(a, b);
}

bool Leq(double a, double b)
{
    return a < b || Eq(a, b);
}

std::string StrToHex(const std::string& text, size_t num)
{
    std::string binary;
    for (auto i : text) {
        // 8-bit binary.
        bitset<8> bs(i);
        binary += bs.to_string();
    }
    int32_t status = 0;
    // 2 means binary.
    int32_t convertNum = ConvertString2Int(binary, status, 2);
    if (status != 0) {
        HILOG_ERROR_I18N("StrToHex: convert %{public}s to number error", binary.c_str());
        return "";
    }
    std::stringstream ss;
    ss << std::hex << std::setw(num) << std::setfill('0') << convertNum;
    std::string hex;
    ss >> hex;
    return hex;
}

std::string HexToStr(const std::string& hex)
{
    // The length of hex must be a multiple of 2.
    size_t len = 2;
    if (hex.length() % len != 0) {
        return "";
    }

    bool flag = true;
    std::string result;
    for (size_t i = 0; i < hex.length(); i += len) {
        std::string hexStr = hex.substr(i, len);
        if (flag && hexStr.compare("00") == 0) {
            continue;
        }
        flag = false;
        int32_t value = 0;
        try {
            value = std::stoi(hexStr, nullptr, 16); // 16 represents hexadecimal.
        } catch (const std::invalid_argument &except) {
            HILOG_ERROR_I18N("HexToStr: invalid_argument, hexStr: %{public}s.", hexStr.c_str());
            return "";
        } catch (const std::out_of_range &except) {
            HILOG_ERROR_I18N("HexToStr: out_of_range, hexStr: %{public}s.", hexStr.c_str());
            return "";
        } catch (...) {
            HILOG_ERROR_I18N("HexToStr: unknow error, hexStr: %{public}s.", hexStr.c_str());
            return "";
        }
        result += char(value);
    }
    return result;
}
} // namespace I18n
} // namespace Global
} // namespace OHOS