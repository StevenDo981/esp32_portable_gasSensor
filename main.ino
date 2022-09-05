#define BLYNK_TEMPLATE_ID "TMPLXxLBxZee"
#define BLYNK_DEVICE_NAME "Steven Do"
#define BLYNK_AUTH_TOKEN "XasvWBAp_PyqEgd8cH4j9utsdTX7LLzs"
#define BLYNK_FIRMWARE_VERSION        "0.1.0"
#define BLYNK_PRINT Serial
//#define BLYNK_DEBUG
#define APP_DEBUG
#include "BlynkEdgent.h"
#include "UI.h"
#include <EEPROM.h>
UI ui;
const int bt1 = 27;// This GPIO will use for DHT11 sensor to get Humi and temp
const int bt2 = 26;// also
//const int bt3 = 25;
const int PWM = 25;// if use this GPIO while using WIFI then it cann't use to DAC
const int out1 = 13;
const int out2 = 12;
const int out3 = 14;
const int sensor1 = 32;
const int sensor2 = 35;
const int sensor3 = 34;
boolean bt1State = LOW;
boolean bt2State = LOW;
boolean bt3State = LOW;
boolean bt4State = LOW;
int count = 1;
int v_Out, V_g;
int Vb1, Vb2, Vb3;
boolean pumbState = true;
#define FLASH_MEMORY_SIZE 50
int analog_Volt[FLASH_MEMORY_SIZE];
double concen = 0;
int polate_l;
int polate_h;
//Global variables of draw chart
int valueBlock[5000];
int timeBlock[5000];
int locationBlock[5000];
int valuePos;
int blockPos;
//Editable Variables
bool proDebug = 0;
uint16_t graphColor = BLUE;
uint16_t pointColor = WHITE;
uint16_t lineColor = GREEN;

int graphRange = 3300;
int markSize = 3;

//Calculate Values
const int numberOfMarks = 8;
const int originX = 22;
const int originY = 220;
const int sizeX = 220;
const int sizeY = 150;
const int deviation = 20;
//Number of mark on Graph
int boxSize = (sizeX / numberOfMarks);
int mark[] = {(boxSize + deviation), ((boxSize * 2) + deviation), ((boxSize * 3) + deviation), ((boxSize * 4) + deviation), ((boxSize * 5) + deviation), ((boxSize * 6) + deviation), ((boxSize * 7) + deviation), ((boxSize * 8) + deviation)};

const int minorSizeY = (originY + 5);
const int minorSizeX = (originX - 5);

int numberSize = (sizeY / 6);
int number[] = {numberSize, (numberSize * 2), (numberSize * 3), (numberSize * 4), (numberSize * 5), (numberSize * 6)};

int numberValue = (graphRange / 6);
int val[] = {graphRange, (numberValue * 5), (numberValue * 4), (numberValue * 3), (numberValue * 2), numberValue};
int concen1 = 0;
double dentaU;
WidgetLED ledConnect(V3);
unsigned long timesBlinkLed = millis();
unsigned long timesRunBase = millis();// set times for Run base
SimpleTimer timer_updateData;
//==============================================
void setup(){
  Serial.begin(115200);
  delay(100);
  pinMode(bt1,INPUT_PULLUP);
  pinMode(bt2,INPUT_PULLUP);
//  pinMode(bt3,INPUT_PULLUP);
  pinMode(out1,OUTPUT);
  pinMode(out2,OUTPUT);
  pinMode(out3,OUTPUT);
  //pinMode(PWM, OUTPUT);
  digitalWrite(out1, HIGH);
  digitalWrite(out2, HIGH);
  digitalWrite(out3, HIGH);
  BlynkEdgent.begin();
  EEPROM.begin(FLASH_MEMORY_SIZE);
  ui.setupLCD();
  timer_updateData.setInterval(400, updateVolttoLCD);// Display the data value on TFT every 400 ms
  drawGraph();
  //print the Volt of sensor
  tft.setCursor(260, 130);// change the backgr color of Volt 
  tft.setTextColor(WHITE, BLACK);
  tft.setTextSize(2);
  String s;
  s = "1";
  tft.println(s);
  tft.setCursor(280, 130);
  tft.println(" V");
  //Print the volt of Pump
  tft.setCursor(270, 80);//Change the BackGr color of Pump
  tft.setTextColor(WHITE, BLACK);
  tft.setTextSize(2);
  String s1 = "1";
  tft.print(s1);
}
//====================================
void loop(){
  BlynkEdgent.run();
  //timer_updateTime.run();
  timer_updateData.run();
  if(millis()-timesBlinkLed>1000){
    if(ledConnect.getValue()){
      ledConnect.off();
    }else{
      ledConnect.on();
    }
    //unsigned long value = millis() / 1000;
    //Blynk.virtualWrite(V9, String(value));
    timesBlinkLed=millis();
  }
  getVoltChange1();
  graph();
  writeDatatoBlynk();
}
// có thể suy nghĩ để chạy nền tự động khoảng vài giây

BLYNK_CONNECTED() {
  // Request Blynk server to re-send latest values for all pins
  Blynk.syncAll();
  //Blynk.syncVirtual(V0, V2);
}
// turn on the Air vale for sensor to restore
BLYNK_WRITE(V4){
  int p = param.asInt();
  if(p==1){
    //need to put the code control pumb speed or put max volt
    digitalWrite(out1,LOW);
    digitalWrite(out2, LOW);
    tft.setTextSize(1);
    tft.fillRect(245, 172, 75, 30, RED);//Frame of Prepare;//Fill Frame of Semple when the button is activated
    tft.drawRect(245, 172, 75, 30, YELLOW);//Frame of Prepare
    tft.setTextColor(WHITE);
    tft.setTextColor(ILI9341_GREEN);
    tft.setCursor(260, 178);// Disp prepare
    tft.println("Prepare");
  }else{
    digitalWrite(out1,HIGH);
    digitalWrite(out2, HIGH);
    tft.setTextSize(1);
    tft.fillRect(245, 172, 75, 30, BLACK);//Frame of Prepare;//Fill Frame of Semple when the button is deactivated
    tft.drawRect(245, 172, 75, 30, YELLOW);//Frame of Prepare
    tft.setTextColor(WHITE);
    tft.setTextColor(ILI9341_GREEN);
    tft.setCursor(260, 178);// Disp prepare
    tft.println("Prepare");
  }
}
// sample
BLYNK_WRITE(V5){
  int p = param.asInt();
  if(p==1){
    //need to put the code that can control pumb speed and use delay
    V_g = log(map(analogRead(sensor1),0, 4096, 0, 5000));//saving under form of logarit
    digitalWrite(out1,LOW);
    digitalWrite(out3, LOW);
    tft.setTextSize(1);
    tft.fillRect(245, 202, 75, 30, GREEN);//Fill Frame of Sample when the button is activated
    tft.drawRect(245, 202, 75, 30, YELLOW);// Frame of Semple
    tft.setTextColor(WHITE);
    //tft.setTextColor(ILI9341_GREEN);
    tft.setCursor(260, 210);// Disp Dample
    tft.println("Sample");
  }else{
    digitalWrite(out1,HIGH);
    digitalWrite(out3, HIGH);
    tft.setTextSize(1);
    tft.fillRect(245, 202, 75, 30, BLACK);//Fill Frame of Sample when the button is deactivated
    tft.drawRect(245, 202, 75, 30, YELLOW);// Frame of Semple
    //tft.setTextColor(WHITE);
    tft.setTextColor(ILI9341_GREEN);
    tft.setCursor(260, 210);// Disp Dample
    tft.println("Sample");
  }
}
// volt change

BLYNK_WRITE(V15){
  int p = param.asInt();
  if(p == 1){
    volt_OutPlus();
    tft.setCursor(260, 1300);// change the backgr color off Volt 
    tft.setTextColor(WHITE, BLACK);
    tft.setTextSize(2);
    switch(count){
      case 1: 
       if(count = 1){
        String s;
        s = String(1.5);
        tft.println(s);
       }
       break;
       case 2: 
       if(count = 2){
        String s;
        s = String(2);
        tft.println(s);
       }
       break;
       case 3: 
       if(count = 3){
        String s;
        s = String(3.3);
        tft.println(s);
        }
        break;
        case 4: 
        if(count = 4){
         String s;
         s = String(5);
         tft.println(s);
        }
        break;
        default:
         String s;
         s = String(1);
         tft.println(s);
    }

  }
}
//Pump Control
BLYNK_WRITE(V12){
  volt_PumpLV();
  int p = param.asInt();
  if(p == 1){
    volt_OutPlus();
    tft.setCursor(270, 80);// change the backgr color off Volt 
    tft.setTextColor(WHITE, BLACK);
    tft.setTextSize(2);
    switch(count){
      case 1: 
       if(count = 1){
        String s;
        s = String(1);
        tft.println(s);
       }
       break;
       case 2: 
       if(count = 2){
        String s;
        s = String(2);
        tft.println(s);
       }
       break;
       case 3: 
       if(count = 3){
        String s;
        s = String(3);
        tft.println(s);
        }
        break;
        case 4: 
        if(count = 4){
         String s;
         s = String(5);
         tft.println(s);
        }
        break;
        default:
         String s;
         s = String(1);
         tft.println(s);
    }

  }
}
void writeDatatoBlynk(){
  Blynk.virtualWrite(V10, polate_h);
  Blynk.virtualWrite(V9, polate_l);
  Blynk.virtualWrite(V11, concen);
}
BLYNK_WRITE(V0){
  int p = param.asInt();
  if(p==1){
    canculate_Concen();
  }
}

/*
BLYNK_WRITE(V8){
  int p = param.asInt();
  if(p == 1){
    tft.fillScreen(WHITE);
    graphColor = BLACK;
    pointColor = WHITE;
    lineColor = YELLOW;
    drawGraph();
    graph();
  }
  if(p == 0){
    tft.fillScreen(BLACK);
    graphColor = BLACK;
    pointColor = BLACK;
    lineColor = BLACK;
    ui.setupLCD();  
  }
}
*/
//using timer.setInterval to set the values in TFT LCD

//get the change values from ESP32
double getVoltChange1(){
     
}

double canculate_Concen(){
  int LEN_ROW_TABLE_INTERPOLATE = sizeof(analog_Volt)/sizeof(analog_Volt[0]);
  for(int i = 0;i < LEN_ROW_TABLE_INTERPOLATE - 1;i++){
      Serial.println(" ");
      Serial.printf("Ô nhớ thứ i: ");
      Serial.println(EEPROM.read(i));
    }
    uint8_t flag, flag1, flag2;
    for(int i = 0; i < LEN_ROW_TABLE_INTERPOLATE - 1;i++){
      for(int j = i + 1;j < LEN_ROW_TABLE_INTERPOLATE; j++){
        flag1 = EEPROM.read(i);
        flag2 = EEPROM.read(j);
        if(flag1 > flag2){
          flag = flag1;
          flag1 = flag2;
          flag2 = flag;
          EEPROM.commit();
        }
      }
    }
    //Starting calculate concen...
    const int polate_ref = EEPROM.read(0);// Save original value...this value cannt be change all the time
    polate_l = EEPROM.read(0);
    polate_h = EEPROM.read(LEN_ROW_TABLE_INTERPOLATE - 1);
    dentaU = log(polate_h - polate_l);
    concen = 0.003 + dentaU * 2.065;
    if(concen < 1){
      concen = 0;
    }
    return concen, polate_h, polate_l;
}
//change the voltage level of Pump 
int volt_PumpLV(){
  count++;
  switch(count){
    case 1: 
      dacWrite(PWM, 0);//1V
      break;
    case 2: 
      dacWrite(PWM, 102);//2V
      break;
    case 3:
      dacWrite(PWM, 168);//3.3V
      break;
    case 4:
      dacWrite(PWM, 200);//4V
      break;
    case 5:
      dacWrite(PWM, 255);//5V
  }
}
// change voltage level from GPIO
int volt_OutPlus(){
  count++;
  switch(count){
    case 2: 
      dacWrite(PWM, 102);// 2V
      break;
    case 3:
      dacWrite(PWM, 168);//3.3V
      break;
    case 4: 
      dacWrite(PWM, 200);//5V
      break;
    case 5:
    dacWrite(PWM, 255);
    default:
      dacWrite(PWM, 51);//1V
      break;
  }
  if(count > 5){
    count = 1;
  }
  return count;
}
void updateVolttoLCD(){
  tft.setTextSize(1);
  tft.setCursor(260, 30);tft.setTextColor(RED, BLACK);
  String concenLast = String(concen);
  tft.println(concenLast);
}
//

// Draw chart
void drawGraph()
{ 
  // draw outline
  tft.drawLine(originX, originY, (originX + sizeX), originY, graphColor);
  tft.drawLine(originX, originY, originX, (originY - sizeY), graphColor);

  // draw lables
  for(int i = 0; i < numberOfMarks; i++)
  {
    tft.drawLine(mark[i], originY, mark[i], minorSizeY, graphColor);
  }

  // draw numbers
  for(int i = 0; i < 6; i++)
  {
    tft.drawLine(originX, (originY - number[i]), minorSizeX, (originY - number[i]), graphColor);
  }

  // draw number values
  for(int i = 0; i < 6; i++)
  {
    tft.setCursor((minorSizeX - 18), (number[i] + numberSize + 20));
    tft.setTextColor(graphColor);
    tft.setTextSize(1);
    tft.println(val[i]);
  }
}

void graph()
{
  timeBlock[valuePos] = ((millis() - 4500) / 1000);

  valueBlock[valuePos] = V_g;
  
  if(proDebug)
  {
    //Serial.println(timeBlock[valuePos]);
  }
  
  if(blockPos < 8)
  {
    // print the time
    tft.setCursor((mark[valuePos] - 5), (originY + 5));
    tft.setTextColor(graphColor, WHITE);
    tft.setTextSize(1);
    tft.println(timeBlock[valuePos]);
    
    // map the value
    locationBlock[valuePos] = map(V_g, 0, graphRange, originY, (originY - sizeY));

    // draw point
    tft.fillRect((mark[valuePos] - 1), (locationBlock[valuePos] - 1), markSize, markSize, pointColor);

    // try connecting to previous point
    if(valuePos != 0)
    {
      tft.drawLine(mark[valuePos], locationBlock[valuePos], mark[(valuePos - 1)], locationBlock[(valuePos - 1)], lineColor);
    }

    blockPos++;
  }
  else
  {
    // clear the graph's canvas
    tft.fillRect((originX + 2), (originY - sizeY), sizeX, sizeY, BLACK);

    // map the value - current point
    locationBlock[valuePos] = map(V_g, 0, graphRange, originY, (originY - sizeY));

    // draw point - current point
    tft.fillRect((mark[7]), (locationBlock[valuePos] - 1), markSize, markSize, pointColor);

    // draw all points
    for(int i = 0; i < 8; i++)
    {
      tft.fillRect((mark[(blockPos - (i + 1))] - 1), (locationBlock[(valuePos - i)] - 1), markSize, markSize, pointColor);
    }

    // draw all the lines
    for(int i = 0; i < 7; i++)
    {
      tft.drawLine(mark[blockPos - (i + 1)], locationBlock[valuePos - i], mark[blockPos - (i + 2)], locationBlock[valuePos - (i + 1)], lineColor);
    }
    
    // change time lables
    for(int i = 0; i <= 7; i++)
    {
      tft.setCursor((mark[(7 - i)] - 5), (originY + 5));
      tft.setTextColor(graphColor, WHITE);
      tft.setTextSize(1);
      tft.println(timeBlock[valuePos - i]);
    }
  }

  valuePos++;
}
