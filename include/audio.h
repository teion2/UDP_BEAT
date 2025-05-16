#ifndef AUDIO_H
#define AUDIO_H

#include <portaudio.h>

// Структура для передачи аудиоданных между потоками
typedef struct {
    float* buffer;
    unsigned long frames;
    int channels;
} AudioChunk;

// Инициализация аудиозахвата
int audio_init();
// Запуск аудиозахвата
int audio_start();
// Остановка аудиозахвата
void audio_cleanup();

#endif // AUDIO_H