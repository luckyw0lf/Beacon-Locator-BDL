
# Copyright 2026 NXP
#
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.22.0)
if(NOT DEFINED board OR "${board}" STREQUAL "")
    set(board frdmmcxa153 CACHE STRING "Target board" FORCE)
endif()

cmake_path(APPEND conf_file_path ${CMAKE_CURRENT_LIST_DIR} ${board} prj.conf)
set(CONF_FILE ${conf_file_path} ${CONF_FILE})
include(${SdkRootDirPath}/cmake/extension/mcux.cmake)

project(getting_started LANGUAGES C CXX ASM PROJECT_BOARD_PORT_PATH ${board_root}/${board}/ese_examples/general/getting_started)


include(${board}/board_files.cmake)
include(${SdkRootDirPath}/CMakeLists.txt)


mcux_add_source(
    SOURCES board/board.h
            lib/fifo/fifo.c
            lib/fifo/fifo.h
            lib/leds/leds.c
            lib/leds/leds.h
            lib/serial/retarget.c
            lib/serial/serial.c
            lib/serial/serial.h
            lib/lcd/lcd.c
            lib/lcd/lcd.h
            lib/lpi2c/lpi2c.c
            lib/lpi2c/lpi2c.h
            lib/oled/oled.c
            lib/oled/oled.h
            lib/mpr121/mpr121.c
            lib/mpr121/mpr121.h
            lib/serial2lpuart/lpuart2.c
            lib/serial2lpuart/lpuart2.h
            lib/sll/sll.c
            lib/sll/sll.h
            lib/system_timer/system_timer.c
            lib/system_timer/system_timer.h

            main.c
            src/states.c
            src/states.h
            src/states/state_init.c
            src/states/state_init.h
            src/states/state_searching_beacon.c
            src/states/state_searching_beacon.h
            src/states/state_navigation.c
            src/states/state_navigation.h
            src/states/state_game_missions/state_game_missions.h
            src/states/state_game_missions/state_experience_mission.c
            src/states/state_game_missions/state_material_question.c
            src/states/state_game_missions/state_material_collection.c
            src/states/state_game_missions/state_build_test.c
            src/states/state_game_missions/state_finish.c
            src/states/state_menu.c
            src/states/state_menu.h
            src/beacon/beacon_parser.c
            src/beacon/beacon_parser.h
            src/navigation/routes.c
            src/navigation/routes.h
            src/hm10/hm10.c
            src/hm10/hm10.h
            src/helperFunctions.c
            src/helperFunctions.h
            src/keypad.c
            src/keypad.h
            src/game_logger/game_logger.h
            src/game_logger/game_logger.c
            src/game_logger/fsl_flashiap.c
            src/game_logger/fsl_flashiap.h
)

mcux_add_include(
    INCLUDES .
             board
             lib/fifo
             lib/leds
             lib/serial
             lib/lcd
             lib/lpi2c
             lib/oled
             lib/mpr121
             lib/serial2lpuart
             lib/sll
             lib/system_timer
             
             src
             src/game_logger
             src/states
             src/states/state_game_missions
             src/beacon
             src/navigation
             src/hm10
)
mcux_convert_binary(BINARY ${APPLICATION_BINARY_DIR}/${MCUX_SDK_PROJECT_NAME}.bin)

