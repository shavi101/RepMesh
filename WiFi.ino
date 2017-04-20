#include <Adafruit_ESP8266.h>
#include <SoftwareSerial.h>
#include <Wire.h>
String request="GET";

#define ESP_RX   9//3
#define ESP_TX   10//4
#define ESP_RST  8
SoftwareSerial softser(ESP_RX, ESP_TX);
//********
Stream *esp = &softser;

// Must declare output stream before Adafruit_ESP8266 constructor; can be
// a SoftwareSerial stream, or Serial/Serial1/etc. for UART.
Adafruit_ESP8266 wifi(&softser, &Serial, ESP_RST);
// Must call begin() on the stream(s) before using Adafruit_ESP8266 object.

#define ESP_SSID "Latiff Residence"//"mesh"//"Dialog 4G" // Your network name here
#define ESP_PASS "commandos"//"karthiWED123"//"GBGGQMT6AB5" // Your network password here
//.....................................................................................
#define REPLYBUFFSIZ 255
char replybuffer[REPLYBUFFSIZ];
uint8_t getReply(char const *send, uint16_t timeout = 500, boolean echo = true);
uint8_t espreadline(uint16_t timeout = 500, boolean multiline = false);//is an integer type that can store 16 bits (2 bytes atleast)
boolean sendCheckReply(char const *send, char const *reply, uint16_t timeout = 500);
enum {WIFI_ERROR_NONE=0, WIFI_ERROR_AT, WIFI_ERROR_RST, WIFI_ERROR_SSIDPWD, WIFI_ERROR_SERVER, WIFI_ERROR_UNKNOWN};
//.......................................................................................
#define HOST     "api.pushingbox.com"      // Find/Google your email provider's SMTP outgoing server name for unencrypted email

#define PORT     80                    // Find/Google your email provider's SMTP outgoing port for unencrypted email

int count = 0; // we'll use this int to keep track of which command we need to send next
bool send_flag = false; // we'll use this flag to know when to send the email commands

void setup() {
  char buffer[50];
  delay(2000);
Wire.begin();        // join i2c bus (address optional for master)
  Serial.begin(57600); 
  Wire.requestFrom(8, 6);    // request 6 bytes from slave device #8
  while (Wire.available()) { // slave may send less than requested
   char c = Wire.read(); // receive a byte as character
    //Serial.print(c);  
    if(c=='p'){
      Serial.println("ID-0005792778 Scanned"); 
      request+= " /pushingbox?devid=vCB2DD101A50CEFD&status=ID-0005792778-DEFINICILS_WATERPROOF(High_Definition_Mascara)-$27.50&submit=Submit";
      break;
    }
    else if(c=='q') {
      Serial.println("ID-0007230545 Scanned"); 
    request+= " /pushingbox?devid=vCB2DD101A50CEFD&status=ID-0007230545-TEINT_IDOLE_ULTRA_LONGWEAR_FOUNDATION_STICK(210_BUFF_N-$42.00)&submit=Submit";
    break;
    }
    else
    request+="No data received"; 
    
   // print the character
  }// start serial for output
  
  // This might work with other firmware versions (no guarantees)
  // by providing a string to ID the tail end of the boot message:
 
  // comment/replace this if you are using something other than v 0.9.2.4!
  wifi.setBootMarker(F("Version:0.9.2.4]\r\n\r\nready"));

  softser.begin(9600); // Soft serial connection to ESP8266
  // while(!Serial); // UART serial debug
  
Serial.println(F("Adafruit ESP8266 Email"));//You can pass flash-memory(non-volatile arduino memory) based strings to Serial.print() by wrapping them with F(). 

  // Test if module is ready
  Serial.print(F("Hard reset..."));
  if(!wifi.hardReset()) {
    Serial.println(F("no response from module."));
    for(;;);
  }
  Serial.println(F("OK."));

  Serial.print(F("Soft reset..."));
  if(!wifi.softReset()) {
    Serial.println(F("no response from module."));
    for(;;);
  }
  Serial.println(F("OK."));

  Serial.print(F("Checking firmware version..."));
  wifi.println(F("AT+GMR"));
  if(wifi.readLine(buffer, sizeof(buffer))) {
    Serial.println(buffer);
    wifi.find(); // Discard the 'OK' that follows
  } else {
    Serial.println(F("error"));
  }

  Serial.print(F("Connecting to WiFi..."));
  if(wifi.connectToAP(F(ESP_SSID), F(ESP_PASS))) {

    // IP addr check isn't part of library yet, but
    // we can manually request and place in a string.
    Serial.print(F("OK\nChecking IP addr..."));
    wifi.println(F("AT+CIFSR"));
    if(wifi.readLine(buffer, sizeof(buffer))) {
        Serial.println(buffer);
        wifi.find(); // Discard the 'OK' that follows

        Serial.print(F("Connecting to host..."));

        Serial.print("Connected..");
        wifi.println("AT+CIPMUX=0"); // configure for single connection, 
                                     //we should only be connected to one SMTP server
        wifi.find();
        wifi.closeTCP(); // close any open TCP connections
        wifi.find();
        Serial.println("Type \"send it\" to send an email");
        
    } else { // IP addr check failed
      Serial.println(F("error"));
    }
  } else { // WiFi connection failed
    Serial.println(F("FAIL"));
  }



  // put your main code here, to run repeatedly:
 Serial.println("Connecting...");
        uint32_t ip = getIP();
  Serial.print("ESP setup success, my IP addr:");
  if (ip) {
    Serial.println(ip, HEX);
  } else {
    Serial.println("none");
  }

  sendCheckReply("AT+CIPSTO=0", "OK");
   delay(1000);

}
void loop()
{
    delay(25);
    
  ESP_GETpage(HOST, 80);
  delay(2000);
  
  Serial.println(F("**********REPLY***********"));
  Serial.print(replybuffer);
  Serial.println(F("**************************"));

  sendCheckReply("AT+CIPCLOSE", "OK");
  
  debugLoop();
  
  delay(1000);
  
  while (1);
}

boolean ESP_GETpage(char const *host, uint16_t port) {//,char const *page
  String cmd = "AT+CIPSTART=\"TCP\",\"";//Establish TCP connection or register UDP port and start a connection
 
  cmd += host;
  cmd += "\",";
  cmd += port;
  cmd.toCharArray(replybuffer, REPLYBUFFSIZ);
 delay(1000);
  getReply(replybuffer);

  if (strcmp(replybuffer, "OK") != 0) {
     // this is OK! could be a version that says "Linked"
     if (strcmp(replybuffer, "Linked") != 0) {
       sendCheckReply("AT+CIPCLOSE", "OK");
        delay(1000);
       return false;
     }
  }

//  sprintf(postmsg,"GET /pushingbox?devid=%c&status=%d HTTP/1.1",devid,avtemp);
  
 // request += page;
  request += " HTTP/1.1\r\nHost: ";
  request += host;
  request += "\r\n\r\n";
  
  cmd = "AT+CIPSEND=";
  cmd += request.length();
  cmd.toCharArray(replybuffer, REPLYBUFFSIZ);
  sendCheckReply(replybuffer, ">");

  Serial.print("Sending: "); Serial.println(request.length());
  Serial.println(F("*********SENDING*********"));
  Serial.print(request);
  Serial.println(F("*************************"));
  
  request.toCharArray(replybuffer, REPLYBUFFSIZ);

  esp->println(request);

  while (true) {
    espreadline(3000);  // this is the 'echo' from the data
    Serial.print(">"); Serial.println(replybuffer); // probably the 'busy s...'

    // LOOK AT ALL THESE POSSIBLE ARBITRARY RESPONSES!!!
    if (strstr(replybuffer, "wrong syntax")) 
      continue;
    else if (strstr(replybuffer, "ERROR")) 
      continue;
    else if (strstr(replybuffer, "busy s...")) 
      continue;
    else break;
  }
  
  if (! strstr(replybuffer, "SEND OK") ) return false;
  
  espreadline(1000);  Serial.print("3>"); Serial.println(replybuffer);
  char *s = strstr(replybuffer, "+IPD,");
  if (!s) return false;
  uint16_t len = atoi(s+5);
  //Serial.print(len); Serial.println(" bytes total");

  int16_t contentlen = 0;
  while (1) {
    espreadline(50);
    s = strstr(replybuffer, "Content-Length: ");
    if (s) {
      //Serial.println(replybuffer);
      contentlen = atoi(s+16);
      Serial.print(F("C-Len = ")); Serial.println(contentlen);
    }
    s = strstr(replybuffer, "Content-Type: ");
    if (s && contentlen) {
      int16_t i;
      char c;
      
      for (i=-2; i<contentlen; i++) {  // eat the first 2 chars (\n\r)
        while (!esp->available());
        c = esp->read(); //UDR0 = c;
        if (i >= 0) {
          replybuffer[i] = c;
        }
      }
      replybuffer[i] = 0;
      return true;
    }
  }
  //while (1) {
  //  if (esp.available()) UDR0 = esp.read();
  //} 
}
         
         
uint32_t getIP() {
  getReply("AT+CIFSR", 500, true);

  return 0;
}




/************************/
uint8_t espreadline(uint16_t timeout, boolean multiline) {
  uint16_t replyidx = 0;
  
  while (timeout--) {
    if (replyidx > REPLYBUFFSIZ-1) break;
    
    while(esp->available()) {
      char c =  esp->read();
      if (c == '\r') continue;
      if (c == 0xA) {
        if (replyidx == 0)   // the first 0x0A is ignored
          continue;
        
        if (!multiline) {
          timeout = 0;         // the second 0x0A is the end of the line
          break;
        }
      }
      replybuffer[replyidx] = c;
      // Serial.print(c, HEX); Serial.print("#"); Serial.println(c);
      replyidx++;
    }
    
    if (timeout == 0) break;
    delay(1);
  }
  replybuffer[replyidx] = 0;  // null term
  return replyidx;
}

uint8_t getReply(char const*send, uint16_t timeout, boolean echo) {
  // flush input
  while(esp->available()) {
     esp->read();
  }
  
  if (echo) {
    Serial.print("---> "); Serial.println(send); 
  }
  esp->println(send);
  
  // eat first reply sentence (echo)
  uint8_t readlen = espreadline(timeout);  
  
  //Serial.print("echo? "); Serial.print(readlen); Serial.print(" vs "); Serial.println(strlen(send));
  
  if (strncmp(send, replybuffer, readlen) == 0) {
    // its an echo, read another line!
    readlen = espreadline();
  }
  
  if (echo) {
    Serial.print ("<--- "); Serial.println(replybuffer);
  }
  return readlen;
}

boolean sendCheckReply(char const *send, char const *reply, uint16_t timeout) {
   delay(1000);
  getReply(send, timeout, true);

  return (strcmp(replybuffer, reply) == 0);
}

void debugLoop() {
  Serial.println("========================");
  //serial loop mode for diag
  while(1) {
    if (Serial.available()) {
      esp->write(Serial.read());
      delay(10);
    }
    if (esp->available()) {
      Serial.write(esp->read());
      delay(10);
    }
  }
}
