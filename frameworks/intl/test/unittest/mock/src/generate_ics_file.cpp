/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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
#include <iostream>
#include <fstream>
#include <vector>
#include "generate_ics_file.h"

namespace OHOS {
namespace Global {
namespace I18n {

IcsFileWriter::IcsFileWriter()
{
}

IcsFileWriter::~IcsFileWriter()
{
}

std::string IcsFileWriter::GenerateFile()
{
    std::string filePath = "/data/log/TR.ics";
    std::ofstream fstream(filePath);
    if (!fstream.is_open()) {
        printf("file can't access.\r\n");
        return filePath;
    }
    std::vector<std::string> list = {
        "BEGIN:VCALENDAR\r\n",
        "METHOD:PUBLISH\r\n",
        "BEGIN:VEVENT\r\n",
        "UID:1\r\n",
        "DTSTART;VALUE=DATE:20220625\r\n",
        "DTEND;VALUE=DATE:20220625\r\n",
        "SUMMARY:Sacrifice Feast Holiday\r\n",
        "RESOURCES;LANGUAGE=TR:Kurban Bayrami Tatili\r\n"
        "END:VEVENT\r\n",
        "BEGIN:VEVENT\r\n",
        "UID:2\r\n",
        "DTSTART;VALUE=DATE:20220626\r\n",
        "DTEND;VALUE=DATE:20220626\r\n",
        "SUMMARY:The Second Day of Sacrifice Feast\r\n",
        "RESOURCES;LANGUAGE=TR:Kurban Bayrami 2. Günü\r\n"
        "END:VEVENT\r\n",
        "BEGIN:VEVENT\r\n",
        "UID:3\r\n",
        "DTSTART;VALUE=DATE:20220625\r\n",
        "DTEND;VALUE=DATE:20220625\r\n",
        "SUMMARY:Test Holiday\r\n",
        "RESOURCES;LANGUAGE=TR:test holiday\r\n"
        "END:VEVENT\r\n",
        "END:VCALENDAR\r\n"
    };
    for (size_t i = 0; i < list.size(); i++) {
        fstream << list[i] << std::endl;
    }
    fstream.close();
    return filePath;
}

bool IcsFileWriter::WriteFile(const std::string& content, const std::string& filePath) const
{
    std::ofstream fstream(filePath);
    if (!fstream.is_open()) {
        printf("file can't access.\r\n");
        return false;
    }
    fstream << content << std::endl;
    fstream.close();
    return true;
}

std::string IcsFileWriter::WriteManifest(const std::vector<std::string>& list, std::string& fileName)
{
    std::string filePath = "/data/log/" + fileName;
    std::ofstream fstream(filePath);
    if (!fstream.is_open()) {
        printf("file can't access.\r\n");
        return filePath;
    }
    for (size_t i = 0; i < list.size(); i++) {
        fstream << list[i] << std::endl;
    }
    fstream.close();
    return filePath;
}

std::string IcsFileWriter::WriteBinaryFile(std::string& fileName)
{
    std::string filePath = "/data/log/" + fileName;
    std::ofstream fstream(filePath, std::ios_base::out | std::ios_base::binary);
    if (!fstream.is_open()) {
        printf("file can't access.\r\n");
        return filePath;
    }
    if (fstream.is_open()) {
        const char* data = "01010101000101001010";
        const size_t len = 20;
        fstream.write(data, len);
    }
    fstream.close();
    return filePath;
}
} // namespace I18n
} // namespace Global
} // namespace OHOS