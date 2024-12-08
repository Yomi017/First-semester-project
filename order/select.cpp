#include <bits/stdc++.h>
#include "../project.h"
using namespace std;

void is_float(string s) {
    if (s.find('.') != string::npos) {
        out << fixed << setprecision(2) << stod(s);
    } else {
        out << s;
    }
}

// 选择数据
void out_inner_join() {
    if (select_num != 0) {
        out << "---" << endl;
    }
    ++select_num;
    for (const auto& col : tablename_columnname_select_out) {
        out << col;
        if (col != tablename_columnname_select_out.back()) {
            out << ",";
        }
    }
    out << endl;
    for (const auto& row : T.data) {
        for (const auto& col : tablename_columnname_select_out) {
            auto it = find_if(T.columns.begin(), T.columns.end(), [&](const Column& column) {
                return column.name == col;
            });
            if (it != T.columns.end()) {
                size_t index = distance(T.columns.begin(), it);
                if (is_number_where(row[index])) {
                    is_float(row[index]);
                } else {
                    out << "'" << row[index] << "'";
                }
                if (col != tablename_columnname_select_out.back()) {
                    out << ",";
                }
            }
        }
        out << endl;
    }
}

void inner_helper_f() {
    vector<Table> tables;
    for (const auto& tname : tablename) {
        Table t = current_database->tables[tname];
        tables.push_back(t);
    }
    // 建立ON的映射，并将两个表能匹配的数据存入T中
    // 先将第1个表的所有数据存入T中
    vector<Column> col_row;
    // 以table.columns的形式存储column
    Table t = tables[0];
    for (auto& col : t.columns) {
        col.name = tablename[0] + "." + col.name;
        col_row.push_back(col);
    }
    T.columns = col_row;
    // 将第1个表的数据存入T中
    for (const auto& row : t.data) {
        T.data.push_back(row);
    }
    // 输出T
    // cout << "T: " << endl;
    // for (const auto& col : T.columns) {
    //     cout << col.name << " ";
    // }
    // cout << endl;
    // for (const auto& row : T.data) {
    //     for (const auto& value : row) {
    //         cout << value << " ";
    //     }
    //     cout << endl;
    // }
    // cout << tablename_columnname_innerjoin.size() << endl;
    // 循环匹配剩下的表，将匹配的数据存入T中，删除前面不匹配的数据
    for (int id = 0; id < tablename_columnname_innerjoin.size(); ++id){
        t = tables[id + 1];
        col_row.clear();
        Table temp;
        for (auto& col : t.columns) {
            col.name = tablename[id + 1] + "." + col.name;
            col_row.push_back(col);
        }
        temp.columns = col_row;
        // 将第1个表的数据存入T中
        for (const auto& row : t.data) {
            temp.data.push_back(row);
        }
        vector<size_t> index(2);
        index[0] = distance(T.columns.begin(), find_if(T.columns.begin(), T.columns.end(), [&](const Column& col) {
            return col.name == tablename_columnname_innerjoin[id][0];
        }));
        index[1] = distance(temp.columns.begin(), find_if(temp.columns.begin(), temp.columns.end(), [&](const Column& col) {
            return col.name == tablename_columnname_innerjoin[id][1];
        }));
        if (index[0] == T.columns.size() || index[1] == temp.columns.size()) {
            cerr << "ERROR! Column does not exist in table " << "." << "At column " << colnum << endl;
            return;
        }
        vector<vector<string>> data_temp;
        for (const auto& target_row : T.data) {
            for (const auto& row : temp.data) {
                if (target_row[index[0]] == row[index[1]]) {
                    vector<string> new_row;
                    new_row.insert(new_row.end(), target_row.begin(), target_row.end());
                    new_row.insert(new_row.end(), row.begin(), row.end());
                    data_temp.push_back(new_row);
                }
            }
        }
        T.data = data_temp;

        col_row.clear();
        // 合并列名
        for (auto& col : t.columns) {
            col_row.push_back(col);
        }

        // 将新列名加入 T.columns
        T.columns.insert(T.columns.end(), col_row.begin(), col_row.end());

        // 输出T
        // cout << "T: " << endl;
        // for (const auto& col : T.columns) {
        //     cout << col.name << " ";
        // }
        // cout << endl;
        // for (const auto& row : T.data) {
        //     for (const auto& value : row) {
        //         cout << value << " ";
        //     }
        //     cout << endl;
        // }
    }
}

void inner(vector<TokenWithValue>::const_iterator& it, vector<TokenWithValue>::const_iterator end) {
    if (it != end && it->token == Token::INNER) {
        ++it;
        if (it != end && it->token == Token::JOIN) {
            ++it;
            if (it != end && it->token == Token::IDENTIFIER) {
                if (current_database == nullptr) {
                    cerr << "ERROR! No database selected." << "At column " << colnum << endl;
                    return;
                }
                if (current_database->tables.find(it->value) == current_database->tables.end()) {
                    cerr << "ERROR! Table " << it->value << " does not exist." << "At column " << colnum << endl;
                    return;
                }
                if (find(tablename.begin(), tablename.end(), it->value) == tablename.end()) {
                    tablename.push_back(it->value);
                }
                ++it;
                if (it != end && it->token == Token::ON) {
                    ++it;
                    if (it != end && it->token == Token::IDENTIFIER) {
                        string tname = it->value;
                        if (current_database == nullptr) {
                            cerr << "ERROR! No database selected." << "At column " << colnum << endl;
                            return;
                        }
                        if (current_database->tables.find(it->value) == current_database->tables.end()) {
                            cerr << "ERROR! Table " << it->value << " does not exist." << "At column " << colnum << endl;
                            return;
                        }
                        if (find(tablename.begin(), tablename.end(), it->value) == tablename.end()) {
                            cerr << "ERROR! Table " << it->value << " does not exist." << "At column " << colnum << endl;
                            return;
                        }
                        ++it;
                        if (it != end && it->token == Token::POINT) {
                            ++it;
                            if (it != end && it->token == Token::IDENTIFIER) {
                                // cout << it->value << endl;
                                Table t0 = current_database->tables[tname];
                                auto col_it = find_if(t0.columns.begin(), t0.columns.end(), [&](const Column& col) {
                                    return col.name == it->value;
                                });
                                if (col_it == t0.columns.end()) {
                                    cerr << "ERROR! Column " << it->value << " does not exist in table " << tname << "." << "At column " << colnum << endl;
                                    return;
                                }
                                tablename_columnname_innerjoin.resize(inner_num + 1);
                                tablename_columnname_innerjoin[inner_num].push_back(tname + "." + it->value);
                                if (tablename_columnname_innerjoin.size() == 0) {
                                    cout << "tablename_columnname_innerjoin is empty." << endl;
                                    return;
                                }
                                // 输出tablename_columnname_innerjoin
                                ++it;
                                if (it != end && it->token == Token::EQUAL) {
                                    ++it;
                                    if (it != end && it->token == Token::IDENTIFIER) {
                                        tname = it->value;
                                        if (current_database == nullptr) {
                                            cerr << "ERROR! No database selected." << "At column " << colnum << endl;
                                            return;
                                        }
                                        if (current_database->tables.find(it->value) == current_database->tables.end()) {
                                            cerr << "ERROR! Table " << it->value << " does not exist." << "At column " << colnum << endl;
                                            return;
                                        }
                                        if (find(tablename.begin(), tablename.end(), it->value) == tablename.end()) {
                                            tablename.push_back(it->value);
                                        }
                                        ++it;
                                        if (it != end && it->token == Token::POINT) {
                                            ++it;
                                            if (it != end && it->token == Token::IDENTIFIER) {
                                                t0 = current_database->tables[tname];
                                                col_it = find_if(t0.columns.begin(), t0.columns.end(), [&](const Column& col) {
                                                    return col.name == it->value;
                                                });
                                                if (col_it == t0.columns.end()) {
                                                    cerr << "ERROR! Column " << it->value << " does not exist in table " << tname << "." << "At column " << colnum << endl;
                                                    return;
                                                }
                                                tablename_columnname_innerjoin[inner_num].push_back(tname + "." + it->value);
                                                // cout << "tablename_columnname_innerjoin: " << endl;
                                                // for (const auto& m : tablename_columnname_innerjoin) {
                                                //     for (const auto& p : m) {
                                                //         cout << p << " ";
                                                //     }
                                                //     cout << endl;
                                                // }
                                                ++it;
                                                ++inner_num;
                                                inner(it, end);
                                            } else {
                                                cerr << "ERROR! Expected column name." << "At column " << colnum << endl;
                                                return;
                                            }
                                        } else {
                                            cerr << "ERROR! Expected '.'." << "At column " << colnum << endl;
                                            return;
                                        }
                                    } else {
                                        cerr << "ERROR! Expected column name." << "At column " << colnum << endl;
                                        return;
                                    }
                                } else {
                                    cerr << "ERROR! Expected =." << "At column " << colnum << endl;
                                    return;
                                }
                            } else {
                                cerr << "ERROR! Expected column name." << "At column " << colnum << endl;
                                return;
                            }
                        } else {
                            cerr << "ERROR! Expected '.'." << "At column " << colnum << endl;
                            return;
                        }
                    }
                } else {
                    cerr << "ERROR! Expected ON after table name. " << "At column " << colnum << endl;
                    return;
                }
            } else {
                cerr << "ERROR! Expected table name after JOIN. " << "At column " << colnum << endl;
                return;
            }
        } else {
            cerr << "ERROR! Expected JOIN after INNER. " << "At column " << colnum << endl;
            return;
        }
    } else if (it != end && (it->token == Token::WHERE || it->token == Token::SEMICOLON)) {
        inner_helper_f();
        if (it != end && it->token == Token::SEMICOLON) {
            out_inner_join();
        } else if (it != end && it->token == Token::WHERE) {
            // where_inner(it, end);
        } else {
            cerr << "ERROR! Expected WHERE or ; after INNER JOIN. " << "At column " << colnum << endl;
            return;
        }
    } else {
        cerr << "ERROR! Expected INNER after FROM. " << "At column " << colnum << endl;
        return;
    }
}

void Inner_join(vector<TokenWithValue>::const_iterator& it, vector<TokenWithValue>::const_iterator end) {
    table_inner.columns.clear();
    T.columns.clear();
    T.data.clear();
    table_inner.data.clear();
    tablename_columnname_innerjoin.clear();
    tablename_columnname_select_out.clear();
    tablename_columnname_where.clear();
    tablename.clear();
    inner_num = 0;
    while (it != end && it->token != Token::FROM) {
        if (it != end && it->token == Token::IDENTIFIER) {
            if (current_database == nullptr) {
                cerr << "ERROR! No database selected." << "At column " << colnum << endl;
                return;
            }
            if (current_database->tables.find(it->value) == current_database->tables.end()) {
                cerr << "ERROR! Table " << it->value << " does not exist." << "At column " << colnum << endl;
                return;
            }
            string tablename = it->value;
            ++it;
            if (it != end && it->token == Token::POINT) {
                ++it;
                if (it != end && it->token == Token::IDENTIFIER) {
                    tablename_columnname_select_out.push_back(tablename + "." + it->value);
                    ++it;
                    if (it != end && it->token == Token::COMMA) {
                        ++it;
                    } else {
                        break;
                    }
                } else {
                    cerr << "ERROR! Expected column name." << "At column " << colnum << endl;
                    return;
                }
            } else {
                cerr << "ERROR! Expected '.'." << "At column " << colnum << endl;
                return;
            }
        } else {
            cerr << "ERROR! Expected table name." << "At column " << colnum << endl;
            return;
        }
    }
    if (it != end && it->token == Token::FROM) {
        ++it;
        if (it != end && it->token == Token::IDENTIFIER) {
            if (current_database == nullptr) {
                cerr << "ERROR! No database selected." << "At column " << colnum << endl;
                return;
            }
            if (current_database->tables.find(it->value) == current_database->tables.end()) {
                cerr << "ERROR! Table " << it->value << " does not exist." << "At column " << colnum << endl;
                return;
            }
            tablename.push_back(it->value);
            ++it;
            inner(it, end);
        } else {
            cerr << "ERROR! Expected table name." << "At column " << colnum << endl;
            return;
        }
    } else {
        cerr << "ERROR! Expected FROM." << "At column " << colnum << endl;
        return;
    }
}

// 2. 选择特定的数据
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


void identifier_select(const string& column_name, vector<TokenWithValue>::const_iterator& it, vector<TokenWithValue>::const_iterator end) {
    if (it != end && it->token == Token::POINT) {
        --it;
        Inner_join(it, end);
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
            Table& table = current_database->tables[table_name];
            if (column_name == "*") {
                column_Name.clear();
                for (auto col : table.columns) {
                    column_Name.push_back(col.name);
                }
            }
            ++it;
            if (it != end && it->token == Token::WHERE) {
                Table table1;
                where_select(column_Name,it, end, table, table1);
                ++it;
                if (it != end && it->token == Token::AND) {
                    Table table2;
                    where_select(column_Name,it, end, table, table2);
                    if (select_num !=0 ){
                            out << "---" << endl;
                        }
                        ++select_num;
                    for (const auto &column : column_Name) {
                        auto it = find_if(table.columns.begin(), table.columns.end(), [&](const Column& col) {
                            return col.name == column;
                        });
                        if (it == table.columns.end()) {
                            cerr << "ERROR! Column " << column << " does not exist in table " << table_name << "." << "At column " << colnum << endl;
                            return;
                        }
                        
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
                                    if (is_number_where(value)) {
                                        is_float(value);
                                    } else {
                                        out << "'" << value << "'";
                                    }
                                    if (&value != &row1.back()) {
                                        out << ",";
                                    }
                                }
                                out << endl;
                            }
                        }
                    }
                    column_Name.clear();
                } else if (it != end && it->token == Token::OR) {
                    Table table2;
                    where_select(column_Name,it, end, table, table2);
                    if (select_num !=0 ){
                        out << "---" << endl;
                        }
                        ++select_num;
                    for (const auto &column : column_Name) {
                        auto it = find_if(table.columns.begin(), table.columns.end(), [&](const Column& col) {
                            return col.name == column;
                        });
                        if (it == table.columns.end()) {
                            cerr << "ERROR! Column " << column << " does not exist in table " << table_name << "." << "At column " << colnum << endl;
                            return;
                        }
                        
                        out << column;
                        if (column != column_Name.back()) {
                            out << ",";
                        }
                    }
                    out << endl;
                    for (const auto& row : table1.data) {
                        for (const auto& value : row) {
                            if (is_number_where(value)) {
                                is_float(value);
                            } else {
                                out << "'" << value << "'";
                            }
                            if (&value != &row.back()) {
                                out << ",";
                            }
                        }
                        out << endl;
                    }
                    for (const auto& row : table2.data) {
                        if (find(table1.data.begin(), table1.data.end(), row) == table1.data.end()) {
                            for (const auto& value : row) {
                                is_float(value);
                                if (&value != &row.back()) {
                                    out << ",";
                                }
                            }
                            out << endl;
                        }
                    }
                    column_Name.clear();
                } else {
                    if (select_num !=0 ){
                            out << "---" << endl;
                        }
                        ++select_num;
                    for (const auto &column : column_Name) {
                        auto it = find_if(table.columns.begin(), table.columns.end(), [&](const Column& col) {
                            return col.name == column;
                        });
                        if (it == table.columns.end()) {
                            cerr << "ERROR! Column " << column << " does not exist in table " << table_name << "." << "At column " << colnum << endl;
                            return;
                        }
                        
                        out << column;
                        if (column != column_Name.back()) {
                            out << ",";
                        }
                    }
                    out << endl;
                    for (const auto& row : table1.data) {
                        for (const auto& value : row) {
                            if (is_number_where(value)) {
                                is_float(value);
                            } else {
                                out << "'" << value << "'";
                            }
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
            if (select_num !=0 ){
                out << "---" << endl;
            }
            for (const auto &column : column_Name) {
                auto it = find_if(table.columns.begin(), table.columns.end(), [&](const Column& col) {
                    return col.name == column;
                });
                if (it == table.columns.end()) {
                    cerr << "ERROR! Column " << column << " does not exist in table " << table_name << "." << "At column " << colnum << endl;
                    return;
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
                        if (is_number_where(row[index])) {
                            is_float(row[index]);
                        } else {
                            out << "'" << row[index] << "'";
                        }
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
        string column_name = "*";
        column_Name.push_back(column_name);
        identifier_select(column_name, it, end);
    } else if (it != end && it->token == Token::IDENTIFIER){
        string column_name = it->value;
        ++it;
        column_Name.push_back(column_name);
        identifier_select(column_name, it, end);
    }
}