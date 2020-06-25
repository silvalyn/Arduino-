
#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
//


#include <TinyGPS++.h>


#include <SoftwareSerial.h>
#include <Ultrasonic.h>

#include <DHT.h>  // Including library for dht
 
#include <ESP8266WiFi.h>

#define FIREBASE_HOST "gsmdata-15fd4.firebaseio.com"
#define FIREBASE_AUTH "k1j69XGhOzP3KTjpFAXmj0QpETPIHMVaievhIu63"

#define WIFI_SSID "Khumalo"
#define WIFI_PASSWORD "144khumalo45"

#define trigPin 11
#define echoPin 12


 
String apiKey = "VRA0AIHIHXBCRNIU";     //  Enter your Write API key from ThingSpeak
 
const char *ssid =  "Khumalo";     // replace with your wifi ssid and wpa2 key
const char *pass =  "144khumalo45";
const char* server = "api.thingspeak.com";

int GIu_Ultrasonic_Dist_CM=0;

String myGabageLevel;
String myTemp;
String myHumidity;
String myGPS;

float latitude , longitude;
int year , month , date, hour , minute , second;
String date_str , time_str , lat_str , lng_str;
int pm;
 
#define DHTPIN 2          //pin where the dht11 is connected
Ultrasonic ultrasonic(14,12);  
 
DHT dht(DHTPIN, DHT11);

TinyGPSPlus gps;

SoftwareSerial ss(4, 5);


 
WiFiClient client;
 
void setup() 
{
       Serial.begin(115200);
       delay(10);
       dht.begin();
       Serial.println("Connecting to ");
       Serial.println(ssid);
       ss.begin(9600);
 
       WiFi.begin(ssid, pass);
 
      while (WiFi.status() != WL_CONNECTED) 
     {
            delay(500);
            Serial.print(".");
     }
      Serial.println("");
      Serial.println("WiFi connected");
    
//     Firebase.reconnectWiFi(true);
      WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
       Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }


  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());
//     Firebase.setString("Variable/GabegeLevel","fahad");

 Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}

void checkGPS(){
  if (gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
     
  }
}
 int n = 0;
void loop() 
{
  

//      while (ss.available() > 0) 
//    {
//      if(ss.available()){
//          Serial.print("availabel");
//        }
//      
//      
//      // sketch displays information every time a new sentence is correctly encoded.
//      if (gps.encode(ss.read()))
//       float latitude = (gps.location.lat());     //Storing the Lat. and Lon. 
//       float longitude = (gps.location.lng());
//                       
//        Serial.print("GPS Coodinates");
//        latitude = (gps.location.lat());     //Storing the Lat. and Lon. 
//        longitude = (gps.location.lng());
//        Serial.print("LAT:  ");
//        Serial.println(latitude, 6);  // float to x decimal places
//        Serial.print("LONG: ");
//        Serial.println(longitude, 6);
//       
//  }


      
      delay(2000);



long duration, distance;
int max = 80; // Let consider as Height of the Garbage Bin is = 80 cm.
float diff, perc;
digitalWrite(14, LOW);
delayMicroseconds(2);
digitalWrite(14, HIGH);
delayMicroseconds(10);
digitalWrite(14, LOW);
duration = pulseIn(12, HIGH);
distance = (duration/2) / 10;
diff = max - distance; // 'diff' variable tells u that, how much the Garbage Bin is Left to fill.
perc = (diff/max)*100; // 'perc' variable tells u that, how much percentage the Garbage Bin is filled.
Serial.print("Garbage level: ");
Serial.print(perc);
Serial.print("%\t");

      
      GIu_Ultrasonic_Dist_CM=ultrasonic.Ranging(CM);  
                Serial.print(GIu_Ultrasonic_Dist_CM); 
                Serial.println(" cm" );
                
//        Serial.print("GPS Coodinates");
//        float latitude = (gps.location.lat());     //Storing the Lat. and Lon. 
//        float longitude = (gps.location.lng());
//        Serial.print("LAT:  ");
//        Serial.println(latitude, 6);  // float to x decimal places
//        Serial.print("LONG: ");
//        Serial.println(longitude, 6);


 while (ss.available() > 0) //while data is available
    if (gps.encode(ss.read())) //read gps data
    {
      if (gps.location.isValid()) //check whether gps location is valid
      {
        latitude = gps.location.lat();
        lat_str = String(latitude , 6); // latitude location is stored in a string
        longitude = gps.location.lng();
        lng_str = String(longitude , 6); //longitude location is stored in a string


 Serial.print("LAT:  ");
        Serial.println(lat_str);  // float to x decimal places
        Serial.print("LONG: ");
        Serial.println(lng_str);
        
      }
      if (gps.date.isValid()) //check whether gps date is valid
      {
        date_str = "";
        date = gps.date.day();
        month = gps.date.month();
        year = gps.date.year();
        if (date < 10)
         date_str = '0';
        date_str += String(date);// values of date,month and year are stored in a string
        date_str += " / ";
       }

    }
        
      float h = dht.readHumidity();
      float t = dht.readTemperature();
      
              if (isnan(h) || isnan(t)) 
                 {
                     Serial.println("Failed to read from DHT sensor!");
                      return;
                 }
 
                         if (client.connect(server,80))   //   "184.106.153.149" or api.thingspeak.com
                      {  
                            
                             String postStr = apiKey;
                             postStr +="&field1=";
                             postStr += String(t);
                             postStr +="&field2=";
                             postStr += String(h);
                             postStr +="&field3=";
                             postStr += String(perc);
                            
                             postStr += "\r\n\r\n\r\n";
 
                             client.print("POST /update HTTP/1.1\n");
                             client.print("Host: api.thingspeak.com\n");
                             client.print("Connection: close\n");
                             client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
                             client.print("Content-Type: application/x-www-form-urlencoded\n");
                             client.print("Content-Length: ");
                             client.print(postStr.length());
                             client.print("\n\n");
                             client.print(postStr);
 
                             Serial.print("Temperature: ");
                             Serial.print(t);
                             Serial.print(" degrees Celcius, Humidity: ");
                             Serial.print(h);
                             Serial.println("%. Send to Thingspeak.");
                        }
          
 
          Serial.println("Waiting...");

          myGabageLevel = String(perc);
          myTemp = String(t);
          myHumidity = String(h);
         String  myGPS1 = String(longitude);
         String myGPS2 = String(latitude);
          
          Firebase.setString("BinData/Bin1/GabegeLevel", myGabageLevel); 
            if (Firebase.failed()) {
      Serial.print("setting /message failed:");
      Serial.println(Firebase.error());  
      return;
  }
  delay(1000);
          Firebase.setString("BinData/Bin1/Humidity", myHumidity);
            if (Firebase.failed()) {
      Serial.print("setting /message failed:");
      Serial.println(Firebase.error());  
      return;
  }
  delay(1000);
          Firebase.setString("BinData/Bin1/Location", lng_str);
            if (Firebase.failed()) {
      Serial.print("setting /message failed:");
      Serial.println(Firebase.error());  
      return;
  }
  delay(1000);
    Firebase.setString("BinData/Bin1/Location2", lat_str);
            if (Firebase.failed()) {
      Serial.print("setting /message failed:");
      Serial.println(Firebase.error());  
      return;
  }
  delay(1000);
  
          Firebase.setString("BinData/Bin1/Temperature", myTemp);
            if (Firebase.failed()) {
      Serial.print("setting /message failed:");
      Serial.println(Firebase.error());  
      return;
  }
  delay(1000);

          client.stop();
  
  // thingspeak needs minimum 15 sec delay between updates
  delay(1000);
}
