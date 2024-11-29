#include <bits/stdc++.h>
#include "project.h"
using namespace std;

// 词法分析
// 词法分析器将输入的字符串分解为 token，每个 token 包含 token 类型和对应的值
vector<TokenWithValue> lex(const string& input) {
    vector<TokenWithValue> result;
    const char* ip = input.c_str();  // ip 是指向字符串的字符指针
    string token;

    while (*ip != '\0') {  // 当 ip 指向的字符不是字符串结尾时
        if (isspace(*ip)) {  // 跳过空白字符
            ++ip;
            continue;
        }

        // 如果是数字，识别为 Token::NUMBER
        if (isdigit(*ip)) {
            token.clear();
            while (isdigit(*ip) || *ip == '.') {  // 允许浮点数
                token.push_back(*ip);
                ++ip;
            }
            result.push_back(TokenWithValue(Token::NUMBER, token));
        }
        // 如果是有效的字符串常量
        else if (*ip == '\'') {  // 字符串常量以单引号开始
            token.clear();
            ++ip;  // 跳过开头的单引号
            while (*ip != '\'' && *ip != '\0') {  // 字符串结束于单引号
                token.push_back(*ip);
                ++ip;
            }
            if (*ip == '\'') {  // 跳过结尾的单引号
                ++ip;
            }
            result.push_back(TokenWithValue(Token::STRING, token));
        }
        // 处理括号符号
        else if (*ip == '(') {
            // if (!inside_paren) {
                // inside_paren = true;  // 标记进入括号
                result.push_back(TokenWithValue(Token::LPAREN, "("));
            // }
            ++ip;
        }
        else if (*ip == ')') {
            // if (inside_paren) {
                result.push_back(TokenWithValue(Token::RPAREN, ")"));
                // inside_paren = false;  // 标记离开括号
            // }
            ++ip;
        }
        // 处理逗号
        else if (*ip == ',') {
            result.push_back(TokenWithValue(Token::COMMA, ","));
            ++ip;
        }
        // 处理等号
        else if (*ip == '=') {
            result.push_back(TokenWithValue(Token::EQUAL, "="));
            ++ip;
        }
        // 处理分号
        else if (*ip == ';') {
            result.push_back(TokenWithValue(Token::SEMICOLON, ";"));
            ++ip;
        }
        // 处理星号
        else if (*ip == '*') {
            result.push_back(TokenWithValue(Token::ASTERISK, "*"));
            ++ip;
        }
        // 处理点号
        else if (*ip == '.') {
            result.push_back(TokenWithValue(Token::POINT, "."));
            ++ip;
        }
        // 检查 token 是否为已知的关键词或符号
        else if (token_map.find(string(1, *ip)) != token_map.end()) {
            token.clear();
            token.push_back(*ip);
            result.push_back(TokenWithValue(token_map[token], token));
            ++ip;
        }
        // 否则当作标识符处理
        else if (isalpha(*ip) || *ip == '_') {  // 标识符以字母或下划线开头
            token.clear();
            while (isalnum(*ip) || *ip == '_') {  // 标识符后可以包含字母、数字或下划线
                token.push_back(*ip);
                ++ip;
            }
            // 如果 token 是关键字，映射为对应的 token 类型
            if (token_map.find(token) != token_map.end()) {
                result.push_back(TokenWithValue(token_map[token], token));
            } else {
                result.push_back(TokenWithValue(Token::IDENTIFIER, token));
            }
        }
        else {
            ++ip;  // 跳过无法识别的字符
        }
    }

    return result;
}

// 词法分析文件
vector<vector<TokenWithValue>> lexfile(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "ERROR! Cannot open file " << filename << endl;
        return {};
    }

    vector<vector<TokenWithValue>> all_tokens;  // 二维容器，存储每一行的解析结果
    vector<TokenWithValue> current_line_tokens;  // 当前行的 token 容器
    string line;

    // 逐行读取文件
    while (getline(file, line)) {
        vector<TokenWithValue> tokens = lex(line);  // 调用 lex 函数解析每一行
        for (const auto& token : tokens) {
            current_line_tokens.push_back(token);  // 将当前 token 加入行容器
            if (token.token == Token::SEMICOLON) {  // 检测到分号
                all_tokens.push_back(current_line_tokens);  // 保存当前行
                current_line_tokens.clear();  // 开始新的行
            }
        }
    }

    // 如果最后还有未保存的 token，将其加入到 all_tokens 中
    if (!current_line_tokens.empty()) {
        all_tokens.push_back(current_line_tokens);
    }

    file.close();
    return all_tokens;
}
