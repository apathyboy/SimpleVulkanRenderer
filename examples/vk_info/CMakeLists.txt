add_executable(vk_info main.cpp)

target_compile_features(vk_info PRIVATE cxx_std_20)
target_compile_options(vk_info PRIVATE $<$<CXX_COMPILER_ID:MSVC>:-W4 -WX>)

target_link_libraries(vk_info PRIVATE fmt::fmt Vulkan::Vulkan)
