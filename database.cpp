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

#include "sqlite/database.hpp"

#include <stdexcept>

namespace sqlite
{
    /// Locate DB

    /// @returns DB connection
    /// @exception std::logic_error when not initialized
    Connection & Database::get()
    {
        if(!_db)
            throw std::logic_error("Database not initialized");

        return *_db;
    }

    /// Set DB

    /// @param[in] db (non-owning) Pointer to DB connection object, or \c nullptr to de-initialize
    void Database::init(Connection * db)
    {
        _db = db;
    }

    // init to null
    Connection * Database::_db = nullptr;
};
