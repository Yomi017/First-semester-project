#include <bits/stdc++.h>
#include "../project.h"
using namespace std;
void asterisk_delete(vector<TokenWithValue>::const_iterator& it, vector<TokenWithValue>::const_iterator end){
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
        table.data.clear();
        // cout << "All data from table " << table_name << " deleted.\n";
}

void where_delete(vector<TokenWithValue>::const_iterator& it, vector<TokenWithValue>::const_iterator end, Table& table, Table& table1) {
        ++it;
    if (it != end && it->token == Token::IDENTIFIER) {
        string column_name = it->value;
        ++it;
        // 检查操作符
        if (it != end && (it->token == Token::EQUAL || it->token == Token::GT || it->token == Token::LT || it->token == Token::NOT)) {
            string tp = it->value;
            if (it != end && it->token == Token::NOT) {
                ++it;
                if (it != end && it->token == Token::EQUAL) {
                    tp = "!=";
                } else {
                    cerr << "ERROR! Expected = after !." << "At column " << colnum << endl;
                    return;
                }
            }
            string op = tp;
            ++it;
            if (it != end && (it->token == Token::STRING || it->token == Token::NUMBER)) {
                string value = it->value;
                // try {
                //     std::stod(value);  // 尝试将字符串转换为数字
                // } catch (const std::invalid_argument&) {
                // } catch (const std::out_of_range&) {
                // }
                vector<bool> match(table.data.size());
                auto its = find_if(table.columns.begin(), table.columns.end(), [&](const Column& col) {
                        return col.name == column_name;
                    });
                for (const auto& target_row : table.data) {
                    size_t index = distance(table.columns.begin(), its);
                    if (its != table.columns.end()) {
                        if (op == "=") {
                            if (is_number_where(value)) {
                                    if (target_row[index] == value) {
                                        match[distance(table.data.begin(), find(table.data.begin(), table.data.end(), target_row))] = true;
                                }
                            } else {
                                if (target_row[index] == value) {
                                    match[distance(table.data.begin(), find(table.data.begin(), table.data.end(), target_row))] = true;
                                }
                            }
                        } else if (op == ">") {
                            if (is_number_where(value)) {
                                if (is_number_where(target_row[index])) {
                                    if (target_row[index] > value) {
                                        match[distance(table.data.begin(), find(table.data.begin(), table.data.end(), target_row))] = true;
                                    }
                                }
                            }
                        } else if (op == "<") {
                            if (is_number_where(value)) {
                                if (is_number_where(target_row[index])) {
                                    if (target_row[index] < value) {
                                        match[distance(table.data.begin(), find(table.data.begin(), table.data.end(), target_row))] = true;
                                    }
                                }
                            }
                        } else if (op == "!=") {
                            if (is_number_where(value)) {
                                if (is_number_where(target_row[index])) {
                                    if (target_row[index] != value) {
                                        match[distance(table.data.begin(), find(table.data.begin(), table.data.end(), target_row))] = true;
                                    }
                                }
                            }
                        }
                    }
                }
                for (size_t i = table.data.size(); i-- > 0;) {
                    if (match[i] == true) {
                        table.data.erase(table.data.begin() + i);
                    }
                }
            } else {
                cerr << "ERROR! Expected value after operator." << "At column " << colnum << endl;
                return;
            }
        } else {
            cerr << "ERROR! Expected operator after column name." << "At column " << colnum << endl;
            return;
        }
    } else {
        cerr << "ERROR! Expected column name after WHERE." << "At column " << colnum << endl;
        return;
    }
}

void identifier_delete(const string& table_name, vector<TokenWithValue>::const_iterator& it, vector<TokenWithValue>::const_iterator end){
    ++it;
    if (it != end && it->token == Token::WHERE){
        Table& table = current_database->tables[table_name];
        Table table1;
        where_delete(it, end, table, table1);
        ++it;
        } else if (it != end && it->token == Token::SEMICOLON) {
            --it;
            asterisk_delete(it, end);
        } else {
            cerr << "ERROR! Expected WHERE or ; after column name." << "At column " << colnum << endl;
            return;
        }
}

void delete_data(vector<TokenWithValue>::const_iterator& it, vector<TokenWithValue>::const_iterator end){
        if (it != end && it->token == Token::FROM) {
            ++it;
            if (it != end && it->token == Token::IDENTIFIER) {
                 string table_name = it->value;
                identifier_delete(table_name, it, end);
        } else {
            cerr << "ERROR! Expected FROM after DELETE." << "At column " << colnum << endl;
            return;
        }
    }
}
