cmake_minimum_required(VERSION 3.13)

# Importar el SDK de la tarjeta (debe ir antes de project)
include(../picoSDK_import.cmake)

# Importar el Kernel de FreeRTOS
include(FreeRTOS_Kernel_import.cmake)

project(c2_led C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

# --- EXPERIMENTO 2: CONTROL DE BRILLO DEL LED ---
add_executable(c2_led main.c)

# Incluir el directorio actual para que el compilador detecte FreeRTOSConfig.h
target_include_directories(c2_led PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Ligar librerías base, el kernel de FreeRTOS, el gestor de memoria (Heap 4), el PWM
target_link_libraries(c2_led pico_stdlib FreeRTOS-Kernel FreeRTOS-Kernel-Heap4 hardware_pwm)

# Habilitar salida estándar por USB
pico_enable_stdio_usb(c2_led 1)
pico_enable_stdio_uart(c2_led 0)

# Generar el archivo .uf2 
pico_add_extra_outputs(c2_led)
