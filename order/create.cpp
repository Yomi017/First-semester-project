#include <bits/stdc++.h>
#include "../project.h"
using namespace std;

// 创建数据库
void create_database(const string& db_name) {
    if (databases.find(db_name) != databases.end()) {
        cerr << "Database " << db_name << " already exists." << "At column " << colnum << endl;
        return;
    }
    // cout << "Database " << db_name << " created.\n";
    databases[db_name] = Database{db_name};
    // cout << "Database " << db_name << " created.\n";
}

// 进入数据库
void use_database(const string& db_name) {
    if (databases.find(db_name) == databases.end()) {
        cerr << "ERROR! Database " << db_name << " does not exist." << "At column " << colnum << endl;
        return;
    }
    current_database = &databases[db_name];
    // cout << "Using database " << db_name << ".\n";
}

// 创建表
void create_table(const string& table_name, vector<TokenWithValue>::const_iterator& it, vector<TokenWithValue>::const_iterator end) {
    if (current_database == nullptr) {
        cerr << "ERROR! No database selected." << "At column " << colnum << endl;
        return;
    }

    // 创建表结构
    Table table(table_name);
    
    if (it == end || it->token != Token::LPAREN) {
        cerr << "ERROR! Expected '(' after table name." << "At column " << colnum << endl;
        return;
    }
    ++it; 

  
    while (it != end && it->token != Token::RPAREN) {
        if (it->token == Token::IDENTIFIER) {
            string column_name = it->value;
            ++it; 
            if (it == end || it->token != Token::IDENTIFIER) {
                cerr << "ERROR! Expected column type after column name." << "At column " << colnum << endl;
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
        cerr << "ERROR! Expected ')' after column definitions. " << "At column " << colnum << endl;
        return;
    }

    ++it;

    current_database->tables[table_name] = table;
    // cout << "Table " << table_name << " created.\n";
}
