#include <daisy_seed.h>
#include <daisysp.h>
#include <cJSON.h>

// namespaces
using namespace daisy;

// my includes
#include "sd_card.h"
#include "audio_player.h"
#include "pitch_detector.h"
#include "lcd_1602.h"

// hardware
DaisySeed hw;

// data
FIL dataFile;
char* rawData;
cJSON* jsonData;

// header defines
void ConfigureLCDScreen();
void ConfigureRegistry();


// audio callback
void AudioCallback(AudioHandle::InterleavingInputBuffer  in,
                   AudioHandle::InterleavingOutputBuffer out,
                   size_t                                size)
{
    int16_t* ambi_out;

    for(size_t i = 0; i < size; i += 2)
    {
        // retrieve samples
        ambi_out = ambiSampler.StreamStereo();

        // send to output buffer
        out[i]     = s162f(ambi_out[0]);
        out[i + 1] = s162f(ambi_out[1]);
    }
}


int main(void)
{
    // initialize hardware
    hw.Configure();
    hw.Init();

    // logger
    hw.StartLog(true);
    hw.SetLed(1);

    System::Delay(1000);

    // configurations
    ConfigureSDCard();
    ConfigureRegistry();
    ConfigureAudioPlayer();
    ConfigurePitchDetection();
    ConfigureLCDScreen();

    // start audio
    hw.SetAudioBlockSize(48);
    hw.StartAudio(AudioCallback);

    hw.SetLed(0);

    for(;;)
    {
        LoopAudioPlayer();
    }
}


// LCD screen configuration
void ConfigureLCDScreen()
{
    // config LCD screen
    Lcd1602::Config lcd_config;
    // pins
    lcd_config.rs = hw.GetPin(28);
    lcd_config.en = hw.GetPin(27);
    lcd_config.d4 = hw.GetPin(26);
    lcd_config.d5 = hw.GetPin(25);
    lcd_config.d6 = hw.GetPin(24);
    lcd_config.d7 = hw.GetPin(23);

    // initialize
    Lcd1602 lcd;
    lcd.Init(lcd_config);

    // print out
    lcd.SetCursor(0, 2);
    lcd.Print("TALK TO ME");
    lcd.SetCursor(1, 3);
    lcd.Print("--    --");
}


void ConfigureRegistry()
{   
    // open the file
    if(f_open(&dataFile, "registry.json", (FA_OPEN_EXISTING | FA_READ)) != FR_OK)
    {
        hw.PrintLine("failed to open file.");
        asm("bkpt 255");
    }

    // iterators
    unsigned long offset = 0;
    size_t bytesread = 0;

    // read file to the end
    while(!f_eof(&dataFile))
    {
        // read 512 bytes
        if(f_read(&dataFile, rawData + offset, 512, &bytesread)
           != FR_OK)
        {
            hw.PrintLine("failed to read file.");
            asm("bkpt 255");
        };

        // offset the pointer by amt of bytes actually read
        offset += bytesread;
    }

    // null terminate
    rawData[offset] = '\0';
    // close file
    f_close(&dataFile);

    // copy to static length string
    char temp[offset+1];
    strcpy(temp, rawData);
    free(rawData);
    // parse to JSON
    jsonData = cJSON_Parse(temp);
    const char* err = cJSON_GetErrorPtr();

    // error if it is null
    if(jsonData == NULL)
    {
        hw.PrintLine("jsonData returns null -- %s", err);
        asm("bkpt 255");
    }

    // retrieve information here
    // TODO: fill this in with actual info
    const cJSON *name = cJSON_GetObjectItemCaseSensitive(jsonData, "key");
    if(cJSON_IsString(name) && (name->valuestring != NULL))
    {
        hw.PrintLine("value: %s", name->valuestring);
    }
    else
    {
        hw.PrintLine("couldn't retrieve key value");
        asm("bkpt 255");
    }

    // read
}