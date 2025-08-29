cmake_minimum_required(VERSION 3.10)
project(nlcpp)

include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	add_compile_options(-Wno-unqualified-std-cast-call)
endif()

add_library(nlcpp
	src/exception.cpp
	src/generic.cpp
	src/netlink.cpp
	src/nl80211.cpp
	src/route.cpp
)

find_package(PkgConfig REQUIRED)
pkg_check_modules(NL libnl-3.0)
pkg_check_modules(NL_ROUTE libnl-route-3.0)
pkg_check_modules(NL_GENL libnl-genl-3.0)

target_include_directories(nlcpp PUBLIC
	${NL_INCLUDE_DIRS}
	${NL_GENL_INCLUDE_DIRS}
	${NL_ROUTE_INCLUDE_DIRS}
)

target_include_directories(nlcpp PUBLIC
	include
)

target_link_libraries(nlcpp PUBLIC
	${NL_LIBRARIES}
	${NL_GENL_LIBRARIES}
	${NL_ROUTE_LIBRARIES}
)

install(TARGETS nlcpp
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
install(DIRECTORY include/nlcpp TYPE INCLUDE)


add_executable(nlcpp-test-tool
	src/test-tool.cpp
)

target_link_libraries(nlcpp-test-tool
	nlcpp
)
