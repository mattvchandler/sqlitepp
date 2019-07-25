// Sqlite statement library wrapper

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

#include <sqlitepp/sqlite.hpp>

#include <sqlitepp/error.hpp>

using namespace std::string_literals;

namespace sqlite
{
    Connection::Stmt::Stmt(const std::string & sql, Connection & db):
        db_(db.get_c_obj())
    {
        int status = sqlite3_prepare_v2(db.get_c_obj(), sql.c_str(), sql.length() + 1, &stmt_, NULL);

        if(status != SQLITE_OK)
        {
            throw Logic_error("Error parsing SQL: "s + sqlite3_errmsg(db.get_c_obj()), sql, status, db_);
        }
    }

    Connection::Stmt::~Stmt()
    {
        sqlite3_finalize(stmt_);
    }

    Connection::Stmt::Stmt(Connection::Stmt && other): stmt_{other.stmt_}, db_{other.db_}
    {
        other.stmt_ = nullptr;
    }

    Connection::Stmt & Connection::Stmt::operator=(Connection::Stmt && other)
    {
        if(&other != this)
        {
            sqlite3_finalize(stmt_);
            stmt_ = other.stmt_;
            db_ = other.db_;
            other.stmt_ = nullptr;
        }
        return *this;
    }


    void Connection::Stmt::bind(const int index, const double val)
    {
        int status = sqlite3_bind_double(stmt_, index, val);
        if(status != SQLITE_OK)
        {
            throw Logic_error("Error binding index " +
                std::to_string(index) + ": " + sqlite3_errmsg(db_), sqlite3_sql(stmt_), status, db_);
        }
    }

    void Connection::Stmt::bind(const int index, const int val)
    {
        int status = sqlite3_bind_int(stmt_, index, val);
        if(status != SQLITE_OK)
        {
            throw Logic_error("Error binding index " +
                std::to_string(index) + ": " + sqlite3_errmsg(db_), sqlite3_sql(stmt_), status, db_);
        }
    }

    void Connection::Stmt::bind(const int index, const sqlite3_int64 val)
    {
        int status = sqlite3_bind_int64(stmt_, index, val);
        if(status != SQLITE_OK)
        {
            throw Logic_error("Error binding index " +
                std::to_string(index) + ": " + sqlite3_errmsg(db_), sqlite3_sql(stmt_), status, db_);
        }
    }

    void Connection::Stmt::bind(const int index, const std::string & val)
    {
        int status = sqlite3_bind_text(stmt_, index, val.c_str(), val.length(), SQLITE_TRANSIENT);
        if(status != SQLITE_OK)
        {
            throw Logic_error("Error binding index " +
                std::to_string(index) + ": " + sqlite3_errmsg(db_), sqlite3_sql(stmt_), status, db_);
        }
    }

    void Connection::Stmt::bind(const int index, const char * val)
    {
        int status = sqlite3_bind_text(stmt_, index, val, -1, SQLITE_TRANSIENT);
        if(status != SQLITE_OK)
        {
            throw Logic_error("Error binding index " +
                std::to_string(index) + ": " + sqlite3_errmsg(db_), sqlite3_sql(stmt_), status, db_);
        }
    }

    void Connection::Stmt::bind_null(const int index)
    {
        int status = sqlite3_bind_null(stmt_, index);
        if(status != SQLITE_OK)
        {
            throw Logic_error("Error binding index " +
                std::to_string(index) + ": " + sqlite3_errmsg(db_), sqlite3_sql(stmt_), status, db_);
        }
    }

    void Connection::Stmt::bind(const int index)
    {
        bind_null(index);
    }

    void Connection::Stmt::bind(const std::string & name, const double val)
    {
        int status = sqlite3_bind_double(stmt_, bind_parameter_index(name), val);
        if(status != SQLITE_OK)
        {
            throw Logic_error("Error binding " + name +
                ": " + sqlite3_errmsg(db_), sqlite3_sql(stmt_), status, db_);
        }
    }

    void Connection::Stmt::bind(const std::string & name, const int val)
    {
        int status = sqlite3_bind_int(stmt_, bind_parameter_index(name), val);
        if(status != SQLITE_OK)
        {
            throw Logic_error("Error binding " + name +
                ": " + sqlite3_errmsg(db_), sqlite3_sql(stmt_), status, db_);
        }
    }

    void Connection::Stmt::bind(const std::string & name, const sqlite3_int64 val)
    {
        int status = sqlite3_bind_int64(stmt_, bind_parameter_index(name), val);
        if(status != SQLITE_OK)
        {
            throw Logic_error("Error binding " + name +
                ": " + sqlite3_errmsg(db_), sqlite3_sql(stmt_), status, db_);
        }
    }

    void Connection::Stmt::bind(const std::string & name, const std::string & val)
    {
        int status = sqlite3_bind_text(stmt_, bind_parameter_index(name), val.c_str(), val.length(), SQLITE_TRANSIENT);
        if(status != SQLITE_OK)
        {
            throw Logic_error("Error binding " + name +
                ": " + sqlite3_errmsg(db_), sqlite3_sql(stmt_), status, db_);
        }
    }

    void Connection::Stmt::bind(const std::string & name, const char * val)
    {
        int status = sqlite3_bind_text(stmt_, bind_parameter_index(name), val, -1, SQLITE_TRANSIENT);
        if(status != SQLITE_OK)
        {
            throw Logic_error("Error binding " + name +
                ": " + sqlite3_errmsg(db_), sqlite3_sql(stmt_), status, db_);
        }
    }

    void Connection::Stmt::bind_null(const std::string & name)
    {
        int status = sqlite3_bind_null(stmt_, bind_parameter_index(name));
        if(status != SQLITE_OK)
        {
            throw Logic_error("Error binding " + name +
                ": " + sqlite3_errmsg(db_), sqlite3_sql(stmt_), status, db_);
        }
    }

    void Connection::Stmt::bind(const std::string & name)
    {
        bind_null(name);
    }

    std::string Connection::Stmt::bind_parameter_name(const int index)
    {
        const char * name = sqlite3_bind_parameter_name(stmt_, index);
        if(!name)
        {
            throw Logic_error("Error looking up bind var name for index " +
                std::to_string(index) + ": " + sqlite3_errmsg(db_), sqlite3_sql(stmt_), SQLITE_ERROR, db_);
        }
        return std::string(name);
    }

    int Connection::Stmt::bind_parameter_index(const std::string & name)
    {
        int index = sqlite3_bind_parameter_index(stmt_, name.c_str());
        if(!index)
        {
            throw Logic_error("Error looking up index for bind var " +
                name + ": " + sqlite3_errmsg(db_), sqlite3_sql(stmt_), SQLITE_ERROR, db_);
        }
        return index;
    }

    int Connection::Stmt::bind_parameter_count()
    {
        return sqlite3_bind_parameter_count(stmt_);
    }

    bool Connection::Stmt::step()
    {
        int status = sqlite3_step(stmt_);
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
                sqlite3_errmsg(db_), sqlite3_sql(stmt_), status, db_);
        }
    }

    /// @name Get columns
    /// get_col() template specializations
    /// @{

    /// @copydoc sqlite::Connection::Stmt::get_col()
    template<>
    double Connection::Stmt::get_col<double>(const int column)
    {
        return sqlite3_column_double(stmt_, column);
    }

    /// @copydoc sqlite::Connection::Stmt::get_col()
    template<>
    int Connection::Stmt::get_col<int>(const int column)
    {
        return sqlite3_column_int(stmt_, column);
    }

    /// @copydoc sqlite::Connection::Stmt::get_col()
    template<>
    sqlite3_int64 Connection::Stmt::get_col<sqlite3_int64>(const int column)
    {
        return sqlite3_column_int64(stmt_, column);
    }

    /// @copydoc sqlite::Connection::Stmt::get_col()
    /// @memberof sqlite::Connection::Stmt
    template<>
    std::string Connection::Stmt::get_col<std::string>(const int column)
    {
        const char * str = reinterpret_cast<const char *>(sqlite3_column_text(stmt_, column));

        if(!str)
            return ""s; // empty str for NULL data
        else
            return std::string(str);
    }

    /// @copydoc sqlite::Connection::Stmt::get_col()
    template<>
    const char * Connection::Stmt::get_col<const char *>(const int column)
    {
        return reinterpret_cast<const char *>(sqlite3_column_text(stmt_, column));
    }

    /// @}

    void Connection::Stmt::reset()
    {
        int status = sqlite3_reset(stmt_);
        if(status != SQLITE_OK)
        {
            throw Logic_error("Error resetting statement: "s +
                sqlite3_errmsg(db_), sqlite3_sql(stmt_), status, db_);
        }
    }

    void Connection::Stmt::clear_bindings()
    {
        sqlite3_clear_bindings(stmt_);
    }

    bool Connection::Stmt::busy()
    {
        return bool(sqlite3_stmt_busy(stmt_));
    }

    bool Connection::Stmt::readonly()
    {
        return bool(sqlite3_stmt_readonly(stmt_));
    }

    const sqlite3_stmt * Connection::Stmt::get_c_obj() const
    {
        return stmt_;
    }

    sqlite3_stmt * Connection::Stmt::get_c_obj()
    {
        return stmt_;
    }
};
