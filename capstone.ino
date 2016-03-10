#include <SoftwareSerial.h>

#include <OneWire.h>
#include <DallasTemperature.h>

// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 2
// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);
// arrays to hold device address
DeviceAddress insideThermometer;

#define LED 13        //LED Control

#define RXBT 10       //Bluetooth Serial
#define TXBT 11       //Bluetooth Serial

SoftwareSerial bluetooth(RXBT, TXBT);

int currentTemperature = 0;
int temperatureSetting = 0;

String val = "";

void setup() {
  pinMode(LED, OUTPUT);
  digitalWrite(LED,LOW);
  
  Serial.begin(9600);
  bluetooth.begin(9600);
  
  sensors.begin();
   // Search for devices on the bus and assign based on an index. Ideally,
  // you would do this to initially discover addresses on the bus and then 
  // use those addresses and manually assign them (see above) once you know 
  // the devices on your bus (and assuming they don't change).
  if (!sensors.getAddress(insideThermometer, 0)) Serial.println("Unable to find address for Device 0"); 
  sensors.setResolution(insideThermometer, 9);
 
}

void loop() {
  
  //measureTH();
  readBluetooth();
  
}

void readBluetooth(){
  val = "";
  while(bluetooth.available() > 0) {      // if data is available to read
    val = val + bluetooth.read();       // read it and store it in 'val'
  }
  if (val != ""){
  Serial.print("Bluetooth Data Received: ");
  Serial.println(val);
  if (val.substring(0,6) == "766968") { //LED in ASCII: 76-69-68
    //LED Command
    Serial.println("LED COMMAND!");
    processLED();
  }
  else if (val.substring(0,8) == "84697780") { //TEMP in ASCII: 84-69-77-80
    //Temperature Command
    Serial.println("TEMP COMMAND!");
   processTH();   
  }
  else{
    //unknown device command
  }
  }
}

void processLED() { //process a command for the LED
  if( val.substring(8,12) == "7978" ) {              // ON in ASCII: 79-78
    digitalWrite(LED, HIGH);  // turn ON the LED on pin 13
    Serial.println("ON");
  } 
  else if (val.substring(8,14) == "797070"){        // OFF in ASCII: 79-70-70
    digitalWrite(LED, LOW);   // turn OFF the LED on pin 13
    Serial.println("OFF");
  }
  else{
    //unknown LED action  
  }
}

void processTH() {
  if( val.substring(10,16) == "836984" ) {              // SET: 83-69-84
    Serial.println("SET");
    Serial.print("Old Temp: ");
    Serial.println(temperatureSetting);
    int newTemp = 0;
    int tens = val.substring(18,20).toInt();
    int ones = val.substring(20,22).toInt();
    for (int x=0; x<10; x++){
      if (tens == 48 + x){
        newTemp += x * 10;
      }
      if (ones == 48 + x){
        newTemp += x;
      }
    }
      
    setTH(newTemp); // set temperature to XX
    Serial.print("New Temp: ");
    Serial.println(temperatureSetting);
  } 
  else if (val.substring(10,16) == "716984"){        // GET in ASCII: 71-69-84
    Serial.println("GET");
    Serial.println(getTH());
    //bluetooth.write(getTH()); //send current temp/humidity data
  }
  else{
    //unknown temperature action 
  }
}

float getTH(){
  return currentTemperature;
}

void setTH(int newTemp) {
  temperatureSetting = newTemp;
}

void measureTH(){
  sensors.requestTemperatures(); // Send the command to get temperatures
  float tempC = sensors.getTempC(insideThermometer);
  currentTemperature = round((DallasTemperature::toFahrenheit(tempC))); // Converts tempC to Fahrenheit
}

void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    if (deviceAddress[i] < 16) Serial.print("0");
    //Serial.print(deviceAddress[i], HEX);
  }
}
