project(test_package)
cmake_minimum_required(VERSION 2.8.11)
add_definitions(-DUSE_STANDALONE_ASIO)
set(CXX_STANDARD "c++17")
if(${CMAKE_CXX_COMPILER_ID} STREQUAL GNU)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z -fconcepts")
elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL Clang OR ${CMAKE_CXX_COMPILER_ID} STREQUAL AppleClang)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z")
elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
    set(MSVC_STD_FLAG "/std:c++17" CACHE STRING "Language Standard Version. /std:[c++17|c++latest]  ")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${MSVC_STD_FLAG} /W3 /EHsc")
else()
    message(WARNING "Unsupported compiler!")
endif()

set(CMAKE_VERBOSE_MAKEFILE TRUE)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

file(GLOB SOURCE_FILES *.cpp)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} ${CONAN_LIBS})
