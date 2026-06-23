cmake_minimum_required(VERSION 3.15)

project(RayTracingRenderer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(MINIFB_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(MINIFB_BUILD_VERSION_INFO OFF CACHE BOOL "" FORCE)
set(MINIFB_USE_OPENGL_API OFF CACHE BOOL "" FORCE)

set(APP_SOURCE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/sources")
string(REPLACE "[" "__CMAKE_LBRACKET__" APP_SOURCE_GLOB_ROOT "${APP_SOURCE_ROOT}")
string(REPLACE "]" "__CMAKE_RBRACKET__" APP_SOURCE_GLOB_ROOT "${APP_SOURCE_GLOB_ROOT}")
string(REPLACE "__CMAKE_LBRACKET__" "[[]" APP_SOURCE_GLOB_ROOT "${APP_SOURCE_GLOB_ROOT}")
string(REPLACE "__CMAKE_RBRACKET__" "[]]" APP_SOURCE_GLOB_ROOT "${APP_SOURCE_GLOB_ROOT}")

file(GLOB_RECURSE APP_SOURCES CONFIGURE_DEPENDS
    "${APP_SOURCE_GLOB_ROOT}/*.cpp"
    "${APP_SOURCE_GLOB_ROOT}/*.h"
    "${APP_SOURCE_GLOB_ROOT}/*.hpp"
)

add_executable(RayTracingRenderer ${APP_SOURCES})

if(MSVC)
    target_compile_options(RayTracingRenderer PRIVATE /std:c++latest)
else()
    target_compile_options(RayTracingRenderer PRIVATE -std=c++23)
endif()

target_include_directories(RayTracingRenderer PRIVATE
    "${CMAKE_SOURCE_DIR}/includes/minifb/include"
    "${CMAKE_SOURCE_DIR}/includes/glm"
)

add_subdirectory(includes/minifb)

if(WIN32 AND NOT MSVC)
    set_target_properties(minifb PROPERTIES
        LINK_LIBRARIES ""
        INTERFACE_LINK_LIBRARIES ""
    )
endif()

target_link_libraries(RayTracingRenderer PRIVATE minifb)

if(WIN32)
    if(MSVC)
        target_link_libraries(RayTracingRenderer PRIVATE
            opengl32
            winmm
            kernel32
            user32
            gdi32
            winspool
            shell32
            ole32
            oleaut32
            uuid
            comdlg32
            advapi32
        )
    else()
        target_link_libraries(RayTracingRenderer PRIVATE
            "-lopengl32"
            "-lwinmm"
            "-lkernel32"
            "-luser32"
            "-lgdi32"
            "-lwinspool"
            "-lshell32"
            "-lole32"
            "-loleaut32"
            "-luuid"
            "-lcomdlg32"
            "-ladvapi32"
        )
    endif()
endif()

target_compile_definitions(RayTracingRenderer PRIVATE
    GLM_FORCE_RADIANS
    GLM_FORCE_DEPTH_ZERO_TO_ONE
)
