//
// Created by 23591 on 2019/1/24.
//

#ifndef MD5_DIR_TOOLS_H
#define MD5_DIR_TOOLS_H

#include <string>
#include <vector>
#include "result.hpp"

#define MD5_FILE "md5_result.json"
#define CHECK_RES "检查结果.txt"

int file_digest(const std::string &file, std::string &result);

int dir_digest(const char *dir, std::vector<Result> &result);

cJSON *translate_results_to_c_json(const std::vector<Result> &result);

int check_dir(const char *dir, std::vector<CheckResult> &check_res, bool use_prefix = false);

bool is_ignore(const char * name);
#endif //MD5_DIR_TOOLS_H
