void loop() 
{
  if(configBt.available()) // if the HC05 is sending something… 
  {
    Serial.print(configBt.readString()); // print in serial monitor
  }
  if(Serial.available()) // if serial monitor is outputting something… 
  {
    configBt.write(Serial.read()); // write to Arduino’s Tx pin
  }
}
