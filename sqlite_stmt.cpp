// sqlite_stmt.cpp
// sqlite statement library wrapper

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

#include "sqlite/sqlite.hpp"

#include <stdexcept>

Sqlite_db_conn::Stmt::Stmt(const std::string & sql, Sqlite_db_conn & db):
    _db(db())
{
    int status = sqlite3_prepare_v2(db(), sql.c_str(), sql.length() + 1, &_stmt, NULL);

    if(status != SQLITE_OK)
    {
        throw std::logic_error("Error parsing SQL (" + sql + "): " + sqlite3_errmsg(db()));
    }
}

Sqlite_db_conn::Stmt::~Stmt()
{
    sqlite3_finalize(_stmt);
}

void Sqlite_db_conn::Stmt::bind_null(const int index)
{
    int status = sqlite3_bind_null(_stmt, index);
    if(status != SQLITE_OK)
    {
        throw std::logic_error("Error binding index " +
            std::to_string(index) + " (" + sqlite3_sql(_stmt) + "): " + sqlite3_errmsg(_db));
    }
}

void Sqlite_db_conn::Stmt::bind(const int index)
{
    bind_null(index);
}

void Sqlite_db_conn::Stmt::bind(const int index, const double val)
{
    int status = sqlite3_bind_double(_stmt, index, val);
    if(status != SQLITE_OK)
    {
        throw std::logic_error("Error binding index " +
            std::to_string(index) + " (" + sqlite3_sql(_stmt) + "): " + sqlite3_errmsg(_db));
    }
}

void Sqlite_db_conn::Stmt::bind(const int index, const int val)
{
    int status = sqlite3_bind_int(_stmt, index, val);
    if(status != SQLITE_OK)
    {
        throw std::logic_error("Error binding index " +
            std::to_string(index) + " (" + sqlite3_sql(_stmt) + "): " + sqlite3_errmsg(_db));
    }
}

void Sqlite_db_conn::Stmt::bind(const int index, const sqlite3_int64 val)
{
    int status = sqlite3_bind_int64(_stmt, index, val);
    if(status != SQLITE_OK)
    {
        throw std::logic_error("Error binding index " +
            std::to_string(index) + " (" + sqlite3_sql(_stmt) + "): " + sqlite3_errmsg(_db));
    }
}

void Sqlite_db_conn::Stmt::bind(const int index, const std::string & val)
{
    int status = sqlite3_bind_text(_stmt, index, val.c_str(), val.length(), SQLITE_TRANSIENT);
    if(status != SQLITE_OK)
    {
        throw std::logic_error("Error binding index " +
            std::to_string(index) + " (" + sqlite3_sql(_stmt) + "): " + sqlite3_errmsg(_db));
    }
}

void Sqlite_db_conn::Stmt::bind(const int index, const sqlite3_value * val)
{
    int status = sqlite3_bind_value(_stmt, index, val);
    if(status != SQLITE_OK)
    {
        throw std::logic_error("Error binding index " +
            std::to_string(index) + " (" + sqlite3_sql(_stmt) + "): " + sqlite3_errmsg(_db));
    }
}

bool Sqlite_db_conn::Stmt::step()
{
    int status = sqlite3_step(_stmt);
    if(status == SQLITE_ROW)
    {
        return true;
    }
    else if(status == SQLITE_DONE)
    {
        return false;
    }
    else
    {
        throw std::logic_error(std::string("Error evaluating SQL (") +
            sqlite3_sql(_stmt) + "): " + sqlite3_errmsg(_db));
    }
}

template<>
double Sqlite_db_conn::Stmt::get_col<double>(const int column)
{
    return sqlite3_column_double(_stmt, column);
}

template<>
int Sqlite_db_conn::Stmt::get_col<int>(const int column)
{
    return sqlite3_column_int(_stmt, column);
}

template<>
sqlite3_int64 Sqlite_db_conn::Stmt::get_col<sqlite3_int64>(const int column)
{
    return sqlite3_column_int64(_stmt, column);
}

template<>
std::string Sqlite_db_conn::Stmt::get_col<std::string>(const int column)
{
    const char * str = reinterpret_cast<const char *>(sqlite3_column_text(_stmt, column));

    if(!str)
        return std::string("");
    else
        return std::string(str);
}

template<>
const char * Sqlite_db_conn::Stmt::get_col<const char *>(const int column)
{
    return reinterpret_cast<const char *>(sqlite3_column_text(_stmt, column));
}

template<>
sqlite3_value * Sqlite_db_conn::Stmt::get_col<sqlite3_value *>(const int column)
{
    return sqlite3_column_value(_stmt, column);
}

void Sqlite_db_conn::Stmt::reset()
{
    int status = sqlite3_reset(_stmt);
    if(status != SQLITE_OK)
    {
        throw std::logic_error("Error resetting statement (" +
            std::string(sqlite3_sql(_stmt)) + "): " + sqlite3_errmsg(_db));
    }
}

void Sqlite_db_conn::Stmt::clear_bindings()
{
    sqlite3_clear_bindings(_stmt);
}

const sqlite3_stmt * Sqlite_db_conn::Stmt::operator()() const
{
    return _stmt;
}

sqlite3_stmt * Sqlite_db_conn::Stmt::operator()()
{
    return _stmt;
}
