#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <Arduino.h>
#include "mastermind.h"
#include "lcd_wrapper.h"

void num_button();
int switch_cursor(int x);
void num_button(int arr[4], const int x);
bool enter(const char* secret, const char* guess, int* a, int* b);
void again();

int BTN_1_PIN_state;

//cursor button
int BTN_2_PIN_state;

//Enter button
int BTN_ENTER_PIN_state;

unsigned long debounceDelay = 5;

//Algorithms
void append(char *s, char c){
    short unsigned len = strlen(s);
    s[len] = c;
    s[len + 1] = '\0';
}

bool check(int* arr, int a, int len){
    for(int i = 0; i < len; i++){
        if(arr[i] == a){
            return false;
        }
    }
    return true;
}

char* generate_code(bool repeat, int length){
    if(length <= 0) return NULL;

    char *s = calloc(length, sizeof(char));

    srand(232);
    if(repeat == true){
        for (int i = 0; i < length; i++){
            append(s, 48 + (random(10)));
        }
    }
    else{
        int arr[length];
        for (int i = 0; i < length; i++){
            int r = 48 + (random(10)); 
            if(check(arr, r, i)){
                append(s, r);
                arr[i] = r;
            }
            else{
                r = 48 + (random(10));
                while(!check(arr, r, i)){
                    r = 48 + (random(10));
                }
                append(s, r);
                arr[i] = r;
            }
        }
    }
    return s;
}

void get_score(const char* secret, const char* guess, int* peg_a, int* peg_b){
    short len = strlen(secret);
    int total_match = 0;
    int correct_pos = 0;

    for (int i = 0; i < len; i++)
    {
        for (int j = 0; j < len; j++)
        {
            if (secret[i] == guess[j])
            {
              if(i == j){
                correct_pos++;
                break;
              }
              else{
                total_match++;
                break;
              }
            }
            
        }
        /*if(secret[i] == guess[i]){
            correct_pos++;
            continue;
        }*/
    }
    //total_match = total_match - correct_pos;
    *peg_a = correct_pos;
    *peg_b = total_match;
}
//End of algorithmss

//Code for arduino
void render_greeteng(){
    lcd_print_at(0, 0, "Welcome to");
    lcd_print_at(1, 0, "MasterMind");

    delay(2000);
    lcd_clear();

    lcd_print_at(0, 0, "Your goal is");
    lcd_print_at(1, 0, "to guess a number");
    delay(2000);
    lcd_clear();

    lcd_print_at(0, 0, "Guessing a secret");
    lcd_print_at(1, 0, "number...");  

    delay(2000);    
    lcd_clear();
}

void turn_off_leds(){
  digitalWrite(LED_BLUE_1, LOW);
  digitalWrite(LED_RED_1 , LOW);
  digitalWrite(LED_BLUE_2, LOW);
  digitalWrite(LED_RED_2 , LOW);
  digitalWrite(LED_BLUE_3, LOW);
  digitalWrite(LED_RED_3 , LOW);
  digitalWrite(LED_BLUE_4, LOW);
  digitalWrite(LED_RED_4, LOW);
}

void turn_on_leds(){
  digitalWrite(LED_BLUE_1, HIGH);
  digitalWrite(LED_RED_1 , HIGH);
  digitalWrite(LED_BLUE_2, HIGH);
  digitalWrite(LED_RED_2 , HIGH);
  digitalWrite(LED_BLUE_3, HIGH);
  digitalWrite(LED_RED_3 , HIGH);
  digitalWrite(LED_BLUE_4, HIGH);
  digitalWrite(LED_RED_4, HIGH);
}

void render_victory(int g){
  lcd_clear();

  char str[16];
  sprintf(str, "in %d guesses", g);
  lcd_print_at(0, 0, "Good job!You win");
  lcd_print_at(1, 0, str);
  turn_off_leds();
  
  digitalWrite(LED_BLUE_1, HIGH);
  delay(200);
  digitalWrite(LED_RED_1 , HIGH);
  delay(200);
  digitalWrite(LED_BLUE_2, HIGH);
  delay(200);
  digitalWrite(LED_RED_2 , HIGH);
  delay(200);
  digitalWrite(LED_BLUE_3, HIGH);
  delay(200);
  digitalWrite(LED_RED_3 , HIGH);
  delay(200);
  digitalWrite(LED_BLUE_4, HIGH);
  delay(200);
  digitalWrite(LED_RED_4, HIGH);

  delay(250);
  digitalWrite(LED_BLUE_1, LOW);
  delay(200);
  digitalWrite(LED_RED_1 , LOW);
  delay(200);
  digitalWrite(LED_BLUE_2, LOW);
  delay(200);
  digitalWrite(LED_RED_2 , LOW);
  delay(200);
  digitalWrite(LED_BLUE_3, LOW);
  delay(200);
  digitalWrite(LED_RED_3 , LOW);
  delay(200);
  digitalWrite(LED_BLUE_4, LOW);
  delay(200);
  digitalWrite(LED_RED_4, LOW);

  turn_on_leds();
  delay(100);
  turn_off_leds();
  delay(100);
  turn_on_leds();
  delay(100);
  turn_off_leds();
  delay(100);
  turn_on_leds();
  delay(100);
  turn_off_leds();

  lcd_clear();
}

void render_leds(const int peg_a, const int peg_b){
  turn_off_leds();

  int a = peg_a;
  int b = peg_b;
  
  for(int i = 6; i < 15; i+=2){
    if(a > 0){
      digitalWrite(i, HIGH);
      a--;
    }
    else if(b > 0){
      digitalWrite(i+1, HIGH);
      b--;
    }
  }
}



void render_history(char* secret, char** history, const int entry_nr){
  int a = 0;
  int b = 0;
  
  get_score(secret, history[entry_nr], &a, &b);
  
  char str[16];
  if(entry_nr < 10){
    sprintf(str, "0%d %s %dA%dB", entry_nr, history[entry_nr], a, b);
  }
  else if(entry_nr >= 10){
    sprintf(str, "%d %s %dA%dB", entry_nr, history[entry_nr], a, b);
  }
  lcd_print_at(0, 0, str);

  render_leds(a, b);
  
}

char* arr_to_str(int arr[4]){
  char* s = calloc(4, sizeof(char));
  for(int i = 0; i < 4; i++){
    s[i] = arr[i] + '0';
  }
  s[4] = '\0';
  return s;
}

void failure(int secret){
  char str[16];
  sprintf(str, "secret was:%s", secret);
  
  lcd_clear();
  lcd_print_at(0, 0, "Looser");
  lcd_print_at(1, 0, str);
}

//Main function
void play_game(char* secret){
  lcd_clear();
  render_greeteng();
  
  int guess = 0;
  char** history = calloc(40, sizeof(char));
    
  int arr[4] = {0, 0, 0, 0};
  int x = 0; //Cursor

  int a = 0;//peg_a Numbres on correct position
  int b = 0;//Correct nums but on wrong positions
  
  char* s = arr_to_str(arr); // Enter (arr) converted to string
  
  lcd_clear();
  
  
  while(guess < 20){
    
    num_button(arr, x);//listen to button that increse number(++)
    x = switch_cursor(x);//listen to button that moves cursor
    s = arr_to_str(arr);//Converts entered array to string
    
    lcd_print_at(1, 0, "Your guess:");
    lcd_print_at(1, 11, s);

    //Readnig ENTER button
    //Tried to make as a function but attempt wasn't succesul =(
    BTN_ENTER_PIN_state = digitalRead(BTN_ENTER_PIN);

    get_score(secret, s, &a, &b);

    if (BTN_ENTER_PIN_state == HIGH) {
      if(a == 4){ //Check for victory
        render_victory(guess);
        delay(100);
        arr[0] = 0;
        arr[1] = 0;
        arr[2] = 0;
        arr[3] = 0;
        
        guess = 21;
      }
      else{
        history[guess] = s;
        render_history(secret, history, guess);
        delay(100);
        guess++;
      }
    }
    free(s);
  }
  if(guess == 20){ //Prints failure text if player ran out of guesses
      failure(secret);
      delay(2000);
  }
  lcd_clear();
  lcd_print_at(0, 0, "Thx u for playin");
  lcd_print_at(1, 0, "Yurii Levchenko");
  delay(3000);
  
  lcd_clear();
  lcd_print_at(0, 0, "Play again");
  delay(2000);
  
  lcd_clear();
  turn_off_leds();

  free(history);
}


//Buttons
//Used examples to make this
int switch_cursor(int x){
    int BTN_2_PIN_state = digitalRead(BTN_2_PIN);
    // only toggle the LED if the new button state is HIGH
    if (BTN_2_PIN_state == HIGH) {
      if (x < 3) {
        x++;
        delay(200);
      }
      else {
        x = 0;
        delay(200);
      }
    }
  return x;
}


void num_button(int arr[4], const int x){
    BTN_1_PIN_state = digitalRead(BTN_1_PIN);

    // only toggle the LED if the new button state is HIGH
    if (BTN_1_PIN_state == HIGH) {
      if (arr[x] < 9) {
        arr[x] += 1;
        delay(200);
      }
      else {
        arr[x] = 0;
        delay(200);
      }
    }
}
