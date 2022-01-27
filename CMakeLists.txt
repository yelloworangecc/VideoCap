cmake_minimum_required(VERSION 2.8.12.1)
set(CMAKE_CONFIGURATION_TYPES Debug Release)
project(VideoCap)

set_property(GLOBAL PROPERTY OS_FOLDERS ON)

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

file(GLOB_RECURSE SRC_LIST
     "${PROJECT_SOURCE_DIR}/src/*.cpp"
     )
   
file(GLOB_RECURSE RC_LIST
     "${PROJECT_SOURCE_DIR}/resources/*.rc"
     )

add_executable(VideoCap WIN32
               ${SRC_LIST}
               ${RC_LIST}
                )

target_include_directories(VideoCap PUBLIC
               "${PROJECT_SOURCE_DIR}/include"
               )
	     
target_link_libraries(VideoCap strmiids.lib d2d1.lib dwrite.lib ${OpenCV_LIBS})
