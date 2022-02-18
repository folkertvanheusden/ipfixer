# (C) 2020-2021 by folkert van heusden <mail@vanheusden.com>, released under Apache License v2.0
cmake_minimum_required(VERSION 3.9.4)

project(ipfixer VERSION 6.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_definitions("-D_FILE_OFFSET_BITS=64")

set(CMAKE_BUILD_TYPE Debug)

add_compile_options(-Wall -pedantic)

add_executable(ipfixer
	src/buffer.cpp
	src/db.cpp
	src/db-mongodb.cpp
	src/db-mysql.cpp
	src/db-postgres.cpp
	src/db-sql.cpp
	src/error.cpp
	src/ipfix.cpp
	src/ipfix-common.cpp
	src/logging.cpp
	src/main.cpp
	src/net.cpp
	src/netflow-v5.cpp
	src/netflow-v9.cpp
	src/str.cpp
	src/time.cpp
	src/yaml-helpers.cpp
	)

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads)
target_link_libraries(ipfixer Threads::Threads)

include(FindPkgConfig)

pkg_check_modules(LIBMONGOCXX libmongocxx)
target_link_libraries(ipfixer ${LIBMONGOCXX_LIBRARIES})
target_include_directories(ipfixer PUBLIC ${LIBMONGOCXX_INCLUDE_DIRS})
target_compile_options(ipfixer PUBLIC ${LIBMONGOCXX_CFLAGS_OTHER})

pkg_check_modules(YAML REQUIRED yaml-cpp)
target_link_libraries(ipfixer ${YAML_LIBRARIES})
target_include_directories(ipfixer PUBLIC ${YAML_INCLUDE_DIRS})
target_compile_options(ipfixer PUBLIC ${YAML_CFLAGS_OTHER})

pkg_check_modules(POSTGRES libpqxx)
target_link_libraries(ipfixer ${POSTGRES_LIBRARIES})
target_include_directories(ipfixer PUBLIC ${POSTGRES_INCLUDE_DIRS})
target_compile_options(ipfixer PUBLIC ${POSTGRES_CFLAGS_OTHER})

pkg_check_modules(JANSSON jansson)
target_link_libraries(ipfixer ${JANSSON_LIBRARIES})
target_include_directories(ipfixer PUBLIC ${JANSSON_INCLUDE_DIRS})
target_compile_options(ipfixer PUBLIC ${JANSSON_CFLAGS_OTHER})

pkg_check_modules(MARIADB libmariadb)
target_link_libraries(ipfixer ${MARIADB_LIBRARIES})
target_include_directories(ipfixer PUBLIC ${MARIADB_INCLUDE_DIRS})
target_compile_options(ipfixer PUBLIC ${MARIADB_CFLAGS_OTHER})

configure_file(src/config.h.in config.h)
target_include_directories(ipfixer PUBLIC "${PROJECT_BINARY_DIR}")
