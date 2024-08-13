/* Bibliotecas */
#include <Wire.h>
#include <ArduinoHA.h>
#include <ESP8266WiFi.h>
#include <Adafruit_ADS1X15.h>
/* Definições */
#define BROKER_ADDR IPAddress(192,168,16,1)
#define BROKER_USERNAME  "vitor" 
#define BROKER_PASSWORD  "C0n$n3t3"
#define Intp 0
#define clk_74h 2
#define SRCLK_74H 15
#define Sel_Anl_A 13
#define Sel_Anl_B 14
#define Multp 12


/* Declarações */
HADevice device;
WiFiClient espClient;
HAMqtt mqtt(espClient, device);
const char* ssid = "consnete";
const char* senha = "cons1490";
HASensorNumber LUM_sen("Luminosidade");
HASensorNumber NHT("Temperatura");
HABinarySensor Pr_Sn("Presenca");
HASwitch Lamp_1("Luz_1");
HASwitch Lamp_2("Luz_2");
HASwitch Lamp_3("Luz_3");
HASwitch Lamp_4("Luz_4");
IPAddress local_IP(192, 168, 16, 180);
IPAddress gateway(192, 168, 16, 1);
IPAddress subnet(255, 255, 0, 0);
Adafruit_ADS1115 ads;
long temp_ant = -3 ;
float t_ant = 0;
float temp = 0;
long timer = millis() /  1000;
int t_at = 0;
bool lamp_1_st = false;
bool lamp_2_st = false;
bool lamp_3_st = false;
bool lamp_4_st = false;
bool intp_1_st;
bool intp_2_st;
bool intp_3_st;
bool intp_4_st;
int Lum = 0;
bool Pir_ST = false;

void WF_Begin() {
if (!WiFi.config(local_IP, gateway, subnet)) {
    Serial.println("STA Failed to configure");
  }
byte mac[WL_MAC_ADDR_LENGTH];
WiFi.macAddress(mac);
device.setUniqueId(mac, sizeof(mac));
WiFi.begin(ssid, senha);
  Serial.print("Conectando");
 while (WiFi.status() != WL_CONNECTED){
    delay(300);
    Serial.print(".");
  }
Serial.println("");
Serial.println("WiFi Conectada");
Serial.println(WiFi.localIP());

}

void onMqttConnected() {
    Serial.println("Connected to the broker!");
}

void onMqttDisconnected() {
    Serial.println("Disconnected from the broker!");
}

void onMqttStateChanged(HAMqtt::ConnectionState state) {
    Serial.print("MQTT state changed to: ");
    Serial.println(static_cast<int8_t>(state));
}

void Pins_MD(){
  pinMode(Sel_Anl_A, OUTPUT);
  pinMode(Sel_Anl_B, OUTPUT);
  pinMode(Multp, INPUT);
  pinMode(Intp, INPUT);
  pinMode(SRCLK_74H, OUTPUT);  
  pinMode(clk_74h, OUTPUT);
  Serial.println("Pins Sucesso"); 
}

void HAOS_setup() {
device.setName("Central Bruno");
device.setSoftwareVersion("1.0.0");

LUM_sen.setIcon("mdi:lightbulb-on-50");
LUM_sen.setName("Luminosidade");
LUM_sen.setUnitOfMeasurement("lm");

NHT.setIcon("mdi:thermometer-lines");
NHT.setName("Temperatura");
NHT.setUnitOfMeasurement("C");

Pr_Sn.setIcon("mdi:account");
Pr_Sn.setName("Presença");
Pr_Sn.setCurrentState(Pir_ST);

Lamp_1.setIcon("mdi:lightbulb-outline");
Lamp_1.setName("Luz 1");
Lamp_1.onCommand(onSwitchCommand1);
Lamp_1.setCurrentState(lamp_1_st);

Lamp_2.setIcon("mdi:lightbulb-outline");
Lamp_2.setName("Luz 2");
Lamp_2.onCommand(onSwitchCommand2);
Lamp_2.setCurrentState(lamp_2_st);

Lamp_3.setIcon("mdi:lightbulb-outline");
Lamp_3.setName("Luz 3");
Lamp_3.onCommand(onSwitchCommand3);
Lamp_3.setCurrentState(lamp_3_st);

Lamp_4.setIcon("mdi:lightbulb-outline");
Lamp_4.setName("Luz 4");
Lamp_4.onCommand(onSwitchCommand4);
Lamp_4.setCurrentState(lamp_4_st);

Serial.println("Haos Setup Sucesso");
}

void MQTT_START(){
  mqtt.onConnected(onMqttConnected);
  mqtt.onDisconnected(onMqttDisconnected);
  mqtt.begin(BROKER_ADDR, BROKER_USERNAME, BROKER_PASSWORD);
  Serial.println("MQTT Inicializado");
}

void ADS_BEGIN(){
  if (!ads.begin())
  {
    Serial.println("Failed to initialize ADS.");
    while (1);
  }
 Serial.println("ads Iniciado");
}
void setup() {
Serial.begin(115200);
WF_Begin();
MQTT_START();
HAOS_setup();
ADS_BEGIN();
Pins_MD();
Serial.println("Init...."); 
}

void Lamps(int L){
  switch (L){
    case 1:
    break;
    case 2:
    break;
    case 3:
    break;
    case 4:
    break;
  }
}

void onSwitchCommand1(bool state, HASwitch* sender){
  Lamps(1);
  sender->setState(state);
}

void onSwitchCommand2(bool state, HASwitch* sender){
  Lamps(2);
  sender->setState(state);
}

void onSwitchCommand3(bool state, HASwitch* sender){
  Lamps(3);
  sender->setState(state);
}

void onSwitchCommand4(bool state, HASwitch* sender){
  Lamps(4);
  sender->setState(state);
}

void buttons(){
//digitalWrite(Sel_Anl_A, HIGH);
//Sel_Anl_B
}

void Lum_Read(){

  Lum = ads.readADC_SingleEnded(1)* 0.00125;
  LUM_sen.setValue(Lum);
}

void NHT_Read(){
    float R1 = 10000;
    float logR2, R2, T, Tc, Tf;
    float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;
    int Vo;
    Vo = ads.readADC_SingleEnded(0); // Read ADC value from ADS1115
  R2 = R1 * (1023.0 / (float)Vo - 1.0);
  logR2 = log(R2);
  T = (1.0 / (c1 + c2*logR2 + c3*logR2*logR2*logR2));
  Tc = T - 273.15;

    Vo = ads.readADC_SingleEnded(0); // Read ADC value from ADS1115
    temp = adcVal * ads_bit_Voltage;     // Using default TwoThirds gain = 0.1875mV per 1-bit
    t_at = temp / lm35_constant;; 
    NHT.setValue(t_at);
}

void Pir_Read(){
   int tempp = ads.readADC_SingleEnded(3);
   if (tempp >= 1000){
    Pir_ST = true;
   }else{ Pir_ST = false;}  
   Pr_Sn.setState(Pir_ST);
}

void loop() {
 mqtt.loop();
 //buttons();
 timer = millis() /  1000;
   if (timer > (temp_ant + 1)){
   temp_ant = timer;
   NHT_Read();
   Lum_Read();
   Pir_Read();
   
   }
}
