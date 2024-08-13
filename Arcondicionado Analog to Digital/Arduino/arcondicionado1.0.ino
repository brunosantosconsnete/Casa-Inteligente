

/* Portas e conexões
    d0=USB                           a0= disp 6
    d1=USB                           a1= disp 8
    d2=  dips 1                      a2= disp 9
    d3=  disp 2                      a3= disp 7
    d4=  disp 3                      a4= RTC SDA
    d5=  disp 4                      a5= RTC SCL
    d6= Led Amarelo + Relê Refri     a6= NHT Dados
    d7= Led Vermelho + Relê Vmin     a7= 
    d8= Led Verde + Relê Vmax
    d9= Led Branco + Relê Aquecimento
    d10= IRreciver
    d11=  disp 5
    d12= TX ESP01
    d13= RX ESP01
*/

// Declaração de Bibliotecas RTC
#include <RTClib.h>
RTC_DS1307 rtc;



// Declaração NTC
#include <Thermistor.h>
Thermistor Ttemp(A6);

// Declaração IR
#include <IRremote.h>
#define DECODE_JVC
#define IR_PIN 10

//Declaração portas
#define rl_refri 6
#define rl_vent1 7
#define rl_vent2 8
#define rl_aquec 9

//Biblioteca display
#include <DisplayLS200.h>
DisplayLS200 MeuDisplay;

// Bibliotecas Serial
#include <SoftwareSerial.h>
#define rxPin 13
#define txPin 12


//Declaração de Variaveis
bool clim_st = false; //Climatização on/off
bool vent_st = false; //Ventilação min/max
bool pw_st = false; //Power on-off
bool dp_on = true; //Display on-off
int t_alv = 23; //Temperatura Alvo
int t_at = 0;//Temperatura Atual
SoftwareSerial mySerial =  SoftwareSerial(rxPin, txPin);
char cmd_rv = 0;
const char STX = 2;
const char ETX = 3;
long temp_ant = -3000 ;
int t_ant = 0;
int temp = Ttemp.getTemp();
long rl_temp = 0 ;
bool MSB_pw = false;
bool MSB_p1 = false;
long MSB_inc = 0;
bool clk_pw = false;
DateTime tempo;
int timer = 0;
bool aquec_st = false;
bool refri_st = false;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("inicializando...");

  pinMode(rl_refri, OUTPUT);
  pinMode(rl_vent1, OUTPUT);
  pinMode(rl_vent2, OUTPUT);
  pinMode(rl_aquec, OUTPUT);
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);
  mySerial.begin(9600);

  Serial.println("inicializando IR...");
  IrReceiver.begin(IR_PIN);
  if ( !rtc.begin()) {                         //Se o RTC nao for inicializado, faz
    Serial.println("RTC NAO INICIALIZADO");    //Imprime o texto
  }
  Serial.println("inicializando display...");
  MeuDisplay.Begin(2,3,4,5,11,A0,A1,A2,A3);
  Serial.println("sucesso.");
  delay(500);
  dp_start();
}

void MSB_start()
{
  if (pw_st & clim_st){
  MSB_pw = !MSB_pw;
  envia_dados();
  Serial.println("Modo Sono Bom Iniciado");
  }
}
void dp_atu()
{
  if  (clk_pw) {
    String str;
    str="00"+String(((tempo.hour() * 100) + tempo.minute()));
    MeuDisplay.DisplayString(str.substring(str.length()-4));
    if (tempo.second() & 1){
      MeuDisplay.DDot(true);
    }
    else{
      MeuDisplay.DDot(false);
    }
  }
  else{
    char tmp_t_at[6];
    dtostrf((t_alv * 100) + t_at, 5, 2, tmp_t_at);
    MeuDisplay.DDot(false);
    MeuDisplay.DisplayString(tmp_t_at);
  }
}

void clock_pw()
{
  if (pw_st){
  clk_pw = !clk_pw;
  envia_dados();
//  Serial.println("Clock Power");
  }
}

void Buttons()
{
 int bt_prs;
 
 bt_prs = MeuDisplay.RefreshDisplay();
 if (bt_prs != 0)
  switch (bt_prs)
  {
    case 7 : // pw
      pw();
      break;
    case 8 : // vent
      vent();
      break;
    case 6 : // temp +
      T_P();
      break;
    case 3 : // temp -
      T_M();
      break;
    case 5 : // Modo sono bom
      MSB_start();
      break;
    case 4 : // clock
      clock_pw();
      break;
    case 2 : // dp_on
      power_dp();
      break;
    case 1 : // clim
      power_clim();
      break;
    default:
       break;
  }
}

void envia_dados() {
  char t_disp[12];
  char tmp =0;
  char rl = 0;
  if (pw_st)
    tmp=tmp+1;
  if (vent_st)
    tmp=tmp+2;
  if (clim_st)
    tmp=tmp+4;
  if (dp_on)
    tmp=tmp+8;
  if (clk_pw)
    tmp=tmp+16;
  if (MSB_pw)
    tmp=tmp+32;
  if (aquec_st)
    rl=rl+1;
  if (refri_st)
    rl=rl+2;  
  t_disp[0]=t_alv;
  t_disp[1]=t_at;
  t_disp[2]=tmp;
  t_disp[3]=rl;
  mySerial.write(t_disp, 4);
//  Serial.println(tmp);
  Serial.write(t_disp,4);
}

void T_S(int valor)
{
  t_alv = valor;
//  Serial.print("Temp Alterada para:");
//  Serial.println(t_alv);
}

void T_P() {
  if (t_alv < 30) {
    t_alv = t_alv + 1;
    envia_dados();
//    Serial.print("Temp Alterada para:");
//    Serial.println(t_alv);
  }
}

void T_M() {
  if (t_alv > 18) {
    t_alv = t_alv - 1;
    envia_dados();
    Serial.print("Temp Alterada para:");
    Serial.println(t_alv);
  }
}


void power_clim()
{
  if (pw_st){
  clim_st = !clim_st;
  envia_dados();
  MeuDisplay.Led1(!clim_st);
  //MeuDisplay.RefreshDisplay();  
  Serial.println("Climate press");
  }
}

void power_dp()
{
  if (pw_st){
  dp_on = !dp_on;
  envia_dados();
  Serial.println("Power Display press");
  }
}

void vent()
{
  if (pw_st){
  vent_st = !vent_st;
  envia_dados();
  Serial.println("Ventilação Alterada");
  }
}

void pw()
{
  if (pw_st){
  pw_st = false;
  dp_on = false;
  vent_st = false;
  MSB_pw = false;
  clim_st = false;
  digitalWrite (rl_vent2, LOW);
  digitalWrite (rl_vent1, LOW);
  digitalWrite(rl_refri, LOW);
  digitalWrite(rl_aquec, LOW);
  aquec_st = false;
  refri_st = false;
  dp_clean();  
  envia_dados();
  Serial.println("Power press");
  }
  else
  {
      digitalWrite (rl_vent1, HIGH);
    pw_st = true;
    dp_on = true;
    envia_dados(); 
  }
}
void dp_start()
{
  MeuDisplay.DisplayString("Inic");
}

void dp_clean()
{
  MeuDisplay.DisplayString("    ");
  MeuDisplay.DDot(false);
  MeuDisplay.Led1(false);
  MeuDisplay.Led2(false);
}

void Trata_dados(char c)
{
  //Serial.print("serial rec=> ");
  //Serial.print(c);
  if (c == STX)
    {
        cmd_rv = 0;
//        Serial.println(" STX");
        return;
    }
  if ( c == ETX)
    {
//     Serial.println(" ETX"); 
     if (cmd_rv > 31)
     {
//  Serial.print("TRATA");
  switch (cmd_rv) {
    case 'T':
      T_P();
      break;
    case 't':
      T_M();
      break;
    case 'i':
      envia_dados();
      break;
    case 'V':
      vent();
      break;
    case 'p':
      pw();
      break;
    case 'c':
      power_clim();
      break;
    case 'd':
      power_dp();
      break;
    case 'C':
      clock_pw();
      break;      
    case 'm':
      MSB_start();
      break;      
    case 0x12:
    case 0x13:
    case 0x14:
    case 0x15:
    case 0x16:
    case 0x17:
    case 0x18:
    case 0x19:
    case 0x1A:
    case 0x1B:
    case 0x1C:
    case 0x1D:
    case 0x1E:
      T_S(c);
      break;
    default:
      break;
  }

      
        cmd_rv = 0;
        return;
     }
    }

//  Serial.println("INFO ");
//  Serial.print(cmd_rv);
//  Serial.println(c);
  if (cmd_rv == 0)
    cmd_rv=c;
  else
    cmd_rv=1;
//  Serial.print(cmd_rv);
}

void IR_RECIVE()
{
  if (IrReceiver.decode())
  {

    switch (IrReceiver.decodedIRData.decodedRawData)
    {
      case 65537:
        pw();
        break;

      case 262148:
        T_P();
        break;

      case 131074:
        T_M();
        break;

      case 524296:
        vent();
        break;

      case 8388736:
        power_dp();
        break;

      case 2097184:
        power_clim();
        break;
      default:
        Serial.println("codigo inexistente");
        Serial.println(IrReceiver.decodedIRData.decodedRawData);
    }

    IrReceiver.resume();
  }
}
void climatizar()
{
   if (timer >= (rl_temp + 3))
   {
    if (t_at > t_alv + 1)
    {
      digitalWrite(rl_refri, HIGH);
      digitalWrite(rl_aquec, LOW);
      aquec_st = false;
      refri_st = true;
      rl_temp = timer;
      envia_dados();
    }
    if (t_at < t_alv - 1)
    {
      digitalWrite(rl_refri, LOW);
      digitalWrite(rl_aquec, HIGH);
      aquec_st = true;
      refri_st = false;
      rl_temp = timer;
      envia_dados();
    }
    if (t_at == t_alv)
    {
      digitalWrite(rl_refri, LOW);
      digitalWrite(rl_aquec, LOW);
      aquec_st = false;
      refri_st = false;      
      rl_temp = timer;
      envia_dados();
    }
  }
}

void Modo_SonoB()
{
  if(timer>= MSB_inc + 3600 & !MSB_p1)
  {
    T_P();
    MSB_p1 = true;
    MSB_inc = timer;
  }
  if (timer >= MSB_inc + 7200 & MSB_p1)
  {
    T_P;
    T_P;
    MSB_p1 = false;
    MSB_inc = 0;
    MSB_pw = false;
    envia_dados();
    Serial.println("Modo Sono Bom Terminado");    
  }
}

void loop()
{
  timer = millis() /  1000;
  tempo = rtc.now();
  Buttons();
  bool display_st = dp_on;
  if (mySerial.available() > 0) {
    char c = mySerial.read();
    Trata_dados (c);
  }
  if (Serial.available() > 0) {
    char cc = Serial.read();
    Trata_dados (cc);
  }
  if (pw_st){
  digitalWrite (rl_vent2 , (pw_st & vent_st) ? HIGH : LOW);
  digitalWrite (rl_vent1 , (pw_st & !vent_st) ? HIGH : LOW);
  }
  
  if (timer >= (temp_ant + 3))
  {
    temp = Ttemp.getTemp();
    // check if any reads failed
    if (isnan(temp)) {
      Serial.println("Fail");
    }
    else
    {
      t_at = temp;
      temp_ant = timer;
    }
  
  }
  
  if (clim_st & pw_st)
  {
    climatizar();
  }
  else
  {
    digitalWrite(rl_refri, LOW);
    digitalWrite(rl_aquec, LOW);
    aquec_st = false;
    refri_st = false;
  }

  if (!dp_on or !pw_st )
  {
    dp_clean();
  }
  if (dp_on & display_st & t_at != Ttemp.getTemp() & pw_st)
  {
    dp_atu();
  }
  if (pw_st & MSB_pw & clim_st)
  {
    Modo_SonoB();
  }
  IR_RECIVE();
  
  if (t_ant != t_at ){
  envia_dados();
  t_ant = t_at;
  }
  
}
