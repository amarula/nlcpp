cmake_minimum_required(VERSION 3.10)
project(nlcpp VERSION 0.1.0)

include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(BUILD_SHARED_LIBS "Build using shared libraries" ON)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	add_compile_options(-Wno-unqualified-std-cast-call)
endif()

add_library(nlcpp
	src/exception.cpp
	src/generic.cpp
	src/message.cpp
	src/netlink.cpp
	src/nl80211.cpp
	src/route.cpp
)

set_target_properties(nlcpp PROPERTIES
	VERSION ${PROJECT_VERSION}
	SOVERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

find_package(PkgConfig REQUIRED)
pkg_check_modules(NL IMPORTED_TARGET libnl-3.0)
pkg_check_modules(NL_ROUTE IMPORTED_TARGET libnl-route-3.0)
pkg_check_modules(NL_GENL IMPORTED_TARGET libnl-genl-3.0)

target_include_directories(nlcpp PUBLIC
	include
)

target_link_libraries(nlcpp PRIVATE
	PkgConfig::NL
	PkgConfig::NL_ROUTE
	PkgConfig::NL_GENL
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
