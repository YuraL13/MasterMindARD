#include <Arduino.h>
#include "lcd_wrapper.h"
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C LCD(0x27, 16, 2);

void lcd_init(){
    LCD.init();
    LCD.backlight();
}

void lcd_clear(){
  LCD.clear();
}


void lcd_set_cursor(int y, int x){
  LCD.setCursor(x, y);
}


void lcd_print(char* text){
    //Serial.println(text);
    LCD.print(text);
}


void lcd_print_at(int y, int x, char* text){
    lcd_set_cursor(y, x);
    lcd_print(text);
}
