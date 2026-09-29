/*----------------------------------------------------------------
 * Experimento 1: LED parpadeante y lectura de temperatura
 * Autor: Diego Ambrosio González
 *----------------------------------------------------------------
*/

// Librerias del Kernel
#include <FreeRTOS.h>
#include <task.h>

// Librerias de hardware
#include <stdio.h>
#include <pico/stdlib.h>
#include <hardware/gpio.h>
#include <hardware/adc.h>

// Configuración de hardware
static void setup();

// Tarea principal para el LED y el sensor 
static void mainTask( void *param );


void main( void ){
	setup();
	printf("::: Experiment 1 :::\n");

	xTaskCreate( mainTask,                  // Función que implementa la tarea.
				 "main",                    // Nombre asignado a la tarea.
				 configMINIMAL_STACK_SIZE,  // Tamaño asignado en el stack para la tarea.
				 NULL,                      // Parámetro asignado a la tarea.
				 1,                         // Prioridad asignada a la tarea.
				 NULL );                    // Identificador de la tarea.

	vTaskStartScheduler();
	for( ;; );
}

// Función de configuración del hardware
static void setup(){
	stdio_init_all();
	adc_init();
	adc_set_temp_sensor_enabled(true);
	adc_select_input(4); // Sensor interno de temperatura 

	gpio_init(PICO_DEFAULT_LED_PIN);
	gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
	gpio_put(PICO_DEFAULT_LED_PIN, 0);
}

// Tarea principal: LED parpadeante y lectura de temperatura 
static void mainTask( void *param ){
	int value = 0;
	uint16_t raw;
	float vADC;
	float temp_C;
	float temp_F;

	while(true){
		raw = adc_read();
		vADC = raw * 3.3f / 4095;
		temp_C = 27 - (vADC - 0.706) / 0.001721;
		temp_F = (temp_C * 1.8) + 32;
		printf("Temp. Celsius: %0.0fC | Fahrenheit: %0.0fF | VADC: %0.3fV \n", temp_C, temp_F, vADC);
		gpio_put(PICO_DEFAULT_LED_PIN, value);
		value = !value;
		vTaskDelay(pdMS_TO_TICKS(1000));
	}

}
