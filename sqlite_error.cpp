// sqlie_error.cpp
// exception types for sqlite errors

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

#include "sqlite/sqlite_error.hpp"

Sqlite_error::~Sqlite_error()
{
}

// get SQL code where error was thrown
const char * Sqlite_error::sql() const noexcept
{
    return _sql.c_str();
}

// get sqlite3 error code
int Sqlite_error::err_code() const noexcept
{
    return _sqlite_error_code;
}

// get the sqlite3 internal error message
const char * Sqlite_error::err_msg() const noexcept
{
    if(!_db)
        return "";

    return sqlite3_errmsg(_db);
}

Sqlite_error::Sqlite_error(const std::string & sql, int sqlite_error_code, sqlite3 * db):
    _sql(sql),
    _sqlite_error_code(sqlite_error_code),
    _db(db)
{
}

Sqlite_logic_error::Sqlite_logic_error(const std::string & what, const std::string & sql,
        int sqlite_error_code, sqlite3 * db):
    std::logic_error(what),
    Sqlite_error(sql, sqlite_error_code, db)
{
}

Sqlite_runtime_error::Sqlite_runtime_error(const std::string & what, const std::string & sql,
        int sqlite_error_code, sqlite3 * db):
    std::runtime_error(what),
    Sqlite_error(sql, sqlite_error_code, db)
{
}