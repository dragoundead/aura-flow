cmake_minimum_required(VERSION 3.20)
project(AuraFlow LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ── CPU Optimizations ─────────────────────────────────────
# Enable AVX2 for faster math operations (critical for whisper.cpp performance)
if(MSVC)
    add_compile_options(/arch:AVX2 /fp:fast /openmp)
endif()

# ── Qt 6.5 Configuration ──────────────────────────────────
# set(CMAKE_PREFIX_PATH "C:/Qt/6.5.0/msvc2019_64") # Removed to use CMakeSettings.json value
find_package(Qt6 6.5 COMPONENTS Widgets REQUIRED)
qt_standard_project_setup()

# ── whisper.cpp with DirectML ─────────────────────────────
# We assume whisper.cpp is in 3rdparty/whisper.cpp
option(WHISPER_DIRECTML "Use DirectML for AMD GPU acceleration" ON)

if(WHISPER_DIRECTML)
    add_definitions(-DWHISPER_DIRECTML)
    # Link against Windows DirectML/D3D12
    set(DIRECTML_LIBS "D3D12.lib" "DirectML.lib" "dxgi.lib")
endif()

# ── Include Directories ──────────────────────────────────
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/whisper.cpp/include
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/whisper.cpp/ggml/include
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/whisper.cpp/ggml/src
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/whisper.cpp/ggml/src/ggml-cpu
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/whisper.cpp/ggml/src/ggml-cpu/arch/x86
)

# ── Source Files ──────────────────────────────────────────
file(GLOB_RECURSE SOURCES 
    "src/*.cpp"
    "src/*.h"
)

# Core whisper.cpp and ggml sources
set(WHISPER_SOURCES
    "3rdparty/whisper.cpp/src/whisper.cpp"
    "3rdparty/whisper.cpp/ggml/src/ggml.c"
    "3rdparty/whisper.cpp/ggml/src/ggml-alloc.c"
    "3rdparty/whisper.cpp/ggml/src/ggml-backend.cpp"
    "3rdparty/whisper.cpp/ggml/src/ggml-backend-reg.cpp"
    "3rdparty/whisper.cpp/ggml/src/ggml-backend-dl.cpp"
    "3rdparty/whisper.cpp/ggml/src/ggml-quants.c"
    "3rdparty/whisper.cpp/ggml/src/gguf.cpp"
    "3rdparty/whisper.cpp/ggml/src/ggml-threading.cpp"
    "3rdparty/whisper.cpp/ggml/src/ggml-cpu/ggml-cpu.c"
    "3rdparty/whisper.cpp/ggml/src/ggml-cpu/ggml-cpu.cpp"
    "3rdparty/whisper.cpp/ggml/src/ggml-cpu/ops.cpp"
    "3rdparty/whisper.cpp/ggml/src/ggml-cpu/quants.c"
    "3rdparty/whisper.cpp/ggml/src/ggml-cpu/vec.cpp"
    "3rdparty/whisper.cpp/ggml/src/ggml-cpu/unary-ops.cpp"
    "3rdparty/whisper.cpp/ggml/src/ggml-cpu/binary-ops.cpp"
    "3rdparty/whisper.cpp/ggml/src/ggml-cpu/traits.cpp"
    "3rdparty/whisper.cpp/ggml/src/ggml-cpu/repack.cpp"
    "3rdparty/whisper.cpp/ggml/src/ggml-cpu/arch/x86/quants.c"
    "3rdparty/whisper.cpp/ggml/src/ggml-cpu/arch/x86/repack.cpp"
    "3rdparty/whisper.cpp/ggml/src/ggml-cpu/arch/x86/cpu-feats.cpp"
)

# ── Executable ────────────────────────────────────────────
qt_add_executable(${PROJECT_NAME} 
    ${SOURCES}
    ${WHISPER_SOURCES}
)

target_link_libraries(${PROJECT_NAME} PRIVATE 
    Qt6::Widgets
    ${DIRECTML_LIBS}
)

# Define WHISPER and GGML versions
target_compile_definitions(${PROJECT_NAME} PRIVATE 
    WHISPER_VERSION="1.8.3"
    GGML_VERSION="1.0.0"
    GGML_COMMIT="unknown"
    GGML_USE_CPU
)

# TEMPORARILY DISABLED for debugging (shows console window)
# set_target_properties(${PROJECT_NAME} PROPERTIES
#     WIN32_EXECUTABLE TRUE
# )