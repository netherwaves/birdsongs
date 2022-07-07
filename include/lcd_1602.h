#pragma once
#ifndef DSY_LCD_1602_H
#define DSY_LCD_1602_H

#include "daisy_core.h"
#include "per/gpio.h"


/**
   @brief Device Driver for 16x2 LCD panel. \n 
   HD44780 with 4 data lines. \n
   Example product: https://www.adafruit.com/product/181
   @author StaffanMelin
   @date March 2021
   @ingroup device
*/
namespace daisy {
  class Lcd1602
  {
    public:
      Lcd1602() {}
      ~Lcd1602() {}

      struct Config
      {
          bool         cursor_on = false;
          bool         cursor_blink = false;
          bool         display = true;
          dsy_gpio_pin rs, en, d4, d5, d6, d7;
      };

      /** 
      Initializes the LCD.
      * \param config is a struct that sets cursor on/off, cursor blink on/off and the dsy_gpio_pin's that connects to the LCD.
      */
      void Init(const Config &config);

      /** 
      Prints a string on the LCD.
      * \param string is a C-formatted string to print.
      */
      void Print(const char *string);

      /** 
      Prints an integer value on the LCD.
      * \param number is an integer to print.
      */
      void PrintInt(int number);

      /** 
      Moves the cursor of the LCD (the place to print the next value).
      * \param row is the row number (0 or 1).
      * \param col is the column number (0 to 15).
      */
      void SetCursor(uint8_t row, uint8_t col);

      /** 
      Clears the contents of the LCD.
      */
      void Clear();

      /** 
      Sets the cursor to its original position (column 0, row 0).
      */
      void Home();

      void Display();
      void NoDisplay();
      void Blink();
      void NoBlink();
      void Cursor();
      void NoCursor();
      void ScrollDisplayLeft();
      void ScrollDisplayRight();



    private:
      bool     cursor_on;
      bool     cursor_blink;
      bool     display;
      dsy_gpio lcd_pin_rs;
      dsy_gpio lcd_pin_en;
      dsy_gpio lcd_data_pin[4]; // D4-D7

      // command registers
      
      uint8_t display_command;
      uint8_t function_set;
      uint8_t entry_mode_set;

      // write functions

      void Set4BitMode();
      void WriteData(uint8_t);
      void WriteCommand(uint8_t);
      void Write(uint8_t, uint8_t);
  };
};


#endif
