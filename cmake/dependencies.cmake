include(FetchContent)

# ------------------------------------------------------------
# reflection
# ------------------------------------------------------------

if(MWAC_ENABLE_REFLECTION)
    if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        message(FATAL_ERROR
            "MWAC_ENABLE_REFLECTION currently requires GCC 16 or newer"
        )
    endif()

    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 16)
        message(FATAL_ERROR
            "C++26 reflection requires GCC 16 or newer"
        )
    endif()

endif()

# rlImGui requires both raylib and Dear ImGui.
if(MWAC_WITH_RLIMGUI)
    set(MWAC_WITH_RAYLIB ON)
    set(MWAC_WITH_IMGUI ON)
endif()

# ------------------------------------------------------------
# raylib
# ------------------------------------------------------------

if(MWAC_WITH_RAYLIB)
    message(STATUS "Enabling raylib")

    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(BUILD_GAMES OFF CACHE BOOL "" FORCE)

    FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG 6.0
        GIT_SHALLOW TRUE
    )

    FetchContent_MakeAvailable(raylib)

    target_link_libraries(
        project_dependencies
        INTERFACE
            raylib
    )
endif()

# ------------------------------------------------------------
# Dear ImGui
# ------------------------------------------------------------

if(MWAC_WITH_IMGUI)
    message(STATUS "Enabling Dear ImGui")

    FetchContent_Declare(
        imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        GIT_TAG v1.92.8-docking
        GIT_SHALLOW TRUE
    )

    FetchContent_MakeAvailable(imgui)

    add_library(imgui STATIC
        "${imgui_SOURCE_DIR}/imgui.cpp"
        "${imgui_SOURCE_DIR}/imgui_draw.cpp"
        "${imgui_SOURCE_DIR}/imgui_tables.cpp"
        "${imgui_SOURCE_DIR}/imgui_widgets.cpp"
        "${imgui_SOURCE_DIR}/imgui_demo.cpp"
    )

    target_include_directories(
        imgui
        PUBLIC
            "${imgui_SOURCE_DIR}"
    )

    target_compile_features(
        imgui
        PUBLIC
            cxx_std_20
    )

    target_link_libraries(
        project_dependencies
        INTERFACE
            imgui
    )
endif()

# ------------------------------------------------------------
# rlImGui
# ------------------------------------------------------------

if(MWAC_WITH_RLIMGUI)
    message(STATUS "Enabling rlImGui")

    FetchContent_Declare(
        rlimgui
        GIT_REPOSITORY https://github.com/raylib-extras/rlImGui.git
        GIT_TAG Raylib_6_0
        GIT_SHALLOW TRUE
    )

    FetchContent_MakeAvailable(rlimgui)

    add_library(rlimgui STATIC
        "${rlimgui_SOURCE_DIR}/rlImGui.cpp"
    )

    target_include_directories(
        rlimgui
        PUBLIC
            "${rlimgui_SOURCE_DIR}"
    )

    target_link_libraries(
        rlimgui
        PUBLIC
            raylib
            imgui
    )

    target_compile_features(
        rlimgui
        PUBLIC
            cxx_std_20
    )

    target_link_libraries(
        project_dependencies
        INTERFACE
            rlimgui
    )
endif()
