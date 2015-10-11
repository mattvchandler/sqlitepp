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

#include <stdexcept>
#include <string>

#include <sqlite3.h>

#include <sigc++/sigc++.h>

class Sqlite_db_conn final: public sigc::trackable
{
public:
    class Stmt final: public sigc::trackable
    {
    public:
        Stmt(const std::string & sql, Sqlite_db_conn & db);
        ~Stmt();

        void bind_null(const int index);
        void bind(const int index);
        void bind(const int index, const int val);
        void bind(const int index, const sqlite3_int64 val);
        void bind(const int index, const double val);
        void bind(const int index, const std::string & val);
        void bind(const int index, const sqlite3_value * val);

        void bind_null(const std::string & name);
        void bind(const std::string & name);
        void bind(const std::string & name, const int val);
        void bind(const std::string & name, const sqlite3_int64 val);
        void bind(const std::string & name, const double val);
        void bind(const std::string & name, const std::string & val);
        void bind(const std::string & name, const sqlite3_value * val);

        std::string bind_parameter_name(const int index);
        int bind_parameter_index(const std::string & name);

        bool step();

        template<typename T>
        T get_col(const int column);

        void reset();
        void clear_bindings();

        const sqlite3_stmt * operator()() const;
        sqlite3_stmt * operator()();

    private:
        sqlite3_stmt * _stmt = nullptr;
        sqlite3 * _db = nullptr;
    };

    Sqlite_db_conn(const std::string & filename);
    ~Sqlite_db_conn();

    Stmt create_statement(const std::string & sql);

    void exec(const std::string & sql, int (*callback)(void *, int, char **, char **) = nullptr,
        void * arg = nullptr);
    void begin_transaction();
    void commit();
    void rollback();

    const sqlite3 * operator()() const;
    sqlite3 * operator()();

private:
    sqlite3 * _db = nullptr;
};

class Sqlite_error
{
public:
    virtual ~Sqlite_error() = default;
    virtual const char * sql() const noexcept
    { return _sql.c_str(); }

protected:
    Sqlite_error(const std::string & sql):
        _sql(sql)
    {}

private:
    std::string _sql;
};

class Sqlite_logic_error: public std::logic_error, public Sqlite_error
{
public:
    Sqlite_logic_error(const std::string & what, const std::string & sql):
        std::logic_error(what),
        Sqlite_error(sql)
    {}
    virtual ~Sqlite_logic_error() = default;
};

class Sqlite_runtime_error: public std::runtime_error, public Sqlite_error
{
public:
    Sqlite_runtime_error(const std::string & what, const std::string & sql):
        std::runtime_error(what),
        Sqlite_error(sql)
    {}
    virtual ~Sqlite_runtime_error() = default;
};

# endif // SQLITE_HPP
