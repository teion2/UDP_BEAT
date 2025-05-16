#include "audio.h"
#include "shared.h"
#include <portaudio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static PaStream* audio_stream = NULL;

static int audio_callback(const void* inputBuffer, void* outputBuffer,
                         unsigned long framesPerBuffer,
                         const PaStreamCallbackTimeInfo* timeInfo,
                         PaStreamCallbackFlags statusFlags,
                         void* userData) {
    (void)outputBuffer;
    (void)timeInfo;
    (void)statusFlags;
    (void)userData;

    pthread_mutex_lock(&shared.mutex);
    
    // Освобождаем старый буфер если существует
    if (shared.audio_buffer != NULL) {
        free(shared.audio_buffer);
        shared.audio_buffer = NULL;
    }

    // Выделяем память с проверкой
    shared.audio_buffer = (float*)malloc(framesPerBuffer * sizeof(float));
    if (shared.audio_buffer == NULL) {
        pthread_mutex_unlock(&shared.mutex);
        fprintf(stderr, "Memory allocation failed in audio callback\n");
        return paAbort;
    }

    // Копируем данные
    memcpy(shared.audio_buffer, inputBuffer, framesPerBuffer * sizeof(float));
    shared.buffer_size = framesPerBuffer;
    shared.new_data = 1;

    pthread_cond_signal(&shared.cond);
    pthread_mutex_unlock(&shared.mutex);

    return paContinue;
}

int audio_init() {
    PaError err = Pa_Initialize();
    if (err != paNoError) {
        fprintf(stderr, "PortAudio initialization error: %s\n", Pa_GetErrorText(err));
        return -1;
    }

    PaStreamParameters inputParameters = {
        .device = Pa_GetDefaultInputDevice(),
        .channelCount = 2,
        .sampleFormat = paFloat32,
        .suggestedLatency = Pa_GetDeviceInfo(Pa_GetDefaultInputDevice())->defaultLowInputLatency,
        .hostApiSpecificStreamInfo = NULL
    };

    err = Pa_OpenStream(&audio_stream,
                       &inputParameters,
                       NULL,
                       44100,
                       512,
                       paClipOff,
                       audio_callback,
                       NULL);
    if (err != paNoError) {
        fprintf(stderr, "PortAudio stream open error: %s\n", Pa_GetErrorText(err));
        Pa_Terminate();
        return -1;
    }

    return 0;
}

int audio_start() {
    if (audio_stream == NULL) {
        fprintf(stderr, "Audio stream not initialized\n");
        return -1;
    }

    PaError err = Pa_StartStream(audio_stream);
    if (err != paNoError) {
        fprintf(stderr, "PortAudio stream start error: %s\n", Pa_GetErrorText(err));
        return -1;
    }
    return 0;
}

void audio_cleanup() {
    if (audio_stream) {
        Pa_StopStream(audio_stream);
        Pa_CloseStream(audio_stream);
        audio_stream = NULL;
    }
    Pa_Terminate();
}