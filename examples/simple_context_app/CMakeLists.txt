add_executable(simple_context_app WIN32 color_renderer.cpp color_renderer.hpp
                                        win32_main.cpp)

target_compile_features(simple_context_app PRIVATE cxx_std_20)
target_compile_options(simple_context_app PRIVATE $<$<CXX_COMPILER_ID:MSVC>:-W4 -WX>)

target_link_libraries(simple_context_app PRIVATE esb::svr)
