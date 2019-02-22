//
// Created by 23591 on 2019/1/24.
//

#ifndef MD5_DIR_RESULT_H
#define MD5_DIR_RESULT_H

#include <string>
#include <sstream>      // std::stringbuf

#include "log.h"
#include "cJSON.h"

#define PREFIX_STR "prefix"
#define RELATIVE_STR "relative"
#define NAME_STR "name"
#define MD5_STR "md5"

//#define RESULT_DEBUG

class Result {
public:
    std::string prefix;
    std::string relative;
    std::string name;
    std::string md5;

public:
    Result() {
#ifdef RESULT_DEBUG
        DEBUG("called defult")
#endif
        prefix = "";
        relative = "";
        name = "";
        md5 = "";
    }

    Result(const std::string &_prefix, const std::string &_relative, const std::string &_name,
           const std::string &_md5) {
#ifdef RESULT_DEBUG
        DEBUG("init by string & :" << _name)
#endif
        prefix = _prefix;
        relative = _relative;
        name = _name;
        md5 = _md5;
    }

    Result(const std::string &&_prefix, const std::string &&_relative, const std::string &&_name,
           const std::string &&_md5) {
#ifdef RESULT_DEBUG
        DEBUG("init by string && :" << _name)
#endif
        prefix = _prefix;
        relative = _relative;
        name = _name;
        md5 = _md5;
    }

    Result(const Result &&r) {
#ifdef RESULT_DEBUG
        DEBUG("init by Result&& :" << r.name)
#endif
        prefix = r.prefix;
        relative = r.relative;
        name = r.name;
        md5 = r.md5;
    }

    Result(const Result &r) {
#ifdef RESULT_DEBUG
        DEBUG("init by Result& :" << r.name)
#endif
        prefix = r.prefix;
        relative = r.relative;
        name = r.name;
        md5 = r.md5;
    }


    Result(cJSON *root) : Result() {
#ifdef RESULT_DEBUG
        DEBUG("init by cJSON* :")
#endif
        if (root->type != cJSON_Object) {
            ERROR("type not eq: " << root->type)
            return;
        }
        init_from_json(root, this);
    }

    cJSON *to_c_json() const {
        cJSON *root = cJSON_CreateObject();
        cJSON_AddItemToObject(root, PREFIX_STR, cJSON_CreateString(prefix.c_str()));
        cJSON_AddItemToObject(root, RELATIVE_STR, cJSON_CreateString(relative.c_str()));
        cJSON_AddItemToObject(root, NAME_STR, cJSON_CreateString(name.c_str()));
        cJSON_AddItemToObject(root, MD5_STR, cJSON_CreateString(md5.c_str()));
        return root;
    }

    Result &operator=(const Result &r) {
        this->prefix = r.prefix;
        this->relative = r.relative;
        this->name = r.name;
        this->md5 = r.md5;
        return *this;
    }

public:
    int init_from_json(cJSON *root, Result *r) {
        int err = 0;
        cJSON *tmp;
        if (((tmp = cJSON_GetObjectItem(root, PREFIX_STR)) == nullptr) || (tmp->type != cJSON_String)) {
            ERROR("PREFIX_STR is null")
            err = -1;
        } else {
            r->prefix = std::string(tmp->valuestring);
        }
        if (((tmp = cJSON_GetObjectItem(root, RELATIVE_STR)) == nullptr) || (tmp->type != cJSON_String)) {
            ERROR("RELATIVE_STR is null")
            err = -1;
        } else {
            r->relative = std::string(tmp->valuestring);
        }
        if (((tmp = cJSON_GetObjectItem(root, NAME_STR)) == nullptr) || (tmp->type != cJSON_String)) {
            ERROR("NAME_STR is null")
            err = -1;
        } else {
            r->name = std::string(tmp->valuestring);
        }
        if (((tmp = cJSON_GetObjectItem(root, MD5_STR)) == nullptr) || (tmp->type != cJSON_String)) {
            ERROR("MD5_STR is null")
            err = -1;
        } else {
            r->md5 = std::string(tmp->valuestring);
        }
        return err;
    }
};

enum CheckEnum {
    Normal = 0,
    Missing,
    NotEquire,
    Excess,
    ChaekEnumMax = 10,
};

class CheckResult {
public:
    Result r;
    CheckEnum status;
public:
    CheckResult(Result &_r, CheckEnum _status) {
        r = _r;
        status = _status;
    }

    std::string to_string() const {
        std::ostringstream res;
        if (strcmp("", r.prefix.c_str()) != 0) {
            res << r.prefix<<"/";
        } else {
            res << "./";
        }
        res << "\t" << r.relative;
        res << "\t/" << r.name;
        res << "\t:状态：\t" << CheckEnum_to_string(status);
        res << "\n";
        return res.str();
    }

private:
    std::string CheckEnum_to_string(const CheckEnum &e) const {
        switch (e) {
            case Normal:
                return "正常";
            case Missing:
                return "错误：文件丢失";
            case NotEquire:
                return "错误：文件损坏，请留意";
            case Excess:
                return "错误：没有在原始MD5记录中找到该文件信息";
            default:
                return "错误：未预期的错误";
        }
    }
};

#endif //MD5_DIR_RESULT_H
