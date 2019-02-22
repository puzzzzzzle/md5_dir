#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include "tools.h"
#include "result.hpp"
#include "log.h"
#include "cJSON.h"

using namespace std;

string get_final_result(vector<CheckResult> check_res) {
    ostringstream normal, missing, not_equire, excess, other, final;
    for (const CheckResult &r:check_res) {
        switch (r.status) {
            case Normal:
                normal << r.to_string();
                break;
            case Missing:
                missing << r.to_string();
                break;
            case NotEquire:
                not_equire << r.to_string();
                break;
            case Excess:
                excess << r.to_string();
                break;
            default:
                other << r.to_string();
                break;;
        }
    }
    final << "\t\t文件出错：\n" << not_equire.str() << "\n";
    final << "\t\t文件丢失：\n" << missing.str() << "\n";
    final << "\t\t多出文件：\n" << excess.str() << "\n";
    final << "\t\t其他错误：\n" << other.str() << "\n";
    final << "\t\t正常文件：\n" << normal.str() << "\n";
    return final.str();
}

int main(int argc, char **argv) {
    string file = "./";
    bool calc = false, check = false;

    if (access((file + MD5_FILE).c_str(),R_OK|W_OK)) {
        calc = true;
        check = false;
    } else {
        calc = false;
        check = true;
    }
    if (calc) {
        //计算MD5
        vector<Result> ress;
        dir_digest(file.c_str(), ress);
        cJSON *root = translate_results_to_c_json(ress);
        char *json_str = cJSON_PrintBuffered(root, 10240, 1);
        INFO(json_str);

        ofstream out;
        out.open(file + "./" + MD5_FILE, ios::trunc);
        if (!out) {
            FATAL("open err" << strerror(errno))
            exit(-4);
        }
        out << json_str << endl;
        out.close();
        free(json_str);
        cJSON_Delete(root);
        INFO("结果已写入文件")
    }
    if (check) {
        //检查
        ifstream in;
        string read_buff;
        vector<CheckResult> check_res;

        in.open(file + "./" + MD5_FILE, ios::in);
        if (!in) {
            FATAL("open err" << strerror(errno))
            exit(-4);
        }
        if (check_dir(file.c_str(), check_res)) {
            FATAL("check_dir err")
            exit(-4);
        }

        string final = get_final_result(check_res);
        ofstream final_f;
        final_f.open(file + "./" + CHECK_RES, ios::trunc);
        final_f << final << endl;
        final_f.close();
        INFO("\t\t最终结果：\n" << final << "\n结果将保存在：" << CHECK_RES << "\t中")
    }
    return 0;
}
