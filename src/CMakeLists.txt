if (WIN32)
  add_custom_target(CuteR ALL python "${CMAKE_CURRENT_LIST_DIR}/setup.py" build SOURCES setup.py)
else ()
  add_custom_target(CuteR ALL cxfreeze "${CMAKE_CURRENT_LIST_DIR}/../ext/CuteR/CuteR/CuteR.py" --target-dir
    "${CMAKE_BINARY_DIR}/src/dist" SOURCES ../ext/CuteR/CuteR/CuteR.py)
endif ()

include_directories (../ext/glad/include ../lib/moemtk/include ../lib/moemtk/ext/glfw/include ../lib/moemtk/ext/nanovg
        ${Boost_INCLUDE_DIRS})
add_executable (CuteRUI WIN32 rui DominantColor Button Widget StaticText Image Window EventDispatcher ImageButton
        TextLoader TextBox ../resource/resource.c ../ext/nanovg/nanovg $<$<BOOL:${WIN32}>:cuterui.rc>)

target_link_libraries (CuteRUI glfw glad)
add_dependencies (CuteRUI CuteR)
