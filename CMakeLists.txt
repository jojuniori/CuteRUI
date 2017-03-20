project (CuteRUI C CXX)
cmake_minimum_required (VERSION 3.6)
set (CMAKE_CXX_STANDARD 11)

find_package (Boost REQUIRED)

set (USE_MSVC_RUNTIME_LIBRARY_DLL OFF CACHE BOOL "force soci to use C++ 11" FORCE)
add_subdirectory (ext/glfw)

foreach (flag CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE CMAKE_C_FLAGS_MINSIZEREL CMAKE_C_FLAGS_RELWITHDEBINFO CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
  if (${flag} MATCHES "/MD")
    string (REGEX REPLACE "/MD" "/MT" ${flag} "${${flag}}")
  endif ()
  if (${flag} MATCHES "/MDd")
    string (REGEX REPLACE "/MDd" "/MTd" ${flag} "${${flag}}")
  endif ()
endforeach ()
add_library (glad STATIC ext/glad/src/glad)

include_directories ("${CMAKE_CURRENT_LIST_DIR}/ext/glad/include" "${CMAKE_CURRENT_LIST_DIR}/ext" "ext/nanovg" "${Boost_INCLUDE_DIRS}" "${CMAKE_CURRENT_LIST_DIR}/ext/glfw/include")

include (cmake/bin2c.cmake)
bin2c(resource/resource resource/text.txt resource/close.png resource/info.png resource/changed.png resource/unchanged.png resource/about.png)
add_subdirectory (src)