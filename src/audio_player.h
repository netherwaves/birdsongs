#pragma once
#ifndef H_AUDIOPLAYER
#define H_AUDIOPLAYER


// includes
#include <daisy_seed.h>
#define BUFSIZE 4096

// namespace
using namespace daisy;


// objects
SdmmcHandler sdcard;
FatFSInterface fsi;
// samplers
WavPlayer sampler;
//buffers
int16_t DSY_SDRAM_BSS buffer[BUFSIZE];


void ConfigureAudioPlayer() {
    // initialize SD/MMC handler
    SdmmcHandler::Config sdconfig;
    sdconfig.Defaults();
    sdconfig.speed = SdmmcHandler::Speed::VERY_FAST;
    sdcard.Init(sdconfig);

    // initialize FatFS interface
    fsi.Init(FatFSInterface::Config::MEDIA_SD);

    // mount SD card
    f_mount(&fsi.GetSDFileSystem(), fsi.GetSDPath(), 0);

    // define path for birdcalls
    char *pathBirdcalls;
    strcpy(pathBirdcalls, fsi.GetSDPath());
    strcat(pathBirdcalls, "birdcalls/");

    // initialize birdcall sampler
    sampler.Init(pathBirdcalls, buffer, BUFSIZE, 1);
}

void LoopAudioPlayer() {
    sampler.Prepare();
}

#endif
