
# Copyright 2026 NXP
#
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.22.0)

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

            main.c
            src/states.c
            src/states.h
            src/states/state_init.c
            src/states/state_init.h
            src/states/state_searching_beacon.c
            src/states/state_searching_beacon.h
            src/states/state_menu.c
            src/states/state_menu.h
            src/hm10/hm10.c
            src/hm10/hm10.h
            src/helperFunctions.c
            src/helperFunctions.h
            src/keypad.c
            src/keypad.h
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
             src
             src/states
             src/hm10
)
mcux_convert_binary(BINARY ${APPLICATION_BINARY_DIR}/${MCUX_SDK_PROJECT_NAME}.bin)

