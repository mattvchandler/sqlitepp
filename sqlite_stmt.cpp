/// @file
/// @brief Sqlite statement library wrapper

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

using namespace std::string_literals;

namespace sqlite
{
    Connection::Stmt::Stmt(const std::string & sql, Connection & db):
        _db(db.get_c_obj())
    {
        int status = sqlite3_prepare_v2(db.get_c_obj(), sql.c_str(), sql.length() + 1, &_stmt, NULL);

        if(status != SQLITE_OK)
        {
            throw Logic_error("Error parsing SQL: "s + sqlite3_errmsg(db.get_c_obj()), sql, status, _db);
        }
    }

    Connection::Stmt::~Stmt()
    {
        sqlite3_finalize(_stmt);
    }

    void Connection::Stmt::bind_null(const int index)
    {
        int status = sqlite3_bind_null(_stmt, index);
        if(status != SQLITE_OK)
        {
            throw Logic_error("Error binding index " +
                std::to_string(index) + ": " + sqlite3_errmsg(_db), sqlite3_sql(_stmt), status, _db);
        }
    }

    /// @todo TODO: use grouping to tie bind and get_col together

    void Connection::Stmt::bind(const int index)
    {
        bind_null(index);
    }

    void Connection::Stmt::bind(const int index, const int val)
    {
        int status = sqlite3_bind_int(_stmt, index, val);
        if(status != SQLITE_OK)
        {
            throw Logic_error("Error binding index " +
                std::to_string(index) + ": " + sqlite3_errmsg(_db), sqlite3_sql(_stmt), status, _db);
        }
    }

    void Connection::Stmt::bind(const int index, const sqlite3_int64 val)
    {
        int status = sqlite3_bind_int64(_stmt, index, val);
        if(status != SQLITE_OK)
        {
            throw Logic_error("Error binding index " +
                std::to_string(index) + ": " + sqlite3_errmsg(_db), sqlite3_sql(_stmt), status, _db);
        }
    }

    void Connection::Stmt::bind(const int index, const double val)
    {
        int status = sqlite3_bind_double(_stmt, index, val);
        if(status != SQLITE_OK)
        {
            throw Logic_error("Error binding index " +
                std::to_string(index) + ": " + sqlite3_errmsg(_db), sqlite3_sql(_stmt), status, _db);
        }
    }

    void Connection::Stmt::bind(const int index, const std::string & val)
    {
        int status = sqlite3_bind_text(_stmt, index, val.c_str(), val.length(), SQLITE_TRANSIENT);
        if(status != SQLITE_OK)
        {
            throw Logic_error("Error binding index " +
                std::to_string(index) + ": " + sqlite3_errmsg(_db), sqlite3_sql(_stmt), status, _db);
        }
    }

    void Connection::Stmt::bind(const int index, const sqlite3_value * val)
    {
        int status = sqlite3_bind_value(_stmt, index, val);
        if(status != SQLITE_OK)
        {
            throw Logic_error("Error binding index " +
                std::to_string(index) + ": " + sqlite3_errmsg(_db), sqlite3_sql(_stmt), status, _db);
        }
    }

    void Connection::Stmt::bind_null(const std::string & name)
    {
        int status = sqlite3_bind_null(_stmt, bind_parameter_index(name));
        if(status != SQLITE_OK)
        {
            throw Logic_error("Error binding " + name +
                ": " + sqlite3_errmsg(_db), sqlite3_sql(_stmt), status, _db);
        }
    }

    void Connection::Stmt::bind(const std::string & name)
    {
        bind_null(name);
    }

    void Connection::Stmt::bind(const std::string & name, const int val)
    {
        int status = sqlite3_bind_int(_stmt, bind_parameter_index(name), val);
        if(status != SQLITE_OK)
        {
            throw Logic_error("Error binding " + name +
                ": " + sqlite3_errmsg(_db), sqlite3_sql(_stmt), status, _db);
        }
    }

    void Connection::Stmt::bind(const std::string & name, const sqlite3_int64 val)
    {
        int status = sqlite3_bind_int64(_stmt, bind_parameter_index(name), val);
        if(status != SQLITE_OK)
        {
            throw Logic_error("Error binding " + name +
                ": " + sqlite3_errmsg(_db), sqlite3_sql(_stmt), status, _db);
        }
    }

    void Connection::Stmt::bind(const std::string & name, const double val)
    {
        int status = sqlite3_bind_double(_stmt, bind_parameter_index(name), val);
        if(status != SQLITE_OK)
        {
            throw Logic_error("Error binding " + name +
                ": " + sqlite3_errmsg(_db), sqlite3_sql(_stmt), status, _db);
        }
    }

    void Connection::Stmt::bind(const std::string & name, const std::string & val)
    {
        int status = sqlite3_bind_text(_stmt, bind_parameter_index(name), val.c_str(), val.length(), SQLITE_TRANSIENT);
        if(status != SQLITE_OK)
        {
            throw Logic_error("Error binding " + name +
                ": " + sqlite3_errmsg(_db), sqlite3_sql(_stmt), status, _db);
        }
    }

    void Connection::Stmt::bind(const std::string & name, const sqlite3_value * val)
    {
        int status = sqlite3_bind_value(_stmt, bind_parameter_index(name), val);
        if(status != SQLITE_OK)
        {
            throw Logic_error("Error binding " + name +
                ": " + sqlite3_errmsg(_db), sqlite3_sql(_stmt), status, _db);
        }
    }

    std::string Connection::Stmt::bind_parameter_name(const int index)
    {
        const char * name = sqlite3_bind_parameter_name(_stmt, index);
        if(!name)
        {
            throw Logic_error("Error looking up bind var name for index " +
                std::to_string(index) + ": " + sqlite3_errmsg(_db), sqlite3_sql(_stmt), SQLITE_ERROR, _db);
        }
        return std::string(name);
    }

    int Connection::Stmt::bind_parameter_index(const std::string & name)
    {
        int index = sqlite3_bind_parameter_index(_stmt, name.c_str());
        if(!index)
        {
            throw Logic_error("Error looking up index for bind var " +
                name + ": " + sqlite3_errmsg(_db), sqlite3_sql(_stmt), SQLITE_ERROR, _db);
        }
        return index;
    }

    bool Connection::Stmt::step()
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
            throw Logic_error("Error evaluating SQL: "s +
                sqlite3_errmsg(_db), sqlite3_sql(_stmt), status, _db);
        }
    }

    /// @todo TODO: wrap remaining supported types

    /// Get SELECTed column as int

    /// @param[in] column Column number
    /// - Unlike bind() indexes, column indexes start at 0
    /// @returns Column data for the current row
    /// @warning Result is undefined if the statemnt does not point ot a vaild row,
    /// or if the column index is out of range.
    template<>
    int Connection::Stmt::get_col<int>(const int column)
    {
        return sqlite3_column_int(_stmt, column);
    }

    /// Get SELECTed column as sqlite3_int64

    /// @param[in] column Column number
    /// - Unlike bind() indexes, column indexes start at 0
    /// @returns Column data for the current row
    /// @warning Result is undefined if the statemnt does not point ot a vaild row,
    /// or if the column index is out of range.
    template<>
    sqlite3_int64 Connection::Stmt::get_col<sqlite3_int64>(const int column)
    {
        return sqlite3_column_int64(_stmt, column);
    }

    /// Get SELECTed column as double

    /// @param[in] column Column number
    /// - Unlike bind() indexes, column indexes start at 0
    /// @returns Column data for the current row
    /// @warning Result is undefined if the statemnt does not point ot a vaild row,
    /// or if the column index is out of range.
    template<>
    double Connection::Stmt::get_col<double>(const int column)
    {
        return sqlite3_column_double(_stmt, column);
    }

    /// Get SELECTed column as string

    /// @param[in] column Column number
    /// - Unlike bind() indexes, column indexes start at 0
    /// @returns Column data for the current row
    /// @memberof Connection::Stmt
    /// @warning Result is undefined if the statemnt does not point ot a vaild row,
    /// or if the column index is out of range.
    template<>
    std::string Connection::Stmt::get_col<std::string>(const int column)
    {
        const char * str = reinterpret_cast<const char *>(sqlite3_column_text(_stmt, column));

        if(!str)
            return ""s; // empty str for NULL data
        else
            return std::string(str);
    }

    /// @todo TODO: search code for where using const char * instead of string may be more efficient

    /// Get SELECTed column as C string

    /// @param[in] column Column number
    /// - Unlike bind() indexes, column indexes start at 0
    /// @returns Column data for the current row
    /// @warning Result is undefined if the statemnt does not point ot a vaild row,
    /// or if the column index is out of range.
    template<>
    const char * Connection::Stmt::get_col<const char *>(const int column)
    {
        return reinterpret_cast<const char *>(sqlite3_column_text(_stmt, column));
    }

    /// @todo TODO: create wrapper for sqlite3_value?

    /// Get SELECTed column as sqlite3_value

    /// @param[in] column Column number
    /// - Unlike bind() indexes, column indexes start at 0
    /// @returns Column data for the current row
    /// @warning Result is undefined if the statemnt does not point ot a vaild row,
    /// or if the column index is out of range.
    template<>
    sqlite3_value * Connection::Stmt::get_col<sqlite3_value *>(const int column)
    {
        return sqlite3_column_value(_stmt, column);
    }

    void Connection::Stmt::reset()
    {
        int status = sqlite3_reset(_stmt);
        if(status != SQLITE_OK)
        {
            throw Logic_error("Error resetting statement: "s +
                sqlite3_errmsg(_db), sqlite3_sql(_stmt), status, _db);
        }
    }

    void Connection::Stmt::clear_bindings()
    {
        sqlite3_clear_bindings(_stmt);
    }

    const sqlite3_stmt * Connection::Stmt::get_c_obj() const
    {
        return _stmt;
    }

    sqlite3_stmt * Connection::Stmt::get_c_obj()
    {
        return _stmt;
    }
};
