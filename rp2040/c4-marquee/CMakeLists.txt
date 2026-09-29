cmake_minimum_required(VERSION 3.13)

# Importar el SDK de la tarjeta (debe ir antes de project)
include(../picoSDK_import.cmake)

# Importar el Kernel de FreeRTOS
include(FreeRTOS_Kernel_import.cmake)

project(c4_marquee C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

# - EXPERIMENTO 4: LECTURA DE TEMPERATURA Y CONTROL DE LED Y MARQUESINA POR USB -
add_executable(c4_marquee main.c)

# Incluir el directorio actual para que el compilador detecte FreeRTOSConfig.h
target_include_directories(c4_marquee PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Ligar librerías base, el kernel de FreeRTOS, el gestor de memoria (Heap 4), el PWM y el ADC
target_link_libraries(c4_marquee pico_stdlib FreeRTOS-Kernel FreeRTOS-Kernel-Heap4 hardware_pwm hardware_adc)

# Habilitar salida estándar por USB
pico_enable_stdio_usb(c4_marquee 1)
pico_enable_stdio_uart(c4_marquee 0)

# Generar el archivo .uf2 
pico_add_extra_outputs(c4_marquee)
