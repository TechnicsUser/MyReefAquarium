// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS A0
// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

#define heater  6 // RELAY5

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
float volt4 = 3.2300;
float volt7 = 2.6924; //2.7150
float calibrationTempC = 21.4;
int phPin = A1;

int xAxis = 0;
boolean heaterOn = true;

float getTempAdjusted4()
{
  //http://www.omega.com/Green/pdf/pHbasics_REF.pdf
  float adjustTemp = calibrationTempC;
  float difference = adjustTemp - 25;
  float phAdjust = (0.009 * difference);
  float tempAdjusted4 = 4 + phAdjust;
  return tempAdjusted4;
}
float measurePHVolts()
{
  float phADC = analogRead(phPin);
  float phVolts = (phADC / 1024.0) * 5.0;
  return phVolts;
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
  float s = sensors.getTempCByIndex(0);

  //  float tempADC = analogRead(tempPin);
  //  float tempVolts = (tempADC / 1024) * 5.0;
  //  float tempC = (tempVolts / 0.010);
  //  return tempC;
  return s;
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
  //  phTime[0] = h + phRunTime[0];
  //  phTime[1] = m + phRunTime[1];
  //  phTime[2] = s + phRunTime[2];
  //  lastReadPhTime = rtc.getTime();

  //  Serial.print(" measuredPH = ");
  //  dtostrf(avgMeasuredPH, 6, 4, chrPh);
  //  String s = chrPh;
  // Serial.print("ph" + avgMeasuredPH);
  //    Serial.print(" roomTempMeasuredPH-");
  //    Serial.print(avgRoomTempMeasuredPH,4);
  Serial.println("");

  Serial.print("*H");

  Serial.print("X");
  Serial.print(xAxis++);

  Serial.print("Y");
  Serial.print(avgMeasuredPH);
  Serial.print("*");

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
void readTemp() {
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

