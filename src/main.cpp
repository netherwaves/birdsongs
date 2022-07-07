#include <daisy_seed.h>

// stuff
// #include "pitch_detector.h"
// #include "in_to_out.h"
#include "wav_reader.h"

#include "lcd_1602.h"

// #include "Lcd595.h"

// namespaces
using namespace daisy;

DaisySeed hw;


int main(void) {
    // initialize hardware
    hw.Configure();
    hw.Init();

    // config LCD screen
    Lcd1602::Config lcd_config;
    // cursor
    lcd_config.cursor_blink = false;
    lcd_config.cursor_on = false;
    // pins
    lcd_config.rs = hw.GetPin(28);
    lcd_config.en = hw.GetPin(27);
    lcd_config.d4 = hw.GetPin(26);
    lcd_config.d5 = hw.GetPin(25);
    lcd_config.d6 = hw.GetPin(24);
    lcd_config.d7 = hw.GetPin(23);

    Lcd1602 lcd;
    lcd.Init(lcd_config);

    lcd.SetCursor(0, 2);
    lcd.Print("TALK TO ME");

    lcd.SetCursor(1, 3);
    lcd.Print("--    --");

    //setup_sampler(hw);
}