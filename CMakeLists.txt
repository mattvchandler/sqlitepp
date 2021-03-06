cmake_minimum_required (VERSION 3.1 FATAL_ERROR)
project(sqlitepp)
set(TITLE "Sqlitepp")
set(AUTHOR "Matthew Chandler <tardarsauce@gmail.com>")
set(COPYRIGHT "2019 Matthew Chandler")
set(SUMMARY "A thin sqlite C++ wrapper")
set(WEBSITE "http://github.com/mattvchandler/sqlitepp")

set(VERSION_MAJOR 1)
set(VERSION_MINOR 2)
set(VERSION_PATCH 3)

set(VERSION_SHORT "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(-Wall -Wextra)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()

# libraries
option(SQLITEPP_GEN_DOC_TARGET "Generate 'doc' target" ON)

option(INCLUDE_SQLITE "Download and include sqlite3 (removes dependency on external sqlite library)" OFF)

option(DISABLE_SHARED_LIBS "Disable building of static library" OFF)
option(DISABLE_STATIC_LIBS "Disable building of shared library" OFF)

if(INCLUDE_SQLITE)
    find_package(Threads REQUIRED)

    set(SQLITE_VERSION_STR "3280000")
    set(SQLITE_ARCHIVE_NAME "sqlite-amalgamation-${SQLITE_VERSION_STR}")

    file(GENERATE OUTPUT "${PROJECT_BINARY_DIR}/get_sqlite.cmake" CONTENT
        "file(DOWNLOAD
        https://www.sqlite.org/2019/${SQLITE_ARCHIVE_NAME}.zip
            \"${PROJECT_BINARY_DIR}/${SQLITE_ARCHIVE_NAME}.zip\"
            SHOW_PROGRESS)

        execute_process(COMMAND \${CMAKE_COMMAND} -E tar xf
            \"${PROJECT_BINARY_DIR}/${SQLITE_ARCHIVE_NAME}.zip\"
            )

        file(COPY \"${PROJECT_BINARY_DIR}/${SQLITE_ARCHIVE_NAME}/sqlite3.h\"
            DESTINATION \"${PROJECT_BINARY_DIR}/include/${PROJECT_NAME}/\")\n")

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

    file(GENERATE OUTPUT "${PROJECT_BINARY_DIR}/include/${PROJECT_NAME}/sqlite3.h"
        CONTENT "#include <sqlite3.h>\n")

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

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
    ${PROJECT_BINARY_DIR}/Doxyfile
    @ONLY)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}-config.cmake.in
    ${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config.cmake
    @ONLY)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}-config-version.cmake.in
    ${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake
    @ONLY)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}.pc.in
    ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.pc
    @ONLY)

# main compilation
add_library(${PROJECT_NAME}_obj OBJECT ${SOURCES})
set_target_properties(${PROJECT_NAME}_obj PROPERTIES POSITION_INDEPENDENT_CODE 1)

set(SHARED_LIBS ${PROJECT_NAME})
set(STATIC_LIBS ${PROJECT_NAME}-static)
set(TARGETS ${SHARED_LIBS} ${STATIC_LIBS})

if(DISABLE_STATIC_LIBS AND DISABLE_SHARED_LIBS)
    message(FATAL_ERROR "Both static and shared libs disabled - nothing to do!")
endif()
if(DISABLE_SHARED_LIBS)
    set(STATIC_LIBS ${SHARED_LIBS})
    unset(SHARED_LIBS)
    set(TARGETS ${STATIC_LIBS})
endif()
if(DISABLE_STATIC_LIBS)
    unset(STATIC_LIBS)
    set(TARGETS ${SHARED_LIBS})
endif()

if(NOT DISABLE_SHARED_LIBS)
    add_library(${SHARED_LIBS} SHARED $<TARGET_OBJECTS:${PROJECT_NAME}_obj>)
endif()

if(NOT DISABLE_STATIC_LIBS)
    add_library(${STATIC_LIBS} STATIC $<TARGET_OBJECTS:${PROJECT_NAME}_obj>)
endif()

if(INCLUDE_SQLITE)
    add_dependencies(${PROJECT_NAME}_obj
        get_sqlite)

    foreach(TARGET ${TARGETS})
        target_link_libraries(${TARGET}
            ${CMAKE_THREAD_LIBS_INIT}
            ${CMAKE_DL_LIBS}
            )
    endforeach()
else()
    foreach(TARGET ${TARGETS})
        target_link_libraries(${TARGET}
        ${SQLITE_LIBRARIES}
        )
    endforeach()
endif()

# install targets
install(TARGETS ${TARGETS}
    EXPORT "${PROJECT_NAME}-targets"
    ARCHIVE DESTINATION "lib"
    LIBRARY DESTINATION "lib"
    RUNTIME DESTINATION "bin"
    )

install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}" DESTINATION "include")
install(DIRECTORY "${PROJECT_BINARY_DIR}/include/${PROJECT_NAME}" DESTINATION "include")

install(FILES "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config.cmake" DESTINATION "lib/cmake/${PROJECT_NAME}")
install(EXPORT "${PROJECT_NAME}-targets" DESTINATION "lib/cmake/${PROJECT_NAME}")
install(FILES "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake" DESTINATION "lib/cmake/${PROJECT_NAME}")

install(FILES "${PROJECT_BINARY_DIR}/${PROJECT_NAME}.pc" DESTINATION "share/pkgconfig")

# CPack vars
set(CPACK_GENERATOR TGZ)
set(CPACK_PACKAGE_CONTACT "${AUTHOR}")
set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY)
set(CPACK_PACKAGE_NAME "lib${PROJECT_NAME}-dev")
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
