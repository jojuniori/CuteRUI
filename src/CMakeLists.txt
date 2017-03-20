if (WIN32)
  add_custom_target(CuteR ALL python "${CMAKE_CURRENT_LIST_DIR}/setup.py" build SOURCES setup.py)
else ()
  add_custom_target(CuteR ALL cxfreeze "${CMAKE_CURRENT_LIST_DIR}/../ext/CuteR/CuteR/CuteR.py" --target-dir "${CMAKE_BINARY_DIR}/src/dist" SOURCES ../ext/CuteR/CuteR/CuteR.py)
endif ()


include_directories (../ext/glad/include ../lib/moemtk/include ../lib/moemtk/ext/glfw/include ../lib/moemtk/ext/nanovg "${Boost_INCLUDE_DIRS}")
add_executable (CuteRUI WIN32 rui Button Widget StaticText Image Window EventDispatcher ImageButton TextLoader TextBox ../resource/resource.c ../ext/nanovg/nanovg $<${WIN32}:cuterui.rc>)

target_link_libraries (CuteRUI glfw glad)
add_dependencies (CuteRUI CuteR)

# 2017/03/18  00:33            23,552 CuteR.exe
# 2017/01/04  06:00           149,504 libpng16.dll
# 2017/03/18  00:33         4,579,850 library.zip
# 2016/07/16  19:42            86,232 MPR.dll
# 2017/01/04  06:02           687,616 PIL._imaging.pyd
# 2017/01/04  06:02           402,944 PIL._imagingft.pyd
# 2016/12/20  04:36         2,471,936 python27.dll
# 2017/03/18  00:10           686,359 sample_input.png
# 2017/03/18  00:39             9,844 sample_output.png
# 2016/12/20  04:36            49,664 w9xpopen.exe
