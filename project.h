#ifndef PROJECT_H
#define PROJECT_H
#include <bits/stdc++.h>
using namespace std;
enum class Token {
    EQUAL,          // =
    ASTERISK,       // *
    LPAREN,         // (
    RPAREN,         // )
    SEMICOLON,      // ;
    CREATE,
    USE,
    INSERT,
    DROP,
    SELECT,
    INNER_JOIN,
    UPDATE,
    SET,
    DELETE,
    DATABASE,
    TABLE,
    INTO,
    ON,
    FROM,
    WHERE,
    POINT,
    VALUES,         // VALUES 关键字
    IDENTIFIER,     // 标识符
    COMMA,          // ,
    NUMBER,         // 数字（整数或浮点数）
    STRING,          // 字符串常量
    GT,
    LT
};

// 列的定义
class Column {
public:
    string name;        // 列名
    string type;        // 列的数据类型
    bool is_primary_key = false;  // 是否是主键
    bool is_not_null = false;     // 是否为 NOT NULL

    Column(string name, string type)
        : name(name), type(type) {}

    // 设置主键约束
    void set_primary_key() {
        is_primary_key = true;
    }

    // 设置 NOT NULL 约束
    void set_not_null() {
        is_not_null = true;
    }

    // 输出列信息
    // void print() const {
    //     cout << name << " " << type;
    //     if (is_not_null) cout << " NOT NULL";
    //     if (is_primary_key) cout << " PRIMARY KEY";
    //     cout << endl;
    // }
};

// 表的定义
class Table {
public:
    string name;               // 表名
    vector<Column> columns;    // 列
    vector<vector<string>> data; // 数据
    Table() : name("") {}
    Table(string name) : name(name) {}

    // 添加列
    // void add_column(const string& name, const string& type) {
    //     columns.push_back(Column(name, type));
    // }

    // 输出表的结构
    // void print() const {
    //     cout << "CREATE TABLE " << name << " (" << endl;
    //     for (size_t i = 0; i < columns.size(); ++i) {
    //         columns[i].print();
    //         if (i < columns.size() - 1) cout << ", ";
    //     }
    //     cout << ");" << endl;
    // }
};

class Database {
    public:
        string name;
        unordered_map<string, Table> tables;
};



extern unordered_map<string, Token> token_map;
extern string token_to_string(Token token);

struct TokenWithValue {
    Token token;
    string value;
    
    TokenWithValue(Token t, const string& v) : token(t), value(v) {}
};

bool is_number(const string& s);
bool is_string_literal(const string& s);
vector<TokenWithValue> lex(const string& input);
vector<vector<TokenWithValue>> lexfile(const string& filename);

#endif