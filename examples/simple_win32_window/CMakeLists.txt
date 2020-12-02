add_executable(simple_win32_window WIN32 win32_main.cpp)

target_compile_features(simple_win32_window PRIVATE cxx_std_20)
target_compile_options(simple_win32_window PRIVATE $<$<CXX_COMPILER_ID:MSVC>:-W4 -WX>)
