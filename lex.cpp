#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <cctype>
#include <algorithm>
#include "project.h"
using namespace std;
unordered_map<string, Token> token_map = {
    {"=", Token::EQUAL},
    {">", Token::GT},
    {"<", Token::LT},
    {"*", Token::ASTERISK},
    {"(", Token::LPAREN},
    {")", Token::RPAREN},
    {";", Token::SEMICOLON},
    {",", Token::COMMA},
    {".", Token::POINT},
    {"CREATE", Token::CREATE},
    {"USE", Token::USE},
    {"INSERT", Token::INSERT},
    {"DROP", Token::DROP},
    {"SELECT", Token::SELECT},
    {"INNER_JOIN", Token::INNER_JOIN},
    {"UPDATE", Token::UPDATE},
    {"SET", Token::SET},
    {"DELETE", Token::DELETE},
    {"DATABASE", Token::DATABASE},
    {"TABLE", Token::TABLE},
    {"INTO", Token::INTO},
    {"ON", Token::ON},
    {"FROM", Token::FROM},
    {"WHERE", Token::WHERE},
    {"VALUES", Token::VALUES}  // 新增 VALUES 关键字
};

string token_to_string(Token token) {
    switch (token) {
        case Token::EQUAL: return "Token::EQUAL";
        case Token::GT: return "Token::GT";
        case Token::LT: return "Token::LT";
        case Token::LPAREN: return "Token::LPAREN";
        case Token::RPAREN: return "Token::RPAREN";
        case Token::COMMA: return "Token::COMMA";
        case Token::POINT: return "Token::POINT";
        case Token::SEMICOLON: return "Token::SEMICOLON";
        case Token::ASTERISK: return "Token::ASTERISK";
        case Token::CREATE: return "Token::CREATE";
        case Token::USE: return "Token::USE";
        case Token::INSERT: return "Token::INSERT";
        case Token::DROP: return "Token::DROP";
        case Token::SELECT: return "Token::SELECT";
        case Token::INNER_JOIN: return "Token::INNER_JOIN";
        case Token::UPDATE: return "Token::UPDATE";
        case Token::SET: return "Token::SET";
        case Token::DELETE: return "Token::DELETE";
        case Token::DATABASE: return "Token::DATABASE";
        case Token::TABLE: return "Token::TABLE";
        case Token::INTO: return "Token::INTO";
        case Token::ON: return "Token::ON";
        case Token::FROM: return "Token::FROM";
        case Token::WHERE: return "Token::WHERE";
        case Token::VALUES: return "Token::VALUES";
        case Token::IDENTIFIER: return "Token::IDENTIFIER";
        case Token::NUMBER: return "Token::NUMBER";
        case Token::STRING: return "Token::STRING";
        default: return "Unknown Token";
    }
}

// 判断字符串是否是数字
bool is_number(const string& s) {
    return !s.empty() && all_of(s.begin(), s.end(), ::isdigit);
}

// 判断字符串是否是有效的字符串常量
bool is_string_literal(const string& s) {
    return !s.empty() && s.front() == '\'' && s.back() == '\'';  // 判断是否是单引号括起来的字符串
}

// 词法分析函数
vector<TokenWithValue> lex(const string& input,bool& inside_paren) {
    vector<TokenWithValue> result;
    const char* ip = input.c_str();  // ip 是指向字符串的字符指针
    string token;
      // 记录是否在括号内

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
            if (!inside_paren) {
                inside_paren = true;  // 标记进入括号
                result.push_back(TokenWithValue(Token::LPAREN, "("));
            }
            ++ip;
        }
        else if (*ip == ')') {
            if (inside_paren) {
                result.push_back(TokenWithValue(Token::RPAREN, ")"));
                inside_paren = false;  // 标记离开括号
            }
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


vector<vector<TokenWithValue>> lexfile(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "ERROR! Cannot open file " << filename << endl;
        return {};
    }
    bool inside_paren = false;  // 标记是否在括号内
    vector<vector<TokenWithValue>> all_tokens;  // 二维容器，存储每一行的解析结果
    string line;

    // 逐行读取文件
    while (getline(file, line)) {
        // 调用 Lex 函数解析每一行
        vector<TokenWithValue> tokens = lex(line,inside_paren);
        if ((inside_paren && line.find('(') == string::npos) || (!inside_paren && line.find(')') != string::npos && line.find('(') == string::npos)) {
            all_tokens.back().insert(all_tokens.back().end(), tokens.begin(), tokens.end());
        } else {
            all_tokens.push_back(tokens);
        }
    }
    file.close();
    return all_tokens;
}