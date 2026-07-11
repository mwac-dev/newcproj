cmake_minimum_required(VERSION 3.24)

project(
    MYPROJECT_CHANGENAME
    VERSION 0.1.0
    LANGUAGES C CXX
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ------------------------------------------------------------
# Optional features
# ------------------------------------------------------------

option(MWAC_WITH_RAYLIB  "Enable raylib" OFF)
option(MWAC_WITH_RLIMGUI "Enable rlImGui integration" OFF)
option(MWAC_WITH_IMGUI   "Enable Dear ImGui" OFF)

# ------------------------------------------------------------
# Dependencies
# ------------------------------------------------------------

# Everything linked to this interface target is forwarded to the app.
add_library(project_dependencies INTERFACE)

include(cmake/dependencies.cmake)

# ------------------------------------------------------------
# App
# ------------------------------------------------------------

file(GLOB_RECURSE APP_SOURCES
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
)

add_executable(app
    ${APP_SOURCES}
)

target_compile_features(app PRIVATE cxx_std_23)

set_target_properties(
    app
    PROPERTIES
        CXX_EXTENSIONS OFF
)

target_include_directories(
    app
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(
    app
    PRIVATE
        project_dependencies
)

target_compile_definitions(
    app
    PRIVATE
        PROJECT_NAME="${PROJECT_NAME}"
)

# ------------------------------------------------------------
# Warnings
# ------------------------------------------------------------

if(MSVC)
    target_compile_options(
        app
        PRIVATE
            /W4
            /permissive-
    )
else()
    target_compile_options(
        app
        PRIVATE
            -Wall
            -Wextra
            -Wpedantic
    )
endif()

# ------------------------------------------------------------
# Assets
# ------------------------------------------------------------

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/assets")
    add_custom_command(
        TARGET app
        POST_BUILD
        COMMAND
            ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/assets"
            "$<TARGET_FILE_DIR:app>/assets"
        COMMENT "Copying assets"
    )
endif()
