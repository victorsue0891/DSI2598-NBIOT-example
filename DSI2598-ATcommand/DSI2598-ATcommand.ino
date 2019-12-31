#include <SoftwareSerial.h>
SoftwareSerial mySerial(8, 9);

String data = "";

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  mySerial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  while (mySerial.available()){  //Check if there is an available byte to read,
    delay(10); //Delay added to make thing stable 
    char c = mySerial.read(); //Conduct a serial read
    if (c == '\n') {break;} //Exit the loop when the "" is detected after the word
    data += c; //Shorthand for data = data + c
  }

  if (data.length() > 0) { // 判斷data內有值在更換
    
    data.trim();
    delay(100);
    Serial.println(data);
  }
  data = "";
  
  while (Serial.available()){  //Check if there is an available byte to read,
    delay(10); //Delay added to make thing stable 
    char c = Serial.read(); //Conduct a serial read
    if (c == '\n') {break;} //Exit the loop when the "" is detected after the word
    data += c; //Shorthand for data = data + c
  }

  if (data.length() > 0) { // 判斷data內有值在更換
    
    data.trim();
    delay(100);
//    Serial.println(data);
    mySerial.println(data);
  }
  data = "";
}
