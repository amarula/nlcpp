cmake_minimum_required(VERSION 3.10)
project(nlcpp)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	add_compile_options(-Wno-unqualified-std-cast-call)
endif()

add_library(amarula-netlink
	src/exception.cpp
	src/netlink.cpp
)

find_package(PkgConfig REQUIRED)
pkg_check_modules(NL libnl-3.0)
pkg_check_modules(NL_ROUTE libnl-route-3.0)

target_include_directories(amarula-netlink PUBLIC
	${NL_INCLUDE_DIRS}
	${NL_ROUTE_INCLUDE_DIRS}
)

target_include_directories(amarula-netlink PUBLIC
	include
)

target_link_libraries(amarula-netlink PUBLIC
	${NL_LIBRARIES}
	${NL_ROUTE_LIBRARIES}
)
