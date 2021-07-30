#include <OneWire.h>
#include <DallasTemperature.h>
#include <WiFi.h>
#include <PubSubClient.h>

const int portaLDR = 32; //Define pino do sensor luz
const int PINO_ONEWIRE = 4; // Define pino do sensor temperatura

const int NIVEL_PIN = 13; // ESP32 pin connected to button's pin

// Replace with your network credentials
const char* ssid = "xxxxx"; // id wifi
const char* password = "xxxxx";

OneWire oneWire(PINO_ONEWIRE); // Cria um objeto OneWire
DallasTemperature sensor(&oneWire); // Informa a referencia da biblioteca dallas temperature para Biblioteca onewire
DeviceAddress endereco_temp; // Cria um endereco temporario da leitura do sensor
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient); 

#define MSG_BUFFER_SIZE  (70)
char msg[MSG_BUFFER_SIZE];

char *mqttServer = "xxx.xxx.x.xx"; // local in my case
int mqttPort = 1883;
int last_time  = 0;
char nivelString[11];


void setup() {
  Serial.begin(115200); // Inicia a porta serial
  Serial.println("Iniciando Aquário inteligente ... "); // Imprime a mensagem inicial

  // Wifi e Mqqt configuracoes
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi");

    while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println();
  
  // Print ESP Local IP Address
  Serial.println(WiFi.localIP());
  
  setupMQTT();
  
//  
  sensor.begin(); // Inicia o sensor temperatura
//  
  pinMode(portaLDR, INPUT); //Inicia o sensor luminosidade

  pinMode(NIVEL_PIN, INPUT_PULLUP); // set ESP32 pin to input pull-up mode

}


void setupMQTT() {
  mqttClient.setServer(mqttServer, mqttPort);
//  // set the callback function
//  mqttClient.setCallback(callback);
}

  
void loop() {

  if (!mqttClient.connected()){
      reconnect();
    }
    
    mqttClient.loop();

//    // realizar leitura do sensor ldr
//    int leitura = analogRead(portaLDR);
//    
//    // Realizar o print da leitura no serial
//    Serial.println("Leitura do Sensor de LDR:");
//    Serial.println(leitura);
  
  sensor.requestTemperatures(); // Envia comando para realizar a conversão de temperatura
  if (!sensor.getAddress(endereco_temp,0)) { // Encontra o endereco do sensor no barramento
    Serial.println("SENSOR NAO CONECTADO"); // Sensor conectado, imprime mensagem de erro
    Serial.println(sensor.getTempCByIndex(0));
    
  } else {

    long now = millis();
    if (now - last_time > 1000) {
    
      // Collect temperature
      Serial.print("Temperatura = "); // Imprime a temperatura no monitor serial
      float temperatura = sensor.getTempC(endereco_temp);
      Serial.println(temperatura, 1); // Busca temperatura para dispositivo

      //Collect Luminosity
      Serial.print("Luminosidade = ");
      int luminosidade = analogRead(portaLDR);  
      Serial.println(luminosidade);
      
      Serial.print("Nível água = ");
      int currentNivelState = digitalRead(NIVEL_PIN);
      if(currentNivelState == 0){
        strncpy(nivelString, "Nivel Alto", sizeof(nivelString));
//        nivelString = "Nivel Alto";
        }else{
          strncpy(nivelString, "Nivel Baixo", sizeof(nivelString));
        }
        Serial.println(nivelString);
      
      
      
      // build payload
      char temperaturaString[8];
      dtostrf(temperatura, 1, 2, temperaturaString);

      char luminosidadeString[8];
      dtostrf(luminosidade, 1, 2, luminosidadeString);
      
//      snprintf(msg, MSG_BUFFER_SIZE, "{\"temperatura\": %s,\"luminosidade\": %s,\"nivel\": %s}", temperaturaString, luminosidadeString, nivelString);
      snprintf(msg, MSG_BUFFER_SIZE, "{\"temperatura\": %s,\"luminosidade\": %s,\"nivel\": \"%s\"}", temperaturaString, luminosidadeString, nivelString);
      Serial.println("-----PAYLOAD-----");
      Serial.println(msg);
      Serial.println("-----------------");
      
      // publish payload
      mqttClient.publish("/admin/3a79ef/attrs", msg);
      
      last_time = now;
  }
//
}
}

void reconnect() {
  Serial.println("Connecting to MQTT Broker...");
  while (!mqttClient.connected()) {
      Serial.println("Reconnecting to MQTT Broker..");
      String clientId = "ESP32Client-";
      clientId += String(random(0xffff), HEX);
      
      if (mqttClient.connect(clientId.c_str())) {
        Serial.println("Connected.");
//        // subscribe to topic
//        mqttClient.subscribe("/swa/commands");
      }
      
  }
}
