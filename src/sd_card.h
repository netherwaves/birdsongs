#pragma once
#ifndef H_SD_CARD
#define H_SD_CARD


#include <daisy_seed.h>
using namespace daisy;


// config objects
SdmmcHandler sdcard;
FatFSInterface fsi;


void ConfigureSDCard()
{
    // initialize SD/MMC handler
    SdmmcHandler::Config sdconfig;
    sdconfig.Defaults();
    sdconfig.speed = SdmmcHandler::Speed::VERY_FAST;
    sdconfig.width = SdmmcHandler::BusWidth::BITS_4;
    sdcard.Init(sdconfig);

    // initialize FatFS interface
    fsi.Init(FatFSInterface::Config::MEDIA_SD);

    // mount SD card
    f_mount(&fsi.GetSDFileSystem(), fsi.GetSDPath(), 0);
}


#endif