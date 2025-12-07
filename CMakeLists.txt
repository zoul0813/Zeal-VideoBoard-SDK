cmake_minimum_required(VERSION 3.16)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)

set(ZOS_TOOLCHAIN sdcc)
include($ENV{ZOS_PATH}/cmake/zos_init.cmake)

project(zvb-sdk-libs C)

# Function to create object libraries (compile once, use many times)
function(zvb_add_library name)
    add_library(${name}_objects OBJECT ${ARGN})
    target_include_directories(${name}_objects PRIVATE ${CMAKE_SOURCE_DIR}/include)
    target_compile_options(${name}_objects PRIVATE --nostdlib)
    # Create the static library from the object files
    add_library(${name} STATIC $<TARGET_OBJECTS:${name}_objects>)
endfunction()

# Function to convert relative paths to absolute and export to parent scope
macro(zvb_export_sources var_name source_list)
    set(abs_sources)
    foreach(src ${source_list})
        list(APPEND abs_sources ${CMAKE_CURRENT_SOURCE_DIR}/${src})
    endforeach()
    set(${var_name} ${abs_sources} PARENT_SCOPE)
endmacro()

# Input directory for the source files
set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)

# Add subdirectories for each module
add_subdirectory(${SRC_DIR}/crc)
add_subdirectory(${SRC_DIR}/dma)
add_subdirectory(${SRC_DIR}/sound)
add_subdirectory(${SRC_DIR}/gfx)

# Create combined library with all modules (reuse object files from other libraries)
add_library(zvb STATIC
    $<TARGET_OBJECTS:zvb_gfx_objects>
    $<TARGET_OBJECTS:zvb_crc_objects>
    $<TARGET_OBJECTS:zvb_sound_objects>
    $<TARGET_OBJECTS:zvb_dma_objects>
)

# Group target to build all
add_custom_target(all_libs DEPENDS zvb_gfx zvb_crc zvb_sound zvb_dma zvb)
