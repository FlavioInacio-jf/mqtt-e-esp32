#include <WiFi.h>
#include <PubSubClient.h>
const byte LED = 2;

/*Dados do WIFI*/
const char* ssid = "Redmi 3S"; //SSID do Wifi
const char* password =  "123456789"; //Senha do wifi

/*Dados do Broker */
const int mqttPort = 14600;
const char* mqttUser = "chvezqkg";
const char* mqttPassword = "nQOogcrPLnRe";
const char* mqttServer = "soldier.cloudmqtt.com";

const char* subscribe_topic_led = "estufa/led";

byte estado_led = 0;

WiFiClient espClient;
PubSubClient client(espClient);

void Conexao_Wifi () {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }
  if (WiFi.status() == WL_CONNECTED) {
    lcd.print("Conect. na rede WiFi");
    client.setServer(mqttServer, mqttPort);
    client.setCallback(callback);
    
    while (!client.connected()) {
      Serial.print("Conect. Broker");
      if (client.connect("ESP32Client", mqttUser, mqttPassword )) {
        Serial.print("Conectado");
      }
      else {
        lcd.print("falha ");
      }
      delay(1000);
    }
    client.subscribe(subscribe_topic_led);
  }
  else {
    Serial.print("Falha na conexação");
    delay(1000);
  }
}

void callback(char* topic, byte* payload, unsigned int length) {//Função Callback
  
  payload[length] = '\0';
  String strMSG = String((char*)payload);
  String topico_string = String(topic);
  
  estado_led = strMSG.toInt();
}


void reconect() { //função pra reconectar ao servido MQTT
  if (!client.connected()) {
    Serial.print("Tentando conect. servidor MQTT");
    bool conectado = strlen(mqttUser) > 0 ?
                     client.connect("ESP32Client", mqttUser, mqttPassword) :
                     client.connect("ESP32Client");
    if (conectado) {
      Serial.println("Conectado!");
      client.subscribe(subscribe_topic_led, 1);
    }
    else {
      lcd.print("Falha: ");
      lcd.print( String(client.state()).c_str());
      delay(1000);
    }
  }
}

void setup() {
  Serial.begin(9600);//Inicializando o monitor serial com a velocidade de 9600bits/s
  pinMode(LED, OUTPUT);//Definindo o pino do led como saida(OUTPUT)
}

void loop() {

  if(estado_led == 1){
    digitalWrite(LED, !digitalRead(LED));  
  }
  estado_led = 0
  
  if (!client.connected()) {
    reconect();
  }
  client.loop();

}
