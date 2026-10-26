cmake_minimum_required(VERSION 3.16)

project(MotorDeInferencias
    VERSION 0.1.0
    DESCRIPTION "Motor de inferencias basado en logica proposicional"
    LANGUAGES CXX
)

include(CTest)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(inference_engine
    src/Formula.cpp
    src/AtomExtractor.cpp
    src/NaturalLanguageParser.cpp
    src/Evaluator.cpp
    src/SemanticTableau.cpp
    src/ModelReporter.cpp
)

target_include_directories(inference_engine
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(inference_engine PUBLIC cxx_std_17)

if(MSVC)
    target_compile_options(inference_engine PRIVATE /W4 /permissive- /utf-8)
else()
    target_compile_options(inference_engine PRIVATE -Wall -Wextra -Wpedantic -finput-charset=UTF-8 -fexec-charset=UTF-8)
endif()

option(BUILD_GUI "Build the Dear ImGui desktop application" OFF)

if(BUILD_GUI)
    set(IMGUI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui"
        CACHE PATH "Path to Dear ImGui")
    find_package(glfw3 CONFIG REQUIRED)
    find_package(OpenGL REQUIRED)

    add_executable(motor_inferencias
        src/GuiApp.cpp
        src/gui_main.cpp
        ${IMGUI_DIR}/imgui.cpp
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_tables.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
        ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
        ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
    )
    target_include_directories(motor_inferencias PRIVATE
        ${IMGUI_DIR}
        ${IMGUI_DIR}/backends
    )
    target_link_libraries(motor_inferencias PRIVATE
        inference_engine
        glfw
        OpenGL::GL
        gdiplus
        winmm
    )
else()
    add_executable(motor_inferencias src/main.cpp)
    target_link_libraries(motor_inferencias PRIVATE inference_engine)
endif()

if(BUILD_TESTING)
    add_executable(inference_engine_tests
        tests/test_main.cpp
        tests/test_formulas.cpp
        tests/test_atoms.cpp
        tests/test_parser.cpp
        tests/test_evaluator.cpp
        tests/test_tableau.cpp
    )

    target_link_libraries(inference_engine_tests PRIVATE inference_engine)

    if(MSVC)
        target_compile_options(inference_engine_tests PRIVATE /W4 /permissive- /utf-8)
    else()
        target_compile_options(inference_engine_tests PRIVATE -Wall -Wextra -Wpedantic -finput-charset=UTF-8 -fexec-charset=UTF-8)
    endif()

    add_test(NAME inference_engine_tests COMMAND inference_engine_tests)
endif()
