cmake_minimum_required(VERSION 3.1)
project(TankGame CXX)

set(CMAKE_MODULE_PATH "${TankGame_SOURCE_DIR}/cmake")

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${TankGame_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${TankGame_SOURCE_DIR}/bin)

add_definitions(-DGLM_FORCE_RADIANS -DZLIB_CONST)

file(GLOB_RECURSE SOURCE_FILES
		${TankGame_SOURCE_DIR}/src/*.cpp
		${TankGame_SOURCE_DIR}/src/*.h
		${TankGame_SOURCE_DIR}/src/*.inl
		${TankGame_SOURCE_DIR}/imgui/*.cpp
		${TankGame_SOURCE_DIR}/imgui/*.h
)

#Finds other libraries
find_package(OpenGL REQUIRED)
find_package(OpenAL REQUIRED)
find_package(OGG REQUIRED)
find_package(Vorbis REQUIRED)
find_package(VorbisFile REQUIRED)
find_package(GLFW REQUIRED)
find_package(GLEW REQUIRED)
find_package(Freetype REQUIRED)
find_package(ZLIB REQUIRED)

if (CMAKE_COMPILER_IS_GNUCXX)
	set(LIBS ${LIBS} stdc++fs X11)
endif()

#Sets link libraries for all projects
link_libraries(
	${OPENGL_LIBRARIES}
	${OPENAL_LIBRARY}
	${OGG_LIBRARY}
	${VORBIS_LIBRARY}
	${VORBISFILE_LIBRARY}
	${GLFW_LIBRARIES}
	${GLEW_LIBRARIES}
	${FREETYPE_LIBRARIES}
	${ZLIB_LIBRARIES}
	${LIBS}
)

#Sets include directories for all projects
include_directories(
	${OPENGL_INCLUDE_DIRS}
	${OPENAL_INCLUDE_DIR}
	${OGG_INCLUDE_DIR}
	${VORBIS_INCLUDE_DIR}
	${VORBISFILE_INCLUDE_DIR}
	${GLFW_INCLUDE_DIRS}
	${GLEW_INCLUDE_DIRS}
	${FREETYPE_INCLUDE_DIRS}
	${ZLIB_INCLUDE_DIRS}
	${TankGame_SOURCE_DIR}/inc
	${TankGame_SOURCE_DIR}/imgui
)

add_executable(tankgame ${SOURCE_FILES})

set_property(TARGET tankgame PROPERTY CXX_STANDARD 14)
