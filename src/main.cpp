#include <daisy_seed.h>
#include <daisysp.h>
#include <cJSON.h>

// namespaces
using namespace daisy;

// my includes
#include "Chant.h"
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
std::vector<Chant*> chants;

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
        PrepareSamplers();
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
    // if we don't do this, cJSON_Parse doesn't parse At All
    // probably because the memory isn't set in stone... hrmmmm,..,
    char temp[offset+1];
    strcpy(temp, rawData);

    // parse to JSON
    jsonData = cJSON_Parse(temp);
    const char* err = cJSON_GetErrorPtr();

    // error if it is null
    if(jsonData == NULL)
    {
        hw.PrintLine("jsonData returns null -- %s", err);
        cJSON_Delete(jsonData);
        asm("bkpt 255");
    }

    // delete buffers
    delete[] rawData;
    delete[] temp;

    // get chants array
    cJSON *chantsRaw = cJSON_GetObjectItemCaseSensitive(jsonData, "chants");
    if(!cJSON_IsArray(chantsRaw) && cJSON_GetArraySize(chantsRaw) <= 0)
    {
        hw.PrintLine("ERROR: chants is not an array");
        cJSON_Delete(chantsRaw);
        asm("bkpt 255");
    }
    
    // set properties to fill in here...
    cJSON *cJSON_Filename = NULL,
          *cJSON_Name = NULL,
          *cJSON_PitchStdev = NULL,
          *cJSON_PitchMean = NULL,
          *cJSON_AmpStdev = NULL,
          *cJSON_AmpMean = NULL,
          *cJSON_LengthMean = NULL;
    cJSON *it = NULL;

    // some cool variables
    uint8_t counter     = 0;
    uint8_t numChants   = cJSON_GetArraySize(chantsRaw);
    
    // loop through each chant
    cJSON_ArrayForEach(it, chantsRaw)
    {
        if(!cJSON_IsObject(it))
        {
            hw.PrintLine("ERROR: unexpected chant type. are you sure this is an object?");
            hw.PrintLine("--- at: %s", it->string);

            asm("bkpt 255");
        }

        // set props
        cJSON_Filename      = cJSON_GetObjectItem(it, "filename");
        cJSON_Name          = cJSON_GetObjectItem(it, "name");
        cJSON_PitchStdev    = cJSON_GetObjectItem(it, "pitch_stdev");
        cJSON_PitchMean     = cJSON_GetObjectItem(it, "pitch_mean");
        cJSON_AmpStdev      = cJSON_GetObjectItem(it, "amp_stdev");
        cJSON_AmpMean       = cJSON_GetObjectItem(it, "amp_mean");
        cJSON_LengthMean    = cJSON_GetObjectItem(it, "length_mean");

        // create chant
        Chant *c = new Chant();
        // set props
        c->filename     =        cJSON_Filename->valuestring;
        c->name         =        cJSON_Name->valuestring;
        c->pitch_stdev  = (float)cJSON_PitchStdev->valuedouble;
        c->pitch_mean   = (float)cJSON_PitchMean->valuedouble;
        c->amp_stdev    = (float)cJSON_AmpStdev->valuedouble;
        c->amp_mean     = (float)cJSON_AmpMean->valuedouble;
        c->length_mean  = (float)cJSON_LengthMean->valuedouble;

        // add to chants array
        chants.push_back(c);

        // delete cJSON objects
        cJSON_Delete(cJSON_Filename);
        cJSON_Delete(cJSON_Name);
        cJSON_Delete(cJSON_PitchStdev);
        cJSON_Delete(cJSON_PitchMean);
        cJSON_Delete(cJSON_AmpStdev);
        cJSON_Delete(cJSON_AmpMean);
        cJSON_Delete(cJSON_LengthMean);

        // successful!
        counter++;
        hw.PrintLine("parsed chant %ld/%ld...", counter, numChants);
    }

    // all done now! :3
    hw.PrintLine("~~~~~~~~");
    hw.PrintLine("done parsing all chants!");
    hw.PrintLine("%ld chants have been successfully initialized.", chants.size());

    // clear cJSON objects
    cJSON_Delete(chantsRaw);
    cJSON_Delete(jsonData);
}