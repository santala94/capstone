#include <SoftwareSerial.h>
   #define SSID "106EPark"
   #define PASS "Adambrucecos"
   #define DST_IP "141.101.112.175" //adafruit.com
   SoftwareSerial esp(6, 5); // RX, TX
   void setup()
   {
     //setup RGB LED as indicator instead of softserial
     pinMode(13, OUTPUT);
     digitalWrite(13, HIGH);

     // Open serial communications and wait for port to open:
     esp.begin(115200);
     esp.setTimeout(5000);
     Serial.begin(9600); //can't be faster than 19200 for softserial
     Serial.println("ESP8266 Demo");
     //test if the module is ready
     esp.println("AT+RST");
     delay(1000);
     int in = esp.read();
     Serial.print(in);
     while (in != -1){
       in = esp.read();
       Serial.print(in);
     }
     esp.flush();
     if(esp.find("AT+RST"))
     {
       Serial.println("Module is ready");
       //blink green 1sec
       digitalWrite(13, LOW);
       delay(1000);
       digitalWrite(13, HIGH);
      }
     else
     {
       Serial.println("Module have no response.");
       //while(1);
     }
     delay(1000);
     //connect to the wifi
     boolean connected=false;
     for(int i=0;i<5;i++)
     {
       if(connectWiFi())
       {
         connected = true;
       //blink blue 1sec
       digitalWrite(13, LOW);
       delay(1000);
       digitalWrite(13, HIGH);
         break;
       }
     }
     if (!connected){while(1);}
     delay(5000);
     //print the ip addr
     /*Serial.println("AT+CIFSR");
     dbgSerial.println("ip address:");
     while (Serial.available())
     dbgSerial.write(Serial.read());*/
     //set the single connection mode
     Serial.println("AT+CIPMUX=0");
   }
   void loop()
   {
     String cmd = "AT+CIPSTART=\"TCP\",\"";
     cmd += DST_IP;
     cmd += "\",80";
     esp.println(cmd);
     Serial.println(cmd);
     if(esp.find("Error")) return;
     cmd = "GET / HTTP/1.0\r\n\r\n";
     esp.print("AT+CIPSEND=");
     esp.println(cmd.length());
     if(esp.find(">"))
     {
       Serial.print(">");
       }else
       {
         esp.println("AT+CIPCLOSE");
         Serial.println("connect timeout");
         delay(1000);
         return;
       }
       esp.print(cmd);
       delay(2000);
       //Serial.find("+IPD");
       while (esp.available())
       {
         char c = esp.read();
         Serial.write(c);
         if(c=='\r') Serial.print('\n');
       }
       Serial.println("====");
       delay(1000);
     }
     boolean connectWiFi(){
       esp.println("AT+CWMODE=1");
       String cmd="AT+CWJAP=\"";
       cmd+=SSID;
       cmd+="\",\"";
       cmd+=PASS;
       cmd+="\"";
       Serial.println(cmd);
       esp.println(cmd);
       delay(2000);
       if(esp.find("OK")){
         Serial.println("OK, Connected to WiFi.");
         return true;
       }
       else{
           Serial.println("Can not connect to the WiFi.");
           return false;
       }
     }
