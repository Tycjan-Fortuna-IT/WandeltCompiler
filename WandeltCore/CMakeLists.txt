project(WandeltCore)

include(../GroupSourcesByFolder.cmake)

include_directories(${PROJECT_SOURCE_DIR}/src)

file(GLOB_RECURSE HEADERS "src/*.hpp")
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Automatically create source groups in Visual Studio based on the file paths
foreach(SRC_FILE IN LISTS SRC_FILES)
    get_filename_component(SRC_PATH "${SRC_FILE}" PATH)
    string(REPLACE "${CMAKE_SOURCE_DIR}/src" "" GROUP_NAME "${SRC_PATH}")
    string(REPLACE "/" "\\" GROUP_NAME "${GROUP_NAME}")
    source_group("${GROUP_NAME}" FILES "${SRC_FILE}")
endforeach()

add_library(${PROJECT_NAME} STATIC ${HEADERS} ${SOURCES})
GroupSourcesByFolder(${PROJECT_NAME})

# Modules
add_subdirectory(modules/Logger)
target_link_libraries(${PROJECT_NAME} Logger)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/WandeltCore/modules/Logger/src)

set(LLVM_DIR "E:\\LLVM_DEV\\LLVM-19.1.0-Windows-X64\\lib\\cmake\\llvm")
message(STATUS "LLVM_DIR: ${LLVM_DIR}")
find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

target_include_directories(${PROJECT_NAME} PUBLIC ${LLVM_INCLUDE_DIRS})
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})

llvm_map_components_to_libnames(llvm_libs support core irreader native)

message(STATUS "LLVM_INCLUDE_DIRS: ${LLVM_INCLUDE_DIRS}")
message(STATUS "LLVM_DEFINITIONS: ${LLVM_DEFINITIONS}")
message(STATUS "LLVM_LIBS: ${llvm_libs}")

target_link_libraries(${PROJECT_NAME} ${llvm_libs})
