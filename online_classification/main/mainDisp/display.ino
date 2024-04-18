

// #define SCREEN_WIDTH 128 // OLED display width, in pixels
// #define SCREEN_HEIGHT 64 // OLED display height, in pixels

// // Declaration for SSD1306 display connected using software SPI (default case):
// #define OLED_MOSI  9
// #define OLED_CLK   8
// #define OLED_DC    11
// #define OLED_CS    12
// #define OLED_RESET 10
// Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
//   OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

// #define LOGO_HEIGHT   16
// #define LOGO_WIDTH    16
// static const unsigned char PROGMEM logo_bmp[] =
// { 0b00000000, 0b11000000,
//   0b00000001, 0b11000000,
//   0b00000001, 0b11000000,
//   0b00000011, 0b11100000,
//   0b11110011, 0b11100000,
//   0b11111110, 0b11111000,
//   0b01111110, 0b11111111,
//   0b00110011, 0b10011111,
//   0b00011111, 0b11111100,
//   0b00001101, 0b01110000,
//   0b00011011, 0b10100000,
//   0b00111111, 0b11100000,
//   0b00111111, 0b11110000,
//   0b01111100, 0b11110000,
//   0b01110000, 0b01110000,
//   0b00000000, 0b00110000 };


void dispCalibInstructions(void){
  display.clearDisplay();
  display.setTextSize(2);             
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,0);      
  // delay(2000);
  display.println("Confirming sensor    contact: ");
  // display.println(F("Confirming good sensor contact."));
  // display.println(F("Keep your bicep and tricep as relaxed as possible."));
  // display.println(F("Values printed to the screen should be as close to 0 as possible"));
  // display.println(F("A message will be displayed in 10 seconds with more instructions."));
  // display.display();
  // delay(3000);
  // display.clearDisplay();
  // display.setTextSize(3);             
  // display.setTextColor(SSD1306_WHITE);        // Draw white text
  // display.setCursor(0,0);   
  // display.startscrollright(0x00, 0x0F);
  // display.println(F("Relax muscles. Values should be low."));
  display.display();
  delay(1000);
  
}
void dispCalib(unsigned int bicep, unsigned int tricep){
  display.clearDisplay();
  display.setTextSize(2);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner
  display.println(F("Bi:"));
  display.println(bicep);
  display.println(F("Tri:"));
  display.println(tricep);
  display.display();
  // delay(250);
}

void dispCalibFail(int sensor){
  display.clearDisplay();
  display.setTextSize(2);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner
  if (sensor = 0){
    display.println("Bicep Sensor Failed");
  }
  else if (sensor = 1){
    display.println("Tricep Sensor Failed");
  }
  else{
    display.println("Both Sensors Failed");
  }
  display.display();
  // delay(250);
}
void dispFlex(void){
  display.clearDisplay();
  display.setTextSize(2);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner
  display.println(F("Begin"));
  display.println(F("Flexion"));
  display.println(F("After"));
  display.println(F("Countdown"));
  display.display();
  delay(2000);
}

void dispExt(void){
  display.clearDisplay();
  display.setTextSize(2);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner
  display.println(F("Begin"));
  display.println(F("Extension"));
  display.println(F("After"));
  display.println(F("Countdown"));
  display.display();
  delay(2000);
}

void countDown(void){
  for (int i = 3; i > 0; i--){
    display.clearDisplay();
    display.setTextSize(5);             // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE);        // Draw white text
    display.setCursor(50,0);             // Start at top-left corner
    display.println(i);
    display.display();
    delay(1000);
  }
}

void dispTrain(void){
  display.clearDisplay();
  display.setTextSize(2);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner
  display.println(F("Relax Arm"));
  display.println(F("Model Training in Progress"));
  display.display();
  delay(2000);
}

void confAccuracy(void){
  display.clearDisplay();
  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner
  display.println(F("Model accuracy is above 85%. "));
  display.println(F("Predictions will now be made on new data."));
  display.display();
  delay(2000);
}

void failAccuracy(){
  display.clearDisplay();
  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner
  display.println(F("Model is below 85% accuracy. "));
  display.println(F("It should be retrained with new data."));
  display.display();
  delay(2000);
}