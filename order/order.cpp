#include <bits/stdc++.h>
#include "../project.h"
using namespace std;
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
                    cerr << "ERROR! No database selected." << "At column " << colnum << endl;
                    return;
                }
                if (current_database->tables.find(table_name) == current_database->tables.end()) {
                    cerr << "ERROR! Table " << table_name << " does not exist." << "At column " << colnum << endl;
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
                    cerr << "ERROR! No database selected." << "At column " << colnum << endl;
                    return;
                }
                if (current_database->tables.find(table_name) == current_database->tables.end()) {
                    cerr << "ERROR! Table " << table_name << " does not exist." << "At column " << colnum << endl;
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
                                cerr << "ERROR! Number of values does not match number of columns." << "At column " << colnum << endl;
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
    else if (it != tokens.end() && it->token == Token::SELECT) { // 查询数据
        ++it;
        select_data(it, tokens.end());
    } else if (it != tokens.end() && it->token == Token::UPDATE) { // 更新数据
        ++it;
        update_data(it, tokens.end());
    } else if (it != tokens.end() && it->token == Token::DELETE) { // 删除数据
        ++it;
        delete_data(it, tokens.end());
    } else { // 未知命令
        cerr << "ERROR! Unknown command." << "At column " << colnum << endl;
    }
}
