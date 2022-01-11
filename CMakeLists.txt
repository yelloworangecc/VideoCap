cmake_minimum_required(VERSION 2.8.12.1)
set(CMAKE_CONFIGURATION_TYPES Debug Release)
project(VideoCap)

set_property(GLOBAL PROPERTY OS_FOLDERS ON)

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

add_executable(VideoCap WIN32 main.cpp VideoDevice.cpp)
target_link_libraries(VideoCap strmiids.lib ${OpenCV_LIBS})
