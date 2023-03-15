cmake_minimum_required(VERSION 3.2)
project(game)

add_subdirectory(ogl)

file(GLOB_RECURSE GAME_HEADER_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "project/src/*.hpp")
file(GLOB_RECURSE GAME_SOURCE_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "project/src/*.cpp")

add_executable(
    game
    ${GAME_HEADER_FILES}
    ${GAME_SOURCE_FILES}
)

target_link_libraries(
    game

    PUBLIC ogl
)

target_include_directories(
    game

    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/project/src
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ogl/src
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ogl/thirdparty/entt/single_include
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ogl/thirdparty/glm
)

set_target_properties(
    game

    PROPERTIES 
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED OFF
    CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/arch"
    CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)

target_compile_definitions(
    game PUBLIC 

    _CRT_NONSTDC_NO_WARNINGS
)

add_custom_command(
    TARGET game 
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/project/assets
    $<TARGET_FILE_DIR:game>/project/assets
)

add_custom_command(
    TARGET game 
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/ogl/assets
    $<TARGET_FILE_DIR:game>/ogl/assets
)

if (MSVC)
    add_custom_command(
        TARGET game POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        $<TARGET_FILE_DIR:ogl>
        $<TARGET_FILE_DIR:game>
    )

    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT game)
endif ()
