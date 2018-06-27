void setup()
{
  Serial.begin(9600);
}

void loop()
{
  if(Serial.available() > 3){
    Serial.read();
    Serial.read();
    Serial.read();
    Serial.read();
    Serial.flush();
    Serial.print("OK");
  }else{
    Serial.print("FAIL");    
  }
  delay(10);
}
