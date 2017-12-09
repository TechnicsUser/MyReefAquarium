


boolean lphIsOn = false;
boolean mphIsOn = false;
boolean sphIsOn = false;
boolean skimmerPower = false;
boolean heaterPower = false;
#define lph 3     // RELAY1                        
#define mph 2     // RELAY2                      
#define sph 4     // RELAY3   

#define skimmer  5 // RELAY4  
#define heater  6 // RELAY5

//Lights
#define light1  7 // RELAY6                        
#define light2  8 // RELAY7               
#define light3  9 // RELAY8
int xAxisTemp = 0;

//setup auto top up
void autoTopUp(int pumpTime) {


}


void flipSkimmer() {
  if (skimmerPower) {
    Serial.println("Alarm: - skimmer On");
    digitalWrite(skimmer, HIGH);   // turn the LED on (HIGH is the voltage level)
    skimmerPower = false;
  }
  else {
    Serial.println("Alarm: - skimmer off");
    digitalWrite(skimmer, LOW);   // turn the LED on (HIGH is the voltage level)
    skimmerPower = true;

  }
}
void flipHeater() {
  if (heaterPower) {
    Serial.println("Alarm: - heater On");
    digitalWrite(heater, HIGH);   // turn the LED on (HIGH is the voltage level)
    heaterPower = false;
  }
  else {
    Serial.println("Alarm: - heater off");
    digitalWrite(heater, LOW);   // turn the LED on (HIGH is the voltage level)
    heaterPower = true;

  }
}
void lphOn(){
      digitalWrite(lph, HIGH);   // turn the LED on (HIGH is the voltage level)
    lphIsOn = true;


}
void lphPulse() {
   if (!lphIsOn) {
    //   Serial.println("Alarm: - mph On");
    digitalWrite(lph, HIGH);   // turn the LED on (HIGH is the voltage level)
    lphIsOn = true;
  }
  else {
    //   Serial.println("Alarm: - mph off");
    digitalWrite(lph, LOW);   // turn the LED on (HIGH is the voltage level)
    lphIsOn = false;
    Alarm.timerOnce(300, lphOn); // 5 MINS
    digitalWrite(mph, HIGH);   // turn the LED on (HIGH is the voltage level)
    mphIsOn = true;
  }
 
 
}

void mphPulse() {
  if (!mphIsOn) {
    //   Serial.println("Alarm: - mph On");
    digitalWrite(mph, HIGH);   // turn the LED on (HIGH is the voltage level)
    mphIsOn = true;
  }
  else {
    //   Serial.println("Alarm: - mph off");
    digitalWrite(mph, LOW);   // turn the LED on (HIGH is the voltage level)
    mphIsOn = false;
    digitalWrite(lph, HIGH);   // turn the LED on (HIGH is the voltage level)
      lphIsOn = true;


  }
}
void MorningAlarmAcitic() {
  Serial.println("MorningAlarmAcitic");

  digitalWrite(light1, HIGH);   // turn the LED on (HIGH is the voltage level)
}
void MorningAlarm() {
  Serial.println("MorningAlarm ");
  digitalWrite(light2, HIGH);   // turn the LED on (HIGH is the voltage level)
}
void DayOnAlarm() {
  digitalWrite(light3, HIGH);   // turn the LED on (HIGH is the voltage level)
}
void DayOffAlarm() {
  digitalWrite(light3, LOW);   // turn the LED on (HIGH is the voltage level)
}
void EveningAlarm() {
  digitalWrite(light2, LOW);   // turn the LED on (HIGH is the voltage level)
}
void EveningAlarmAcitic() {
  digitalWrite(light1, LOW);   // turn the LED on (HIGH is the voltage level)
}
void updateDisplay() {
//  ringMeter(maxPh, minPh, minTemp, maxTemp, avgRoomTempMeasuredPH, avgTemp, 6, 9, xpos, ypos, radius, " Ph", GREEN2RED );
  Serial.println("ring meter");

}
void WeeklyAlarm() {
  Serial.println("Alarm: - its Monday Morning");
}

void ExplicitAlarm() {
  Serial.println("Alarm: - this triggers only at the given date and time");
}

void Repeats() {
  Serial.println("15 second timer");
}

void Repeats2() {
  Serial.println("2 second timer");
}

void OnceOnly() {
  Serial.println("This timer only triggers once, stop the 2 second timer");
  lphPulse();
}


void digitalClockDisplay(float t) {
  // digital clock display of the time
//  t = rtc.getTime();
  // Serial.print( t.hour + t.min + t.sec);
  //  printDigits( t.min);
  //  printDigits(t.sec);
  // Serial.println();
  //   Serial.println("Requesting temperatures...");
  // sensors.requestTemperatures(); // Send the command to get temperatures
  //  Serial.println("DONE");
  // String tempOut = "temp" , sensors.getTempCByIndex(0)
  //  Serial.print("Temperature for the device 1 (index 0) is: ");
  // Serial.println(tempOut);
 // String s =   dtostrf(sensors.getTempCByIndex(0), 6, 4, chrTemp);

  //   Serial.println("\n");

 Serial.println();
    Serial.print("*T");

  Serial.print( t);


   Serial.println();
    Serial.print("*A");

    Serial.print("X");
      Serial.print(xAxisTemp++);

    Serial.print("Y");
  Serial.print( t);
    Serial.print("*");

  // char outChar [12];
  // outChar = chrTemp + chrTempIn;
  //  Serial.println(chrTemp);

}

void printDigits(int digits) {
  Serial.print(":");
  if (digits < 10)
    Serial.print('0');
  Serial.print(digits);
}
