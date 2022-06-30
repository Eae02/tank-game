cmake_minimum_required(VERSION 3.16)
project(TankGame CXX)

set(BUILD_DIR_NAME ${CMAKE_BUILD_TYPE})
set(CMAKE_MODULE_PATH "${TankGame_SOURCE_DIR}/cmake")

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${TankGame_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${TankGame_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE})

#gets the commit hash
execute_process(COMMAND git log -1 --format=%h
	WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
	OUTPUT_VARIABLE COMMIT_HASH
	OUTPUT_STRIP_TRAILING_WHITESPACE
)
string(TIMESTAMP BUILD_DATE "%d-%m-%Y")

file(GLOB_RECURSE SOURCE_FILES ${TankGame_SOURCE_DIR}/src/*.cpp)

add_executable(tankgame ${SOURCE_FILES}
	deps/imgui/imgui.cpp deps/imgui/imgui_draw.cpp deps/imgui/imgui_tables.cpp deps/imgui/imgui_widgets.cpp
)

target_precompile_headers(tankgame PRIVATE ${TankGame_SOURCE_DIR}/inc/pch.h)

target_compile_options(tankgame PRIVATE
	-DGLM_FORCE_RADIANS -DZLIB_CONST -DGLM_ENABLE_EXPERIMENTAL -DGLM_FORCE_CTOR_INIT
	-DMENU_BUILD_INFO="${COMMIT_HASH} ${BUILD_DATE}"
	-Wall -Wpedantic -Wfatal-errors
)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Emscripten")
	set(USE_FLAGS -sUSE_ZLIB=1 -sUSE_OGG=1 -sUSE_VORBIS=1)
	
	target_link_options(tankgame PRIVATE -sEXPORTED_RUNTIME_METHODS=['cwrap'] -sEXPORTED_FUNCTIONS=['_WebMain'])
	target_link_options(tankgame PRIVATE -sALLOW_MEMORY_GROWTH=1 -sWASM=1 -sUSE_WEBGL2=1 -sFULL_ES3=1 -sFORCE_FILESYSTEM=1 -sFETCH=1 -sDISABLE_DEPRECATED_FIND_EVENT_TARGET_BEHAVIOR=0)
	target_link_options(tankgame PRIVATE -lidbfs.js -lopenal ${USE_FLAGS})
	target_link_options(tankgame PRIVATE --preload-file ${TankGame_SOURCE_DIR}/res@/res --use-preload-plugins)
	
	target_compile_options(tankgame PRIVATE ${USE_FLAGS} -Wno-sign-conversion -Wno-shorten-64-to-32 -Wno-mismatched-tags -Wno-inconsistent-missing-override)
	
	if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
		target_link_options(tankgame PRIVATE "-g4 --source-map-base='http://0.0.0.0:7000/'")
		target_compile_options(tankgame PRIVATE -gsource-map)
	endif()
	
	set(LUA_LIBRARIES "${LUA_SRC_PATH}/liblua.a")
	set(LUA_INCLUDE_DIR ${LUA_SRC_PATH})
else()
	find_package(OpenGL REQUIRED)
	find_package(OpenAL REQUIRED)
	find_package(OGG REQUIRED)
	find_package(VORBIS REQUIRED)
	find_package(VORBISFILE REQUIRED)
	find_package(ZLIB REQUIRED)
	find_package(Lua REQUIRED)
	find_package(glfw3 REQUIRED)
	
	target_compile_options(tankgame PRIVATE -DTG_AUDIO_DYNAMIC_LOAD)
	
	if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
		find_package(PkgConfig REQUIRED)
		pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
		target_include_directories(tankgame PRIVATE ${GTK3_INCLUDE_DIRS})
		target_link_libraries(tankgame PRIVATE stdc++fs X11)
	endif()
	
	target_link_libraries(tankgame PRIVATE
		glfw
		ZLIB::ZLIB
		${LUA_LIBRARIES}
		${CMAKE_DL_LIBS}
		${CMAKE_THREAD_LIBS_INIT}
	)
	
	target_include_directories(tankgame SYSTEM PRIVATE
		${OPENAL_INCLUDE_DIR}
		${OGG_INCLUDE_DIR}
		${VORBIS_INCLUDE_DIR}
		${VORBISFILE_INCLUDE_DIR}
		${LUA_INCLUDE_DIR}
	)
endif()

target_link_libraries(tankgame PRIVATE ${LUA_LIBRARIES})

target_include_directories(tankgame SYSTEM PRIVATE
	${LUA_INCLUDE_DIR}
	${TankGame_SOURCE_DIR}/inc
	${TankGame_SOURCE_DIR}/deps/glm
	${TankGame_SOURCE_DIR}/deps/imgui
	${TankGame_SOURCE_DIR}/deps/stb
	${TankGame_SOURCE_DIR}/deps/pcg/include
)

set_target_properties(tankgame PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}-${CMAKE_SYSTEM_NAME}
	LINKER_LANGUAGE CXX
	CXX_STANDARD 20
)

if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
	set_target_properties(tankgame PROPERTIES INSTALL_RPATH "$ORIGIN" BUILD_WITH_INSTALL_RPATH TRUE)
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
	target_link_options(tankgame PRIVATE "-Wl,-subsystem,windows")
endif()
