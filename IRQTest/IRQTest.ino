void setup() {
  // put your setup code here, to run once:
  
  Serial.begin(9600);
   Serial.println("Program Start") ;
  attachInterrupt(0, TheButtonPressed, LOW);
}

void loop() {
  // put your main code here, to run repeatedly:
    Serial.print("now program run in loop()");
}

void TheButtonPressed()
{
  Serial.println("The Button is pressed by user") ;
}
  
 
