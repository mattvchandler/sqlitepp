/// @file
/// Exception types for sqlite errors

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

/// @todo TODO: document exceptions

namespace sqlite
{
    Error::~Error()
    {
    }

    /// Get SQL code where error was thrown
    const char * Error::sql() const noexcept
    {
        return _sql.c_str();
    }

    /// Get sqlite3 error code
    int Error::err_code() const noexcept
    {
        return _sqlite_error_code;
    }

    /// Get a description of the sqlite3 error code
    const char * Error::err_str() const noexcept
    {
        return sqlite3_errstr(err_code());
    }

    /// Get the sqlite3 error message
    const char * Error::err_msg() const noexcept
    {
        if(!_db)
            return "";

        return sqlite3_errmsg(_db);
    }

    /// @param[in] sql Last SQL code ran
    /// @param[in] sqlite_error_code Sqlite3 extended error code
    /// @param[in] db Sqlite3 DB object
    Error::Error(const std::string & sql, int sqlite_error_code, sqlite3 * db):
        _sql(sql),
        _sqlite_error_code(sqlite_error_code),
        _db(db)
    {
    }

    /// @param[in] what %Error message
    /// @param[in] sql Last SQL code ran
    /// @param[in] sqlite_error_code Sqlite3 extended error code
    /// @param[in] db Sqlite3 DB object
    Logic_error::Logic_error(const std::string & what, const std::string & sql,
            int sqlite_error_code, sqlite3 * db):
        std::logic_error(what),
        Error(sql, sqlite_error_code, db)
    {
    }

    /// @param[in] what %Error message
    /// @param[in] sql Last SQL code ran
    /// @param[in] sqlite_error_code Sqlite3 extended error code
    /// @param[in] db Sqlite3 DB object
    Runtime_error::Runtime_error(const std::string & what, const std::string & sql,
            int sqlite_error_code, sqlite3 * db):
        std::runtime_error(what),
        Error(sql, sqlite_error_code, db)
    {
    }
};
