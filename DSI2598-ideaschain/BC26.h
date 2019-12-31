#include <SoftwareSerial.h>
SoftwareSerial mySerial(8, 9);

String AK = "xxxxxxxxxxxxxxxxxxxx";    //  平台的AK金鑰參數
String DID = "0150455698870876";   //  平台的裝置ID參數
String SID = "S000001";          //  平台的感測器ID參數

String data = "";                  //  儲存來自BC26回覆的字串參數
String data_on = "";               //  擷取部分字串參數的儲存參數
int sta = 1;                       //  迴圈的status參數判斷
int time_count = 0;
int count = 0;                     //  副程式中來自BC26的回覆次數計數器
//int count1 = 0;                  //  副程式中的時間計數器
int band = 8;                      //  設定頻段(B8)
int IP_count1 = 0;
void(* resetFunc) (void) = 0;      //  宣告系統重置參數
int reset_count = 0;               //  系統重新啟動計時器宣告
String data_tcp_build = "AT+QSOC=1,1,1";   //  建立TCP通訊的AT指令
//String tcpserver_connect = "AT+QSOCON=0,7788,\"114.34.107.226\"";
String tcpserver_connect = "AT+QSOCON=0,7789,\"150.117.122.166\"";
String noreply = "no reply, reset all";

void serial_read(){
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
    if(sta == 1){
      count++;
    }
    if(sta == 1){
      if(count >= 1){         //  Turn on BC26 status
//        Serial.println("Turn on BC26");
        sta++;
        count = 0;
      }
    }else if(sta == 2){
      if(data == "OK"){         //  設置頻段
        sta++;
      }
    }else if(sta == 3){       //  wait for IP status
      for(int i = 0; i < 3; i++){
        data_on += data[i];
      }
      if(data_on == "+IP"){
        sta++;
      }else if(data_on =="+CG"){
        sta++;
      }
    }else if(sta == 4){
      for(int i = 0; i < 7; i++){
        data_on += data[i];
      }
      if(data_on == "+QSOC=0"){         //  開啟TCP通訊通道
        sta++;
      }
    }else if(sta == 5){
      if(data == "OK"){         //  連線至伺服器
        sta++;
      }
    }else if(sta > 5){        //  傳輸資料與伺服器端訊息回傳
      //  +QSONMI
      for(int i = 0; i < 8; i++){
        data_on += data[i];
      }
      if(data_on == "+QSONMI=" || data_on == "AT+QSOSE"){
        sta++;
      }else if(data[0] == 'O' && data[1] == 'K'){
        sta++;
      }
    }
  }
   data = "";
   data_on = "";
}

void BC26_reset(){                                //  reset BC26
  mySerial.println("AT+QRST=1");
  while (sta == 1) {          //  等待模組訊息回覆
    serial_read();
    delay(1*1000);
    count++;
    if (count > 10) {        //  超過10秒未有回覆，重新啟動系統
      count = 0;
      Serial.println(noreply);
      resetFunc();
    }
  }
  count = 0;
}

void ask_for_IP(){                                //  查詢IP狀況
  mySerial.println("AT+CGPADDR=1");
  while (sta == 3) {          //  等待模組訊息回覆
    serial_read();
    delay(1*1000);
    count++;
    if (count > 10) {       //  每10秒問一次IP狀況
      IP_count1++;
      mySerial.println("AT+CGPADDR=1");
      count = 0;
      if(IP_count1 > 6){      //  一分鐘後仍沒找到IP，重新開機
        Serial.println(noreply);
        resetFunc();
      }
    }
  }
  count = 0;
  IP_count1 = 0;
}

void set_band(int band){                          //  設置頻段
  String AT_band = "AT+QBAND=1,";
  AT_band.concat(band);
  mySerial.println(AT_band);
  while (sta == 2) {          //  等待模組訊息回覆
    serial_read();
    delay(1*1000);
    count++;
    if (count > 10) {        //  超過10秒未有回覆，重新啟動系統
      count = 0;
      Serial.println(noreply);
      resetFunc();
    }
  }
  count = 0;
}

void build_TCP_connect(){                         //  建立TCP連線通道
  mySerial.println(data_tcp_build);
}

void connect_toserver(){                          //  與伺服器連接
  mySerial.println(tcpserver_connect);
}

String str2hex(String str){     //  API字串轉hex格式function
  String hex_string = "";
  for(int i = 0; i < str.length(); i++){
    hex_string += String(str[i], HEX);
  }
  return hex_string;
}

String api_string(String AK, String DID, String SID, String data){
  String api_pass = "{\"a\":\"";
  api_pass.concat(AK);
  api_pass.concat("\",\"d\":\"");
  api_pass.concat(DID);
  api_pass.concat("\",\"s\":\"");
  api_pass.concat(SID);
  api_pass.concat("\",\"v\":\"");
  api_pass.concat(data);
  api_pass.concat("\"}");
  Serial.print("api_pass: ");
  Serial.println(api_pass);
  return api_pass;
}

void reading(int sta_pre, int sta){
  
  while(sta_pre == sta){          //  等待模組訊息回覆
    serial_read();
    delay(1*1000);
    break;
    count++;
  }
  if(count > 10){   //  超過10秒未有回覆，
    count = 0;
    Serial.println("no replay, send next data after 2 seconds");
    delay(1*1000);
    sta_pre++;
  }
  count = 0;
  sta_pre = sta;
}

void data_update(String AK, String DID, String SID, String data){     //  資料上傳function
  Serial.println("Sending data......");
  int sta_pre = sta;
  build_TCP_connect();
  reading(sta_pre, sta);
  
  connect_toserver();
  reading(sta_pre, sta);
  
  mySerial.print("AT+QSOSEND=0,"+String(api_string(AK, DID, SID, data).length()+2)+",");
  mySerial.println(str2hex(api_string(AK, DID, SID, data))+"0d0a");
  reading(sta_pre, sta);
  
  mySerial.println("AT+QSODIS=0");
  reading(sta_pre, sta);
  
  mySerial.println("AT+QSOCL=0");
  reading(sta_pre, sta);  
}

void BC26_initail(){
  BC26_reset();
  delay(1*1000);
  set_band(band);
  delay(20*1000);       //  等待20秒連線
  ask_for_IP();         //  查詢IP狀況
  delay(5*1000);
}

void long_delay(int hr, int minut, int sec){        //  設定delay時間長度
  for(int i = 0; i < (hr*60*60 + minut*60 + sec); i++){
    delay(1000);
  }
}

