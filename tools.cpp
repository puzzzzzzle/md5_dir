//
// Created by 23591 on 2019/1/24.
//
#include <iostream>
#include <dirent.h>
#include <sys/stat.h>

#include "md5.h"
#include "tools.h"
#include "log.h"

#define BUFF_SIZE 12040

using namespace std;

bool is_ignore(const char *name) {
    return (
            (strcmp(".", name) == 0)
            || (strcmp("..", name) == 0)
            || (strcmp(MD5_FILE, name) == 0)
            || (strcmp(CHECK_RES, name) == 0)
    );
}

int file_digest(const string &file, string &result) {

    ifstream in(file.c_str(), ios::binary);
    MD5 md5;
    std::streamsize length;
    char buffer[BUFF_SIZE];

    if (!in)
        return -1;

    while (!in.eof()) {
        in.read(buffer, BUFF_SIZE);
        length = in.gcount();
        if (length > 0) {
            md5.update(buffer, length);
        }
    }
    in.close();
    result = md5.toString();
    return 0;
}

static void dir_digest_loop(const char *prefix, const char *reletive, std::vector<Result> &result) {
    DIR *curr;
    dirent entry{}, *entry_ptr;
    string md5;
    string curr_path = string(prefix) + "/" + string(reletive) + "/";
    if ((curr = opendir(curr_path.c_str())) == nullptr) {
        FATAL("unexcept err happen :opendir err:" << strerror(errno) << "\t:" << curr_path)
        exit(-2);
    }

    readdir_r(curr, &entry, &entry_ptr);
    while (entry_ptr != nullptr) {
        do {
            if (is_ignore(entry.d_name)) {
                break;
            }
            if (DT_DIR == entry.d_type) {
                dir_digest_loop(prefix, (string(reletive) + string("/") + string(entry.d_name)).c_str(), result);
            } else if (DT_REG == entry.d_type) {
                INFO("md5 for : " << entry.d_name)
                if (file_digest(curr_path + string(entry.d_name), md5)) {
                    ERROR("err happen in file :" << entry.d_name)
                    md5 = "-1";
                }
                result.emplace_back(string(prefix), "./" + string(reletive),
                                    string(entry.d_name), md5);
            } else {
                ERROR("un support type  " << entry.d_name)
            }
        } while (false);
        readdir_r(curr, &entry, &entry_ptr);
    }
    closedir(curr);
}

int dir_digest(const char *dir, std::vector<Result> &result) {
    DIR *curr;
    dirent entry{}, *entry_ptr;
    string md5;
    if ((curr = opendir(dir)) == nullptr) {
        FATAL("opendir err:" << strerror(errno) << "\t:" << dir)
        return -2;
    }

    readdir_r(curr, &entry, &entry_ptr);
    while (entry_ptr != nullptr) {
        do {
            if (is_ignore(entry.d_name)) {
                break;
            }
            if (DT_DIR == entry.d_type) {
                dir_digest_loop(dir, entry.d_name, result);
            } else if (DT_REG == entry.d_type) {
                INFO("md5 for : " << entry.d_name)
                if (file_digest(string(dir) + string("/") + string(entry.d_name), md5)) {
                    ERROR("err happen in file :" << entry.d_name)
                    md5 = "-1";
                }
                result.emplace_back(string(dir), string("./"), string(entry.d_name), md5);
            } else {
                ERROR("un support type  " << entry.d_name)
            }
        } while (false);
        readdir_r(curr, &entry, &entry_ptr);
    }

    closedir(curr);
    return 0;
}

cJSON *translate_results_to_c_json(const std::vector<Result> &result) {
    cJSON *root = cJSON_CreateArray();
    for (const Result &r:result) {
        cJSON_AddItemToArray(root, r.to_c_json());
    }
    return root;
}

bool compare_without_md5(const Result *l_res, const Result *r_res, bool use_prefix) {
    if (use_prefix && (l_res->prefix != r_res->prefix)) {
        return false;
    }
    if ((l_res->name != r_res->name)
        || (l_res->relative != r_res->relative)) {
        return false;
    }
    return true;
}

static int check_cJSON(cJSON *read_from_file, cJSON *calc_from_dir, bool use_prefix, vector<CheckResult> &check_res) {
    cJSON *file__tmp, *text_temp;
    int dir_size = cJSON_GetArraySize(calc_from_dir);
    if (read_from_file->type != cJSON_Array || calc_from_dir->type != cJSON_Array) {
        FATAL("root must be array")
        return -3;
    }

    for (int i = 0; i < dir_size; i++) {
        file__tmp = cJSON_GetArrayItem(calc_from_dir, i);
        Result file_r(file__tmp);
        for (int j = 0; j < cJSON_GetArraySize(read_from_file); j++) {
            text_temp = cJSON_GetArrayItem(read_from_file, j);
            Result text_r(text_temp);
            if (compare_without_md5(&file_r, &text_r, use_prefix)) {
                if (file_r.md5 == text_r.md5) {
                    check_res.emplace_back(file_r, Normal);
                } else {
                    check_res.emplace_back(file_r, NotEquire);
                }
                cJSON_DeleteItemFromArray(read_from_file, j);
                break;
            }
            check_res.emplace_back(file_r, Excess);
        }
    }
    for (int j = 0; j < cJSON_GetArraySize(read_from_file); j++) {
        text_temp = cJSON_GetArrayItem(read_from_file, j);
        Result text_r(text_temp);
        check_res.emplace_back(text_r, Missing);
    }
    return 0;
}

int check_dir(const char *dir, vector<CheckResult> &check_res, bool use_prefix) {
    vector<Result> calc_dir_res;
    string md5_file_path = string(dir) + "./" + MD5_FILE;
    string read_buff;
    ifstream md5_json_in;
    ostringstream json_buf;
    cJSON *read_from_file, *calc_from_dir;
    md5_json_in.open(md5_file_path, ios::in);

    if (!md5_json_in) {
        FATAL("md5 file open err")
        exit(-3);
    }
    while (md5_json_in >> read_buff) {
        json_buf << read_buff;
    }
    md5_json_in.close();
    read_from_file = cJSON_Parse(json_buf.str().c_str());
    if (read_from_file == nullptr) {
        FATAL("err happened when cJSON_Parse")
        exit(-3);
    }

    if (dir_digest(dir, calc_dir_res)) {
        FATAL("err happened when md5 the dir")
        exit(-3);
    }
    calc_from_dir = translate_results_to_c_json(calc_dir_res);

    if (check_cJSON(read_from_file, calc_from_dir, use_prefix, check_res)) {
        FATAL("err happened when check_cJSON")
        exit(-3);
    }

    cJSON_Delete(calc_from_dir);
    cJSON_Delete(read_from_file);
    return 0;
}

