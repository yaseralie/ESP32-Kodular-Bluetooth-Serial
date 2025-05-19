/*  using ESP8266 to connect to Bluetooth WEMOS® D1 R32 W/ ESP32 UNO R3 PINOUT
 *  to select your board follow these steps.
    Tools → boards → scroll down to find ESP32 Dev Module → select the port
    Reference GPIO  https://www.botnroll.com/en/esp32/3639-wemos-d1-r32-w-esp32-uno-r3-pinout.html
    When uploading, don't connect pin TX and RX (GPIO16 and 17)
*/

#include <BluetoothSerial.h>

#include <DHT.h>
#define DHTPIN 26  //GPIO2 atau D4
// Uncomment the type of sensor in use:
//#define DHTTYPE    DHT11     // DHT 11
//#define DHTTYPE    DHT11     // DHT 22 (AM2302)
#define DHTTYPE    DHT21     // DHT 21 (AM2301)
DHT dht(DHTPIN, DHTTYPE);

// Pengaturan Bluetooth
BluetoothSerial SerialBT;

// current temperature & humidity, updated in loop()
float t = 0.0;
float h = 0.0;

//pin output
int red_pin = 25;
int yellow_pin = 27;
int green_pin = 14;

//Incoming Serial Data Array
const byte channel = 3;
char* arr[channel];

//millis================================
//Set every 5 sec read DHT
unsigned long previousMillis = 0;  // variable to store the last time the task was run
const long interval = 5000;        // time interval in milliseconds (eg 1000ms = 1 second)
//======================================

void setup() {
  Serial.begin(9600);
  SerialBT.begin("ESP32"); // Nama bluetooth
  delay(500);
  pinMode(red_pin, OUTPUT);
  pinMode(yellow_pin, OUTPUT);
  pinMode(green_pin, OUTPUT);

  Serial.println("Sensor Suhu dan Kelembaban Udara DHT11, dan kirim data ke Bluetooth");
  dht.begin();
  delay(1000);
}

void loop() {
  unsigned long currentMillis = millis();  // mendapatkan waktu sekarang
  // Checks whether it is time to run the task
  if (currentMillis - previousMillis >= interval) {
    // Save the last time the task was run
    previousMillis = currentMillis;

    t = dht.readTemperature();
    h = dht.readHumidity();
    Serial.print(t);
    Serial.print(",");
    Serial.print(h);
    Serial.println();

    //Send to Bluetooth
    SerialBT.print(t);
    SerialBT.print(",");
    SerialBT.print(h);
    SerialBT.println();
  }
  // Read bluetooth
  processIncomingSerial();
}

// INCOMING SERIAL DATA PROCESSING CODE----------------------------------------
void processIncomingSerial()
{
  if (SerialBT.available()) {
    parseData(GetSerialData());
    //Get data from serial, format: red_value,yellow_value, green_value. ex:0,0,1
    int red = atoi(arr[0]);
    int yellow = atoi(arr[1]);
    int green = atoi(arr[2]);

    //turn ON/OFF lamp
    digitalWrite(red_pin, red);
    digitalWrite(yellow_pin, yellow);
    digitalWrite(green_pin, green);
  }
}

// Gathers bytes from serial port to build inputString
//char* GetSerialData()
char* GetSerialData()
{
  static char inputString[64]; // Create a char array to store incoming data
  memset(inputString, 0, sizeof(inputString)); // Clear the memory from a pervious reading
  while (SerialBT.available()) {
    SerialBT.readBytesUntil('\n', inputString, 64); //Read every byte in Serial buffer until line end or 64 bytes
  }
  return inputString;
}

// Seperate the data at each delimeter
void parseData(char data[])
{
  char *token = strtok(data, ","); // Find the first delimeter and return the token before it
  int index = 0; // Index to track storage in the array
  while (token != NULL) { // Char* strings terminate w/ a Null character. We'll keep running the command until we hit it
    arr[index] = token; // Assign the token to an array
    token = strtok(NULL, ","); // Conintue to the next delimeter
    index++; // incremenet index to store next value
  }
}
