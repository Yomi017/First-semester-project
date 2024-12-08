#include <bits/stdc++.h>
#include "project.h"
using namespace std;
int colnum = 0;// 记录当前列数
int select_num = 0; // 记录当前select的次数, 用于输出时判断是否需要输出"---"
Database* current_database = nullptr;// 当前数据库指针, 指向选中的数据库
ofstream out;// 输出文件流

vector<string> column_Name;// 用于存储select语句中的列名
string symbol;// 用于存储where语句中的符号
unordered_map<string, Database> databases;// 所有数据库集合

// Select_inner_join语句的辅助变量
vector<string> tablename_columnname_select_out;// 用于存储select语句要输出的表名和列名
vector<vector<string>> tablename_columnname_innerjoin;
unordered_map<string, vector<string>> tablename_columnname_where;// 用于存储where语句要比较的表名和列名
vector<string> tablename;// 用于存储inner join语句中的表名
Table table_inner;// 用于存储inner join语句中的表
int inner_num = 0;// 用于存储inner join语句的次数
Table T; // 用于存储所有结果

// 清空表数据，以便重新插入数据
void initialize_output_file(string filename) {
    out.open(filename, ios::out | ios::trunc); // 以截断模式打开，清空文件
    if (!out.is_open()) {
        cerr << "ERROR! Unable to open output.csv for writing." << endl;
    }
}

// 关闭输出文件
void close_output_file() {
    if (out.is_open()) {
        out.close();
    }
}

// token 映射表, 关键字到 Token 的映射
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
    {"UPDATE", Token::UPDATE},
    {"SET", Token::SET},
    {"DELETE", Token::DELETE},
    {"DATABASE", Token::DATABASE},
    {"TABLE", Token::TABLE},
    {"INTO", Token::INTO},
    {"ON", Token::ON},
    {"FROM", Token::FROM},
    {"WHERE", Token::WHERE},
    {"VALUES", Token::VALUES},
    {"AND", Token::AND},
    {"OR", Token::OR},
    {"INNER", Token::INNER},
    {"JOIN", Token::JOIN},
    {"+", Token::PLUS},
    {"-", Token::MINUS},
    {"/", Token::DIVIDE},
    {"*", Token::MULTIPLY},
    {"!", Token::NOT},
};

// 输出Token用于调试
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
        case Token::INNER: return "Token::INNER";
        case Token::JOIN: return "Token::JOIN";
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
        case Token::AND: return "Token::AND";
        case Token::OR: return "Token::OR";
        case Token::PLUS: return "Token::PLUS";
        case Token::MINUS: return "Token::MINUS";
        case Token::DIVIDE: return "Token::DIVIDE";
        case Token::MULTIPLY: return "Token::MULTIPLY";
        default: return "Unknown Token";
    }
}

// 判断字符串是否是数字
bool is_number(const string& s) {
    return !s.empty() && all_of(s.begin(), s.end(), ::isdigit);
}

// 判断字符串是否是有效的字符串常量，即单引号括起来的字符串
bool is_string_literal(const string& s) {
    return !s.empty() && s.front() == '\'' && s.back() == '\'';  // 判断是否是单引号括起来的字符串
}

// 判断字符串是否是数字，并尝试转换，在 where 子句中使用
bool is_number_where(const std::string& s) {
    try {
        stod(s);  // 尝试将字符串转换为数字
        return true;
    } catch (const invalid_argument&) {
        return false;  // 如果不能转换为数字，返回 false
    } catch (const out_of_range&) {
        return false;  // 如果超出范围，返回 false
    }
}

// 执行主函数
int main() {
    // 读取 store.sql 文件，恢复数据库状态
    vector<vector<TokenWithValue>> lex_store = lexfile("store.sql");

    // for (const auto& line_tokens : lex_store) {
    //     for (const auto& token : line_tokens) {
    //         cout << token_to_string(token.token) << " ";
    //     }
    //     cout << endl;
    // }  // 输出词法分析结果

    for (const auto& line_tokens : lex_store) {
        execute_query(line_tokens);
    }  // 执行 SQL 命令
    string input, output;
    // cin >> input >> output;
    input = "input.sql";
    output = "output.csv";
    vector<vector<TokenWithValue>> lex_output = lexfile(input);
    initialize_output_file(output);

    for (const auto& line_tokens : lex_output) {
        for (const auto& token : line_tokens) {
            cout << token_to_string(token.token) << " ";
        }
        cout << endl;
    }  // 输出词法分析结果

    for (const auto& line_tokens : lex_output) {
        ++colnum;
        execute_query(line_tokens);
    } // 执行 SQL 命令
    if (select_num != 0) {
        out << "---" << endl;
    }
    close_output_file();

    // 将数据库状态写入 store.txt
    ofstream store;
    // 截断文件，以便写入新的数据库状态
    store.open("store.sql", ios::trunc);
    if (store.is_open()) {
        for (const auto& db_pair : databases) {
            const Database& db = db_pair.second;
            store << "CREATE" << " " << "DATABASE " << db.name << ";" << endl;
            store << "USE" << " " << "DATABASE " << db.name << ";" << endl;
            for (const auto& table_pair : db.tables) {
                const Table& table = table_pair.second;
                store << "CREATE" << " " << "TABLE " << table.name << " (" << endl;
                for (const auto& column : table.columns) {
                    store << "  ";
                    store << column.name << " " << column.type;
                    if (&column != &table.columns.back()) {
                        store << ",";
                    }
                    store << endl;
                }
                store << ")" << ";" << endl;
                for (const auto& row : table.data) {
                    store << "INSERT" << " " << "INTO " << table.name << " VALUES (";
                    for (const auto& value : row) {
                        if (is_number_where(value)) {
                            store << value;
                        } else {
                            store << "'" << value << "'";
                        }
                        if (&value != &row.back()) {
                            store << ",";
                        }
                    }
                    store << ")" << ";" << endl;
                }
            }
        }
        store.close();
    } else {
        cerr << "ERROR! Unable to open store.sql for writing." << endl;
    }
    return 0;
}
