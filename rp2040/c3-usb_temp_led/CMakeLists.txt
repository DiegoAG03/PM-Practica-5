cmake_minimum_required(VERSION 3.13)

# Importar el SDK de la tarjeta (debe ir antes de project)
include(../picoSDK_import.cmake)

# Importar el Kernel de FreeRTOS
include(FreeRTOS_Kernel_import.cmake)

project(c3_usb_temp_led C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

# -- EXPERIMENTO 3: LECTURA DE TEMPERATURA Y CONTROL DE BRILLO DEL LED POR USB --
add_executable(c3_usb_temp_led main.c)

# Incluir el directorio actual para que el compilador detecte FreeRTOSConfig.h
target_include_directories(c3_usb_temp_led PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Ligar librerías base, el kernel de FreeRTOS, el gestor de memoria (Heap 4), el PWM y el ADC
target_link_libraries(c3_usb_temp_led pico_stdlib FreeRTOS-Kernel FreeRTOS-Kernel-Heap4 hardware_pwm hardware_adc)

# Habilitar salida estándar por USB
pico_enable_stdio_usb(c3_usb_temp_led 1)
pico_enable_stdio_uart(c3_usb_temp_led 0)

# Generar el archivo .uf2 
pico_add_extra_outputs(c3_usb_temp_led)
