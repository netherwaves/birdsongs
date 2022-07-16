#include <daisy_seed.h>
#include <daisysp.h>

// namespaces
using namespace daisy;


// my includes
#include "audio_player.h"
#include "pitch_detector.h"
#include "lcd_1602.h"


// hardware
DaisySeed hw;


// header defines
void ConfigureLCDScreen();


// audio callback
void AudioCallback(AudioHandle::InterleavingInputBuffer in,
                   AudioHandle::InterleavingOutputBuffer out,
                   size_t size)
{
    WavPlayer::StereoSample sampler_out;

    for (size_t i = 0; i < size; i += 2) {
        // get samples and iterate
        sampler_out = sampler.StreamStereo();

        out[i]      = sampler_out.l;
        out[i + 1]  = sampler_out.r;
    }
}


int main(void) {
    // initialize hardware
    hw.Configure();
    hw.Init();

    // logger
    hw.StartLog(true);
    hw.SetLed(1);

    System::Delay(1000);

    // LCD screen
    ConfigureLCDScreen();
    // audio sampler
    ConfigureAudioPlayer();
    // pitch detection
    ConfigurePitchDetection();

    hw.SetAudioBlockSize(48);
    hw.StartAudio(AudioCallback);

    hw.SetLed(0);

    for(;;) {
        LoopAudioPlayer();
    }
}


// LCD screen configuration
void ConfigureLCDScreen() {
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