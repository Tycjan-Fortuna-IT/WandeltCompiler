project(WandeltCore)

include_directories(${PROJECT_SOURCE_DIR}/src)

file(GLOB_RECURSE HEADERS "src/*.hpp")
file(GLOB_RECURSE SOURCES "src/*.cpp")

add_library(${PROJECT_NAME} STATIC ${HEADERS} ${SOURCES})

# Modules
add_subdirectory(modules/Logger)
target_link_libraries(${PROJECT_NAME} Logger)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/WandeltCore/modules/Logger/src)

# set(LLVM_DIR "E:/LLVM")
set(LLVM_DIR "C:\\Users\\tycja\\Downloads\\LLVM-19.1.0-Windows-X64\\LLVM-19.1.0-Windows-X64\\lib\\cmake\\llvm")
find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

target_include_directories(${PROJECT_NAME} PUBLIC ${LLVM_INCLUDE_DIRS})
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})

llvm_map_components_to_libnames(llvm_libs support core irreader)

target_link_libraries(${PROJECT_NAME} ${llvm_libs})
