# Práctica 5: Implementación de FreeRTOS en RP2040 (DualMCU)

**Autor:** Diego Ambrosio González
**Asignatura:** Procesadores Multinúcleo

Repositorio creado para la Práctica 5 de Procesadores Multinúcleo, el cual contiene la implementación de un sistema operativo en tiempo real (RTOS) utilizando FreeRTOS sobre la arquitectura de un microcontrolador RP2040. El objetivo principal es gestionar múltiples tareas concurrentes (lectura ADC, control PWM y comunicación serial) protegiendo los recursos compartidos mediante el uso de *Mutex*.

## Estructura del Proyecto

El repositorio está dividido en cuatro experimentos incrementales:

*   **`c1-temp/`**: Tarea dedicada a la configuración del canal 4 del ADC para la lectura continua del sensor de temperatura interno del RP2040, tanto en grados Celsius como Fahrenheit.
*   **`c2-led/`**: Tarea dedicada al control de la intensidad luminosa del LED integrado utilizando modulación por ancho de pulsos (PWM).
*   **`c3-usb_temp_led/`**: Integración de las tareas anteriores sumando una interfaz de comandos por puerto serial (USB). Permite solicitar la lectura de temperatura (comando `t`) o ajustar el nivel de PWM del LED (comando `l`) mediante el uso de variables globales protegidas por exclusión mutua (`mutex_enter_blocking`).
*   **`c4-marquee/`**: Implementación de una cuarta tarea que controla una marquesina de 8 LEDs externos. Introduce una máquina de estados controlada por el comando `m`, permitiendo cambiar la dirección de la secuencia (`l`: izquierda, `r`: derecha, `p`: ping-pong, `o`: apagado) y su velocidad de ejecución en tiempo real sin interrumpir los cálculos térmicos ni la salida PWM.

## Requisitos de Software

*   Entorno Ubuntu Linux.
*   Raspberry Pi Pico SDK configurado.
*   Código fuente de FreeRTOS.
*   Toolchain de compilación: `cmake`, `make`, `gcc-arm-none-eabi`.
*   Terminal serial: `gtkterm`.

## Instrucciones de Compilación y Flasheo

### 1. Compilación del código
Abre una terminal en la raíz de la carpeta `rp2040` y ejecuta los siguientes comandos para generar los archivos binarios:

```bash
mkdir build
cd build
cmake ..
make
```
Esto generará los archivos `.uf2` ejecutables dentro de las subcarpetas correspondientes a cada experimento en el directorio `build`.

### 2. Carga del binario a la DualMCU (RP2040)
1. Conecta la placa UNIT DualMCU a la computadora vía USB mientras mantienes presionado el botón **BOOT** correspondiente al RP2040.
2. El sistema montará un dispositivo de almacenamiento masivo.
3. Arrastra y suelta el archivo `.uf2` del experimento deseado (por ejemplo, `c4_marquee.uf2`) dentro de la unidad de la placa.
4. La placa se reiniciará automáticamente y comenzará a ejecutar el kernel de FreeRTOS.

### 3. Ejecución de la Interfaz Serial
Para interactuar con el menú de comandos en los experimentos 3 y 4, abre la terminal serial con permisos de superusuario configurando los baudios a 115200:

```bash
sudo gtkterm -p /dev/ttyACM0 -s 115200
```
> **Nota de hardware:** Debido a la gestión de los puertos seriales virtuales, es estrictamente necesario ir a la pestaña `Configuration -> Control signals` dentro de la ventana de `gtkterm` y habilitar la señal **DTR** en cada nueva conexión para visualizar la salida estándar.
