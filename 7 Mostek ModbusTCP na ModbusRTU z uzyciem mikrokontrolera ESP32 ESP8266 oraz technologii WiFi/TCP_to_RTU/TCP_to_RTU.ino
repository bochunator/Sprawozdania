 #if defined(ESP8266)
 #include <ESP8266WiFi.h>
 #include <SoftwareSerial.h>
 SoftwareSerial S(4, 5);
 #define MAX485_DE_RE 14
#elif defined(ESP32)
 #include <WiFi.h>
 #define RXD2 16
 #define TXD2 17
 #define MAX485_DE_RE 4
#endif
#include <ModbusTCP.h>
#include <ModbusRTU.h>

ModbusRTU rtu;
ModbusTCP tcp;

IPAddress srcIp;

uint16_t transRunning = 0;
uint8_t slaveRunning = 0;
unsigned long savedTime = 0;

bool cbRtuTrans(Modbus::ResultCode event, uint16_t transactionId, void* data) {
    if (event != Modbus::EX_SUCCESS)
      Serial.printf("Modbus result: %02X, Mem: %d\n", event, ESP.getFreeHeap());
    return true;
}

Modbus::ResultCode cbTcpRaw(uint8_t* data, uint8_t len, void* custom) {
  auto src = (Modbus::frame_arg_t*) custom;
  Serial.print("TCP IP in - ");
  Serial.print(IPAddress(src->ipaddr));
  Serial.printf(" Fn: %02X, len: %d \n\r", data[0], len);
  if (transRunning) {
    tcp.setTransactionId(src->transactionId);
    tcp.errorResponce(IPAddress(src->ipaddr), (Modbus::FunctionCode)data[0], Modbus::EX_SLAVE_DEVICE_BUSY);
    return Modbus::EX_SLAVE_DEVICE_BUSY;
  }
  rtu.rawRequest(src->unitId, data, len, cbRtuTrans);
  if (!src->unitId) {
    tcp.setTransactionId(src->transactionId);
    tcp.errorResponce(IPAddress(src->ipaddr), (Modbus::FunctionCode)data[0], Modbus::EX_ACKNOWLEDGE);
    transRunning = 0;
    slaveRunning = 0;
    return Modbus::EX_ACKNOWLEDGE;
  }
  srcIp = IPAddress(src->ipaddr);
  slaveRunning = src->unitId;
  transRunning = src->transactionId;
  Serial.print("slaveRunning = ");
  Serial.println(slaveRunning);
  Serial.print("transRunning = ");
  Serial.println(transRunning);
  return Modbus::EX_SUCCESS;
}

Modbus::ResultCode cbRtuRaw(uint8_t* data, uint8_t len, void* custom) {
  auto src = (Modbus::frame_arg_t*) custom;
  if (!transRunning)
      return Modbus::EX_PASSTHROUGH;
  tcp.setTransactionId(transRunning);
  uint16_t succeed = tcp.rawResponce(srcIp, data, len, slaveRunning);
  if (!succeed){
    Serial.print("TCP IP out - failed");
  }
  Serial.printf("RTU Slave: %d, Fn: %02X, len: %d, ", src->slaveId, data[0], len);
  Serial.print("Response TCP IP: ");
  Serial.println(srcIp);
  transRunning = 0;
  slaveRunning = 0;
  return Modbus::EX_PASSTHROUGH;
}

void setup() {
  #if defined(ESP8266)
    S.begin(9600, SWSERIAL_8N1);
    rtu.begin(&S, MAX485_DE_RE);
  #elif defined(ESP32)
    Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
    rtu.begin(&Serial2, MAX485_DE_RE);
  #endif
  Serial.begin(9600, SERIAL_8N1);
  WiFi.softAP("KoniowifiESP32");
  WiFi.begin("Koniowifi", "soniasonia");
  tcp.server();
  tcp.onRaw(cbTcpRaw);
  rtu.master();
  rtu.onRaw(cbRtuRaw);
}

void loop() {
  if (savedTime < millis()/10000) {
    savedTime = millis()/10000;
    Serial.print("WiFi.localIP() = ");
    Serial.println(WiFi.localIP());
  }
  rtu.task();
  tcp.task();
  yield();
}
