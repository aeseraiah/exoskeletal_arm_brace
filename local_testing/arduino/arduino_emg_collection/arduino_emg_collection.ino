int sensorPin = A0;   // select the input pin for the potentiometer
int digitalValue = 0;  // variable to store the value coming from the sensor
float analogVoltage = 0.00;

void setup() {
  Serial.begin(115200);
}

void loop() {
  digitalValue = analogRead(sensorPin);// read the value from the analog channel
  Serial.print("digital value = ");
  Serial.print(digitalValue);        //print digital value on serial monitor
  //convert digital value to analog voltage
  analogVoltage = (digitalValue * 3.3)/1023.00;
  // voltage = (adc_value / 1023.0) * 3.3
  Serial.print("  analog voltage = ");
  Serial.println(analogVoltage);
  // Serial.println(voltage);
  delay(1000);
}
