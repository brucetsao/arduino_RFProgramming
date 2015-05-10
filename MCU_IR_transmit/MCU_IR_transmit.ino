
long sendnum = 0 ;
long receivenum = 0 ;
long number = millis() ;

void setup() {
  // put your setup code here, to run once:
    Serial.begin(9600);
    Serial2.begin(9600);
    Serial3.begin(9600);
    Serial3.setTimeout(1000) ;
    

}

void loop() {
  // put your main code here, to run repeatedly:
      Serial2.write(number)  ;
      delay(50);  
      if (Serial3.available())
        {
       
               Serial.println(Serial3.read()) ;  
      }
     number ++ ;
  //  delay(100);    

}
