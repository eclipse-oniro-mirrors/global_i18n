/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef INTERFACES_CJ_INCLUDE_I18N_STRUCT_H_
#define INTERFACES_CJ_INCLUDE_I18N_STRUCT_H_

#include <string>

const int32_t I18N_NOT_VALID = 890001;

extern "C" {
    struct UnitInfo {
        char* unit;
        char* measureSystem;
    };

    struct CDate {
        int64_t year;
        int64_t month;
        int64_t day;
        double icuUdate;
        bool isNull;
    };

    struct CHolidayLocalName {
        char* language;
        char* name;
    };

    struct HolidayLocalNameArr {
        int64_t size;
        CHolidayLocalName *head;
    };

    struct CHolidayInfoItem {
        char* baseName;
        int32_t year;
        int32_t month;
        int32_t day;
        char* language;
        HolidayLocalNameArr localNames;
    };

    struct HolidayInfoItemArr {
        int64_t size;
        CHolidayInfoItem *head;
    };

    struct CArrStr {
        char** data;
        int32_t length;
    };

    struct CEntityInfoItem {
        char* type;
        int32_t begin;
        int32_t end;
    };

    struct CEntityInfoItemArr {
        int32_t size;
        CEntityInfoItem* head;
    };
}


struct EntityInfoItem {
    std::string type;
    int32_t begin;
    int32_t end;
};

#endif  // INTERFACES_CJ_INCLUDE_I18N_STRUCT_H_
