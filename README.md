# Proyecto: Procesamiento de Audio en ESP32 con Bluetooth y Salidas PWM

Autor: Jashua Jafet Solon Aquino

## Descripción
Este proyecto permite la recepción de audio mediante Bluetooth en un ESP32, la conversión de la señal de audio en cuatro bandas de frecuencia y la salida de señales PWM que pueden ser utilizadas para controlar dispositivos externos, como variadores de frecuencia o sistemas de iluminación.

## Características
- **Recepción de Audio Bluetooth**: Utiliza la librería `BluetoothA2DPSink` para recibir audio desde un dispositivo externo.
- **Análisis de Frecuencia**: Se implementa una Transformada Rápida de Fourier (FFT) con `arduinoFFT` para dividir el audio en cuatro bandas de frecuencia: sub-bajos, bajos, medios y agudos.
- **Generación de PWM**: Se generan señales PWM en cuatro pines del ESP32, cada una representando una banda de frecuencia específica.
- **Suavizado de Señal**: Se aplica un factor de suavizado a la señal PWM para evitar fluctuaciones bruscas.

## Hardware Utilizado
- ESP32
- Módulo Bluetooth integrado en el ESP32
- Variadores de frecuencia o dispositivos de salida compatibles con PWM
- Circuito de potencia o amplificación si es necesario

## Conexiones
| Pin ESP32 | Función |
|-----------|---------|
| 23        | PWM Sub-Bajos |
| 22        | PWM Bajos |
| 21        | PWM Medios |
| 19        | PWM Agudos |

## Librerías Requeridas
- `Arduino.h`
- `arduinoFFT.h`
- `BluetoothA2DPSink.h`

## Instalación y Uso
1. Instalar las librerías necesarias en el entorno de desarrollo (Arduino IDE, PlatformIO, etc.).
2. Subir el código al ESP32.
3. Conectar un dispositivo Bluetooth al ESP32 con el nombre `Ov. Familia Audio 360`.
4. Observar la conversión del audio en señales PWM en los pines de salida.

## Licencia
Este proyecto está licenciado bajo la **Licencia MIT**. Puedes usarlo, modificarlo y distribuirlo libremente, siempre y cuando se mencione la autoría original.

---

## Contacto
Para consultas o mejoras, puedes contactar a Jashua Jafet Solon Aquino.

