#pragma once
#ifndef H_AUDIOPLAYER
#define H_AUDIOPLAYER


// includes
#include <daisy_seed.h>
#include "sd_card.h"

using namespace daisy;

// buffer
#define BUFSIZE 4096U
int16_t DSY_SDRAM_BSS ambiBuffer[BUFSIZE];
int16_t DSY_SDRAM_BSS birdBuffer[BUFSIZE];

// samplers
WavPlayer ambiSampler;
WavPlayer birdSampler;


void ConfigureAudioPlayer() {
    // define path for birdcalls
    char *pathBirdcalls;
    strcpy(pathBirdcalls, fsi.GetSDPath());
    strcat(pathBirdcalls, "birdcalls/");

    // define path for ambience
    char *pathAmbience;
    strcpy(pathAmbience, fsi.GetSDPath());
    strcat(pathAmbience, "ambience/");


    // initialize samplers
    ambiSampler.Init(pathAmbience, ambiBuffer, BUFSIZE, 2);
    birdSampler.Init(pathBirdcalls, birdBuffer, BUFSIZE, 1);
}

void PrepareSamplers() {
    ambiSampler.Prepare();
    birdSampler.Prepare();
}

#endif
