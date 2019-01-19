/*Test driver program for Daktronics marquee */

#include "font5x7.h"

/* Driver pins */
#define LINE_PIN_MIN  2
#define LINE_PIN_MAX  8
#define PIN_LINE_1  8//2
#define PIN_LINE_2  2//3
#define PIN_LINE_3  3//4
#define PIN_LINE_4  4//5
#define PIN_LINE_5  5//6
#define PIN_LINE_6  6//7
#define PIN_LINE_7  7//8
#define PIN_SDA     9
#define PIN_SCK     10
#define PIN_SS      11
#define INFO_PIN    13

/* General constans */
#define SEGMENT_WIDTH       5
#define SEGMENTS_PER_MODULE 6
#define MODULES             3
#define LINE_LENGTH         (SEGMENT_WIDTH * SEGMENTS_PER_MODULE * MODULES)
#define LINE_HEIGHT         7
#define FRAMEBUFFER_SIZE    (LINE_LENGTH)// * LINE_HEIGHT)

#define CHAR_WIDTH          5
#define MAX_CHARS           15

#define CYCLE_TIME          5
#define MULTIPLEX_TIME      500

static unsigned char framebuffer[FRAMEBUFFER_SIZE];
int line = 0;
//unsigned long counter = 0;
int offset = 0;
int chars_written = 0;

static const uint8_t linePins[] = {
  PIN_LINE_1,
  PIN_LINE_2,
  PIN_LINE_3,
  PIN_LINE_4,
  PIN_LINE_5,
  PIN_LINE_6,
  PIN_LINE_7
};

void setup() {
  // put your setup code here, to run once:
  for(int i = LINE_PIN_MIN; i <= LINE_PIN_MAX; ++i){
    pinMode(i, OUTPUT);
    digitalWrite(i, HIGH);
  }
  for(int i = PIN_SDA; i <= INFO_PIN; ++i){
    pinMode(i, OUTPUT);
    digitalWrite(i, LOW);
  }

  //pinMode(INFO_PIN, OUTPUT);

  digitalWrite(PIN_LINE_1, LOW);
  digitalWrite(PIN_SS, HIGH);

  /*for(int i = 0; i < LINE_LENGTH;++i){
    digitalWrite(PIN_SDA, (i % 3) % 2);
    delay(CYCLE_TIME / 2);
    digitalWrite(PIN_SCK, HIGH);
    digitalWrite(INFO_PIN, HIGH);
    delay(CYCLE_TIME / 2);
    digitalWrite(PIN_SCK, LOW);
    digitalWrite(INFO_PIN, LOW);
  }*/

  memset(framebuffer, 0, sizeof(framebuffer));
  Serial.begin(9600);
}

void add_char(char data){
  Serial.print("add_char ");
  Serial.println(data);
  if(chars_written < MAX_CHARS && data >= 32 && data <= 127){
    //int x = chars_written * CHAR_WIDTH;
    for(int charPos = 0 ; charPos < CHAR_WIDTH; ++charPos){
      const uint8_t bitmap = Font5x7[((data - 32) * 5) + charPos];
      /*for(int y = 0; y < LINE_HEIGHT; ++y){
        framebuffer[y * LINE_LENGTH + x] = (bitmap & (0x80 << y)) != 0;
      }*/
      framebuffer[(chars_written * (CHAR_WIDTH + 1)) + charPos] = bitmap;
    }

    ++chars_written;
  }
}

void updatebuffer(){
  Serial.println("Received data");
  chars_written = 0;
  char data;
  do{
    data = Serial.read();
    if(data >= 32){
      add_char(data);
    }
  }
  while(data != 0x0A && data != 0x0D);
}

void write_line(int y){
  for(int x = 0; x < LINE_LENGTH; x++){
    //int y = line;
    //digitalWrite(PIN_SDA, framebuffer[y * LINE_LENGTH + x]);
    byte dot = ((framebuffer[x] & (0x40 >> y)));// >> (7 - y));
    digitalWrite(PIN_SDA, dot);
    delayMicroseconds(CYCLE_TIME);
    digitalWrite(PIN_SCK, HIGH);
    delayMicroseconds(CYCLE_TIME);
    digitalWrite(PIN_SCK, LOW);
  }
}

void loop() {
  if(Serial.available()){
    updatebuffer();
  }

  /*if(counter == 0){
    offset = (offset + 1) % 8;
  }*/
  //digitalWrite(INFO_PIN, HIGH);
  digitalWrite(linePins[line], LOW);
  digitalWrite(PIN_SS, LOW);
  write_line(line);
  //digitalWrite(INFO_PIN, LOW);
  //delayMicroseconds(CYCLE_TIME);
  digitalWrite(linePins[line], HIGH);
  digitalWrite(PIN_SS, HIGH);
  if(line == 6){
    line = 0;
  }
  else{
    ++line;
  }
  //counter = (counter + 1) % 2147483648;
}
