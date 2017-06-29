
//#include <EEPROM.h>
//#include <LCD5110.h>

//My scratch notes
//measuredPH-6.8580 roomTempMeasuredPH-6.8965 
//tempC-28.0195 phVolts-2.3535 7CalVolts-2.3047 4CalVolts-3.3594 4CalTempAdjusted-3.9640


//float volt4 = 3.359;
//float volt7 = 2.304;
//float calibrationTempC = 21;

float volt4 = 3.26;//3.171 worked ...3.341;
float volt7 = 2.5108;//2.684;
float calibrationTempC = 18.1;



//A3 = temp
//A2 = PH
//CALIBRATE = D8
int phPin = A1;
int tempPin = A3;
int calPin = 8;
int relayPin =13;

void setup()
{
   // LcdInitialise();
 //   LcdClear();
    Serial.begin(9600);
    
    pinMode(relayPin,OUTPUT);
    digitalWrite(relayPin,LOW);
}

float measurePHVolts()
{
    float phADC = analogRead(phPin);
    float phVolts = (phADC/1024.0)*5.0;
    return phVolts;
}

float getTempAdjusted4()
{
    //http://www.omega.com/Green/pdf/pHbasics_REF.pdf
    float adjustTemp = calibrationTempC;
    float difference = adjustTemp-25;
    float phAdjust = (0.009 * difference);
    float tempAdjusted4 = 4 + phAdjust;
    return tempAdjusted4;
}  


float measurePH()
{
    
    float phVolt = measurePHVolts();
      
    float tempAdjusted4 = getTempAdjusted4();
    float voltsPerPH = (abs(volt7-volt4)) / (7-tempAdjusted4);
  
    //acid ph's produce positive voltages, basic ph's produce negative voltages
    //that one site was wrong
    
    float realPHVolt = (volt7 - phVolt);
    float phUnits = realPHVolt / voltsPerPH;
    float measuredPH = 7 + phUnits;
    
    return measuredPH;
}

float doPHTempCompensation(float PH, float temp)
{
    float difference = temp-25;
    float phAdjust = (0.009 * difference);
    float tempAdjustedPH = PH + phAdjust;
    return tempAdjustedPH;
}


float measureTempC()
{
    float tempADC = analogRead(tempPin);
    float tempVolts = (tempADC/1024)*5.0;
    float tempC = (tempVolts/0.010);
    return tempC; 
}



// floatToString.h
//

// float to string
// 
// If you don't save this as a .h, you will want to remove the default arguments 
//     uncomment this first line, and swap it for the next.  I don't think keyword arguments compile in .pde files

//char * floatToString(char * outstr, float value, int places, int minwidth=, bool rightjustify) {
char * floatToString(char * outstr, float value, int places, int minwidth=0, bool rightjustify=false) {
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
        d/= 10.0;    
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
    if (minwidth > charcount){        
        extra = minwidth - charcount;
        charcount = minwidth;
    }

    if (extra > 0 and rightjustify) {
        for (int i = 0; i< extra; i++) {
            outstr[c++] = ' ';
        }
    }

    // write out the negative if needed
    if (value < 0)
        outstr[c++] = '-';

    if (tenscount == 0) 
        outstr[c++] = '0';

    for (i=0; i< tenscount; i++) {
        digit = (int) (tempfloat/tens);
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
        for (int i = 0; i< extra; i++) {
            outstr[c++] = ' ';
        }
    }


    outstr[c++] = '\0';
    return outstr;
}


void loop()
{
  //delay(100);
    int x;
    int sampleSize = 5000;

    float avgMeasuredPH= 0;
    float avgRoomTempMeasuredPH =0;
    float avgTemp = 0;
    float avgPHVolts =0;
    float avgVoltsPerPH =0;
    float phTemp = 0;
 
    
    float tempAdjusted4 = getTempAdjusted4();

    for(x=0;x< sampleSize;x++)
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
    
    Serial.print(" measuredPH-");
    Serial.print(avgMeasuredPH,4);
    Serial.print(" roomTempMeasuredPH-");
    Serial.print(avgRoomTempMeasuredPH,4);
    Serial.print(" tempC-");
    Serial.print(avgTemp,4);
    Serial.print(" phVolts-");
    Serial.print(avgPHVolts,4);
    Serial.print(" 7CalVolts-");
    Serial.print(volt7,4);
    Serial.print(" 4CalVolts-");
    Serial.print(volt4,4);    
    Serial.print(" 4CalTempAdjusted-");
    Serial.println(tempAdjusted4,4);
    
//    
//    drawBox();
//    char charBuffer[25];
//    gotoXY(5,1);
//    LcdString("PH:");
//    gotoXY(30,1);
//    floatToString(charBuffer,avgRoomTempMeasuredPH,2);
//    LcdString(charBuffer);
//    
//    gotoXY(4,2);
//    LcdString("TempC: ");
//    gotoXY(45,2);
//    floatToString(charBuffer,avgTemp,1);
//    LcdString(charBuffer);
//  
//    gotoXY(4,3);
//    LcdString("V:");  
//    gotoXY(20,3);
//    floatToString(charBuffer,avgPHVolts,3);
//    LcdString(charBuffer);
  
    if(avgMeasuredPH > 5.9)
    {
      digitalWrite(relayPin,HIGH);
//      gotoXY(4,4);
//      LcdString("dosing on");
    }
    else
    {
      digitalWrite(relayPin,LOW);
//      gotoXY(4,4);
//      LcdString("dosing off");
    }


}



