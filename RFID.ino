#include <LiquidCrystal.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include <SeeedRFIDLib.h>

#define RFID_RX_PIN 2
#define RFID_TX_PIN 3

LiquidCrystal lcd(12,11,4,5,6,7);

SeeedRFIDLib RFID(RFID_RX_PIN, RFID_TX_PIN);
RFIDTag tag;

void setup() {
     lcd.begin(16,2);
     Serial.begin(57600);
      lcd.print("  RepMesh  ");
  // join i2c bus with address #8
  // register event
}

void loop() {
  
  if(RFID.isIdAvailable()) {
    Wire.begin(8); 
    tag = RFID.readId();
    Wire.onRequest(requestEvent);
   }
}
// function that executes whenever data is requested by master
// this function is registered as an event, see setup()
void requestEvent() {
  if(tag.id==5792778){
  Wire.write("p");
  lcd.clear();
  lcd.print("ID5792778-DEFINICILS-$27.50");
 }
  else if(tag.id==7230545){
   Wire.write("q");
   lcd.clear();
   lcd.print("ID7230545-TEINT IDOLE-$42.00");
   }
   else {
   Wire.write("e");  }
  // respond with message of 6 bytes
 // Wire.endTransmission();
  // as expected by master
}
