#include <WiFi.h>
#include <PubSubClient.h>

/*Dados do WIFI*/
const char* ssid = "rau"; //SSID do Wifi
const char* password =  "11229217"; //Senha do wifi

/*Dados do Broker */
const int mqttPort = 1883;
const char* mqttUser = "jflavioinacio22@gmail.com";
const char* mqttPassword = "cb62d165";
const char* mqttServer = "mqtt.dioty.co";

const char* subscribe_topic_led = "/jflavioinacio22@gmail.com/led";

byte estado_led = 0;

WiFiClient espClient;
PubSubClient client(espClient);

void Conexao_Wifi () {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Conectando ao WiFi...");
  }

  Serial.println("Conectado na rede WiFi");
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);

  while (!client.connected()) {
    Serial.println("Conectando ao Broker");
    if (client.connect("ESP32Client", mqttUser, mqttPassword )) {
      Serial.println("Conectado");
    }
    else {
      Serial.println("falha");
    }
    delay(1000);
  }
  client.publish("/jflavioinacio22@gmail.com/", "Funcionando");
  client.subscribe(subscribe_topic_led);
}


void callback(char* topic, byte* payload, unsigned int length) {//Função Callback
  
  payload[length] = '\0';
  String strMSG = String((char*)payload);
  Serial.print("Recebido: ");
  Serial.println(strMSG);
  String topico_string = String(topic);

  estado_led = strMSG.toInt();
}


void reconect() { //função pra reconectar ao servido MQTT
  if (!client.connected()) {
    Serial.println("Tentando conect. servidor MQTT");
    bool conectado = strlen(mqttUser) > 0 ?
                     client.connect("ESP32Client", mqttUser, mqttPassword) :
                     client.connect("ESP32Client");
    if (conectado) {
      Serial.println("Conectado!");
      client.subscribe(subscribe_topic_led, 1);
    }
    else {
      Serial.println("Falha: ");
      Serial.println( String(client.state()).c_str());
      delay(1000);
    }
  }
}

void setup() {

  Serial.begin(9600);//Inicializando o monitor serial com a velocidade de 9600bits/s
  Conexao_Wifi();
  pinMode(LED_BUILTIN, OUTPUT);//Definindo o pino do led como saida(OUTPUT)

}

void loop() {

  switch (estado_led) {
    case 0:
      digitalWrite(LED_BUILTIN, 0);
      break;

    case 1:
      digitalWrite(LED_BUILTIN, 1);
      break;
  }

  if (!client.connected()) {
    reconect();
  }
  client.loop();

}
