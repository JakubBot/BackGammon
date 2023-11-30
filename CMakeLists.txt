cmake_minimum_required(VERSION 3.26)
project(BackGammon2 C)

set(CMAKE_C_STANDARD 17)

add_executable(BackGammon2 main.c
        src/menu.c
        headers/menu.h
        src/renderGame.c
        headers/renderGame.h
        src/author.c
        headers/author.h
        src/rollDice.c
        headers/rollDice.h
        headers/vectorPawn.h
        src/vectorPawn.c
        src/utils.c
        headers/utils.h
        src/playerTurn.c
        headers/playerTurn.h
        src/move.c
        headers/move.h
        src/bidirectionalList.c
        headers/menuUtils.h
        src/menuUtils.c
        src/renderGameUtils.c
        headers/renderGameUtils.h
)

find_package(Curses REQUIRED)
target_link_libraries(BackGammon2 ${CURSES_LIBRARIES})