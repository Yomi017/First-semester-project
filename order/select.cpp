#include <bits/stdc++.h>
#include "../project.h"
using namespace std;

// 选择数据
// 2. 选择所有数据
void asterisk_select(vector<TokenWithValue>::const_iterator& it, vector<TokenWithValue>::const_iterator end) {
    if (it != end && it->token == Token::FROM) {
        ++it;
        if (it != end && it->token == Token::IDENTIFIER) {
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
            if (select_num !=0 ){
                out << "---" << endl;
            }
            ++select_num;
            for (const auto& column : table.columns) {
                out << column.name;
                if (&column != &table.columns.back()) {
                    out << ",";
                }
            }
            out << endl;
            for (const auto& row : table.data) {
                for (const auto& value : row) {
                    out << value;
                    if (&value != &row.back()) {
                        out << ",";
                    }
                }
                out << endl;
            }
        } else {
            cerr << "ERROR! Expected table name after FROM." << "At column " << colnum << endl;
            return;
        }
    } else {
        cerr << "ERROR! Expected FROM after *." << "At column " << colnum << endl;
        return;
    }
}

// 3.选择特定的数据where子函数
void where_select(vector<string>& column_Name, vector<TokenWithValue>::const_iterator& it, vector<TokenWithValue>::const_iterator end, Table& table, Table& table1) {
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
                // 万恶之源
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
                for (size_t i = 0; i < table.data.size(); ++i) {
                    if (match[i] == true) {
                        const auto& row = table.data[i];
                        vector<string> new_row;
                        for (const auto& column : column_Name) {
                            auto it = find_if(table.columns.begin(), table.columns.end(), [&](const Column& col) {
                                return col.name == column;
                            });
                            if (it != table.columns.end()) {
                                size_t index = distance(table.columns.begin(), it);
                                new_row.push_back(row[index]);
                            }
                        }
                        table1.data.push_back(new_row);
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

// 3. innerjoin选择的执行函数
void inner_helper(Table& table1, Table& table2, const string& column1_name1, const string& column1_name2, const string& column2_name1, const string& column2_name2) {
    // 查找列索引
    int col1_index1 = -1, col1_index2 = -1, col2_index1 = -1, col2_index2 = -1;

    for (size_t i = 0; i < table1.columns.size(); ++i) {
        if (table1.columns[i].name == column1_name1) col1_index1 = i;
        if (table1.columns[i].name == column1_name2) col1_index2 = i;
    }

    for (size_t i = 0; i < table2.columns.size(); ++i) {
        if (table2.columns[i].name == column2_name1) col2_index1 = i;
        if (table2.columns[i].name == column2_name2) col2_index2 = i;
    }

    // 检查列是否找到
    if (col1_index1 == -1 || col1_index2 == -1 || col2_index1 == -1 || col2_index2 == -1) {
        cerr << "One or more columns not found!" << endl;
        return;
    }

    // 构建结果
    vector<vector<string>> result;

    // 建立表1的 column1_name2 -> column1_name1 映射
    unordered_map<string, string> table1_map;
    for (const auto& row : table1.data) {
        table1_map[row[col1_index2]] = row[col1_index1];  // 使用 column1_name2 作为 key, column1_name1 作为 value
    }

    // 遍历表2数据，匹配并构造结果
    for (const auto& row : table2.data) {
        const string& key = row[col2_index2];  // 获取 column2_name2 的值作为 key
        if (table1_map.find(key) != table1_map.end()) {
            vector<string> new_row = {table1_map[key], row[col2_index1]};  // 匹配后输出 column1_name1 和 column2_name1 的值
            result.push_back(new_row);
        } else {
            cerr << "No match found for " << key << endl;
        }
    }

    if (result.empty()) {
        cerr << "No result found." << endl;
        return;
    }

    if (select_num !=0 ){
        out << "---" << endl;
    }
    ++select_num;
    // 输出结果
    out << column1_name1 << ", " << column2_name1 << endl;
    for (const auto& row : result) {
        for (size_t i = 0; i < row.size(); ++i) {
            out << row[i];
            if (i < row.size() - 1) out << ", ";
        }
        out << endl;
    }
}

// 3. innerjoin选择part2
void innerjoin(vector<TokenWithValue>::const_iterator& it, vector<TokenWithValue>::const_iterator end, string& table_name1, string& column1_name1, string& table_name2, string& column2_name1, Table& table1, Table& table2) {
    if (it != end && it->token == Token::IDENTIFIER && it->value == table_name1) {
        ++it;
        if (it != end && it->token == Token::POINT) {
            ++it;
            if (it != end && it->token == Token::IDENTIFIER) {
                string column1_name2 = it->value;
                ++it;
                if (it != end && it->token == Token::EQUAL) {
                    ++it;
                    if (it != end && it->token == Token::IDENTIFIER && it->value == table_name2) {
                        ++it;
                        if (it != end && it->token == Token::POINT) {
                            ++it;
                            if (it != end && it->token == Token::IDENTIFIER) {
                                string column2_name2 = it->value;
                                inner_helper(table1, table2, column1_name1, column1_name2, column2_name1, column2_name2);
                            } else {
                                cerr << "ERROR! Expected column name after POINT. " << "At column " << colnum << endl;
                                return;
                            }
                        } else {
                            cerr << "ERROR! Expected POINT after table name. " << "At column " << colnum << endl;
                            return;
                        }
                    } else if (it != end && it->token == Token::IDENTIFIER && it->value != table_name2) {
                        cerr << "ERROR! The table name does not equal to the second table name. " << "At column " << colnum << endl;
                        return;
                    } else {
                        cerr << "ERROR! Expected table name after EQUAL. " << "At column " << colnum << endl;
                        return;
                    }
                } else {
                    cerr << "ERROR! Expected EQUAL after column name." << "At column " << colnum << endl;
                    return;
                }
            } else {
                cerr << "ERROR! Expected column name after POINT." << "At column " << colnum << endl;
                return;
            }
        } else {
            cerr << "ERROR! Expected POINT after table name." << "At column " << colnum << endl;
            return;
        }
    } else if (it != end && it->token == Token::IDENTIFIER && it->value != table_name1) {
        cerr << "ERROR! The table name does not equal to the first table name." << "At column " << colnum << endl;
        return;
    } else {
        cerr << "ERROR! Expected table name after ON." << "At column " << colnum << endl;
        return;}
}

// 3. innerjoin选择part1
void INNERJOIN(vector<TokenWithValue>::const_iterator& it, vector<TokenWithValue>::const_iterator end) {
    --it;
    if (it != end && it->token == Token::IDENTIFIER) {
        string table_name1 = it->value;
        Table& table1 = current_database->tables[table_name1];
        ++it;
        if (it != end && it->token == Token::POINT) {
            ++it;
            if (it != end && it->token == Token::IDENTIFIER) {
                string column1_name1 = it->value;
                ++it;
                if (it != end && it->token == Token::COMMA) {
                    ++it;
                    if (it != end && it->token == Token::IDENTIFIER){
                        string table_name2 = it->value;
                        Table& table2 = current_database->tables[table_name2];
                        ++it;
                        if (it != end && it->token == Token::POINT) {
                            ++it;
                            if (it !=end && it->token == Token::IDENTIFIER) {
                                string column2_name1 = it->value;
                                ++it;
                                if (it != end && it->token == Token::FROM){
                                    ++it;
                                    if (it != end && it->token == Token::IDENTIFIER) {
                                        ++it;
                                        if (it != end && it->token == Token::INNER) {
                                            ++it;
                                            if (it != end && it->token == Token::JOIN) {
                                                ++it;
                                                if (it != end && it->token == Token::IDENTIFIER) {
                                                    ++it;
                                                    if (it != end && it->token == Token::ON) {
                                                        ++it;
                                                        innerjoin(it, end, table_name1, column1_name1, table_name2, column2_name1, table1, table2);
                                                    } else {
                                                        cerr << "ERROR! Expected ON after JOIN. " << "At column " << colnum << endl;
                                                        return;
                                                    }
                                                }
                                            } else {
                                                cerr << "ERROR! Expected JOIN after INNER. " << "At column " << colnum << endl;
                                                return;
                                            }
                                        } else {
                                            cerr << "ERROR! Expected INNER after FROM. " << "At column " << colnum << endl;
                                            return;
                                        }
                                    } else {
                                        cerr << "ERROR! Expected table name after FROM. " << "At column " << colnum << endl;
                                        return;
                                    }
                                } else {
                                    cerr << "ERROR! Expected FROM after column name. " << "At column " << colnum << endl;
                                    return;
                                }
                            } else {
                                cerr << "ERROR! Expected column name after POINT. " << "At column " << colnum << endl;
                                return;
                            }
                        } else {
                            cerr << "ERROR! Expected POINT after table name. " << "At column " << colnum << endl;
                            return;
                        }
                    } else {
                        cerr << "ERROR! Expected table name after COMMA. " << "At column " << colnum << endl;
                        return;
                    }
                } else {
                    cerr << "ERROR! Expected COMMA after column name. " << "At column " << colnum << endl;
                    return;
                }
            } else {
                cerr << "ERROR! Expected column name after POINT. " << "At column " << colnum << endl;
                return;
            }
        } else {
            cerr << "ERROR! Expected POINT after table name. " << "At column " << colnum << endl;
            return;
        }
    } else {
        cerr << "ERROR! Expected table name after POINT. " << "At column " << colnum << endl;
        return;}
}

// 2. 选择特定的数据
void identifier_select(const string& column_name, vector<TokenWithValue>::const_iterator& it, vector<TokenWithValue>::const_iterator end) {
    if (it != end && it->token == Token::POINT) {
        INNERJOIN(it ,end);
    }
    if (it != end && it->token == Token::COMMA) {
        ++it;
        select_data(it, end);
    } else if (it != end && it->token == Token::FROM && column_Name.size() >= 1) {
        ++it;
        if (it != end && it->token == Token::IDENTIFIER) {
            string table_name = it->value;
            if (current_database == nullptr) {
                cerr << "ERROR! No database selected." << "At column " << colnum << endl;
                return;
            }
            if (current_database->tables.find(table_name) == current_database->tables.end()) {
                cerr << "ERROR! Table " << table_name << " does not exist." << "At column " << colnum << endl;
                return;
            }
            ++it;
            if (it != end && it->token == Token::WHERE) {
                Table& table = current_database->tables[table_name];
                Table table1;
                where_select(column_Name,it, end, table, table1);
                ++it;
                if (it != end && it->token == Token::AND) {
                    Table table2;
                    where_select(column_Name,it, end, table, table2);
                    for (const auto &column : column_Name) {
                        auto it = find_if(table.columns.begin(), table.columns.end(), [&](const Column& col) {
                            return col.name == column;
                        });
                        if (it == table.columns.end()) {
                            cerr << "ERROR! Column " << column << " does not exist in table " << table_name << "." << "At column " << colnum << endl;
                            return;
                        }
                        if (select_num !=0 ){
                            out << "---" << endl;
                        }
                        ++select_num;
                        out << column;
                        if (column != column_Name.back()) {
                            out << ",";
                        }
                    }
                    out << endl;
                    for (const auto& row1 : table1.data) {
                        for (const auto& row2 : table2.data) {
                            if (row1 == row2) {
                                for (const auto& value : row1) {
                                    out << value;
                                    if (&value != &row1.back()) {
                                        out << ",";
                                    }
                                }
                                out << endl;
                            }
                        }
                    }
                } else if (it != end && it->token == Token::OR) {
                    Table table2;
                    where_select(column_Name,it, end, table, table2);
                    for (const auto &column : column_Name) {
                        auto it = find_if(table.columns.begin(), table.columns.end(), [&](const Column& col) {
                            return col.name == column;
                        });
                        if (it == table.columns.end()) {
                            cerr << "ERROR! Column " << column << " does not exist in table " << table_name << "." << "At column " << colnum << endl;
                            return;
                        }
                        if (select_num !=0 ){
                        out << "---" << endl;
                        }
                        ++select_num;
                        out << column;
                        if (column != column_Name.back()) {
                            out << ",";
                        }
                    }
                    out << endl;
                    for (const auto& row : table1.data) {
                        for (const auto& value : row) {
                            out << value;
                            if (&value != &row.back()) {
                                out << ",";
                            }
                        }
                        out << endl;
                    }
                    for (const auto& row : table2.data) {
                        if (find(table1.data.begin(), table1.data.end(), row) == table1.data.end()) {
                            for (const auto& value : row) {
                                out << value;
                                if (&value != &row.back()) {
                                    out << ",";
                                }
                            }
                            out << endl;
                        }
                    }
                } else {
                    for (const auto &column : column_Name) {
                        auto it = find_if(table.columns.begin(), table.columns.end(), [&](const Column& col) {
                            return col.name == column;
                        });
                        if (it == table.columns.end()) {
                            cerr << "ERROR! Column " << column << " does not exist in table " << table_name << "." << "At column " << colnum << endl;
                            return;
                        }
                        if (select_num !=0 ){
                            out << "---" << endl;
                        }
                        ++select_num;
                        out << column;
                        if (column != column_Name.back()) {
                            out << ",";
                        }
                    }
                    out << endl;
                    for (const auto& row : table1.data) {
                        for (const auto& value : row) {
                            out << value;
                            if (&value != &row.back()) {
                                out << ",";
                            }
                        }
                        out << endl;
                    }
                }
            column_Name.clear();
            } else {
            Table& table = current_database->tables[table_name];
            for (const auto &column : column_Name) {
                auto it = find_if(table.columns.begin(), table.columns.end(), [&](const Column& col) {
                    return col.name == column;
                });
                if (it == table.columns.end()) {
                    cerr << "ERROR! Column " << column << " does not exist in table " << table_name << "." << "At column " << colnum << endl;
                    return;
                }
                if (select_num !=0 ){
                    out << "---" << endl;
                }
                ++select_num;
                out << column;
                if (column != column_Name.back()) {
                    out << ",";
                }
            }
            out << endl;
            for (const auto& row : table.data) {
                for (const auto& column : column_Name) {
                    auto it = find_if(table.columns.begin(), table.columns.end(), [&](const Column& col) {
                        return col.name == column;
                    });
                    if (it != table.columns.end()) {
                        size_t index = distance(table.columns.begin(), it);
                        out << row[index];
                        if (column != column_Name.back()) {
                            out << ",";}
                    }
                }
                out << endl;
            }
            column_Name.clear();
        }
        } else {
            cerr << "ERROR! Expected WHERE or ; after column name." << "At column " << colnum << endl;
            return;
        } 
    }
}

// 1. 选择数据主函数
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
