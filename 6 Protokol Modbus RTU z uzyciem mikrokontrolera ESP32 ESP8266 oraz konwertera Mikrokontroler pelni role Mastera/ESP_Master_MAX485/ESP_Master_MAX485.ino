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

#define SLAVE_ID 1
#define ADDRESS_READ_REGISTER 0
#define ADDRESS_WRITE_REGISTER 1
#define LED 2

ModbusRTU rtu;

uint16_t read_registers[1];
uint16_t write_register = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(LED, OUTPUT);
  Serial.begin(9600);
  #if defined(ESP8266)
    S.begin(9600, SWSERIAL_8N1);
    rtu.begin(&S, MAX485_DE_RE);
  #elif defined(ESP32)
    Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
    rtu.begin(&Serial2, MAX485_DE_RE);
  #endif
  rtu.master();
}

void loop() {
  // put your main code here, to run repeatedly:
  if (!rtu.slave()) {
    rtu.readHreg(SLAVE_ID, ADDRESS_READ_REGISTER, read_registers, 1);
    while(rtu.slave()) {
      rtu.task();
      delay(10);
    }
    Serial.print("Odczytana wartosc rejestru o adresie 0 to = ");
    Serial.println(read_registers[0]);
    if (read_registers[0] != 0) {
      digitalWrite(LED, HIGH);
    } else {
      digitalWrite(LED, LOW);
    }
  }
  delay(1000);
  if (!rtu.slave()) {
    write_register++;
    rtu.writeHreg(SLAVE_ID, ADDRESS_WRITE_REGISTER, write_register);
    while(rtu.slave()) {
      rtu.task();
      delay(10);
    }
    Serial.print("Wpisana wartosc do rejestru o adresie 1 to = ");
    Serial.println(write_register);
  }
  delay(1000);
}
