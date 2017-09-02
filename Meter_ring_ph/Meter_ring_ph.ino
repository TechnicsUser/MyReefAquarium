/*

  upload new sketch via bluetooth

  setup wave control
  setup auto top up ... some done but has delay
  track ph over 24 hour
  track temp over 24 hour
  different menu's for temp, relays, wave control

*/

#include <TFT_HX8357.h>
#include <SD.h>
#include <TimeLib.h>
#include <TimeAlarms.h>
#include <DS1307.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <stdlib.h>
#include <NewPing.h>

#define TRIGGER_PIN  11  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     10  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 40 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.
#define RETURNPUMP 20


NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.

int returnPumpSpeed = 255;
char chrTemp [3];
char chrPh [5];

char chrTempIn [6];

// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS A0
// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);


DS1307 rtc(20, 21);// Init the DS1307
AlarmId aIdMorningLights;
Time  t;
Time  t2;
String incomingString = "abc";   // for incoming serial data

#define RED2RED 0
#define GREEN2GREEN 1
#define BLUE2BLUE 2
#define BLUE2RED 3
#define GREEN2RED 4
#define RED2GREEN 5
//+++++++++++++++ SOLID STATE RELAY'S++++++++++++++++++++
//Powerheads
#define lph 2     // RELAY1                        
#define mph 3     // RELAY2                      
#define sph 4     // RELAY3   

#define skimmer  5 // RELAY4  
#define heater  6 // RELAY5

//Lights
#define light1  7 // RELAY6                        
#define light2  8 // RELAY7               
#define light3  9 // RELAY8

//+++++++++++++++ OLD SKOOL RELAY'S++++++++++++++++++++
// TODO

//++++++++++++++++++ PH +++++++++++++++++++++++++++++++++++

float volt4 = 3.2300; 
float volt7 = 2.6924; //2.7150
float calibrationTempC = 21.4;
int phPin = A1;



//++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int relayPin = 13;
int floatNumOne = 1;
int topUpPump = 2;
int largePowerHead = 3;
int mediumPowerHead = 4;
int smallPowerHead = 5;
int largePowerHeadOnOffPerHour = 6;
int pumpRunCount = 0;
//----------------------------------------------------------------------------------------------------------------------------
//++++++++++++++++++  RUN TIMES  +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//----------------------------------------------------------------------------------------------------------------------------
int phTime[2];
int phRunTime[] = {0, 1,  0};

int lphOnTime[2] ; // large power head
int lphOffTime[2] ; // large power head
int lphRunTime[] = {0, 0, 5};
boolean setMyOnTime = true;
boolean setMyOffTime = true;
int mphOnTime[2];   //medium power head
int mphRunTime[] = {0, 10, 0};
int sphOnTime[2];
int sphRunTime[] = {0, 10, 0};
//----------------------------------------------------------------------------------------------------------------------------
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//----------------------------------------------------------------------------------------------------------------------------
// long unsigned int lastReadPhTime;





TFT_HX8357 tft = TFT_HX8357();       // Invoke custom library

#define HX8357_GREY 0x2104 // Dark grey 16 bit colour
// SD chip select
#define SDC_CS 53
uint32_t runTime = -99999;       // time for next update

int reading = 0; // Value to be displayed
int d = 0; // Variable used for the sinewave test waveform
boolean alert = 0;
boolean startup = true;
int8_t ramp = 1;
float maxPh = 0;
float minPh = 0;
float minTemp = 0;
float maxTemp = 0;
int x;
int sampleSize = 5;

float avgMeasuredPH = 0;
float avgRoomTempMeasuredPH = 0;
float avgTemp = 0;
float avgPHVolts = 0;
float avgVoltsPerPH = 0;
float phTemp = 0;
String dataString = "";
int xpos = 0, ypos = 5, gap = 4, radius = 52;



boolean lphIsOn = true;
boolean mphIsOn = true;
boolean sphIsOn = true;
boolean skimmerPower = true;
boolean heaterPower = true;


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
    pinMode(RETURNPUMP, OUTPUT);

      analogWrite(RETURNPUMP, returnPumpSpeed);   // turn the LED on (HIGH is the voltage level)

      digitalWrite(lph, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(500);                       // wait for a second
     digitalWrite(mph, HIGH);   // turn the LED on (HIGH is the voltage level)
       delay(500);                       // wait for a second
      digitalWrite(sph, HIGH);   // turn the LED on (HIGH is the voltage level)
     delay(500);   
  digitalWrite(skimmer, HIGH);   // turn the LED on (HIGH is the voltage level)
      delay(500);   
  digitalWrite(heater, HIGH);   // turn the LED on (HIGH is the voltage level)
         delay(500);   
    digitalWrite(light1, HIGH);   // turn the LED on (HIGH is the voltage level)
         delay(500);   
      digitalWrite(light2, HIGH);   // turn the LED on (HIGH is the voltage level)
         delay(500);   
     digitalWrite(light3, HIGH);   // turn the LED on (HIGH is the voltage level)

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
 * Aqua Pink
 * Actinic +
 * Reef Spec Pink
 * Super Purple
 * Pure Actinic */


  // create the alarms for lights to trigger at specific times
   Alarm.alarmRepeat(10, 00, 00, MorningAlarmAcitic); // 6:30am every day
  Alarm.alarmRepeat(10,  30,00, MorningAlarm); // 12:30pm every day
  Alarm.alarmRepeat(11,  30, 00, DayOnAlarm); // 2:30pm every day
  Alarm.alarmRepeat(20, 00, 00, DayOffAlarm); // 8:00 pm every day
  Alarm.alarmRepeat(21, 00,00, EveningAlarm); // 9:45pm every day
  Alarm.alarmRepeat(22,00, 00, EveningAlarmAcitic); // 10:30pm every day


  //+++++++++++++++++ POWERHEADS +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++



  Alarm.timerRepeat(7200, lphPulse);         
  Alarm.timerRepeat(1200, mphPulse);         
//  Alarm.timerRepeat(50, sphPulse);          

  //   Alarm.timerRepeat(15, lphPulse);

  //+++++++++++++++ RETURN PUMP ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  Alarm.timerRepeat(1, checkSumpLevel);         



  //+++++++++++++++AUTO TOP OFF ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

  //  Alarm.alarmRepeat(01, 00, 0, resetPumpCount); // 1:00am every day
  //
  //  Alarm.alarmRepeat(dowSaturday, 8, 30, 30, MorningAlarm); // 8:30:30 every Saturday
  //  Alarm.alarmRepeat(dowSunday, 8, 30, 30, MorningAlarm); // 8:30:30 every Sunday

  // +++++++++++++++++++ PH & TEMPRETURE +++++++++++++++++++++++++++++++++++++++++++++++++

  sensors.begin();
// Alarm.timerRepeat(0, 0, 30, readPh);         // ph
  //  Alarm.timerRepeat(0, 5, 5, checkTopUp);         // Auto top-up
  //

  //++++++++++++++++++++ DISPLAY ++++++++++++++++++++++++++++++++++++++++++++++++++++++
  tft.begin();
  tft.setRotation(1);
  tft.fillScreen(HX8357_BLACK);

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
  if (!SD.begin(SDC_CS)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  Serial.println("card initialized.");
  //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
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
    //                // say what you got:
    //                Serial.print("I received: ");
    //                Serial.println(incomingString);
    //                String Slph = "lph";

  }
  if (incomingString.startsWith("sph")) {
    String strValue = incomingString.substring(3, 5);
    int value = strValue.toInt();

    //   Serial.print("strValue");
    sphSpeed(value);
    //  Serial.print("sphSpeed");

  }
  if (incomingString.startsWith("light1")) {
    // use Alarm.free() to disable a timer and recycle its memory.
    Alarm.free(aIdMorningLights);
    // optional, but safest to "forget" the ID after memory recycled
    aIdMorningLights = dtINVALID_ALARM_ID;
    // you can also use Alarm.disable() to turn the timer off, but keep
    // it in memory, to turn back on later with Alarm.enable().

    String strHour = incomingString.substring(6, 7);
    int intHour = strHour.toInt();
    String strMin = incomingString.substring(8, 9);
    int intMin = strMin.toInt();
    String strSec = incomingString.substring(10, 11);
    int intSec = strSec.toInt();
    aIdMorningLights =  Alarm.alarmRepeat(intHour, intMin, intSec, MorningAlarmAcitic); // 6:30am every day
    Serial.print(strHour + strMin + strSec);

  }


  digitalClockDisplay();


  //    if (millis() - runTime >= 0L) { // Execute every 2s
  //     runTime = millis();
  xpos = 480 / 2 - 160, ypos = 0, gap = 15, radius = 170;
  //      ringMeter(maxPh, minPh, minTemp, maxTemp, avgRoomTempMeasuredPH, avgTemp, 6, 9, xpos, ypos, radius, " Ph", GREEN2RED ); // Draw analogue meter ,maxPh,minPh,minTemp, maxTemp
  Alarm.delay(1000); // wait one second between clock display
  //    }
}
void readPh() {
  //  Serial.print(" measuredPH-11");

  float tempC = sensors.getTempCByIndex(0);
  float tempAdjusted4 = getTempAdjusted4();
  for (x = 0; x < sampleSize; x++)
  {

    float measuredPH = measurePH();
    float phTemp = measureTempC();
    float roomTempPH = doPHTempCompensation(measuredPH, phTemp);

    float phVolt = measurePHVolts();

    avgMeasuredPH += measuredPH;
    avgRoomTempMeasuredPH += roomTempPH;
    avgTemp += phTemp;
    avgPHVolts += phVolt;
  }

  avgMeasuredPH /= sampleSize;
  avgRoomTempMeasuredPH /= sampleSize;
  avgTemp /= sampleSize;
  avgPHVolts /= sampleSize;
  if (maxPh < avgMeasuredPH) {
    maxPh = avgMeasuredPH;
  }
  if (minPh > avgMeasuredPH ) {
    minPh = avgMeasuredPH;
  }
  if (maxTemp < avgTemp) {
    maxTemp = avgTemp;
  }
  if (minTemp > avgTemp) {
    minTemp = avgTemp;
  }
  //  phTime[0] = h + phRunTime[0];
  //  phTime[1] = m + phRunTime[1];
  //  phTime[2] = s + phRunTime[2];
  //  lastReadPhTime = rtc.getTime();

  //  Serial.print(" measuredPH = ");
  dtostrf(avgMeasuredPH, 6, 4, chrPh);// This is where I get confused, I read that the 6 is the length of the number
  String s = chrPh;
  Serial.print("ph" + s);
  //    Serial.print(" roomTempMeasuredPH-");
  //    Serial.print(avgRoomTempMeasuredPH,4);
  Serial.print(avgMeasuredPH);
  //    Serial.print(avgTemp,4);
  //    Serial.print(" phVolts-");
  //    Serial.print(avgPHVolts,4);
  //    Serial.print(" 7CalVolts-");
  //    Serial.print(volt7,4);
  //    Serial.print(" 4CalVolts-");
  //    Serial.print(volt4,4);
  //    Serial.print(" 4CalTempAdjusted-");
  //    Serial.println(tempAdjusted4,4);
  dataString = avgMeasuredPH;
  // dataString += t.hour;
  // writeToSd();
}
float measurePHVolts()
{
  float phADC = analogRead(phPin);
  float phVolts = (phADC / 1024.0) * 5.0;
  return phVolts;
}

float getTempAdjusted4()
{
  //http://www.omega.com/Green/pdf/pHbasics_REF.pdf
  float adjustTemp = calibrationTempC;
  float difference = adjustTemp - 25;
  float phAdjust = (0.009 * difference);
  float tempAdjusted4 = 4 + phAdjust;
  return tempAdjusted4;
}


float measurePH()
{

  float phVolt = measurePHVolts();

  float tempAdjusted4 = getTempAdjusted4();
  float voltsPerPH = (abs(volt7 - volt4)) / (7 - tempAdjusted4);

  //acid ph's produce positive voltages, basic ph's produce negative voltages
  //that one site was wrong

  float realPHVolt = (volt7 - phVolt);
  float phUnits = realPHVolt / voltsPerPH;
  float measuredPH = 7 + phUnits;

  return measuredPH;
}

float doPHTempCompensation(float PH, float temp)
{
  float difference = temp - 25;
  float phAdjust = (0.009 * difference);
  float tempAdjustedPH = PH + phAdjust;
  return tempAdjustedPH;
}


float measureTempC()
{
  sensors.requestTemperatures(); // Send the command to get temperatures
  //  Serial.println("DONE");
  //  Serial.print("Temperature for the device 1 (index 0) is: ");
  // Serial.println(sensors.getTempCByIndex(0));

  //  float tempADC = analogRead(tempPin);
  //  float tempVolts = (tempADC / 1024) * 5.0;
  //  float tempC = (tempVolts / 0.010);
  //  return tempC;
  return 99.99;
}


char * floatToString(char * outstr, float value, int places, int minwidth = 0, bool rightjustify = false) {
  // this is used to write a float value to string, outstr.  oustr is also the return value.
  int digit;
  float tens = 0.1;
  int tenscount = 0;
  int i;
  float tempfloat = value;
  int c = 0;
  int charcount = 1;
  int extra = 0;
  // make sure we round properly. this could use pow from <math.h>, but doesn't seem worth the import
  // if this rounding step isn't here, the value  54.321 prints as 54.3209

  // calculate rounding term d:   0.5/pow(10,places)
  float d = 0.5;
  if (value < 0)
    d *= -1.0;
  // divide by ten for each decimal place
  for (i = 0; i < places; i++)
    d /= 10.0;
  // this small addition, combined with truncation will round our values properly
  tempfloat +=  d;

  // first get value tens to be the large power of ten less than value
  if (value < 0)
    tempfloat *= -1.0;
  while ((tens * 10.0) <= tempfloat) {
    tens *= 10.0;
    tenscount += 1;
  }

  if (tenscount > 0)
    charcount += tenscount;
  else
    charcount += 1;

  if (value < 0)
    charcount += 1;
  charcount += 1 + places;

  minwidth += 1; // both count the null final character
  if (minwidth > charcount) {
    extra = minwidth - charcount;
    charcount = minwidth;
  }

  if (extra > 0 and rightjustify) {
    for (int i = 0; i < extra; i++) {
      outstr[c++] = ' ';
    }
  }

  // write out the negative if needed
  if (value < 0)
    outstr[c++] = '-';

  if (tenscount == 0)
    outstr[c++] = '0';

  for (i = 0; i < tenscount; i++) {
    digit = (int) (tempfloat / tens);
    itoa(digit, &outstr[c++], 10);
    tempfloat = tempfloat - ((float)digit * tens);
    tens /= 10.0;
  }

  // if no places after decimal, stop now and return

  // otherwise, write the point and continue on
  if (places > 0)
    outstr[c++] = '.';


  // now write out each decimal place by shifting digits one by one into the ones place and writing the truncated value
  for (i = 0; i < places; i++) {
    tempfloat *= 10.0;
    digit = (int) tempfloat;
    itoa(digit, &outstr[c++], 10);
    // once written, subtract off that digit
    tempfloat = tempfloat - (float) digit;
  }
  if (extra > 0 and not rightjustify) {
    for (int i = 0; i < extra; i++) {
      outstr[c++] = ' ';
    }
  }


  outstr[c++] = '\0';
  return outstr;
}


// #########################################################################
//  Draw the meter on the screen, returns x coord of righthand side
// #########################################################################
int ringMeter(float maxPh,  float minPh, float minTemp, float maxTemp , float value, float Temp, int vmin, int vmax, int x, int y, int r, char *units, byte scheme ) // ,float maxPh,  float minPh,float minTemp,float maxTemp
{


  // Minimum value of r is about 52 before value text intrudes on ring
  // drawing the text first is an option

  x += r; y += r;   // Calculate coords of centre of ring

  int w = r / 3;    // Width of outer ring is 1/4 of radius

  int angle = 150;  // Half the sweep angle of meter (300 degrees)

  int v = map(value, vmin, vmax, -angle, angle); // Map the value to an angle v

  byte seg = 3; // Segments are 3 degrees wide = 100 segments for 300 degrees
  byte inc = 6; // Draw segments every 3 degrees, increase to 6 for segmented ring

  // Variable to save "value" text colour from scheme and set default
  int colour = HX8357_BLUE;

  // Draw colour blocks every inc degrees
  for (int i = -angle + inc / 2; i < angle - inc / 2; i += inc) {
    // Calculate pair of coordinates for segment start
    float sx = cos((i - 90) * 0.0174532925);
    float sy = sin((i - 90) * 0.0174532925);
    uint16_t x0 = sx * (r - w) + x;
    uint16_t y0 = sy * (r - w) + y;
    uint16_t x1 = sx * r + x;
    uint16_t y1 = sy * r + y;

    // Calculate pair of coordinates for segment end
    float sx2 = cos((i + seg - 90) * 0.0174532925);
    float sy2 = sin((i + seg - 90) * 0.0174532925);
    int x2 = sx2 * (r - w) + x;
    int y2 = sy2 * (r - w) + y;
    int x3 = sx2 * r + x;
    int y3 = sy2 * r + y;

    if (i < v) { // Fill in coloured segments with 2 triangles
      switch (scheme) {
        case 0: colour = HX8357_RED; break; // Fixed colour
        case 1: colour = HX8357_GREEN; break; // Fixed colour
        case 2: colour = HX8357_BLUE; break; // Fixed colour
        case 3: colour = rainbow(map(i, -angle, angle, 0, 127)); break; // Full spectrum blue to red
        case 4: colour = rainbow(map(i, -angle, angle, 70, 127)); break; // Green to red (high temperature etc)
        case 5: colour = rainbow(map(i, -angle, angle, 127, 63)); break; // Red to green (low battery etc)
        default: colour = HX8357_BLUE; break; // Fixed colour
      }
      tft.fillTriangle(x0, y0, x1, y1, x2, y2, colour);
      tft.fillTriangle(x1, y1, x2, y2, x3, y3, colour);
      //text_colour = colour; // Save the last colour drawn
    }
    else // Fill in blank segments
    {
      tft.fillTriangle(x0, y0, x1, y1, x2, y2, HX8357_GREY);
      tft.fillTriangle(x1, y1, x2, y2, x3, y3, HX8357_GREY);
    }
  }
  // Convert value to a string
  char buf[10];
  byte len = 6; if (value > 999) len = 9;
  //  dtostrf(value, len, 0, buf);
  //  buf[len] = ' '; buf[len+1] = 0; // Add blanking space and terminator, helps to centre text too!
  // Set the text colour to default
  tft.setTextSize(1);

  if (value < vmin || value > vmax) {
    drawAlert(x, y + 90, 50, 1);
  }
  else {
    drawAlert(x, y + 90, 50, 0);
  }

  tft.setTextColor(HX8357_WHITE, HX8357_BLACK);
  // Uncomment next line to set the text colour to the last segment value!
  tft.setTextColor(colour, HX8357_BLACK);
  tft.setTextDatum(MC_DATUM);
  // Print value, if the meter is large then use big font 8, othewise use 4
  //  if (r > 84) {
  tft.setTextPadding(45 * 3); // Allow for 3 digits each 55 pixels wide
  // 	tft.drawFloat(maxPh,1,x +130, y - 45, 6); // Value in middle
  //	tft.drawFloat(minPh,1,x - 130, y - 45, 6); // Value in middle

  tft.drawFloat(value, 3, x, y - 45, 6); // Value in middle

  //    tft.drawFloat(maxTemp,1,x + 130, y + 65 , 6); // Value in middle
  // tft.drawFloat(minTemp,1,x - 190, y + 65 , 6); // Value in middle

  tft.drawFloat(Temp, 1, x, y + 65 , 6); // Value in middle

  //    }
  //  else {
  //    tft.setTextPadding(3 * 14); // Allow for 3 digits each 14 pixels wide
  //    tft.drawFloat(value,3,x, y - 45, 6); // Value in middle
  //        tft.drawFloat(Temp,1,x, y + 65 , 6); // Value in middle
  //
  //
  //  }
  tft.setTextSize(1);
  tft.setTextPadding(0);
  // Print units, if the meter is large then use big font 4, othewise use 2
  tft.setTextColor(HX8357_WHITE, HX8357_BLACK);
  // if (r > 84)     tft.drawFloat(value,8,x, y, 2); // Value in middle
  //tft.drawString(units, x, y + 60, 4); // Units display
  //  else     tft.drawFloat(value, x, y, 4,3); // Value in middle
  tft.drawString(units, x,  75, 4); // Units display
  tft.drawString("Temp", x, y + 20, 4); // Units display

  // Calculate and return right hand side x coordinate
  return x + r;
}

void drawAlert(int x, int y , int side, boolean draw)
{
  if (draw && !alert) {
    tft.fillTriangle(x, y, x + 30, y + 47, x - 30, y + 47, rainbow(95));
    tft.setTextColor(HX8357_BLACK);
    tft.drawCentreString("!", x, y + 6, 4);
    alert = 1;
  }
  else if (!draw) {
    tft.fillTriangle(x, y, x + 30, y + 47, x - 30, y + 47, HX8357_BLACK);
    alert = 0;
  }
}

// #########################################################################
// Return a 16 bit rainbow colour
// #########################################################################
unsigned int rainbow(byte value)
{
  // Value is expected to be in range 0-127
  // The value is converted to a spectrum colour from 0 = blue through to 127 = red

  byte red = 0; // Red is the top 5 bits of a 16 bit colour value
  byte green = 0;// Green is the middle 6 bits
  byte blue = 0; // Blue is the bottom 5 bits

  byte quadrant = value / 32;

  if (quadrant == 0) {
    blue = 31;
    green = 2 * (value % 32);
    red = 0;
  }
  if (quadrant == 1) {
    blue = 31 - (value % 32);
    green = 63;
    red = 0;
  }
  if (quadrant == 2) {
    blue = 0;
    green = 63;
    red = value % 32;
  }
  if (quadrant == 3) {
    blue = 0;
    green = 63 - 2 * (value % 32);
    red = 31;
  }
  return (red << 11) + (green << 5) + blue;
}

// #########################################################################
// Return a value in range -1 to +1 for a given phase angle in degrees
// #########################################################################
float sineWave(int phase) {
  return sin(phase * 0.0174532925);
}
//setup auto top up
void autoTopUp(int pumpTime) {


}

void runWaves(int h, int m, int s) {
  // run 3 puwer heads alternatley over 24 hour period, calmer at night
  //largePowerHead, mediumPowerHead, smallPowerHead
  //largePowerHeadOnOffPerHour
  //  phTime[0] = now.hour();


  if ( startup = true) {
    lphOnTime[0] = h;
    lphOnTime[1] = m;
    lphOnTime[2] = s;

    startup = false;

  }
  if (h >= lphOnTime[0]  && m >= lphOnTime[1] && s >= lphOnTime[2]) {
    //  largePowerHead on
    digitalWrite(relayPin, HIGH);   // turn the LED on (HIGH is the voltage level)
    if (setMyOnTime) {
      lphOffTime[0] =  lphOnTime[0] + lphRunTime[0];
      lphOffTime[1] = lphOnTime[1] + lphRunTime[1];
      lphOffTime[2] = lphOnTime[2] + lphRunTime[2];
      // Serial.println("lphOffTime" + lphOffTime);
      setMyOnTime = false;
      setMyOffTime = true;
    }

  }
  if (h >= lphOffTime[0]  && m >= lphOffTime[1] && s >= lphOnTime[2]) {
    //  largePowerHead off
    digitalWrite(relayPin, LOW);   // turn the LED on (HIGH is the voltage level)
    if (setMyOffTime) {
      lphOnTime[0] = lphOffTime[0] + lphRunTime[0] ;
      lphOnTime[1] = lphOffTime[1] + lphRunTime[1] ;
      lphOnTime[2] = lphOffTime[2] + lphRunTime[2] ;

      setMyOnTime = true;
      setMyOffTime = false;
    }
  }

}
void writeToSd( ) {
  File dataFile = SD.open("PHLOG.txt", FILE_WRITE);
  dataString += " | ";
  dataString += avgTemp;
  dataString += " | ";


  dataString += hour();
  dataString += ":";

  dataString += minute();
  dataString += " | ";
  dataString += t.date;
  dataString += "/";

  dataString += t.mon;
  dataString += "/";

  dataString += t.year;
  dataString += " | ";
  dataString += pumpRunCount;
  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    Serial.println("card write.");

    dataFile.close();
    // print to the serial port too:
    Serial.println(dataString);
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
    dataFile.close();
  }
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
void lphPulse() {
 
    digitalWrite(lph, HIGH);   // turn the LED on (HIGH is the voltage level)
    Alarm.timerOnce(300, lphOn); // 5 MINS
    lphIsOn = false;
 
}
void lphOn(){
      digitalWrite(lph, LOW);   // turn the LED on (HIGH is the voltage level)
    lphIsOn = true;

}
void mphPulse() {
  if (mphIsOn) {
    //   Serial.println("Alarm: - mph On");
    digitalWrite(mph, HIGH);   // turn the LED on (HIGH is the voltage level)
    mphIsOn = false;
  }
  else {
    //   Serial.println("Alarm: - mph off");
    digitalWrite(mph, LOW);   // turn the LED on (HIGH is the voltage level)
    mphIsOn = true;

  }
}
void sphPulse() {
  if (sphIsOn) {
    //  Serial.println("Alarm: - sph On");
    digitalWrite(sph, HIGH);   // turn the LED on (HIGH is the voltage level)
    sphIsOn = false;
  }
  else {
    //   Serial.println("Alarm: - sph off");
    digitalWrite(sph, LOW);   // turn the LED on (HIGH is the voltage level)
    sphIsOn = true;

  }
}
void sphSpeed(int s) {
  analogWrite(sph, s);   // turn the LED on (HIGH is the voltage level)

}
void checkTopUp() {
  //if float is low //read float switch
  if (floatNumOne) {
    //run pump for given time
    topUpPump = HIGH;
    // count pumps
    pumpRunCount++;
    //retest float switch
  }
  // run pump
  //create alarm to turn pump off
  Serial.println("pump on");

  Alarm.timerOnce(10, pumpOff);            // called once after 10 seconds

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
void resetPumpCount() {
  pumpRunCount = 0;
}
void pumpOff() {
  topUpPump = LOW;

  Serial.println("pump off");

}
void updateDisplay() {
  ringMeter(maxPh, minPh, minTemp, maxTemp, avgRoomTempMeasuredPH, avgTemp, 6, 9, xpos, ypos, radius, " Ph", GREEN2RED );
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
void checkSumpLevel() {
  Serial.println("checkSumpLevel");
  int distance = sonar.ping_cm();
  Serial.print(distance);
  if (distance <= 12 ){
    returnPumpSpeed + 5;
    if(returnPumpSpeed >= 254)returnPumpSpeed = 254;
    analogWrite(RETURNPUMP, returnPumpSpeed);   // turn the LED on (HIGH is the voltage level)
  }
  if( distance > 12 && distance <15){
    
  }
  else{
        returnPumpSpeed - 5;
    if(returnPumpSpeed <= 100)returnPumpSpeed = 100;

        analogWrite(RETURNPUMP, returnPumpSpeed);   // turn the LED on (HIGH is the voltage level)

  }

}

void digitalClockDisplay() {
  // digital clock display of the time
  t = rtc.getTime();
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
  dtostrf(sensors.getTempCByIndex(0), 6, 4, chrTemp);// This is where I get confused, I read that the 6 is the length of the number
  String s = chrTemp;
  //   Serial.println("\n");

//  Serial.println("temp" + s);
  Serial.println( s);

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
