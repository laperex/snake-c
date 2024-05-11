cmake_minimum_required(VERSION 3.0.0)
project(snake VERSION 0.1.0 LANGUAGES C)

include(CTest)
enable_testing()

file(GLOB_RECURSE TARGET_SRC "main.c")

add_executable(main ${TARGET_SRC})

find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_ttf CONFIG REQUIRED)
target_link_libraries(main PRIVATE SDL2::SDL2 SDL2::SDL2main)
target_link_libraries(main PRIVATE SDL2_ttf)


set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
