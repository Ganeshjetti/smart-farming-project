#include <WiFi.h>
#include <DHT.h>
#define DHTPIN 2// what digital pin we're connected to
#define DHTTYPE DHT11   // DHT 22  (AM2302), AM2321)
DHT dht(DHTPIN, DHTTYPE);
#define echoPin 14 // attach pin D2 Arduino to pin Echo of HC-SR04
#define trigPin 15//attach pin D3 Arduino to pin Trig of HC-SR04
long duration; // variable for the duration of sound wave travel
int distance; // variable for the distance measurement
const char* MY_SSID = "Redmi";
const char* MY_PWD = "12345678";
WiFiClient client;
const char* TS_SERVER = "api.thingspeak.com";
String TS_API_KEY = "PZARAIFI61DCOKH6";

void connectWifi()
{
  Serial.print("Connecting to " + *MY_SSID);
  WiFi.begin(MY_SSID, MY_PWD);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi Connected");
  Serial.println("");
    
}

int soil=36;
int ldr=39;
int buzzer=5;
void setup() 
{
    // put your setup code here, to run once:
     pinMode(trigPin, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(echoPin, INPUT); // Sets the echoPin as an INPUT
    pinMode(ldr,INPUT);
    pinMode(soil,INPUT);
    pinMode(buzzer,OUTPUT);
    Serial.begin(9600);
   // pinMode(relay,OUTPUT);
   dht.begin();
    delay(1000);
    connectWifi();
}

void loop() {
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
}
  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);
  Serial.print(F("Humidity: "));
  Serial.println(h);
  delay(1000);
  Serial.print(F("Temperature: "));
  Serial.println(t);
  delay(1000);
 
  // Turn on the blacklight and print a message.
  if(h>56)
  {
    Serial.println("humditity:");
    Serial.println(h);
    digitalWrite(buzzer,HIGH);
    delay(1000);
  }
  if(t>=35)
  {
    digitalWrite(buzzer,HIGH);
    delay(1000);
  }
   
  int x = analogRead(soil);
  Serial.print("soil value: ");
  Serial.println(x);
  delay(1000);
  if(x>3500)
  {
   Serial.println("dry");
   delay(1000); 
  digitalWrite(buzzer,HIGH);
  delay(1000);
  }
  else
  {
   Serial.println("wet");
   delay(1000); 
  digitalWrite(buzzer,LOW);
  delay(1000);
  }
  int y = analogRead(ldr);
  Serial.print("ldr value: ");
  Serial.println(y);
  delay(1000);
   if(y>3400)
  {
   Serial.println("day time");
   delay(1000);
   
   
  }
  else{
 Serial.println("night time");
   delay(1000);
  }
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  distance = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)
  // Displays the distance on the Serial Monitor
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
  delay(1000);
  if(distance<10) {
    Serial.println("crop growth");
    delay(1000);
    }
  
 
   if (client.connect(TS_SERVER, 80))
  {
    String postStr = TS_API_KEY;
    postStr += "&field1=";
    postStr += String(x);
     postStr += "&field2=";
    postStr += String(y);
    postStr += "&field3=";
    postStr += String(t);
    postStr += "&field4=";
    postStr += String(h);
    postStr += "&field5=";
    postStr += String(distance);
    postStr += "\r\n\r\n";

    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: " + TS_API_KEY + "\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);
    delay(1000);
  }
  client.stop();
}
