cmake_minimum_required(VERSION 3.13)

# Importar el SDK de la tarjeta (debe ir antes de project)
include(../picoSDK_import.cmake)

# Importar el Kernel de FreeRTOS
include(FreeRTOS_Kernel_import.cmake)

project(c1_temp C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

# --- EXPERIMENTO 1: LECTURA DE TEMPERATURA Y LED ---
add_executable(c1_temp main.c)

# Incluir el directorio actual para que el compilador detecte FreeRTOSConfig.h
target_include_directories(c1_temp PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Ligar librerías base, el kernel de FreeRTOS, el gestor de memoria (Heap 4), el ADC
target_link_libraries(c1_temp pico_stdlib FreeRTOS-Kernel FreeRTOS-Kernel-Heap4 hardware_adc)

# Habilitar salida estándar por USB
pico_enable_stdio_usb(c1_temp 1)
pico_enable_stdio_uart(c1_temp 0)

# Generar el archivo .uf2 
pico_add_extra_outputs(c1_temp)
