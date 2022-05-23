#include <LiquidCrystal_I2C.h>
#include "lcd_wrapper.h"
#include "mastermind.h"
//char* secret = generate_code(false, 4);

void setup() {
    Serial.begin(9600);
    pinMode(BTN_1_PIN, INPUT);
    pinMode(BTN_2_PIN, INPUT);
    pinMode(BTN_ENTER_PIN, INPUT);
    
    lcd_init();
}

void loop() {
    char* s = generate_code(false, 4);
    Serial.println(s);
    play_game(s);
    free(s);
}
