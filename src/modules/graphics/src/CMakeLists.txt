cmake_minimum_required(VERSION 3.5.0)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)

# source files
set(SOURCE_ALL VkInstance.cpp VkPhysicalDevice.cpp
        GraphicEngine.cpp
        facade.cpp)
set(HEADER_ALL VkInstance.hpp VkPhysicalDevice.hpp
        GraphicEngine.cpp)

find_package(VulkanHeaders CONFIG)
find_package(glfw3 CONFIG REQUIRED)

add_library(graphics ${SOURCE_ALL} ${HEADER_ALL})

# link libraries
# vulkan
if (win32)
    target_link_directories(graphics PRIVATE $ENV{VULKAN_SDK}/Lib)
    target_link_libraries(graphics PRIVATE Vulkan-1)
endif (win32)

if (APPLE)
    target_link_libraries(graphics PRIVATE $ENV{VULKAN_SDK}/lib/libvulkan.dylib)
endif (APPLE)

if (UNIX AND NOT APPLE)
    find_package(Vulkan REQUIRED)
    target_link_libraries(graphics PRIVATE Vulkan::Vulkan)
endif (UNIX AND NOT APPLE)

# glfw
target_link_libraries(graphics PUBLIC glfw)