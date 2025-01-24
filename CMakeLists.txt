# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.0)
set(toolchainVersion 13_3_Rel1)
set(picotoolVersion 2.1.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(gpio C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1
add_executable(gpio
    gpio.c
    buzzer.c
)

# Pull in common dependencies
target_link_libraries(gpio pico_stdlib hardware_pwm hardware_pio hardware_dma)

# Gerar o código PIO para WS2812
pico_generate_pio_header(gpio ${CMAKE_CURRENT_LIST_DIR}/ws2812.pio)

if (PICO_CYW43_SUPPORTED)
    target_link_libraries(gpio pico_cyw43_arch_none)
endif()

# Create map/bin/hex file etc.
pico_add_extra_outputs(gpio)

# Add URL via pico_set_program_url
