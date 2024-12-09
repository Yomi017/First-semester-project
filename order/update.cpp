#include <bits/stdc++.h>
#include "../project.h"
using namespace std;

// 更新数据
// 计算器（支持加减乘除和括号，含有一个变量）
double evaluate(const string& expression, const map<string, double>& variables) {
    stack<double> values;
    stack<char> operators;

    // 判断运算符优先级
    auto precedence = [](char op) {
        if (op == '+' || op == '-') return 1;
        if (op == '*' || op == '/') return 2;
        return 0;
    };

    // 执行运算
    auto applyOp = [](double a, double b, char op) {
        switch (op) {
            case '+': return a + b;
            case '-': return a - b;
            case '*': return a * b;
            case '/': if (b == 0) throw runtime_error("Division by zero");
                      return a / b;
            default: throw runtime_error("Unsupported operator");
        }
    };

    // 解析变量或数字
    auto resolveVariable = [&](const string& token) -> double {
        if (isdigit(token[0]) || (token[0] == '-' && token.size() > 1)) {
            return stod(token); // 是数字，直接转换
        }
        auto it = variables.find(token);
        if (it != variables.end()) {
            return it->second; // 从变量表中获取值
        }
        throw runtime_error("Undefined variable: " + token);
    };
    // 预处理表达式：加空格
    string expr = expression;
    for (size_t i = 0; i < expr.length(); ++i) {
        if (ispunct(expr[i]) && !isspace(expr[i])) {
            expr.insert(i, " ");
            expr.insert(i + 2, " ");
            i += 2; // 跳过刚插入的空格
        }
    }

    stringstream ss(expr);
    string token;

    while (ss >> token) {
        if (isdigit(token[0]) || (token[0] == '-' && token.size() > 1)) {
            values.push(stod(token)); // 直接压入数字
        } else if (isalpha(token[0])) {
            for (int n = 0; n < variables.size(); ++n) {
                try {
                    values.push(resolveVariable(token)); // 解析变量
                    break;
                } catch (const runtime_error& e) {
                    if (n == variables.size() - 1) {
                        throw e; // 未找到变量
                    }
                }
            }
        } else if (token == "(") {
            operators.push('('); // 左括号直接压入栈
        } else if (token == ")") {
            // 计算括号内的表达式
            while (!operators.empty() && operators.top() != '(') {
                double b = values.top(); values.pop();
                double a = values.top(); values.pop();
                char op = operators.top(); operators.pop();
                values.push(applyOp(a, b, op));
            }
            operators.pop(); // 弹出 '('
        } else {
            // 处理运算符
            while (!operators.empty() && precedence(operators.top()) >= precedence(token[0])) {
                double b = values.top(); values.pop();
                double a = values.top(); values.pop();
                char op = operators.top(); operators.pop();
                values.push(applyOp(a, b, op));
            }
            operators.push(token[0]);
        }
    }

    // 处理剩下的运算符
    while (!operators.empty()) {
        double b = values.top(); values.pop();
        double a = values.top(); values.pop();
        char op = operators.top(); operators.pop();
        values.push(applyOp(a, b, op));
    }

    return values.top(); // 返回最终的结果
}

// 获取小数点后的位数
int getDecimalPlaces(const string& s) {
    size_t pos = s.find('.');
    if (pos == string::npos) {
        return 0;
    }
    return s.size() - pos - 1;
}

// 更新数据辅助执行函数(部分数据)
void update_helper(Table& table, const string& column_name, const string& expression, const string& condition_column, const string& op, const string& value, int digit_or_identifier, vector<string>& variable_name, vector<TokenWithValue>::const_iterator& it, vector<TokenWithValue>::const_iterator end) {
    vector<bool> match(table.data.size(), false);
    // out <<digit_or_identifier;
    if (digit_or_identifier == 1) {
        auto its = find_if(table.columns.begin(), table.columns.end(), [&](const Column& col) {
            return col.name == condition_column;
        });
        if (its == table.columns.end()) {
            throw runtime_error("Condition column not found.");
        }
        size_t condition_index = distance(table.columns.begin(), its);

        for (size_t i = 0; i < table.data.size(); ++i) {
            const auto& target_row = table.data[i];
            const auto& target_value = target_row[condition_index];

            if (op == "=" && target_value == value) {
                match[i] = true;
            } else if (op == ">" && is_number_where(value) && is_number_where(target_value)) {
                if (stod(target_value) - stod(value) > 1e-9) {
                    match[i] = true;
                }
            } else if (op == "<" && is_number_where(value) && is_number_where(target_value)) {
                if (stod(target_value) - stod(value) < 1e-9) {
                    match[i] = true;
                }
            }
        }
        ++it;
        if (it != end && it->token == Token::AND) {
            ++it;
            if (it != end && it->token == Token::IDENTIFIER) {
                string condition_column2 = it->value;
                ++it;
                if (it != end && (it->token == Token::EQUAL || it->token == Token::GT || it->token == Token::LT || it->token == Token::NOT)) {
                    string symbol2 = it->value;
                    if (it != end && it->token == Token::NOT) {
                        ++it;
                        if (it != end && it->token != Token::EQUAL) {
                            cerr << "ERROR! Expected = after NOT." << "At column " << colnum << endl;
                            return;
                        }
                    }
                    ++it;
                    if (it != end && (it->token == Token::STRING || it->token == Token::NUMBER)) {
                        string condition_value2 = it->value;
                        auto its2 = find_if(table.columns.begin(), table.columns.end(), [&](const Column& col) {
                            return col.name == condition_column2;
                        });
                        if (its2 == table.columns.end()) {
                            throw runtime_error("Condition column not found.");
                        }
                        size_t condition_index2 = distance(table.columns.begin(), its2);

                        for (size_t i = 0; i < table.data.size(); ++i) {
                            const auto& target_row = table.data[i];
                            const auto& target_value = target_row[condition_index2];

                            if (match[i]) {
                                if (symbol2 == "=" && target_value == condition_value2) {
                                    match[i] = true;
                                } else if (symbol2 == ">" && is_number_where(condition_value2) && is_number_where(target_value)) {
                                    if (stod(target_value) - stod(condition_value2) > 1e-9) {
                                        match[i] = true;
                                    }
                                } else if (symbol2 == "<" && is_number_where(condition_value2) && is_number_where(target_value)) {
                                    if (stod(target_value) - stod(condition_value2) < 1e-9) {
                                        match[i] = true;
                                    }
                                } else {
                                    match[i] = false;
                                }
                            }
                        }
                    } else {
                        cerr << "ERROR! Expected value after symbol." << "At column " << colnum << endl;
                        return;
                    }
                } else {
                    cerr << "ERROR! Expected symbol after column name." << "At column " << colnum << endl;
                    return;
                }
            } else {
                cerr << "ERROR! Expected column name after AND." << "At column " << colnum << endl;
                return;
            }
        } else if (it != end && it->token == Token::OR) {
            ++it;
            if (it != end && it->token == Token::IDENTIFIER) {
                string condition_column2 = it->value;
                ++it;
                if (it != end && (it->token == Token::EQUAL || it->token == Token::GT || it->token == Token::LT || it->token == Token::NOT)) {
                    string symbol2 = it->value;
                    if (it != end && it->token == Token::NOT) {
                        ++it;
                        if (it != end && it->token != Token::EQUAL) {
                            cerr << "ERROR! Expected = after NOT." << "At column " << colnum << endl;
                            return;
                        }
                    }
                    ++it;
                    if (it != end && (it->token == Token::STRING || it->token == Token::NUMBER)) {
                        string condition_value2 = it->value;
                        auto its2 = find_if(table.columns.begin(), table.columns.end(), [&](const Column& col) {
                            return col.name == condition_column2;
                        });
                        if (its2 == table.columns.end()) {
                            throw runtime_error("Condition column not found.");
                        }
                        size_t condition_index2 = distance(table.columns.begin(), its2);

                        for (size_t i = 0; i < table.data.size(); ++i) {
                            const auto& target_row = table.data[i];
                            const auto& target_value = target_row[condition_index2];

                            if (!match[i]) {
                                if (symbol2 == "=" && target_value == condition_value2) {
                                    match[i] = true;
                                } else if (symbol2 == ">" && is_number_where(condition_value2) && is_number_where(target_value)) {
                                    if (stod(target_value) - stod(condition_value2) > 1e-9) {
                                        match[i] = true;
                                    }
                                } else if (symbol2 == "<" && is_number_where(condition_value2) && is_number_where(target_value)) {
                                    if (stod(target_value) - stod(condition_value2) < 1e-9) {
                                        match[i] = true;
                                    }
                                }
                            }
                        }
                    } else {
                        cerr << "ERROR! Expected value after symbol." << "At column " << colnum << endl;
                        return;
                    }
                } else {
                    cerr << "ERROR! Expected symbol after column name." << "At column " << colnum << endl;
                    return;
                }
            } else {
                cerr << "ERROR! Expected column name after OR." << "At column " << colnum << endl;
                return;
            }
        }
        for (size_t i = 0; i < table.data.size(); ++i) {
            if (match[i]) {
                    auto col_it = find_if(table.columns.begin(), table.columns.end(), [&](const Column& col) {
                        return col.name == column_name;
                    });
                    if (col_it != table.columns.end()) {
                        size_t index = distance(table.columns.begin(), col_it);
                        table.data[i][index] = expression;
                        // cout << "Updated"<< endl;
                }
            }
        }
    }  else if (digit_or_identifier > 1) {
        auto its = find_if(table.columns.begin(), table.columns.end(), [&](const Column& col) {
            return col.name == condition_column;
        });
        if (its == table.columns.end()) {
            throw runtime_error("Condition column not found.");
        }
        size_t condition_index = distance(table.columns.begin(), its);

        for (size_t i = 0; i < table.data.size(); ++i) {
            const auto& target_row = table.data[i];
            const auto& target_value = target_row[condition_index];

            if (op == "=" && target_value == value) {
                match[i] = true;
            } else if (op == ">" && is_number_where(value) && is_number_where(target_value)) {
                if (stod(target_value) > stod(value)) {
                    match[i] = true;
                }
            } else if (op == "<" && is_number_where(value) && is_number_where(target_value)) {
                if (stod(target_value) < stod(value)) {
                    match[i] = true;
                }
            }
        }
        ++it;
        if (it != end && it->token == Token::AND) {
            ++it;
            if (it != end && it->token == Token::IDENTIFIER) {
                string condition_column2 = it->value;
                ++it;
                if (it != end && (it->token == Token::EQUAL || it->token == Token::GT || it->token == Token::LT || it->token == Token::NOT)) {
                    string symbol2 = it->value;
                    if (it != end && it->token == Token::NOT) {
                        ++it;
                        if (it != end && it->token != Token::EQUAL) {
                            cerr << "ERROR! Expected = after NOT." << "At column " << colnum << endl;
                            return;
                        }
                    }
                    ++it;
                    if (it != end && (it->token == Token::STRING || it->token == Token::NUMBER)) {
                        string condition_value2 = it->value;
                        auto its2 = find_if(table.columns.begin(), table.columns.end(), [&](const Column& col) {
                            return col.name == condition_column2;
                        });
                        if (its2 == table.columns.end()) {
                            throw runtime_error("Condition column not found.");
                        }
                        size_t condition_index2 = distance(table.columns.begin(), its2);

                        for (size_t i = 0; i < table.data.size(); ++i) {
                            const auto& target_row = table.data[i];
                            const auto& target_value = target_row[condition_index2];

                            if (match[i]) {
                                if (symbol2 == "=" && target_value == condition_value2) {
                                    match[i] = true;
                                } else if (symbol2 == ">" && is_number_where(condition_value2) && is_number_where(target_value)) {
                                    if (stod(target_value) - stod(condition_value2) > 1e-9) {
                                        match[i] = true;
                                    }
                                } else if (symbol2 == "<" && is_number_where(condition_value2) && is_number_where(target_value)) {
                                    if (stod(target_value) - stod(condition_value2) < 1e-9) {
                                        match[i] = true;
                                    }
                                } else {
                                    match[i] = false;
                                }
                            }
                        }
                    } else {
                        cerr << "ERROR! Expected value after symbol." << "At column " << colnum << endl;
                        return;
                    }
                } else {
                    cerr << "ERROR! Expected symbol after column name." << "At column " << colnum << endl;
                    return;
                }
            } else {
                cerr << "ERROR! Expected column name after AND." << "At column " << colnum << endl;
                return;
            }
        } else if (it != end && it->token == Token::OR) {
            ++it;
            if (it != end && it->token == Token::IDENTIFIER) {
                string condition_column2 = it->value;
                ++it;
                if (it != end && (it->token == Token::EQUAL || it->token == Token::GT || it->token == Token::LT || it->token == Token::NOT)) {
                    string symbol2 = it->value;
                    if (it != end && it->token == Token::NOT) {
                        ++it;
                        if (it != end && it->token != Token::EQUAL) {
                            cerr << "ERROR! Expected = after NOT." << "At column " << colnum << endl;
                            return;
                        }
                    }
                    ++it;
                    if (it != end && (it->token == Token::STRING || it->token == Token::NUMBER)) {
                        string condition_value2 = it->value;
                        auto its2 = find_if(table.columns.begin(), table.columns.end(), [&](const Column& col) {
                            return col.name == condition_column2;
                        });
                        if (its2 == table.columns.end()) {
                            throw runtime_error("Condition column not found.");
                        }
                        size_t condition_index2 = distance(table.columns.begin(), its2);

                        for (size_t i = 0; i < table.data.size(); ++i) {
                            const auto& target_row = table.data[i];
                            const auto& target_value = target_row[condition_index2];

                            if (!match[i]) {
                                if (symbol2 == "=" && target_value == condition_value2) {
                                    match[i] = true;
                                } else if (symbol2 == ">" && is_number_where(condition_value2) && is_number_where(target_value)) {
                                    if (stod(target_value) - stod(condition_value2) > 1e-9) {
                                        match[i] = true;
                                    }
                                } else if (symbol2 == "<" && is_number_where(condition_value2) && is_number_where(target_value)) {
                                    if (stod(target_value) - stod(condition_value2) < 1e-9) {
                                        match[i] = true;
                                    }
                                }
                            }
                        }
                    } else {
                        cerr << "ERROR! Expected value after symbol." << "At column " << colnum << endl;
                        return;
                    }
                } else {
                    cerr << "ERROR! Expected symbol after column name." << "At column " << colnum << endl;
                    return;
                }
            } else {
                cerr << "ERROR! Expected column name after OR." << "At column " << colnum << endl;
                return;
            }
        }
        // 替换表达式中的列名为 'x'
        string replaced_expression = expression;
        size_t pos = 0;
        // while ((pos = replaced_expression.find(column_name, pos)) != string::npos) {
        //     replaced_expression.replace(pos, column_name.length(), "x");
        //     pos += column_name.length();  // 移动到下一个位置
        // }

        for (size_t i = 0; i < table.data.size(); ++i) {
            if (match[i]) {
                vector<vector<Column>::iterator> col_itself;
                for (int j = 0; j < variable_name.size() ; ++j) {
                    auto col_it = find_if(table.columns.begin(), table.columns.end(), [&](const Column& col) {
                        return col.name == variable_name[j];
                    });
                    if (col_it == table.columns.end()) {
                        cerr << "Error: Column not found for variable " << variable_name[j] << endl;
                        // 可以选择跳过或继续处理错误
                    } else {
                        col_itself.push_back(col_it);
                    }
                }
                int decimal_places;
                size_t index;
                map<string, double> variables;
                if (all_of(col_itself.begin(), col_itself.end(), [&](vector<Column>::iterator it) { return it != table.columns.end(); })) {
                    for (int k = 0; k < variable_name.size(); ++k) {
                        auto col_itself_k = col_itself[k];
                        if (col_itself_k == table.columns.end()) {
                            cerr << "Error: column iterator is invalid for k=" << k << endl;
                            return;
                        }
                        size_t index_in = distance(table.columns.begin(), col_itself[k]);
                        // 构造变量表
                        string value_name = table.columns[index_in].name;
                        if (index_in >= table.data[i].size()) {
                            cerr << "Error: index " << index_in << " is out of bounds for table.data[" << i << "]" << endl;
                            return;
                            // 处理越界的情况（例如跳过当前循环或给出默认值）
                        } else {
                            variables[value_name] = stod(table.data[i][index_in]);
                            // cout << variables[value_name] << endl;
                        }
                        if (k == 0) {
                            index = index_in;
                            decimal_places = getDecimalPlaces(table.data[i][index]);
                        }
                    }
                    // 计算结果并格式化
                    try {
                        ostringstream oss;
                        oss << fixed << setprecision(decimal_places) << evaluate(replaced_expression, variables);
                        table.data[i][index] = oss.str();
                    } catch (const runtime_error& e) {
                        cerr << "Error evaluating expression: " << e.what() << endl;
                    }
                } else {
                    cerr << "Error: Column not found for variable" << endl;
                    // 可以选择跳过或继续处理错误
                }
            }
        }
    } else {
        cerr << "ERROR! Expected digit or identifier after EQUAL." << "At column " << colnum << endl;
    }
}

// 更新数据辅助执行函数(所有数据)
void update_helper2(Table& table, const string& column_name, const string& expression, int digit_or_identifier, vector<string>& variable_name) {
    // cout << "in update_helper2" << endl;
    if (digit_or_identifier == 1) {
        for (size_t i = 0; i < table.data.size(); ++i) {
            auto col_it = find_if(table.columns.begin(), table.columns.end(), [&](const Column& col) {
                return col.name == column_name;
            });
            if (col_it != table.columns.end()) {
                size_t index = distance(table.columns.begin(), col_it);
                table.data[i][index] = expression;
                // cout << "Updated"<< endl;
            }
        }
    } else if (digit_or_identifier > 1) {
        // 替换表达式中的列名为 'x'
        string replaced_expression = expression;
        size_t pos = 0;
        // while ((pos = replaced_expression.find(column_name, pos)) != string::npos) {
        //     replaced_expression.replace(pos, column_name.length(), "x");
        //     pos += column_name.length();  // 移动到下一个位置
        // }

        for (size_t i = 0; i < table.data.size(); ++i) {
            vector<vector<Column>::iterator> col_itself;
                for (int j = 0; j < variable_name.size() ; ++j) {
                    auto col_it = find_if(table.columns.begin(), table.columns.end(), [&](const Column& col) {
                        return col.name == variable_name[j];
                    });
                    if (col_it == table.columns.end()) {
                        cerr << "Error: Column not found for variable " << variable_name[j] << endl;
                        // 可以选择跳过或继续处理错误
                    } else {
                        col_itself.push_back(col_it);
                    }
                }
                int decimal_places;
                size_t index;
                map<string, double> variables;
                if (all_of(col_itself.begin(), col_itself.end(), [&](vector<Column>::iterator it) { return it != table.columns.end(); })) {
                    for (int k = 0; k < variable_name.size(); ++k) {
                        auto col_itself_k = col_itself[k];
                        if (col_itself_k == table.columns.end()) {
                            cerr << "Error: column iterator is invalid for k=" << k << endl;
                            return;
                        }
                        size_t index_in = distance(table.columns.begin(), col_itself[k]);
                        // 构造变量表
                        string value_name = table.columns[index_in].name;
                        if (index_in >= table.data[i].size()) {
                            cerr << "Error: index " << index_in << " is out of bounds for table.data[" << i << "]" << endl;
                            return;
                            // 处理越界的情况（例如跳过当前循环或给出默认值）
                        } else {
                            variables[value_name] = stod(table.data[i][index_in]);
                            // cout << variables[value_name] << endl;
                        }
                        if (k == 0) {
                            index = index_in;
                            decimal_places = getDecimalPlaces(table.data[i][index]);
                        }
                    }
                    // 计算结果并格式化
                    try {
                        ostringstream oss;
                        oss << fixed << setprecision(decimal_places) << evaluate(replaced_expression, variables);
                        table.data[i][index] = oss.str();
                    } catch (const runtime_error& e) {
                        cerr << "Error evaluating expression: " << e.what() << endl;
                    }
                } else {
                    cerr << "Error: Column not found for variable" << endl;
                    // 可以选择跳过或继续处理错误
                }
            }
        } else {
            cerr << "ERROR! Expected digit or identifier after EQUAL." << "At column " << colnum << endl;
        }
}

// 更新数据主函数
void update_data(vector<TokenWithValue>::const_iterator& it, vector<TokenWithValue>::const_iterator end) {
    vector<string> expression_list;
    vector<string> column_name_list;
    string expression;
    int digit_or_identifier = 0;
    vector<int> digit_or_identifier_list;
    int size = 0;  // 初始化 size，避免未定义的行为
    vector<string> variable_name;
    if (it != end && it->token == Token::IDENTIFIER) {
        Table& table = current_database->tables[it->value];
        ++it;
        if (it != end && it->token == Token::SET) {
            ++it;
            while (it != end && it->token != Token::WHERE && it->token != Token::SEMICOLON) {
                if (it != end && it->token == Token::IDENTIFIER) {
                    column_name_list.push_back(it->value);  // 记录列名
                    ++it;
                    if (it != end && it->token == Token::EQUAL) {
                        ++it;
                        expression.clear();  // 每次处理新表达式时清空
                        while (it != end && it->token != Token::COMMA && it->token != Token::WHERE && it->token != Token::SEMICOLON) {
                            if (digit_or_identifier >= 1) {
                                expression += " ";  // 添加空格分隔符
                            }
                            expression += it->value;  // 拼接表达式
                            if (it != end && it->token == Token::IDENTIFIER){
                                variable_name.push_back(it->value);
                            }
                            ++digit_or_identifier;
                            ++it;
                        }
                        expression_list.push_back(expression);  // 将表达式加入列表
                        ++size;  // 增加已处理表达式数量
                        expression.clear();  // 清空 expression 以处理下一个
                        digit_or_identifier_list.push_back(digit_or_identifier);  // 记录当前表达式中数字和标识符的数量
                        digit_or_identifier = 0;  // 重置拼接标志
                    }
                }
                if (it != end && it->token == Token::COMMA) {
                    ++it;  // 跳过逗号
                }
            }
            // 解析 WHERE 子句
            if (it != end && it->token == Token::WHERE) {
                ++it;
                if (it != end && it->token == Token::IDENTIFIER) {
                    string condition_column = it->value;
                    ++it;
                    if (it != end && (it->token == Token::EQUAL || it->token == Token::GT || it->token == Token::LT)) {
                        string symbol = it->value;
                        ++it;
                        if (it != end && (it->token == Token::STRING || it->token == Token::NUMBER)) {
                            string condition_value = it->value;
                            // 调用更新函数
                            for (int i = 0; i < size; ++i) {
                                update_helper(table, column_name_list[i], expression_list[i], condition_column, symbol, condition_value, digit_or_identifier_list[i], variable_name, it, end);
                            }
                        } else {
                            cerr << "ERROR! Expected value after symbol." << "At column " << colnum << endl;
                            return;
                        }
                    } else {
                        cerr << "ERROR! Expected symbol after column name." << "At column " << colnum << endl;
                        return;
                    }
                } else {
                    cerr << "ERROR! Expected column name after WHERE." << "At column " << colnum << endl;
                    return;
                }
            } else if (it != end && it->token != Token::WHERE) {
                for (int i = 0; i < size; ++i) {
                    update_helper2(table, column_name_list[i], expression_list[i], digit_or_identifier_list[i], variable_name);
                }
            } else {
                cerr << "ERROR! Expected WHERE or ; after expressions." << "At column " << colnum << endl;
                return;
            }
        } else {
            cerr << "ERROR! Expected SET after table name." << "At column " << colnum << endl;
            return;
        }
    } else {
        cerr << "ERROR! Expected table name after UPDATE." << "At column " << colnum << endl;
        return;
    }
    variable_name.clear();
}