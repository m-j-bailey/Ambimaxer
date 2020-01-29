cmake_minimum_required(VERSION 2.6)
project(ambimaxer)

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

find_package(Boost 1.50 REQUIRED COMPONENTS filesystem)    
message(STATUS "Boost version: ${Boost_VERSION}")

INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )

add_executable(ambimaxer main.cpp)

TARGET_LINK_LIBRARIES( ambimaxer PUBLIC Boost::filesystem)

install(TARGETS ambimaxer RUNTIME DESTINATION bin)
