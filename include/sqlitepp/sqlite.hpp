/// @file
/// @brief Sqlite library wrapper

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

#ifndef SQLITE_HPP
#define SQLITE_HPP

#include <string>

#include <sqlitepp/sqlite3.h>

/// Sqlite C++ wrapper and associated types

/// @ingroup sqlite
/// @sa [C API](https://www.sqlite.org/c3ref/intro.html)
namespace sqlite
{
    /// Sqlite database connection

    /// Holds a connection to a database. SQL may be run with either the exec() method,
    /// or by using create_statement() to build a Connection::Stmt object
    /// @sa [C API](https://www.sqlite.org/c3ref/sqlite3.html)
    class Connection final
    {
    public:
        class Stmt;

        /// Open / create new DB

        /// @param[in] filename Path to sqlite database file
        /// @exception Runtime_error on error connecting to DB
        /// @sa [C API](https://www.sqlite.org/c3ref/open.html)
        explicit Connection(const std::string & filename);
        ~Connection();

        // non-copyable
        Connection(const Connection &) = delete;
        Connection & operator=(const Connection &) = delete;

        // need to explicitly say we want default move ctors
        Connection(Connection &&) = default;
        Connection & operator=(Connection &&) = default;

        /// Create a new prepared statement

        /// @param[in] sql SQL code to prepare
        /// @return Prepared statement for the SQL code input
        /// @exception Logic_error on error parsing SQL
        /// @sa [C API](https://www.sqlite.org/c3ref/prepare.html)
        Stmt create_statement(const std::string & sql);

        /// Execute SQL statement(s)

        /// Will execute the SQL in-place, without needing to create a Connection::Stmt object.
        /// @param[in] sql SQL code to execute
        /// @param[in] callback Function to call for every row. May be NULL. Parameters are:
        /// - arg: The arg parameter from the exec call
        /// - column_data: Array of column data (as strings) for the current row
        /// - column_names: Array of column names
        /// @param[in,out] arg Data to pass as 1st arg of callback. May be NULL.
        /// @exception Logic_error on error evaluating SQL
        /// @sa [C API](https://www.sqlite.org/c3ref/exec.html)
        void exec(const std::string & sql, int (*callback)(void *, int, char **, char **) = nullptr,
            void * arg = nullptr);

        /// Start a transaction

        /// @sa [C API](https://www.sqlite.org/lang_transaction.html)
        void begin_transaction();

        /// End a transaction & commit

        /// @sa [C API](https://www.sqlite.org/lang_transaction.html)
        void commit();

        /// End a transaction & rollback

        /// @sa [C API](https://www.sqlite.org/lang_transaction.html)
        void rollback();

        /// Interrupt a long-running query

        /// @sa [C API](https://www.sqlite.org/c3ref/interrupt.html)
        void interrupt();

        /// Get last INSERTed Row ID

        /// @ returns Row ID for last INSERTed row
        /// @sa [C API](https://www.sqlite.org/c3ref/last_insert_rowid.html)
        sqlite3_int64 last_insert_rowid();

        /// Get total number of rows modified

        /// @returns Total number of rows inserted, modified or deleted by all
        /// INSERT, UPDATE or DELETE statements completed since the database
        /// connection was opened, including those executed as part of trigger
        /// programs.
        /// @sa [C API](https://www.sqlite.org/c3ref/total_changes.html)
        int total_changes();


        /// Column metadata info

        /// This is the return type for table_column_metadata()
        struct Column_metadata
        {
            std::string type; ///< Column's declared data type
            std::string collation; ///< Name of default collation sequence
            bool not_null = false; ///< \c true if the column has a NOT NULL constraint
            bool primary_key = false; ///< \c true if column is part of the PRIMARY KEY
            bool auto_inc = false; ///< \c true True if column is AUTOINCREMENT
        };

        /// Get column metadata

        /// @param[in] table_name Table name
        /// @param[in] column_name Column name
        /// @param[in] db_name DB name, or \c "main" if omitted
        /// @returns The chosen column's metadata
        /// @exception Runtime_error on error looking up info
        /// @sa [C API](https://www.sqlite.org/c3ref/table_column_metadata.html)
        Column_metadata table_column_metadata(const std::string & table_name, const std::string & column_name,
            const std::string & db_name = "main");

        /// Get wrapped C sqlite3 object (for use with the sqlite [C API](https://www.sqlite.org/c3ref/intro.html))

        /// @returns C sqlite3 object
        const sqlite3 * get_c_obj() const;

        /// Get wrapped C sqlite3 object (for use with the sqlite [C API](https://www.sqlite.org/c3ref/intro.html))

        /// @returns C sqlite3 object
        sqlite3 * get_c_obj();

    private:
        /// sqlite C API's DB connection obj
        sqlite3 * db_ = nullptr;
    };

    /// Prepared statement obj - usually created by Connection::create_statement

    /// Data is bound to the statement with the bind() overloads, step() is called
    /// to execute the statement, and then get_col() can be used to retrieve rows
    /// from a SELECT statement.
    ///
    /// When INSERTing or UPDATing multiple rows, call reset() to reuse the statement obj.
    /// @sa [C API](https://www.sqlite.org/c3ref/stmt.html)
    class Connection::Stmt final
    {
    public:
        /// Prepare a new statement for the given SQL

        /// It is usually easier to use Connection::create_statement instead of this
        /// @param[in] sql SQL code to prepare
        /// @param[in] db Database Connection to prepare statement for
        /// @exception Logic_error on error parsing SQL
        /// @sa [C API](https://www.sqlite.org/c3ref/prepare.html)
        Stmt(const std::string & sql, Connection & db);
        ~Stmt();

        // non-copyable
        Stmt(const Stmt &) = delete;
        Stmt & operator=(const Stmt &) = delete;

        // need to explicit say we want default move ctors
        Stmt(Stmt &&) = default;
        Stmt & operator=(Stmt &&) = default;

        /// @name Bind functions
        /// Functions for binding data to SQL statements
        /// @{

        /// Bind var by index

        /// @note As in the sqlite C API, bind var indexes start at 1
        /// @param[in] index Bind variable index
        /// @param[in] val Bind variable value
        /// @exception Logic_error on error binding
        /// @sa [C API](https://www.sqlite.org/c3ref/bind_blob.html)
        void bind(const int index, const double val);

        /// @overload bind(const int, const int)
        void bind(const int index, const int val);

        /// @overload bind(const int, const int)
        void bind(const int index, const sqlite3_int64 val);

        /// @overload bind(const int, const int)
        void bind(const int index, const std::string & val);

        /// @overload bind(const int, const int)
        void bind(const int index, const char * val);

        /// Bind null by index

        /// @note As in the sqlite C API, bind var indexes start at 1
        /// @param[in] index Bind variable index
        /// @exception Logic_error on error binding
        /// @sa [C API](https://www.sqlite.org/c3ref/bind_blob.html)
        void bind_null(const int index);

        /// @copydoc bind_null(const int)
        void bind(const int index);

        /// Bind var by name

        /// @param[in] name Bind variable name
        /// @param[in] val Bind variable value
        /// @exception Logic_error on error binding
        /// @sa [C API](https://www.sqlite.org/c3ref/bind_blob.html)
        void bind(const std::string & name, const double val);

        /// @overload bind(const std::string &, const int)
        void bind(const std::string & name, const int val);

        /// @overload bind(const std::string &, const int)
        void bind(const std::string & name, const sqlite3_int64 val);

        /// @overload bind(const std::string &, const int)
        void bind(const std::string & name, const std::string & val);

        /// @overload bind(const std::string &, const int)
        void bind(const std::string & name, const char * val);

        /// Bind null by name

        /// @param[in] name Bind variable name
        /// @exception Logic_error on error binding
        /// @sa [C API](https://www.sqlite.org/c3ref/bind_blob.html)
        void bind_null(const std::string & name);

        /// @copydoc bind_null(const std::string &)
        void bind(const std::string & name);

        /// @}

        /// Get bind var name from index

        /// @param[in] index Bind variable index
        /// @returns Bind variable name
        /// @exception Logic_error on error finding name
        /// @sa [C API](https://www.sqlite.org/c3ref/bind_parameter_name.html)
        std::string bind_parameter_name(const int index);

        /// Get bind var index by name

        /// @param[in] name Bind variable name
        /// @returns Bind variable index
        /// @exception Logic_error on error finding index
        /// @sa [C API](https://www.sqlite.org/c3ref/bind_parameter_index.html)
        int bind_parameter_index(const std::string & name);

        /// Get number of bind parameters

        /// @returns Number of bind parameters
        /// @sa [C API](https://www.sqlite.org/c3ref/bind_parameter_count.html)
        int bind_parameter_count();

        /// Run the statement.

        /// @returns
        /// - \c true on SELECT statements when more rows remain to be fetched
        /// - \c false for UPDATE, DELETE, or database commands, or when no more rows can be SELECTED
        /// @exception Logic_error on error evaluating SQL
        /// @sa [C API](https://www.sqlite.org/c3ref/step.html)
        bool step();

        /// Get SELECTed column

        /// @param[in] column Column number
        /// - Unlike bind() indexes, column indexes start at 0
        /// @returns Column data for the current row
        /// @note Only specific template types are allowed
        /// See documentation for template specializations.
        /// @sa [C API](https://www.sqlite.org/c3ref/column_blob.html)
        template<typename T>
        T get_col(const int column);

        /// Reset the statement

        /// Useful for inserting or updating multiple rows
        /// @exception Logic_error on error resetting
        /// @sa [C API](https://www.sqlite.org/c3ref/reset.html)
        void reset();

        /// Clear all bind vars to NULL

        /// @sa [C API](https://www.sqlite.org/c3ref/clear_bindings.html)
        void clear_bindings();

        /// Determine if the statment has been reset

        /// @returns \c true if the statement is busy (step has been called, but is not complete, nor reset)
        /// @sa [C API](https://www.sqlite.org/c3ref/stmt_busy.html)
        bool busy();

        /// Determine if the statment is read-only

        /// @returns \c true if the statement does not directly write to the DB
        /// @sa [C API](https://www.sqlite.org/c3ref/stmt_readonly.html)
        bool readonly();

        /// Get wrapped C sqlite3_stmt object (for use with the sqlite [C API](https://www.sqlite.org/c3ref/intro.html))

        /// @returns C sqlite3_stmt object
        const sqlite3_stmt * get_c_obj() const;

        /// Get wrapped C sqlite3_stmt object (for use with the sqlite [C API](https://www.sqlite.org/c3ref/intro.html))

        /// @returns C sqlite3_stmt object
        sqlite3_stmt * get_c_obj();

    private:
        /// Sqlite C API's prepared statement obj
        sqlite3_stmt * stmt_ = nullptr;
        /// Copy of sqlite DB connection obj
        sqlite3 * db_ = nullptr;
    };
};

# endif // SQLITE_HPP
