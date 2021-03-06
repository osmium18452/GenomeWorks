#
# Copyright (c) 2019, NVIDIA CORPORATION.  All rights reserved.
#
# NVIDIA CORPORATION and its licensors retain all intellectual property
# and proprietary rights in and to this software, related documentation
# and any modifications thereto.  Any use, reproduction, disclosure or
# distribution of this software and related documentation without an express
# license agreement from NVIDIA CORPORATION is strictly prohibited.
#

project(cudapoa)

# Process data subdirectory first
add_subdirectory(data)

# Optimization flags interfere with nvcc and cause compilation issues with reworked
# file structure.
if(NOT DEFINED cga_cuda_before_10_0)
    message(FATAL_ERROR "${PROJECT_NAME} : variable cga_cuda_before_10_0 is not defined yet. Please make sure CUDA.cmake is loaded first.")
elseif(cga_cuda_before_10_0)
    message(STATUS "${PROJECT_NAME} : Remove -O optimization when building for CUDA < 10 as it causes compilation issues.")
    string(REGEX REPLACE "-O[0-3]" "" CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE})
    string(REGEX REPLACE "-O[0-3]" "" CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})
endif()

# Project specific NVCC flags, --expt-relaxed-constexpr is being added to allow using numeric_limits inside device kernels.
# Using in CUDA 10.1+ to avoid compilation issues with CUDA <= 10.0.
set(CUDA_NVCC_FLAGS "${CUDA_NVCC_FLAGS} -std=c++14")
if(cga_cuda_after_10_0)
    set(CUDA_NVCC_FLAGS "${CUDA_NVCC_FLAGS} --expt-relaxed-constexpr")
endif()
message(STATUS "nvcc flags for ${PROJECT_NAME}: ${CUDA_NVCC_FLAGS}")

get_property(cga_library_type GLOBAL PROPERTY cga_library_type)
cuda_add_library(cudapoa ${cga_library_type}
    src/cudapoa.cpp
    src/batch.cu
    src/utils.cu
    )

target_link_libraries(cudapoa cgabase)

target_compile_options(cudapoa PRIVATE -Werror -Wall -Wextra)
if (cga_optimize_for_native_cpu)
    target_compile_options(cudapoa PRIVATE -march=native)
endif()

if(cga_enable_cudapoa_nw_print)
    target_compile_definitions(cudapoa PUBLIC NW_VERBOSE_PRINT)
endif()

if(spoa_accurate)
    target_compile_definitions(cudapoa PUBLIC SPOA_ACCURATE)
endif()

target_include_directories(cudapoa
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

add_doxygen_source_dir(${CMAKE_CURRENT_SOURCE_DIR}/include)

install(TARGETS cudapoa
    COMPONENT cgalogging
    EXPORT cudapoa
    DESTINATION lib
    INCLUDES DESTINATION include
)
install(DIRECTORY include/ DESTINATION include)
install(EXPORT cudapoa DESTINATION cmake)

# Add other folders.
add_subdirectory(tests)
add_subdirectory(benchmarks)
add_subdirectory(samples)


# Add auto formatting.
cga_enable_auto_formatting("${CMAKE_CURRENT_SOURCE_DIR}")
