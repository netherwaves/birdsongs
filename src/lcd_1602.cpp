/*
  
Project: LCD library for Electrosmith's Daisy Seed
Description: LCD class for a 16x2 LCD using the Hitachi HD44780 driver chip
Author: Staffan Melin, staffan.melin@oscillator.se
License: The MIT License (MIT)
Version: 202101
Project site: http://www.oscillator.se/opensource
Based on: HD44780-Stm32HAL by Olivier Van den Eede (https://github.com/4ilo/HD44780-Stm32HAL)

*/


#include <stdio.h>
#include <string.h>

#include "daisy_core.h"
#include "sys/system.h"

#include "lcd_1602.h"


#define CLEAR_DISPLAY   0x01
#define RETURN_HOME     0x02

#define ENTRY_MODE_SET  0x04
#define OPT_S           0x01   // Shift entire display to right
#define OPT_INC         0x02 // Cursor increment

#define DISPLAY_ON_OFF_CONTROL 0x08
#define OPT_D 0x04 // Turn on display
#define OPT_C 0x02 // Turn on cursor
#define OPT_B 0x01 // Turn on cursor blink

#define LCD_CURSOR_SHIFT 0x10
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

#define FUNCTION_SET 0x20
#define OPT_DL 0x10 // Set interface data length
#define OPT_N 0x08  // Set number of display lines
#define OPT_F 0x04  // Set alternate font
#define SETCGRAM_ADDR 0x040
#define SET_DDRAM_ADDR 0x80 // Set DDRAM address

#define LCD_NIB 4
#define LCD_BYTE 8
#define LCD_DATA_REG 1
#define LCD_COMMAND_REG 0


namespace daisy
{
// Init LCD

void Lcd1602::Init(const Config& config)
{
    // init pins

    lcd_pin_rs.pin  = config.rs;
    lcd_pin_rs.mode = DSY_GPIO_MODE_OUTPUT_PP;
    lcd_pin_rs.pull = DSY_GPIO_NOPULL;
    dsy_gpio_init(&lcd_pin_rs);

    lcd_pin_en.pin  = config.en;
    lcd_pin_en.mode = DSY_GPIO_MODE_OUTPUT_PP;
    lcd_pin_en.pull = DSY_GPIO_NOPULL;
    dsy_gpio_init(&lcd_pin_en);

    lcd_data_pin[0].pin  = config.d4;
    lcd_data_pin[0].mode = DSY_GPIO_MODE_OUTPUT_PP;
    lcd_data_pin[0].pull = DSY_GPIO_NOPULL;
    dsy_gpio_init(&lcd_data_pin[0]);

    lcd_data_pin[1].pin  = config.d5;
    lcd_data_pin[1].mode = DSY_GPIO_MODE_OUTPUT_PP;
    lcd_data_pin[1].pull = DSY_GPIO_NOPULL;
    dsy_gpio_init(&lcd_data_pin[1]);

    lcd_data_pin[2].pin  = config.d6;
    lcd_data_pin[2].mode = DSY_GPIO_MODE_OUTPUT_PP;
    lcd_data_pin[2].pull = DSY_GPIO_NOPULL;
    dsy_gpio_init(&lcd_data_pin[2]);

    lcd_data_pin[3].pin  = config.d7;
    lcd_data_pin[3].mode = DSY_GPIO_MODE_OUTPUT_PP;
    lcd_data_pin[3].pull = DSY_GPIO_NOPULL;
    dsy_gpio_init(&lcd_data_pin[3]);

    cursor_on    = config.cursor_on;
    cursor_blink = config.cursor_blink;

    //??MASSIVE DELAY
    System::Delay(50);

    // init LCD
    
    // 1. reset RS and EN pins
    dsy_gpio_write(&lcd_pin_rs, 0);
    dsy_gpio_write(&lcd_pin_en, 0);
    //??RW is always grounded

    // 2. 4bit mode
    Set4BitMode();

    //??3. function set
    function_set = FUNCTION_SET | OPT_N;
    WriteCommand(function_set); // 4-bit mode, 2 lines

    // 3.5 sure, why not!
    System::Delay(1);

    // 4. display control
    display_command = DISPLAY_ON_OFF_CONTROL |
        (display ? OPT_D : 0) |
        (cursor_on ? OPT_C : 0) |
        (cursor_blink ? OPT_B : 0);

    WriteCommand(display_command);      // LCD on, cursor + blink settings
    
    //??5. clear display
    Clear();

    // 6. entry mode
    entry_mode_set = ENTRY_MODE_SET | OPT_INC;
    WriteCommand(entry_mode_set); // Increment cursor
}


// Print string on current pos

void Lcd1602::Print(const char* string)
{
    for(uint8_t i = 0; i < strlen(string); i++)
    {
        WriteData(string[i]);
    }
}


// Print number

void Lcd1602::PrintInt(int number)
{
    char buffer[11];
    sprintf(buffer, "%d", number);

    Print(buffer);
}


// Set cursor position

void Lcd1602::SetCursor(uint8_t row, uint8_t col)
{
    WriteCommand(SET_DDRAM_ADDR | ((row == 1 ? 0x40 : 0) + col));
}


// Clear screen

void Lcd1602::Clear()
{
    WriteCommand(CLEAR_DISPLAY);
    System::DelayUs(2000);
}

void Lcd1602::Home() {
    WriteCommand(RETURN_HOME);
    System::DelayUs(2000);
}


void Lcd1602::Display() {
    display = true;
    display_command |= OPT_D;
    WriteCommand(display_command);
}
void Lcd1602::NoDisplay() {
    display = false;
    display_command &= ~OPT_D;
    WriteCommand(display_command);
}


void Lcd1602::Blink() {
    cursor_blink = true;
    display_command |= OPT_B;
    WriteCommand(display_command);
}
void Lcd1602::NoBlink() {
    cursor_blink = false;
    display_command &= ~OPT_B;
    WriteCommand(display_command);
}


void Lcd1602::Cursor() {
    cursor_on = true;
    display_command |= OPT_C;
    WriteCommand(display_command);
}
void Lcd1602::NoCursor() {
    cursor_on = false;
    display_command &= ~OPT_C;
    WriteCommand(display_command);
}


void Lcd1602::ScrollDisplayLeft() {
    WriteCommand(LCD_CURSOR_SHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}
void Lcd1602::ScrollDisplayRight() {
    WriteCommand(LCD_CURSOR_SHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}



// Private methods

// Command sequence to set the screen in 4-bit mode (default)

void Lcd1602::Set4BitMode() {
    dsy_gpio_write(&lcd_pin_rs, LCD_COMMAND_REG);

    // instructions with appropriate timings...
    Write(0x03, LCD_NIB);
    System::DelayUs(4500);
    Write(0x03, LCD_NIB);
    System::DelayUs(4500);
    Write(0x03, LCD_NIB);
    System::DelayUs(150);
    Write(0x02, LCD_NIB);
}


// Write byte to command register

void Lcd1602::WriteCommand(uint8_t command)
{
    dsy_gpio_write(&lcd_pin_rs, LCD_COMMAND_REG);

    Write((command >> 4), LCD_NIB);
    Write(command & 0x0F, LCD_NIB);
}


// Write byte to data register

void Lcd1602::WriteData(uint8_t data)
{
    dsy_gpio_write(&lcd_pin_rs, LCD_DATA_REG);

    Write(data >> 4, LCD_NIB);
    Write(data & 0x0F, LCD_NIB);
}


// Write data

void Lcd1602::Write(uint8_t data, uint8_t len)
{
    for(uint8_t i = 0; i < len; i++)
    {
        dsy_gpio_write(&lcd_data_pin[i], (data >> i) & 0x01);
    }

    // toggle

    dsy_gpio_write(&lcd_pin_en, 1);
    System::Delay(1);
    dsy_gpio_write(&lcd_pin_en, 0);
    System::DelayUs(100);
}

} // namespace daisy
