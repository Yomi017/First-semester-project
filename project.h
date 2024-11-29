#ifndef PROJECT_H
#define PROJECT_H
#include <bits/stdc++.h>
using namespace std;
extern int colnum;
extern int select_num;
extern ofstream out;
extern bool is_number(const string& s);
extern bool is_string_literal(const string& s);

extern vector<string> column_Name;
extern string symbol;

enum class Token {
    EQUAL,          // =
    ASTERISK,       // *
    LPAREN,         // (
    RPAREN,         // )
    SEMICOLON,      // ;
    CREATE,         // CREATE
    USE,            // USE
    INSERT,         // INSERT
    DROP,
    SELECT,
    INNER,
    JOIN,
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
    STRING,         // 字符串常量
    GT,             // >
    LT,             // <
    AND,            // AND
    OR,             // OR
    PLUS,           // +
    MINUS,          // -
    DIVIDE,         // ÷
    MULTIPLY,       // ×
    NOT,            // !
};

// 列的定义
class Column {
public:
    string name;        // 列名
    string type;        // 列的数据类型
    bool is_primary_key = false;
    bool is_not_null = false;

    Column(string name, string type)
        : name(name), type(type) {}

    // 设置约束
    void set_primary_key() {
        is_primary_key = true;
    }

    void set_not_null() {
        is_not_null = true;
    }
};

// 表的定义
class Table {
public:
    string name;               // 表名
    vector<Column> columns;    // 列
    vector<vector<string>> data; // 数据
    Table() : name("") {}
    Table(string name) : name(name) {}
};

class Database {
    public:
        string name;
        unordered_map<string, Table> tables;
};

struct TokenWithValue {
    Token token;
    string value;
    
    TokenWithValue(Token t, const string& v) : token(t), value(v) {}
};

extern unordered_map<string, Database> databases;
extern Database* current_database;
extern vector<TokenWithValue> lex(const string& input);
extern vector<vector<TokenWithValue>> lexfile(const string& filename);

void create_database(const string& db_name);
void use_database(const string& db_name);
void create_table(const string& table_name, vector<TokenWithValue>::const_iterator& it, vector<TokenWithValue>::const_iterator end);

void select_data(vector<TokenWithValue>::const_iterator& it, vector<TokenWithValue>::const_iterator end);
void identifier_select(const string& column_name, vector<TokenWithValue>::const_iterator& it, vector<TokenWithValue>::const_iterator end);
void asterisk_select(vector<TokenWithValue>::const_iterator& it, vector<TokenWithValue>::const_iterator end);
void where_select(vector<string>& column_Name, vector<TokenWithValue>::const_iterator& it, vector<TokenWithValue>::const_iterator end, Table& table, Table& table1);
void innerjoin(vector<TokenWithValue>::const_iterator& it, vector<TokenWithValue>::const_iterator end, string& table_name1, string& column1_name1, string& table_name2, string& column2_name1, Table& table1, Table& table2);
void inner_helper(Table& table1, Table& table2, const string& column1_name1, const string& column1_name2, const string& column2_name1, const string& column2_name2);
void INNERJOIN(vector<TokenWithValue>::const_iterator& it, vector<TokenWithValue>::const_iterator end);
bool is_number_where(const string& s);

void asterisk_delete(vector<TokenWithValue>::const_iterator& it, vector<TokenWithValue>::const_iterator end);
void identifier_delete(const string& column_name, vector<TokenWithValue>::const_iterator& it, vector<TokenWithValue>::const_iterator end);
void where_delete(vector<string>& column_Name, vector<TokenWithValue>::const_iterator& it, vector<TokenWithValue>::const_iterator end, Table& table);
void delete_data(vector<TokenWithValue>::const_iterator& it, vector<TokenWithValue>::const_iterator end);

void update_helper(Table& table, const string& column_name, const string& expression, const string& condition_column, const string& op, const string& value, int digit_or_identifier);
void update_data(vector<TokenWithValue>::const_iterator& it, vector<TokenWithValue>::const_iterator end);
double evaluate(const string& expression, const map<string, double>& variables);
int getDecimalPlaces(const string& s);

void execute_query(const vector<TokenWithValue>& tokens);

bool is_number(const string& s);
bool is_string_literal(const string& s);
bool is_number_where(const string& s);
void initialize_output_file(string filename);
void close_output_file();
extern unordered_map<string, Token> token_map;
string token_to_string(Token token);
#endif