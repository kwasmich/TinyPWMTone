//
//  main.c
//  PWMTone
//
//  Created by Michael Kwasnicki on 27.08.14.
//  Copyright (c) 2014 Kwasi-ich. All rights reserved.
//

#include <assert.h>
#include <iso646.h>
#include <math.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


const char *TONE_NAME[] = {
    "C-",
    "C#",
    "D-",
    "D#",
    "E-",
    "F-",
    "F#",
    "G-",
    "G#",
    "A-",
    "A#",
    "B-",
    "--",
};

const float TONE_HZ[] = {
    4186.01f, // C-8
    4434.92f, // C#8
    4698.63f, // D-8
    4978.03f, // D#8
    5274.04f, // E-8
    5587.65f, // F-8
    5919.91f, // F#8
    6271.93f, // G-8
    6644.88f, // G#8
    7040.00f, // A-8
    7458.62f, // A#8
    7902.13f, // B-8 (H8)
    0         // --
};


int main(int argc, const char *argv[]) {
    unsigned int timeBase = 3000000;
    char buffer[10];
    char prevLine[10];

    char tone[3] = "--";
    int octave;
    unsigned int duration;

    char *line = fgets(buffer, 10, stdin);
    assert(line != NULL);

    int parsed = sscanf(line, "%c%c%i %i", &tone[0], &tone[1], &octave, &duration);

    while (parsed == 4) {
        //printf( "%s%i, 1/%i\n", tone, octave, duration );
        fflush(stdout);
        int divisor = 1 << (8 - octave - 2);
        int toneIndex = 0;

        for (toneIndex = 0; toneIndex < 13; toneIndex++) {
            if (0 == strncmp(tone, TONE_NAME[toneIndex], 2)) {
                break;
            }
        }

        assert(toneIndex < 13);
        uint16_t melody;

        if (toneIndex < 12) {
            float frequency = TONE_HZ[toneIndex] / divisor;
            float F_CPU = 9.6e6 / 8.0f;
            bool found = false;

            for (int p = 1, pp = 1; p < 65; p *= 8, pp++) {
                float fOCRnx = (F_CPU / (2.0f * p * frequency)) - 1.0f;
                uint16_t OCRnx = roundf(fOCRnx);

                if (OCRnx < 256) {
                    float tinyf = F_CPU / (2.0f * p * (1.0f + OCRnx));
                    melody = (OCRnx << 8) bitor(pp << 6) bitor((32 / duration) bitand 0x3F);
                    printf("0x%04x, ", melody);
                    found = true;
                    break;
                }
            }

            if (!found) {
                printf("\nERROR\n\n");
            }

            unsigned int range = (unsigned int)(600000.0f / frequency);
        } else {
            melody = ((32 / duration) bitand 0x3F);
            printf("0x%04x, ", melody);
        }

        strncpy(prevLine, line, 10);
        line = fgets(buffer, 10, stdin);
        parsed = 0;

        if (line) {
            parsed = sscanf(line, "%c%c%i %i", &tone[0], &tone[1], &octave, &duration);
        }
    }

    // insert code here...
    printf("Hello, World!\n");
    return 0;
}

