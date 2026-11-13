Program Code:

#define BLYNK_TEMPLATE_ID "TMPL3NKgkqtsr" #define BLYNK_TEMPLATE_NAME "Fluid monitoring" #include <WiFi.h>
#include
<BlynkSimpleEsp32.h> #include <Wire.h>
#include <Adafruit_GFX.h> #include
<Adafruit_SSD1306.h> #include <HardwareSerial.h> char auth[] =
"3EpAgAPyt7E3J1bE7Wgh0UhQnny0yZjr"; char ssid[]
= "project"; char pass[] =
"12345678"; #define
BUTTON 4
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1); HardwareSerial sim800(2);
#define GSM_TX 17 #define GSM_RX 16 bool smsSent=false; #define S0 14
#define S1 27
#define S2 26
#define S3 25 #define sensorOut 34 int R,G,B;
int lyR,lyG,lyB; int dyR,dyG,dyB; int lbR,lbG,lbB; int dbR,dbG,dbB; String level=""; int ppm=0;
void showOLED(String l1,String l2="",String l3="")
{
display.clearDisplay();
display.setCursor(0,0); display.println(l1); display.setCursor(0,18); display.println(l2); display.setCursor(0,36); display.println(l3); display.display();
}
void initGSM()
{ sim800.begin(9600,SERIAL_8N1,GSM_RX,GSM_T X);
delay(3000);
sim800.println("AT"); delay(1000); sim800.println("AT+CMGF= 1"); delay(1000);
sim800.println("AT+CNMI=1,2,0,0,0");
delay(1000); Serial.println("SIM800 Ready");
}
void sendSMS(String num,String msg)
{
sim800.println("AT+CMGF=1 "); delay(500); sim800.print("AT+CMGS=\""); sim800.print(num); sim800.println("\""); delay(500);
sim800.print(msg); delay(500); sim800.write(26); delay(5000);
}
void checkSMS()
{
if(sim800.available())
{
String msg=sim800.readString(); msg.toUpperCase(); if(msg.indexOf("STATUS")>=0
)
{
String reply=
"Smart Insecticide Residue Monitoring in Precision Agriculture" "\n\nLEVEL:"+level+
"\nPPM :"+String(ppm);
sendSMS("9865104970",reply);
}
}
}
int readRed()
{
digitalWrite(S2,LOW); digitalWrite(S3,LOW);
return pulseIn(sensorOut,LOW);
}
int readGreen()
{
digitalWrite(S2,HIGH); digitalWrite(S3,HIGH);
return pulseIn(sensorOut,LOW);
}
int readBlue()
{
digitalWrite(S2,LOW); digitalWrite(S3,HIGH);
return pulseIn(sensorOut,LOW);
}
void readRGB()
{
int r=0,g=0,b=0; for(int i=0;i<10;i++)
{
r+=readRed(); delay(20); g+=readGreen(); delay(20); b+=readBlue(); delay(20);
}
R=r/ 10;
G=g/ 10;
B=b/ 10;
}
int diff(int r,int g,int b)
{
return abs(R-r)+abs(G-g)+abs(B-b);
}
void detectPPM()
{
Int dLY=diff(lyR,lyG,lyB);
int dDY=diff(dyR,dyG,dyB); int dLB=diff(lbR,lbG,lbB); int dDB=diff(dbR,dbG,dbB); int
minVal=min(min(dLY,dDY),min(dLB,dDB)); if(minVal==dLY)
{
level="LOW"; ppm=0;
}
else if(minVal==dDY)
{
level="LOW"; ppm=0;
}
else if(minVal==dLB)
{
level="MEDIUM"; ppm=1;
}
else
{
level="HIGH"; ppm=10;
}
Serial.println("------ RESULT	");
Serial.print("RGB:"); Serial.print(R); Serial.print(","); Serial.print(G); Serial.print(","); Serial.println(B); Serial.print("LEVEL:"); Serial.println(level); Serial.print("PPM:"); Serial.println(ppm); showOLED( "LEVEL:"+level, "PPM:"+String(ppm),
"R="+String(R)+" G="+String(G)
);
Blynk.virtualWrite(V0,leve l); Blynk.virtualWrite(V1,ppm
);
Blynk.virtualWrite(V2,ppm
);
}
void calibrateColors()
{
showOLED("Calibrate","Light Brown"); delay(5000); readRGB();
lyR=R; lyG=G; lyB=B; showOLED("Calibrate","Dark Brown"); delay(5000);
readRGB();
dyR=R; dyG=G; dyB=B; showOLED("Calibrate","Black"); delay(5000);
readRGB();
dbR=R; dbG=G; dbB=B; showOLED("Calibration","Don e"); delay(2000);
}
void setup()
{
Serial.begin(115200); pinMode(BUTTON,INPUT_PULLUP);
pinMode(S0,OUTPU T);pinMode(S1,OUT PUT);
pinMode(S2,OUTPU T);
pinMode(S3,OUTPU T);
pinMode(sensorOut,INPUT); digitalWrite(S0,HIGH); digitalWrite(S1,LOW); Wire.begin(21,22);
display.begin(SSD1306_SWITCHCAPVCC,0x3C); display.setTextSize(1); display.setTextColor(SSD1306_WHITE); showOLED("System Ready","Press button"); initGSM();
Blynk.begin(auth,ssid,pass);
}
void loop()
{
Blynk.run(); if(digitalRead(BUTTON)==LO W)
{
delay(300); calibrateColors();
}
readRGB(); detectPPM(); checkSMS();
if(level=="HIGH" && smsSent==false)
{
String msg=
"Smart Insecticide Residue Monitoring in Precision Agriculture \nALERT" "\nLEVEL: HIGH"
"\nPPM : "+String(ppm); sendSMS("9865104970",ms
g); smsSent=true;
showOLED("SMS Sent","HIGH ALERT");
} if(level!="HIGH")
{
smsSent=false;
}
delay(1000);
}
