#!/usr/bin/env python
# coding=utf-8
"""
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
"""
import re
import os
import logging
import pickle
import png


def read_data(filename):
    data_res = {}
    loc_pattern = re.compile("\(.*\)")
    tz_pattern = re.compile("\[.*\]")
    raw_lines = []
    with open(filename, "r") as f:
        lines = f.readlines()
        for line in lines:
            raw_lines.append(line)
    for line in raw_lines:
        line = line.strip()
        loc = re.search(loc_pattern, line).group()
        tz = re.search(tz_pattern, line).group()
        location = eval(loc)
        timezones = tz[1:-1].split(",")
        data_res[location] = timezones
    with os.fdopen(os.open("data/loc2tz.pickle", os.O_RDWR | os.O_CREAT, 0o640), "wb") as f:
        pickle.dump(data_res, f)
    return data_res


def load_data(pickle_name):
    with open(pickle_name, "rb") as f:
        loaded_data = pickle.load(f)
    return loaded_data


def classify_timezones(data_input):
    group_wn = set()
    group_en = set()
    group_ws = set()
    group_es = set()
    for loc, tzs in data_input.items():
        if loc[0] < 0 and loc[1] >= 0:
            for tz in tzs:
                group_wn.add(tz)
        elif loc[0] >= 0 and loc[1] >= 0:
            for tz in tzs:
                group_en.add(tz)
        elif loc[0] < 0 and loc[1] < 0:
            for tz in tzs:
                group_ws.add(tz)
        else:
            for tz in tzs:
                group_es.add(tz)
    return [group_wn, group_en, group_ws, group_es]


def numerized_dictionary(classes):
    numerized_classes = []
    for group in classes:
        temp_group = {}
        sgroup = list(group)
        sgroup.sort()
        for num, name in enumerate(sgroup):
            temp_group[name] = num
        numerized_classes.append(temp_group)
    for i, c in enumerate(numerized_classes):
        logging.info(i)
        logging.info(c)
    return numerized_classes


def export_numerized_dicts(numerized_classes):
    with os.fdopen(os.open("data/name2num.txt", os.O_RDWR | os.O_CREAT, 0o640), "w", encoding="utf-8") as f:
        f.write(str(numerized_classes))
    num2name_dict_list = []
    for d in numerized_classes:
        num2name = dict(zip(d.values(), d.keys()))
        num2name_dict_list.append(num2name)
    with os.fdopen(os.open("data/num2name.txt", os.O_RDWR | os.O_CREAT, 0o640), "w", encoding="utf-8") as f:
        f.write(str(num2name_dict_list))


def generate_array_from_data(data_input, numerized_classes):
    total_nums_list = []
    for key, val in data_input.items():
        original_keys = key
        if original_keys[0] < 0 and original_keys[1] >= 0:
            quater = 0
        elif original_keys[0] >= 0 and original_keys[1] >= 0:
            quater = 1
        elif original_keys[0] < 0 and original_keys[1] < 0:
            quater = 2
        else:
            quater = 3
        names_l = []
        for v in val:
            names_l.append(numerized_classes[quater][v])
        nums_l = [255, 255, 255]
        for i in enumerate(names_l):
            nums_l[i[0]] = names_l[i[0]]
        total_nums_list += nums_l
    return total_nums_list


def group_in_total(total_list):
    i = 0
    map_of_array = []
    col_of_array = []
    for item in total_list:
        col_of_array.append(item)
        i += 1
        if i == 1800 * 3:
            map_of_array.append(col_of_array)
            col_of_array = []
            i = 0
    return map_of_array


def pack2png(map_of_array):
    png.from_array(map_of_array, "RGB").save("data/loc2tz.dat")


def save_png_test():
    array_one_point = [255, 0, 0]
    array_one_row = [array_one_point] * 3600
    array_map = [array_one_row] * 1800
    png.from_array(array_map, "RGB").save("03.png")


if __name__ == '__main__':
    read_data("data/location2tz.txt")
    data = load_data("data/loc2tz.pickle")
    name_classes = classify_timezones(data)
    nmrzd_classes = numerized_dictionary(name_classes)
    export_numerized_dicts(nmrzd_classes)
    t_list = generate_array_from_data(data, nmrzd_classes)
    map_array = group_in_total(t_list)
    pack2png(map_array)