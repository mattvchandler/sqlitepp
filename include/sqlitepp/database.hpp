/// @file
/// @brief Service locator for database

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

#ifndef DATABSE_HPP
#define DATABSE_HPP

#include <sqlitepp/sqlite.hpp>

/// @ingroup sqlite
namespace sqlite
{
    /// Sqlite Database service locator (Static class)
    class Database final
    {
    public:
        // make noncreatable
        Database() = delete;
        ~Database() = delete;

        /// Locate DB

        /// @returns DB connection
        /// @exception std::logic_error when not initialized
        static Connection & get();

        /// Set DB

        /// @param[in] db (non-owning) Pointer to DB connection object, or \c nullptr to de-initialize
        static void init(Connection * db);

    private:
        /// (non-owned) DB connection
        static Connection * db_;
    };
};

# endif // DATABSE_HPP
