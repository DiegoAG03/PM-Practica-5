/*----------------------------------------------------------------
 * Experimento 3: Control de brillo del LED y lectura del sensor 
   de temperatura por medio de comandos vía USB
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
#include <hardware/adc.h>
#include <hardware/pwm.h>

// Variables compartidas por las tres tareas
volatile uint32_t svBrightness = 500;
volatile float svTempC = 0.0f;
volatile float svTempF = 0.0f;

// Mutex utilizado para sincronizar el acceso a las variables compartidas
static mutex_t mutex;

// Configuración de hardware
static void setup();

// Tarea 1: Lectura de temperatura del integrado
static void tempTask( void *param );

// Tarea 2: Control del brillo del LED
static void ledTask( void *param );

// Tarea 3: Lectura de comandos vía USB
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

// Lee un único carácter de comando de stdin de forma segura
static char readCommand(void);

void main( void ){
	setup();
	printf("::: Experiment 3 :::\n");

	xTaskCreate( tempTask,                  // Función que implementa la tarea.
				 "temp",                    // Nombre asignado a la tarea.
				 configMINIMAL_STACK_SIZE,  // Tamaño asignado en el stack para la tarea.
				 NULL,                      // Parámetro asignado a la tarea.
				 tskIDLE_PRIORITY,          // Prioridad asignada a la tarea.
				 NULL );                    // Identificador de la tarea.

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

	adc_init();
	adc_set_temp_sensor_enabled(true);
	adc_select_input(4); // Sensor interno de temperatura 

  	gpio_set_function(PICO_DEFAULT_LED_PIN, GPIO_FUNC_PWM);
  	uint slice_num = pwm_gpio_to_slice_num(PICO_DEFAULT_LED_PIN);
  	pwm_set_wrap(slice_num, 100);
  	pwm_set_enabled(slice_num, true);
}

// Tarea 1: Lectura de temperatura promedio del integrado
static void tempTask( void *param ){
	uint16_t raw;
	float vADC, temp_C, temp_F;
	float suma_C, suma_F; 

	while(true){
		suma_C = 0;
		suma_F = 0;

	// Temperatura promedio para 10 muestras de 1 segundo 
	for(int i = 0; i < 10; i++){

    	raw = adc_read();
    	vADC = raw * 3.3f / 4095;
    	temp_C = 27 - (vADC - 0.706) / 0.001721;
    	temp_F = (temp_C * 1.8) + 32;

    	suma_C += temp_C;
    	suma_F += temp_F;
    	vTaskDelay(pdMS_TO_TICKS(100));
    	}

		mutex_enter_blocking(&mutex);
    	svTempC = suma_C / 10.0f;
    	svTempF = suma_F / 10.0f;
    	mutex_exit(&mutex);
	}
}

// Tarea 2: Control del brillo del LED
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

// Tarea 3: Lectura de comandos vía USB
static void usbTask( void *param ){
	uint32_t brightness;
	char command;
	float printTempC, printTempF;
	vTaskDelay(pdMS_TO_TICKS(500));
	while(true){
		printf("\nInput command: ");
		command = readCommand();

		switch(command){
			case 't':
				mutex_enter_blocking(&mutex);
        		printTempC = svTempC;
        		printTempF = svTempF;
        		mutex_exit(&mutex);
        		printf("Temp: %0.1fC | %0.1fF\n", printTempC, printTempF);
				break;

			case 'l':
				printf("LED Brightness [0-100]: ");
        		if(!readInt(&brightness)){
          			printf("\nERROR: Invalid input.\n");
          		continue;
        		}
				mutex_enter_blocking(&mutex);
				svBrightness = brightness;
				mutex_exit(&mutex);
				printf("Brightness set to %d\n", brightness);
				break;

			default:
       			printf("ERROR: Unknown command.\n");
        		break;
		}
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

// Función para capturar un comandos de forma segura
static char readCommand(void){
    char command = 0;
    int char_count = 0;
    
    while(true){
        char c = getchar();
        
        if(c == '\b' || c == '\x7f'){
            if(char_count > 0){
                char_count--;
                printf("\b \b");
            }
        } else if(c == '\r' || c == '\n'){
            if(char_count > 0){
                putchar('\n');
                return command;
            }
        } else {
            if(char_count == 0){
                command = c;
                char_count++;
                putchar(c);
            }
        }
    }
}