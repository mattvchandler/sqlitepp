/// @file
/// @brief Sqlite library wrapper

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

    Connection::Stmt Connection::create_statement(const std::string & sql)
    {
        return Stmt(sql, *this);
    }

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

    void Connection::begin_transaction()
    {
        exec("BEGIN TRANSACTION;");
    }

    void Connection::commit()
    {
        exec("COMMIT;");
    }

    void Connection::rollback()
    {
        exec("ROLLBACK;");
    }

    const sqlite3 * Connection::get_c_obj() const
    {
        return _db;
    }

    sqlite3 * Connection::get_c_obj()
    {
        return _db;
    }
};
