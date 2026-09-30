cmake_minimum_required(VERSION 4.3)
project(cshooter C)

set(CMAKE_C_STANDARD 11)

find_package(raylib REQUIRED)

add_executable(cshooter main.c
        common.h
        player.h
        player.c
        bullet.h
        bullet.c
        enemy.h
        enemy.c
        common.c
        my_math.h
        my_math.c)

target_link_libraries(cshooter PRIVATE raylib m)