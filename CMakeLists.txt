cmake_minimum_required(VERSION 3.16)
project(DOS_GAME)


set (SOURCES
    ${CMAKE_SOURCE_DIR}/MAIN_VGA.C
    ${CMAKE_SOURCE_DIR}/GFX.C
    ${CMAKE_SOURCE_DIR}/WALL.C
    ${CMAKE_SOURCE_DIR}/MATH.C
    ${CMAKE_SOURCE_DIR}/KEY_INPUT.C
    ${CMAKE_SOURCE_DIR}/BSP.C
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(BSP ${SOURCES})
target_compile_options(BSP PUBLIC -O3 -Wall -Wextra)
