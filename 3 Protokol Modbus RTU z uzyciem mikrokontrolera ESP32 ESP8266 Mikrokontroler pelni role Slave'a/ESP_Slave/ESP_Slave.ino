#include <ModbusRTU.h>

#define SLAVE_ID 1
#define LED 2

ModbusRTU rtu;

void setup() {
  // put your setup code here, to run once:
  pinMode(LED,OUTPUT);
  Serial.begin(9600);
  rtu.begin(&Serial);
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
