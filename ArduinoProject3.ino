#include <DHT.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define DHTPIN D7
#define DHTTYPE DHT11

const char* ssid     = "";
const char* password = "";
 
const char* mqtt_server = "" ;
const int mqtt_port = 1883;


WiFiClient ESPClient;
PubSubClient client(ESPClient);

// Inicializamos el sensor DHT11
DHT dht(DHTPIN, DHTTYPE);
 
void setup() {
  // Inicializamos comunicación serie
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid,password);
  Serial.println("Empiezo");
  Serial.print("Conectando a:\t");
  Serial.println(ssid); 

  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(200);
  Serial.print('.');
  }

  // Mostrar mensaje de exito y dirección IP asignada
  Serial.println();
  Serial.print("Conectado a:\t");
  Serial.println(WiFi.SSID()); 
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());
  // Comenzamos el sensor DHT
  dht.begin();
  client.setServer(mqtt_server,mqtt_port);
  
 
}
void reconnect() {
  while (!client.connected()){
    Serial.print("Intentando conexion MQTT");
    if (client.connect("ESP8266Client")){
      Serial.println("Conectado");
    } else {
      Serial.print("Fallo");
      Serial.print(client.state());
      delay (5000);
    }
  }
}
void loop() {
    // Esperamos 5 segundos entre medidas
  delay(5000);
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  // Leemos la humedad relativa
  float h = dht.readHumidity();
  // Leemos la temperatura en grados centígrados (por defecto)
  float t = dht.readTemperature();

  // Comprobamos si ha habido algún error en la lectura
  if (!isnan(h) || !isnan(t)) {
    char tempString[8];
    char humString[8];
    dtostrf(t, 1, 2, tempString);
    dtostrf(h, 1, 2, humString);
    client.publish("pic2/temperature", tempString);
    client.publish("pic2/humidity", humString);
    Serial.println("Humedad: ");
    Serial.println(h);
    Serial.println("Temperatura: ");
    Serial.println(t);
    Serial.println("Datos enviados al broker");
  }
  else{
    Serial.println("Error leyendo el sensor DHT11");
  }
  
  
}
