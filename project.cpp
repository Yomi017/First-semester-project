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

void select_data(vector<TokenWithValue>::const_iterator& it, vector<TokenWithValue>::const_iterator end);
unordered_map<string, Database> databases;
Database* current_database = nullptr;
vector<string> column_Name;
string symbol;

// 数据库
void create_database(const string& db_name) {
    if (databases.find(db_name) != databases.end()) {
        cerr << "Database " << db_name << " already exists.\n";
        return;
    }
    // cout << "Database " << db_name << " created.\n";
    databases[db_name] = Database{db_name};
    // cout << "Database " << db_name << " created.\n";
}

// 进入数据库
void use_database(const string& db_name) {
    if (databases.find(db_name) == databases.end()) {
        cerr << "ERROR! Database " << db_name << " does not exist.\n";
        return;
    }
    current_database = &databases[db_name];
    // cout << "Using database " << db_name << ".\n";
}

// 创建表
void create_table(const string& table_name, vector<TokenWithValue>::const_iterator& it, vector<TokenWithValue>::const_iterator end) {
    if (current_database == nullptr) {
        cerr << "ERROR! No database selected.\n";
        return;
    }

    // 创建表结构
    Table table(table_name);
    
    if (it == end || it->token != Token::LPAREN) {
        cerr << "ERROR! Expected '(' after table name.\n";
        return;
    }
    ++it; 

  
    while (it != end && it->token != Token::RPAREN) {
        if (it->token == Token::IDENTIFIER) {
            string column_name = it->value;
            ++it; 
            if (it == end || it->token != Token::IDENTIFIER) {
                cerr << "ERROR! Expected column type after column name.\n";
                return;
            }
            string column_type = it->value;
            table.columns.push_back({column_name, column_type});
            ++it; 
        }
        if (it != end && it->token == Token::COMMA) {
            ++it; 
        }
    }

    if (it == end || it->token != Token::RPAREN) {
        cerr << "ERROR! Expected ')' after column definitions.\n";
        return;
    }

    ++it;

    current_database->tables[table_name] = table;
    // cout << "Table " << table_name << " created.\n";
}

// 选择所有列
void asterisk_select(vector<TokenWithValue>::const_iterator& it, vector<TokenWithValue>::const_iterator end) {
    if (it != end && it->token == Token::FROM) {
        ++it;
        if (it != end && it->token == Token::IDENTIFIER) {
            string table_name = it->value;
            if (current_database == nullptr) {
                cerr << "ERROR! No database selected.\n";
                return;
            }
            if (current_database->tables.find(table_name) == current_database->tables.end()) {
                cerr << "ERROR! Table " << table_name << " does not exist.\n";
                return;
            }
            Table& table = current_database->tables[table_name];
            for (const auto& column : table.columns) {
                cout << column.name << ",";
            }
            cout << endl;
            for (const auto& row : table.data) {
                for (const auto& value : row) {
                    cout << value << ",";
                }
                cout << endl;
            }
        }
    }
}

// 选择条件的数据
bool is_number_where(const string& s) {
    try {
        stof(s);  // 如果能够转换为浮动类型，说明是数字
        return true;
    } catch (const std::invalid_argument&) {
        return false;  // 无效的数字格式
    } catch (const std::out_of_range&) {
        return false;  // 超出范围
    }
}
void where_select(const string& column_name, vector<TokenWithValue>::const_iterator& it, vector<TokenWithValue>::const_iterator end, Table& table) {
    string symbol;
    string value;

    // 确保我们在 token 列表的正确位置
    if (it != end && it->token == Token::IDENTIFIER) {
        string column_name = it->value;  // 获取要筛选的列名
        ++it;

        // 检查操作符：=，<，或>
        if (it != end && (it->token == Token::EQUAL || it->token == Token::LT || it->token == Token::GT)) {
            if (it->token == Token::EQUAL) {
                symbol = "=";
            } else if (it->token == Token::LT) {
                symbol = "<";
            } else if (it->token == Token::GT) {
                symbol = ">";
            }
            ++it;

            // 获取待比较的值
            if (it != end && (it->token == Token::STRING || it->token == Token::NUMBER)) {
                value = it->value;
            }
        }
    }

    // 现在我们有了列名、操作符和要比较的值，过滤行
    for (const auto& row : table.data) {
        int col_index = -1;

        // 找到表中对应列的索引
        for (int i = 0; i < table.columns.size(); ++i) {
            if (table.columns[i].name == column_name) {
                col_index = i;
                break;
            }
        }

        if (col_index != -1) {  // 如果该列存在
            string row_value = row[col_index];
            bool match = false;

            // 数字比较：如果 row_value 和 value 都是数字
            if (is_number_where(row_value) && is_number_where(value)) {  
                // 将 row_value 和 value 转换为 float 进行比较
                float row_val = stof(row_value);
                float compare_val = stof(value);

                if (symbol == "=") {
                    match = (row_val == compare_val);
                } else if (symbol == "<") {
                    match = (row_val < compare_val);
                } else if (symbol == ">") {
                    match = (row_val > compare_val);
                }
            } 
            // 字符串比较：如果 value 不是数字，直接按字符串比较
            else {
                if (symbol == "=") {
                    match = (row_value == value);
                } else if (symbol == "<") {
                    match = (row_value < value);
                } else if (symbol == ">") {
                    match = (row_value > value);
                }
            }

            // 如果符合条件，输出该行
            if (match) {
                for (const auto& column : table.columns) {
                    int col_index = -1;
                    for (int i = 0; i < table.columns.size(); ++i) {
                        if (table.columns[i].name == column.name) {
                            col_index = i;
                            break;
                        }
                    }
                    if (col_index != -1) {
                        cout << row[col_index] << " ";
                    }
                }
                cout << endl;
            }
        }
    }
}

// 选择特定一列或几列
void identifier_select(const string& column_name, vector<TokenWithValue>::const_iterator& it, vector<TokenWithValue>::const_iterator end) {
    if (it != end && it->token == Token::COMMA) {
        cout << column_name << ",";
        ++it;
        select_data(it, end);
    } else if (it != end && it->token == Token::FROM && column_Name.size() > 1) {
        cout << column_name << endl;
        ++it;
        if (it != end && it->token == Token::IDENTIFIER) {
            string table_name = it->value;
            if (current_database == nullptr) {
                cerr << "ERROR! No database selected.\n";
                return;
            }
            if (current_database->tables.find(table_name) == current_database->tables.end()) {
                cerr << "ERROR! Table " << table_name << " does not exist.\n";
                return;
            }
            ++it;
            if (it != end && it->token == Token::WHERE) {
                Table& table = current_database->tables[table_name];
                where_select(column_name, it, end, table);
            }else {
            Table& table = current_database->tables[table_name];
            for (const auto& row : table.data) {
                for (size_t i = 0; i < column_Name.size(); ++i) {
                    for (size_t j = 0; j < table.columns.size(); ++j) {
                        if (column_Name[i] == table.columns[j].name) {
                            cout << row[j];
                            if (i < column_Name.size() - 1) {
                                cout << ",";
                            }
                        }
                    }
                }
                cout << endl;
            }
        }
            column_Name.clear();
        }
    }

}

// 选择数据
void select_data(vector<TokenWithValue>::const_iterator& it, vector<TokenWithValue>::const_iterator end) {
    if (it != end && it->token == Token::ASTERISK) {
        ++it;
        asterisk_select(it, end);
    } else if (it != end && it->token == Token::IDENTIFIER){
        string column_name = it->value;
        ++it;
        column_Name.push_back(column_name);
        identifier_select(column_name, it, end);
    }
}

// 执行 SQL 命令
void execute_query(const vector<TokenWithValue>& tokens) {
    auto it = tokens.begin();
    if (it != tokens.end() && it->token == Token::CREATE){ // 创建数据库
        ++it;
        if (it != tokens.end() && it->token == Token::DATABASE) {
            ++it;
            if (it != tokens.end() && it->token == Token::IDENTIFIER) {
                string db_name = it->value;
                create_database(db_name);
            }
        } else if (it != tokens.end() && it->token == Token::TABLE) { // 创建表
            ++it;
            if (it != tokens.end() && it->token == Token::IDENTIFIER) {
                string table_name = it->value;
                create_table(table_name, ++it, tokens.end());
            }
        }
    } else if (it != tokens.end() && it->token == Token::USE) {
        ++it;
        if (it != tokens.end() && it->token == Token::DATABASE) {
            ++it;
            if (it != tokens.end() && it->token == Token::IDENTIFIER) {
                string db_name = it->value;
                use_database(db_name);}
        }
    } else if (it != tokens.end() && it->token == Token::DROP){ // 删除表
        ++it;
        if (it != tokens.end() && it->token == Token::TABLE) {
            ++it;
            if (it != tokens.end() && it->token == Token::IDENTIFIER) {
                string table_name = it->value;
                if (current_database == nullptr) {
                    cerr << "ERROR! No database selected.\n";
                    return;
                }
                if (current_database->tables.find(table_name) == current_database->tables.end()) {
                    cerr << "ERROR! Table " << table_name << " does not exist.\n";
                    return;
                }
                current_database->tables.erase(table_name);
                // cout << "Table " << table_name << " dropped.\n";
            }
        }
    } else if (it != tokens.end() && it->token == Token::INSERT) { // 插入数据
        ++it;
        if (it != tokens.end() && it->token == Token::INTO) {
            ++it;
            if (it != tokens.end() && it->token == Token::IDENTIFIER) {
                string table_name = it->value;
                if (current_database == nullptr) {
                    cerr << "ERROR! No database selected.\n";
                    return;
                }
                if (current_database->tables.find(table_name) == current_database->tables.end()) {
                    cerr << "ERROR! Table " << table_name << " does not exist.\n";
                    return;
                }
                Table& table = current_database->tables[table_name];
                ++it;
                if (it != tokens.end() && it->token == Token::VALUES) {
                    ++it;
                    if (it != tokens.end() && it->token == Token::LPAREN) {
                        ++it;
                        vector<string> values;
                        while (it != tokens.end() && it->token != Token::RPAREN) {
                            if (it->token == Token::STRING || it->token == Token::NUMBER) {
                                values.push_back(it->value);
                                ++it;
                            }
                            if (it != tokens.end() && it->token == Token::COMMA) {
                                ++it;
                            }
                        }
                        if (it != tokens.end() && it->token == Token::RPAREN) {
                            if (values.size() != table.columns.size()) {
                                cerr << "ERROR! Number of values does not match number of columns.\n";
                                return;
                            }
                            table.data.push_back(values);
                            // cout << "Inserted into " << table_name << ": ";
                            // for (const auto& value : values) {
                            //     cout << value << " ";
                            // }
                            // cout << endl;
                        }
                    }
                }
            }
        }
    } 
    else if (it != tokens.end() && it->token == Token::SELECT) { 
        ++it;
        select_data(it, tokens.end());
    }
}

int main() {
    vector<vector<TokenWithValue>> lex_output = lexfile("input.sql");
    // for (const auto& line_tokens : lex_output) {
    //     for (const auto& token : line_tokens) {
    //         cout << token_to_string(token.token) << " ";
    //     }
    //     cout << endl;
    // }
    for (const auto& line_tokens : lex_output) {
        execute_query(line_tokens);
    }
    // for (const auto& db : databases) {
    //     cout << "Database " << db.first << ":\n";
    //     for (const auto& table : db.second.tables) {
    //         cout << "Table " << table.first << ":\n";
    //         for (const auto& column : table.second.columns) {
    //             cout << column.name << " " << column.type << endl;
    //         }
    //         for (const auto& row : table.second.data) {
    //             for (const auto& value : row) {
    //                 cout << value << " ";
    //             }
    //             cout << endl;
    //         }
    //     }
    // }
    return 0;
}
