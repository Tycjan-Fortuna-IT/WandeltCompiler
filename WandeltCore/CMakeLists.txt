project(WandeltCore)

include_directories(${PROJECT_SOURCE_DIR}/src)

file(GLOB_RECURSE HEADERS "src/*.hpp")
file(GLOB_RECURSE SOURCES "src/*.cpp")

add_library(${PROJECT_NAME} STATIC ${HEADERS} ${SOURCES})

# Modules
add_subdirectory(modules/Logger)
target_link_libraries(${PROJECT_NAME} Logger)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/WandeltCore/modules/Logger/src)
