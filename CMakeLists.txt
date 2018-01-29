project(OnsetDetection)
cmake_minimum_required(VERSION 2.8)

include_directories(${CMAKE_CURRENT_LIST_DIR}/include)
include_directories(D:/Program/jsoncpp-src-0.5.0/jsoncpp-src-0.5.0/include)
include_directories(D:/Program/Anaconda2/include)
link_directories(D:/Program/jsoncpp-src-0.5.0/jsoncpp-src-0.5.0/lib)
link_directories(D:/Program/Anaconda2/libs)

file(GLOB INC include/*.h include/*.hpp)
file(GLOB SRC src/*.c src/*.cc src/*.cpp)

source_group(include FILES ${INC})
source_group(src FILES ${SRC})

add_executable(OnsetDetection ${INC} ${SRC})

