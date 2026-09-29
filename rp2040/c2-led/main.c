/*----------------------------------------------------------------
 * Experimento 2: Control de brillo del LED centinela
 * Autor: Diego Ambrosio González
 *----------------------------------------------------------------
*/

// Librerias del Kernel
#include <FreeRTOS.h>
#include <task.h>

// Librerias de hardware
#include <stdio.h>
#include <pico/stdlib.h>
#include <pico/mutex.h>
#include <hardware/gpio.h>
#include <hardware/pwm.h>

// Variable de retardo compartida por las dos tareas
volatile uint32_t svBrightness = 500;

// Mutex utilizado para sincronizar el acceso a las variables compartidas
static mutex_t mutex;

// Configuración de hardware
static void setup();

// Tarea 1: Ajustar el brillo del LED centinela
static void ledTask( void *param );

// Tarea 2: Leer el puerto USB
static void usbTask( void *param );

// Lee un valor de tipo uint32_t de stdin y lo muestra en stdout.
// Cuando esta función regresa, contiene el valor leído.
// True si se leyó un entero sin signo; false en caso contrario.
static bool readInt(uint32_t* i);

// Analiza la cadena interpretando su contenido como un número entero.
// buffer: Cadena que comienza con la representación de un número entero.
// endptr: Referencia a un objeto de tipo char*, cuyo valor es establecido por la función al siguiente carácter en str después del valor numérico.
// Si la operación tiene éxito, la función devuelve el número entero convertido como un valor de tipo uint32_t.
static uint32_t atou(char* buffer, char** endptr);

void main( void ){
	setup();
	printf("::: Experiment 2 :::\n");

	xTaskCreate( ledTask,                   // Función que implementa la tarea.
				 "led",                     // Nombre asignado a la tarea.
				 configMINIMAL_STACK_SIZE,  // Tamaño asignado en el stack para la tarea.
				 NULL,                      // Parámetro asignado a la tarea.
				 tskIDLE_PRIORITY,          // Prioridad asignada a la tarea.
				 NULL );                    // Identificador de la tarea.

	xTaskCreate( usbTask,                   // Función que implementa la tarea.
				 "usb",                     // Nombre asignado a la tarea.
				 configMINIMAL_STACK_SIZE,  // Tamaño asignado en el stack para la tarea.
				 NULL,                      // Parámetro asignado a la tarea.
				 tskIDLE_PRIORITY,          // Prioridad asignada a la tarea.
				 NULL );                    // Identificador de la tarea.

	mutex_init(&mutex);

	vTaskStartScheduler();
	for( ;; );
}

// Función de configuración del hardware
static void setup(){
	stdio_init_all();
    gpio_set_function(PICO_DEFAULT_LED_PIN, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(PICO_DEFAULT_LED_PIN);
    pwm_set_wrap(slice_num, 100);
    pwm_set_enabled(slice_num, true);
}

// Tarea 1: Ajustar el brillo del LED centinela
static void ledTask( void *param ){
	uint32_t value;
	vTaskDelay(pdMS_TO_TICKS(400));
	printf("Starting PWM control.\n");

	while(true){
        mutex_enter_blocking(&mutex);
        value = svBrightness;
        mutex_exit(&mutex);
        if(value > 100) value = 100;
        pwm_set_gpio_level(PICO_DEFAULT_LED_PIN, value);
        vTaskDelay(pdMS_TO_TICKS(50));
    }

}

// Tarea 2: Leer el puerto USB
static void usbTask( void *param ){
	uint32_t brightness;
	vTaskDelay(pdMS_TO_TICKS(500));
	while(true){
		printf("LED Brightness [0-100]: ");
		if(!readInt(&brightness)){
			printf("\nInvalid input\n");
			continue;
		}
		mutex_enter_blocking(&mutex);
		svBrightness = brightness;
		mutex_exit(&mutex);
		printf("Brightness set to %d\n", brightness);
	}
}

// Función para leer un valor de tipo uint32_t de stdin y mostrarlo en stdout
static bool readInt(uint32_t* i){
	char ix = 0;
	char buffer[6];
	while(ix < 5){
		buffer[ix] = getchar();
		buffer[ix+1] = 0;
		if(buffer[ix] == '\b' || buffer[ix] == '\x7f'){
			if(ix > 0){
				ix--;
				printf("\b \b");
			}
			continue;
		}
		putchar(buffer[ix]);
		if((buffer[ix] == '\n') || (buffer[ix] == '\r')){
			if(buffer[ix] == '\r') putchar('\n');
			if(ix < 1) return false;
			buffer[ix] = 0;
			*i = atou(buffer, NULL);
			return true;
		}
		else if( (buffer[ix] < '0') || (buffer[ix] > '9') ){
			return false;
		}
		++ix;
	}
	return false;
}

// Función para analizar una cadena de datos interpretando su contenido como un número entero
static uint32_t atou(char* buffer, char** endptr){
	uint32_t num = 0;
	uint32_t pow = 1;
	char *end = buffer;
	while( (*end >= '0') && (*end <= '9') )
		++end;
	if(endptr != NULL) *endptr = end-1;

	for(--end; end >= buffer; --end){
		num+= (*end - '0') * pow;
		pow*= 10;
	}
	return num;
}
