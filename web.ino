#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <time.h>
//#include <stdlib.h>
//#include <Adafruit_NeoPixel.h>
//#include <SimpleTimer.h>

//#define PIN D2
//#define NUMPIXELS 1
//#define PIN A0

const char* ssid ="";// wifi ssid;
const char* password = "";// wifi password;
String location = "Daejeon"; //날씨정보를 보고싶은 지역

IPAddress hostIp;
uint8_t ret;

ESP8266WebServer server(80); 
WiFiClient client ;   
//Adafruit_NeoPixel pixels = Adafruit_NeoPixel(1, PIN, NEO_GRB + NEO_KHZ800);                         
//SimpleTimer timer;   //timer 쓰겠다고 선언

//const int led = LED_BUILTIN;    //기본 내장 led
//int vr=0;
//String webstring = "";
//int reqCount=0;
//int count=0;
//int c;
//int setTimer(long d, timer_callback f, int n);
//boolean currentLineIsBlank;
//int ln1 = 5;
//int ln2 = 6;
unsigned long lastAttemptTime = 0;            // 마지막으로 서버에서 데이터를 전송받은 시간
bool tagInside = false;  // 태그 안쪽인지 바깥쪽인지 구별하는 변수
bool flagStartTag = false; // 스타트 태그인지 구별하는 변수
String currentTag = ""; // 현재 태그를 저장하기 위한 변수, 공백으로 초기화함
String currentData = ""; // 태그 사이의 컨텐츠를 저장하는 변수
String startTag = ""; // 현재 elements의 start tag 저장
String endTag = "";   // 현재 elements의 end tag 저장
String temp="";
String tempt="";
String Button_O="";
String Button_C="";
String Rain="";
String CDS="";
String IR="";
String tempValue="";
int result_rain=0;
int result_cds=0;
int result_ir=0;

void analog_read(){
  String Analog_R="";
  if(Serial.available()>0){
    while(Serial.available()>0){
      char Analog_Read=Serial.read();
      Analog_R+=Analog_Read;
    }
  }
  //Serial.println(Analog_R);   
  int Button_o=Analog_R.indexOf("O");
  int Button_c=Analog_R.indexOf("C");
  int rain=Analog_R.indexOf("R");
  int cds=Analog_R.indexOf("D");
  int ir=Analog_R.indexOf("I");
  
  Button_O=Analog_R.substring(0,Button_o);
  Button_C=Analog_R.substring(Button_o+1,Button_c); 
  Rain=Analog_R.substring(Button_c+1,rain);
  CDS=Analog_R.substring(rain+1,cds);
  IR=Analog_R.substring(cds+1,ir);
    
  Serial.print("Button_O : ");
  Serial.print(Button_O);
  Serial.print(" ");
  Serial.print("Button_C : ");
  Serial.println(Button_C);
  Serial.print("Rain : ");
  Serial.println(Rain);
  Serial.print("CDS : ");
  Serial.println(CDS);
  Serial.print("IR : ");
  Serial.println(IR);
}

void StringToInt(){
  String num_rain = Rain;
  String num_cds = CDS;
  String num_ir = IR;
  int i, len_rain, len_cds, len_ir;
 
  result_rain=0;
  result_cds=0;
  result_ir=0;
  
  Serial.print("Number_rain: "); 
  Serial.println(num_rain);
  Serial.print("Number_cds: "); 
  Serial.println(num_cds);
  Serial.print("Number_ir: ");
  Serial.println(num_ir); 
 
  len_rain = num_rain.length();
  len_cds = num_cds.length();
  len_ir = num_ir.length();
 
  for(i=0; i<len_rain; i++)
  {
    result_rain = result_rain * 10 + ( num_rain[i] - '0' );
  }
  for(i=0; i<len_cds; i++)
  {
    result_cds = result_cds * 10 + ( num_cds[i] - '0' );
  }
  for(i=0; i<len_ir; i++)
  {
    result_ir = result_ir * 10 + ( num_ir[i] - '0' );
  } 
}

void handleRoot() {
  server.send(200, "text/plain", "hello");
}
void handleNotFound(){

//  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();                                                                        // 파라미터의 개수.
  message += "\n";

  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  //digitalWrite(led, 0);

}

/*void  CALLBACK sum(HWND hWnd, UINT nMsg, UINT_PTR nIDEvent, DWORD dwTime){
  if(nIDEvent==1){
  count++;
  Serial.print(count);
  }
}*/

void setup(void){
 
//  SetTimer(hWnd,1,1000,sum);   //타이머
  //pinMode(led, OUTPUT);    
  pinMode(D5,OUTPUT);
  pinMode(D6,OUTPUT);
  pinMode(D2,OUTPUT);
  pinMode(D3,OUTPUT);
  //digitalWrite(led, 0);
  //pixels.begin();
  Serial.begin(115200);                    
  WiFi.begin(ssid, password);            
  Serial.println("");

  //configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");
  //Serial.println("\nWaiting for time");
  //while (!time(nullptr)) {
    //Serial.print(".");
    //delay(1000);
  //}

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {               
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());                                      

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  ret = WiFi.hostByName("api.openweathermap.org", hostIp);
  Serial.print("ret: ");
  Serial.println(ret);
  Serial.print("Host IP: ");
  Serial.println(hostIp);
  
   
/*  
  server.on("/", handleRoot);                     
  server.on("/inline", [](){                          
  server.send(200, "text/plain", "this works as well");  
  });
  
  server.on("/on", [](){                               
    server.send(200, "text/plain", "LED ON");
      //digitalWrite(led, color());
      pixels.setPixelColor(D2, pixels.Color(0,255,0));
      pixels.show();

  });
  server.on("/off", [](){                              
  server.send(200, "text/plain", "LED OFF");
      pixels.setPixelColor(D2, pixels.Color(0,0,0));
      pixels.show();
  });
 
  server.on("/VR_on",VRRead);*/
  
  analog_read();
  StringToInt();
  connectToServer();
  server.on("/",Weather);
  server.on("/moter_o",moter_O);
  server.on("/moter_c", moter_C);
  server.on("/blind_u",blind_U);
  server.on("/blind_d", blind_D);
  server.onNotFound(handleNotFound);                   
  server.begin();
  Serial.println("HTTP server started");
  
}


void loop(void){
  server.handleClient();   //HTML 페이지를 전달하는 콜백 함수, HTTP 요청의 실제 수신처리
  tempt="";
  analog_read();
  StringToInt();

  
  Serial.print("result_rain: "); 
  Serial.println(result_rain);
  Serial.print("result_cds: "); 
  Serial.println(result_cds);

  tempt+="\nCDS : ";
  tempt+=result_cds;

  if((result_ir>665)||(result_ir<145)){
    digitalWrite(D2,HIGH);
    digitalWrite(D3,HIGH);
  }
  if(result_rain<450){
    tempt+="\nIt's raining";    
    digitalWrite(D5,HIGH);
    digitalWrite(D6,LOW);
    if(Button_C=="0"){
      digitalWrite(D5,HIGH);
      digitalWrite(D6,HIGH);
    }
  }

  if (Button_O=="0"){
    tempt+="\nWindow is Open";
    Serial.println("Window is Open");
  } else if (Button_C=="0"){
    tempt+="\nWindow is close";
    Serial.println("Window is close");
  }

  if((Button_C=="0")||(Button_O=="0")){
      //Serial.println("qkqh");
      digitalWrite(D5,HIGH);
      digitalWrite(D6,HIGH);
  }
  
  delay(300);
}

void connectToServer() {
  Serial.println("connecting to server...");
  String content = "";
  if (client.connect(hostIp, 80)) {
    Serial.println("Connected! Making HTTP request to api.openweathermap.org for "+location+"...");
    client.println("GET /data/2.5/weather?q=Daejeon&appid=fd2feb6166d777ad1b0948c661a036c9&units=metric&mode=xml HTTP/1.1");
    //위에 지정된 주소와 연결한다.
    client.print("HOST: api.openweathermap.org");
    client.println("Connection: close");

    client.println();
    Serial.println("Weather information for "+location);
  }
  //마지막으로 연결에 성공한 시간을 기록
  lastAttemptTime = millis();
  pashing();
}

void pashing(){
  if (client.connected()) {
   while (client.available()) {
    char c = client.read();
    //Serial.print(c);
    if (c == '<') {
      tagInside = true;
    }
    if (tagInside) {
      currentTag += c;
    } else if (flagStartTag) {
      currentData += c;
    } 
    if (c == '>') {
      tagInside = false;
      if (currentTag.startsWith("</")) {
        flagStartTag = false;
        endTag = currentTag;  // store end Tag
        if (startTag.indexOf("country") != -1) {
          if (endTag.indexOf("country") != -1) {
            Serial.print("Country : ");
            Serial.println(currentData);
          }
        }
        currentData = ""; 
      } else {
        flagStartTag = true;                
        startTag = currentTag; // store start Tag
        startTagProcessing();
      } 
      currentTag = "";
    }
  }
  if (!client.connected()) {
    Serial.println();
    Serial.println("disconnecting from server.");
    client.stop();
    // do nothing forevermore:
    while (true);
   }   
  }
client.stop();
Serial.print("diconnect");
}

void startTagProcessing() {
  if (startTag.startsWith("<city")) {
    int attribName = startTag.indexOf("name=");
    if (attribName != -1) {
      String cityName = startTag.substring(attribName + 6);
      int quote = cityName.indexOf("\"");
      cityName = cityName.substring(0, quote);
      Serial.println("City : " + cityName);
    }
  }
  else if (startTag.startsWith("<temperature")) {
    int attribValue = startTag.indexOf("value=");
    if (attribValue != -1) {
      tempValue = startTag.substring(attribValue + 7);
      int quote = tempValue.indexOf("\"");
      tempValue = tempValue.substring(0, quote);
      Serial.println("Temperature : " + tempValue);
      temp+="Temperature : ";
      temp+=tempValue;
    }
  }
  else if (startTag.startsWith("<humidity")) {
    int attribValue = startTag.indexOf("value=");
    if (attribValue != -1) {
      String humidValue = startTag.substring(attribValue + 7);
      int quote = humidValue.indexOf("\"");
      humidValue = humidValue.substring(0, quote);
      Serial.println("Humidity : " + humidValue + "%");
      temp+="\nHumidity : ";
      temp+=humidValue;
      temp+="%";
      //temp+="\nLight : ";
      //temp+=result_cds;
      
      Serial.println("temp");
      Serial.print(temp);      
    }
  }
 /* 
  if (result_cds>result_tem){
    temp+="\n,Light : ";
    temp+=result_cds;
    temp+="\nLight is alot.";
  }
  if(result<450){
        Serial.println(result);
        temp+="\nRaining.";
        temp+=result;
        //Serial.println("qkqh");
  }*/
}

void Weather(){
  server.send(200,"text/plain",temp+tempt);
}

void moter_O()
{
  digitalWrite(D5,LOW);
  digitalWrite(D6,HIGH);
  server.send(200,"text/plain","Window is Open"); 
}

void moter_C()
{
  digitalWrite(D5,HIGH);
  digitalWrite(D6,LOW); 
  server.send(200,"text/plain","Window is Close");
}

void blind_U()
{
  digitalWrite(D2,HIGH);
  digitalWrite(D3,LOW);
  server.send(200,"text/plain","Blind is Up"); 
}

void blind_D()
{
  digitalWrite(D2,LOW);
  digitalWrite(D3,HIGH); 
  server.send(200,"text/plain","Blind is Down");
}




/*
void VRRead(){
  webstring = "VR = "+(String)analogRead(A0);
  Serial.println(webstring);

   
  
  //server.send(200, "text/plain", webstring);
  char temp[400];
  char temp1[400];

  if(analogRead(A0)<600){
   Serial.println("Window is Close");
   snprintf ( temp, 400, 
  
 
  "<html>\
    <head>\ 
      <meta http-equiv='refresh' content='2'/>\ 
      <title>ESP8266 Demo</title>\ 
      <style>\ 
        body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\ 
      </style>\ 
    </head>\ 
    <body>\ 
      <h1>VR from ESP8266!</h1>\ 
      <p>VR = %4d</p>\
      <p>Window is Close</p>\
      <img src=\"/test.svg\" />\ 
    </body>\
  </html>",
  
 
       analogRead(A0)
    ); 
    server.send ( 200, "text/html", temp );
 }
 if(analogRead(A0)>=600){
      Serial.println("window is open");
       snprintf ( temp1, 400, 
  
 
  "<html>\ 
    <head>\ 
      <meta http-equiv='refresh' content='2'/>\ 
      <title>ESP8266 Demo</title>\ 
      <style>\ 
        body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\ 
      </style>\ 
    </head>\ 
    <body>\ 
      <h1>VR from ESP8266!</h1>\ 
      <p>VR = %4d</p>\
      <p>Window is Open</p>\
      <img src=\"/test.svg\" />\ 
    </body>\ 
  </html>", 
  
 
       analogRead(A0)
    ); 
      server.send(200,"text/html",temp1); 
    
  }
}
*/

