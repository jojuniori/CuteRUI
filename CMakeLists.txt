project (CuteRUI C CXX)
cmake_minimum_required (VERSION 3.6)
set (CMAKE_CXX_STANDARD 11)

find_package (Boost REQUIRED)
add_subdirectory (ext/glfw)
add_library (glad STATIC ext/glad/src/glad)
include_directories ("${CMAKE_CURRENT_LIST_DIR}/ext/glad/include" "${CMAKE_CURRENT_LIST_DIR}/ext" "ext/nanovg" "${Boost_INCLUDE_DIRS}" "${CMAKE_CURRENT_LIST_DIR}/ext/glfw/include")

include (cmake/bin2c.cmake)
bin2c(resource/resource resource/text.txt resource/close.png resource/info.png resource/changed.png resource/unchanged.png resource/about.png)
add_subdirectory (src)