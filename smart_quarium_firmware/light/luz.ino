const int portaLDR = 13; //Define pino do sensor luz
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200); // Inicia a porta serial
  pinMode(portaLDR, INPUT); //Inicia o sensor luz
  delay(1000);  
}

void loop() {
  // put your main code here, to run repeatedly:
 int val = analogRead(portaLDR);  
 Serial.println(val);  
 delay(100);  

}
