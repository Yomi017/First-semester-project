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
bool is_number_where(const std::string& s) {
    try {
        std::stod(s);  // 尝试将字符串转换为数字
        return true;
    } catch (const std::invalid_argument&) {
        return false;  // 如果不能转换为数字，返回 false
    } catch (const std::out_of_range&) {
        return false;  // 如果超出范围，返回 false
    }
}

void where_select(vector<string>& column_Name, vector<TokenWithValue>::const_iterator& it, vector<TokenWithValue>::const_iterator end, Table& table) {
    if (it != end && it->token == Token::IDENTIFIER) {
        string column_name = it->value;
        ++it;

        // 检查操作符
        if (it != end && it->token == Token::EQUAL) {
            ++it;
            if (it != end && (it->token == Token::STRING || it->token == Token::NUMBER)) {
                string value = it->value;
                ++it;

                for (size_t i = 0; i < table.columns.size(); ++i) {
                    if (column_name == table.columns[i].name) {  // 找到目标列
                        for (const auto& row : table.data) {
                            bool match = false;

                            // 进行比较，考虑数字与字符串的情况
                            if (is_number(value)) {
                                // 如果 value 是数字
                                double compare_val = stod(value);
                                double row_val = stod(row[i]);  // 将行中的数据转为数字进行比较
                                match = (row_val == compare_val);
                            } else {
                                // 如果 value 是字符串
                                match = (row[i] == value);
                            }

                            // 如果匹配，输出对应的列
                            if (match) {
                                for (size_t j = 0; j < column_Name.size(); ++j) {
                                    for (size_t k = 0; k < table.columns.size(); ++k) {
                                        if (column_Name[j] == table.columns[k].name) {
                                            cout << row[k];
                                            if (j < column_Name.size() - 1) {
                                                cout << ",";
                                            }
                                        }
                                    }
                                }
                                cout << endl;
                            }
                        }
                    }
                }
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
                where_select(column_Name,it, end, table);
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
    for (const auto& table : current_database->tables) {
        cout << "Table " << table.first << ":\n";
        for (const auto& column : table.second.columns) {
            cout << column.name << "," ;
        }
        cout << endl;
        for (const auto& row : table.second.data) {
            for (const auto& value : row) {
                cout << value << ",";
            }
            cout << endl;
        }
    }
    // for (const auto& db : databases) {
    //     cout << "Database " << db.first << ":\n";
    //     for (const auto& table : db.second.tables) {
    //         cout << "Table " << table.first << ":\n";
    //         for (const auto& column : table.second.columns) {
    //             cout << column.name << "," ;
    //         }
    //         for (const auto& row : table.second.data) {
    //             for (const auto& value : row) {
    //                 cout << value << ",";
    //             }
    //             cout << endl;
    //         }
    //     }
    // }
    return 0;
}
