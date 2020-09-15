cmake_minimum_required(VERSION 3.17)

include(FetchContent)


set(GLFW_VERSION_TAG 3.3.2)
set(GLM_VERSION_TAG 0.9.9.8)
set(PHMAP_VERSION_TAG 1.32)
set(ENTT_VERSION_TAG v3.5.0)
set(FMT_VERSION_TAG 7.0.3)
set(SPDLOG_VERSION_TAG v1.8.0)
set(ZLIB_VERSION_TAG v1.2.11)


### glfw ###
FetchContent_Declare(
		glfw
		GIT_REPOSITORY https://github.com/glfw/glfw
		GIT_TAG ${GLFW_VERSION_TAG}
)
FetchContent_GetProperties(glfw)
if (NOT glfw_POPULATED)
	FetchContent_Populate(glfw)
	set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Build the GLFW example programs" FORCE)
	set(GLFW_BUILD_TESTS OFF CACHE BOOL "Build the GLFW test programs" FORCE)
	set(GLFW_BUILD_DOCS OFF CACHE BOOL "Build the GLFW documentation" FORCE)
	set(GLFW_INSTALL OFF CACHE BOOL "Generate installation target" FORCE)
	add_subdirectory(${glfw_SOURCE_DIR} ${glfw_BINARY_DIR})
endif()


### glm ###
FetchContent_Declare(
		glm
		GIT_REPOSITORY https://github.com/g-truc/glm.git
		GIT_TAG ${GLM_VERSION_TAG}
)
FetchContent_GetProperties(glm)
if (NOT glm_POPULATED)
	FetchContent_Populate(glm)
	set(GLM_TEST_ENABLE OFF CACHE BOOL "" FORCE)
	add_subdirectory(${glm_SOURCE_DIR} ${glm_BINARY_DIR})
endif()


### glad ###
FetchContent_Declare(
		glad
		GIT_REPOSITORY https://github.com/Dav1dde/glad.git
)
FetchContent_GetProperties(glad)
if (NOT glad_POPULATED)
	FetchContent_Populate(glad)
	set(GLAD_PROFILE "core" CACHE STRING "")
	set(GLAD_API "gl=3.3" CACHE STRING "")
	set(GLAD_GENERATOR "c" CACHE STRING "")
	set(GLAD_EXTENSIONS "GL_AMD_draw_buffers_blend,GL_ARB_conservative_depth,GL_ARB_draw_buffers_blend,GL_ARB_internalformat_query,GL_ARB_separate_shader_objects,GL_ARB_shading_language_include,GL_ARB_texture_filter_anisotropic,GL_ARB_texture_storage,GL_EXT_texture_filter_anisotropic" CACHE STRING "")
	set(GLAD_SPEC "gl" CACHE STRING "")
	add_subdirectory(${glad_SOURCE_DIR} ${glad_BINARY_DIR})
endif()


### plf libraries (plf_colony, plf_list, and plf_stack) ###
FetchContent_Declare(
		plf-colony
		GIT_REPOSITORY https://github.com/mattreecebentley/plf_colony.git
)
FetchContent_GetProperties(plf-colony)
FetchContent_Declare(
		plf-list
		GIT_REPOSITORY https://github.com/mattreecebentley/plf_list.git
)
FetchContent_GetProperties(plf-list)
FetchContent_Declare(
		plf-stack
		GIT_REPOSITORY https://github.com/mattreecebentley/plf_stack.git
)
FetchContent_GetProperties(plf-stack)

set(PLF_INCLUDE_DIR "${FETCHCONTENT_BASE_DIR}/plf-src")
file(MAKE_DIRECTORY ${PLF_INCLUDE_DIR})

if ((NOT plf-colony_POPULATED) OR (NOT plf-list_POPULATED) OR (NOT plf-stack_POPULATED))
	add_library(plf INTERFACE)
	target_include_directories(plf INTERFACE ${PLF_INCLUDE_DIR})
endif()
if (NOT plf-colony_POPULATED)
	FetchContent_Populate(plf-colony)
	file(COPY "${plf-colony_SOURCE_DIR}/plf_colony.h" DESTINATION "${PLF_INCLUDE_DIR}/plf")
	file(RENAME "${PLF_INCLUDE_DIR}/plf/plf_colony.h" "${PLF_INCLUDE_DIR}/plf/colony.h")
endif()
if (NOT plf-list_POPULATED)
	FetchContent_Populate(plf-list)
	file(COPY "${plf-list_SOURCE_DIR}/plf_list.h" DESTINATION "${PLF_INCLUDE_DIR}/plf")
	file(RENAME "${PLF_INCLUDE_DIR}/plf/plf_list.h" "${PLF_INCLUDE_DIR}/plf/list.h")
endif()
if (NOT plf-stack_POPULATED)
	FetchContent_Populate(plf-stack)
	file(COPY "${plf-stack_SOURCE_DIR}/plf_stack.h" DESTINATION "${PLF_INCLUDE_DIR}/plf")
	file(RENAME "${PLF_INCLUDE_DIR}/plf/plf_stack.h" "${PLF_INCLUDE_DIR}/plf/stack.h")
endif()


### parallel-hashmap ###
FetchContent_Declare(
		phmap
		GIT_REPOSITORY https://github.com/greg7mdp/parallel-hashmap.git
		GIT_TAG ${PHMAP_VERSION_TAG}
)
FetchContent_GetProperties(phmap)
if (NOT phmap_POPULATED)
	FetchContent_Populate(phmap)
	set(PHMAP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
	set(PHMAP_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
	add_library(phmap INTERFACE)
	target_include_directories(phmap INTERFACE ${phmap_SOURCE_DIR})
endif()


### stb ###
FetchContent_Declare(
		stb
		GIT_REPOSITORY https://github.com/nothings/stb.git
)
FetchContent_GetProperties(stb)
if (NOT stb_POPULATED)
	FetchContent_Populate(stb)
	add_library(stb INTERFACE)
	target_include_directories(stb INTERFACE ${stb_SOURCE_DIR})
endif()


### EnTT ###
#FetchContent_Declare(
#		entt
#		GIT_REPOSITORY https://github.com/skypjack/entt.git
#		GIT_TAG ${ENTT_VERSION_TAG}
#)
#FetchContent_GetProperties(entt)
#if (NOT entt_POPULATED)
#	FetchContent_Populate(entt)
#	add_library(EnTT INTERFACE)
#	add_library(EnTT::EnTT ALIAS EnTT)
#	target_include_directories(EnTT INTERFACE "${entt_SOURCE_DIR}/src")
#endif()


### fmtlib ###
FetchContent_Declare(
  fmtlib
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG        ${FMT_VERSION_TAG}
)
FetchContent_GetProperties(fmtlib)
if (NOT fmtlib_POPULATED)
	FetchContent_Populate(fmtlib)
	add_subdirectory(${fmtlib_SOURCE_DIR} ${fmtlib_BINARY_DIR})
	target_compile_definitions(fmt PUBLIC FMT_STATIC_THOUSANDS_SEPARATOR)
endif()


### spdlog ###
#FetchContent_Declare(
#  spdlog
#  GIT_REPOSITORY https://github.com/gabime/spdlog.git
#  GIT_TAG        ${SPDLOG_VERSION_TAG}
#)
#FetchContent_GetProperties(spdlog)
#if (NOT spdlog_POPULATED)
#	FetchContent_Populate(spdlog)
#	set(SPDLOG_FMT_EXTERNAL ON CACHE BOOL "" FORCE)
#	add_subdirectory(${spdlog_SOURCE_DIR} ${spdlog_BINARY_DIR})
#endif()


### zlib ###
FetchContent_Declare(
  zlib
  GIT_REPOSITORY https://github.com/madler/zlib.git
  GIT_TAG        ${ZLIB_VERSION_TAG}
)
FetchContent_GetProperties(zlib)
if (NOT zlib_POPULATED)
	FetchContent_Populate(zlib)
	add_subdirectory(${zlib_SOURCE_DIR} ${zlib_BINARY_DIR})
	target_include_directories(zlib PUBLIC "${zlib_SOURCE_DIR}" "${zlib_BINARY_DIR}")
	target_include_directories(zlibstatic PUBLIC "${zlib_SOURCE_DIR}" "${zlib_BINARY_DIR}")
endif()


### zstr ###
FetchContent_Declare(
  zstr
  GIT_REPOSITORY https://github.com/the-realest-stu/zstr.git
)
FetchContent_GetProperties(zstr)
if (NOT zstr_POPULATED)
	FetchContent_Populate(zstr)
	add_library(zstr INTERFACE)
	target_include_directories(zstr INTERFACE "${zstr_SOURCE_DIR}/src")
endif()

