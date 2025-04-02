/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#ifndef OHOS_GLOBAL_MATCHED_DATE_TIME_INFO_H
#define OHOS_GLOBAL_MATCHED_DATE_TIME_INFO_H

#include <string>
#include <unicode/regex.h>
#include "utils.h"

namespace OHOS {
namespace Global {
namespace I18n {
class MatchedDateTimeInfo {
public:
    MatchedDateTimeInfo() : _begin(-1), _end(-1), type(0), isTimePeriod(false) {}
    MatchedDateTimeInfo(int begin, int end, std::string& regex)
        : _begin(begin), _end(end), regex(regex), type(0), isTimePeriod(false) {}
    ~MatchedDateTimeInfo() {}

    void SetIsTimePeriod(bool is)
    {
        isTimePeriod = is;
    }

    bool IsTimePeriod()
    {
        if (isTimePeriod) {
            return isTimePeriod;
        }
        if (regex == "") {
            return false;
        }
        int32_t status = 0;
        int key = ConvertString2Int(regex, status);
        // 49999 and 60000 are rules number.
        return key > 49999 && key < 60000;
    }

    void SetBegin(int begin)
    {
        this->_begin = begin;
    }

    int GetBegin()
    {
        return _begin;
    }

    void SetEnd(int end)
    {
        this->_end = end;
    }

    int GetEnd()
    {
        return _end;
    }

    std::string GetRegex()
    {
        return regex;
    }

    int GetType()
    {
        return type;
    }

    void SetType(int type)
    {
        this->type = type;
    }

    bool operator<(const MatchedDateTimeInfo& info) const
    {
        if (_begin < info._begin) {
            return true;
        }
        return false;
    }

private:
    // Start position of the phone number
    int _begin;
    // End position of the phone number
    int _end;
    // Date rule key
    std::string regex;
    int type;
    bool isTimePeriod;
};
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif