#include "shared.h"
#include <stdlib.h>

SharedData shared = {
    .audio_buffer = NULL,
    .buffer_size = 0,
    .new_data = 0,
    .mutex = PTHREAD_MUTEX_INITIALIZER,
    .cond = PTHREAD_COND_INITIALIZER
};