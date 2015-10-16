// sqlite.hpp
// sqlite library wrapper

// Copyright 2015 Matthew Chandler

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef SQLITE_HPP
#define SQLITE_HPP

#include <string>

#include <sqlite3.h>

// DB obj
class Sqlite_db_conn final
{
public:
    class Stmt;

    // open / create new DB
    Sqlite_db_conn(const std::string & filename);
    ~Sqlite_db_conn();

    // non-copyable
    Sqlite_db_conn(const Sqlite_db_conn &) = delete;
    Sqlite_db_conn & operator=(const Sqlite_db_conn &) = delete;

    // need to explicit say we want default move ctors
    Sqlite_db_conn(Sqlite_db_conn &&) = default;
    Sqlite_db_conn & operator=(Sqlite_db_conn &&) = default;

    // create a new prepared statement
    Stmt create_statement(const std::string & sql);

    // execute a block of sql
    // callback can be NULL
    // arg will be passed as 1st arg of callback
    // callback(arg, # columns in result, column text arr, column name arr)
    void exec(const std::string & sql, int (*callback)(void *, int, char **, char **) = nullptr,
        void * arg = nullptr);

    // start a transaction
    void begin_transaction();

    // end transaction & commit
    void commit();

    // end transaction & rollback
    void rollback();

    // get contained C obj (for use with C API - we don't wrap it all)
    const sqlite3 * operator()() const;
    sqlite3 * operator()();

private:
    // C API's DB obj
    sqlite3 * _db = nullptr;
};

// prepared statement obj - usually created by Sqlite_db_conn::create_statement
class Sqlite_db_conn::Stmt final
{
public:
    // prepare a new statement for the given SQL
    Stmt(const std::string & sql, Sqlite_db_conn & db);
    ~Stmt();

    // non-copyable
    Stmt(const Stmt &) = delete;
    Stmt & operator=(const Stmt &) = delete;

    // need to explicit say we want default move ctors
    Stmt(Stmt &&) = default;
    Stmt & operator=(Stmt &&) = default;

    // bind var by index
    // note: not all sqlite3 supported types are wrapped yet
    void bind_null(const int index);
    void bind(const int index);
    void bind(const int index, const int val);
    void bind(const int index, const sqlite3_int64 val);
    void bind(const int index, const double val);
    void bind(const int index, const std::string & val);
    void bind(const int index, const sqlite3_value * val);

    // bind var by name (note: as in C sqlite, bind var indexes start at 1)
    void bind_null(const std::string & name);
    void bind(const std::string & name);
    void bind(const std::string & name, const int val);
    void bind(const std::string & name, const sqlite3_int64 val);
    void bind(const std::string & name, const double val);
    void bind(const std::string & name, const std::string & val);
    void bind(const std::string & name, const sqlite3_value * val);

    // get bind var name from index
    std::string bind_parameter_name(const int index);
    // get bind var index by name
    int bind_parameter_index(const std::string & name);

    // run the statement. for multi-row SELECTs, fetches one row, and returns true when no rows remain
    bool step();

    // get SELECTed col
    // note: not all sqlite3 supported types are wrapped yet
    template<typename T>
    T get_col(const int column);

    // reset the statment - useful for INSERTing multiple rows
    void reset();
    // clear bind vars to NULL
    void clear_bindings();

    // get contained C obj (for use with C API - we don't wrap it all)
    const sqlite3_stmt * operator()() const;
    sqlite3_stmt * operator()();

private:
    // C API statemnt obj
    sqlite3_stmt * _stmt = nullptr;
    // copy of C API DB obj
    sqlite3 * _db = nullptr;
};

# endif // SQLITE_HPP
