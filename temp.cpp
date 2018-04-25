//
// Created by Yunfan Li on 2018/4/20.
//

#include <iostream>
#include <fstream>
#include <vector>
#include <regex>

using namespace std;

regex regex1("^#include\\s+\"[A-Za-z]+\\.h\"$");
regex regex2("^#define\\s+");//#define PART1
regex regex3("^#define\\s+[^\\s:]+\\s+[^\\s:]+$");//#define Cong(arg) "PLUSES "#arg
regex regex4("^#define\\s+[^\\s:]+$");//#define A 1
regex regex5("^#ifndef\\s+");
regex regex6("^#endif");
regex regex7("^#undef\\s+[^\\s:]+$");
regex regex8("^#if\\s+");
regex regex9("^#ifdef\\s+");
regex regex10("^#if[a-z]{0,4}");//匹配#if、#ifndef、#ifdef
regex regex11("^#else");
regex regex12("^#define\\s+[^\\s:\\(\\)]+\\([^\\s:\\(\\)]+\\)\\s+[^:]+");

vector<vector<string> > part;
vector<vector<string> > define;
vector<vector<string> > param;


vector<int> handler(int i, vector<string> &cpp);

vector<int> handler(int i, vector<string> &cpp) {
    int temp = 0;
    int ifCount = 0;
    int elseCount = 0;
    int plug = 0;
    int flag = 0;
    int elsePosition = 0;
    int j;
    for (j = i + 1; j < cpp.size(); j++) {
        if (regex_search(cpp[j], regex8)) {
            ifCount++;
        }
        //记录else的位置
        if (regex_match(cpp[j], regex11)) {
            elseCount++;
            if (elseCount == ifCount + 1 && temp == 0) {
                temp++;
                elsePosition = j;
            }
        }
        if (regex_search(cpp[j], regex10)) {
            plug++;
            continue;
        }
        if (regex_match(cpp[j], regex6)) {
            flag++;
            if (flag == plug + 1) {
                break;
            }
            continue;
        }
    }
    vector<int> result;
    result.push_back(elsePosition);
    result.push_back(j);
    return result;
}

vector<string> split(string &str, char temp) {
    vector<string> result;
    for (int i = 0, j = 0; i < str.length(); i++) {
        if (str[i] == temp) {
            result.push_back(str.substr(j, i - j));
            j = i + 1;
        }
        if (i == str.length() - 1) {
            result.push_back(str.substr(j, i - j + 1));
        }
    }
    return result;
}

void readFromFile(vector<string> &cpp, string inPath) {
    ifstream OpenFile(inPath);
    string line;
    while (getline(OpenFile, line)) {
        cpp.push_back(line);
    }
    OpenFile.close();
}

vector<string> writeOutFile(string outPath, vector<string> &cpp) {
    ofstream OutPut(outPath);
    if (OutPut.is_open()) {
        for (int i = 0; i < cpp.size(); i++) {
            OutPut << cpp[i];
            OutPut << endl;
        }
        OutPut.close();
    }

}

void handleInclude(vector<string> &cpp, int i) {
    string line;
    if (regex_match(cpp[i], regex1)) {
        vector<string> head = split(cpp[i], ' ');
        ifstream HeaderOpen(head[1].substr(1, head[1].size() - 2));
        cpp.erase(cpp.begin() + i);
        int j = i;
        while (getline(HeaderOpen, line)) {
            cpp.insert(cpp.begin() + j, line);
            j++;
        }
        HeaderOpen.close();
    }
}

void handleDefine(vector<string> &cpp, int i) {
    //数据结构define 每个define[i] 都是由{"#define","XXX","XXX"...,"0"}构成
    // 最后的0为标志符
    if (regex_match(cpp[i], regex3) && !regex_match(cpp[i], regex12)) {
        part.push_back(split(cpp[i], ' '));
        vector<string> temp;
        temp.push_back(split(cpp[i], ' ')[0]);
        temp.push_back(split(cpp[i], ' ')[1]);
        define.push_back(temp);
    }
    //识别标识符定义
    if (regex_match(cpp[i], regex4)) {
        define.push_back(split(cpp[i], ' '));
    }
    //处理带参数宏
    if (regex_match(cpp[i], regex12)) {
        vector<string> temp = split(split(cpp[i], ' ')[1], '(');
        temp[1] = temp[1].substr(0, temp[1].length() - 1);
        //避免宏中自带的空格引起的不必要分割
        string str = "";
        for (int t = 2; t < split(cpp[i], ' ').size(); t++) {
            str += split(cpp[i], ' ')[t] + " ";
        }
        str = str.substr(0, str.length() - 1);
        temp.push_back(str);
        param.push_back(temp);
        define.push_back(temp);
    }
}

void initDefineFlag() {
    //将标识符的状态设置为初始化为0
    for (int i = 0; i < define.size(); i++)
        define[i].push_back("0");
}


void handleMacro(vector<regex> &macro) {
    for (int i = 0; i < part.size(); i++) {
        regex reg(" " + part[i][1]);
        macro.push_back(reg);
    }
}

void handleUnmacro(vector<regex> &unmacro) {
    //取消宏定义
    for (int i = 0; i < part.size(); i++) {
        regex reg("#undef " + part[i][1]);
        unmacro.push_back(reg);
    }
}

void handlePrams(vector<regex> &define_reg, vector<regex> &param_name, vector<regex> &param_reg) {
    for (int i = 0; i < define.size(); i++) {
        if (define[i].size() == 3) {
            regex reg("#define " + define[i][1]);
            define_reg.push_back(reg);
        } else {
            regex reg("#define " + define[i][0] + "\\(" + define[i][1] + "\\)");
            regex reg_(define[i][0] + "\\([^\\s:\\(\\)]+\\)");
            regex reg_name(define[i][0]);
            define_reg.push_back(reg);
            param_reg.push_back(reg_);
            param_name.push_back(reg_name);
        }
    }
}

void handleUndef(vector<string> &cpp, int i) {
    //处理#undef引起的标识符状态的改变
    if (regex_match(cpp[i], regex7)) {
        string str = split(cpp[i], ' ')[1];
        for (int t = 0; t < define.size(); t++) {
            if (define[t][1] == str || define[t][0] + "(" + define[t][1] + ")" == str) {
                cpp.erase(cpp.begin() + i);
                define[t][define[t].size() - 1] = "0";
                break;
            }
        }
        if (i > 0)
            i--;
    }
}

void handleIfdef(vector<string> &cpp, int i) {
    //处理#ifdef
    if (regex_search(cpp[i], regex9)) {
        string def = split(cpp[i], ' ')[1];
        for (int j = 0; j < define.size(); j++) {
            if (define[j][1] == def && define[j][2] == "1") {
                //匹配到相应的#endif，保留执行中间的代码，去掉外包的#ifdef和#endif
                vector<int> result = handler(i, cpp);
                if (result[0] == 0) {
                    cpp.erase(cpp.begin() + i);
                    cpp.erase(cpp.begin() + result[1] - 1);
                    break;
                } else {
                    cpp.erase(cpp.begin() + result[0], cpp.begin() + result[1] + 1);
                    cpp.erase(cpp.begin() + i);
                    break;
                }
            } else if ((define[j][1] == def && define[j][2] == "0") ||
                       (j == define.size() - 1 && define[j][1] != def)) {
                //查找有没有相应的else，如果有则执行else的内容，若没有则匹配到相应的#endif，跳过这段代码
                vector<int> result = handler(i, cpp);
                if (result[0] != 0) {
                    cpp.erase(cpp.begin() + result[1]);
                    cpp.erase(cpp.begin() + i, cpp.begin() + result[0] + 1);
                    break;
                } else {
                    cpp.erase(cpp.begin() + i, cpp.begin() + result[1] + 1);
                    break;
                }
            }
        }
        //将ifdef去掉后应保持i的位置
        if (i > 0)
            i--;
    }
}

void handleIfndef(vector<string> &cpp, int i) {
    //处理#ifndef
    if (regex_search(cpp[i], regex5)) {
        string def = split(cpp[i], ' ')[1];
        for (int j = 0; j < define.size(); j++) {
            if ((define[j][1] == def && define[j][2] == "0") ||
                (j == define.size() - 1 && def != define[j][1])) {
                vector<int> result = handler(i, cpp);
                if (result[0] == 0) {
                    cpp.erase(cpp.begin() + i);
                    cpp.erase(cpp.begin() + result[1] - 1);
                    break;
                } else {
                    cpp.erase(cpp.begin() + i);
                    cpp.erase(cpp.begin() + result[0], cpp.begin() + result[1] + 1);
                    break;
                }
            } else if (define[j][1] == def && define[j][2] == "1") {
                vector<int> result = handler(i, cpp);
                if (result[0] != 0) {
                    cpp.erase(cpp.begin() + result[1]);
                    cpp.erase(cpp.begin() + i, cpp.begin() + result[0] + 1);
                    break;
                } else {
                    cpp.erase(cpp.begin() + i, cpp.begin() + result[1] + 1);
                    break;
                }
            }
        }
        if (i > 0)
            i--;
    }
}

void handleIf(vector<string> &cpp, int i) {
    //处理#if
    if (regex_search(cpp[i], regex8)) {
        string str = split(cpp[i], ' ')[1];
        for (int m = 0; m < part.size(); m++) {
            if (str == part[m][1] && (part[m][2] == "0" || part[m][2] == "1")) {
                str = part[m][2];
            }
        }
        if (str == "1") {
            vector<int> result = handler(i, cpp);
            if (result[0] == 0) {
                cpp.erase(cpp.begin() + result[1]);
                cpp.erase(cpp.begin() + i);
            } else {
                cpp.erase(cpp.begin() + result[0], cpp.begin() + result[1] + 1);
                cpp.erase(cpp.begin() + i);
            }
        } else {
            vector<int> result = handler(i, cpp);
            if (result[0] == 0) {
                cpp.erase(cpp.begin() + i, cpp.begin() + result[1] + 1);
            } else {
                cpp.erase(cpp.begin() + result[1]);
                cpp.erase(cpp.begin() + i, cpp.begin() + result[0] + 1);
            }
        }
        if (i > 0)
            i--;
    }
}

void handleMacroWithPrams(vector<string> &cpp, int i, vector<regex> &param_name) {
    //处理带参数的宏
    int flag = 1;
    for (int p = 0; p < param_name.size(); p++) {
        for (int q = 0; q < define.size(); q++) {
            if (regex_search(define[q][0], param_name[p]) && define[q][define[q].size() - 1] == "1") {
                if (regex_search(cpp[i], param_name[p])) {
                    int position_start = cpp[i].find(param[p][0]) + param[p][0].length();
                    //匹配对应的）
                    int leftCount = 0;
                    int rightCount = 0;
                    int position_end = position_start;
                    for (int g = position_start; g < cpp[i].size(); g++) {
                        if (cpp[i][g] == '(') {
                            leftCount++;
                        }
                        if (cpp[i][g] == ')') {
                            rightCount++;
                        }
                        if (leftCount == rightCount && leftCount != 0) {
                            position_end = g;
                            break;
                        }
                    }
                    string realparam = cpp[i].substr(position_start + 1, position_end - position_start - 1);
                    cpp[i].replace(cpp[i].find(param[p][0]), position_end - cpp[i].find(param[p][0]) + 1,
                                   param[p][2]);
                    //处理#运算符
                    if (cpp[i][cpp[i].find(param[p][1]) - 1] == '#') {
                        realparam = "\"" + realparam + "\"";
                        cpp[i].replace(cpp[i].find(param[p][1]) - 1, param[p][1].length() + 1, realparam);
                    } else {
                        cpp[i].replace(cpp[i].find(param[p][1]), param[p][1].length(), realparam);
                    }
                    if (flag == 1) {
                        i--;
                        flag--;
                    }
                }
            }
        }
    }

}

void handleDef(vector<string> &cpp, int i, vector<regex> &define_reg) {
    if (regex_search(cpp[i], regex2)) {
        for (int j = 0; j < define_reg.size(); j++) {
            //将标识符的状态设置为1，表示标识符开始起作用
            if (regex_search(cpp[i], define_reg[j])) {
                cpp.erase(cpp.begin() + i);
                if (i > 0)
                    i--;
                define[j][define[j].size() - 1] = "1";
                continue;
            }
        }
    }
}

void doTheReplacement(vector<string> &cpp, vector<regex> &macro, vector<regex> &unmacro) {
    //进行宏替换
    for (int i = 0; i < cpp.size(); i++) {
        for (int j = 0; j < macro.size(); j++) {
            if (regex_match(cpp[i], regex3))
                continue;
            if (regex_match(cpp[i], unmacro[j])) {
                part.erase(part.begin() + j * 3, part.begin() + (j + 1) * 3);
                macro.erase(macro.begin() + j);
                continue;
            }
            if (regex_search(cpp[i], macro[j])) {
                cpp[i] = regex_replace(cpp[i], macro[j], " " + part[j][2]);
            }
        }
    }
}


//int main(int argc, char **argv) {

int main() {
    string argv[3];
    argv[1] = "demo.cpp";
    argv[2] = "out2.cpp";
    string inPath = (string) (argv[1]);
    string outPath = (string) (argv[2]);

    vector<string> cpp;
    readFromFile(cpp, inPath);

    string line;
    for (int i = 0; i < cpp.size(); i++) {
        //从自定义头文件原封不动copy到outfile中
        // 处理include
        handleInclude(cpp, i);
        //处理宏定义，存入vector<vector<string> > define
        handleDefine(cpp, i);
    }
    initDefineFlag();

    //链式替换（优化）
    for (int i = 0; i < part.size(); i++) {
        for (int j = 0; j < part.size(); j++) {
            if (part[i][2] == part[j][1]) {
                part[i][2] = part[j][2];
            }
        }
    }

    vector<regex> macro;
    handleMacro(macro);
    //取消宏定义
    vector<regex> unmacro;
    handleUnmacro(unmacro);

    vector<regex> define_reg;
    vector<regex> param_name;
    vector<regex> param_reg;
    handlePrams(define_reg, param_name, param_reg);


    //处理各种预编译符
    for (int i = 0; i < cpp.size(); i++) {
        handleDef(cpp, i, define_reg);
        handleUndef(cpp, i);
        handleIfdef(cpp, i);
        handleIfndef(cpp, i);
        handleIf(cpp, i);
        handleMacroWithPrams(cpp, i, param_name);
    }
    doTheReplacement(cpp, macro, unmacro);

    writeOutFile(outPath, cpp);
    return 0;
}
