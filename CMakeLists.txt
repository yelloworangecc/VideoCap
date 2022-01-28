cmake_minimum_required(VERSION 2.8.12.1)
cmake_policy(SET CMP0048 NEW)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CONFIGURATION_TYPES Debug Release)

project(VideoCap VERSION 1.0.0)

# set_property(GLOBAL PROPERTY OS_FOLDERS ON)

## google test
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/e2239ee6043f73722e7aa812a459f54a28552929.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

## opencv
find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

## include directory
include_directories( "${PROJECT_SOURCE_DIR}/include" )

## source list
file(GLOB_RECURSE SRC_LIST
     "${PROJECT_SOURCE_DIR}/src/*.cpp"
     "${PROJECT_SOURCE_DIR}/src/*.h"
     "${PROJECT_SOURCE_DIR}/src/*.hpp"
     "${PROJECT_SOURCE_DIR}/include/*.h"
     "${PROJECT_SOURCE_DIR}/include/*.hpp"
     )
   
file(GLOB_RECURSE RC_LIST
     "${PROJECT_SOURCE_DIR}/resources/*.rc"
     )

## exe target
add_executable(VideoCap WIN32
               ${SRC_LIST}
               ${RC_LIST}
                )
                

## link libs
target_link_libraries(VideoCap strmiids.lib d2d1.lib dwrite.lib ${OpenCV_LIBS})

## google test
enable_testing()

file(GLOB_RECURSE TEST_SRC_LIST
     "${PROJECT_SOURCE_DIR}/test/*.cpp"
     )

add_executable(
  VideoCapTest
  ${PROJECT_SOURCE_DIR}/src/BaseSettings.cpp
  ${TEST_SRC_LIST}
)

target_link_libraries(
  VideoCapTest
  gtest_main
)

file(COPY ${PROJECT_SOURCE_DIR}/test/VideoCap.json DESTINATION ${PROJECT_BINARY_DIR})

include(GoogleTest)
gtest_discover_tests(VideoCapTest)