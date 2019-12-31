#include "BC26.h"

#define RED 11
#define GREEN 12
#define BLUE 13

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  mySerial.begin(9600);
  pinInitial();
  LED("red");
  delay(500);
  Serial.println("Boot BC26 ");
  BC26_initail();               //  啟動與設定連線的伺服器
  LED("green");
  delay(2000);
  LED("blue");
  data_update(AK, DID, SID, String(reset_count));
  delay(2000);
  LED("green");
}

void loop() {
  // put your main code here, to run repeatedly:  
  if(reset_count >= 3600){     //  30min後系統重開
    resetFunc();
  }
  if(time_count >= 60){
    LED("blue");
    data_update(AK, DID, SID, String(reset_count));
    time_count = 0;
  }
  LED("green");
  delay(500);
  time_count++;
  reset_count++;              //  系統重新啟動計時器累加
}

void pinInitial(){
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);
  digitalWrite(RED, LOW);
  digitalWrite(GREEN, LOW);
  digitalWrite(BLUE, LOW);
}

void LED(String string){
  if(string == "red"){
    digitalWrite(RED, HIGH);
    digitalWrite(GREEN, LOW);
    digitalWrite(BLUE, LOW);  
//    delay(1000);
  }else if(string == "green"){
    digitalWrite(RED, LOW);
    digitalWrite(GREEN, HIGH);
    digitalWrite(BLUE, LOW);  
//    delay(1000);
  }else if(string == "blue"){
    digitalWrite(RED, LOW);
    digitalWrite(GREEN, LOW);
    digitalWrite(BLUE, HIGH);
//    delay(1000);
  }else if(string == "none"){
    digitalWrite(RED, LOW);
    digitalWrite(GREEN, LOW);
    digitalWrite(BLUE, LOW); 
//    delay(1000);
  }else if(string == "yellow"){
    digitalWrite(RED, HIGH);
    digitalWrite(GREEN, HIGH);
    digitalWrite(BLUE, LOW);  
//    delay(1000);
  }else if(string == "purple"){
    digitalWrite(RED, HIGH);
    digitalWrite(GREEN, LOW);
    digitalWrite(BLUE, HIGH);  
//    delay(1000);
  }else if(string == "sky"){
    digitalWrite(RED, LOW);
    digitalWrite(GREEN, HIGH);
    digitalWrite(BLUE, HIGH); 
//    delay(1000);
  }else if(string == "white"){
    digitalWrite(RED, HIGH);
    digitalWrite(GREEN, HIGH);
    digitalWrite(BLUE, HIGH); 
//    delay(1000);
  }else if(string == "nocolor"){
    digitalWrite(RED, LOW);
    digitalWrite(GREEN, LOW);
    digitalWrite(BLUE, LOW); 
//    delay(1000);
  }
}
