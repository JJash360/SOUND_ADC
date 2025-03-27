Proyecto: Control de Variadores de Frecuencia con ESP32

Autor

Jashua Jafet Solon Aquino

Descripción

Este proyecto permite la recepción de audio mediante Bluetooth en un ESP32, donde se realiza un análisis de la señal mediante FFT para dividirla en cuatro bandas de frecuencia (sub-bajos, bajos, medios y agudos). La intensidad de cada banda se convierte en una señal PWM que se envía a una placa de control de variadores de frecuencia.

Características

Recepción de audio por Bluetooth utilizando BluetoothA2DPSink.

Procesamiento de la señal mediante Transformada Rápida de Fourier (FFT) con la biblioteca arduinoFFT.

Salida de 4 señales PWM correspondientes a 4 bandas de frecuencia diferentes.

Suavizado y ajuste de intensidades PWM con umbrales configurables.

Integración con una placa electrónica personalizada para el control de variadores de frecuencia.

Requisitos

ESP32 DevKit V4

Arduino IDE con bibliotecas:

BluetoothA2DPSink

arduinoFFT

Fuente de alimentación adecuada

Placa electrónica de control de variadores de frecuencia (diseñada por el autor)

Conexión de Hardware

ESP32

Descripción

23

PWM Canal 1

22

PWM Canal 2

21

PWM Canal 3

19

PWM Canal 4

Instalación y Uso

Cargar el código en el ESP32 mediante Arduino IDE.

Emparejar el ESP32 con un dispositivo Bluetooth bajo el nombre Ov. Familia Audio 360.

Reproducir música desde un dispositivo Bluetooth.

La señal de audio se procesará en tiempo real y se generarán las señales PWM correspondientes.

Licencia

Este proyecto se distribuye bajo la Licencia MIT. Puedes modificar y distribuir el código con la debida atribución al autor.

