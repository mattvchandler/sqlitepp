// sqlie_error.hpp
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

#ifndef SQLITE_ERROR_HPP
#define SQLITE_ERROR_HPP

#include <stdexcept>
#include <string>

#include <sqlite3.h>

// common data for SQL errors (abstract base)
// ideally, this would be a concrete class deriving from std::exception,
// but then our derived exceptions below couldn't derive from std::runtime & logic_errors
class Sqlite_error
{
public:
    virtual ~Sqlite_error() = 0;
    // get SQL code where error was thrown
    virtual const char * sql() const noexcept;
    // get sqlite3 error code
    virtual int err_code() const noexcept;
    // get the sqlite3 internal error message
    virtual const char * err_msg() const noexcept;

protected:
    Sqlite_error(const std::string & sql, int sqlite_error_code, sqlite3 * db);

private:
    std::string _sql;
    int _sqlite_error_code;
    sqlite3 * _db;
};

class Sqlite_logic_error: public virtual std::logic_error, public Sqlite_error
{
public:
    Sqlite_logic_error(const std::string & what, const std::string & sql,
            int sqlite_error_code, sqlite3 * db);
    virtual ~Sqlite_logic_error() = default;
};

class Sqlite_runtime_error: public virtual std::runtime_error, public Sqlite_error
{
public:
    Sqlite_runtime_error(const std::string & what, const std::string & sql,
            int sqlite_error_code, sqlite3 * db);
    virtual ~Sqlite_runtime_error() = default;
};

# endif // SQLITE_ERROR_HPP
