/// @file
/// Sqlite library wrapper

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

#include "sqlite/sqlite_error.hpp"

namespace sqlite
{
    /// Open / create new DB

    /// @param[in] filename Path to sqlite databse file
    Connection::Connection(const std::string & filename)
    {
        int status = sqlite3_open(filename.c_str(), &_db);
        if(status != SQLITE_OK)
        {
            sqlite3_close(_db);
            throw Runtime_error("Error connecting to db (" +
                filename + "): " + sqlite3_errmsg(_db), "", status, nullptr);
        }
        sqlite3_extended_result_codes(_db, true);
    }

    Connection::~Connection()
    {
        sqlite3_close(_db);
    }

    /// Create a new prepared statement

    /// @param[in] sql SQL code to prepare
    /// @return Prepared statement for the SQL code input
    Connection::Stmt Connection::create_statement(const std::string & sql)
    {
        return Stmt(sql, *this);
    }

    /// Execute SQL statement(s)

    /// Will execute the SQL in-place, without needing to create a Connection::Stmt object.
    /// @param[in] sql SQL code to execute
    /// @param[in] callback Function to call for every row. May be NULL. Parameters are:
    /// - arg: The arg parameter from the exec call
    /// - column_data: Array of column data (as strings) for the current row
    /// - column_names: Array of column names
    /// @param[in,out] arg Data to pass as 1st arg of callback. May be NULL.
    void Connection::exec(const std::string & sql, int (*callback)(void *, int, char **, char **), void * arg)
    {
        char * err_msg = nullptr;
        int status = sqlite3_exec(_db, sql.c_str(), callback, arg, &err_msg);

        if(err_msg || status != SQLITE_OK)
        {
            std::string err = "";
            if(err_msg)
            {
                err = err_msg;
                sqlite3_free(err_msg);
            }
            throw Logic_error("Error evaluating SQL: " + err, sql, status, _db);
        }
    }

    /// Start a transaction
    void Connection::begin_transaction()
    {
        exec("BEGIN TRANSACTION;");
    }

    /// End a transaction & commit
    void Connection::commit()
    {
        exec("COMMIT;");
    }

    /// End a transaction & rollback
    void Connection::rollback()
    {
        exec("ROLLBACK;");
    }

    /// Get wrapped C sqlite3 object (for use with the sqlite <a href=https://www.sqlite.org/c3ref/intro.html>C API</a> - we don't wrap it all)

    /// @returns C sqlite3 object
    const sqlite3 * Connection::operator()() const
    {
        return _db;
    }

    sqlite3 * Connection::operator()()
    {
        return _db;
    }
};
