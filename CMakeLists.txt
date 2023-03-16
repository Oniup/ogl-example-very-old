cmake_minimum_required(VERSION 3.2)
project(example_project)

add_subdirectory(ogl)

file(GLOB_RECURSE EXAMPLE_PROJECT_HEADER_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "example/src/*.hpp")
file(GLOB_RECURSE EXAMPLE_PROJECT_SOURCE_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "example/src/*.cpp")

add_executable(
    example_project
    ${EXAMPLE_PROJECT_HEADER_FILES}
    ${EXAMPLE_PROJECT_SOURCE_FILES}
)

target_link_libraries(
    example_project

    PUBLIC ogl
)

target_include_directories(
    example_project

    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/example/src
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ogl/src
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ogl/thirdparty/entt/single_include
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ogl/thirdparty/glm
)

set_target_properties(
    example_project

    PROPERTIES 
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED OFF
    CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/arch"
    CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)

target_compile_definitions(
    example_project PUBLIC 

    _CRT_NONSTDC_NO_WARNINGS
)

add_custom_command(
    TARGET example_project 
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/example/assets
    $<TARGET_FILE_DIR:example_project>/example/assets
)

add_custom_command(
    TARGET example_project 
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/ogl/assets
    $<TARGET_FILE_DIR:example_project>/ogl/assets
)

if (MSVC)
    add_custom_command(
        TARGET example_project POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        $<TARGET_FILE_DIR:ogl>
        $<TARGET_FILE_DIR:example_project>
    )

    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT example_project)
endif ()
