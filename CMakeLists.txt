cmake_minimum_required(VERSION 2.8.12.1)
cmake_policy(SET CMP0048 NEW)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CONFIGURATION_TYPES Debug Release)

project(VideoCap VERSION 1.0.0)

# set_property(GLOBAL PROPERTY OS_FOLDERS ON)

## opencv
find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

## include directory
include_directories( "${PROJECT_SOURCE_DIR}/include" )
include_directories( "${PROJECT_SOURCE_DIR}/DShowBase" )

## source list
file(GLOB_RECURSE SRC_LIST
     "${PROJECT_SOURCE_DIR}/src/*.cpp"
     "${PROJECT_SOURCE_DIR}/src/*.h"
     "${PROJECT_SOURCE_DIR}/src/*.hpp"
     "${PROJECT_SOURCE_DIR}/include/*.h"
     "${PROJECT_SOURCE_DIR}/include/*.hpp"
     "${PROJECT_SOURCE_DIR}/DShowBase/*.cpp"
     "${PROJECT_SOURCE_DIR}/DShowBase/*.h"
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
target_link_libraries(VideoCap shlwapi.lib Winmm.lib strmiids.lib d2d1.lib Comctl32.lib UxTheme.lib ${OpenCV_LIBS})
