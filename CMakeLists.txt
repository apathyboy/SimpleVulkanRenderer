cmake_minimum_required(VERSION 3.16)

project(SimpleVulkanRenderer VERSION 0.0.1)

find_package(fmt CONFIG REQUIRED)
find_package(Vulkan REQUIRED)

add_library(svr include/svr/svr.hpp src/svr.cpp)

add_library(esb::svr ALIAS svr)

target_include_directories(
    svr
    PUBLIC $<INSTALL_INTERFACE:include>
           $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/svr ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_compile_features(svr PUBLIC cxx_std_20)
target_compile_options(svr PRIVATE $<$<CXX_COMPILER_ID:MSVC>:-W4 -WX>)

add_subdirectory(examples)
add_subdirectory(tests)
