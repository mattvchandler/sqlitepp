# Sqlitepp - a thin C++ wrapper library for [Sqlite](https://www.sqlite.org/)

Sqlitepp is a simple wrapper for [sqlite](https://www.sqlite.org). It simply
adds RAII to the most commonly used sqlite types and a few convenience methods
for their most used functionality. Sqlitepp types have a get_c_obj() method to
gain access to the underlying sqlite type, which allows you to use any part of
the sqlite C library not wrapped by sqlitepp. That way you can take advantage of
sqlitepp without losing any functionality.

### [Sqlitepp on Github](https://github.com/mattvchandler/sqlitepp)

### [Documentation](https://mattvchandler.github.io/sqlitepp/index.html)

## Structure

The library is split into 4 modules:

### sqlite::Connection (corresponds to sqlite's [sqlite3](https://www.sqlite.org/c3ref/sqlite3.html) type)

Represents the sqlite database connection itself.

### sqlite::Connection::Stmt (corresponds to sqlite's [sqlite3_stmt](https://www.sqlite.org/c3ref/stmt.html) type)

A prepared SQL statement. Can be created directly, or from
sqlite::Connection::create_statement

### sqlite::Error, sqlite::Runtime_error, sqlite::Logic_error

Exception types thrown from Connection and Stmt. sqlite::Error is an abstract
type, and should not be directly created, but may be caught.
sqlite::Runtime_error and sqlite::Logic_error inherit from c++'s
std::runtime_error and std::logic_error, respectively.

### sqlite::Database

A simple [service locator](https://en.wikipedia.org/wiki/Service_locator_pattern)
object for use with Connection. This is provided as a convinence, and is not
required to use the rest of the library.

## Building & Installation

### Dependencies

* Sqlite

Sqlitepp uses CMake to build:

    $ mkdir build && cd build
    $ cmake .. -DCMAKE_INSTALL_PREFIX=<prefix path> # add -DBUILD_SHARED_LIBS for a shared libary
    $ make
    # make install

#### Debian & derivatives
Sqlitepp is configured to generate a .deb package file. To do so, substitute the
above with the following:

    $ mkdir build && cd build
    $ cmake .. -DCMAKE_INSTALL_PREFIX=/usr -DBUILD_SHARED_LIBS
    $ make
    $ cpack
    # dpkg -i libsqlitepp-dev*.deb

When linking your own code using sqlitepp, you will need to link to both the
sqlitepp and sqlite3 libraries: `-l sqlitepp -lsqlite3` (depending on your OS)

#### Documentation
If doxygen is installed, library documentation can be generated with: `$ make doc`

@todo
* Add CMake find file and pkg-config file
* Add option to statically link to / compile in sqlite
