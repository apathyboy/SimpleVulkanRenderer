find_package(Catch2 CONFIG REQUIRED)

include(CTest)
include(Catch)

add_executable(svr-tests main.cpp)

target_compile_options(svr PRIVATE $<$<CXX_COMPILER_ID:MSVC>:-W4 -WX -wd6330>)

target_link_libraries(svr-tests PRIVATE Catch2::Catch2 esb::svr)

catch_discover_tests(svr-tests)
