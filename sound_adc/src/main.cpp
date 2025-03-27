// #include <Arduino.h>
// #include "BluetoothA2DPSink.h"

// #define PIN_POT 35        // Pin del potenciómetro para ajustar alpha
// #define PIN_PWM1 23       // Salida PWM 1
// #define PIN_PWM2 22       // Salida PWM 2
// #define PIN_PWM3 21       // Salida PWM 3
// #define PIN_PWM4 19       // Salida PWM 4

// const int freqPWM = 5000; // Frecuencia del PWM
// const int resolution = 8; // Resolución de 8 bits (0-255)

// const float alpha = 0.01; // Factor de suavizado ajustable con potenciómetro

// // Crear objeto Bluetooth
// BluetoothA2DPSink a2dp_sink;

// // Variables de procesamiento
// float audioIntensity = 0;
// // float alpha = 0.1; // Factor de suavizado ajustable con potenciómetro

// // Definir los canales PWM
// const int pwmChannels[] = {0, 1, 2, 3};

// // Función que recibe los datos de audio Bluetooth
// void audio_data_callback(const uint8_t *data, uint32_t length) {
//     float sum = 0;

//     // Recorrer los datos y calcular la intensidad del audio
//     for (uint32_t i = 0; i < length; i += 2) {
//         int16_t sample = (data[i] | (data[i + 1] << 8)); // Convertir a entero
//         sum += abs(sample);
//     }

//     // Promediar la intensidad del audio
//     float newIntensity = sum / (length / 2);

//     // Leer el potenciómetro y ajustar el suavizado (0.01 - 0.5)
//     // int potValue = analogRead(PIN_POT);
//     // alpha = map(potValue, 0, 4095, 1, 30) / 500.0;

//     // Aplicar suavizado
//     audioIntensity = alpha * newIntensity + (1 - alpha) * audioIntensity;

//     // Mapear la intensidad a un rango de PWM (5% - 95%)
//     int pwmValue = map(audioIntensity, 0, 3000, 12, 242);

//     // Aplicar el PWM a las 4 salidas
//     for (int i = 0; i < 4; i++) {
//         ledcWrite(pwmChannels[i], pwmValue);
//     }

//     // Salida Serial
//     Serial.print("Intensidad del audio: ");
//     Serial.print(audioIntensity);
//     Serial.print(" | PWM: ");
//     Serial.print(pwmValue);
//     Serial.print(" | Alpha: ");
//     Serial.println(alpha);
// }

// void setup() {
//     Serial.begin(115200);

//     // Configurar los 4 canales PWM
//     int pwmPins[] = {PIN_PWM1, PIN_PWM2, PIN_PWM3, PIN_PWM4};
//     for (int i = 0; i < 4; i++) {
//         ledcSetup(pwmChannels[i], freqPWM, resolution);
//         ledcAttachPin(pwmPins[i], pwmChannels[i]);
//     }

//     // Configurar el Bluetooth A2DP como receptor
//     a2dp_sink.set_stream_reader(audio_data_callback);
//     a2dp_sink.start("Ov. Familia Audio"); // Nombre Bluetooth visible en el celular
// }

// void loop() {
//     // Esperar a que lleguen datos de audio
//     delay(100);
// }
#include <Arduino.h>
#include "arduinoFFT.h"
#include "BluetoothA2DPSink.h"

#define N 1024            // Número de puntos para la FFT
#define SAMPLE_RATE 44100 // Frecuencia de muestreo
#define PIN_PWM_1 23
#define PIN_PWM_2 22
#define PIN_PWM_3 21
#define PIN_PWM_4 19

// Definir bandas de frecuencia (ajustables)
#define SUB_BASS_MAX 100
#define BASS_MAX 250
#define MID_MAX 2000
#define HIGH_MAX 6000

// Definir umbrales mínimos y máximos para cada banda
float minThresholds[4] = {3, 3, 3, 3};         // Valores mínimos de activación
float maxThresholds[4] = {150, 150, 150, 150}; // Valores máximos de activación

// Definir factores de suavizado (alpha) para cada banda
float alpha[4] = {0.01, 0.01, 0.01, 0.01}; // Valores personalizables

int regulador[4] = {32, 26, 8, 7}; // Valores de regulación de PWM baje 2 para el maximo y suba 2 para el minimo
//                  4   3   2   1

BluetoothA2DPSink a2dp_sink;
double vReal[N];
double vImag[N];
ArduinoFFT<double> FFT = ArduinoFFT<double>(vReal, vImag, N, SAMPLE_RATE);

float smoothedPWM[4] = {0, 0, 0, 0}; // Valores suavizados de PWM

void calcularFFT();
void actualizarPWM(double *intensidades);
void audio_callback(const uint8_t *data, uint32_t length);

void setup()
{
    Serial.begin(115200);
    a2dp_sink.set_stream_reader(audio_callback);
    a2dp_sink.start("Ov. Familia Audio 360");

    // Configurar los 4 canales PWM
    ledcSetup(0, 5000, 8);
    ledcAttachPin(PIN_PWM_1, 0);
    ledcSetup(1, 5000, 8);
    ledcAttachPin(PIN_PWM_2, 1);
    ledcSetup(2, 5000, 8);
    ledcAttachPin(PIN_PWM_3, 2);
    ledcSetup(3, 5000, 8);
    ledcAttachPin(PIN_PWM_4, 3);

    // Definir umbrales y suavizado desde el código
    minThresholds[0] = 10;
    maxThresholds[0] = 180; // Sub-bajos
    minThresholds[1] = 10;
    maxThresholds[1] = 160; // Bajos
    minThresholds[2] = 10;
    maxThresholds[2] = 160; // Medios
    minThresholds[3] = 10;
    maxThresholds[3] = 160; // Agudos

    alpha[0] = 0.1; // Sub-bajos
    alpha[1] = 0.1; // Bajos
    alpha[2] = 0.1; // Medios
    alpha[3] = 0.1; // Agudos
}

void audio_callback(const uint8_t *data, uint32_t length)
{
    for (int i = 0; i < N && i < length; i++)
    {
        vReal[i] = ((int8_t)data[i]) / 128.0; // Normalizar entrada de audio
        vImag[i] = 0;
    }
    calcularFFT();
}

void calcularFFT()
{
    FFT.windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    FFT.compute(FFT_FORWARD);
    FFT.complexToMagnitude();

    double intensidades[4] = {0};
    for (int i = 0; i < (N / 2); i++)
    {
        double freq = (i * SAMPLE_RATE) / N;
        if (freq <= SUB_BASS_MAX)
            intensidades[0] += vReal[i];
        else if (freq <= BASS_MAX)
            intensidades[1] += vReal[i];
        else if (freq <= MID_MAX)
            intensidades[2] += vReal[i];
        else if (freq <= HIGH_MAX)
            intensidades[3] += vReal[i];
    }

    // Normalizar intensidades
    for (int i = 0; i < 4; i++)
    {
        intensidades[i] = sqrt(intensidades[i]); // Hacer la escala más uniforme
    }
    actualizarPWM(intensidades);
}

void actualizarPWM(double *intensidades)
{

    // for (int i = 0; i < 2; i++)
    // {
    //     // Convertir la intensidad en un valor de PWM dentro de los umbrales definidos
    //     int pwmValue = map(intensidades[i] * regulador[i], 0, 500, minThresholds[i], maxThresholds[i]);
    //     pwmValue = constrain(pwmValue, minThresholds[i], maxThresholds[i]); // Asegurar límites

    //     // Aplicar suavizado exponencial
    //     smoothedPWM[i] = alpha[i] * pwmValue + (1 - alpha[i]) * smoothedPWM[i];

    //     ledcWrite(i, (int)smoothedPWM[i]);
    // }

    // // Convertir la intensidad en un valor de PWM dentro de los umbrales definidos
    // int pwmValue = map(intensidades[3] * regulador[3], 0, 500, minThresholds[3], maxThresholds[3]);
    // pwmValue = constrain(pwmValue, minThresholds[3], maxThresholds[3]); // Asegurar límites

    // // Aplicar suavizado exponencial
    // smoothedPWM[3] = alpha[3] * pwmValue + (1 - alpha[3]) * smoothedPWM[3];

    // ledcWrite(0, (int)smoothedPWM[3]);

    // Convertir la intensidad en un valor de PWM dentro de los umbrales definidos
    int pwmValue = map(intensidades[0] * regulador[0], 0, 500, minThresholds[0], maxThresholds[0]);
    pwmValue = constrain(pwmValue, minThresholds[0], maxThresholds[0]); // Asegurar límites

    // Aplicar suavizado exponencial
    smoothedPWM[0] = alpha[0] * pwmValue + (1 - alpha[0]) * smoothedPWM[0];

    ledcWrite(0, (int)smoothedPWM[0]);

    int pwmValue1 = map(intensidades[1] * regulador[1], 0, 500, minThresholds[1], maxThresholds[1]);
    pwmValue1 = constrain(pwmValue1, minThresholds[1], maxThresholds[1]); // Asegurar límites

    // Aplicar suavizado exponencial
    smoothedPWM[1] = alpha[1] * pwmValue1 + (1 - alpha[1]) * smoothedPWM[1];

    ledcWrite(1, (int)smoothedPWM[1]);

    // Convertir la intensidad en un valor de PWM dentro de los umbrales definidos
    int pwmValue2 = map(intensidades[2] * regulador[2], 0, 500, minThresholds[2], maxThresholds[2]);
    pwmValue2 = constrain(pwmValue2, minThresholds[2], maxThresholds[2]); // Asegurar límites

    // Aplicar suavizado exponencial
    smoothedPWM[2] = alpha[2] * pwmValue2 + (1 - alpha[2]) * smoothedPWM[2];

    ledcWrite(2, (int)smoothedPWM[2]);
    ledcWrite(3, (int)smoothedPWM[2]);
}

void loop()
{
    delay(100);
}