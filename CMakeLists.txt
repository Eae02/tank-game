cmake_minimum_required(VERSION 3.1)
project(TankGame CXX)

set(CMAKE_MODULE_PATH "${TankGame_SOURCE_DIR}/cmake")

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${TankGame_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${TankGame_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE})

add_definitions(-DGLM_FORCE_RADIANS -DZLIB_CONST)

file(GLOB_RECURSE SOURCE_FILES
		${TankGame_SOURCE_DIR}/src/game/*.cpp
		${TankGame_SOURCE_DIR}/src/game/*.h
		${TankGame_SOURCE_DIR}/src/game/*.inl
		${TankGame_SOURCE_DIR}/imgui/*.cpp
		${TankGame_SOURCE_DIR}/imgui/*.h
)

#Finds other libraries
find_package(OpenGL REQUIRED)
find_package(OpenAL REQUIRED)
find_package(GLM REQUIRED)
find_package(OGG REQUIRED)
find_package(Vorbis REQUIRED)
find_package(VorbisFile REQUIRED)
find_package(Freetype REQUIRED)
find_package(ZLIB REQUIRED)
find_package(Threads REQUIRED)
find_package(Lua REQUIRED)

find_path(GLFW_INCLUDE_DIR GLFW/glfw3.h ${HEADER_SEARCH_PATH})
find_library(GLFW_LIBRARIES NAMES glfw glfw3dll ${LIBRARY_SEARCH_PATH})

if (CMAKE_COMPILER_IS_GNUCXX)
	add_definitions(-Wall)
	
	set(CMAKE_INSTALL_RPATH "$ORIGIN")
	set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
	
	find_package(PkgConfig REQUIRED)
	pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
	
	include_directories(${GTK3_INCLUDE_DIRS})
	
	link_libraries(stdc++fs dl X11 glib-2.0)
endif()

if (${MSVC})
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4250 /D_CRT_SECURE_NO_WARNINGS")
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

add_library(tankgame SHARED ${SOURCE_FILES})
add_executable(tclient src/client.cpp)
add_executable(tserver src/server.cpp)

target_link_libraries(tclient tankgame)
target_link_libraries(tserver tankgame)

add_dependencies(tclient tankgame)
add_dependencies(tserver tankgame)

set_property(TARGET tankgame PROPERTY CXX_STANDARD 14)
set_property(TARGET tclient PROPERTY CXX_STANDARD 14)
set_property(TARGET tserver PROPERTY CXX_STANDARD 14)
