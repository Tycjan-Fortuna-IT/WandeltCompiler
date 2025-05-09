cmake_minimum_required(VERSION 3.16...3.26 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_COMPILER "clang++")

set(CMAKE_C_FLAGS -fcolor-diagnostics)
set(CMAKE_CXX_FLAGS -fansi-escape-codes)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -g")

set(CMAKE_SUPPRESS_REGENERATION true) # Suppresses ZERO_CHECK project in Visual Studio

if(WIN32)
    add_compile_definitions(SW_PLATFORM_WINDOWS)
elseif(UNIX)
    add_compile_definitions(SW_PLATFORM_LINUX)
endif()

project(Wandelt VERSION 0.0.1 LANGUAGES CXX)

add_subdirectory(WandeltCore WandeltCore)

add_executable(${PROJECT_NAME} Runtime/src/main.cpp)

# Add the Wandelt Core library
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/WandeltCore/src)
target_link_libraries(${PROJECT_NAME} WandeltCore)

# Precompiled header
set(PCH_HEADER "WandeltCore/src/pch.hpp")
message("-- Using precompiled header: ${PCH_HEADER}")

target_precompile_headers(WandeltCore PRIVATE ${PCH_HEADER})
target_precompile_headers(${PROJECT_NAME} REUSE_FROM WandeltCore)
