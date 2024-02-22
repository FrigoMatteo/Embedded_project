#include <WiFi.h>
#include <PubSubClient.h>

// Wi-Fi setting
const char* ssid = "/Wifi name/";     // *********** IMPORTANT *********** WiFi name
//const char* ssid = "/Wifi name/";     // *********** IMPORTANT *********** WiFi name
const char* password = "/WifiPassword/";       // *********** IMPORTANT *********** WiFi password

// MQTT setting
const char* ipServer = "192.168.24.240"; // *********** IMPORTANT *********** MQTT server IP
const int mqttPort = 1883;                // port for the server communication
const char* mqttUser = "MPS432";          // casual name
const char* mqttPassword = "";            // left it empty it's not request

// subscriber topic
const char* RECEIVING_TOPIC = "msp";

// publisher topic
const char* SENSOR_FREQ = "sensor/freq";
const char* SENSOR_DATA_C = "sensor/data/curr";
const char* SENSOR_DATA_H = "sensor/data/hum";
const char* SENSOR_DATA_T = "sensor/data/temp";
const char* SENSOR_DATA_L = "sensor/data/lux";
const char* MIN_MAX = "min_max";
const char* ACK = "ack";
const char* ALARM = "alarm";

WiFiClient espClient;
PubSubClient client(espClient);

const int baund_rate = 115200;
String data;
char c;

// Data from web to msp
void callback(char* topic, byte* payload, unsigned int length) {   
  Serial.println("da web to msp:");
  for (int i = 0; i < length; i++) {
    Serial2.write(payload[i]);
    //delay(100);
  }
  // Send string terminator
  Serial2.write("#");
}

void reconnect() {
  while (!client.connected()) {
    Serial.println("Connessione al server MQTT...");
    if (client.connect("ESP32Client", mqttUser, mqttPassword)) {
      Serial.println("Connesso al server MQTT");
      Serial2.write("q");
      Serial2.write("Q");
      Serial2.write("0");
      Serial2.write("#");
      client.subscribe(RECEIVING_TOPIC);
    } else {
      Serial.print("Connessione fallita con errore ");
      Serial.println(client.state());
      Serial2.write("q");
      Serial2.write("Q");
      Serial2.write("1");
      Serial2.write("#");
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);

  // Connection to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connessione alla rete Wi-Fi in corso...");
  }
  Serial.println("Connesso alla rete Wi-Fi");

  // Connection to server MQTT
  client.setServer(ipServer, mqttPort);
  while (!client.connected()) {
    Serial.println("Connessione al server MQTT...");
    if (client.connect("ESP32Client", mqttUser, mqttPassword)) {
      Serial.println("Connesso al server MQTT");
      Serial2.write("q");
      Serial2.write("Q");
      Serial2.write("0");
      Serial2.write("#");
      Serial.println("MQ0");

      // Subscribed to topic
      client.subscribe(RECEIVING_TOPIC);
    } else {
      Serial.print("Connessione fallita con errore ");
      Serial.println(client.state());
      Serial2.write("q");
      Serial2.write("Q");
      Serial2.write("1");
      Serial2.write("#");
      Serial.println("MQ1");
      delay(2000);
    }
  }
  // Setup pin 
  Serial2.begin(baund_rate,SERIAL_8N1,16,17);

  // "Thread" listening on MQTT
  client.setCallback(callback);
}

String getData(String input) {
    String data = "";
    for (int i = 2; i < input.length(); i++) {
        char c = input.charAt(i);
        if (isDigit(c) || c == ',') {
            data += c;
        } else {
            // Se non è un carattere numerico, esci dal ciclo
            break;
        }
    }
    return data;
}

void loop() {
  
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
  // Channel from msp to web
  while(Serial2.available()){
    data = Serial2.readStringUntil('#');
    Serial.println("da msp a web:" + data);
    if (data.compareTo("") != 0){
      
      if (data.startsWith("CU"))
        client.publish(SENSOR_DATA_C, getData(data).c_str());

      else if (data.startsWith("HU"))
        client.publish(SENSOR_DATA_H, getData(data).c_str());
      
      else if (data.startsWith("TE")) 
        client.publish(SENSOR_DATA_T, getData(data).c_str());
    
      else if (data.startsWith("LU"))
        client.publish(SENSOR_DATA_L, getData(data).c_str());
      
      else if (data.equals("15") || data.equals("30") || data.equals("60") || data.equals("1D")) 
        client.publish(SENSOR_FREQ, data.c_str());
      
      else if (data.compareTo("AC1D") == 0) 
        client.publish(ACK, data.c_str());
    
      else if (data.startsWith("AC"))
        client.publish(ACK, data.c_str());
      
      else if (data.equals("DD") || data.equals("MM") || data.equals("YY") || data.equals("AT")) 
        client.publish(MIN_MAX, data.c_str());
      
      else if (data.compareTo("!!") == 0)
        client.publish(ALARM, data.c_str());

      else if (data.compareTo("AS") == 0)
        client.publish(ALARM, data.c_str());  
/*
      else if (data.compareTo("XX") == 0)
        client.publish(ALARM, data.c_str());  
*/

      else if (data.compareTo("XX") == 0) {
          Serial2.write("M");
          Serial2.write("Q");
          Serial2.write("0");
          Serial2.write("#");
      }

    }
    data = String("");
  }
  
}
