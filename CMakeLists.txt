cmake_minimum_required(VERSION 3.1)
project(TankGame CXX)

set(BUILD_DIR_NAME ${CMAKE_BUILD_TYPE})
set(CMAKE_MODULE_PATH "${TankGame_SOURCE_DIR}/cmake")

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${TankGame_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${TankGame_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE})

add_definitions(-DGLM_FORCE_RADIANS -DZLIB_CONST -DGLM_ENABLE_EXPERIMENTAL -DGLM_FORCE_CTOR_INIT -Wall -Wpedantic -Wfatal-errors)

file(GLOB_RECURSE SOURCE_FILES ${TankGame_SOURCE_DIR}/src/*.cpp)

#Finds other libraries
find_package(OpenGL REQUIRED)
find_package(OpenAL REQUIRED)
find_package(GLM REQUIRED)
find_package(OGG REQUIRED)
find_package(Vorbis REQUIRED)
find_package(VorbisFile REQUIRED)
find_package(Freetype REQUIRED)
find_package(ZLIB REQUIRED)
find_package(Lua REQUIRED)
find_package(nlohmann_json REQUIRED)

find_path(GLFW_INCLUDE_DIR GLFW/glfw3.h ${HEADER_SEARCH_PATH})
find_library(GLFW_LIBRARIES NAMES glfw glfw3dll ${LIBRARY_SEARCH_PATH})

if (CMAKE_COMPILER_IS_GNUCXX)
	find_package(PkgConfig REQUIRED)
	pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
	include_directories(${GTK3_INCLUDE_DIRS})
	link_libraries(stdc++fs dl X11)
endif()

#Sets link libraries for all projects
link_libraries(
	${OPENGL_LIBRARY}
	${OPENAL_LIBRARY}
	${OGG_LIBRARY}
	${VORBIS_LIBRARY}
	${VORBISFILE_LIBRARY}
	${GLFW_LIBRARIES}
	${FREETYPE_LIBRARIES}
	${ZLIB_LIBRARIES}
	${LUA_LIBRARIES}
	${CMAKE_THREAD_LIBS_INIT}
	nlohmann_json::nlohmann_json
)

#Sets include directories for all projects
include_directories(SYSTEM
	${OPENGL_INCLUDE_DIR}
	${OPENAL_INCLUDE_DIR}
	${OGG_INCLUDE_DIR}
	${GLM_INCLUDE_DIRS}
	${VORBIS_INCLUDE_DIR}
	${VORBISFILE_INCLUDE_DIR}
	${GLFW_INCLUDE_DIR}
	${FREETYPE_INCLUDE_DIRS}
	${LUA_INCLUDE_DIR}
	${ZLIB_INCLUDE_DIRS}
	${TankGame_SOURCE_DIR}/inc
	${TankGame_SOURCE_DIR}/imgui
)

add_executable(tankgame ${SOURCE_FILES} imgui/imgui.cpp imgui/imgui_draw.cpp)

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

set_source_files_properties(imgui/imgui.cpp PROPERTIES COMPILE_FLAGS -w)
set_source_files_properties(imgui/imgui_draw.cpp PROPERTIES COMPILE_FLAGS -w)
