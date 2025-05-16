#include "audio.h"
#include "shared.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include <windows.h>
#include <conio.h>

volatile sig_atomic_t stop = 0;

void handle_sigint(int sig) {
    (void)sig;
    stop = 1;
}

void* input_thread(void* arg) {
    (void)arg;
    printf("Press Enter to stop...\n");
    while (!stop) {
        if (_kbhit()) {
            int c = _getch();
            if (c == '\r' || c == '\n') {
                stop = 1;
                break;
            }
        }
        Sleep(100);
    }
    return NULL;
}

int main() {
    signal(SIGINT, handle_sigint);

    // Инициализация shared данных
    memset(&shared, 0, sizeof(shared));
    if (pthread_mutex_init(&shared.mutex, NULL) != 0) {
        fprintf(stderr, "Mutex init failed\n");
        return 1;
    }
    if (pthread_cond_init(&shared.cond, NULL) != 0) {
        fprintf(stderr, "Condition init failed\n");
        pthread_mutex_destroy(&shared.mutex);
        return 1;
    }

    // Инициализация аудио
    if (audio_init() != 0) {
        fprintf(stderr, "Audio init failed\n");
        pthread_mutex_destroy(&shared.mutex);
        pthread_cond_destroy(&shared.cond);
        return 1;
    }

    // Поток для обработки ввода
    pthread_t input_thr;
    if (pthread_create(&input_thr, NULL, input_thread, NULL) != 0) {
        fprintf(stderr, "Thread creation failed\n");
        audio_cleanup();
        pthread_mutex_destroy(&shared.mutex);
        pthread_cond_destroy(&shared.cond);
        return 1;
    }

    // Запуск аудио
    printf("Starting audio capture...\n");
    if (audio_start() != 0) {
        fprintf(stderr, "Audio start failed\n");
        stop = 1;
    }

    // Главный цикл
    while (!stop) {
        // Здесь может быть обработка данных
        Sleep(100);
    }

    // Очистка
    audio_cleanup();
    pthread_join(input_thr, NULL);
    
    pthread_mutex_destroy(&shared.mutex);
    pthread_cond_destroy(&shared.cond);
    
    if (shared.audio_buffer) {
        free(shared.audio_buffer);
        shared.audio_buffer = NULL;
    }

    printf("Program stopped\n");
    return 0;
}