#include <NTPClient.h>
//#include <WiFi.h>
#include <WiFiUdp.h>
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");
//Week Days
String weekDays[7]={"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
int gio, phut, giay, thu, ngay, thang, nam;
SimpleTimer timer_updateTime;

void getThoigian(){
  while(!timeClient.update()) {
    timeClient.forceUpdate();
  }
  gio  = timeClient.getHours();
  phut = timeClient.getMinutes();
  giay = timeClient.getSeconds();
  thu  = timeClient.getDay();
  unsigned long epochTime = timeClient.getEpochTime();
  struct tm *ptm = gmtime ((time_t *)&epochTime); 
  ngay  = ptm->tm_mday;
  thang = ptm->tm_mon+1;     //1-->11;
  nam   = ptm->tm_year+1900;
}

void updateTimeLCD(){ 
  //Hien thi thu trong tuan
  tft.setCursor(5, 100);
  tft.setTextColor(YELLOW,BLACK);
  tft.setTextSize(1);
  String weekDay = weekDays[thu];
  tft.println(weekDay);
  
  //Print complete date:
  tft.setCursor(65, 100);
  tft.setTextColor(CYAN,BLACK);
  tft.setTextSize(1);
  String ngay_,thang_;
  if(ngay<10){
    ngay_= "0" + String(ngay);
  }else{
    ngay_=String(ngay);
  }
  if(thang<10){
    thang_= "0" + String(thang);
  }else{
    thang_=String(thang);
  }
  String currentDate = ngay_ + "-" + thang_ + "-" + String(nam);
  tft.println(currentDate);
  
  tft.setCursor(15, 70);
  tft.setTextColor(WHITE,BLACK);
  tft.setTextSize(2);
  String gio_,phut_,giay_;
  if(gio<10){
    gio_= "0" + String(gio);
  }else{
    gio_=String(gio);
  }
  if(phut<10){
    phut_= "0" + String(phut);
  }else{
    phut_=String(phut);
  }
  if(giay<10){
    giay_= "0" + String(giay);
  }else{
    giay_=String(giay);
  }
  String formattedTime = gio_ + ":" + phut_ + ":" + giay_;
  tft.print(formattedTime);
}
void updateTime(){
  giay++;
  if(giay == 60) {
    phut++;
    giay = 0;
  }
  if(phut == 60) {
    gio++;
    phut = 0;
  }
  if(gio == 24) {                              // Sau 1 ngày cập nhật thời gian 1 lần
    gio = 0;
    getThoigian();
  }
  updateTimeLCD();
}

void setUp(){
  timeClient.begin();                             //Khoi chay lay thoi gian tu NTP
  timeClient.setTimeOffset(25200);                //Cai dat mui gio GMT+7
  
  getThoigian();                                  //Lay thoi gian tren NTP  
  updateTimeLCD();
  timer_updateTime.setInterval(1000, updateTime);
}
