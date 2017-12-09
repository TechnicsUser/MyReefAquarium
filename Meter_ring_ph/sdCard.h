//
//void writeToSd(Time  t3) {
//  
//  File dataFile = SD.open("PHLOG.txt", FILE_WRITE);
//  dataString += " | ";
//  dataString += avgTemp;
//  dataString += " | ";
//
//
//  dataString += hour();
//  dataString += ":";
//
//  dataString += minute();
//  dataString += " | ";
//  dataString += t3.date;
//  dataString += "/";
//
//  dataString += t3.mon;
//  dataString += "/";
//
//  dataString += t3.year;
//  dataString += " | ";
// // dataString += pumpRunCount;
//  // if the file is available, write to it:
//  if (dataFile) {
//    dataFile.println(dataString);
//    Serial.println("card write.");
//
//    dataFile.close();
//    // print to the serial port too:
//    Serial.println(dataString);
//  }
//  // if the file isn't open, pop up an error:
//  else {
//    Serial.println("error opening datalog.txt");
//    dataFile.close();
//  }
//}
