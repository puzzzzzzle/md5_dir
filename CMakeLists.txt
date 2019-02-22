cmake_minimum_required(VERSION 3.10)
project(md5_dir)
set(CMAKE_CXX_STANDARD 14)

set(md5_dir md5_dir)
message("cmake env flags ${CMAKE_BUILD_TYPE}")

set(CMAKE_CXX_FLAGS "-fPIC ${CMAKE_CXX_FLAGS}")

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    message("build type debug!")
    set(CMAKE_CXX_FLAGS "-Wall -O0  -g -ggdb ${CMAKE_CXX_FLAGS}")
elseif ("${CMAKE_BUILD_TYPE}}" STREQUAL "Release")
    message("build type release!")
    set(CAKE_CXX_FLAGS "-O3 -Wall ${CMAKE_CXX_FLAGS}")
else ()
    message("unknow type:${CMAKE_BUILD_TYPE}")
endif ()

aux_source_directory(${PROJECT_SOURCE_DIR} CPPS)
include_directories(${PROJECT_SOURCE_DIR})
add_executable(${md5_dir} main.cpp ${CPPS}

        tools.cpp cJSON.c

        )

add_definitions(-DBOOST_LOG_DYN_LINK)

#添加库文件连接
target_link_libraries(${md5_dir}
        pthread boost_log
        )
