#include <ModbusRTU.h>

#define SLAVE_ID 1
#define ADDRESS_READ_REGISTER 0
#define ADDRESS_WRITE_REGISTER 1
#define LED 2

ModbusRTU rtu;

uint16_t read_registers[1];
uint16_t write_register = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(LED,OUTPUT);
  Serial.begin(9600);
  rtu.begin(&Serial);
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
  }
  delay(1000);
}
