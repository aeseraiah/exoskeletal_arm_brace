
void dispCalibInstructions(){
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
  
  
}
void dispCalib(unsigned int bicep, unsigned int tricep){
  display.clearDisplay();
  display.setTextSize(2);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner
  display.println("Bi:");
  display.println(bicep);
  display.println("Tri:");
  display.println(tricep);
  display.display();
  // delay(250);
}

void dispCalibFail(int sensor){
  display.clearDisplay();
  display.setTextSize(2);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner
  if (sensor == 0){
    display.println("Bicep");
    display.println("Sensor");
    display.println("Failed");
  }
  else if (sensor == 1){
    display.println("Tricep");
    display.println("Sensor");
    display.println("Failed");
  }
  else if (sensor == 2){
    display.println("Both");
    display.println("Sensor");
    display.println("Failed");
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
  display.clearDisplay();
  display.setTextSize(3);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner
  display.println("Begin");
  display.display();
}

void dispTrain(void){
  display.clearDisplay();
  display.setTextSize(2);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner
  display.println(F("Relax Arm"));
  display.println(F("Model Training in Progress"));
  display.display();
  // delay(2000);
}
// void dispEpoch(int i){
//   display.clearDisplay();
//   display.setTextSize(2);             // Normal 1:1 pixel scale
//   display.setTextColor(SSD1306_WHITE);        // Draw white text
//   display.setCursor(0,0);             // Start at top-left corner
//   display.println(F("Iterations"));
//   display.println(F("left: "));
//   display.println(i);
//   display.display();
// }

void dispAcc(float acc){
  display.clearDisplay();
  display.setTextSize(2);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner
  display.println(F("Accuracy:"));
  display.println(acc);
  display.display();
}
void confAccuracy(void){
  display.clearDisplay();
  display.setTextSize(2);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner
  // display.println(F("Model accuracy is above 85%. "));
  // display.println(F("Predictions will now be made on new data."));
  display.println(F("Model"));
  display.println(F("Accuracy"));
  display.println(F("Confirmed"));
  display.display();
  // delay(2000);
}

void failAccuracy(){
  display.clearDisplay();
  display.setTextSize(2);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner
  // display.println(F("Model is below 85% accuracy. "));
  // display.println(F("It should be retrained with new data."));
  display.println(F("Model"));
  display.println(F("Accuracy"));
  display.println(F("Failed"));
  display.display();
  // delay(2000);
}

void dispServo(int servo){
  if (servo == 1){
    display.clearDisplay();
    display.setTextSize(2);             // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE);        // Draw white text
    display.setCursor(0,0);             // Start at top-left corner
    display.println(F("servo flexed"));
    display.display();
  }
  else{
    display.clearDisplay();
    display.setTextSize(2);             // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE);        // Draw white text
    display.setCursor(0,0);             // Start at top-left corner
    display.println(F("servo extended"));
    display.display();
  }
}

void modelDisp(void){
  display.clearDisplay();
    display.setTextSize(2);             // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE);        // Draw white text
    display.setCursor(0,0);             // Start at top-left corner
    display.println(F("Training"));
    display.println(F("Model"));
    display.display();
}