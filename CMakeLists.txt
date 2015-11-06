cmake_minimum_required (VERSION 2.8.8)
project(sqlitepp)
set(TITLE "Sqlitepp")
set(AUTHOR "Matthew Chandler <tardarsauce@gmail.com>")
set(COPYRIGHT "2015 Matthew Chandler")
set(SUMMARY "A thin sqlite C++ wrapper")
set(WEBSITE "http://github.com/mattvchandler/sqlitepp")

#flags
set(CMAKE_CXX_FLAGS "-Wall -Wextra -std=c++14")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "-g -DDEBUG")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()

# libraries
set(SQLITEPP_GEN_DOC_TARGET ON CACHE BOOL "Generate 'doc' target")
set(SQLITEPP_GIT_VERSIONING ON CACHE BOOL "Get version info from git")
set(INCLUDE_SQLITE OFF CACHE BOOL "Download and include sqlite3 (removes dependency on external sqlite library)")

if(INCLUDE_SQLITE)
    find_package(Threads REQUIRED)

    set(SQLITE_VERSION_STR "3090200")
    set(SQLITE_ARCHIVE_NAME "sqlite-amalgamation-${SQLITE_VERSION_STR}")

    file(GENERATE OUTPUT "${PROJECT_BINARY_DIR}/get_sqlite.cmake" CONTENT
        "file(DOWNLOAD
        https://www.sqlite.org/2015/${SQLITE_ARCHIVE_NAME}.zip
            \"${PROJECT_BINARY_DIR}/${SQLITE_ARCHIVE_NAME}.zip\"
            SHOW_PROGRESS)

        execute_process(COMMAND \${CMAKE_COMMAND} -E tar xf
            \"${PROJECT_BINARY_DIR}/${SQLITE_ARCHIVE_NAME}.zip\"
            )

        file(COPY \"${PROJECT_BINARY_DIR}/${SQLITE_ARCHIVE_NAME}/sqlite3.h\"
            DESTINATION \"${PROJECT_BINARY_DIR}/include/sqlitepp/\")\n")

    add_custom_command(OUTPUT "${PROJECT_BINARY_DIR}/${SQLITE_ARCHIVE_NAME}/sqlite3.c"
        COMMAND ${CMAKE_COMMAND} -P "${PROJECT_BINARY_DIR}/get_sqlite.cmake"
        COMMENT "Getting sqlite source code")

    add_custom_target(get_sqlite
        DEPENDS "${PROJECT_BINARY_DIR}/${SQLITE_ARCHIVE_NAME}/sqlite3.c")

    include_directories(
        "${CMAKE_CURRENT_SOURCE_DIR}/include/"
        "${PROJECT_BINARY_DIR}/include"
        )
    set(SOURCES
        "${PROJECT_BINARY_DIR}/${SQLITE_ARCHIVE_NAME}/sqlite3.c"
        src/database.cpp
        src/sqlite.cpp
        src/error.cpp
        src/stmt.cpp
        )
else()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(SQLITE sqlite3 REQUIRED)

    file(GENERATE OUTPUT "${PROJECT_BINARY_DIR}/include/sqlitepp/sqlite3.h"
        CONTENT "#include <sqlite3.h>")

    include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/include/
        ${SQLITE_INCLUDE_DIRS}
        "${PROJECT_BINARY_DIR}/include"
        )
    link_directories(
        ${SQLITE_LIBRARY_DIRS}
        )
    set(SOURCES
        src/database.cpp
        src/sqlite.cpp
        src/error.cpp
        src/stmt.cpp
        )
endif()

if(SQLITEPP_GEN_DOC_TARGET)
    find_package(Doxygen)
    if(DOXYGEN_FOUND)
        add_custom_command(
            OUTPUT ${PROJECT_BINARY_DIR}/doc/html/index.html
            COMMAND ${DOXYGEN_EXECUTABLE} ${PROJECT_BINARY_DIR}/Doxyfile
            WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
            DEPENDS ${PROJECT_BINARY_DIR}/Doxyfile
            DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/README.md
            DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*
            COMMENT "Generating documentation with Doxygen")

        add_custom_target(doc DEPENDS ${PROJECT_BINARY_DIR}/doc/html/index.html)

        set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES doc)
    endif()
endif()

# git version info
if(SQLITEPP_GIT_VERSIONING)
    find_package(Git)
endif()
if(GIT_FOUND)
    file(GENERATE OUTPUT ${PROJECT_BINARY_DIR}/version.cmake CONTENT
        "# copy vars from main CMakeLists.txt
        set(CMAKE_CURRENT_SOURCE_DIR \"${CMAKE_CURRENT_SOURCE_DIR}\")
        set(PROJECT_BINARY_DIR \"${PROJECT_BINARY_DIR}\")
        set(PROJECT_NAME \"${PROJECT_NAME}\")
        set(TITLE \"${TITLE}\")
        set(AUTHOR \"${AUTHOR}\")
        set(COPYRIGHT \"${COPYRIGHT}\")
        set(SUMMARY \"${SUMMARY}\")
        set(WEBSITE \"${WEBSITE}\")

        execute_process(COMMAND ${GIT_EXECUTABLE} describe --tags --dirty
            OUTPUT_STRIP_TRAILING_WHITESPACE
            OUTPUT_VARIABLE VERSION_FULL)

        string(REGEX REPLACE \"^v([0-9]+)\\\\..*\" \"\\\\1\" VERSION_MAJOR \"\${VERSION_FULL}\")
        string(REGEX REPLACE \"^v[0-9]+\\\\.([0-9]+).*\" \"\\\\1\" VERSION_MINOR \"\${VERSION_FULL}\")
        string(REGEX REPLACE \"^v[0-9]+\\\\.[0-9]+-([0-9]+).*\" \"\\\\1\" VERSION_PATCH \"\${VERSION_FULL}\")
        string(REGEX REPLACE \"^v[0-9]+\\\\.[0-9]+-[0-9]+-(.*)\" \"\\\\1\" VERSION_SHA1 \"\${VERSION_FULL}\")

        if(NOT VERSION_PATCH OR VERSION_PATCH STREQUAL VERSION_FULL)
            set(VERSION_PATCH 0)
        endif()
        if(NOT VERSION_SHA1 OR VERSION_SHA1 STREQUAL VERSION_FULL)
            set(VERSION_SHA1 \"\")
        endif()

        set(VERSION_SHORT \"\${VERSION_MAJOR}.\${VERSION_MINOR}.\${VERSION_PATCH}\")

        message(STATUS \"Hymns version \${VERSION_FULL}\")

        configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
            ${PROJECT_BINARY_DIR}/Doxyfile)\n")

    add_custom_target(version
        COMMAND ${CMAKE_COMMAND} -P ${CMAKE_BINARY_DIR}/version.cmake
        DEPENDS
            ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
        BYPRODUCTS
            ${PROJECT_BINARY_DIR}/Doxyfile
        COMMENT "Getting version from git"
        )

    execute_process(COMMAND ${GIT_EXECUTABLE} describe --tags --dirty
        OUTPUT_STRIP_TRAILING_WHITESPACE
        OUTPUT_VARIABLE VERSION_FULL)

    string(REGEX REPLACE "^v([0-9]+)\\..*" "\\1" VERSION_MAJOR "${VERSION_FULL}")
    string(REGEX REPLACE "^v[0-9]+\\.([0-9]+).*" "\\1" VERSION_MINOR "${VERSION_FULL}")
    string(REGEX REPLACE "^v[0-9]+\\.[0-9]+-([0-9]+).*" "\\1" VERSION_PATCH "${VERSION_FULL}")
    string(REGEX REPLACE "^v[0-9]+\\.[0-9]+-[0-9]+-(.*)" "\\1" VERSION_SHA1 "${VERSION_FULL}")

    if(NOT VERSION_PATCH OR VERSION_PATCH STREQUAL VERSION_FULL)
        set(VERSION_PATCH 0)
    endif()
    if(NOT VERSION_SHA1 OR VERSION_SHA1 STREQUAL VERSION_FULL)
        set(VERSION_SHA1 "")
    endif()

    set(VERSION_SHORT "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

    if(DOXYGEN_FOUND)
        add_dependencies(doc version)
    endif()
else()
    # manually update each minor release
    set(VERSION_MAJOR 1)
    set(VERSION_MINOR 1)
    set(VERSION_PATCH 0)
    set(VERSION_SHA1 "")
    set(VERSION_SHORT "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
    set(VERSION_FULL "${VERSION_SHORT}")

    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
        ${PROJECT_BINARY_DIR}/Doxyfile)
endif()

# main compilation
add_library(${PROJECT_NAME}
    ${SOURCES}
    )

if(INCLUDE_SQLITE)
    add_dependencies(${PROJECT_NAME}
        get_sqlite)

    target_link_libraries(${PROJECT_NAME}
        ${CMAKE_THREAD_LIBS_INIT}
        ${CMAKE_DL_LIBS}
        )
else()
    target_link_libraries(${PROJECT_NAME}
        ${SQLITE_LIBRARIES}
        )
endif()

# install targets
install(TARGETS ${PROJECT_NAME}
    ARCHIVE DESTINATION "lib"
    LIBRARY DESTINATION "lib"
    RUNTIME DESTINATION "bin"
    )

install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}" DESTINATION "include")
install(DIRECTORY "${PROJECT_BINARY_DIR}/include/${PROJECT_NAME}" DESTINATION "include")

# CPack vars
set(CPACK_GENERATOR TGZ)
set(CPACK_PACKAGE_CONTACT "${AUTHOR}")
set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY)
set(CPACK_PACKAGE_NAME "libsqlitepp-dev")
set(CPACK_PACKAGE_VERSION_MAJOR "${VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${VERSION_PATCH}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${TITLE}: ${SUMMARY}")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
set(CPACK_PACKAGE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
set(CPACK_PACKAGE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")

set(CPACK_SOURCE_IGNORE_FILES
    ${PROJECT_BINARY_DIR}/*
    ${CMAKE_CURRENT_SOURCE_DIR}/.git/*
    ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}
    )

# Debian-specific settings
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    find_program(DPKG "dpkg")
    if(DPKG)
        set(CPACK_GENERATOR DEB;TGZ)
        execute_process(COMMAND ${DPKG} --print-architecture
            OUTPUT_STRIP_TRAILING_WHITESPACE
            OUTPUT_VARIABLE CPACK_DEBIAN_PACKAGE_ARCHITECTURE)
        if(NOT INCLUDE_SQLITE)
            set(CPACK_DEBIAN_PACKAGE_DEPENDS
                "libsqlite3-0")
        else()
            set(CPACK_DEBIAN_PACKAGE_DEPENDS "")
        endif()
        set(CPACK_DEBIAN_PACKAGE_SECTION "devel")
        string(TOLOWER "${CPACK_PACKAGE_NAME}" CPACK_DEBIAN_PACKAGE_NAME)
        set(CPACK_PACKAGE_FILE_NAME
            "${CPACK_DEBIAN_PACKAGE_NAME}_${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}_${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}")
    endif()
endif()

include(CPack)
