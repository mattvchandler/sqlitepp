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

#include "sqlite/sqlite_error.hpp"

using namespace std::string_literals;

// prepare a new statement for the given SQL
Sqlite_db_conn::Stmt::Stmt(const std::string & sql, Sqlite_db_conn & db):
    _db(db())
{
    int status = sqlite3_prepare_v2(db(), sql.c_str(), sql.length() + 1, &_stmt, NULL);

    if(status != SQLITE_OK)
    {
        throw Sqlite_logic_error("Error parsing SQL: "s + sqlite3_errmsg(db()), sql, status, _db);
    }
}

Sqlite_db_conn::Stmt::~Stmt()
{
    sqlite3_finalize(_stmt);
}

// bind NULL to var by index
void Sqlite_db_conn::Stmt::bind_null(const int index)
{
    int status = sqlite3_bind_null(_stmt, index);
    if(status != SQLITE_OK)
    {
        throw Sqlite_logic_error("Error binding index " +
            std::to_string(index) + ": " + sqlite3_errmsg(_db), sqlite3_sql(_stmt), status, _db);
    }
}

// bind NULL to var by index
void Sqlite_db_conn::Stmt::bind(const int index)
{
    bind_null(index);
}

// bind double to var by index
void Sqlite_db_conn::Stmt::bind(const int index, const double val)
{
    int status = sqlite3_bind_double(_stmt, index, val);
    if(status != SQLITE_OK)
    {
        throw Sqlite_logic_error("Error binding index " +
            std::to_string(index) + ": " + sqlite3_errmsg(_db), sqlite3_sql(_stmt), status, _db);
    }
}

// bind int to var by index
void Sqlite_db_conn::Stmt::bind(const int index, const int val)
{
    int status = sqlite3_bind_int(_stmt, index, val);
    if(status != SQLITE_OK)
    {
        throw Sqlite_logic_error("Error binding index " +
            std::to_string(index) + ": " + sqlite3_errmsg(_db), sqlite3_sql(_stmt), status, _db);
    }
}

// bind sqlite3_int64 to var by index
void Sqlite_db_conn::Stmt::bind(const int index, const sqlite3_int64 val)
{
    int status = sqlite3_bind_int64(_stmt, index, val);
    if(status != SQLITE_OK)
    {
        throw Sqlite_logic_error("Error binding index " +
            std::to_string(index) + ": " + sqlite3_errmsg(_db), sqlite3_sql(_stmt), status, _db);
    }
}

// bind std::string to var by index
void Sqlite_db_conn::Stmt::bind(const int index, const std::string & val)
{
    int status = sqlite3_bind_text(_stmt, index, val.c_str(), val.length(), SQLITE_TRANSIENT);
    if(status != SQLITE_OK)
    {
        throw Sqlite_logic_error("Error binding index " +
            std::to_string(index) + ": " + sqlite3_errmsg(_db), sqlite3_sql(_stmt), status, _db);
    }
}

// bind sqlite3_value to var by index
void Sqlite_db_conn::Stmt::bind(const int index, const sqlite3_value * val)
{
    int status = sqlite3_bind_value(_stmt, index, val);
    if(status != SQLITE_OK)
    {
        throw Sqlite_logic_error("Error binding index " +
            std::to_string(index) + ": " + sqlite3_errmsg(_db), sqlite3_sql(_stmt), status, _db);
    }
}

// bind null to var by name
void Sqlite_db_conn::Stmt::bind_null(const std::string & name)
{
    int status = sqlite3_bind_null(_stmt, bind_parameter_index(name));
    if(status != SQLITE_OK)
    {
        throw Sqlite_logic_error("Error binding " + name +
            ": " + sqlite3_errmsg(_db), sqlite3_sql(_stmt), status, _db);
    }
}

// bind null to var by name
void Sqlite_db_conn::Stmt::bind(const std::string & name)
{
    bind_null(name);
}

// bind double to var by name
void Sqlite_db_conn::Stmt::bind(const std::string & name, const double val)
{
    int status = sqlite3_bind_double(_stmt, bind_parameter_index(name), val);
    if(status != SQLITE_OK)
    {
        throw Sqlite_logic_error("Error binding " + name +
            ": " + sqlite3_errmsg(_db), sqlite3_sql(_stmt), status, _db);
    }
}

// bind int to var by name
void Sqlite_db_conn::Stmt::bind(const std::string & name, const int val)
{
    int status = sqlite3_bind_int(_stmt, bind_parameter_index(name), val);
    if(status != SQLITE_OK)
    {
        throw Sqlite_logic_error("Error binding " + name +
            ": " + sqlite3_errmsg(_db), sqlite3_sql(_stmt), status, _db);
    }
}

// bind sqlite3_int64 to var by name
void Sqlite_db_conn::Stmt::bind(const std::string & name, const sqlite3_int64 val)
{
    int status = sqlite3_bind_int64(_stmt, bind_parameter_index(name), val);
    if(status != SQLITE_OK)
    {
        throw Sqlite_logic_error("Error binding " + name +
            ": " + sqlite3_errmsg(_db), sqlite3_sql(_stmt), status, _db);
    }
}

// bind std::string to var by name
void Sqlite_db_conn::Stmt::bind(const std::string & name, const std::string & val)
{
    int status = sqlite3_bind_text(_stmt, bind_parameter_index(name), val.c_str(), val.length(), SQLITE_TRANSIENT);
    if(status != SQLITE_OK)
    {
        throw Sqlite_logic_error("Error binding " + name +
            ": " + sqlite3_errmsg(_db), sqlite3_sql(_stmt), status, _db);
    }
}

// bind sqlite3_value to var by name
void Sqlite_db_conn::Stmt::bind(const std::string & name, const sqlite3_value * val)
{
    int status = sqlite3_bind_value(_stmt, bind_parameter_index(name), val);
    if(status != SQLITE_OK)
    {
        throw Sqlite_logic_error("Error binding " + name +
            ": " + sqlite3_errmsg(_db), sqlite3_sql(_stmt), status, _db);
    }
}

// get bind var name from index
std::string Sqlite_db_conn::Stmt::bind_parameter_name(const int index)
{
    const char * name = sqlite3_bind_parameter_name(_stmt, index);
    if(!name)
    {
        throw Sqlite_logic_error("Error looking up bind var name for index " +
            std::to_string(index) + ": " + sqlite3_errmsg(_db), sqlite3_sql(_stmt), SQLITE_ERROR, _db);
    }
    return std::string(name);
}

// get bind var index by name
int Sqlite_db_conn::Stmt::bind_parameter_index(const std::string & name)
{
    int index = sqlite3_bind_parameter_index(_stmt, name.c_str());
    if(!index)
    {
        throw Sqlite_logic_error("Error looking up index for bind var " +
            name + ": " + sqlite3_errmsg(_db), sqlite3_sql(_stmt), SQLITE_ERROR, _db);
    }
    return index;
}

// run the statement. for multi-row SELECTs, fetches one row, and returns true when no rows remain
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
        throw Sqlite_logic_error("Error evaluating SQL: "s +
            sqlite3_errmsg(_db), sqlite3_sql(_stmt), status, _db);
    }
}

// get SELECTed col as double
template<>
double Sqlite_db_conn::Stmt::get_col<double>(const int column)
{
    return sqlite3_column_double(_stmt, column);
}

// get SELECTed col as int
template<>
int Sqlite_db_conn::Stmt::get_col<int>(const int column)
{
    return sqlite3_column_int(_stmt, column);
}

// get SELECTed col as sqlite3_int64
template<>
sqlite3_int64 Sqlite_db_conn::Stmt::get_col<sqlite3_int64>(const int column)
{
    return sqlite3_column_int64(_stmt, column);
}

// get SELECTed col as std::string
template<>
std::string Sqlite_db_conn::Stmt::get_col<std::string>(const int column)
{
    const char * str = reinterpret_cast<const char *>(sqlite3_column_text(_stmt, column));

    if(!str)
        return ""s; // empty str for NULL data
    else
        return std::string(str);
}

// get SELECTed col as const char *
template<>
const char * Sqlite_db_conn::Stmt::get_col<const char *>(const int column)
{
    return reinterpret_cast<const char *>(sqlite3_column_text(_stmt, column));
}

// get SELECTed col as sqlite3_value *
template<>
sqlite3_value * Sqlite_db_conn::Stmt::get_col<sqlite3_value *>(const int column)
{
    return sqlite3_column_value(_stmt, column);
}

// reset the statment - useful for INSERTing multiple rows
void Sqlite_db_conn::Stmt::reset()
{
    int status = sqlite3_reset(_stmt);
    if(status != SQLITE_OK)
    {
        throw Sqlite_logic_error("Error resetting statement: "s +
            sqlite3_errmsg(_db), sqlite3_sql(_stmt), status, _db);
    }
}

// clear bind vars to NULL
void Sqlite_db_conn::Stmt::clear_bindings()
{
    sqlite3_clear_bindings(_stmt);
}

// get contained C obj (for use with C API - we don't wrap it all)
const sqlite3_stmt * Sqlite_db_conn::Stmt::operator()() const
{
    return _stmt;
}

sqlite3_stmt * Sqlite_db_conn::Stmt::operator()()
{
    return _stmt;
}
