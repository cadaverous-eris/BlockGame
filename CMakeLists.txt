cmake_minimum_required(VERSION 3.17)


set(MAIN_PROJECT_NAME game)
set(MAIN_PROJECT_TITLE "Smol Game")


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
if (MSVC)
	set(CMAKE_GENERATOR_PLATFORM x64)
endif()


project(${MAIN_PROJECT_NAME} LANGUAGES CXX C)
include("cmake/standard_project_settings.cmake")


# enable cache system
include("cmake/cache.cmake")


# standard compiler warnings
add_library(project_warnings INTERFACE)
include("cmake/warnings.cmake")
set_project_warnings(project_warnings)


# add dependencies
include("cmake/dependencies.cmake")


# add source files
file(GLOB_RECURSE SOURCES src/*.cpp)


add_executable(${MAIN_PROJECT_NAME} ${SOURCES})

target_include_directories(${MAIN_PROJECT_NAME} PRIVATE src)

target_compile_definitions(${MAIN_PROJECT_NAME} PRIVATE NOMINMAX GLFW_INCLUDE_NONE PROJECT_NAME="${MAIN_PROJECT_TITLE}")

if(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
	target_compile_options(${MAIN_PROJECT_NAME} PRIVATE "--stdlib=libstdc++")
endif()

target_link_libraries(
	${MAIN_PROJECT_NAME}
	PRIVATE
		glfw
		glm::glm
		glad
		plf
		phmap
		stb
		#EnTT::EnTT
		fmt::fmt
		#spdlog::spdlog
		zlibstatic
		zstr
)
# enable warnings
target_link_libraries(${MAIN_PROJECT_NAME} PRIVATE project_warnings)

#target_compile_definitions(glm INTERFACE GLM_FORCE_SILENT_WARNINGS=1)

