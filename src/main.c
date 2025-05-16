#include <stdio.h>
#include <pthread.h>
#include <windows.h>
#include <stdint.h>
#include <stdatomic.h>
#include <conio.h>

typedef struct {
    int thread_id;
    uint64_t counter;
    pthread_mutex_t lock;
    atomic_int keep_running;
    int sleep_ms;
    const char* thread_name;
    const char* action;
} ThreadData;

void* thread_func(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    while(data->keep_running) {
        Sleep(data->sleep_ms);
        pthread_mutex_lock(&data->lock);
        printf("[%s-%d] %s (итерация: %llu)\n", 
               data->thread_name, data->thread_id, 
               data->action, ++data->counter);
        pthread_mutex_unlock(&data->lock);
        
        if(!data->keep_running) break;
    }
    pthread_mutex_lock(&data->lock);
    printf("[%s-%d] Поток завершен (финальная итерация: %llu)\n", 
           data->thread_name, data->thread_id, data->counter);
    pthread_mutex_unlock(&data->lock);
    return NULL;
}

int main() {
    pthread_t threads[3];
    ThreadData thread_data[3] = {
        {1, 0, PTHREAD_MUTEX_INITIALIZER, 1, 300, "Аудио", "Захват данных"},
        {2, 0, PTHREAD_MUTEX_INITIALIZER, 1, 500, "BTT", "Анализ BPM"},
        {3, 0, PTHREAD_MUTEX_INITIALIZER, 1, 700, "UDP", "Отправка данных"}
    };

    printf("=== Система управления потоками ===\n");
    printf("Потоки:\n");
    for(int i = 0; i < 3; i++) {
        printf("%d. %s (%dms) - %s\n", 
               thread_data[i].thread_id, 
               thread_data[i].thread_name, 
               thread_data[i].sleep_ms,
               thread_data[i].action);
    }
    printf("Нажмите Enter для остановки...\n\n");

    // Создание потоков
    for(int i = 0; i < 3; i++) {
        pthread_create(&threads[i], NULL, thread_func, &thread_data[i]);
    }

    // Ожидание нажатия Enter
    while(!_kbhit() || getchar() != '\n') {
        Sleep(100);
    }

    // Установка флагов завершения
    for(int i = 0; i < 3; i++) {
        thread_data[i].keep_running = 0;
    }

    // Ожидание завершения потоков (без tryjoin_np)
    for(int i = 0; i < 3; i++) {
        printf("Ожидание завершения потока %s-%d...\n", 
              thread_data[i].thread_name, thread_data[i].thread_id);
        pthread_join(threads[i], NULL);
        pthread_mutex_destroy(&thread_data[i].lock);
    }

    printf("\nИтоговые счётчики:\n");
    for(int i = 0; i < 3; i++) {
        printf("%s-%d: %llu итераций\n", 
               thread_data[i].thread_name,
               thread_data[i].thread_id, 
               thread_data[i].counter);
    }

    return 0;
}