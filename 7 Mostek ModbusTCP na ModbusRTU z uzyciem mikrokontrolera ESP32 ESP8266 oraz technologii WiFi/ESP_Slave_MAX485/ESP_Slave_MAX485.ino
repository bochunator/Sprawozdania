#include <ModbusRTU.h>

#if defined(ESP8266)
 #include <SoftwareSerial.h>
 SoftwareSerial S(4, 5);
 #define MAX485_DE_RE 14
#elif defined(ESP32)
 #define RXD2 16
 #define TXD2 17
 #define MAX485_DE_RE 4
#endif

#define SLAVE_ID 2
#define LED 2

ModbusRTU rtu;

void setup() {
  // put your setup code here, to run once:
  pinMode(LED, OUTPUT);
  #if defined(ESP8266)
    S.begin(9600, SWSERIAL_8N1);
    rtu.begin(&S, MAX485_DE_RE);
  #elif defined(ESP32)
    Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
    rtu.begin(&Serial2, MAX485_DE_RE);
  #endif
  rtu.server(SLAVE_ID);
  for (uint16_t i = 0; i < 100; i++) {
    rtu.addHreg(i, i*100 + SLAVE_ID);
  }
  rtu.addHreg(100, 0);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (rtu.Hreg(100) != 0) {
    digitalWrite(LED, HIGH);
  } else {
    digitalWrite(LED, LOW);
  }
  rtu.task();
}
