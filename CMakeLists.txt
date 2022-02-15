# (C) 2020-2021 by folkert van heusden <mail@vanheusden.com>, released under Apache License v2.0
cmake_minimum_required(VERSION 3.9.4)

project(ipfixer VERSION 6.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_definitions("-D_FILE_OFFSET_BITS=64")

set(CMAKE_BUILD_TYPE Debug)

add_compile_options(-Wall -pedantic)

add_executable(ipfixer
	buffer.cpp
	db.cpp
	db-mongodb.cpp
	error.cpp
	ipfix.cpp
	logging.cpp
	main.cpp
	net.cpp
	netflow-v9.cpp
	str.cpp
	time.cpp
	yaml-helpers.cpp
	)

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads)
target_link_libraries(ipfixer Threads::Threads)

configure_file(config.h.in config.h)

include(FindPkgConfig)

pkg_check_modules(LIBMONGOCXX libmongocxx)
target_link_libraries(ipfixer ${LIBMONGOCXX_LIBRARIES})
target_include_directories(ipfixer PUBLIC ${LIBMONGOCXX_INCLUDE_DIRS})
target_compile_options(ipfixer PUBLIC ${LIBMONGOCXX_CFLAGS_OTHER})

pkg_check_modules(YAML REQUIRED yaml-cpp)
target_link_libraries(ipfixer ${YAML_LIBRARIES})
target_include_directories(ipfixer PUBLIC ${YAML_INCLUDE_DIRS})
target_compile_options(ipfixer PUBLIC ${YAML_CFLAGS_OTHER})
