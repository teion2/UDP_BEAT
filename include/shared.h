#ifndef SHARED_H
#define SHARED_H

#include <pthread.h>

typedef struct {
    float* audio_buffer;     // Указатель на аудиобуфер
    size_t buffer_size;      // Размер буфера
    int new_data;            // Флаг новых данных
    pthread_mutex_t mutex;   // Мьютекс
    pthread_cond_t cond;     // Условная переменная
} SharedData;

extern SharedData shared;    // Глобальный экземпляр

#endif