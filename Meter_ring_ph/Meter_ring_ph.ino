/*

  upload new sketch via bluetooth

  setup wave control
  setup auto top up ... some done but has delay
  track ph over 24 hour
  track temp over 24 hour
  different menu's for temp, relays, wave control

*/
#include <TFT_HX8357.h>
//#include <SD.h>
#include <TimeLib.h>
#include <TimeAlarms.h>
#include <DS1307.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <stdlib.h>
//#include <NewPing.h>
//#include "phAndTemp.h"
#include "display.h"
#include "sdCard.h"
#include"alarmFunctions.h"
//
//#define TRIGGER_PIN  11  // Arduino pin tied to trigger pin on the ultrasonic sensor.
//#define ECHO_PIN     10  // Arduino pin tied to echo pin on the ultrasonic sensor.
//#define MAX_DISTANCE 40 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.
//#define RETURNPUMP 20

// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS A0
// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

//NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.

int returnPumpSpeed = 255;
char chrTemp [3];
char chrPh [5];

char chrTempIn [6];
boolean heaterOn = true;




DS1307 rtc(20, 21);// Init the DS1307
AlarmId aIdMorningLights;
Time  t;
Time  t2;
String incomingString = "abc";   // for incoming serial data


//+++++++++++++++ SOLID STATE RELAY'S++++++++++++++++++++
//Powerheads
//#define lph 3     // RELAY1
//#define mph 2     // RELAY2
//#define sph 4     // RELAY3
//
//#define skimmer  5 // RELAY4
//#define heater  6 // RELAY5
//
////Lights
//#define light1  7 // RELAY6
//#define light2  8 // RELAY7
//#define light3  9 // RELAY8

//+++++++++++++++ OLD SKOOL RELAY'S++++++++++++++++++++
// TODO

//++++++++++++++++++ PH +++++++++++++++++++++++++++++++++++




//++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//int relayPin = 13;
//int floatNumOne = 1;
//int topUpPump = 2;
//int largePowerHead = 3;
//int mediumPowerHead = 4;
//int smallPowerHead = 5;
//int largePowerHeadOnOffPerHour = 6;
//int pumpRunCount = 0;
//----------------------------------------------------------------------------------------------------------------------------
//++++++++++++++++++  RUN TIMES  +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//----------------------------------------------------------------------------------------------------------------------------
//int phTime[2];
//int phRunTime[] = {0, 1,  0};
//
//int lphOnTime[2] ; // large power head
//int lphOffTime[2] ; // large power head
//int lphRunTime[] = {0, 0, 5};
//boolean setMyOnTime = true;
//boolean setMyOffTime = true;
//int mphOnTime[2];   //medium power head
//int mphRunTime[] = {0, 10, 0};
//int sphOnTime[2];
//int sphRunTime[] = {0, 10, 0};
//----------------------------------------------------------------------------------------------------------------------------
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//----------------------------------------------------------------------------------------------------------------------------
// long unsigned int lastReadPhTime;





// SD chip select
#define SDC_CS 53






void setup(void) {

  Serial.begin(115200);

  //++++++++++++++++++++++ RELAYS ++++++++++++++++++++++++++++++++++++++++++++++

  pinMode(lph, OUTPUT);
  pinMode(mph, OUTPUT);
  pinMode(sph, OUTPUT);
  pinMode(heater, OUTPUT);
  pinMode(skimmer, OUTPUT);
  pinMode(light1, OUTPUT);
  pinMode(light2, OUTPUT);
  pinMode(light3, OUTPUT);
  //  pinMode(RETURNPUMP, OUTPUT);

  //    analogWrite(RETURNPUMP, returnPumpSpeed);

  digitalWrite(lph, HIGH);
  delay(500);
  digitalWrite(mph, HIGH);
  delay(500);
  digitalWrite(sph, HIGH);
  delay(500);
  digitalWrite(skimmer, HIGH);
  delay(500);
  digitalWrite(heater, HIGH);
  delay(500);
  digitalWrite(light1, HIGH);
  delay(500);
  digitalWrite(light2, HIGH);
  delay(500);
  digitalWrite(light3, HIGH);

  //+++++++++++++++++++++ DAILY RESET ++++++++++++++++++++++++++
  Alarm.alarmRepeat(1, 00, 00, DailyReset); // 1:00am every day

  //++++++++++++++++++++ CLOCK ++++++++++++++++++++++++++++++++++++++++++++++++
  // Initialize the rtc object
  rtc.begin();
  rtc.halt(false);

  t = rtc.getTime();
  setTime(t.hour, t.min, t.sec, t.mon, t.date, t.year); // set time
  // Alarm.timerOnce(10, OnceOnly);
  //   Alarm.timerOnce(20, OnceOnly);            // called once after 10 seconds
  //  Alarm.timerOnce(30, OnceOnly);            // called once after 10 seconds

  //++++++++++++++++++ LIGHTS +++++++++++++++++++++++++++++++++++++++++++++++++++++

  /* Super Actinic
     Aqua Pink
     Actinic +
     Reef Spec Pink
     Super Purple
     Pure Actinic */

  // actinic
  // create the alarms for lights to trigger at specific times
  Alarm.alarmRepeat(10, 00, 00, MorningAlarmAcitic); // 6:30am every day
  Alarm.alarmRepeat(11,  30, 00, MorningAlarm); // 12:30pm every day
  Alarm.alarmRepeat(12,  30, 00, DayOnAlarm); // 2:30pm every day

  Alarm.alarmRepeat(14, 00, 00, EveningAlarmAcitic); // 10:30pm every day
  Alarm.alarmRepeat(19, 00, 00, MorningAlarmAcitic); // 6:30am every day

  Alarm.alarmRepeat(20, 00, 00, DayOffAlarm); // 8:00 pm every day
  Alarm.alarmRepeat(21, 00, 00, EveningAlarm); // 9:45pm every day
  Alarm.alarmRepeat(22, 00, 00, EveningAlarmAcitic); // 10:30pm every day


  //+++++++++++++++++ POWERHEADS +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++



  Alarm.timerRepeat(11000, lphPulse);     //7200
  Alarm.timerRepeat(1200, mphPulse);
  //  Alarm.timerRepeat(50, sphPulse);

  //   Alarm.timerRepeat(15, lphPulse);

  //+++++++++++++++ RETURN PUMP ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  // Alarm.timerRepeat(1, checkSumpLevel);



  //+++++++++++++++AUTO TOP OFF ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

  //  Alarm.alarmRepeat(01, 00, 0, resetPumpCount); // 1:00am every day
  //
  //  Alarm.alarmRepeat(dowSaturday, 8, 30, 30, MorningAlarm); // 8:30:30 every Saturday
  //  Alarm.alarmRepeat(dowSunday, 8, 30, 30, MorningAlarm); // 8:30:30 every Sunday

  // +++++++++++++++++++ PH & TEMPRETURE +++++++++++++++++++++++++++++++++++++++++++++++++

  sensors.begin();
  Alarm.timerRepeat(0, 0, 30, readTemp);         // ph
  //  Alarm.timerRepeat(0, 5, 5, checkTopUp);         // Auto top-up
  //

  //++++++++++++++++++++ DISPLAY ++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  tft.begin();
//  tft.setRotation(1);
//  tft.fillScreen(HX8357_BLACK);
  //Alarm.timerRepeat(2, updateDisplay);           // Display


  // create timers, to trigger relative to when they're created
  // id = Alarm.timerRepeat(2, Repeats2);      // timer for every 2 seconds
  // Alarm.timerOnce(10, OnceOnly);            // called once after 10 seconds


  //++++++++++++++++++++++ SD CARD +++++++++++++++++++++++++++++++++++++++++++++++++++++
  //    if (!SD.begin(SDC_CS)) {
  //      Serial.println(F("failed!"));
  //      //return;
  //    }
  // Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
//  if (!SD.begin(SDC_CS)) {
//    Serial.println("Card failed, or not present");
//    // don't do anything more:
//    return;
//  }
//  Serial.println("card initialized.");
  //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
}

void DailyReset() {
  // Time  t;
  t = rtc.getTime();
  setTime(t.hour, t.min, t.sec, t.mon, t.date, t.year); // set time

}
void loop() {
  // send data only when you receive data:
  if (Serial.available() > 0) {
    // read the incoming byte:
    incomingString = Serial.readString();
    if (incomingString == "skimmerStatus") {
      Serial.print("I received: skimmerStatus ");
      Serial.print(skimmerPower);

    }
    if (incomingString == "heater") {
      Serial.print("h");

      Serial.print(heaterPower);
      flipHeater();
    }
    if (incomingString == "skimmer") {
      Serial.print("s");

      Serial.print(skimmerPower);
      flipSkimmer();
    }


  }


  //  if (incomingString.startsWith("light1")) {
  //    // use Alarm.free() to disable a timer and recycle its memory.
  //    Alarm.free(aIdMorningLights);
  //    // optional, but safest to "forget" the ID after memory recycled
  //    aIdMorningLights = dtINVALID_ALARM_ID;
  //    // you can also use Alarm.disable() to turn the timer off, but keep
  //    // it in memory, to turn back on later with Alarm.enable().
  //
  //    String strHour = incomingString.substring(6, 7);
  //    int intHour = strHour.toInt();
  //    String strMin = incomingString.substring(8, 9);
  //    int intMin = strMin.toInt();
  //    String strSec = incomingString.substring(10, 11);
  //    int intSec = strSec.toInt();
  //    aIdMorningLights =  Alarm.alarmRepeat(intHour, intMin, intSec, MorningAlarmAcitic); // 6:30am every day
  //    Serial.print(strHour + strMin + strSec);
  //
  //  }


 // digitalClockDisplay(sensors.getTempCByIndex(0));
  sensors.requestTemperatures(); // Send the command to get temperatures

  Serial.println(sensors.getTempCByIndex(0));

  Alarm.delay(1000); // wait one second between clock display
  //    }
}

void readTemp() {
    sensors.requestTemperatures(); // Send the command to get temperatures

  float tempC = sensors.getTempCByIndex(0);
  if (tempC <= 25.5) {
    if (heaterOn == false) {


      digitalWrite(heater, HIGH);   // turn the LED on (HIGH is the voltage level)
      Serial.println("");
      Serial.println("*LR252G0B0");
      Serial.print("*S");
      heaterOn = true;
    }

  }
  if (tempC >= 25.51) {
    if (heaterOn) {


      digitalWrite(heater, LOW);   // turn the LED on (HIGH is the voltage level)
      Serial.println("");
      Serial.println("*LR150G150B150");
      heaterOn = false;
    }

  }

}

/*

    DateTime now = rtc.now();
  startUp(now.hour(), now.minute(), now.second());
  if (now.hour() >= phTime[0] && now.minute() >= phTime[1]  && now.second() >= phTime[2] ) {
    readPh(now.hour(), now.minute(), now.second());
    Serial.print("ph test");

  }
  if (now.hour() >= lphOnTime[0] && now.minute() >= lphOnTime[1]  && now.second() >= lphOnTime[2] ) {
    digitalWrite(relayPin, HIGH);   // turn the LED on (HIGH is the voltage level)
  //  Serial.println("");

    Serial.print("high");
  //        Serial.print(lphOnTime[0]);
  //        Serial.print(":");
  //        Serial.print(lphOnTime[1]);
  //        Serial.print(":");
  //    Serial.print(lphOnTime[2]);
  //        Serial.println("");

    if (setMyOffTime) {
      lphOffTime[0] = lphOnTime[0] + lphRunTime[0];
      if (lphOffTime[0] > 24) {
        lphOffTime[0] = 0;

      }
      lphOffTime[1] = lphOnTime[1] + lphRunTime[1];
      if (lphOffTime[1] >= 59) {
        lphOffTime[1] %= 59;
        lphOffTime[0] += 1;
      }
      lphOffTime[2] =  lphOnTime[2] + lphRunTime[2];
      if (lphOffTime[2] >= 59) {
        lphOffTime[2] %= 59;
        lphOffTime[1] += 1;
      }
      Serial.println(" ");

      Serial.print ("lphOffTime");

      Serial.print(lphOffTime[0]);
      Serial.print(":");
      Serial.print(lphOffTime[1]);
      Serial.print(":");
      Serial.print(lphOffTime[2]);
            Serial.println(" ");

      setMyOnTime = true;
      setMyOffTime = false;
    }
  }
  if (now.hour() >= lphOffTime[0] && now.minute() >= lphOffTime[1]  && now.second() >= lphOffTime[2] ) {
    digitalWrite(relayPin, LOW);   // turn the LED on (HIGH is the voltage level)
    Serial.println("LOW ");
    //   Serial.print(lphOffTime[0]);
    //    Serial.print(",");
    //    Serial.print(lphOffTime[1]);
    //    Serial.print(",");
  //  Serial.print(lphOffTime[2]);
    if (setMyOnTime) {
      lphOnTime[0] = lphOffTime[0] + lphRunTime[0];
      if (lphOnTime[0] > 24 ) {
        lphOnTime[0]  = 0;
      }
      lphOnTime[1] = lphOffTime[1] + lphRunTime[1];
      if (lphOnTime[1] >= 59) {
        lphOnTime[1] %= 59;
        lphOnTime[0] += 1;
      }
      lphOnTime[2] = lphOffTime[2] + lphRunTime[2];
      if (lphOnTime[2] >= 59) {
        lphOnTime[2] %= 59;
        lphOnTime[1] += 1;
      }
      Serial.println(" ");

      Serial.print("lphOnTime");
      Serial.print(lphOnTime[0]);
      Serial.print(":");
      Serial.print(lphOnTime[1]);
      Serial.print(":");
      Serial.print(lphOnTime[2]);
      setMyOnTime = false;
      setMyOffTime = true;
    }



*/
