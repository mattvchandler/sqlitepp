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

#ifndef SQLITE_HPP
#define SQLITE_HPP

#include <string>

#include <sqlite3.h>

/// @todo TODO: wrap entire sqlite3 library?

/// Sqlite C++ wrapper and associated types

/// @ingroup sqlite
namespace sqlite
{
    /// Sqlite database connection

    /// Holds a connection to a database. SQL may be run with either the exec() method,
    /// or by using create_statement() to build a Connection::Stmt object
    class Connection final
    {
    public:
        class Stmt;

        /// Open / create new DB

        /// @param[in] filename Path to sqlite database file
        Connection(const std::string & filename);
        ~Connection();

        // non-copyable
        Connection(const Connection &) = delete;
        Connection & operator=(const Connection &) = delete;

        // need to explicitly say we want default move ctors
        /// Move constructor
        Connection(Connection &&) = default;
        /// Move constructor
        Connection & operator=(Connection &&) = default;

        /// Create a new prepared statement

        /// @param[in] sql SQL code to prepare
        /// @return Prepared statement for the SQL code input
        Stmt create_statement(const std::string & sql);

        /// Execute SQL statement(s)

        /// Will execute the SQL in-place, without needing to create a Connection::Stmt object.
        /// @param[in] sql SQL code to execute
        /// @param[in] callback Function to call for every row. May be NULL. Parameters are:
        /// - arg: The arg parameter from the exec call
        /// - column_data: Array of column data (as strings) for the current row
        /// - column_names: Array of column names
        /// @param[in,out] arg Data to pass as 1st arg of callback. May be NULL.
        void exec(const std::string & sql, int (*callback)(void *, int, char **, char **) = nullptr,
            void * arg = nullptr);

        /// Start a transaction
        void begin_transaction();

        /// End a transaction & commit
        void commit();

        /// End a transaction & rollback
        void rollback();

        /// Get wrapped C sqlite3 object (for use with the sqlite <a href=https://www.sqlite.org/c3ref/intro.html>C API</a>)

        /// @returns C sqlite3 object
        const sqlite3 * get_c_obj() const;

        /// Get wrapped C sqlite3 object (for use with the sqlite <a href=https://www.sqlite.org/c3ref/intro.html>C API</a>)

        /// @returns C sqlite3 object
        sqlite3 * get_c_obj();

    private:
        /// sqlite C API's DB connection obj
        sqlite3 * _db = nullptr;
    };

    /// Prepared statement obj - usually created by Connection::create_statement

    /// Data is bound to the statement with the bind() overloads, step() is called
    /// to execute the statement, and then get_col() can be used to retrieve rows
    /// from a SELECT statement.
    ///
    /// When INSERTing or UPDATing multiple rows, call reset to reuse the statement obj.
    class Connection::Stmt final
    {
    public:
        /// Prepare a new statement for the given SQL

        /// It is usually easier to use Connection::create_statement instead of this
        /// @param[in] sql SQL code to prepare
        /// @param[in] db Database Connection to prepare statement for
        Stmt(const std::string & sql, Connection & db);
        ~Stmt();

        // non-copyable
        Stmt(const Stmt &) = delete;
        Stmt & operator=(const Stmt &) = delete;

        // need to explicit say we want default move ctors
        /// Move constructor
        Stmt(Stmt &&) = default;
        /// Move constructor
        Stmt & operator=(Stmt &&) = default;

        /// @todo TODO: bind and get remaining sqlite3 types, including const char *, possibly cast for int64_t

        /// Bind null by index

        /// @note As in the sqlite C API, bind var indexes start at 1
        /// @param[in] index Bind variable index
        void bind_null(const int index);

        /// Bind null by index

        /// @note As in the sqlite C API, bind var indexes start at 1
        /// @param[in] index Bind variable index
        void bind(const int index);

        /// Bind int var by index

        /// @note As in the sqlite C API, bind var indexes start at 1
        /// @param[in] index Bind variable index
        /// @param[in] val Bind variable value
        void bind(const int index, const int val);

        /// Bind sqlite3_int64 var by index

        /// @note As in the sqlite C API, bind var indexes start at 1
        /// @param[in] index Bind variable index
        /// @param[in] val Bind variable value
        void bind(const int index, const sqlite3_int64 val);

        /// Bind double var by index

        /// @note As in the sqlite C API, bind var indexes start at 1
        /// @param[in] index Bind variable index
        /// @param[in] val Bind variable value
        void bind(const int index, const double val);

        /// Bind string var by index

        /// @note As in the sqlite C API, bind var indexes start at 1
        /// @param[in] index Bind variable index
        /// @param[in] val Bind variable value
        void bind(const int index, const std::string & val);

        /// Bind sqlite3_value var by index

        /// @note As in the sqlite C API, bind var indexes start at 1
        /// @param[in] index Bind variable index
        /// @param[in] val Bind variable value
        void bind(const int index, const sqlite3_value * val);

        /// Bind null by name

        /// @param[in] name Bind variable name
        void bind_null(const std::string & name);

        /// Bind null by name

        /// @param[in] name Bind variable name
        void bind(const std::string & name);

        /// Bind int var by name

        /// @param[in] name Bind variable name
        /// @param[in] val Bind variable value
        void bind(const std::string & name, const int val);

        /// Bind sqlite3_int64 var by name

        /// @param[in] name Bind variable name
        /// @param[in] val Bind variable value
        void bind(const std::string & name, const sqlite3_int64 val);

        /// Bind double var by name

        /// @param[in] name Bind variable name
        /// @param[in] val Bind variable value
        void bind(const std::string & name, const double val);

        /// Bind string string var by name

        /// @param[in] name Bind variable name
        /// @param[in] val Bind variable value
        void bind(const std::string & name, const std::string & val);

        /// Bind sqlite3_value var by name

        /// @param[in] name Bind variable name
        /// @param[in] val Bind variable value
        void bind(const std::string & name, const sqlite3_value * val);

        /// Get bind var name from index

        /// @param[in] index Bind variable index
        /// @returns Bind variable name
        std::string bind_parameter_name(const int index);

        /// Get bind var index by name

        /// @param[in] name Bind variable name
        /// @returns Bind variable index
        int bind_parameter_index(const std::string & name);

        /// Run the statement.

        /// @returns
        /// - \c true on SELECT statements when more rows remain to be fetched
        /// - \c false for UPDATE, DELETE, or database commands, or when no more rows can be SELECTED
        bool step();

        /// Get SELECTed column

        /// @param[in] column Column number
        /// - Unlike bind() indexes, column indexes start at 0
        /// @returns Column data for the current row
        template<typename T>
        T get_col(const int column);

        /// Reset the statement

        /// Useful for inserting or updating multiple rows
        void reset();

        /// Clear all bind vars to NULL
        void clear_bindings();

        /// Get wrapped C sqlite3_stmt object (for use with the sqlite <a href=https://www.sqlite.org/c3ref/intro.html>C API</a>)

        /// @returns C sqlite3_stmt object
        const sqlite3_stmt * get_c_obj() const;

        /// Get wrapped C sqlite3_stmt object (for use with the sqlite <a href=https://www.sqlite.org/c3ref/intro.html>C API</a>)

        /// @returns C sqlite3_stmt object
        sqlite3_stmt * get_c_obj();

    private:
        /// Sqlite C API's prepared statement obj
        sqlite3_stmt * _stmt = nullptr;
        /// Copy of sqlite DB connection obj
        sqlite3 * _db = nullptr;
    };
};
# endif // SQLITE_HPP
