#include <SoftwareSerial.h>
#include <SPI.h>

#include "LoRa.h"
#include "TinyGPS.h"

#define ID 43

//#define DEBUG

// format SOS:ID,LON,LAT

// String target = "$GPGGA,063550.80,2500.72,N,1212.40261,E,1,05,4.54,-28.0,M,17.1,,*4B\n";
String target ="";
String sendPacket="";
TinyGPS gps;
SoftwareSerial ss(10, 11);

float  latitude;
float longitude;
void deCode(String s);

void setup()
{
  //init USB serial communication
  Serial.begin(9600);
   while (!Serial);

  //init LoRa model
   if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }else{
     Serial.println("Lora OK");  
  }
  
   //init GPS serial communication
  ss.begin(9600);
  
  delay(1000);
   
}

void loop()
{
  bool newData = false;
  unsigned long chars;
  unsigned short sentences, failed;

  // For one second we parse GPS data and report some key values
  for (unsigned long start = millis(); millis() - start < 1000;)
  {
    while (ss.available())
    {
      char c = ss.read();
      //Serial.write(c); // uncomment this line if you want to see the GPS data flowing
      target+=c;
     if(c=='\n'){
        if(target.indexOf("GPGGA")!=-1){
          deCode(target);  
        }
        target ="";

        if(longitude!=0 && longitude<180 && longitude >-180 && latitude!=0){
        sendPacket +="SOS:";
        sendPacket+=ID;
        sendPacket+=",";
        sendPacket+=String(longitude,5);
        sendPacket+=",";
        sendPacket+=String(latitude,5);
        sendPacket+="\n";
          
        Serial.println( sendPacket);

        LoRa.beginPacket();
        LoRa.print(sendPacket);
        LoRa.endPacket();
        }
        sendPacket="";
      }
      
    }
  }
  delay(1000);
}


void deCode(String s)
{  
  int startPoint;
   int endPoint; 
   String out="";

    
   //get latitude point
    startPoint = s.indexOf(",");
    startPoint = s.indexOf(",",startPoint+1);
    endPoint = s.indexOf(",",startPoint+1);

    //get latitude data
    for(int i=startPoint+1;i<endPoint;i++){
       out += s[i]; 
     }

     latitude = out.toFloat();
     latitude = latitude/100;
      
#ifdef DEBUG     
     Serial.println(out);
#endif
     out="";
     
     startPoint = s.indexOf(",",startPoint+1);
     endPoint = s.indexOf(",",startPoint+1);

      //get latitude direction
      for(int i=startPoint+1;i<endPoint;i++){
       out += s[i]; 
     }

     if(out[0]=='S'){
        latitude = -latitude;
      }

#ifdef DEBUG  
     Serial.println(out);
 #endif
     out="";
   
    startPoint = s.indexOf(",",startPoint+1);
    endPoint = s.indexOf(",",startPoint+1);

      //get longitude data
      for(int i=startPoint+1;i<endPoint;i++){
       out += s[i]; 
     }

    longitude = out.toFloat();
    longitude = longitude/100;
     
 #ifdef DEBUG  
     Serial.println(out);
 #endif
    out="";

     startPoint = s.indexOf(",",startPoint+1);
     endPoint = s.indexOf(",",startPoint+1);

     //get longitude direction
      for(int i=startPoint+1;i<endPoint;i++){
       out += s[i]; 
     }

      if(out[0]=='S'){
        longitude = -longitude;
      }
     
 #ifdef DEBUG  
     Serial.println(out);
 #endif
   out="";
 
}
