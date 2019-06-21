// Exception types for sqlite errors

// Copyright 2019 Matthew Chandler

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

#include <sqlitepp/error.hpp>

namespace sqlite
{
    Error::~Error()
    {
    }

    const char * Error::sql() const noexcept
    {
        return sql_.c_str();
    }

    int Error::err_code() const noexcept
    {
        return sqlite_error_code_;
    }

    const char * Error::err_str() const noexcept
    {
        return sqlite3_errstr(err_code());
    }

    const char * Error::err_msg() const noexcept
    {
        if(!db_)
            return "";

        return sqlite3_errmsg(db_);
    }

    Error::Error(const std::string & sql, int sqlite_error_code, sqlite3 * db):
        sql_(sql),
        sqlite_error_code_(sqlite_error_code),
        db_(db)
    {
    }

    Logic_error::Logic_error(const std::string & what, const std::string & sql,
            int sqlite_error_code, sqlite3 * db):
        std::logic_error(what),
        Error(sql, sqlite_error_code, db)
    {
    }

    Runtime_error::Runtime_error(const std::string & what, const std::string & sql,
            int sqlite_error_code, sqlite3 * db):
        std::runtime_error(what),
        Error(sql, sqlite_error_code, db)
    {
    }
};
