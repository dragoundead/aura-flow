cmake_minimum_required(VERSION 3.20)
project(AuraFlow LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ── CPU Optimizations ─────────────────────────────────────
# Enable AVX2 for faster math operations (critical for whisper.cpp performance)
if(MSVC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /arch:AVX2 /fp:fast /openmp")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /arch:AVX2 /fp:fast /openmp")
endif()

# ── Qt 6.5 Configuration ──────────────────────────────────
# set(CMAKE_PREFIX_PATH "C:/Qt/6.5.0/msvc2019_64") # Removed to use CMakeSettings.json value
find_package(Qt6 6.5 COMPONENTS Widgets REQUIRED)
qt_standard_project_setup()

# ── whisper.cpp with CUDA or DirectML ─────────────────────
option(WHISPER_CUDA "Use CUDA for NVIDIA GPU acceleration" OFF)
option(WHISPER_DIRECTML "Use DirectML for AMD GPU acceleration" OFF)

if(WHISPER_CUDA)
    enable_language(CUDA)
    add_definitions(-DGGML_USE_CUDA)
    
    # Force NVCC to use our detected C++ compiler
    if(MSVC)
        set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -ccbin \"${CMAKE_CXX_COMPILER}\" -Xcompiler=/Zc:preprocessor")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /Zc:preprocessor")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:preprocessor")
    endif()

    set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -extended-lambda")
    
    find_package(CUDAToolkit REQUIRED)
    file(GLOB_RECURSE WHISPER_CUDA_SOURCES 
        "3rdparty/whisper.cpp/ggml/src/ggml-cuda/*.cu" 
        "3rdparty/whisper.cpp/ggml/src/ggml-cuda/*.cpp"
        "3rdparty/whisper.cpp/ggml/src/ggml-cuda.cu"
        "3rdparty/whisper.cpp/ggml/src/ggml-cuda.cpp"
    )
    # MSVC's INFINITY macro evaluates to 1e+300, which NVCC thinks overflows float bounds and throws an error/warning 221.
    set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -diag-suppress 221")
    set(GPU_LIBS CUDA::cudart CUDA::cublas CUDA::cublasLt CUDA::cuda_driver)
elseif(WHISPER_DIRECTML)
    add_definitions(-DWHISPER_DIRECTML)
    # Link against Windows DirectML/D3D12
    set(GPU_LIBS "D3D12.lib" "DirectML.lib" "dxgi.lib")
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
    ${WHISPER_CUDA_SOURCES}
)

target_link_libraries(${PROJECT_NAME} PRIVATE 
    Qt6::Widgets
    ${GPU_LIBS}
)

# Define WHISPER and GGML versions
target_compile_definitions(${PROJECT_NAME} PRIVATE 
    WHISPER_VERSION="1.8.3"
    GGML_VERSION="1.0.0"
    GGML_COMMIT="unknown"
)

if(NOT WHISPER_CUDA AND NOT WHISPER_DIRECTML)
    target_compile_definitions(${PROJECT_NAME} PRIVATE GGML_USE_CPU)
endif()

# Set Windows subsystem for no console in Release
set_target_properties(${PROJECT_NAME} PROPERTIES
    WIN32_EXECUTABLE TRUE
) 