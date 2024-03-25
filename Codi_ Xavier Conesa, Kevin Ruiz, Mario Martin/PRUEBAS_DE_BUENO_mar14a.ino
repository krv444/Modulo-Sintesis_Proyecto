#include "arduino_secrets.h"
#include <Arduino_JSON.h>
#include <WiFiNINA.h>
#include <Arduino_MKRIoTCarrier.h>
#include <ArduinoMqttClient.h>
#include <PubSubClient.h>


MKRIoTCarrier carrier;
 
const char* ssid = "docent-passadissssss";
const char* password = "08044624";

const char* mqtt_server = "10.1.2.96";
const int mqtt_port = 1883;
const char* mqtt_user = "pi";
const char* mqtt_password = "raspberry";

int keyIndex = 0;

	
int status = WL_IDLE_STATUS;      // connection status
WiFiServer server(80);           // server socket
 
WiFiClient client2;
WiFiClient espClient;
  PubSubClient client(espClient);

const char* topic = "Proyecte";
const char* topic2 = "Proyecte1";


String relayState = "";


String apiKey = "1d22aab0e8be144c58b57f70cb5b9897"; //paste weather map API KEY between ""

	
String location2 = "Copenhagen, DK";
String location3 = "Madrid, ES";

bool changeLocation = false;

	
double temperature;
double feelsLike;
double lon;
double lat;

float temperatura;
float humedad;

void setup() {
  Serial.begin(9600);
  delay(1500);
  CARRIER_CASE = false;
  carrier.begin();
  carrier.display.setTextSize(2);
  carrier.display.setCursor(20, 50); 
  
  enable_WiFi();
  connect_WiFi();
  server.begin();
  printWifiStatus();
    client.setServer(mqtt_server, mqtt_port);
  while (!client.connected()) {
    Serial.println("Conectando al servidor MQTT...");
    if (client.connect("arduinoClient", mqtt_user, mqtt_password)) {
      Serial.println("Conectado al servidor MQTT");
    } else {
      Serial.print("Error al conectar al servidor MQTT, rc=");
      Serial.print(client.state());
      Serial.println(" Intentando de nuevo en 5 segundos");
      delay(5000);
    }
    updateWeather();
  
  carrier.display.setTextSize(2);
  carrier.display.setCursor(20, 130);
  carrier.display.println("press any button");
  carrier.display.setCursor(60, 160);
  carrier.display.println("to start");
  }
}
void enable_WiFi() {
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }
 
  String fv = WiFi.firmwareVersion();
  if (fv < "1.0.0") {
    Serial.println("Please upgrade the firmware");
  }
}
void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
 
  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);



}
void connect_WiFi() {
  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, password);
    carrier.display.println("Waiting for        Connection ...");
    // wait 10 seconds for connection:
    delay(10000);
  }
}


void loop() {
  
  char temperatura_str[10];
  char humedad_str[10];
  sprintf(temperatura_str, "%.2f", temperatura);
  sprintf(humedad_str, "%.2f", humedad);

  char mensaje[50];
  snprintf(mensaje, sizeof(mensaje), "{\"temperatura\": %s, \"humedad\": %s}", temperatura_str, humedad_str);
  // Publicar el valor en el topic MQTT
  if (client.publish(topic, mensaje)) {
    Serial.println("Mensaje publicado correctamente");
  } else {
    Serial.println("Error al publicar el mensaje");
  }

  // Esperar un tiempo antes de enviar el siguiente mensaje
  delay(5000);


  temperatura = carrier.Env.readTemperature();
  humedad = carrier.Env.readHumidity();
  
  updateWeather();
  delay(5000);
  //Update touch buttons
  carrier.Buttons.update();
 
  //function to print out values
  if (carrier.Buttons.onTouchDown(TOUCH0)) {
    printTemperatura();
  }
 
  if (carrier.Buttons.onTouchDown(TOUCH1)) {
    printhumedad();
  }

}
  void printTemperatura() {
  if (temperatura > 80) {
    carrier.display.fillScreen(ST77XX_RED);
    carrier.display.setTextColor(ST77XX_WHITE); //white text
    carrier.display.setTextSize(2); //medium sized text
    carrier.display.setCursor(20, 110); //sets position for printing (x and y)
    carrier.display.print("Temp: ");
    carrier.display.print(temperatura);
    carrier.display.println(" C");
} else if (temperatura > 20 && temperatura <= 30) {
    carrier.display.fillScreen(ST77XX_GREEN);
    carrier.display.setTextColor(ST77XX_WHITE); //white text
    carrier.display.setTextSize(2); //medium sized text
    carrier.display.setCursor(20, 110); //sets position for printing (x and y)
    carrier.display.print("Temp: ");
    carrier.display.print(temperatura);
    carrier.display.println(" C");
} else if (temperatura >= 5 && temperatura <= 20) {
    carrier.display.fillScreen(ST77XX_BLUE);
    carrier.display.setTextColor(ST77XX_WHITE); //white text
    carrier.display.setTextSize(2); //medium sized text
    carrier.display.setCursor(20, 110); //sets position for printing (x and y)
    carrier.display.print("Temp: ");
    carrier.display.print(temperatura);
    carrier.display.println(" C");
}

}
 
void printhumedad() {
  //configuring display, setting background color, text size and text color

  if (humedad > 80) {
    carrier.display.fillScreen(ST77XX_RED);
    carrier.display.setTextColor(ST77XX_WHITE); //white text
    carrier.display.setTextSize(2); //medium sized text
    carrier.display.setCursor(20, 110); //sets position for printing (x and y)
    carrier.display.print("Humi: ");
    carrier.display.print(humedad);
    carrier.display.println(" %");
} else if (humedad > 60 && humedad <= 80) {
    carrier.display.fillScreen(ST77XX_YELLOW);
    carrier.display.setTextColor(ST77XX_WHITE); //white text
    carrier.display.setTextSize(2); //medium sized text
    carrier.display.setCursor(20, 110); //sets position for printing (x and y)
    carrier.display.print("Humi: ");
    carrier.display.print(humedad);
    carrier.display.println(" %");
} else if (humedad >= 30 && humedad <= 60) {
    carrier.display.fillScreen(ST77XX_BLUE);
    carrier.display.setTextColor(ST77XX_WHITE); //white text
    carrier.display.setTextSize(2); //medium sized text
    carrier.display.setCursor(20, 110); //sets position for printing (x and y)
    carrier.display.print("Humi: ");
    carrier.display.print(humedad);
    carrier.display.println(" %");
}

}

void updateWeather() {
  Serial.println("\nStarting connection to server...");
  // if you get a connection, report back via serial:
  if (client2.connect("api.openweathermap.org", 80)) { // Cambia la dirección IP por la del servidor HTTP
    Serial.println("Connected to server");
    // Make a HTTP request:
    client2.print("GET /data/2.5/weather?");
 
    //used to switch between locations
    changeLocation = !changeLocation;
 
    if (changeLocation == true) {
      client2.print("q=" + location3);
    }
    else {
      client2.print("q=" + location2);
    }


    client2.print("&appid=" + apiKey);
    client2.print("&cnt=3");
    client2.println("&units=metric");
    client2.println("Host: api.openweathermap.org");
    client2.println("Connection: close");
    client2.println();
  } else {
    Serial.println("Unable to connect");
  }
  delay(1000);
 
  String line = "";
  if (client2.connected()) {
    line = client2.readStringUntil('\n');
    Serial.println(line);
    JSONVar myObject = JSON.parse(line);
 
    String cityName = JSON.stringify(myObject["name"]);
    String weatherDescription = JSON.stringify(myObject["weather"][0]["description"]);
 
    double temperature = myObject["main"]["temp"];
    double feelsLike = myObject["main"]["feels_like"];
    double lon = myObject["coord"]["lon"];
    double lat = myObject["coord"]["lat"];
 
    // Formatear los datos en un mensaje JSON
    String mensaje = "{\"city\": \"" + cityName + "\", \"weather\": \"" + weatherDescription + "\", \"temperature\": " + String(temperature) + ", \"feels_like\": " + String(feelsLike) + ", \"lon\": " + String(lon) + ", \"lat\": " + String(lat) + "}";

    // Publicar el mensaje en el tema MQTT
    if (client.publish(topic2, mensaje.c_str())) {
      Serial.println("Mensaje publicado correctamente");
    } else {
      Serial.println("Error al publicar el mensaje");
    }
 
    carrier.Buzzer.sound(300);
    delay(300);
    carrier.Buzzer.noSound();
    delay(300);
    carrier.Buzzer.sound(300);
    delay(300);
    carrier.Buzzer.noSound();
  }
}